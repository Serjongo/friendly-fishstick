//
// Created by Serjo on 11/11/2024.
//

#ifndef GB_EMU_2024_MAIN_H
#define GB_EMU_2024_MAIN_H

#include <iostream>
#include <cmath> //may not need it, used originally for pow
#include <cstring>
#include <fstream>
#include <iomanip>  // For std::setw and std::setfill
#include <chrono>
#include <thread>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "PPU.h"

//
using namespace std;
ofstream outMemoryFile;

//shortcuts for code readability
typedef unsigned char BYTE; //8-bit number
typedef unsigned short WORD; //16-bit number, 2byte
typedef signed short SIGNED_WORD ;
typedef unsigned int DWORD; // 32-bit number






//clases declarations
class gameboy_testing;

//credit to codesligner.co.uk for getting started

#define FLAG_Z 7
#define FLAG_N 6
#define FLAG_H 5
#define FLAG_C 4
#define SB_reg 0xFF01 //Serial transfer data
#define SC_reg 0xFF02 //Serial transfer control
#define IF_reg 0xFF0F //Interrupt Flag
#define IE_reg 0xFFFF //interrupt Enabler
//timers
#define DIV_register 0xFF04 //DIV: Divider register
#define TIMA_register 0xFF05 // TIMA: Timer counter
#define TMA_register 0xFF06 // TMA: Timer modulo
#define TAC_register 0xFF07 // TAC: Timer control

//registers class
union Register
{
    //since this is a union, both the WORD and BYTES share the same memory, that way we can refer to each register both byte-wise and whole
    WORD reg;
    struct
    {
        //the order of declaration is important, for example: HL register: mem[n]=L, mem[n+1]=H
        BYTE lo;
        BYTE hi;
    };

};

//gameboy class
class gameboy
{
    friend class gameboy_testing;

public:
//    //constructors
//    gameboy(int testing_mode);
//
//    gameboy_testing* gb_testing;


    BYTE mem[0x10000]; //2^16 bytes
    BYTE IME = 0; //IME FLAG, interrupts enabled/disabled
    BYTE is_halted = 0; //used for halt commands, will have values of 1 - to be halted, 2 - halted. credit : https://rylev.github.io/DMG-01/public/book/cpu/conclusion.html
    unsigned int gb_machine_cycles = 0; //will count cost of operations, will reset every second
    const unsigned int max_machine_cycles_val =  4194304/4; //max amount of cycles per sec. 1 machine cycle = 4 clock cycles
    const unsigned int div_timer_freq = 16384/4; //in hz, in machine cycles

    //registers
    Register AF_reg; //Accumulator + Flags
    Register BC_reg;
    Register DE_reg;
    Register HL_reg;
    Register StackPointer_reg; //stack pointer
    WORD PC; //program counter

    //timers
    WORD DIV_timer = 0; // only the most significant byte will be used for the actual div_timer, since it increments every 256 cycles
    BYTE TIMA_timer = 0; //this will use the most significant m-bits, as appropriate for the speed, according to TAC val

    //a variable for general usage (as one can't declare vars in switch cases)
    WORD tmp; //unsigned short
    signed short tmp_sWord;
    BYTE tmp_uChar;
    char tmp_sChar;
    WORD operand_1;//note that I'll be using these for both 8bit and 16bit arithmetics, which will also assist in detecting "overflow" in 8 bit
    WORD operand_2;

    BYTE nn_lsb; //least significant byte
    BYTE nn_msb; //most significant byte

    //TESTING RELATED
    BYTE testing_mode = 0; //when turned on, will print testing related info, as well as logging data in text files
    static inline int loop_counter = 1;


    enum r8
    {
        B,
        C,
        D,
        E,
        H,
        L,
        HL_8,//pointer to word
        A
    };

    enum r16
    {
        BC,
        DE,
        HL_16,
        SP,
    };

    enum interrupts
    {
        vblank,
        lcd,
        timer,
        serial_link,
        joypad
    };

    //carry locations in registers
    BYTE half_carry_8bit = 0x10; //5th bit on, 1st bit of highest nibble
    WORD half_carry_16bit = 0x1000; //5th bit of the most significant BYTE, 1st of highest nibble
    BYTE carry_8bit = 0x80; //most significant bit
    WORD carry_16bit = 0x8000; //most significant bit

    //used for convenience for interrupts setters and getters
    int interrupts[5] = {vblank,lcd,timer,serial_link,joypad};
    //check if we are dealing with an interrupt request
    BYTE interrupt_mode = 0;

