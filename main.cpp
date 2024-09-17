#include <iostream>
#include <cmath> //may not need it, used originally for pow


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


//constants
BYTE m_CartridgeMemory[0x200000];

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



class gameboy
        {
            //constructor


            //parameters


            //declarations - data structures etc
                WORD memory[65536]; //2^16 bytes


            //registers
            //https://www.reddit.com/r/EmuDev/comments/7ljc41/how_to_algorithmically_parse_gameboy_opcodes/ - may change registers structure
            Register AF_reg; //Accumulator + Flags
            Register BC_reg;
            Register DE_reg;
            Register HL_reg;
            Register StackPointer_reg; //stack pointer
            WORD PC; //program counter

            //a variable for general usage (as one can't declare vars in switch cases)
            WORD tmp;

        //those are the full registers
            Register* r16[4] = {&BC_reg,&DE_reg,&HL_reg,&StackPointer_reg};

        //array of pointers to sub-registers. the way it works is by casting the pointers of unsigned shorts (WORDs) into pointers of bytes.
        // In order to point to the second byte of said short, we increment the pointer by 1(and since we're talking in BYTE resolution, this increments us by 8 bits)
            Register* r8[8] = {BC_reg.hi,&BC_reg.lo, &DE_reg.hi,&DE_reg.lo,&HL_reg.hi,&HL_reg.lo,(BYTE*)&memory[HL_reg],&AF_reg.hi};


            //for redability
            WORD OPCODE;
            //BYTE *OP_0 = &OPCODE;
            //BYTE *OP_1 = &OPCODE;

            //sub-registers
            //
            //BYTE *A = &AF_reg[0]; //pointer to sub-register (first byte)
            //BYTE *B = &BC_reg[0];
            //BYTE *C = &BC_reg[1];
            //BYTE *D = &DE_reg[0];
            //BYTE *E = &DE_reg[1];
            //BYTE *H = &HL_reg[0];
            //BYTE *L = &HL_reg[1];

            //flags
            //may not work, check bitwise arithemtic
            BYTE z = *(r8[7]+1) & 0x80; //zero flag, 7th bit
            BYTE n = *(r8[7]+1) & 0x40; //subtraction flag
            BYTE h = *(r8[7]+1) & 0x20; //half carry flag
            BYTE c = *(r8[7]+1) & 0x10; //carry flag

            //private methods

            void init()
            {
                PC=0x100 ;
                AF_reg=0x01B0;
                BC_reg = 0x0013;
                DE_reg = 0x00D8;
                HL_reg = 0x014D;
                StackPointer_reg=0xFFFE;
                m_Rom[0xFF05] = 0x00 ;
                m_Rom[0xFF06] = 0x00 ;
                m_Rom[0xFF07] = 0x00 ;
                m_Rom[0xFF10] = 0x80 ;
                m_Rom[0xFF11] = 0xBF ;
                m_Rom[0xFF12] = 0xF3 ;
                m_Rom[0xFF14] = 0xBF ;
                m_Rom[0xFF16] = 0x3F ;
                m_Rom[0xFF17] = 0x00 ;
                m_Rom[0xFF19] = 0xBF ;
                m_Rom[0xFF1A] = 0x7F ;
                m_Rom[0xFF1B] = 0xFF ;
                m_Rom[0xFF1C] = 0x9F ;
                m_Rom[0xFF1E] = 0xBF ;
                m_Rom[0xFF20] = 0xFF ;
                m_Rom[0xFF21] = 0x00 ;
                m_Rom[0xFF22] = 0x00 ;
                m_Rom[0xFF23] = 0xBF ;
                m_Rom[0xFF24] = 0x77 ;
                m_Rom[0xFF25] = 0xF3 ;
                m_Rom[0xFF26] = 0xF1 ;
                m_Rom[0xFF40] = 0x91 ;
                m_Rom[0xFF42] = 0x00 ;
                m_Rom[0xFF43] = 0x00 ;
                m_Rom[0xFF45] = 0x00 ;
                m_Rom[0xFF47] = 0xFC ;
                m_Rom[0xFF48] = 0xFF ;
                m_Rom[0xFF49] = 0xFF ;
                m_Rom[0xFF4A] = 0x00 ;
                m_Rom[0xFF4B] = 0x00 ;
                m_Rom[0xFFFF] = 0x00 ;
            }

            //public methods
            //getters
            Register get_reg_

            //setters


        };




int main() {
    std::cout << "Hello, !!!!!!!!" << std::endl;

    while(true)
    {
        //main gameboy loop

        //FETCH
        OPCODE = memory[PC_reg];
        PC_reg++;

        //DECODE & EXECUTE

        //FIRST BYTE - OPCODE SWITCH CASE
        switch(OPCODE) //MAY NEED TO FIX THE OPCODE SINCE ITS 2 BYTES, BUT MAYBE ITS OK SINCE IT SHOULD BE 0 anyway
        {
            case(0x00): //NOP OPERATION
                break;

            case(0x01):case(0x11):case(0x21):case(0x31): //LD BC, d16
                //r16[4th&5th_bits] = memory[PC] which is 2 bytes
                //increment PC twice
                tmp = (OPCODE & 0x30)>>4;
                *r16[tmp] = memory[PC_reg];
                PC_reg++;
                PC_reg++;
                break;


            case(0x02): case(0x12): //LD (BC) OR (DE), A
                tmp = (OPCODE & 0x30)>>4;
                memory[*r16[tmp]] = *r8[7];
                break;

            ///TO CHECK FIRST THING - IS A "TMP" still relevant in this opcode?! I DONT THINK SO, DELETING TMP FOR NOW
            case(0x22): case(0x32): //LD (HL), A
                //tmp = (OPCODE & 0x30)>>4;
                memory[*r16[2]] = *r8[7]; //IS IT POINTER OR NUMBER, RE-CHECK!
                r16[2]++; //increment the CONTENTS of HL, which is a pointer to

            case(0x03):case(0x13):case(0x23):case(0x33): //INC r16[reg]
                tmp = (OPCODE & 0x30)>>4; //relevant opcode bits in r16 are 4th & 5th
                r16[tmp]++;
                break;


            case(0x04):case(0x14):case(0x24):case(0x34): //INC r8[reg]
                tmp = (OPCODE & 0x38)>>3; //relevant opcode bits in r8 are 3rd, 4th & 5th
                (WORD*)r8[tmp]++; ///temporary solution of casting the byte as a word, so it increments in the right place
                break;

            case(0x05):case(0x15):case(0x25):case(0x35): //DEC r8[reg]
                tmp = (OPCODE & 0x38)>>3;
                (WORD*)r8[tmp]--; ///temporary solution of casting the byte as a word, so it increments in the right place
                break;

            default:
                break;




        }

        //nothing
    }

    return 1;
}


//tests
