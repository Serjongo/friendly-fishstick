//
// Created by Serjo on 12/23/2024.
//
#include "PPU.h"


///SFML FUNCTIONS
//taken from stackoverflow for pixel drawing
sf::RectangleShape addPixel(sf::Vector2f position, sf::Uint8 red, sf::Uint8 green, sf::Uint8 blue)
{
    sf::RectangleShape pixel;
    pixel.setSize({ 1.f, 1.f });
    pixel.setFillColor({ red, green, blue });
    pixel.setPosition(position);
    return pixel;
}
///END



//Color

BYTE Color::get_red()
{
    return red;
}
BYTE Color::get_green()
{
    return green;
}
BYTE Color::get_blue()
{
    return blue;
}

void Color::set_red(BYTE red)
{
    this->red = red;
}
void Color::set_green(BYTE green)
{
    this->green = green;
}
void Color::set_blue(BYTE blue)
{
    this->blue = blue;
}





//Pixel

Pixel::Pixel(BYTE color, BYTE palette, BYTE background_priority,BYTE type)
{
    data = 0;
    set_color(color);
    set_palette(palette);
    set_background_priority(background_priority);
    set_type(type); // 0 - background/window, 1 - sprite
}

//getters
BYTE Pixel::get_color()
{
    return (data & 0x03);
}
BYTE Pixel::get_palette()
{
    return ((data & 0x04)>> 2);
};
BYTE Pixel::get_background_priority()
{
    return (data & 0x08);
};
//setters
void Pixel::set_color(BYTE color)
{
    data = (data & 0xfc);
    data = (data | color);
}
void Pixel::set_palette(BYTE palette)
{
    data = (data & 0xfb);
    data = (data | ((palette & 0x01) << 2));
};
void Pixel::set_background_priority(BYTE background_priority)
{
    data = (data & 0xf7);
    data = (data | ((background_priority & 0x01) << 3));
};
void Pixel::set_type(BYTE type) // 0 - background/window, 1 - sprite
{
    data = (data & 0xef);
    data = (data | ((type & 0x01) << 4));
};



//PPU
PPU::PPU(BYTE* OAM_start,BYTE* VRAM_start, BYTE* MEM_start,gameboy& gameboy) : parent(gameboy)
{
    VRAM = VRAM_start;
    //OAM = OAM_start;
    MEM = MEM_start;
//    background_palette[0] = Color(175,203,70);
//    background_palette[1] = Color(121,170,109);
//    background_palette[2] = Color(34,111,95);
//    background_palette[3] = Color(8,41,85);
    background_palette[0] = Color(255,0,70);
    background_palette[1] = Color(0,255,0);
    background_palette[2] = Color(0,0,255);
    background_palette[3] = Color(124,124,124);
    pixels.reserve(100000);
    //OAM = std::make_unique<std::vector<Sprite*>>();
}

void PPU::clean_visible_OAM_buff()
{
    for(Sprite* spr : visible_OAM_buffer)
        visible_OAM_buffer.pop_back();
}

void PPU::clean_OAM_buff()
{
    for(Sprite* spr : OAM)
        OAM.pop_back();
}

void PPU::OAM_SCAN() //mode 2 of the ppu
{
    clean_visible_OAM_buff(); //should set oam size to 0
    clean_OAM_buff();
    // start of every scanline, find sprites to be rendered, add them to buffer given the following conditions per sprites:
    // xpos > 0, LY+16 > ypos, LY+16 < ypos+height (16 or 8), amount of spirtes < 10
    BYTE cur_row = MEM[LY_register]; //LY register
    BYTE tile_size = get_LCDC_sprite_size_status() ? 16 : 8;
    for(int i = OAM_mem_start ; i < OAM_mem_end; i = i+4) // goes over 160 bytes, 40 sprites
    {
        Sprite spr = Sprite(MEM[i],MEM[i+1],MEM[i+2],MEM[i+3]);
        OAM.push_back(&spr);
//        BYTE y_pos = MEM[i];
//        BYTE x_pos = MEM[i+1];
//        BYTE tile_num = MEM[i+2];
//        BYTE attributes = MEM[i+3];

        if(spr.get_x_pos() > 0 && cur_row + 16 >= spr.get_y_pos() && cur_row + 16 < spr.get_y_pos() + tile_size && visible_OAM_buffer.size() < 10)
        {
            visible_OAM_buffer.push_back(&spr); //we point to the first byte of the OAM, currently it is not a distinct struct
        }
    };
}

void PPU::DRAW() //mode 3 of the ppu
{
    //calculate lcd_x_coord...
    //add it to tile_dat_mem when calling pixel_fetcher
    pixel_fetcher();
}

