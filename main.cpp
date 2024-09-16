#include <iostream>

#include <cmath> //may not need it, used originally for pow


//shortcuts for code readability
using BYTE = unsigned char; //8-bit number
using WORD = unsigned short; //16-bit number, 2byte
using DWORD = unsigned int; // 32-bit number

//declarations - data structures etc
WORD memory[65536]; //2^16 bytes


//registers
//https://www.reddit.com/r/EmuDev/comments/7ljc41/how_to_algorithmically_parse_gameboy_opcodes/ - may change registers structure
WORD AF_reg; //Accumulator + Flags
WORD BC_reg;
WORD DE_reg;
WORD HL_reg;
WORD SP_reg; //stack pointer
WORD PC_reg; //program counter

//a variable for general usage (as one can't declare vars in switch cases)
WORD tmp;

//those are the full registers
WORD* r16[] = {&BC_reg,&DE_reg,&HL_reg,&SP_reg};

//array of pointers to sub-registers. the way it works is by casting the pointers of unsigned shorts (WORDs) into pointers of bytes.
// In order to point to the second byte of said short, we increment the pointer by 1(and since we're talking in BYTE resolution, this increments us by 8 bits)
BYTE* r8[] = {(BYTE*)&BC_reg,(BYTE*)&BC_reg+1, (BYTE*)&DE_reg,(BYTE*)&DE_reg+1,(BYTE*)&HL_reg,(BYTE*)&HL_reg+1,(BYTE*)&memory[HL_reg],(BYTE*)&AF_reg};


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

            case(0x03):case(0x13):case(0x23):case(0x33): //INC r16[reg]
                tmp = (OPCODE & 0x30)>>4;
                r16[tmp]++;
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
            case(0x04):case(0x14):case(0x24):case(0x34): //INC r8[reg]
                tmp = (OPCODE & 0x38)>>3;
                r8[tmp]++;
                break;

            case(0x05):case(0x15):case(0x25):case(0x35): //DEC r8[reg]
                tmp = (OPCODE & 0x38)>>3;
                r8[tmp]--;
                break;

            default:
                break;




        }

        //nothing
    }

    return 1;
}
