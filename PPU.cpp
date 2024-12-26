//
// Created by Serjo on 12/23/2024.
//
#include "PPU.h"

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

void PPU::pixel_fetcher(int lcd_x_coord)
{
    //placeholder
    WORD tile_data_base_loc;
    WORD tile_address; //after calculating tile number and base_loc, final address

    BYTE tile_data_low; //first byte of pixels
    BYTE tile_data_high; //second byte of pixels

    int tile_dat_pixel_index = 0;

    WORD tilemap_mem_loc = 0x9800; //window location

    BYTE tile_x;
    BYTE tile_y;

    //may add in the future a check for bit 5. if bit 5 is off, window is to be absolutely ignored regardless of other bits, and background will be drawn instead.
    if(get_LCDC_bg_tile_map_select_status() || get_LCDC_window_tile_map_select())
        tilemap_mem_loc = 0x9C00;


    if ((lcd_x_coord >= WX_reg && MEM[LY_register] >= MEM[WY_reg]) && get_LCDC_window_display_enable_status()) //window rendering
    {
        tile_x = lcd_x_coord;
        tile_y = MEM[LY_register];

        //tile_num

    }
    else //background rendering
    {
        tile_x = (lcd_x_coord + (MEM[SCX]/8) ) & 0x1F;
        tile_y = (MEM[LY_register] + MEM[SCY]) & 255;

        //tile_num
    }

    WORD tile_num = tile_x + (tile_y * 32); //32 is tiles per row;

    if (get_LCDC_tile_data_select())
    {
        //$8000 method
        //meaning that tiles 0-127 are in block 0, and tiles 128-255 are in block 1.
        tile_data_base_loc = 0x8000;
        //here we calculate tile_num using x and y
        tile_address = tile_data_base_loc + tile_num;

    }
    else
    {
        //$8800 method
        //tiles 0-127 are in block 2, and tiles -128 to -1 are in block 1;
        //to put it differently, “$8800 addressing” takes tiles 0-127 from block 2
        // and tiles 128-255 from block 1.
        tile_data_base_loc = 0x9000;

        //here we calculate tile_num using x and y
        tile_address = tile_data_base_loc + tile_num;
    }

    //get tile data low and high
    tile_data_low = MEM[tile_address];
    tile_data_high = MEM[tile_address+1];








    //if 0, not window

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
};


