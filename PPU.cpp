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
    if (this->mode < 0 || this->mode > 3)
    {
        std::cout << "OAM scan is crashing the gameboy\n";
    }
}


void PPU::DRAW() //mode 3 of the ppu
{
    while(CUR_TICK_ppu_machine_cycles < 1)
    {
        switch(this->mode_DRAW) {
            case (0):
                modes_trace.push(30);
                //if we are here, we fetch bg/window normally
//                Fetch_BG_tile_num_and_address();
                Fetch_tile_num_and_address(this->mode_BG);
                num_of_machine_cycles(0.5);
                mode_DRAW++;
                Pop_to_screen();
//                Push_to_SPRITE_FIFO();
                Pop_to_screen();
//                Push_to_SPRITE_FIFO();
                if (CUR_TICK_ppu_machine_cycles >= 1) ///1-M CYCLE TICK LIMITATION
                    break;
            case (1):
                modes_trace.push(31);
//                Fetch_Background_Tile_Data_low();
                Fetch_Tile_Data_low(this->mode_BG);
                num_of_machine_cycles(0.5);
                mode_DRAW++;
                Pop_to_screen();
//                Push_to_SPRITE_FIFO();
                Pop_to_screen();
//                Push_to_SPRITE_FIFO();
                if (CUR_TICK_ppu_machine_cycles >= 1) ///1-M CYCLE TICK LIMITATION
                    break;
            case (2):
                modes_trace.push(32);
//            if(first_iteration_in_line)
//                mode_DRAW = 0;
//            else
//            {
//                Fetch_Background_Tile_Data_high();
                Fetch_Tile_Data_high(this->mode_BG);
                mode_DRAW++;
                Pop_to_screen();
//                Push_to_SPRITE_FIFO();
                Pop_to_screen();
//                Push_to_SPRITE_FIFO();
//            }
                num_of_machine_cycles(0.5);
                if (CUR_TICK_ppu_machine_cycles >= 1) ///1-M CYCLE TICK LIMITATION
                    break;
            case (3):
                modes_trace.push(33);
                if(this->mode_BG || Background_FIFO.empty())
                {
                    Push_to_BG_FIFO();
                    num_of_machine_cycles(0.5);
                }
                else
                {
                    Push_to_SPRITE_FIFO();
                    num_of_machine_cycles(0.5);
                }



                Pop_to_screen();
//                Push_to_SPRITE_FIFO();
                Pop_to_screen();
//                Push_to_SPRITE_FIFO();

                if (screen_coordinate_x >= SCREEN_X_END) //if we finished with the line
                {
                    mode = H_BLANK_MODE;
                    set_LCDS_PPU_MODE_status(H_BLANK_MODE);
                    break;
                }
                if(CUR_TICK_ppu_machine_cycles >= 1) ///1-M CYCLE TICK LIMITATION
                    break;
                else
                {
                    mode_DRAW = 0;
                    break;
                }
                break;

            default:
                std::cout << "what the fuck is going on, mode_DRAW is borked\n";
                int pops = 0;
                while(pops < 5 && !modes_trace.empty())
                {

                    std::cout << +modes_trace.top() << '\n';
                    pops++;
                    modes_trace.pop();
                }
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
    this->mode_H_BLANK++;
    if(mode_H_BLANK >= 114) //count down remaining cycle from 456T cycles = 114 machine cycles
    {
        if(first_window_encounter == 0)
            WINDOW_LINE_COUNTER = (WINDOW_LINE_COUNTER + 1);

        this->pixel_fetcher_x_position_counter = 0;
        this->x_position_sprite = 0;
        this->MEM[LY_register]++;

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
            Background_FIFO.pop_front();
        }
        while(!Sprite_FIFO.empty())
        {
            Sprite_FIFO.pop_front();
        }
        this->mode_DRAW = 0; //restart DRAW cycle


        this->screen_coordinate_x = 0;
        this->first_iteration_in_line = 2;

        ///post mode
        this->mode_H_BLANK = 0;

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
//            Background_FIFO.pop_front();
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
        WINDOW_LINE_COUNTER = 0;
        mode_V_BLANK = 0;
        this->mode = OAM_SCAN_MODE;
        set_LCDS_PPU_MODE_status(OAM_SCAN_MODE);
        MEM[LY_register] = 0;

        ///DEBUG
        if(DEBUG_FLAG)
        {
            DEBUG_FLAG = false;
            std::cout << "VBLANK REACHED!\n";
            std::cout << SCX_VAL_DEBUG << " + " << screen_coordinate_x << '\n';
        }

        while(!modes_trace.empty())
        {
//            std::cout << "emptying trace";
            modes_trace.pop();
        }
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
void PPU::Fetch_tile_num_and_address(bool BG)
{
    /// XXX TO REMOVE
//    first_iteration_in_line = (pixel_fetcher_x_position_counter == 0);

    if(BG) {
        //window rendering
        if ((pixel_fetcher_x_position_counter >= MEM[WX_reg] - 7 && MEM[LY_register] >= MEM[WY_reg]) &&
            get_LCDC_window_display_enable_status()) {
            if (MEM[LY_register] > 0) {
                int tmp = 1;
                tmp++;
            }
            if (first_window_encounter) {
                first_window_encounter = 0; //temporary solution, may add other actions to it like emptying fifo and such
                while (!Background_FIFO.empty()) {
                    Background_FIFO.pop_front();
                }
                pixel_fetcher_x_position_counter = MEM[WX_reg] - 7;
            }

            tilemap_mem_loc = 0x9C00;

            tile_x = ((pixel_fetcher_x_position_counter / 8 - (MEM[WX_reg]) / 8)) & 0x1F;
            tile_y = ((WINDOW_LINE_COUNTER / 8) & 0xff);
            tilenum = MEM[tilemap_mem_loc + ((tile_x + (tile_y * tilemap_row_length_bytes)) % tilemap_size)];


            if (get_LCDC_tile_data_select()) {
                //$8000 method
                //meaning that tiles 0-127 are in block 0, and tiles 128-255 are in block 1.
                tile_data_base_loc = 0x8000;

                //base address of tile data + jump to tile + jump to line in tile
                tile_address_background =
                        tile_data_base_loc + (tile_size_bytes * tilenum) + (2 * (WINDOW_LINE_COUNTER % 8));
            } else {
                //$8800 method
                //tiles 0-127 are in block 2, and tiles -128 to -1 are in block 1;
                //to put it differently, “$8800 addressing” takes tiles 0-127 from block 2
                // and tiles 128-255 from block 1.
                tile_data_base_loc = 0x9000;

                //            tile_address_background = tile_data_base_loc + (tile_size_bytes * char(tilenum) + (2 * (WINDOW_LINE_COUNTER % 8)));

                tile_address_background =
                        tile_data_base_loc + (tile_size_bytes * char(tilenum) + (2 * (WINDOW_LINE_COUNTER % 8)));
            }


        }
            //background rendering
        else {
            tilemap_mem_loc = 0x9800;
            //reset first_window_encounter, since if we're here, we're past/before window
            if (!first_window_encounter)
                first_window_encounter = 1;

            tile_x = ((pixel_fetcher_x_position_counter / 8) + (((MEM[SCX])) / 8)) & 0x1F;
            tile_y = ((MEM[LY_register] + MEM[SCY]) & 0xff) / 8;

            tilenum = MEM[tilemap_mem_loc + ((tile_x + (tile_y * tilemap_row_length_bytes)) % tilemap_size)];


            if (get_LCDC_tile_data_select()) {
                //$8000 method
                //meaning that tiles 0-127 are in block 0, and tiles 128-255 are in block 1.
                tile_data_base_loc = 0x8000;

                //base address of tile data + jump to tile + jump to line in tile
                tile_address_background =
                        tile_data_base_loc + (tile_size_bytes * tilenum) + (2 * ((MEM[LY_register] + MEM[SCY]) % 8));
            } else {
                //$8800 method
                //tiles 0-127 are in block 2, and tiles -128 to -1 are in block 1;
                //to put it differently, “$8800 addressing” takes tiles 0-127 from block 2
                // and tiles 128-255 from block 1.
                tile_data_base_loc = 0x9000;

                tile_address_background = tile_data_base_loc +
                                          (tile_size_bytes * char(tilenum) + (2 * ((MEM[LY_register] + MEM[SCY]) % 8)));
            }
        }
    }
    else //sprite mode
    {
        BYTE current_screen_pixel_x = screen_coordinate_x; //this gets updated with each pixel push_back, as opposed to screen_coordinate which gets updated which each pop_front
        const Sprite *spr = Sprite_Fetch_Requests.front();
        if(spr->get_y_flip_flag())
        {
            tile_address_sprite = 0x8000 + (tile_size_bytes * spr->get_tile_num()) + (2 * ((((spr->get_y_pos()-1) - (MEM[LY_register])) % 8)));
        }
        else
        {
            tile_address_sprite = 0x8000 + (tile_size_bytes * spr->get_tile_num()) + (2 * (7-(((spr->get_y_pos()-1) - (MEM[LY_register])) % 8)));
        }
    }
}


void PPU::Fetch_BG_tile_num_and_address()
{
    first_iteration_in_line = (pixel_fetcher_x_position_counter == 0);

    //may add in the future a check for bit 5. if bit 5 is off, window is to be absolutely ignored regardless of other bits, and background will be drawn instead.
//    if(get_LCDC_bg_tile_map_select_status() || get_LCDC_window_tile_map_select())
//        tilemap_mem_loc = 0x9C00;
//    else
//        tilemap_mem_loc = 0x9800;

    //-----------------------------BACKGROUND/WINDOW FETCHER
//    while(pixel_fetcher_x_position_counter <= 160) //TEMP - should be less than
//    {

    //window rendering
    if ((pixel_fetcher_x_position_counter >= MEM[WX_reg]-7 && MEM[LY_register] >= MEM[WY_reg]) && get_LCDC_window_display_enable_status())
    {
        if(MEM[LY_register] > 0)
        {
            int tmp = 1;
            tmp++;
        }
        if(first_window_encounter)
        {
            first_window_encounter = 0; //temporary solution, may add other actions to it like emptying fifo and such
            while(!Background_FIFO.empty())
            {
                Background_FIFO.pop_front();
            }
            pixel_fetcher_x_position_counter = MEM[WX_reg] - 7;
        }

        tilemap_mem_loc = 0x9C00;
//        //upon reaching this for the first time in the line, we reset pixel_fetcher_x counter and add WX - 7
//        if(first_window_encounter)
//        {
//            //empty out fifo
//            while(!Background_FIFO.empty())
//            {
//                Background_FIFO.pop_front();
//            }
//            pixel_fetcher_x_position_counter = MEM[WX_reg] - 7; //-7
//            first_window_encounter = 0;
//        }
//        tile_x = pixel_fetcher_x_position_counter;
//
//        //tile_y = MEM[LY_register]; //ADD WINDOW LINE COUNTER
//        tile_y = WINDOW_LINE_COUNTER / 8;
//        WINDOW_LINE_COUNTER++; //we reset this counter upon entering V_BLANK

//        tile_x = ((pixel_fetcher_x_position_counter/8) + (MEM[SCX]/8) ) & 0x1F;
//        tile_y = ((MEM[LY_register] + MEM[SCY]) & 0xff) / 8;

        tile_x = ((pixel_fetcher_x_position_counter/8 - (MEM[WX_reg])/8)) & 0x1F;
        tile_y = ((WINDOW_LINE_COUNTER / 8) & 0xff);
        tilenum = MEM[tilemap_mem_loc + ((tile_x + (tile_y * tilemap_row_length_bytes)) % tilemap_size)];



        if (get_LCDC_tile_data_select())
        {
            //$8000 method
            //meaning that tiles 0-127 are in block 0, and tiles 128-255 are in block 1.
            tile_data_base_loc = 0x8000;

            //base address of tile data + jump to tile + jump to line in tile
            tile_address_background = tile_data_base_loc + (tile_size_bytes * tilenum) + (2 * (WINDOW_LINE_COUNTER % 8));
        }
        else
        {
            //$8800 method
            //tiles 0-127 are in block 2, and tiles -128 to -1 are in block 1;
            //to put it differently, “$8800 addressing” takes tiles 0-127 from block 2
            // and tiles 128-255 from block 1.
            tile_data_base_loc = 0x9000;

//            tile_address_background = tile_data_base_loc + (tile_size_bytes * char(tilenum) + (2 * (WINDOW_LINE_COUNTER % 8)));

            tile_address_background = tile_data_base_loc + (tile_size_bytes * char(tilenum) + (2 * (WINDOW_LINE_COUNTER % 8)));
        }


    }
        //background rendering
    else
    {
        tilemap_mem_loc = 0x9800;
        //reset first_window_encounter, since if we're here, we're past/before window
        if(!first_window_encounter)
            first_window_encounter = 1;

        tile_x = ((pixel_fetcher_x_position_counter/8) + (((MEM[SCX]))/8) ) & 0x1F;
        tile_y = ((MEM[LY_register] + MEM[SCY]) & 0xff) / 8;

        tilenum = MEM[tilemap_mem_loc + ((tile_x + (tile_y * tilemap_row_length_bytes)) % tilemap_size)];
//        if(tilenum==0x3F)
//        {
//            std::cout << (int)tilenum << std::endl;
//        }
//        if(get_LCDC_window_display_enable_status())
//        {
//            std::cout << "line: " << (int)MEM[LY_register] << '\n';
//        }


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

void PPU::Fetch_Tile_Data_high(bool BG)
{
    if(BG)
        tile_data_high_background = MEM[tile_address_background + 1];
    else
        tile_data_high_sprite = MEM[tile_address_sprite + 1];
}
void PPU::Fetch_Tile_Data_low(bool BG)
{
    if(BG)
        tile_data_low_background = MEM[tile_address_background];
    else
        tile_data_low_sprite = MEM[tile_address_sprite];

//    if(pixel_fetcher_x_position_counter == 16 || pixel_fetcher_x_position_counter == 8 || pixel_fetcher_x_position_counter == 0)
//    {
//        std::cout << pixel_fetcher_x_position_counter << ":" << tile_address_background << '\n';
//    }
}


void PPU::Scan_visible_OAM_buffer()
{
    for(auto & i : visible_OAM_buffer)
    {
        const Sprite *spr = &i;


        if (spr->get_x_pos() == this->screen_coordinate_x + this->x_position_sprite)
        {
            this->Sprite_Fetch_Requests.push_front(spr);
        }
    }
    if(!Sprite_Fetch_Requests.empty())
    {
        waiting_for_visible_sprite_fetch = true;
//        this->mode_BG = 0;//switching to SPRITE MODE
    }

}
void PPU::Push_to_SPRITE_FIFO()
{
    BYTE current_screen_pixel_x = screen_coordinate_x; //this gets updated with each pixel push_back, as opposed to screen_coordinate which gets updated which each pop_front
    WORD sprite_pixel_row = tileData_to_pixel_row(tile_data_low_sprite,tile_data_high_sprite);
    const Sprite* spr = Sprite_Fetch_Requests.front();

    BYTE color_palette = spr->get_palette_number_flag();

    std::vector<Pixel> sprite_pixels;

    //make pixel row
    for(int j = 0 ; j < 8; j++)
    {
        BYTE temp_pixel_color = ((sprite_pixel_row & 0xC000) >> 14);
        sprite_pixel_row = (sprite_pixel_row << 2);
        BYTE palette = MEM[BG_palette_data_reg]; ///will be changed to sprite palette later on
        Pixel pixel_temp = Pixel(temp_pixel_color,color_palette,spr->get_obj_to_bg_priority_flag(),1); //will need to edit the constructor according to sprite reqs
        sprite_pixels.push_back(pixel_temp);
    }

    //converting FIFO to VECTOR to overwrite transparent pixels
    std::vector<Pixel> Sprite_FIFO_vector(Sprite_FIFO.begin(), Sprite_FIFO.end());

    //merge pixel row with the sprite FIFO
    int flip = spr->get_x_flip_flag();
//    for(size_t fifo_size = Sprite_FIFO_vector.size(), i = 0; i < sprite_pixels.size();i++)
//    BYTE i = (spr->get_x_pos() - (this->screen_coordinate_x+this->x_position_sprite)) % 8;


    ///TEMPORARY UGLY SOLUTION
    if (first_iteration_in_line)
    {
        for(size_t fifo_size = Sprite_FIFO_vector.size(), i = 8-spr->get_x_pos(); i< 8;i++)
        {
            size_t sprite_pixels_index = flip ? 7-i : i;
//        size_t sprite_pixels_index = i;
            //overwrite existing transparent pixels
            if(i < fifo_size && Sprite_FIFO_vector[i].get_color() == 0) //meaning transparent
            {
                Sprite_FIFO_vector[i] = sprite_pixels[sprite_pixels_index];
            }
                //push pixels if possible
            else if(i >= fifo_size && fifo_size < 8)
            {
                Sprite_FIFO_vector.push_back(sprite_pixels[sprite_pixels_index]);
            }
            //if FIFO is both full and has no transparent pixels, nothing will change

        }
    }
    else
    {
        for(size_t fifo_size = Sprite_FIFO_vector.size(), i = 0; i< 8;i++)
        {
            size_t sprite_pixels_index = flip ? 7-i : i;
//        size_t sprite_pixels_index = i;
            //overwrite existing transparent pixels
            if(i < fifo_size && Sprite_FIFO_vector[i].get_color() == 0) //meaning transparent
            {
                Sprite_FIFO_vector[i] = sprite_pixels[sprite_pixels_index];
            }
                //push pixels if possible
            else if(i >= fifo_size && fifo_size < 8)
            {
                Sprite_FIFO_vector.push_back(sprite_pixels[sprite_pixels_index]);
            }
            //if FIFO is both full and has no transparent pixels, nothing will change

        }
    }

    //overwrite the FIFO with the new data
    Sprite_FIFO = std::deque<Pixel>(Sprite_FIFO_vector.begin(),Sprite_FIFO_vector.end());

//    visible_OAM_buffer.erase(visible_OAM_buffer.begin());

    //POPPING THE SPRITE FROM THE REQUESTS QUEUE, RETURNING TO NORMAL if possible
    Sprite_Fetch_Requests.pop_front();
    if(Sprite_Fetch_Requests.empty())
    {
        waiting_for_visible_sprite_fetch = false;
        this->mode_BG = 1;

    }
    else //there's more sprites waiting to be fetched, we restart the draw mode again
    {
        this->mode_BG = 0; //unnecessary, kept for readability
        this->mode_DRAW = 0; //restart process, fetch another sprite
    }


}

//void PPU::Push_to_SPRITE_FIFO() ///OLD
//{
////    if(screen_coordinate_x + 8 >= 0x7C && MEM[LY_register] == 0x45)
////    {
////        std::cout << "oi";
////    }
//    BYTE current_screen_pixel_x = screen_coordinate_x; //this gets updated with each pixel push_back, as opposed to screen_coordinate which gets updated which each pop_front
//    //scan the sprite buffer to find relevant objects
//    for(int i = 0 ; i < visible_OAM_buffer.size();i++)
//    {
//        Sprite spr = visible_OAM_buffer[i];
//        if (spr.get_x_pos() <= screen_coordinate_x + 8)
//        {
//
//            //1.FETCH TILE No.
//            //step 1 - fetch tile number
//            //step 2 - fetch tile data - base tile_data_mem loc is always 0x8000
////            Fetch_SPRITE_tile_address(spr.get_tile_num());
//            if(spr.get_y_flip_flag())
//            {
//                tile_address_sprite = 0x8000 + (tile_size_bytes * spr.get_tile_num()) + (2 * ((((spr.get_y_pos()-1) - (MEM[LY_register])) % 8)));
//            }
//            else
//            {
//                tile_address_sprite = 0x8000 + (tile_size_bytes * spr.get_tile_num()) + (2 * (7-(((spr.get_y_pos()-1) - (MEM[LY_register])) % 8)));
//            }
//
//            //2.FETCH TILE DATA LOW
//            Fetch_Sprite_Tile_Data_low();
//            //3.FETCH TILE DATA HIGH
//            Fetch_Sprite_Tile_Data_high();
//            WORD sprite_pixel_row = tileData_to_pixel_row(tile_data_low_sprite,tile_data_high_sprite);
//
//
//            BYTE color_palette = spr.get_palette_number_flag();
//
//
//
////            if((screen_coordinate_x >= 0x6A &&  visible_OAM_buffer[i].get_x_pos() == 0x72 && visible_OAM_buffer[i].get_y_pos() == 0x3B))
////            {
////                BYTE tmp_cur_line = MEM[LY_register]; //for debugging purposes
//////                std::cout << tmp_cur_line << std::endl;
//////                std::cout << "oi";
////            }
//
//            //we turn the pixel row into an array of 8 pixels - THIS CAN BE OPTIMIZED FURTHER IF NEEDED
//            //4.PUSH TO FIFO
//            std::vector<Pixel> sprite_pixels;
//            for(int j = 0 ; j < 8; j++)
//            {
//                BYTE temp_pixel_color = ((sprite_pixel_row & 0xC000) >> 14);
//                sprite_pixel_row = (sprite_pixel_row << 2);
//                BYTE palette = MEM[BG_palette_data_reg]; ///will be changed to sprite palette later on
//                Pixel pixel_temp = Pixel(temp_pixel_color,color_palette,spr.get_obj_to_bg_priority_flag(),1); //will need to edit the constructor according to sprite reqs
//                sprite_pixels.push_back(pixel_temp);
//            }
//
//            //now we push_back pixels
//            for(int m = Sprite_FIFO.size(); m < 8 && current_screen_pixel_x <= spr.get_x_pos() ; m++,current_screen_pixel_x++)
//            {
//                if(spr.get_x_flip_flag())
//                {
//                    Sprite_FIFO.push_front(sprite_pixels[m]);
//                }
//                else
//                {
//                    Sprite_FIFO.push_back(sprite_pixels[m]);
//                }
//            }
//
//
//            visible_OAM_buffer.erase(visible_OAM_buffer.begin());
//        }
////        else
////        {
////            break;
////        }
//
//    }
//}

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
//we pop_front a single pixel for now
void PPU::Pop_to_screen()
{
    ///dealing with SCX scrolling - XXX TO REMOVE
//    bool pixels_popped = false;
//
//    for(int i = (MEM[SCX] % 8) ; first_iteration_in_line && i > 0 && !Background_FIFO.empty(); i--)
//    {
//        Background_FIFO.pop_front();
//        pixels_popped = true;
//    }
//    if(pixels_popped)
//        first_iteration_in_line = false;
    ///


    if(!Background_FIFO.empty() && !waiting_for_visible_sprite_fetch && screen_coordinate_x <= SCREEN_X_END) //means we'll be popping from the background // && screen_coordinate_x <= SCREEN_X_END
    {
        Pixel cur_bg_pixel = Background_FIFO.front();
        Pixel *cur_visible_pixel = &cur_bg_pixel;
        Pixel cur_sprite_pixel;

        //IF sprite fifo is not empty AND this is not the first iteration AND we are NOT scrolling
        if(!Sprite_FIFO.empty() && first_iteration_in_line == 0 && !(pixel_fetcher_x_position_counter <= 8 && ((MEM[SCX] % 8) > 8 - Background_FIFO.size()))) //means we'll also be popping from the sprite
        {
            cur_sprite_pixel = Sprite_FIFO.front();
            if((cur_sprite_pixel.get_color() == 0 || (cur_sprite_pixel.get_background_priority() && cur_bg_pixel.get_color() != 0))) //0 means transparent
                cur_visible_pixel = &cur_bg_pixel;
            else
                cur_visible_pixel = &cur_sprite_pixel;
            Sprite_FIFO.pop_front();
        }
        Background_FIFO.pop_front();

        ///XXX TO REMOVE
//        Screen[MEM[LY_register]][screen_coordinate_x] = *cur_visible_pixel;
//        screen_coordinate_x++;
//        //Scanning for all matching sprites now
//        Scan_visible_OAM_buffer();

//        if(pixel_fetcher_x_position_counter == 0 && !first_iteration_in_line) //dummy fetch
//        {
//
//        }
        ///TEMPORARY CONSTRUCTION ZONE
        if(first_iteration_in_line) //part of dummy fetch
        {
//            std::cout << "oioi";
            x_position_sprite++; // ALWAYS BETWEEN 0-8
            Scan_visible_OAM_buffer();
            if(x_position_sprite >= 8)
            {
                this->first_iteration_in_line = 0;
//                if(!Background_FIFO.empty())
//                {
//                    std::cout << "oi";
//                }

            }
        }
        else if(pixel_fetcher_x_position_counter <= 8 && ((MEM[SCX] % 8) >= 8 - Background_FIFO.size())) //scrolling
        {

        }
        else //not dummy fetch,
        {
//            if(pixel_fetcher_x_position_counter <= 8)
//            {
//                std::cout << "oi";
//            }
            Screen[MEM[LY_register]][screen_coordinate_x] = *cur_visible_pixel;
            screen_coordinate_x++;
            //Scanning for all matching sprites now
            Scan_visible_OAM_buffer();
        }
        ///

//        if(first_iteration_in_line)
//        {
//            if(screen_coordinate_x > 7 || ((MEM[SCX] % 8) < 8 - Background_FIFO.size()))
//            {
//                first_iteration_in_line = false;
//            }
//            else
//            {
//                std::cout << Background_FIFO.size() << '\n';
//            }
//        }
//        if(!first_iteration_in_line)
//        {
//            Screen[MEM[LY_register]][screen_coordinate_x] = *cur_visible_pixel;
//            screen_coordinate_x++;
//            //Scanning for all matching sprites now
//            Scan_visible_OAM_buffer();
//        }

    }
}


void PPU::Push_to_BG_FIFO()
{


    if(Background_FIFO.empty())
    {
        WORD pixel_row = tileData_to_pixel_row(tile_data_low_background, tile_data_high_background); //generate the pixel row from the tile data
        for(int i = 0 ; i < pixel_row_size;i++)
        {
//        BYTE temp_pixel_color = ((pixel_row & 0xC0) >> 6);
//        Pixel pixel_temp = Pixel(temp_pixel_color,MEM[BG_palette_data_reg],0);
//        Background_FIFO.push_back(pixel_temp);
            //BYTE temp_pixel_color = ((pixel_row & 0xC0) >> 6);
            BYTE temp_pixel_color = ((pixel_row & 0xC000) >> 14);
            pixel_row = (pixel_row << 2);
            BYTE palette = MEM[BG_palette_data_reg];
            //std::cout << MEM[BG_palette_data_reg] << std::endl;
            Pixel pixel_temp = Pixel(temp_pixel_color,MEM[BG_palette_data_reg],0,0);
            Background_FIFO.push_back(pixel_temp);
        }
        pixel_fetcher_x_position_counter += 8;

        ///TEMPORARY CONSTRUCTION ZONE
        if(pixel_fetcher_x_position_counter == 8 && first_iteration_in_line == 2)
        {
            first_iteration_in_line = 1; //now we are dependent on emptying out the BG fifo, while fetching the appropriate 0-7 sprite x_loc
            pixel_fetcher_x_position_counter = 0;

        }
        ///

        mode_DRAW = 0; //restart DRAW cycle
    }
    else // NOT EMPTY
    {
        if (waiting_for_visible_sprite_fetch)
        {
            this->mode_BG = 0;
            mode_DRAW = 0; //restart draw cycle - this time with the sprite fetcher - when done, continue with bg
        }
        else
        {
//            std::cout << pixel_fetcher_x_position_counter << '\n';

        }

    }


    // popping pixels from both fifos

//    if(Background_FIFO.size() > pixel_row_size) // && Sprite_FIFO.size() > pixel_row_size)

        //push_back to screen

//        while(!Background_FIFO.empty())
//        {
//            Pixel cur_pixel = Background_FIFO.front();
//            Screen[MEM[LY_register]][screen_coordinate_x] = cur_pixel.get_color();
//            Background_FIFO.pop_front();
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
//        Background_FIFO.pop_front();
//    }
//
//    screen_coordinate_x = 0;


    //if 0, not window

};

////
BYTE PPU::get_LCDC_bg_window_enable_status() const
{
    return (MEM[LCD_Control_reg] & (BYTE)(1)) == (BYTE)(1);
}

BYTE PPU::get_LCDC_sprite_enable_status() const
{
    return (MEM[LCD_Control_reg] & (BYTE)(1 << 1)) == (BYTE)(1 << 1);
}

BYTE PPU::get_LCDC_sprite_size_status() const
{
    return (MEM[LCD_Control_reg] & (BYTE)(1 << 2)) == (BYTE)(1 << 2);
};

BYTE PPU::get_LCDC_bg_tile_map_select_status() const
{
    return (MEM[LCD_Control_reg] & (BYTE)(1 << 3)) == (BYTE)(1 << 3);
};

BYTE PPU::get_LCDC_tile_data_select() const
{
    return (MEM[LCD_Control_reg] & (BYTE)(1 << 4)) == (BYTE)(1 << 4);
};

BYTE PPU::get_LCDC_window_display_enable_status() const
{
    return (MEM[LCD_Control_reg] & (BYTE)(1 << 5)) == (BYTE)(1 << 5);
};

BYTE PPU::get_LCDC_window_tile_map_select() const
{
    return (MEM[LCD_Control_reg] & (BYTE)(1 << 6)) == (BYTE)(1 << 6);
}

BYTE PPU::get_LCDC_display_enable_status() const
{
    return (MEM[LCD_Control_reg] & (BYTE)(1 << 7)) == (BYTE)(1 << 7);
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

    if(SCX_VAL_DEBUG != MEM[SCX])
    {
        std::cout << "SCX VAL:" << (int)MEM[SCX] << " + " << screen_coordinate_x << '\n';
        SCX_VAL_DEBUG = MEM[SCX];
        DEBUG_FLAG = true;
    }
//    if(MEM[SCX] != 0)
//    {
//        std::cout << screen_coordinate_x << '\n';
//
//    }
//    if(0 == MEM[SCY] && MEM[SCX] != 0)
//    {
//
//        int pops = 0;
//        while(pops < 5 && !modes_trace.empty())
//        {
//
//            std::cout << +modes_trace.top() << '\n';
//            pops++;
//            modes_trace.pop_front();
//        }
//        std::cout << "SCY VAL:" << (int)MEM[SCY] << '\n';
//        std::cout << "loop counter: " << loop_counter << '\n';
////        SCX_VAL_DEBUG = MEM[SCX];
////        DEBUG_FLAG = true;
//    }

    CUR_TICK_ppu_machine_cycles = 0;
    sample_STAT_interrupt_line();
    switch(this->mode)
    {
        case OAM_SCAN_MODE:
            modes_trace.push(2);
            OAM_SCAN();
            return;
        case DRAW_MODE:
//            modes_trace.push_back(3);
            DRAW();
            return;
        case H_BLANK_MODE:
            modes_trace.push(0);
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
            modes_trace.push(1);
            V_BLANK();
            return;
        default:
            std::cout << "pupy's switch case problem";
            return;
    }

};



//

