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
            BYTE nn_lsb; //least significant byte
            BYTE nn_msb; //most significant byte

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
                    (BYTE*)&mem[HL_reg.lo], //[HL] //index 6 is PROBLEMATIC, // (IT POINTS TO BYTE L, WE NEED TO CAST TO WORD* EVERY TIME WE POINT TO IT)
                    &AF_reg.hi}; //A



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


            //deep copy function from cartridge to mem ~@@@@@@@@@@@@@@@@@@@@@@@@@@@@
            ///This is a temporary placeholder of a function. In reality, moving the cartridge mem to gb mem is more complicated, for now we just dump it
            void cartridge_to_mem(long long bytes)
            {
                for(long long i = 0 ; i < bytes ; i++)
                {
                    mem[i] = m_CartridgeMemory[0x100+i];
                }
            }

            //what this currently does is simply read from file, and drop into mem from cell 0x100 and onwards.
            // This is likely temporary and currently done for testing purposes. the reading from cartridge mechanism is more complicated, and we're not there yet
            void read_from_file(string path) //basic version, will change as the project develops
            {
                ifstream input_file(path,ios::binary);
                if(!input_file)
                {
                    cerr << "File error.\n";
                }
                input_file.read((char *)m_CartridgeMemory + 0x100, sizeof(mem) - 1); ///this char cast may cause problems in the long run, may change.
                m_CartridgeMemory[input_file.gcount()] = '\0';
                cartridge_to_mem(input_file.gcount());

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

                    //tested
                    case(0x01):case(0x11):case(0x21):case(0x31): //LD BC, d16
                        //r16[4th&5th_bits] = memory[PC] which is 2 bytes
                        //increment PC twice
                        tmp = (OPCODE & 0x30)>>4;
                        r16[tmp]->lo = mem[PC];
                        PC = PC + 1;
                        r16[tmp]->hi = (BYTE)mem[PC];

                        PC = PC + 1;
                        break;

                    //tested
                    case(0x02): case(0x12): //LD (BC) OR (DE), A
                        tmp = (OPCODE & 0x30)>>4;
                        mem[r16[tmp]->lo] = *r8[7];
                        break;

                    ///TO CHECK FIRST THING - IS A "TMP" still relevant in this opcode?! I DONT THINK SO, DELETING TMP FOR NOW
                    //tested
                    case(0x22): //LD (HL), A
                        //tmp = (OPCODE & 0x30)>>4;
                        mem[r16[2]->reg] = *r8[7];
                        r16[2]->reg++; //increment the CONTENTS of HL --- MAY BE PROBLEMATIC DOWN THE LINE
                        break;

                    //tested
                    case(0x32):
                        mem[r16[2]->reg] = *r8[7];
                        r16[2]->reg--; //increment the CONTENTS of HL --- MAY BE PROBLEMATIC DOWN THE LINE
                        break;


                    //tested
                    case(0x03):case(0x13):case(0x23):case(0x33): //INC r16[reg]
                        tmp = (OPCODE & 0x30)>>4; //relevant opcode bits in r16 are 4th & 5th
                        r16[tmp]->reg++;
                        break;

                    //tested
                    case(0x04):case(0x14):case(0x24):case(0x34): //INC r8[reg]
                        tmp = (OPCODE & 0x38)>>3; //relevant opcode bits in r8 are 3rd, 4th & 5th
                        (*r8[tmp])++; ///may cause error when incrementing B

                        //flags
                        if ((*r8[tmp]) == 0)
                        {
                            AF_reg.lo = (AF_reg.lo | (BYTE)(1 << FLAG_Z)); //should turn on FLAG_ZERO
                        }
                        else
                        {
                            AF_reg.lo = (AF_reg.lo & (BYTE)(~(1 << FLAG_Z))); //should turn off FLAG_ZERO
                        }
                        AF_reg.lo = (AF_reg.lo & (BYTE)(~(1 << FLAG_N))); //should turn off FLAG_N

                        //FLAG H DEAL WITH IT

                        break;

                    //tested
                    case(0x05):case(0x15):case(0x25):case(0x35): //DEC r8[reg]
                        tmp = (OPCODE & 0x38)>>3;
                        (*r8[tmp])--; ///may cause error when incrementing B

                        //flags
                        if ((*r8[tmp]) == 0)
                        {
                            AF_reg.lo = (AF_reg.lo | (BYTE)(1 << FLAG_Z)); //should turn on FLAG_ZERO
                        }
                        else
                        {
                            AF_reg.lo = (AF_reg.lo & (BYTE)(~(1 << FLAG_Z))); //should turn off FLAG_ZERO
                        }
                        AF_reg.lo = (AF_reg.lo | (BYTE)((1 << FLAG_N))); //should turn on FLAG_N

                        //FLAG H DEAL WITH IT
                        break;

                    //to-test
                    case(0x06): case(0x16): case(0x26): case(0x36): //LD SUBREG, d8
                        tmp = (OPCODE & 0x38)>>3;
                        (*r8[tmp]) = mem[PC];
                        PC++;
                        break;

                    ///LD INSTRUCTIONS WITH REGISTERS - MAY UNIFY THEM ALL INTO ONE COMMAND SOON


                    //to-test
                    case(0x40): case(0x41): case(0x42): case(0x43): case(0x44): case(0x45): case(0x46): case(0x47): //LD B,*SUBREG*
                    case(0x50): case(0x51): case(0x52): case(0x53): case(0x54): case(0x55): case(0x56): case(0x57): //LD D,*SUBREG*
                    case(0x60): case(0x61): case(0x62): case(0x63): case(0x64): case(0x65): case(0x66): case(0x67): //LD H,*SUBREG*
                    case(0x70): case(0x71): case(0x72): case(0x73): case(0x74): case(0x75): case(0x77): //LD H,*SUBREG*
                    case(0x48): case(0x49): case(0x4A): case(0x4B): case(0x4C): case(0x4D): case(0x4E): case(0x4F): //LD C,*SUBREG*
                    case(0x58): case(0x59): case(0x5A): case(0x5B): case(0x5C): case(0x5D): case(0x5E): case(0x5F): //LD C,*SUBREG*
                    case(0x68): case(0x69): case(0x6A): case(0x6B): case(0x6C): case(0x6D): case(0x6E): case(0x6F): //LD C,*SUBREG*
                    case(0x78): case(0x79): case(0x7A): case(0x7B): case(0x7C): case(0x7D): case(0x7E): case(0x7F): //LD C,*SUBREG*

                        (*r8[(OPCODE & 0x38)>>3]) = (*r8[(OPCODE & 0x07)]); //dst: relevant opcode bits in r8 are 3rd, 4th & 5th, src: rel bits 0,1,2
                        break;

                    ///

                    //to-test
                    case(0x0A): case(0x1A):  //LD A, (REG)
                        tmp = (OPCODE & 0x38)>>3;
                        (*r8[7]) = mem[r16[tmp]->reg];
                        break;
                    //to-test
                    case(0x2A): //LD A, (HL+)
                        (*r8[7]) = mem[r16[2]->reg]; //HL
                        (r16[2]->reg)++;
                        break;
                    //to-test
                    case(0x3A): //LD A, (HL-)
                        (*r8[7]) = mem[r16[2]->reg]; //HL
                        (r16[2]->reg)--;
                        break;

                    case(0x0E): case(0x1E): case(0x2E): case(0x3E):
                        (*r8[(OPCODE & 0x38)>>3]) = mem[PC];
                        PC++;
                        break;


                    case(0x01):
                        break;


                    default:
                        break;


                }
            }

            void main_loop()
            {
                read_from_file("../test.bin");
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
