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
    BYTE cur_row = MEM[0xFF44]; //LY register
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
    //pixel_fetcher(tile_dat_mem+lcd_x_coord,lcd_x_coord);
}

void PPU::pixel_fetcher(WORD tile_dat_mem,int lcd_x_coord)
{
    //placeholder
    int tile_dat_pixel_index = 0;

    WORD tilemap_mem_loc = 0x9800; //window location

    //may add in the future a check for bit 5. if bit 5 is off, window is to be absolutely ignored regardless of other bits, and background will be drawn instead.
    if(get_LCDC_bg_tile_map_select_status() || get_LCDC_window_tile_map_select())
        tilemap_mem_loc = 0x9C00;

    BYTE tile_num = MEM[tile_dat_mem];
    //check if ppu is rendering background or window (check if one of the bits is on)


    if (lcd_x_coord < WX_reg) //background rendering
    {
        MEM[SCX]/8 +
    }
    else if (lcd_x_coord >= WX_reg) //window rendering
    {

    }





    //if 0, not window

}

BYTE PPU::get_LCDC_sprite_size_status() const
{
    return (MEM[0xFF40] & (BYTE)(1 << 2)) == (BYTE)(1 << 2);
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


