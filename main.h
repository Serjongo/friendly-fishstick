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


using namespace std;
ofstream outMemoryFile;

//shortcuts for code readability
typedef unsigned char BYTE; //8-bit number
typedef unsigned short WORD; //16-bit number, 2byte
typedef signed short SIGNED_WORD ;
typedef unsigned int DWORD; // 32-bit number


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
public:
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

    //TESTING RELATED
    BYTE testing_mode = 0; //when turned on, will print testing related info, as well as logging data in text files

    //flags
    //may not work, check bitwise arithemtic
    BYTE z = *(r8[7]+1) & 0x80; //zero flag, 7th bit
    BYTE n = *(r8[7]+1) & 0x40; //subtraction flag
    BYTE h = *(r8[7]+1) & 0x20; //half carry flag
    BYTE c = *(r8[7]+1) & 0x10; //carry flag

    //flags getters
    BYTE get_Z_flag_status() const; //returns 1 or 0
    BYTE get_N_flag_status() const; //returns 1 or 0
    BYTE get_H_flag_status() const; //returns 1 or 0
    BYTE get_C_flag_status() const; //returns 1 or 0
    //flags setters


};



#endif //GB_EMU_2024_MAIN_H
