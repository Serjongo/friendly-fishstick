//
// Created by Serjo on 12/23/2024.
//
#include "PPU.h"



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
Pixel::Pixel() //trash values, will be edited as we get data from the PPU
{
    data = 0;
    set_color(0);
    set_palette(0);
    set_background_priority(0);
    set_type(0);
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
    return ((data & 0x08) >> 3);
};

BYTE Pixel::get_type()
{
    return ((data & 0x10) >> 4);
}

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










PPU::PPU(BYTE* OAM_start,BYTE* VRAM_start, BYTE* MEM_start,gameboy& gameboy) : parent(gameboy)
{
    VRAM = VRAM_start;
    //OAM = OAM_start;
    MEM = MEM_start;
//    background_palette[0] = Color(175,203,70);
//    background_palette[1] = Color(121,170,109);
//    background_palette[2] = Color(34,111,95);
//    background_palette[3] = Color(8,41,85);

//    background_palette[0] = Color(171,211,143);
//    background_palette[1] = Color(231,251,203);
//    background_palette[2] = Color(84,139,112);
//    background_palette[3] = Color(20,44,56);

    background_palette[0] = Color(232,252,204);
    background_palette[1] = Color(172,212,144);
    background_palette[2] = Color(84,140,112);
    background_palette[3] = Color(20,44,56);

//
//    background_palette[3] = Color(8,24,32);
//    background_palette[2] = Color(52,104,86);
//    background_palette[1] = Color(136,192,112);
//    background_palette[0] = Color(224,248,208);

//    background_palette[0] = Color(255,0,70);
//    background_palette[1] = Color(0,255,0);
//    background_palette[2] = Color(0,0,255);
//    background_palette[3] = Color(124,124,124);

    sprite_palette_0[0] = Color(192,192,255);
    sprite_palette_0[1] = Color(96,96,255);
    sprite_palette_0[2] = Color(0,0,192);
    sprite_palette_0[3] = Color(0,0,96);

    sprite_palette_1[0] = Color(255,192,192);
    sprite_palette_1[1] = Color(255,96,96);
    sprite_palette_1[2] = Color(192,0,0);
    sprite_palette_1[3] = Color(96,0,0);

    //OAM = std::make_unique<std::vector<Sprite*>>();
}

//PPU

//machine clock related
void PPU::num_of_machine_cycles(float num)
{
    ppu_machine_cycles += num;
    CUR_TICK_ppu_machine_cycles += num;
}

//

void PPU::clean_visible_OAM_buff()
{
    for(Sprite spr : visible_OAM_buffer)
        visible_OAM_buffer.pop_back();
}

void PPU::clean_OAM_buff()
{
    for(Sprite spr : OAM)
        OAM.pop_back();
}

/// NOTE: the original order should remain if the values are equal, since it is first sorted by OAM_memory chronology.
bool sprite_comparator(const Sprite a,const Sprite b)
{
    return a.get_x_pos() < b.get_x_pos();
}

