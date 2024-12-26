//
// Created by Serjo on 12/23/2024.
//

#ifndef GB_EMU_2024_PPU_H
#define GB_EMU_2024_PPU_H


#define LY_register 0xFF44
#define SCX 0xFF43 //mem loc of x-screen coordinate for rendering
#define SCY 0xFF42 //mem loc of y-scrren coordinate for rendering
#define WX_reg 0xFF4A //mem loc of leftmost x_coord border of the window (continues till end of screen)
#define WY_reg 0xFF4B //mem loc of top y_coord border of the window
#define oam_size 0xA0 // 160 bytes
#define VRAM_mem_start 0x8000 //mem loc
#define VRAM_mem_end 0x97ff //mem loc bound

#include "main.h"

class PPU{
public:
    BYTE* VRAM; // from the vram start point
    BYTE* OAM; //from the OAM start point
    BYTE* MEM; //1 to 1 memory mapping from the start

    BYTE* visible_OAM_buffer[0x0A]; //pointers to 10 OAMs/sprites
    void pixel_fetcher(int lcd_x_coord);

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

    void OAM_SCAN();
    void clean_visible_OAM_buff();
    void DRAW();

    //getters
    BYTE get_LCDC_sprite_size_status() const; // 1 means tall (8X16 pixels),0 = 8x8
    BYTE get_LCDC_window_display_enable_status() const;
    BYTE get_LCDC_bg_tile_map_select_status() const;
    BYTE get_LCDC_window_tile_map_select() const;
    BYTE get_LCDC_tile_data_select() const;
    //setters



};


#endif //GB_EMU_2024_PPU_H
