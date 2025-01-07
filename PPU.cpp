//
// Created by Serjo on 12/23/2024.
//
#include "PPU.h"
//Pixel

Pixel::Pixel(BYTE color, BYTE palette, BYTE background_priority)
{
    data = 0;
    set_color(color);
    set_palette(palette);
    set_background_priority(background_priority);
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
    data = data & 0xfc;
    data = data | 0x03;
}
void Pixel::set_palette(BYTE palette)
{
    data = data & 0xfb;
    data = data | ((palette & 0x01) << 2);
};
void Pixel::set_background_priority(BYTE background_priority)
{
    data = data & 0xf7;
    data = data | ((background_priority & 0x01) << 3);
};







//PPU
PPU::PPU(BYTE* OAM_start,BYTE* VRAM_start, BYTE* MEM_start)
{
    VRAM = VRAM_start;
    OAM = OAM_start;
    MEM = MEM_start;
}

void PPU::clean_visible_OAM_buff()
{
    for(int i = 0 ; i < 10;i++) //size of the array
    {
        visible_OAM_buffer[i] = nullptr;
    }
}

void PPU::OAM_SCAN() //mode 2 of the ppu
{
    clean_visible_OAM_buff();
    // start of every scanline, find sprites to be rendered, add them to buffer given the following conditions per sprites:
    // xpos > 0, LY+16 > ypos, LY+16 < ypos+height (16 or 8), amount of spirtes < 10
    int sprites_in_buff = 0;
    BYTE cur_row = MEM[LY_register]; //LY register
    BYTE tile_size = get_LCDC_sprite_size_status() ? 16 : 8;
    for(int i = 0 ; i < oam_size; i = i+4)
    {
        BYTE y_pos = OAM[i];
        BYTE x_pos = OAM[i+1];
        BYTE tile_num = OAM[i+2];
        BYTE attributes = OAM[i+3];

        if(x_pos > 0 && cur_row + 16 >= y_pos && cur_row + 16 < y_pos + tile_size && sprites_in_buff < 10)
        {
            visible_OAM_buffer[sprites_in_buff] = &OAM[i]; //we point to the first byte of the OAM, currently it is not a distinct struct
            sprites_in_buff++;
        }
    };
}

void PPU::DRAW() //mode 3 of the ppu
{
    //calculate lcd_x_coord...
    //add it to tile_dat_mem when calling pixel_fetcher
    //pixel_fetcher(lcd_x_coord);
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

        tile_x = (pixel_fetcher_x_position_counter + (MEM[SCX]/8) ) & 0x1F;
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
    for(int i = 0 ; i < pixel_row_size;i++)
    {
        BYTE temp_pixel_color = ((pixel_row & 0xC0) >> 6);
        Pixel pixel_temp = Pixel(temp_pixel_color,MEM[BG_palette_data_reg],0);
        Background_FIFO.push(pixel_temp);
    }


    //-----------------------------SPRITE FETCHER

    for()



    // pushing pixels from both fifos
    if(Background_FIFO.size() > pixel_row_size && Sprite_FIFO.size() > pixel_row_size)
    {
        //pushim ve shit ---
    }



    //if 0, not window

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

void PPU::main_loop()
{
    OAM_SCAN();
    DRAW();
    //H_BLANK();
    if(pixel_fetcher_x_position_counter >= 160)
    {
        //V_BLANK();
    }

};