void PPU::OAM_SCAN() //mode 2 of the ppu
{

    if(OAM_counter == OAM_mem_start){
        clean_visible_OAM_buff(); //should set oam size to 0
        clean_OAM_buff();
    }
    // start of every scanline, find sprites to be rendered, add them to buffer given the following conditions per sprites:
    // xpos > 0, LY+16 > ypos, LY+16 < ypos+height (16 or 8), amount of spirtes < 10
    BYTE cur_row = MEM[LY_register]; //LY register
    BYTE tile_size = get_LCDC_sprite_size_status() ? 16 : 8;

    while(OAM_counter < OAM_mem_end) // goes over 160 bytes, 40 sprites
    {
        Sprite spr = Sprite(MEM[OAM_counter],MEM[OAM_counter+1],MEM[OAM_counter+2],MEM[OAM_counter+3]);
//        OAM.push_back(spr);
//        BYTE y_pos = MEM[i];
//        BYTE x_pos = MEM[i+1];
//        BYTE tile_num = MEM[i+2];
//        BYTE attributes = MEM[i+3];

        //checking that the OAM tile fits the criteria to appear on screen
        if(spr.get_x_pos() > 0 && cur_row + 16 >= spr.get_y_pos() && cur_row + 16 < spr.get_y_pos() + tile_size && visible_OAM_buffer.size() < 10)
        {
//            if(spr.get_y_pos() == 0x3D)
//            {
//                std::cout << "harta";
//            }
            visible_OAM_buffer.push_back(spr); //we point to the first byte of the OAM, currently it is not a distinct struct
        }
        OAM_counter = OAM_counter + 4;
        num_of_machine_cycles(0.5);

        if(CUR_TICK_ppu_machine_cycles >= 1) ///1-M CYCLE TICK LIMITATION
        {
            return;
        }
    }

    //we do not assign cycles to this because the gameboy does not sort the buffer, this is our optimization.
    sort(visible_OAM_buffer.begin(),visible_OAM_buffer.end(),sprite_comparator);

    ///post mode
    OAM_counter = OAM_mem_start; //reset OAM counter once over
    this->mode = DRAW_MODE;
    set_LCDS_PPU_MODE_status(DRAW_MODE);
}


void PPU::DRAW() //mode 3 of the ppu
{
    while(CUR_TICK_ppu_machine_cycles < 1)
    {
        switch(this->mode_DRAW) {
            case (0):
                Fetch_BG_tile_num_and_address();
                num_of_machine_cycles(0.5);
                mode_DRAW++;
                Pop_to_screen();
                Push_to_SPRITE_FIFO();
                Pop_to_screen();
                Push_to_SPRITE_FIFO();
                if (CUR_TICK_ppu_machine_cycles >= 1) ///1-M CYCLE TICK LIMITATION
                    break;
            case (1):
                Fetch_Background_Tile_Data_low();
                num_of_machine_cycles(0.5);
                mode_DRAW++;
                Pop_to_screen();
                Push_to_SPRITE_FIFO();
                Pop_to_screen();
                Push_to_SPRITE_FIFO();
                if (CUR_TICK_ppu_machine_cycles >= 1) ///1-M CYCLE TICK LIMITATION
                    break;
            case (2):
//            if(first_iteration_in_line)
//                mode_DRAW = 0;
//            else
//            {
                Fetch_Background_Tile_Data_high();
                mode_DRAW++;
                Pop_to_screen();
                Push_to_SPRITE_FIFO();
                Pop_to_screen();
                Push_to_SPRITE_FIFO();
//            }
                num_of_machine_cycles(0.5);
                if (CUR_TICK_ppu_machine_cycles >= 1) ///1-M CYCLE TICK LIMITATION
                    break;
            case (3):
                Push_to_BG_FIFO();
                num_of_machine_cycles(0.5);

                Pop_to_screen();
                Push_to_SPRITE_FIFO();
                Pop_to_screen();
                Push_to_SPRITE_FIFO();

                if (screen_coordinate_x > 160) //if we finished with the line
                {
                    mode = H_BLANK_MODE;
                    set_LCDS_PPU_MODE_status(H_BLANK_MODE);
                    break;
                }
                if(CUR_TICK_ppu_machine_cycles >= 1) ///1-M CYCLE TICK LIMITATION
                    break;

        }
    }
//    ///post mode
//    this->mode = H_BLANK_MODE;
//    set_LCDS_PPU_MODE_status(H_BLANK_MODE);
//    if(CUR_TICK_ppu_machine_cycles >= 1) ///1-M CYCLE TICK LIMITATION
//    {
//        return;
//    }



}
///OLD
//void PPU::DRAW() //mode 3 of the ppu
//{
//    Fetch_BG_tile_num_and_address();
//    Fetch_Background_Tile_Data_low();
//    Fetch_Background_Tile_Data_high();
//    Push_to_BG_FIFO();
//}


