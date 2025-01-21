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
#define tilemap_row_length_bytes 32 //window/background, 32 rows of 32 bytes
#define tilemap_size 0x3ff
#define tile_size_bytes 16
#define BG_palette_data_reg 0xFF47
#define pixel_row_size 8
#define OAM_mem_start 0xFE00
#define OAM_mem_end 0xFE9F
#define VRAM_mem_start 0x8000 //mem loc
#define VRAM_mem_end 0x97ff //mem loc bound

#include <queue>
//#include "main.h"


/// -----TEMPORARY
//shortcuts for code readability
typedef unsigned char BYTE; //8-bit number
typedef unsigned short WORD; //16-bit number, 2byte
typedef signed short SIGNED_WORD ;
typedef unsigned int DWORD; // 32-bit number
#include <iostream>
#include <cmath> //may not need it, used originally for pow
#include <cstring>
#include <fstream>
#include <iomanip>  // For std::setw and std::setfill
#include <chrono>
#include <thread>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

/// -----TEMPORARY END


class gameboy;

class Color
{
    public:
        Color(BYTE red, BYTE green, BYTE blue) : red(red),green(green),blue(blue){};
        Color():red(0),green(0),blue(0){}
    //getters
    BYTE get_red();
    BYTE get_green();
    BYTE get_blue();

    //setters
    void set_red(BYTE red);
    void set_green(BYTE green);
    void set_blue(BYTE blue);

    private:
        BYTE red;
        BYTE green;
        BYTE blue;


};

class Pixel
        {
        private:
            //bits:
            // 0-1 - color
            // 2 - palette
            // 3 - background priority - val0 for background
            BYTE data;

        public:
            Pixel(BYTE color, BYTE palette, BYTE background_priority);
            //getters
            BYTE get_color();
            BYTE get_palette();
            BYTE get_background_priority();
            //setters
            void set_color(BYTE color);
            void set_palette(BYTE palette);
            void set_background_priority(BYTE background_priority);

        };

class Sprite
        {
        private:
            BYTE y_pos;
            BYTE x_pos;
            BYTE tile_num;
            BYTE flags;
        public:
            Sprite(BYTE y_pos,BYTE x_pos, BYTE tile_num, BYTE flags)
            {
                this->y_pos = y_pos;
                this->x_pos = x_pos;
                this->tile_num = tile_num;
                this->flags = flags;
            }

            //getters
            BYTE get_y_pos(){
                return y_pos;
            };
            BYTE get_x_pos()
            {
                return x_pos;
            };
            BYTE get_tile_num()
            {
                return tile_num;
            };
            BYTE get_flags()
            {
                return flags;
            };

            BYTE get_palette_number_flag()
            {
                return ((flags & 0x10) >> 4);
            }
            BYTE get_x_flip_flag()
            {
                return ((flags & 0x20) >> 5);
            }
            BYTE get_y_flip_flag()
            {
                return ((flags & 0x40) >> 6);
            }
            BYTE get_obj_to_bg_priority_flag()
            {
                return ((flags & 0x80) >> 7);
            }



            //setters
            void set_y_pos(BYTE y_pos)
            {
                this->y_pos = y_pos;
            }
            void set_x_pos(BYTE x_pos)
            {
                this->x_pos = x_pos;
            }
            void set_tile_num(BYTE tile_num)
            {
                this->tile_num = tile_num;
            }
            void set_flags(BYTE flags)
            {
                this->flags = flags;
            }

            void set_palette_number_flag(BYTE palette_number_flag)
            {
                flags = flags & 0xEF;
                flags = flags | (palette_number_flag << 4);
            }
            void set_x_flip_flag(BYTE x_flip_flag)
            {
                flags = flags & 0xDF;
                flags = flags | (x_flip_flag << 5);
            }
            void set_y_flip_flag(BYTE y_flip_flag)
            {
                flags = flags & 0xBF;
                flags = flags | (y_flip_flag << 6);
            }
            void set_obj_to_bg_priority_flag(BYTE obj_to_bg_priority_flag)
            {
                flags = flags & 0x7F;
                flags = flags | (obj_to_bg_priority_flag << 7);
            }

        };


class PPU{
    private:
        gameboy& parent;

    public:
        //for debug purposes
        BYTE Screen[144][160]; //144 arrays of 160 each


        BYTE* VRAM; // from the vram start point
        std::vector<Sprite*> OAM; //all sprites from the MEM as objects, will hold up to 40 sprites, 160 bytes of data in total
        BYTE* MEM; //1 to 1 memory mapping from the start
        std::queue<Pixel> Background_FIFO;
        std::queue<Pixel> Sprite_FIFO;

        std::vector<Sprite*> visible_OAM_buffer; //up to 10 pointers to OAMs/sprites which are potentially visible in the line
        Color background_palette[4];

        //pixel fetcher vars
        WORD pixel_fetcher_x_position_counter = 0;
        WORD WINDOW_LINE_COUNTER = 0; // credit to: https://hacktix.github.io/GBEDG/ppu/
        BYTE first_window_encounter = 1; //
        void pixel_fetcher();
        //pixel fetcher related funcs
        WORD tileData_to_pixel_row(BYTE tile_data_low,BYTE tile_data_high);

        void H_BLANK();
        void V_BLANK();

        void SFML_draw_screen(int row);

        //

    PPU(BYTE* OAM_start,BYTE* VRAM_start,BYTE* MEM_start,gameboy& gameboy); //
    //PPU(gameboy& gameboy) : parent(gameboy){}

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
    void clean_OAM_buff();
    void DRAW();

    void PPU_cycle();



    //getters
    BYTE get_LCDC_sprite_size_status() const; // 1 means tall (8X16 pixels),0 = 8x8
    BYTE get_LCDC_window_display_enable_status() const;
    BYTE get_LCDC_bg_tile_map_select_status() const;
    BYTE get_LCDC_window_tile_map_select() const;
    BYTE get_LCDC_tile_data_select() const;
    //setters


    //SFML functions and fields (LCD)
    std::vector<sf::RectangleShape> pixels;

};


#endif //GB_EMU_2024_PPU_H