void PPU::H_BLANK()
{
    //placeholder - count down remaining cycle from 456T cycles = 114 machine cycles
}

void PPU::V_BLANK()
{
    //placeholder - wait for 4560T cycles = 1140 machine cycles
    //ideally, let the cpu access vram at this point
    MEM[LY_register] = 0;
}


void PPU::pixel_fetcher()
{
    //placeholder
    WORD tile_data_base_loc;
    WORD tile_address; //after calculating tile number and base_loc, final address

    BYTE tile_data_low; //first byte of pixels
    BYTE tile_data_high; //second byte of pixels

    int tile_dat_pixel_index = 0;

    WORD tilemap_mem_loc = 0x9800; //window location

    WORD tile_x; //where we are on the line
    WORD tile_y; //what line we're on

    //may add in the future a check for bit 5. if bit 5 is off, window is to be absolutely ignored regardless of other bits, and background will be drawn instead.
    if(get_LCDC_bg_tile_map_select_status() || get_LCDC_window_tile_map_select())
        tilemap_mem_loc = 0x9C00;

    BYTE tilenum;

    //-----------------------------BACKGROUND/WINDOW FETCHER
    while(pixel_fetcher_x_position_counter < 160)
    {

    //window rendering
    if ((pixel_fetcher_x_position_counter >= MEM[WX_reg] && MEM[LY_register] >= MEM[WY_reg]) && get_LCDC_window_display_enable_status())
    {
        //upon reaching this for the first time in the line, we reset pixel_fetcher_x counter and add WX - 7
        if(first_window_encounter)
        {
            pixel_fetcher_x_position_counter = MEM[WX_reg]; //-7
            first_window_encounter = 0;
        }
        tile_x = pixel_fetcher_x_position_counter;

        //tile_y = MEM[LY_register]; //ADD WINDOW LINE COUNTER
        tile_y = WINDOW_LINE_COUNTER / 8;
        WINDOW_LINE_COUNTER++; //we reset this counter upon entering V_BLANK

        tilenum = MEM[tilemap_mem_loc + (tile_x + (tilemap_row_length_bytes * tile_y) % tilemap_size)];

    }
    //background rendering
    else
    {
        //reset first_window_encounter, since if we're here, we're past/before window
        if(!first_window_encounter)
            first_window_encounter = 1;

        tile_x = ((pixel_fetcher_x_position_counter/8) + (MEM[SCX]/8) ) & 0x1F;
        tile_y = ((MEM[LY_register] + MEM[SCY]) & 0xff) / 8;

        tilenum = MEM[tilemap_mem_loc + ((tile_x + (tile_y * tilemap_row_length_bytes)) % tilemap_size)];
    }



    if (get_LCDC_tile_data_select())
    {
        //$8000 method
        //meaning that tiles 0-127 are in block 0, and tiles 128-255 are in block 1.
        tile_data_base_loc = 0x8000;

        //base address of tile data + jump to tile + jump to line in tile
        tile_address = tile_data_base_loc + (tile_size_bytes * tilenum) + (2 * ((MEM[LY_register] + MEM[SCY]) % 8));
    }
    else
    {
        //$8800 method
        //tiles 0-127 are in block 2, and tiles -128 to -1 are in block 1;
        //to put it differently, “$8800 addressing” takes tiles 0-127 from block 2
        // and tiles 128-255 from block 1.
        tile_data_base_loc = 0x9000;

        tile_address = tile_data_base_loc + (tile_size_bytes * char(tilenum) + (2 * ((MEM[LY_register] + MEM[SCY]) % 8)));
    }

    //get tile data low and high
    tile_data_low = MEM[tile_address];
    tile_data_high = MEM[tile_address + 1];

    WORD pixel_row = tileData_to_pixel_row(tile_data_low,tile_data_high); //generate the pixel row from the tile data
    for(int i = 0 ; i < pixel_row_size && Background_FIFO.size() < 16;i++)
    {
//        BYTE temp_pixel_color = ((pixel_row & 0xC0) >> 6);
//        Pixel pixel_temp = Pixel(temp_pixel_color,MEM[BG_palette_data_reg],0);
//        Background_FIFO.push(pixel_temp);
        //BYTE temp_pixel_color = ((pixel_row & 0xC0) >> 6);
        BYTE temp_pixel_color = ((pixel_row & 0xC000) >> 14);
        pixel_row = (pixel_row << 2);
        BYTE palette = MEM[BG_palette_data_reg];
        //std::cout << MEM[BG_palette_data_reg] << std::endl;
        Pixel pixel_temp = Pixel(temp_pixel_color,MEM[BG_palette_data_reg],0,0);
        Background_FIFO.push(pixel_temp);
    }


    //-----------------------------SPRITE FETCHER
    for(Sprite* spr : visible_OAM_buffer)
    {
        if(spr->get_x_pos() <= pixel_fetcher_x_position_counter + 8)
        {
            //reset background fetch position, pause it

            //step 1 - fetch tile number
            BYTE spr_tile_num = spr->get_tile_num();

            //step 2 - fetch tile data - base tile_data_mem loc is always 0x8000
            WORD sprite_tile_address = 0x8000 + (tile_size_bytes * spr_tile_num) + (2 * ((MEM[LY_register] + MEM[SCY]) % 8));
            BYTE sprite_tile_data_low = MEM[sprite_tile_address];
            BYTE sprite_tile_data_high = MEM[sprite_tile_address + 1];

            ///TODO: implement conditions of fifo push, since fifo can only store 16 pixels.
            /// valid sprite with lowest x-pos takes precedence, other sprites can draw only on free fifo slots (with initial n-taken slots dropped)
        }
    }


    //------------------------------------- SPRITE FETCHER END -------------------
    ///THIS IS WHERE I AM DEBUGGING CURRENTLY!!!!
    // pushing pixels from both fifos
    if(Background_FIFO.size() > pixel_row_size) // && Sprite_FIFO.size() > pixel_row_size)
    {
        Pixel cur_pixel = Background_FIFO.front();
        Screen[MEM[LY_register]][pixel_fetcher_x_position_counter] = cur_pixel.get_color();
        Background_FIFO.pop();
        //push to screen
        for(int i = pixel_fetcher_x_position_counter ; i < pixel_fetcher_x_position_counter + 8; i++)
        {
            pixels.push_back(addPixel({(float)i,(float)MEM[LY_register]},background_palette[cur_pixel.get_color()].get_red(),background_palette[cur_pixel.get_color()].get_green(),background_palette[cur_pixel.get_color()].get_blue()));
        }
    ///THIS IS WHERE I AM DEBUGGING CURRENTLY!!!!
//if(pixels.size() == 31)
//{
//    std::cout << "a";
//}


//        pixels.push_back(addPixel({ (float_t)pixel_fetcher_x_position_counter, (float_t)MEM[LY_register] }, background_palette[cur_pixel.get_color()].get_red(), background_palette[cur_pixel.get_color()].get_green(), background_palette[cur_pixel.get_color()].get_blue()));



        //        std::cout << pixels.size() << '\n';

        //pushim ve shit ---

    }
        pixel_fetcher_x_position_counter += 8;
        //std::cout << (int)MEM[0xFF47] << '\n';
}
    //std::cout << "Finished horizontal line" << std::endl;
    pixel_fetcher_x_position_counter = 0;
    MEM[LY_register]++;

    //if 0, not window

}

