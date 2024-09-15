#include <iostream>

#include <cmath> //may not need it, used originally for pow


//shortcuts for code readability
using BYTE = unsigned char; //8-bit number
using WORD = unsigned short; //16-bit number, 2byte
using DWORD = unsigned int; // 32-bit number

//declarations - data structures etc
WORD memory[65536]; //2^16 bytes

BYTE AF_reg[2]; //Accumulator + Flags
BYTE BC_reg[2];
BYTE DE_reg[2];
BYTE HL_reg[2];
WORD SP; //stack pointer
WORD PC; //program counter


//for redability
BYTE OPCODE[2];
BYTE *OP_0 = &OPCODE[0];
BYTE *OP_1 = &OPCODE[1];



//sub-registers

BYTE *A = &AF_reg[0]; //pointer to sub-register (first byte)
BYTE *B = &BC_reg[0];
BYTE *C = &BC_reg[1];
BYTE *D = &DE_reg[0];
BYTE *E = &DE_reg[1];
BYTE *H = &HL_reg[0];
BYTE *L = &HL_reg[1];

//flags
//may not work, check bitwise arithemtic
BYTE z = AF_reg[1] & 0x80; //zero flag, 7th bit
BYTE n = AF_reg[1] & 0x40; //subtraction flag
BYTE h = AF_reg[1] & 0x20; //half carry flag
BYTE c = AF_reg[1] & 0x10; //carry flag

int main() {
    std::cout << "Hello, !!!!!!!!" << std::endl;

    while(true)
    {
        //main gameboy loop

        //FETCH
        OPCODE[0] = memory[PC];
        OPCODE[1] = memory[PC+1];
        PC++;

        //DECODE & EXECUTE
        switch(*OP_0)
        {
            case(0x00):
                //NOP OPERATION
                break;



        }

        //nothing
    }

    return 1;
}