    WORD interrupt_routine_addresses[5] = {
            0x0040,
            0x0048,
            0x0050,
            0x0058,
            0x0060
    };

    BYTE TAC_speeds[4] = { //timer speed mods for TIMA
            8, //every 256 machine cycles
            2, //every 4 machine cycles
            4, // every 16 machine cycles
            6 // every 64 machine cycles
    };

    //those are the full registers
    Register* r16[4] = {
            &BC_reg,
            &DE_reg,
            &HL_reg,
            &StackPointer_reg};

    //array of pointers to sub-registers. the way it works is by casting the pointers of unsigned shorts (WORDs) into pointers of bytes.
    // In order to point to the second byte of said short, we increment the pointer by 1(and since we're talking in BYTE resolution, this increments us by 8 bits)
    BYTE* r8[8] = {
            &BC_reg.hi, //B
            &BC_reg.lo, //C
            &DE_reg.hi, //D
            &DE_reg.lo, //E
            &HL_reg.hi, //H
            &HL_reg.lo, //L
            &mem[HL_reg.reg], ///[HL], currently problematic, will re-declare this pointer during init once HL_reg gets properly allocated memory
            &AF_reg.hi}; //A

    WORD OPCODE;



    //flags
    //may not work, check bitwise arithemtic
    BYTE z = *(r8[7]+1) & 0x80; //zero flag, 7th bit
    BYTE n = *(r8[7]+1) & 0x40; //subtraction flag
    BYTE h = *(r8[7]+1) & 0x20; //half carry flag
    BYTE c = *(r8[7]+1) & 0x10; //carry flag

    //flags
    BYTE get_Z_flag_status() const; //returns 1 or 0
    BYTE get_N_flag_status() const; //returns 1 or 0
    BYTE get_H_flag_status() const; //returns 1 or 0
    BYTE get_C_flag_status() const; //returns 1 or 0

    void set_Z_flag_status(BYTE status);
    void set_N_flag_status(BYTE status);
    void set_H_flag_status(BYTE status);
    void set_C_flag_status(BYTE status);

    //interrupts
    BYTE get_interrupt_bit_status(WORD IE_IF, int interrupt_type);
    void set_interrupt_bit(int interrupt_type, int mode);

    //TESTING -- this will be broken into a testing class
//    void init_register_file();
//    void init_memory_file();
//    void print_registers_r8();
//    void gbdoctor_init_register_file();
//    void gbdoctor_print_registers_r8();
//    void print_memory_writes(WORD OPCODE,WORD address, BYTE val);

    //private methods
    void init();

    //deep copy function from cartridge to mem ~@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    ///This is a temporary placeholder of a function. In reality, moving the cartridge mem to gb mem is more complicated, for now we just dump it
    void cartridge_to_mem(long long bytes);

    void read_from_file(string path); //basic version, will change as the project develops

    void fetch();

    //move PC to interrupt handler, backup PC in stack
    void PC_to_interrupt(int interrupt_routine_type);

    void check_interrupts();
    void post_interrupt();
    //timer related funcs
    void update_timers(unsigned int machine_cycles_added,int running_mode);//running modes - 0: reguler, 1: pause: 2:stop
    void check_div_reg_change(WORD address);//checks if DIV_register mem was written into, if so, it is reset to 0
    //machine cycles management
    void num_of_machine_cycles(int num);

    void decode_execute();
    void main_loop();

    //public methods
    //getters
    Register get_reg_AF();
    BYTE get_subreg_A();
    BYTE get_subreg_F();
    Register get_reg_BC();
    BYTE get_subreg_B();
    BYTE get_subreg_C();
    Register get_reg_HL();
    BYTE get_subreg_H();
    BYTE get_subreg_L();
    Register get_reg_SP();
    WORD get_PC();



    //setters
    void set_reg_AF(WORD input);
    void set_subreg_A(BYTE input);
    void set_subreg_F(BYTE input);
    void set_reg_BC(WORD input);
    void set_subreg_B(BYTE input);
    void set_subreg_C(BYTE input);
    void set_reg_HL(WORD input);
    void set_subreg_H(BYTE input);
    void set_subreg_L(BYTE input);
    void set_reg_SP(WORD input);
    void set_PC(WORD input);

};






class gameboy_testing{
public:


    void static init_register_file();
    void static init_memory_file();
    void static print_registers_r8(gameboy& gb);
    void static gbdoctor_init_register_file();
    void static gbdoctor_print_registers_r8(gameboy& gb);
    void static print_memory_writes(WORD OPCODE,WORD address, BYTE val);


};


#endif //GB_EMU_2024_MAIN_H