void PPU::SFML_draw_screen(int row)
{

}


WORD PPU::tileData_to_pixel_row(BYTE tile_data_low,BYTE tile_data_high)
{
    WORD row_8_pixels = 0;
    for(int i = 0; i<8; i++){
        row_8_pixels = row_8_pixels << 1;
        row_8_pixels |= (tile_data_high & 0x80) >> 7;
        tile_data_high =  tile_data_high << 1;
        row_8_pixels = row_8_pixels << 1;
        row_8_pixels |= (tile_data_low & 0x80) >> 7;
        tile_data_low = tile_data_low << 1;

    }
    return row_8_pixels;
}


BYTE PPU::get_LCDC_sprite_size_status() const
{
    return (MEM[0xFF40] & (BYTE)(1 << 2)) == (BYTE)(1 << 2);
};

BYTE PPU::get_LCDC_tile_data_select() const
{
    return (MEM[0xFF40] & (BYTE)(1 << 4)) == (BYTE)(1 << 4);
};

BYTE PPU::get_LCDC_window_display_enable_status() const
{
    return (MEM[0xFF40] & (BYTE)(1 << 5)) == (BYTE)(1 << 5);
};

BYTE PPU::get_LCDC_bg_tile_map_select_status() const
{
    return (MEM[0xFF40] & (BYTE)(1 << 3)) == (BYTE)(1 << 3);
};

BYTE PPU::get_LCDC_window_tile_map_select() const
{
    return (MEM[0xFF40] & (BYTE)(1 << 6)) == (BYTE)(1 << 6);
}

void PPU::PPU_cycle()
{
    OAM_SCAN();
    DRAW();
//    for(int i = 0 ; i < 160; i++)
//    {
//        pixels.push_back(addPixel({(float)i,(float)MEM[LY_register]},100,100,100));
//    }
    H_BLANK();
    //draw row
    if(MEM[LY_register] >= 144)
    {
        V_BLANK();
    }

};



//

