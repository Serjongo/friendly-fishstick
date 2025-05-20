//
// Created by Serjo on 12/23/2024.
//

#ifndef GB_EMU_2024_PPU_H
#define GB_EMU_2024_PPU_H


#define LY_register 0xFF44
#define LYC_register 0xFF45
#define LCD_Control_reg 0xFF40
#define LCD_Status_reg 0xFF41
#define SCX 0xFF43 //mem loc of x-screen coordinate for rendering
#define SCY 0xFF42 //mem loc of y-scrren coordinate for rendering
#define WX_reg 0xFF4B //mem loc of leftmost x_coord border of the window (continues till end of screen)
#define WY_reg 0xFF4A //mem loc of top y_coord border of the window
#define oam_size 0xA0 // 160 bytes
#define tilemap_row_length_bytes 32 //window/background, 32 rows of 32 bytes
#define tilemap_size 0x3ff
#define tile_size_bytes 16
#define BG_palette_data_reg 0xFF47
#define SPRITE_0_palette 0xFF48
#define SPRITE_1_palette 0xFF49
#define pixel_row_size 8
#define OAM_mem_start 0xFE00
#define OAM_mem_end 0xFE9F
#define VRAM_mem_start 0x8000 //mem loc
#define VRAM_mem_end 0x9FFF //mem loc bound
#define tile_map_1_start 0x9800
#define tile_map_1_end 0x9FFF


//modes
#define H_BLANK_MODE 0
#define V_BLANK_MODE 1
#define OAM_SCAN_MODE 2
#define DRAW_MODE 3


