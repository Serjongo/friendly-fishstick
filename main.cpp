#include <iostream>
#include <cmath> //may not need it, used originally for pow
#include <cstring>
#include <fstream>


using namespace std;

//HARTA

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
//gitcommit_harta
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
        public:
            //constructor
            //gameboy();

            //parameters


            //declarations - data structures etc
                WORD mem[0x10000]; //2^16 bytes


            //registers
            Register AF_reg; //Accumulator + Flags
            Register BC_reg;
            Register DE_reg;
            Register HL_reg;
            Register StackPointer_reg; //stack pointer
            WORD PC; //program counter

            //a variable for general usage (as one can't declare vars in switch cases)
            WORD tmp;

        //those are the full registers
            Register* r16[4] = {
                    &BC_reg,
                    &DE_reg,
                    &HL_reg,
                    &StackPointer_reg};

        //array of pointers to sub-registers. the way it works is by casting the pointers of unsigned shorts (WORDs) into pointers of bytes.
        // In order to point to the second byte of said short, we increment the pointer by 1(and since we're talking in BYTE resolution, this increments us by 8 bits)
            BYTE* r8[8] = {
                    &BC_reg.hi,
                    &BC_reg.lo,
                    &DE_reg.hi,
                    &DE_reg.lo,
                    &HL_reg.hi,
                    &HL_reg.lo,
                    (BYTE*)&mem[HL_reg.lo],
                    &AF_reg.hi}; //index 6 is PROBLEMATIC,
            // (IT POINTS TO BYTE L, WE NEED TO CAST TO WORD* EVERY TIME WE POINT TO IT)


            //for redability
            WORD OPCODE;
            //BYTE *OP_0 = &OPCODE;
            //BYTE *OP_1 = &OPCODE;


            //flags
            //may not work, check bitwise arithemtic
            BYTE z = *(r8[7]+1) & 0x80; //zero flag, 7th bit
            BYTE n = *(r8[7]+1) & 0x40; //subtraction flag
            BYTE h = *(r8[7]+1) & 0x20; //half carry flag
            BYTE c = *(r8[7]+1) & 0x10; //carry flag

            //private methods

            void init()
            {
                //fill memory with zeroes
                memset(m_CartridgeMemory,0,sizeof(m_CartridgeMemory));


                PC = 0x100 ;
                AF_reg.reg = (WORD)0x01B0;
                BC_reg.reg = 0x0013;
                DE_reg.reg = 0x00D8;
                HL_reg.reg = 0x014D;
                StackPointer_reg.reg = 0xFFFE;
                mem[0xFF05] = 0x00 ;
                mem[0xFF06] = 0x00 ;
                mem[0xFF07] = 0x00 ;
                mem[0xFF10] = 0x80 ;
                mem[0xFF11] = 0xBF ;
                mem[0xFF12] = 0xF3 ;
                mem[0xFF14] = 0xBF ;
                mem[0xFF16] = 0x3F ;
                mem[0xFF17] = 0x00 ;
                mem[0xFF19] = 0xBF ;
                mem[0xFF1A] = 0x7F ;
                mem[0xFF1B] = 0xFF ;
                mem[0xFF1C] = 0x9F ;
                mem[0xFF1E] = 0xBF ;
                mem[0xFF20] = 0xFF ;
                mem[0xFF21] = 0x00 ;
                mem[0xFF22] = 0x00 ;
                mem[0xFF23] = 0xBF ;
                mem[0xFF24] = 0x77 ;
                mem[0xFF25] = 0xF3 ;
                mem[0xFF26] = 0xF1 ;
                mem[0xFF40] = 0x91 ;
                mem[0xFF42] = 0x00 ;
                mem[0xFF43] = 0x00 ;
                mem[0xFF45] = 0x00 ;
                mem[0xFF47] = 0xFC ;
                mem[0xFF48] = 0xFF ;
                mem[0xFF49] = 0xFF ;
                mem[0xFF4A] = 0x00 ;
                mem[0xFF4B] = 0x00 ;
                mem[0xFFFF] = 0x00 ;
            }

            //what this currently does is simply read from file, and drop into mem from cell 0x100 and onwards.
            // This is likely temporary and currently done for testing purposes. the reading from cartridge mechanism is more complicated, and we're not there yet
            void read_from_file(string path) //basic version, will change as the project develops
            {
                ifstream input_file(path);
                if(!input_file)
                {
                    cerr << "File error.\n";
                }
                input_file.read((char *)m_CartridgeMemory + 0x100, sizeof(mem) - 1); ///this char cast may cause problems in the long run, may change.
                m_CartridgeMemory[input_file.gcount()] = '\0';
            }

            void fetch()
            {
                OPCODE = mem[PC];
                PC++;
            }
            void decode_execute()
            {
                switch(OPCODE) //MAY NEED TO FIX THE OPCODE SINCE ITS 2 BYTES, BUT MAYBE ITS OK SINCE IT SHOULD BE 0 anyway
                {
                    case(0x00): //NOP OPERATION
                        break;

                    case(0x01):case(0x11):case(0x21):case(0x31): //LD BC, d16
                        //r16[4th&5th_bits] = memory[PC] which is 2 bytes
                        //increment PC twice
                        tmp = (OPCODE & 0x30)>>4;
                        r16[tmp]->reg = mem[PC];
                        PC++;
                        PC++;
                        break;


                    case(0x02): case(0x12): //LD (BC) OR (DE), A
                        tmp = (OPCODE & 0x30)>>4;
                        mem[r16[tmp]->lo] = *r8[7];
                        break;

                        ///TO CHECK FIRST THING - IS A "TMP" still relevant in this opcode?! I DONT THINK SO, DELETING TMP FOR NOW
                    case(0x22): case(0x32): //LD (HL), A
                        //tmp = (OPCODE & 0x30)>>4;
                        mem[r16[2]->reg] = *r8[7];
                        r16[2]->reg++; //increment the CONTENTS of HL --- MAY BE PROBLEMATIC DOWN THE LINE

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
            }

            void main_loop()
            {
                read_from_file("../test_commands.txt");
                cout << m_CartridgeMemory[0x100];
                while(true)
                {
                    fetch();
                    decode_execute();
                }

            }

            //public methods
            //getters
            Register get_reg_AF()
            {
                return AF_reg;
            }
            BYTE get_subreg_A()
            {
                return AF_reg.hi;
            }
            BYTE get_subreg_F()
            {
                return AF_reg.lo;
            }
            Register get_reg_BC()
            {
                return BC_reg;
            }
            BYTE get_subreg_B()
            {
                return BC_reg.hi;
            }
            BYTE get_subreg_C()
            {
                return BC_reg.lo;
            }
            Register get_reg_HL()
            {
                return HL_reg;
            }
            BYTE get_subreg_H()
            {
                return HL_reg.hi;
            }
            BYTE get_subreg_L()
            {
                return HL_reg.lo;
            }
            Register get_reg_SP()
            {
                return StackPointer_reg;
            }
            WORD get_PC()
            {
                return PC;
            }

            //setters
            void set_reg_AF(WORD input)
            {
                AF_reg.reg = input;
            }
            void set_subreg_A(BYTE input)
            {
                AF_reg.hi = input;
            }
            void set_subreg_F(BYTE input)
            {
                AF_reg.lo = input;
            }
            void set_reg_BC(WORD input)
            {
                BC_reg.reg = input;
            }
            void set_subreg_B(BYTE input)
            {
                BC_reg.hi = input;
            }
            void set_subreg_C(BYTE input)
            {
                BC_reg.lo = input;
            }
            void set_reg_HL(WORD input)
            {
                HL_reg.reg = input;
            }
            void set_subreg_H(BYTE input)
            {
                HL_reg.hi = input;
            }
            void set_subreg_L(BYTE input)
            {
                HL_reg.lo = input;
            }
            void set_reg_SP(WORD input)
            {
                StackPointer_reg.reg = input;
            }
            void set_PC(WORD input)
            {
                PC = input;
            }


        };




int main() {
    std::cout << "Hello, !!!!!!!!" << std::endl;
    gameboy jibby;
    jibby.main_loop();

    return 1;
}


//tests