void PPU::H_BLANK()
{
    num_of_machine_cycles(1);
    mode_H_BLANK++;
    if(mode_H_BLANK >= 114) //count down remaining cycle from 456T cycles = 114 machine cycles
    {
        pixel_fetcher_x_position_counter = 0;
        MEM[LY_register]++;

        //setting coincidence flag according to LY==LYC -- may be used for interrupts later on
        if(MEM[LYC_register] == MEM[LY_register])
        {
            set_LCDS_coincidence_flag_status(1);
            sample_STAT_interrupt_line();
        }
        else
            set_LCDS_coincidence_flag_status(0);

        ///temporary solution, should probably think of something else
        while(!Background_FIFO.empty())
        {
            Background_FIFO.pop();
        }
        mode_DRAW = 0; //restart DRAW cycle


        screen_coordinate_x = 0;

        ///post mode
        mode_H_BLANK = 0;

        if (MEM[LY_register] >= 144) { //POST H_BLANK
            this->mode = V_BLANK_MODE;
            set_vblank_interrupt();
            set_LCDS_PPU_MODE_status(V_BLANK_MODE);
        }
        else
        {
            this->mode = OAM_SCAN_MODE;
            set_LCDS_PPU_MODE_status(OAM_SCAN_MODE);
        }


    }
    else
        return;
}

///OLD
//void PPU::H_BLANK()
//{
//    //placeholder - count down remaining cycle from 456T cycles = 114 machine cycles
//    if (pixel_fetcher_x_position_counter > 160)
//    {
//        pixel_fetcher_x_position_counter = 0;
//        MEM[LY_register]++;
//
//        //setting coincidence flag according to LY==LYC -- may be used for interrupts later on
//        if(MEM[LYC_register] == MEM[LY_register])
//        {
//            set_LCDS_coincidence_flag_status(1);
//            sample_STAT_interrupt_line();
//        }
//
//        else
//            set_LCDS_coincidence_flag_status(0);
//
//        while(!Background_FIFO.empty())
//        {
//            Background_FIFO.pop();
//        }
//
//        screen_coordinate_x = 0;
//    }
//
//}

void PPU::V_BLANK()
{
    //placeholder - wait for 4560T cycles = 1140 machine cycles
    //ideally, let the cpu access vram at this point
    num_of_machine_cycles(1);
    mode_V_BLANK++;
    if((mode_V_BLANK > 0) && ((mode_V_BLANK % 114) == 0)) ///1-M CYCLE TICK LIMITATION
    {
        MEM[LY_register]++;
        //setting coincidence flag according to LY==LYC -- may be used for interrupts later on
        if(MEM[LYC_register] == MEM[LY_register])
        {
            set_LCDS_coincidence_flag_status(1);
            sample_STAT_interrupt_line();
        }

        else
            set_LCDS_coincidence_flag_status(0);
    }


    if(mode_V_BLANK >= 1140) //happens when we pass through 10 lines
    {
        mode_V_BLANK = 0;
        this->mode = OAM_SCAN_MODE;
        set_LCDS_PPU_MODE_status(OAM_SCAN_MODE);
        MEM[LY_register] = 0;
    }
    return;

}


BYTE PPU::get_pixel_color_from_mem(BYTE color,bool background,bool sprite_1)
{
    WORD loc = background ? BG_palette_data_reg: (sprite_1 ? SPRITE_1_palette : SPRITE_0_palette);
    BYTE palette_mapping = MEM[loc];

    BYTE comparator = 0x03;

    for(int i = 0 ; i < color; i++)
    {
        comparator = (comparator << 2);
    }
    comparator = comparator & palette_mapping;
    for(int i = 0 ; i < color; i++)
    {
        comparator = (comparator >> 2);
    }
    return comparator;
}


