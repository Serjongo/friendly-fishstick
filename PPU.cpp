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

int* PPU::OAM_SCAN()
{
    // start of every scanline, find sprites to be rendered, add them to buffer given the following conditions per sprites:
    // xpos > 0, LY+16 > ypos, LY+16 < ypos+height (16 or 8), amount of spirtes < 10
    int sprites_in_buff = 0;
    BYTE cur_row = MEM[0xFF44]; //LY register
    BYTE tile_size = get_LCDC_sprite_size_status() ? 16 : 8;
    for(int i = 0 ; i < oam_size; i = i+4)
    {
        BYTE y_pos = OAM[i];
        BYTE x_pos = OAM[i+1];
        BYTE tile_idx = OAM[i+2];
        BYTE attributes = OAM[i+3];

        if(x_pos > 0 && cur_row + 16 >= y_pos && cur_row + 16 < y_pos + tile_size && sprites_in_buff < 10)
        {
            visible_OAM_buffer[sprites_in_buff] = &OAM[i]; //we point to the first byte of the OAM, currently it is not a distinct struct
            sprites_in_buff++;
        }
    };

}

BYTE PPU::get_LCDC_sprite_size_status() const
{
    return (MEM[0xFF40] & (BYTE)(1 << 2)) == (BYTE)(1 << 2);
};


