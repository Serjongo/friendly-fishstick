//
// Created by Serjo on 12/23/2024.
//

#ifndef GB_EMU_2024_PPU_H
#define GB_EMU_2024_PPU_H

#include "main.h"

class PPU{
public:
    const WORD VRAM_mem_start = 0x8000;
    const WORD VRAM_mem_end = 0x97ff;
    const int oam_size = 0xA0; //160 bytes


    BYTE* VRAM; // from the vram start point
    BYTE* OAM; //from the OAM start point
    BYTE* MEM; //1 to 1 memory mapping from the start

    BYTE* visible_OAM_buffer[0x0A]; //pointers to 10 OAMs/sprites

    PPU(BYTE* OAM_start,BYTE* VRAM_start,BYTE* MEM_start); //

    //TODO methods:
    //2 bytes make up a row of 8 pixels - each bit of the first byte is combined with the same in the second
    //SHORT 2bpp_mix(SHORT)

    //background maps ($9800-$9BFF and $9C00-$9FFF) - addressing, 8000 method, 8800 method,  meaning if we start from 8800 and treat memory as signed
    // (so we can go back to 8000), or unsigned (no negative)

    //OAM memory ($FE00-$FE9F), breaking down the sprites bytes: 0 - Y-pos, 1-xpos, 2-tile number, 3 sprite flags

    //scanlines
    // PPU MODES (per scanline)
    // OAM SCAN (mode 2) - 80tcycles:
    // start of every scanline, find sprites to be rendered, add them to buffer given the following conditions per sprites:
    // xpos > 0, LY+16 > ypos, LY+16 < ypos+height (16 or 8), amount of spirtes < 10

    // DRAW (mode 3) - as the name implies - this is where graphics library will come into play

    //h-blank (mode 0) - after every line

    int* OAM_SCAN();

    //getters
    BYTE get_LCDC_sprite_size_status() const; // 1 means tall (8X16 pixels),0 = 8x8
    //setters



};


#endif //GB_EMU_2024_PPU_H