//void PPU::V_BLANK()
//{
//    //placeholder - wait for 4560T cycles = 1140 machine cycles
//    //ideally, let the cpu access vram at this point
//
//    while(vblank_counter<10){
//        MEM[LY_register]++;
//        vblank_counter++;
//        num_of_machine_cycles(114);
//        return;
//    }
//    vblank_counter = 0;
//    this->mode = OAM_SCAN_MODE;
//    set_LCDS_PPU_MODE_status(OAM_SCAN_MODE);
//    MEM[LY_register] = 0;
//}


//void PPU::SFML_draw_screen(int row)
//{
//
//}


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

// DRAW()'s inner functions

void PPU::Fetch_BG_tile_num_and_address()
{
    first_iteration_in_line = (pixel_fetcher_x_position_counter == 0);

    //may add in the future a check for bit 5. if bit 5 is off, window is to be absolutely ignored regardless of other bits, and background will be drawn instead.
    if(get_LCDC_bg_tile_map_select_status() || get_LCDC_window_tile_map_select())
        tilemap_mem_loc = 0x9C00;

    //-----------------------------BACKGROUND/WINDOW FETCHER
//    while(pixel_fetcher_x_position_counter <= 160) //TEMP - should be less than
//    {

    //window rendering
    if ((pixel_fetcher_x_position_counter >= MEM[WX_reg] && MEM[LY_register] >= MEM[WY_reg]) && get_LCDC_window_display_enable_status())
    {
        //upon reaching this for the first time in the line, we reset pixel_fetcher_x counter and add WX - 7
        if(first_window_encounter)
        {
            pixel_fetcher_x_position_counter = MEM[WX_reg] - 7; //-7
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
//        if(tilenum==0x3F)
//        {
//            std::cout << (int)tilenum << std::endl;
//        }
    }
    if (get_LCDC_tile_data_select())
    {
        //$8000 method
        //meaning that tiles 0-127 are in block 0, and tiles 128-255 are in block 1.
        tile_data_base_loc = 0x8000;

        //base address of tile data + jump to tile + jump to line in tile
        tile_address_background = tile_data_base_loc + (tile_size_bytes * tilenum) + (2 * ((MEM[LY_register] + MEM[SCY]) % 8));
    }
    else
    {
        //$8800 method
        //tiles 0-127 are in block 2, and tiles -128 to -1 are in block 1;
        //to put it differently, “$8800 addressing” takes tiles 0-127 from block 2
        // and tiles 128-255 from block 1.
        tile_data_base_loc = 0x9000;

        tile_address_background = tile_data_base_loc + (tile_size_bytes * char(tilenum) + (2 * ((MEM[LY_register] + MEM[SCY]) % 8)));
    }
};

void PPU::Fetch_SPRITE_tile_address(BYTE sprite_tile_num)
{
    tile_address_sprite = 0x8000 + (tile_size_bytes * sprite_tile_num) + (2 * ((MEM[LY_register] + MEM[SCY]) % 8));
}



void PPU::Fetch_Background_Tile_Data_low(){//get tile data low
    tile_data_low_background = MEM[tile_address_background];
};
void PPU::Fetch_Background_Tile_Data_high()
{
    tile_data_high_background = MEM[tile_address_background + 1];
};

void PPU::Fetch_Sprite_Tile_Data_low()
{
    tile_data_low_sprite = MEM[tile_address_sprite];
};
void PPU::Fetch_Sprite_Tile_Data_high()
{
    tile_data_high_sprite = MEM[tile_address_sprite + 1];
};



void PPU::Push_to_SPRITE_FIFO()
{
//    if(screen_coordinate_x + 8 >= 0x7C && MEM[LY_register] == 0x45)
//    {
//        std::cout << "oi";
//    }
    BYTE current_screen_pixel_x = screen_coordinate_x; //this gets updated with each pixel push, as opposed to screen_coordinate which gets updated which each pop
    //scan the sprite buffer to find relevant objects
    for(int i = 0 ; i < visible_OAM_buffer.size();i++)
    {
        Sprite spr = visible_OAM_buffer[i];
        if (spr.get_x_pos() <= screen_coordinate_x + 8)
        {

            //step 1 - fetch tile number
            //step 2 - fetch tile data - base tile_data_mem loc is always 0x8000
//            Fetch_SPRITE_tile_address(spr.get_tile_num());
            tile_address_sprite = 0x8000 + (tile_size_bytes * spr.get_tile_num()) + (2 * (7-(((spr.get_y_pos()-1) - (MEM[LY_register]) + MEM[SCY]) % 8)));

            Fetch_Sprite_Tile_Data_low();
            Fetch_Sprite_Tile_Data_high();
            WORD sprite_pixel_row = tileData_to_pixel_row(tile_data_low_sprite,tile_data_high_sprite);


            BYTE color_palette = spr.get_palette_number_flag();



            if((screen_coordinate_x >= 0x6A &&  visible_OAM_buffer[i].get_x_pos() == 0x72 && visible_OAM_buffer[i].get_y_pos() == 0x3B))
            {
                BYTE tmp_cur_line = MEM[LY_register]; //for debugging purposes
//                std::cout << tmp_cur_line << std::endl;
//                std::cout << "oi";
            }

            //we turn the pixel row into an array of 8 pixels - THIS CAN BE OPTIMIZED FURTHER IF NEEDED
            std::vector<Pixel> sprite_pixels;
            for(int j = 0 ; j < 8; j++)
            {
                BYTE temp_pixel_color = ((sprite_pixel_row & 0xC000) >> 14);
                sprite_pixel_row = (sprite_pixel_row << 2);
                BYTE palette = MEM[BG_palette_data_reg]; ///will be changed to sprite palette later on
                Pixel pixel_temp = Pixel(temp_pixel_color,color_palette,0,1); //will need to edit the constructor according to sprite reqs
                sprite_pixels.push_back(pixel_temp);
            }

            //now we push pixels
            if(spr.get_x_flip_flag()) //push them back-to-front
            {
                std::stack<Pixel> temporary_flipper;
                for(int k = 0 ; k < 8 ; k++)
                {
                    temporary_flipper.push(sprite_pixels[i]);
                }
                for(int l = Sprite_FIFO.size(); l < 8 && current_screen_pixel_x <= spr.get_x_pos() ; l++,current_screen_pixel_x++)
                {
                    Sprite_FIFO.push(temporary_flipper.top());
                    temporary_flipper.pop();
                }
            }
            else //regular
            {
                for(int m = Sprite_FIFO.size(); m < 8 && current_screen_pixel_x <= spr.get_x_pos() ; m++,current_screen_pixel_x++)
                {
                    if(sprite_pixels[m].get_color() == 0) //transparent
                    {

                        Sprite_FIFO.push(fill_transparent_sprite_pixel(current_screen_pixel_x,i,sprite_pixels[m],spr));
                    }
                    else
                        Sprite_FIFO.push(sprite_pixels[m]);

                }
            }

            visible_OAM_buffer.erase(visible_OAM_buffer.begin());
        }
//        else
//        {
//            break;
//        }

    }
}

Pixel PPU::fill_transparent_sprite_pixel(BYTE cur_screen_pixel,BYTE sprite_index,Pixel default_pixel,Sprite original_sprite)
{
    for(int i = sprite_index+1 ; i < visible_OAM_buffer.size();i++)
    {
        Sprite spr = visible_OAM_buffer[i];
        if (spr.get_x_pos() <= cur_screen_pixel + 8) {

            //step 1 - fetch tile number
            //step 2 - fetch tile data - base tile_data_mem loc is always 0x8000
//            Fetch_SPRITE_tile_address(spr.get_tile_num());
            tile_address_sprite = 0x8000 + (tile_size_bytes * spr.get_tile_num()) +
                                  (2 * (7 - (((spr.get_y_pos() - 1) - (MEM[LY_register]) + MEM[SCY]) % 8)));

            Fetch_Sprite_Tile_Data_low();
            Fetch_Sprite_Tile_Data_high();
            WORD sprite_pixel_row = tileData_to_pixel_row(tile_data_low_sprite, tile_data_high_sprite);


            BYTE color_palette = spr.get_palette_number_flag();

            //we turn the pixel row into an array of 8 pixels - THIS CAN BE OPTIMIZED FURTHER IF NEEDED
            std::vector<Pixel> sprite_pixels;
            for (int i = 0; i < 8; i++) {
                BYTE temp_pixel_color = ((sprite_pixel_row & 0xC000) >> 14);
                sprite_pixel_row = (sprite_pixel_row << 2);
                BYTE palette = MEM[BG_palette_data_reg]; ///will be changed to sprite palette later on
                Pixel pixel_temp = Pixel(temp_pixel_color, color_palette, 0,
                                         1); //will need to edit the constructor according to sprite reqs
                sprite_pixels.push_back(pixel_temp);
            }
//            if((fifo_index >= 0x70))
//            {
//                BYTE tmp_cur_line = MEM[LY_register]; //for debugging purposes
//                std::cout << tmp_cur_line << std::endl;
//                std::cout << "oi";
//            }


//            sprite_pixels[relative_pixel_in_sprite].set_color(3);
            return sprite_pixels[8 - (spr.get_x_pos() - original_sprite.get_x_pos() + (original_sprite.get_x_pos() - cur_screen_pixel))];

        }
    }
    return default_pixel;
}

//we assume that the fifos are in sync, worst case we'll add it later
//we pop a single pixel for now
void PPU::Pop_to_screen()
{
    if(!Background_FIFO.empty() && !waiting_for_visible_sprite_fetch) //means we'll be popping from the background
    {
        if(!Sprite_FIFO.empty()) //means we'll also be popping from the sprite
        {
            Pixel cur_bg_pixel = Background_FIFO.front();
            Pixel cur_visible_pixel = Sprite_FIFO.front();
            if(cur_visible_pixel.get_color() == 0) //0 means transparent
            {
                cur_visible_pixel = cur_bg_pixel;
            }
            Screen[MEM[LY_register]][screen_coordinate_x] = cur_visible_pixel;
            Background_FIFO.pop();
            Sprite_FIFO.pop();
        }
        else //only from background
        {
            Pixel cur_pixel = Background_FIFO.front();
            Screen[MEM[LY_register]][screen_coordinate_x] = cur_pixel;
            Background_FIFO.pop();
        }

        screen_coordinate_x++; //dont need it for now, might delete later
//        pixel_fetcher_x_position_counter += 1;
    }

}


void PPU::Push_to_BG_FIFO()
{
    WORD pixel_row = tileData_to_pixel_row(tile_data_low_background, tile_data_high_background); //generate the pixel row from the tile data
    if(Background_FIFO.empty())
    {
        for(int i = 0 ; i < pixel_row_size;i++)
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
        pixel_fetcher_x_position_counter += 8;
        mode_DRAW = 0; //restart DRAW cycle
    }


    // popping pixels from both fifos

//    if(Background_FIFO.size() > pixel_row_size) // && Sprite_FIFO.size() > pixel_row_size)

        //push to screen

//        while(!Background_FIFO.empty())
//        {
//            Pixel cur_pixel = Background_FIFO.front();
//            Screen[MEM[LY_register]][screen_coordinate_x] = cur_pixel.get_color();
//            Background_FIFO.pop();
//
//            screen_coordinate_x++; //dont need it for now, might delete later
//        }
//        pixel_fetcher_x_position_counter += 8;



//if(pixels.size() == 31)
//{
//    std::cout << "a";
//}



        //        std::cout << pixels.size() << '\n';

        //pushim ve shit ---




    //std::cout << (int)MEM[0xFF47] << '\n';
//}
    //std::cout << "Finished horizontal line" << std::endl;
//    pixel_fetcher_x_position_counter = 0;
//    MEM[LY_register]++;
//    while(!Background_FIFO.empty())
//    {
//        Background_FIFO.pop();
//    }
//
//    screen_coordinate_x = 0;


    //if 0, not window

};

////

BYTE PPU::get_LCDC_sprite_size_status() const
{
    return (MEM[LCD_Control_reg] & (BYTE)(1 << 2)) == (BYTE)(1 << 2);
};

BYTE PPU::get_LCDC_tile_data_select() const
{
    return (MEM[LCD_Control_reg] & (BYTE)(1 << 4)) == (BYTE)(1 << 4);
};

BYTE PPU::get_LCDC_window_display_enable_status() const
{
    return (MEM[LCD_Control_reg] & (BYTE)(1 << 5)) == (BYTE)(1 << 5);
};

BYTE PPU::get_LCDC_bg_tile_map_select_status() const
{
    return (MEM[LCD_Control_reg] & (BYTE)(1 << 3)) == (BYTE)(1 << 3);
};

BYTE PPU::get_LCDC_window_tile_map_select() const
{
    return (MEM[LCD_Control_reg] & (BYTE)(1 << 6)) == (BYTE)(1 << 6);
}

//LCDS getters

BYTE PPU::get_LCDS_lycly_interrupt_enable_status() const
{
    return (MEM[LCD_Status_reg] & (BYTE)(1 << 6)) == (BYTE)(1 << 6);
}
BYTE PPU::get_LCDS_oam_scan_mode_interrupt_enable_status() const //mode 2
{
    return (MEM[LCD_Status_reg] & (BYTE)(1 << 5)) == (BYTE)(1 << 5);
}
BYTE PPU::get_LCDS_vblank_mode_interrupt_enable_status() const //mode 1
{
    return (MEM[LCD_Status_reg] & (BYTE)(1 << 4)) == (BYTE)(1 << 4);
}
BYTE PPU::get_LCDS_hblank_mode_interrupt_enable_status() const //mode 0
{
    return (MEM[LCD_Status_reg] & (BYTE)(1 << 3)) == (BYTE)(1 << 3);
}
BYTE PPU::get_LCDS_coincidence_flag_status() const //if lyc==ly, changed by ppu
{
    return (MEM[LCD_Status_reg] & (BYTE)(1 << 2)) == (BYTE)(1 << 2);
}
BYTE PPU::get_LCDS_PPU_MODE_status() const //bits 1-0
{
    return (MEM[LCD_Status_reg] & (BYTE)(0x03));
}


//LCDS setters
void PPU::set_LCDS_lycly_interrupt_enable_status(BYTE status)
{
    if(status == 0)
    {
        MEM[LCD_Status_reg] = (MEM[LCD_Status_reg] & (BYTE)(~(1 << 6))); //should turn off FLAG_ZERO
    }
    else
    {
        MEM[LCD_Status_reg] = (MEM[LCD_Status_reg] | (BYTE)(1 << 6)); //should turn on FLAG_ZERO
    }
}
void PPU::set_LCDS_oam_scan_mode_interrupt_enable_status(BYTE status) //mode 2
{
    if(status == 0)
    {
        MEM[LCD_Status_reg] = (MEM[LCD_Status_reg] & (BYTE)(~(1 << 5))); //should turn off FLAG_ZERO
    }
    else
    {
        MEM[LCD_Status_reg] = (MEM[LCD_Status_reg] | (BYTE)(1 << 5)); //should turn on FLAG_ZERO
    }
}
void PPU::set_LCDS_vblank_mode_interrupt_enable_status(BYTE status) //mode 1
{
    if(status == 0)
    {
        MEM[LCD_Status_reg] = (MEM[LCD_Status_reg] & (BYTE)(~(1 << 4))); //should turn off FLAG_ZERO
    }
    else
    {
        MEM[LCD_Status_reg] = (MEM[LCD_Status_reg] | (BYTE)(1 << 4)); //should turn on FLAG_ZERO
    }
}
void PPU::set_LCDS_hblank_mode_interrupt_enable_status(BYTE status) //mode 0
{
    if(status == 0)
    {
        MEM[LCD_Status_reg] = (MEM[LCD_Status_reg] & (BYTE)(~(1 << 3))); //should turn off FLAG_ZERO
    }
    else
    {
        MEM[LCD_Status_reg] = (MEM[LCD_Status_reg] | (BYTE)(1 << 3)); //should turn on FLAG_ZERO
    }
}
void PPU::set_LCDS_coincidence_flag_status(BYTE status)
{
    if(status == 0)
    {
        MEM[LCD_Status_reg] = (MEM[LCD_Status_reg] & (BYTE)(~(1 << 2))); //should turn off FLAG_ZERO
    }
    else
    {
        MEM[LCD_Status_reg] = (MEM[LCD_Status_reg] | (BYTE)(1 << 2)); //should turn on FLAG_ZERO
    }
}
void PPU::set_LCDS_PPU_MODE_status(BYTE status)
{
    if(status >= 0 && status <= 3) //only modes between 0 and 3 are accepted
    {
        MEM[LCD_Status_reg] = (MEM[LCD_Status_reg] & 0xFC); //nullify first 2 bits
        MEM[LCD_Status_reg] = (MEM[LCD_Status_reg] | status);
    }
    else
    {
        std::cout << "set_LCDS_PPU_MODE_status illegal input" << std::endl;
    }
}

bool PPU::sample_STAT_interrupt_line() //if any of the conditions are now met, we will trip the interrupt flag in the memory
{
    if(!STAT_interrupt_line)
    {
        STAT_interrupt_line = (
                ( get_LCDS_coincidence_flag_status() & get_LCDS_lycly_interrupt_enable_status() ) |
                ( (get_LCDS_PPU_MODE_status() == H_BLANK_MODE) & get_LCDS_hblank_mode_interrupt_enable_status() ) |
                ( (get_LCDS_PPU_MODE_status() == V_BLANK_MODE) & get_LCDS_vblank_mode_interrupt_enable_status() ) |
                ( (get_LCDS_PPU_MODE_status() == OAM_SCAN_MODE) & get_LCDS_oam_scan_mode_interrupt_enable_status() )
                );
        if(STAT_interrupt_line)
            MEM[0xFF0F] = MEM[0xFF0F] | 0x02; //IF_reg - turn on LCD interrupt
    }
    return STAT_interrupt_line;
}

void PPU::set_vblank_interrupt() //this will be called every single time the V_BLANK mode is activated, requesting for an interrupt from the CPU
{
    MEM[0xFF0F] = MEM[0xFF0F] | 0x01; //IF_reg - turn on VBLANK interrupt
}


void PPU::PPU_cycle()
{
//    OAM_SCAN();
//    DRAW();
////    for(int i = 0 ; i < 160; i++)
////    {
////        pixels.push_back(addPixel({(float)i,(float)MEM[LY_register]},100,100,100));
////    }
//    H_BLANK();
//    //draw row
//    if(MEM[LY_register] >= 144)
//    {
//        V_BLANK();
//    }

    CUR_TICK_ppu_machine_cycles = 0;
    sample_STAT_interrupt_line();
    switch(this->mode)
    {
        case OAM_SCAN_MODE:
            OAM_SCAN();
            return;
        case DRAW_MODE:
            DRAW();
            return;
        case H_BLANK_MODE:
            H_BLANK();
            //draw row
//            if (MEM[LY_register] >= 144) {
//                this->mode = V_BLANK_MODE;
//                set_vblank_interrupt();
//                set_LCDS_PPU_MODE_status(V_BLANK_MODE);
//            }
//            else
//            {
//                this->mode = OAM_SCAN_MODE;
//                set_LCDS_PPU_MODE_status(OAM_SCAN_MODE);
//            }
            return;
        case V_BLANK_MODE:
            V_BLANK();
            return;
        default:
            std::cout << "pupy's switch case problem";
            return;
    }

};



//