#include <queue>
#include <stack>
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
            // 4 - type, "0" for background/window, "1" for sprite
            BYTE data;

        public:
            Pixel(BYTE color, BYTE palette, BYTE background_priority,BYTE type);
            Pixel(); //default constructor
            //getters
            BYTE get_color();
            BYTE get_palette();
            BYTE get_background_priority();
            BYTE get_type();
            //setters
            void set_color(BYTE color);
            void set_palette(BYTE palette);
            void set_background_priority(BYTE background_priority);
            void set_type(BYTE type);

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
            BYTE const get_y_pos() const
            {
                return y_pos;
            };
            BYTE get_x_pos() const
            {
                return x_pos;
            };
            BYTE get_tile_num() const
            {
                return tile_num;
            };
            BYTE get_flags() const
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
        //for ppu machine_clock counting
        float ppu_machine_cycles = 0; //this will count machine cycles with each action, attempting to be synced in with gb's clock
        float CUR_TICK_ppu_machine_cycles = 0; //will be nullified every tick

        //for debug purposes
        Pixel Screen[144][160]; //144 arrays of 160 each

        BYTE mode = 2; //mode/state, this should change when PPU is changing mode of operation
        BYTE mode_DRAW = 0; //if we're inside draw mode, we'll be checking the specific stage
        BYTE mode_H_BLANK = 0; //if we're inside H_blank, we'll be counting overall clocks with this
        WORD mode_V_BLANK = 0; //if we're inside V_blank, we'll be counting overall clocks with this

        //debug
        std::stack<int> modes_trace;

        int OAM_counter = OAM_mem_start;
        int vblank_counter = 0;

        BYTE* VRAM; // from the vram start point
        std::vector<Sprite> OAM; //all sprites from the MEM as objects, will hold up to 40 sprites, 160 bytes of data in total
        BYTE* MEM; //1 to 1 memory mapping from the start
        std::queue<Pixel> Background_FIFO;
        std::queue<Pixel> Sprite_FIFO;
        bool first_iteration_in_line; //used for DRAW mode quirk

        std::vector<Sprite> visible_OAM_buffer; //up to 10 pointers to OAMs/sprites which are potentially visible in the line
        Color background_palette[4];
        Color sprite_palette_0[4];
        Color sprite_palette_1[4];

        //pixel fetcher vars
        WORD pixel_fetcher_x_position_counter = 0;
        int screen_coordinate_x = 0; //used to count pixels on screen as we pop them from the fifo
        WORD WINDOW_LINE_COUNTER = 0; // credit to: https://hacktix.github.io/GBEDG/ppu/
        BYTE first_window_encounter = 1; //
        void pixel_fetcher();
        //pixel fetcher related funcs
        WORD tileData_to_pixel_row(BYTE tile_data_low,BYTE tile_data_high);


        //placeholder
        WORD tile_data_base_loc;
        WORD tile_address_background; //after calculating tile number and base_loc, final address
        BYTE tile_data_low_background; //first byte of pixels
        BYTE tile_data_high_background; //second byte of pixels

        WORD tile_address_sprite;
        BYTE tile_data_low_sprite; //first byte of sprite pixels
        BYTE tile_data_high_sprite; //second byte of sprite pixels
        bool waiting_for_visible_sprite_fetch = false; //used to pause background fetcher while we pull sprite data to FIFO
        int tile_dat_pixel_index = 0;
        WORD tilemap_mem_loc = 0x9800; //window location
        WORD tile_x; //where we are on the line
        WORD tile_y; //what line we're on
        BYTE tilenum;
        BYTE draw_step = 0;
        float draw_step_ticks_counter = 0;


        void H_BLANK();
        void V_BLANK();

        //interrupt related
        bool STAT_interrupt_line = false;

        bool sample_STAT_interrupt_line(); //if any of the conditions are now met, we will trip the interrupt flag in the memory
        void set_vblank_interrupt(); //this will be called every single time the V_BLANK mode is activated, requesting for an interrupt from the CPU

        //machine clock related
        bool tick_over = false; //this will signal that we've ticked and should finish our current work
        void num_of_machine_cycles(float num);


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

    //get color of pixels

    BYTE get_pixel_color_from_mem(BYTE color,bool background,bool sprite_1 = false); //the pixel's color is actually an index for the mem[color_palette], meaning color 1 is bit 2-3 in mem[palette] which would give us the actual color from 0-3.


    void PPU_cycle();


    // DRAW()'s inner functions

    void Fetch_BG_tile_num_and_address();
    void Fetch_Background_Tile_Data_low();
    void Fetch_Background_Tile_Data_high();

    void Fetch_SPRITE_tile_address(BYTE sprite_tile_num);
    void Fetch_Sprite_Tile_Data_low();
    void Fetch_Sprite_Tile_Data_high();
    void Push_to_BG_FIFO();
    void Push_to_SPRITE_FIFO();
    Pixel fill_transparent_sprite_pixel(BYTE fifo_index,BYTE sprite_index,Pixel default_pixel,Sprite original_sprite);

    void Pop_to_screen(); // pop from eligible fifos to screen


    //getters
    BYTE get_LCDC_bg_window_enable_status() const;
    BYTE get_LCDC_sprite_enable_status() const;
    BYTE get_LCDC_sprite_size_status() const; // 1 means tall (8X16 pixels),0 = 8x8
    BYTE get_LCDC_bg_tile_map_select_status() const;
    BYTE get_LCDC_tile_data_select() const;
    BYTE get_LCDC_display_enable_status() const;
    BYTE get_LCDC_window_tile_map_select() const;
    BYTE get_LCDC_window_display_enable_status() const;




    //LCDS getters
    BYTE get_LCDS_lycly_interrupt_enable_status() const;
    BYTE get_LCDS_oam_scan_mode_interrupt_enable_status() const; //mode 2
    BYTE get_LCDS_vblank_mode_interrupt_enable_status() const; //mode 1
    BYTE get_LCDS_hblank_mode_interrupt_enable_status() const; //mode 0
    BYTE get_LCDS_coincidence_flag_status() const; //if lyc==ly, changed by ppu
    BYTE get_LCDS_PPU_MODE_status() const;

    //LCDS setters
    void set_LCDS_lycly_interrupt_enable_status(BYTE status);
    void set_LCDS_oam_scan_mode_interrupt_enable_status(BYTE status); //mode 2
    void set_LCDS_vblank_mode_interrupt_enable_status(BYTE status); //mode 1
    void set_LCDS_hblank_mode_interrupt_enable_status(BYTE status); //mode 0
    void set_LCDS_coincidence_flag_status(BYTE status); //mode 0
    void set_LCDS_PPU_MODE_status(BYTE status);


};


#endif //GB_EMU_2024_PPU_H
