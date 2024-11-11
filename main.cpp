
#include "main.h"

int loop_counter = 1;

//constants
BYTE m_CartridgeMemory[0x200000];



//flag getters
BYTE gameboy::get_Z_flag_status() const //returns 1 or 0
{
    return (AF_reg.lo & (BYTE)(1 << FLAG_Z)) == (BYTE)(1 << FLAG_Z);
}
BYTE gameboy::get_N_flag_status() const //returns 1 or 0
{
    return (AF_reg.lo & (BYTE)(1 << FLAG_N)) == (BYTE)(1 << FLAG_N);
}
BYTE gameboy::get_H_flag_status() const //returns 1 or 0
{
    return (AF_reg.lo & (BYTE)(1 << FLAG_H)) == (BYTE)(1 << FLAG_H);
}
BYTE gameboy::get_C_flag_status() const //returns 1 or 0
{
    return (AF_reg.lo & (BYTE)(1 << FLAG_C)) == (BYTE)(1 << FLAG_C);
}

class gameboy
        {
        public:
            //constructor
            //gameboy();
            //gameboy() : mem{}{};



            //interrupt getters
            BYTE get_interrupt_bit_status(WORD IE_IF, int interrupt_type)
            {
                return (mem[IE_IF] & (0x01 << interrupts[interrupt_type]));
            }


            //flag setters
            void set_Z_flag_status(BYTE status)
            {
                if(status == 0)
                {
                    AF_reg.lo = (AF_reg.lo | (BYTE)(1 << FLAG_Z)); //should turn on FLAG_ZERO
                }
                else
                {
                    AF_reg.lo = (AF_reg.lo & (BYTE)(~(1 << FLAG_Z))); //should turn off FLAG_ZERO
                }
            }
            void set_N_flag_status(BYTE status)
            {
                if(status == 0)
                {
                    AF_reg.lo = (AF_reg.lo & (BYTE)(~(1 << FLAG_N))); //should turn off FLAG_ZERO
                }
                else
                {
                    AF_reg.lo = (AF_reg.lo | (BYTE)(1 << FLAG_N)); //should turn on FLAG_ZERO
                }
            }
            void set_H_flag_status(BYTE status)
            {
                if(status == 0)
                {
                    AF_reg.lo = (AF_reg.lo & (BYTE)(~(1 << FLAG_H))); //should turn off FLAG_ZERO
                }
                else
                {
                    AF_reg.lo = (AF_reg.lo | (BYTE)(1 << FLAG_H)); //should turn on FLAG_ZERO
                }
            }
            void set_C_flag_status(BYTE status)
            {
                if(status == 0)
                {
                    AF_reg.lo = (AF_reg.lo & (BYTE)(~(1 << FLAG_C))); //should turn off FLAG_ZERO
                }
                else
                {
                    AF_reg.lo = (AF_reg.lo | (BYTE)(1 << FLAG_C)); //should turn on FLAG_ZERO
                }
            }

            //interrupt setters
            void set_interrupt_bit(int interrupt_type, int mode)
            {
                if(mode)
                {
                    mem[IF_reg] = mem[IF_reg] | (BYTE)((0x01) << interrupts[interrupt_type]);
                }
                else
                {
                    mem[IF_reg] = mem[IF_reg] & (BYTE)(~(0x01) << interrupts[interrupt_type]);
                }

            }



            //testing funcs
            void init_register_file(){
                ofstream outStatusFile("../registers_status.txt"); //overwriting the file if it exists

                if (!outStatusFile) {
                    std::cerr << "Error: Could not open 'registers_status.txt'" << endl;
                }
                outStatusFile.close();
            }
            void init_memory_file(){
                ofstream outMemoryFile("../memory_status.txt"); //overwriting the file if it exists

                if (!outMemoryFile) {
                    std::cerr << "Error: Could not open 'memory_status.txt'" << endl;
                }
                outMemoryFile.close();
            }
            void print_registers_r8()
            {
                ofstream outStatusFile("../registers_status.txt", ios::app);

                if (!outStatusFile) {
                    std::cerr << "Error: Could not open 'registers_status.txt'" << endl;
                    return;
                }

                outStatusFile << "A: " << std::hex << std::uppercase << std::setw(2) << std::setfill('0')  << (int)(*r8[A])<< " " << dec;
                outStatusFile << "F: " << std::hex << std::setw(2) << std::setfill('0')  << (int)(AF_reg.lo)<< " " << dec;
                outStatusFile << "B: " << std::hex << std::setw(2) << std::setfill('0')  << (int)(*r8[B])<< " " << dec;
                outStatusFile << "C: " << std::hex << std::setw(2) << std::setfill('0')  << (int)(*r8[C])<< " " << dec;
                outStatusFile << "D: " << std::hex << std::setw(2) << std::setfill('0')  << (int)(*r8[D])<< " " << dec;
                outStatusFile << "E: " << std::hex << std::setw(2) << std::setfill('0')  << (int)(*r8[E])<< " " << dec;
                outStatusFile << "H: " << std::hex << std::setw(2) << std::setfill('0')  << (int)(*r8[H])<< " " << dec;
                outStatusFile << "L: " << std::hex << std::setw(2) << std::setfill('0')  << (int)(*r8[L])<< " " << dec;
                outStatusFile << "SP: " << std::hex << std::setw(4) << std::setfill('0')  << (int)(r16[SP]->reg)<< " " << dec;
                outStatusFile << "PC: 00:" << std::hex << std::setw(4) << std::setfill('0')  << PC << " " << dec;
                outStatusFile << "(" << std::hex << std::setw(2) << std::setfill('0')  << (int)mem[PC] << " " <<
                                           std::setw(2) << std::setfill('0')  << (int)mem[PC + 1] << " " <<
                                           std::setw(2) << std::setfill('0')  <<(int)mem[PC + 2] << " " <<
                                           std::setw(2) << std::setfill('0')  <<(int)mem[PC + 3] << ")\n" << dec;
                outStatusFile.close();

            }


            //for gameboy_doctor
            void gbdoctor_init_register_file(){
                ofstream outStatusFile("../gbdoctor.txt"); //overwriting the file if it exists

                if (!outStatusFile) {
                    std::cerr << "Error: Could not open 'registers_status.txt'" << endl;
                }
                outStatusFile.close();
            }
            void gbdoctor_print_registers_r8()
            {
                ofstream outStatusFile("../gbdoctor.txt", ios::app);

                if (!outStatusFile) {
                    std::cerr << "Error: Could not open 'registers_status.txt'" << endl;
                    return;
                }

                outStatusFile << "A:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0')  << (int)(*r8[A])<< " " << dec;
                outStatusFile << "F:" << std::hex << std::setw(2) << std::setfill('0')  << (int)(AF_reg.lo)<< " " << dec;
                outStatusFile << "B:" << std::hex << std::setw(2) << std::setfill('0')  << (int)(*r8[B])<< " " << dec;
                outStatusFile << "C:" << std::hex << std::setw(2) << std::setfill('0')  << (int)(*r8[C])<< " " << dec;
                outStatusFile << "D:" << std::hex << std::setw(2) << std::setfill('0')  << (int)(*r8[D])<< " " << dec;
                outStatusFile << "E:" << std::hex << std::setw(2) << std::setfill('0')  << (int)(*r8[E])<< " " << dec;
                outStatusFile << "H:" << std::hex << std::setw(2) << std::setfill('0')  << (int)(*r8[H])<< " " << dec;
                outStatusFile << "L:" << std::hex << std::setw(2) << std::setfill('0')  << (int)(*r8[L])<< " " << dec;
                outStatusFile << "SP:" << std::hex << std::setw(4) << std::setfill('0')  << (int)(r16[SP]->reg)<< " " << dec;
                outStatusFile << "PC:" << std::hex << std::setw(4) << std::setfill('0')  << PC << " " << dec;
                outStatusFile << "PCMEM:" << std::hex << std::setw(2) << std::setfill('0')  << (int)mem[PC] << "," <<
                              std::setw(2) << std::setfill('0')  << (int)mem[PC + 1] << "," <<
                              std::setw(2) << std::setfill('0')  <<(int)mem[PC + 2] << "," <<
                              std::setw(2) << std::setfill('0')  <<(int)mem[PC + 3] << "\n" << dec;
                outStatusFile.close();

            }


            static void print_memory_writes(WORD OPCODE,WORD address, BYTE val)
            {
                outMemoryFile.open("../memory_status.txt", ios::app);
                if (!outMemoryFile) {
                    std::cerr << "Error: Could not open 'memory_status.txt'" << std::endl;
                    return;
                }
                outMemoryFile << std::uppercase  << std::setfill('0') << loop_counter << ": (0x" << hex << OPCODE << ") " << "mem[" << address << "] <- "<< std::setw(2) << (int)val << dec << endl;
                outMemoryFile.close();
            }





            //for readability
            BYTE half_carry_8bit = 0x10; //5th bit on, 1st bit of highest nibble
            WORD half_carry_16bit = 0x1000; //5th bit of the most significant BYTE, 1st of highest nibble
            BYTE carry_8bit = 0x80; //most significant bit
            WORD carry_16bit = 0x8000; //most significant bit




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
                mem[0xFF44] = 0x90; //for testing, only for now since we don't have an LCD
                mem[0xFF45] = 0x00 ;
                mem[0xFF47] = 0xFC ;
                mem[0xFF48] = 0xFF ;
                mem[0xFF49] = 0xFF ;
                mem[0xFF4A] = 0x00 ;
                mem[0xFF4B] = 0x00 ;
                mem[0xFFFF] = 0x00 ; //Interrupt Enabler

            }


            //deep copy function from cartridge to mem ~@@@@@@@@@@@@@@@@@@@@@@@@@@@@
            ///This is a temporary placeholder of a function. In reality, moving the cartridge mem to gb mem is more complicated, for now we just dump it
            void cartridge_to_mem(long long bytes)
            {
                for(long long i = 0 ; i < bytes ; i++)
                {
                    mem[i] = m_CartridgeMemory[0x100+i];
//                  mem[i+0x100] = m_CartridgeMemory[i];

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
//                input_file.read((char *)m_CartridgeMemory, sizeof(mem) - 1); ///this char cast may cause problems in the long run, may change.

                m_CartridgeMemory[input_file.gcount()] = '\0';
                cartridge_to_mem(input_file.gcount());
            }

            //this will be the function that read the cartridge and organizes its data into the system
//            void read_cartridge(string path)
//            {
//                ifstream gb_cartridge(path,ios::binary);
//                if(!gb_cartridge)
//                {
//                    cerr << "File error.\n";
//                }
//
//                //HEADER RANGE - $0100 - $014F
//
//                //ENTRY POINT - 0100-0103
//                gb_cartridge.seekg(0x100,ios::beg);
//            }

            void fetch()
            {
                OPCODE = mem[PC];
                PC++;
                num_of_machine_cycles(1);
                if(OPCODE == 0xCB)
                {
                    OPCODE = OPCODE<<8 | mem[PC];
                    PC++;
                }
            }

            //move PC to interrupt handler, backup PC in stack
            void PC_to_interrupt(int interrupt_routine_type)
            {
                interrupt_mode = 1;
                IME = 0;
                set_interrupt_bit(interrupt_routine_type,0);
                //PUSH r16stk command
                r16[SP]->reg = (r16[SP]->reg) - 1;
                mem[r16[SP]->reg] = (PC >> 8); //this is the MSByte of PC
                r16[SP]->reg = (r16[SP]->reg) - 1;
                mem[r16[SP]->reg] = PC; //this is the LSByte of PC

                if(testing_mode)
                {
                    print_memory_writes(0xF4, r16[SP]->reg+1, (PC>>8)); //0xF4 is an arbitrary opcode for interrupt push
                    print_memory_writes(0xF4, r16[SP]->reg, (PC));
                }

                PC = interrupt_routine_addresses[interrupt_routine_type];
            }



            void check_interrupts()
            {
                //check if HALT status can be disabled

                //https://robertovaccari.com/blog/2020_09_26_gameboy/
                if(( mem[IF_reg] & mem[IE_reg] ) != 0)
                {
                    if(is_halted == 2)
                        cout << "HALT STOPPED\n";
                    is_halted = 0;
                    if(IME == 1)
                    {
                        if(get_interrupt_bit_status(IF_reg,vblank) && get_interrupt_bit_status(IE_reg,vblank)) //Vblank
                        {
                            PC_to_interrupt(vblank);
                        }
                        else if(get_interrupt_bit_status(IF_reg,lcd) && get_interrupt_bit_status(IE_reg,lcd)) //LCD
                        {
                            PC_to_interrupt(lcd);
                        }
                        else if(get_interrupt_bit_status(IF_reg,timer) && get_interrupt_bit_status(IE_reg,timer)) //Timer
                        {
                            PC_to_interrupt(timer);
                        }
                        else if(get_interrupt_bit_status(IF_reg,serial_link) && get_interrupt_bit_status(IE_reg,serial_link)) //Serial Link
                        {
                            PC_to_interrupt(serial_link);
                        }
                        else if(get_interrupt_bit_status(IF_reg,joypad) && get_interrupt_bit_status(IE_reg,joypad)) //Joypad
                        {
                            PC_to_interrupt(joypad);
                        }
                    }

                }
            }

            void post_interrupt()
            {
                interrupt_mode = 0;
                IME = 1;
                tmp = 0;
                tmp = mem[r16[SP]->reg];
                r16[SP]->reg = (r16[SP]->reg) + 1;
                tmp = ((r16[SP]->reg) << 8) | tmp; // PC[HI] | PC[LO]
                r16[SP]->reg = (r16[SP]->reg) + 1;
                PC = tmp;

            }

            //timer related funcs
            void update_timers(unsigned int machine_cycles_added,int running_mode) //running modes - 0: reguler, 1: pause: 2:stop
            {
//                if(running_mode < 2) //meaning un-stopped
//                {
                    //previous TIMA result
                    tmp_uChar = (mem[TAC_register] >> 2) & ((DIV_timer >> TAC_speeds[mem[TAC_register] & 0x03]) & 0x01); //result of previous cycle


                    DIV_timer = DIV_timer + 4*machine_cycles_added;
                    mem[DIV_register] = (DIV_timer >> 8); //this means that every 256 clock cycles, this will increment by one
                    tmp = (mem[TAC_register] >> 2) & ((DIV_timer >> TAC_speeds[mem[TAC_register] & 0x03]) & 0x01);

//                    if(running_mode < 1) //meaning un-paused
//                    {
                    //result of current cycle
                    if(tmp_uChar & !(tmp))
                    {
                        if(mem[TIMA_register] + 1 > UCHAR_MAX)
                        {
                            mem[TIMA_register] = mem[TMA_register];
                            set_interrupt_bit(timer,1);
                        }
                        else
                        {
                            mem[TIMA_register] = mem[TIMA_register] + machine_cycles_added;
                        }


                    }
//                        if(mem[TAC_register] >> 2) //check if the "enable" bit is on, 3rd bit from lsb
//                        {
//                            TIMA_timer = TIMA_timer + machine_cycles_added*4;
//                            BYTE shift_amount = TAC_speeds[mem[TAC_register] & 0x03];
//                            WORD result = (WORD)(TIMA_timer >> shift_amount);
//                            if( result  > UCHAR_MAX)
//                            {
//                                TIMA_timer = mem[TMA_register];
//                                mem[TIMA_register] = TIMA_timer;
//                                //set interrupt bit
//                                set_interrupt_bit(timer,1);
//                            }
//                            else
//                            {
//                                mem[TIMA_register] = result; //only 2 first bits relevant
//                            }
//                        }


            }

            //https://github.com/Hacktix/GBEDG/blob/master/timers/index.md#-ff04---divider-register--div-
            void check_div_reg_change(WORD address)//checks if DIV_register mem was written into, if so, it is reset to 0
            {
                if (address == DIV_register)
                {
                    DIV_timer = 0;
                    mem[DIV_register] = 0;
                }
            }

            //machine cycles management
            void num_of_machine_cycles(int num)
            {
                gb_machine_cycles = gb_machine_cycles + num;
            }

            void decode_execute()
            {

                r8[6] = &mem[HL_reg.reg]; ///temporary fix to r8[6] not pointing to the correct memory

                switch(OPCODE) //MAY NEED TO FIX THE OPCODE SINCE ITS 2 BYTES, BUT MAYBE ITS OK SINCE IT SHOULD BE 0 anyway
                {
                    case(0x00): //NOP OPERATION
                        num_of_machine_cycles(1);
                        break;

                    //tested
                    case(0x01):case(0x11):case(0x21):case(0x31): //LD r16, d16
                        //r16[4th&5th_bits] = memory[PC] which is 2 bytes
                        //increment PC twice
                        tmp = (OPCODE & 0x30)>>4;
                        r16[tmp]->lo = mem[PC];
                        PC = PC + 1;
                        r16[tmp]->hi = mem[PC];
                        PC = PC + 1;
                        num_of_machine_cycles(3);
                        break;

                    case(0x10): ///TODO: STOP
                        cout << "STOP COMMAND REACHED\n";
                        num_of_machine_cycles(1);
                        break;

                    case(0x76): ///TODO: HALT, currently it is a temporary solution
                        is_halted = 1;
                        cout << "HALT COMMAND REACHED\n";
                        num_of_machine_cycles(1);
                        break;


                    case(0xF9): //LD SP, HL
                        r16[SP]->reg = r16[HL_16]->reg;
                        num_of_machine_cycles(2);
                        break;

                    case(0xF3): //DI (disable interrupts)
                        IME = 0;
                        num_of_machine_cycles(1);
                        break;

                    case(0xFB): //EI (enable interrupts)
                        IME = 1;
                        num_of_machine_cycles(1);
                        break;

                    //tested
                    case(0x02): case(0x12): //LD (BC) OR (DE), A


                        tmp = (OPCODE & 0x30)>>4;
                        mem[r16[tmp]->reg] = *r8[A];

                        if(testing_mode)
                            print_memory_writes(OPCODE, r16[tmp]->reg,*r8[A]);
//                        outMemoryFile << loop_counter << ": " << hex << "Store the contents of register A: " << (int)*r8[A] << " in the memory location: mem[" << r16[tmp]->reg << "] specified by register pair " << r16[tmp] << dec << endl;

                        check_div_reg_change(r16[tmp]->reg);

                        num_of_machine_cycles(2);
                        break;

                    case(0x08): //LD (a16), SP
                        tmp = 0;
                        tmp = mem[PC];
                        PC++;
                        tmp = ((mem[PC]<<8) | tmp);
                        PC++;
                        mem[tmp] = r16[SP]->lo;
                        mem[tmp+1] = r16[SP]->hi;

                        if(testing_mode)
                        {
                            print_memory_writes(OPCODE, tmp, r16[SP]->lo);
                            print_memory_writes(OPCODE, tmp+1, r16[SP]->hi);
                        }

                        check_div_reg_change(tmp);
                        check_div_reg_change(tmp+1);

                        num_of_machine_cycles(5);
                        break;

                    case(0x07): // RLCA
                        set_C_flag_status(((*r8[A]) & (0x80)) >> 7);
                        *r8[A] = *r8[A] << 1;
                        if(get_C_flag_status() == 0)
                        {
                            AF_reg.hi = (AF_reg.hi & (BYTE)(~(1))); //should turn off bit 0 of Reg A
                        }
                        else
                        {
                            AF_reg.hi = (AF_reg.hi | (BYTE)(1)); //should turn on bit 0 of Reg A
                        }
                        set_Z_flag_status(1); //non zero, meaning flag will be set off
                        set_N_flag_status(0);
                        set_H_flag_status(0);

                        num_of_machine_cycles(1);
                        break;

                    case(0x17): // RLA

                        tmp = get_C_flag_status();
                        set_C_flag_status(((*r8[A]) & (0x80)) >> 7);
                        *r8[A] = *r8[A] << 1;
                        if(tmp == 0)
                        {
                            AF_reg.hi = (AF_reg.hi & (BYTE)(~(1))); //should turn off bit 0 of Reg A
                        }
                        else
                        {
                            AF_reg.hi = (AF_reg.hi | (BYTE)(1)); //should turn on bit 0 of Reg A
                        }
                        set_Z_flag_status(1); //non zero, meaning flag will be set off
                        set_N_flag_status(0);
                        set_H_flag_status(0);
                        num_of_machine_cycles(1);
                        break;

                    case(0x0F): // RRCA

                        set_C_flag_status(((*r8[A]) & (BYTE)(0x01)));
                        *r8[A] = *r8[A] >> 1;
                        if(get_C_flag_status() == 0)
                        {
                            AF_reg.hi = (AF_reg.hi & (BYTE)(~(0x80))); //should turn off bit 7 of Reg A
                        }
                        else
                        {
                            AF_reg.hi = (AF_reg.hi | (BYTE)(0x80)); //should turn on bit 7 of Reg A
                        }
                        set_Z_flag_status(1); //non zero, meaning flag will be set off
                        set_N_flag_status(0);
                        set_H_flag_status(0);
                        num_of_machine_cycles(1);
                        break;

                    case(0x1F): // RRA
                        tmp = get_C_flag_status();
                        set_C_flag_status(((*r8[A]) & (BYTE)(0x01)));
                        *r8[A] = *r8[A] >> 1;
                        if(tmp == 0)
                        {
                            AF_reg.hi = (AF_reg.hi & (BYTE)(~(0x80))); //should turn off bit 7 of Reg A
                        }
                        else
                        {
                            AF_reg.hi = (AF_reg.hi | (BYTE)(0x80)); //should turn on bit 7 of Reg A
                        }
                        set_Z_flag_status(1); //non zero, meaning flag will be set off
                        set_N_flag_status(0);
                        set_H_flag_status(0);
                        num_of_machine_cycles(1);
                        break;

                    case(0x37): // SCF
                        set_N_flag_status(0);
                        set_H_flag_status(0);
                        set_C_flag_status(1);
                        num_of_machine_cycles(1);
                        break;

                    case(0x3F): // CCF
                        set_N_flag_status(0);
                        set_H_flag_status(0);
                        if(get_C_flag_status() == 0)
                            set_C_flag_status(1);
                        else
                            set_C_flag_status(0);

                        num_of_machine_cycles(1);
                        break;


                    case(0x27): //DAA -
                    // credit https://blog.ollien.com/posts/gb-daa/#fn:2
                    // credit https://forums.nesdev.org/viewtopic.php?t=15944

                        if(!get_N_flag_status())
                        {
                            //check upper nibble, since it doesn't affect lower nibble, eliminating need for temp
                            if (*r8[A] > 0x99 || get_C_flag_status())
                            {
                                *r8[A] = *r8[A] + 0x60;
                                set_C_flag_status(1); //C FLAG
                            }

                            if ((*r8[A] & 0x0F) > 0x09 || get_H_flag_status()) //check lower nibble
                                *r8[A] = *r8[A] + 0x06;
                        }
                        else
                        {
                            //this means a subtraction occured, we are checking for borrow operations
                            if(get_C_flag_status())
                                *r8[A] = *r8[A] - 0x60;
                            if(get_H_flag_status())
                                *r8[A] = *r8[A] - 0x06;
                        }

                        //flags
                        set_Z_flag_status(*r8[A]);


                        set_H_flag_status(0);

                        //removed this for now since potentially we could overflow A reg, and thus it wouldn't show
//                        if(*r8[A] > 0x99)
//                            set_C_flag_status(1);
//                        else
//                            set_C_flag_status(0);
                        num_of_machine_cycles(1);

                        break;

                    case(0x2F): //CPL (flip all bits)
                        *r8[A] = ~*r8[A];
                        set_N_flag_status(1);
                        set_H_flag_status(1);
                        num_of_machine_cycles(1);
                        break;


                    ///TO CHECK FIRST THING - IS A "TMP" still relevant in this opcode?! I DONT THINK SO, DELETING TMP FOR NOW
                    //tested
                    case(0x22): //LD (HL), A
                        //tmp = (OPCODE & 0x30)>>4;
                        mem[r16[HL_16]->reg] = *r8[A];
                        r16[HL_16]->reg++; //increment the CONTENTS of HL --- MAY BE PROBLEMATIC DOWN THE LINE

                        if(testing_mode)
                            print_memory_writes(OPCODE, r16[HL_16]->reg,*r8[A]);

                        check_div_reg_change(r16[HL_16]->reg);

                        num_of_machine_cycles(2);
                        break;

                    //tested
                    case(0x32):
                        mem[r16[HL_16]->reg] = *r8[A];
                        r16[HL_16]->reg--; //increment the CONTENTS of HL --- MAY BE PROBLEMATIC DOWN THE LINE

                        if(testing_mode)
                            print_memory_writes(OPCODE, r16[HL_16]->reg,*r8[A]);

                        check_div_reg_change(r16[HL_16]->reg);
                        num_of_machine_cycles(2);
                        break;


                    //tested
                    case(0x03):case(0x13):case(0x23):case(0x33): //INC r16[reg]
                        tmp = (OPCODE & 0x30)>>4; //relevant opcode bits in r16 are 4th & 5th
                        r16[tmp]->reg++;

                        num_of_machine_cycles(2);
                        break;

                    //tested
                    case(0x0B):case(0x1B):case(0x2B):case(0x3B): //DEC r16[reg]
                        tmp = (OPCODE & 0x30)>>4; //relevant opcode bits in r16 are 4th & 5th
                        r16[tmp]->reg--;

                        num_of_machine_cycles(2);
                        break;

                    //to-test
                    case(0x04):case(0x14):case(0x24):case(0x34): //INC r8[reg]
                    case(0x0C):case(0x1C):case(0x2C):case(0x3C): //INC r8[reg]
                        tmp_uChar = *r8[(OPCODE & 0x38)>>3]; //relevant opcode bits in r8 are 3rd, 4th & 5th
                        (*r8[(OPCODE & 0x38)>>3])++; ///may cause error when incrementing B

                        //flags
                        //FLAG_H
                        if ((((tmp_uChar & 0x0F)+1) & half_carry_8bit) == half_carry_8bit)
                            set_H_flag_status(1);
                        else
                            set_H_flag_status(0);

                        set_Z_flag_status((*r8[(OPCODE & 0x38)>>3])); // will set on if 0, else flag val 1
                        set_N_flag_status(0); //should turn off FLAG_N

                        if(testing_mode && (OPCODE == 0x34))
                        {
                            //cout << "0x34\n";
                            print_memory_writes(OPCODE, r16[HL_16]->reg, mem[r16[HL_16]->reg]+1);
                        }

                        if(OPCODE == 0x34)
                        {
                            num_of_machine_cycles(3);
                            check_div_reg_change(r16[HL_16]->reg);
                        }
                        else
                        {
                            num_of_machine_cycles(1);
                        }
                        break;

                    //tested
                    case(0x05):case(0x15):case(0x25):case(0x35): //DEC r8[reg]
                    case(0x0D):case(0x1D):case(0x2D):case(0x3D): //DEC r8[reg]
                        tmp_uChar = *r8[(OPCODE & 0x38)>>3];
                        (*r8[(OPCODE & 0x38)>>3])--; ///may cause error when incrementing B


                        //flags
                        //FLAG_H
                        if ((((tmp_uChar & 0x0F)-1) & half_carry_8bit) == half_carry_8bit) //https://www.reddit.com/r/EmuDev/comments/knm196/gameboy_half_carry_flag_during_subtract_operation/
                            set_H_flag_status(1); //should turn on
                        else
                            set_H_flag_status(0); //should turn OFF FLAG_ZERO

                        set_Z_flag_status((*r8[(OPCODE & 0x38)>>3]));
                        set_N_flag_status(1);

                        if(testing_mode && (OPCODE == 0x35))
                        {
                            print_memory_writes(OPCODE, r16[HL_16]->reg, mem[r16[HL_16]->reg]-1);
                        }

                        if(OPCODE == 0x35)
                        {
                            num_of_machine_cycles(3);
                            check_div_reg_change(r16[HL_16]->reg);
                        }
                        else
                        {
                            num_of_machine_cycles(1);
                        }

                        break;

                    //to-test
                    case(0x06): case(0x16): case(0x26): case(0x36): //LD SUBREG, d8
                        tmp = (OPCODE & 0x38)>>3;
                        (*r8[tmp]) = mem[PC];
                        PC++;

                        if(testing_mode && (OPCODE == 0x36))
                        {
                            print_memory_writes(OPCODE, r16[HL_16]->reg, mem[PC-1]);
                        }

                        if(OPCODE == 0x36)
                        {
                            num_of_machine_cycles(3);
                            check_div_reg_change(r16[HL_16]->reg);
                        }
                        else
                        {
                            num_of_machine_cycles(2);
                        }
                        break;

                    ///LD INSTRUCTIONS WITH REGISTERS - MAY UNIFY THEM ALL INTO ONE COMMAND SOON


                    //to-test
                    case(0x40): case(0x41): case(0x42): case(0x43): case(0x44): case(0x45): case(0x46): case(0x47): //LD B,*SUBREG*
                    case(0x50): case(0x51): case(0x52): case(0x53): case(0x54): case(0x55): case(0x56): case(0x57): //LD D,*SUBREG*
                    case(0x60): case(0x61): case(0x62): case(0x63): case(0x64): case(0x65): case(0x66): case(0x67): //LD H,*SUBREG*
                    case(0x70): case(0x71): case(0x72): case(0x73): case(0x74): case(0x75): case(0x77): //LD (HL),*SUBREG*
                    case(0x48): case(0x49): case(0x4A): case(0x4B): case(0x4C): case(0x4D): case(0x4E): case(0x4F): //LD C,*SUBREG*
                    case(0x58): case(0x59): case(0x5A): case(0x5B): case(0x5C): case(0x5D): case(0x5E): case(0x5F): //LD E,*SUBREG*
                    case(0x68): case(0x69): case(0x6A): case(0x6B): case(0x6C): case(0x6D): case(0x6E): case(0x6F): //LD L,*SUBREG*
                    case(0x78): case(0x79): case(0x7A): case(0x7B): case(0x7C): case(0x7D): case(0x7E): case(0x7F): //LD A,*SUBREG*


                        (*r8[(OPCODE & 0x38)>>3]) = (*r8[(OPCODE & 0x07)]); //dst: relevant opcode bits in r8 are 3rd, 4th & 5th, src: rel bits 0,1,2
                        if(testing_mode && (OPCODE == 0x70 || OPCODE == 0x71 || OPCODE == 0x72 || OPCODE == 0x73 || OPCODE == 0x74 || OPCODE == 0x75 || OPCODE == 0x77))
                        {
                            print_memory_writes(OPCODE ,r16[HL_16]->reg, (*r8[(OPCODE & 0x07)]));
                        }

                        //machine cycles update
                        if(OPCODE == 0x46 || OPCODE == 0x56 || OPCODE == 0x66 || OPCODE == 0x70 || OPCODE == 0x71 || OPCODE == 0x72 || OPCODE == 0x73 ||
                        OPCODE == 0x74 || OPCODE == 0x75 || OPCODE == 0x77 || OPCODE == 0x4E || OPCODE == 0x5E || OPCODE == 0x6E || OPCODE == 0x7E )
                        {
                            num_of_machine_cycles(2);
                            check_div_reg_change(r16[HL_16]->reg);
                        }
                        else
                        {
                            num_of_machine_cycles(1);
                        }
                        break;

                    ///

                    case(0x09):case(0x19):case(0x29):case(0x39): //ADD HL, rr: Add 16bit reg to HL
                        operand_1 = (r16[HL_16]->reg); // for flags
                        operand_2 = (r16[((OPCODE & 0x30)>>4)]->reg);
                        (r16[HL_16]->reg) = operand_1 + operand_2;
                        //FLAGS
                        set_N_flag_status(0); //should turn off FLAG_N

                        //FLAG_C
                        if (operand_1 + operand_2 > USHRT_MAX) //16 BIT!!!
                            set_C_flag_status(1); //should turn on FLAG_CARRY
                        else
                            set_C_flag_status(0); //should turn OFF FLAG_CARRY
                        //FLAG_H
                        if ((((operand_1 & 0x0FFF) + (operand_2 & 0x0FFF)) & half_carry_16bit) == half_carry_16bit) //16 BIT!!!
                            set_H_flag_status(1); //should turn on FLAG_HALF
                        else
                            set_H_flag_status(0); //should turn OFF FLAG_HALF

                        num_of_machine_cycles(2);
                        break;



                    //to-test
                    case(0x0A): case(0x1A):  //LD A, (REG)
                        tmp = (OPCODE & 0x30)>>4;
                        (*r8[A]) = mem[r16[tmp]->reg];
                        num_of_machine_cycles(2);
                        break;


                    case(0xE0): //LD (a8), A
                        tmp_uChar = mem[PC]; //FIX!
                        PC++;
                        tmp = (WORD)0xFF00|tmp_uChar;
                        mem[tmp] = *r8[A]; //MSB is FF, LSB is the PC byte

                        //this is for blaarg's test suite, can be moved to testing_mode if causes problems
                        if(tmp == SC_reg && *r8[A] == 0x81)
                        {
                            cout << mem[SB_reg];
                        }
                        if(testing_mode)
                        {
                            print_memory_writes(OPCODE,(WORD)tmp, *r8[A]);

                        }
                        check_div_reg_change(tmp);
                        num_of_machine_cycles(3);
                        break;

                    case(0xF0): //LD A, (a8)
                        tmp_uChar = mem[PC];
                        PC++;
                        tmp = (WORD)0xFF00|(WORD)tmp_uChar;
                        *r8[A] = mem[tmp]; //MSB is FF, LSB is the PC byte
                        num_of_machine_cycles(3);
                        break;

                    case(0xE2): //LD (C), A
                        tmp = (WORD)0xFF00|*r8[C];
                        mem[tmp] = *r8[A]; //MSB is FF, LSB is the C subreg

                        if(testing_mode)
                        {
                            print_memory_writes(OPCODE, tmp, *r8[A]);
                            if(tmp == SC_reg && *r8[A] == 0x81)
                            {
                                cout << mem[SB_reg];
                            }
                        }

                        check_div_reg_change(tmp);
                        num_of_machine_cycles(2);
                        break;

                    case(0xF2): //LD A, (C)
                        *r8[A] = mem[(WORD)0xFF00|*r8[C]]; //MSB is FF, LSB is the PC byte
                        num_of_machine_cycles(2);
                        break;

                    case(0xEA): //LD (a16), A
                        tmp = 0;
                        tmp = mem[PC];
                        PC++;
                        tmp = ((mem[PC]<<8) | tmp);
                        PC++;
                        mem[tmp] = *r8[A];

                        if(testing_mode)
                        {
                            print_memory_writes(OPCODE, tmp, *r8[A]);
                        }

                        check_div_reg_change(tmp);
                        num_of_machine_cycles(4);
                        break;

                    case(0xFA): //LD A, (a16)
                        tmp = 0;
                        tmp = mem[PC];
                        PC++;
                        tmp = (tmp | (mem[PC]<<8));
                        PC++;
                        *r8[A] = mem[tmp];

                        num_of_machine_cycles(4);
                        break;

                    case(0xF8): //LD HL, SP+e
                        operand_1 = r16[SP]->reg;
                        tmp_sChar = (signed char)mem[PC];

                        r16[HL_16]->reg = operand_1 + tmp_sChar;
                        PC++;

                        //flags
                        set_Z_flag_status(1); //non zero, meaning set flag off
                        set_N_flag_status(0);

                        if ( (( ((short)operand_1 & 0x0F) + (tmp_sChar & 0x0F) ) & half_carry_8bit) == half_carry_8bit)
                            set_H_flag_status(1);
                        else
                            set_H_flag_status(0);

                        if((short)(operand_1 & 0xFF) + (tmp_sChar & 0xFF) > UCHAR_MAX)
                            set_C_flag_status(1);
                        else
                            set_C_flag_status(0);

                        num_of_machine_cycles(3);
                        break;


                    case(0x18): //JR s8
                        tmp_sChar = (signed char)mem[PC];
                        PC++;
                        PC = PC + tmp_sChar;

                        num_of_machine_cycles(3);
                        break;

                    case(0x20): //JR NZ,s8
                        tmp_sChar = (signed char)mem[PC];
                        PC++;
                        if(!get_Z_flag_status())
                        {
                            PC = PC + tmp_sChar;
                            num_of_machine_cycles(1);
                        }

                        num_of_machine_cycles(2);
                        break;


                    case(0x28): //JR Z,s8
                        tmp_sChar = (signed char)mem[PC];
                        PC++;
                        if(get_Z_flag_status())
                        {
                            PC = PC + tmp_sChar;
                            num_of_machine_cycles(1);
                        }


                        num_of_machine_cycles(2);
                        break;



                    case(0x30): //JR NC,s8
                        tmp_sChar = (signed char)mem[PC];
                        PC++;
                        if(!get_C_flag_status())
                        {
                            PC = PC + tmp_sChar;
                            num_of_machine_cycles(1);
                        }


                        num_of_machine_cycles(2);
                        break;


                    case(0x38): //JR C,s8
                        tmp_sChar = (signed char)mem[PC];
                        PC++;
                        if(get_C_flag_status())
                        {
                            PC = PC + tmp_sChar;
                            num_of_machine_cycles(1);
                        }

                        num_of_machine_cycles(2);
                        break;



                    //to-test
                    case(0x2A): //LD A, (HL+)
                        (*r8[A]) = mem[r16[HL_16]->reg]; //HL //may be problematic, since mem is an array of WORDS, so it should only take the first BYTE of the word
                        (r16[HL_16]->reg)++;

                        num_of_machine_cycles(2);
                        break;
                    //to-test
                    case(0x3A): //LD A, (HL-)
                        (*r8[A]) = mem[r16[HL_16]->reg]; //HL
                        (r16[HL_16]->reg)--;

                        num_of_machine_cycles(2);
                        break;

                    case(0x0E): case(0x1E): case(0x2E): case(0x3E): //LD C,d8
                        (*r8[(OPCODE & 0x38)>>3]) = mem[PC];
                        PC++;

                        num_of_machine_cycles(2);
                        break;

                    case(0x80): case(0x81): case(0x82):case(0x83): case(0x84): case(0x85): case(0x86): case(0x87): //ADD r8,r8
                        operand_1 = (*r8[A]);
                        operand_2 = (*r8[(OPCODE & 0x07)]);
                        (*r8[A]) = operand_1 + operand_2;

                        //flags
                        //FLAG_C
                        if (operand_1 + operand_2 > UCHAR_MAX)
                            set_C_flag_status(1); //should turn on FLAG_CARRY
                        else
                            set_C_flag_status(0); //should turn OFF FLAG_CARRY
                        //FLAG_H
                        if ( (((operand_1 & 0x0F)+(operand_2 & 0x0F) ) & half_carry_8bit) == half_carry_8bit)
                            set_H_flag_status(1); //should turn on FLAG_HALF
                        else
                            set_H_flag_status(0); //should turn off FLAG_HALF

                        set_Z_flag_status((*r8[A]));
                        set_N_flag_status(0); //should turn off FLAG_N

                        if(OPCODE == 0x86)
                        {
                            num_of_machine_cycles(2);
                        }
                        else
                        {
                            num_of_machine_cycles(1);
                        }
                        break;

                    case(0xE8): //ADD SP, e (relative)
                        tmp_sChar = (signed char)(mem[PC]);
                        operand_1 = r16[SP]->reg;
                        PC++;
                        r16[SP]->reg = operand_1 + tmp_sChar;

                        //flags
                        //FLAG_C
                        if ((((short)operand_1 & 0xFF) + (tmp_sChar & 0xFF))  > UCHAR_MAX)
                            set_C_flag_status(1); //should turn on CARRY
                        else
                            set_C_flag_status(0); //should turn off CARRY
                        //FLAG_H
                        if ( ((((short)operand_1 & 0x0F)+(tmp_sChar & 0x0F) ) & half_carry_8bit) == half_carry_8bit)
                            set_H_flag_status(1); //should turn on FLAG_HALF
                        else
                            set_H_flag_status(0); //should turn OFF FLAG_HALF

                        set_Z_flag_status(1); //OFF
                        set_N_flag_status(0);

                        num_of_machine_cycles(4);
                        break;

                    case(0x88): case(0x89): case(0x8A):case(0x8B): case(0x8C): case(0x8D): case(0x8E): case(0x8F): //ADC A,r8
                        //here we must add before calculating flags, since it is dependent on the carry flag, we save it to tmp first to back up the final ans, because flag calculation may affect the result
                        operand_1 = (*r8[A]);
                        operand_2 = *r8[(OPCODE & 0x07)];
                        (*r8[A]) = operand_1 + operand_2 + get_C_flag_status();

                        //flags
                        //FLAG_H
                        if ( ( ( (operand_1 & 0x0F) + (operand_2 & 0x0F) + get_C_flag_status() ) & half_carry_8bit) == half_carry_8bit)
                            set_H_flag_status(1);
                        else
                            set_H_flag_status(0);
                        //FLAG_C
                        if (operand_1 + operand_2 + get_C_flag_status() > UCHAR_MAX)
                            set_C_flag_status(1);
                        else
                            set_C_flag_status(0);

                        set_Z_flag_status(*r8[A]);
                        set_N_flag_status(0);

                        if(OPCODE == 0x8E)
                        {
                            num_of_machine_cycles(2);
                        }
                        else
                        {
                            num_of_machine_cycles(1);
                        }
                        break;

                    case(0x90): case(0x91): case(0x92):case(0x93): case(0x94): case(0x95): case(0x96): case(0x97): //SUB A,r8
                        operand_1 = *r8[A];
                        operand_2 = *r8[(OPCODE & 0x07)];
                        *r8[A] = operand_1 - operand_2;

                        //flags
                        set_Z_flag_status(*r8[A]);
                        set_N_flag_status(1);
                        //FLAG_C
                        if (operand_1 < operand_2)
                            set_C_flag_status(1);
                        else
                            set_C_flag_status(0);
                        //FLAG_H
                        if ( (((operand_1 & 0x0F)-(operand_2 & 0x0F) ) & half_carry_8bit) == half_carry_8bit)
                            set_H_flag_status(1);
                        else
                            set_H_flag_status(0);


                        if(OPCODE == 0x96)
                        {
                            num_of_machine_cycles(2);
                        }
                        else
                        {
                            num_of_machine_cycles(1);
                        }
                        break;


                    case(0x98): case(0x99): case(0x9A):case(0x9B): case(0x9C): case(0x9D): case(0x9E): case(0x9F): //SBC A,r8
                        // the reason I'm using tmp as a backup for current register, is because I'll need it later to verify C and H flags
                        operand_1 = (*r8[A]);
                        operand_2 = *r8[(OPCODE & 0x07)];
                        (*r8[A]) = operand_1 - operand_2 - get_C_flag_status();

                        //flags
                        //FLAG_H
                        if (( ((operand_1 & 0x0F) - (operand_2 & 0x0F) - get_C_flag_status() ) & half_carry_8bit) == half_carry_8bit)
                            set_H_flag_status(1);
                        else
                            set_H_flag_status(0);
                        //FLAG_C
                        if ( operand_1 < operand_2 + get_C_flag_status())
                            set_C_flag_status(1);
                        else
                            set_C_flag_status(0);
                        set_Z_flag_status(*r8[A]);
                        set_N_flag_status(1);

                        if(OPCODE == 0x9E)
                        {
                            num_of_machine_cycles(2);
                        }
                        else
                        {
                            num_of_machine_cycles(1);
                        }
                        break;


                    case(0xA0): case(0xA1): case(0xA2):case(0xA3): case(0xA4): case(0xA5): case(0xA6): case(0xA7): //AND A,r8
                        (*r8[A]) = (*r8[A]) & (*r8[(OPCODE & 0x07)]);
                        //flags
                        set_N_flag_status(0);
                        set_H_flag_status(1);
                        set_C_flag_status(0);
                        set_Z_flag_status(*r8[A]);

                        if(OPCODE == 0xA6)
                        {
                            num_of_machine_cycles(2);
                        }
                        else
                        {
                            num_of_machine_cycles(1);
                        }
                        break;

                    case(0xA8): case(0xA9): case(0xAA):case(0xAB): case(0xAC): case(0xAD): case(0xAE): case(0xAF): //XOR A,r8
                        (*r8[A]) = ((*r8[A]) ^ (*r8[(OPCODE & 0x07)]));
                        //flags
                        set_Z_flag_status(*r8[A]);
                        set_C_flag_status(0);
                        set_H_flag_status(0);
                        set_N_flag_status(0);

                        if(OPCODE == 0xAE)
                        {
                            num_of_machine_cycles(2);
                        }
                        else
                        {
                            num_of_machine_cycles(1);
                        }
                        break;


                    case(0xB0): case(0xB1): case(0xB2):case(0xB3): case(0xB4): case(0xB5): case(0xB6): case(0xB7): //OR A,r8
                        (*r8[A]) = (*r8[A]) | (*r8[(OPCODE & 0x07)]);
                        //flags
                        set_N_flag_status(0);
                        set_H_flag_status(0);
                        set_C_flag_status(0);
                        set_Z_flag_status(*r8[A]);

                        if(OPCODE == 0xB6)
                        {
                            num_of_machine_cycles(2);
                        }
                        else
                        {
                            num_of_machine_cycles(1);
                        }
                        break;

                    case(0xC9):  //RET

                        tmp = 0;
                        tmp = tmp | (mem[r16[SP]->reg]);
                        r16[SP]->reg = (r16[SP]->reg) + 1;
                        tmp = tmp | ((mem[r16[SP]->reg]) << 8);
                        r16[SP]->reg = (r16[SP]->reg) + 1;
                        PC = tmp;

                        num_of_machine_cycles(4);
                        break;

                    case(0xD9):  //RETI

                        tmp = 0;
                        tmp = tmp | (mem[r16[SP]->reg]);
                        r16[SP]->reg = (r16[SP]->reg) + 1;
                        tmp = tmp | (mem[r16[SP]->reg]) << 8;
                        r16[SP]->reg = (r16[SP]->reg) + 1;
                        PC = tmp;
                        IME = 1;

                        num_of_machine_cycles(4);
                        break;

                    case(0xC0):  //RET NZ

                        if(!get_Z_flag_status()) {
                            tmp = 0;
                            tmp = tmp | (mem[r16[SP]->reg]);
                            r16[SP]->reg = (r16[SP]->reg) + 1;
                            tmp = tmp | (mem[r16[SP]->reg]) << 8;
                            r16[SP]->reg = (r16[SP]->reg) + 1;
                            PC = tmp;
                            num_of_machine_cycles(3);
                        }
                        num_of_machine_cycles(2);

                        break;

                    case(0xC8):  //RET Z

                        if(get_Z_flag_status()) {
                            tmp = 0;
                            tmp = tmp | (mem[r16[SP]->reg]);
                            r16[SP]->reg = (r16[SP]->reg) + 1;
                            tmp = tmp | (mem[r16[SP]->reg]) << 8;
                            r16[SP]->reg = (r16[SP]->reg) + 1;
                            PC = tmp;
                            num_of_machine_cycles(3);
                        }

                        num_of_machine_cycles(2);
                        break;

                    case(0xD0):  //RET NC

                        if(!get_C_flag_status()) {
                            tmp = 0;
                            tmp = tmp | (mem[r16[SP]->reg]);
                            r16[SP]->reg = (r16[SP]->reg) + 1;
                            tmp = tmp | (mem[r16[SP]->reg]) << 8;
                            r16[SP]->reg = (r16[SP]->reg) + 1;
                            PC = tmp;
                            num_of_machine_cycles(3);
                        }

                        num_of_machine_cycles(2);
                        break;

                    case(0xD8):  //RET C

                        if(get_C_flag_status()) {
                            tmp = 0;
                            tmp = tmp | (mem[r16[SP]->reg]);
                            r16[SP]->reg = (r16[SP]->reg) + 1;
                            tmp = tmp | (mem[r16[SP]->reg]) << 8;
                            r16[SP]->reg = (r16[SP]->reg) + 1;
                            PC = tmp;
                            num_of_machine_cycles(3);
                        }

                        num_of_machine_cycles(2);
                        break;

                    case(0xC1): case(0xD1): case(0xE1): //POP r16stk

                        tmp = (OPCODE & 0x30)>>4;
                        r16[tmp]->lo = mem[r16[SP]->reg];
                        r16[SP]->reg = (r16[SP]->reg) + 1;
                        r16[tmp]->hi = (mem[r16[SP]->reg]);
                        r16[SP]->reg = (r16[SP]->reg) + 1;

                        num_of_machine_cycles(3);
                        break;

                    case(0xF1): //POP AF,stk
                        AF_reg.lo = mem[r16[SP]->reg] & 0xF0; //4 lower bits of F are always 0
                        r16[SP]->reg = (r16[SP]->reg) + 1;
                        AF_reg.hi = (mem[r16[SP]->reg]);
                        r16[SP]->reg = (r16[SP]->reg) + 1;

                        num_of_machine_cycles(3);
                        break;

                    case(0xC5): case(0xD5): case(0xE5): //PUSH r16stk

                        tmp = (OPCODE & 0x30)>>4;
                        r16[SP]->reg = (r16[SP]->reg) - 1;
                        mem[r16[SP]->reg] = r16[tmp]->hi;
                        r16[SP]->reg = (r16[SP]->reg) - 1;
                        mem[r16[SP]->reg] = r16[tmp]->lo;

                        if(testing_mode)
                        {
                            print_memory_writes(OPCODE, r16[SP]->reg+1, r16[tmp]->hi);
                            print_memory_writes(OPCODE, r16[SP]->reg, r16[tmp]->lo);
                        }

                        check_div_reg_change(r16[SP]->reg+1);
                        check_div_reg_change(r16[SP]->reg);
                        //machine cycles update
                        num_of_machine_cycles(4);

                        break;

                    case(0xF5): //PUSH AF,stk
                        r16[SP]->reg = (r16[SP]->reg) - 1;
                        mem[r16[SP]->reg] = AF_reg.hi;
                        r16[SP]->reg = (r16[SP]->reg) - 1;
                        mem[r16[SP]->reg] = AF_reg.lo;

                        if(testing_mode)
                        {
                            print_memory_writes(OPCODE, r16[SP]->reg+1, AF_reg.hi);
                            print_memory_writes(OPCODE, r16[SP]->reg, AF_reg.lo);
                        }

                        check_div_reg_change(r16[SP]->reg+1);
                        check_div_reg_change(r16[SP]->reg);
                        //machine cycles update

                        num_of_machine_cycles(4);

                        break;

                    case(0xC2): //JP NZ, a16

                            tmp = 0;
                            tmp = tmp | (mem[PC]);
                            PC = PC + 1;
                            tmp = tmp | (mem[PC]) << 8;
                            PC = PC + 1; // May not be necessary
                        if(!get_Z_flag_status()) {
                            PC = tmp;
                            num_of_machine_cycles(1);
                        }

                        //machine cycles update

                        num_of_machine_cycles(3);

                        break;

                    case(0xCA): //JP Z, a16

                        tmp = 0;
                        tmp = tmp | (mem[PC]);
                        PC = PC + 1;
                        tmp = tmp | (mem[PC]) << 8;
                        PC = PC + 1; // May not be necessary
                        if(get_Z_flag_status()) {
                            PC = tmp;
                            num_of_machine_cycles(1);
                        }

                        //machine cycles update

                        num_of_machine_cycles(3);

                        break;

                    case(0XD2): //JP NC, a16

                        tmp = 0;
                        tmp = tmp | (mem[PC]);
                        PC = PC + 1;
                        tmp = tmp | (mem[PC]) << 8;
                        PC = PC + 1; // May not be necessary
                        if(!get_C_flag_status()) {
                            PC = tmp;
                            num_of_machine_cycles(1);
                        }

                        //machine cycles update

                        num_of_machine_cycles(3);

                        break;

                    case(0xDA): //JP C, a16

                        tmp = 0;
                        tmp = tmp | (mem[PC]);
                        PC = PC + 1;
                        tmp = tmp | (mem[PC]) << 8;
                        PC = PC + 1; // May not be necessary
                        if(get_C_flag_status()) {
                            PC = tmp;
                            num_of_machine_cycles(1);
                        }

                        //machine cycles update

                        num_of_machine_cycles(3);

                        break;


                    case(0xC3): //JP a16

                        tmp = 0;
                        tmp = tmp | mem[PC];
                        PC = PC + 1;
                        tmp = tmp | (mem[PC] << 8);
                        PC = PC + 1; // May not be necessary
                        PC = tmp;

                        //machine cycles update

                        num_of_machine_cycles(4);

                        break;

                    case(0xE9): //JP HL
                        PC = r16[HL_16]->reg;

                        //machine cycles update

                        num_of_machine_cycles(1);

                        break;


                    case(0xCD): //CALL C, a16
                        tmp = 0;
                        tmp = tmp | (mem[PC]);
                        PC = PC + 1;
                        tmp = tmp | (mem[PC]) << 8;
                        PC = PC + 1; // May not be necessary
                        r16[SP]->reg = (r16[SP]->reg) - 1;
                        mem[r16[SP]->reg] = (BYTE) (0X00FF & ((PC) >> 8));
                        r16[SP]->reg = (r16[SP]->reg) - 1;
                        mem[r16[SP]->reg] = (BYTE) (0X00FF & (PC));
                        PC = tmp;

                        if(testing_mode)
                        {
                            print_memory_writes(OPCODE, r16[SP]->reg+1, (BYTE) (BYTE) (0X00FF & ((PC) >> 8)));
                            print_memory_writes(OPCODE, r16[SP]->reg, (BYTE) (0X00FF & (PC)));
                        }
                        check_div_reg_change(r16[SP]->reg);
                        check_div_reg_change(r16[SP]->reg+1);

                        //machine cycles update

                        num_of_machine_cycles(6);

                        break;

                    case(0xC4): //CALL NZ, a16
                        tmp = 0;
                        tmp = tmp | (mem[PC]);
                        PC = PC + 1;
                        tmp = tmp | (mem[PC]) << 8;
                        PC = PC + 1; // May not be necessary
                        if(!get_Z_flag_status()){
                            r16[SP]->reg = (r16[SP]->reg) - 1;
                            mem[r16[SP]->reg] = (BYTE) (0X00FF & ((PC) >> 8));
                            r16[SP]->reg = (r16[SP]->reg) - 1;
                            mem[r16[SP]->reg] = (BYTE) (0X00FF & (PC));
                            PC = tmp;

                            if(testing_mode)
                            {
                                print_memory_writes(OPCODE, r16[SP]->reg+1, (BYTE) (BYTE) (0X00FF & ((PC) >> 8)));
                                print_memory_writes(OPCODE, r16[SP]->reg, (BYTE) (0X00FF & (PC)));
                            }
                            check_div_reg_change(r16[SP]->reg);
                            check_div_reg_change(r16[SP]->reg+1);

                            num_of_machine_cycles(3);

                        }

                        //machine cycles update

                        num_of_machine_cycles(3);

                        break;


                    case(0xCC): //CALL Z, a16
                        tmp = 0;
                        tmp = tmp | (mem[PC]);
                        PC = PC + 1;
                        tmp = tmp | (mem[PC]) << 8;
                        PC = PC + 1; // May not be necessary
                        if(get_Z_flag_status()){
                            r16[SP]->reg = (r16[SP]->reg) - 1;
                            mem[r16[SP]->reg] = (BYTE) (0X00FF & ((PC) >> 8));
                            r16[SP]->reg = (r16[SP]->reg) - 1;
                            mem[r16[SP]->reg] = (BYTE) (0X00FF & (PC));
                            PC = tmp;

                            if(testing_mode)
                            {
                                print_memory_writes(OPCODE, r16[SP]->reg+1, (BYTE) (BYTE) (0X00FF & ((PC) >> 8)));
                                print_memory_writes(OPCODE, r16[SP]->reg, (BYTE) (0X00FF & (PC)));
                            }
                            check_div_reg_change(r16[SP]->reg);
                            check_div_reg_change(r16[SP]->reg+1);

                            num_of_machine_cycles(3);
                        }

                        //machine cycles update

                        num_of_machine_cycles(3);

                        break;

                    case(0xD4): //CALL NC, a16
                        tmp = 0;
                        tmp = tmp | (mem[PC]);
                        PC = PC + 1;
                        tmp = tmp | (mem[PC]) << 8;
                        PC = PC + 1; // May not be necessary
                        if(!get_C_flag_status()){
                            r16[SP]->reg = (r16[SP]->reg) - 1;
                            mem[r16[SP]->reg] = (BYTE) (0X00FF & ((PC) >> 8));
                            r16[SP]->reg = (r16[SP]->reg) - 1;
                            mem[r16[SP]->reg] = (BYTE) (0X00FF & (PC));
                            PC = tmp;

                            if(testing_mode)
                            {
                                print_memory_writes(OPCODE, r16[SP]->reg+1, (BYTE) (BYTE) (0X00FF & ((PC) >> 8)));
                                print_memory_writes(OPCODE, r16[SP]->reg, (BYTE) (0X00FF & (PC)));
                            }
                            check_div_reg_change(r16[SP]->reg);
                            check_div_reg_change(r16[SP]->reg+1);

                            num_of_machine_cycles(3);
                        }

                        //machine cycles update

                        num_of_machine_cycles(3);

                        break;

                    case(0xDC): //CALL C, a16
                        tmp = 0;
                        tmp = tmp | (mem[PC]);
                        PC = PC + 1;
                        tmp = tmp | (mem[PC]) << 8;
                        PC = PC + 1; // May not be necessary
                        if(get_C_flag_status()){
                            r16[SP]->reg = (r16[SP]->reg) - 1;
                            mem[r16[SP]->reg] = (BYTE) (0X00FF & ((PC) >> 8));
                            r16[SP]->reg = (r16[SP]->reg) - 1;
                            mem[r16[SP]->reg] = (BYTE) (0X00FF & (PC));
                            PC = tmp;

                            if(testing_mode)
                            {
                                print_memory_writes(OPCODE, r16[SP]->reg+1, (BYTE) (BYTE) (0X00FF & ((PC) >> 8)));
                                print_memory_writes(OPCODE, r16[SP]->reg, (BYTE) (0X00FF & (PC)));
                            }
                            check_div_reg_change(r16[SP]->reg);
                            check_div_reg_change(r16[SP]->reg+1);

                            num_of_machine_cycles(3);
                        }

                        //machine cycles update

                        num_of_machine_cycles(3);

                        break;

                    case(0xC7): case(0xD7): case(0xE7): case(0xF7):
                    case(0xCF): case(0xDF): case(0xEF): case(0xFF)://RST tgt3

                        tmp = (OPCODE & 0x38);
                        r16[SP]->reg = (r16[SP]->reg) - 1;
                        mem[r16[SP]->reg] = (BYTE) (0X00FF & ((PC) >> 8));
                        r16[SP]->reg = (r16[SP]->reg) - 1;
                        mem[r16[SP]->reg] = (BYTE) (0X00FF & (PC));
                        PC = tmp;

                        if(testing_mode)
                        {
                            print_memory_writes(OPCODE, r16[SP]->reg+1, (BYTE) (0X00FF & ((PC) >> 8)));
                            print_memory_writes(OPCODE, r16[SP]->reg, (BYTE) (0X00FF & (PC)));
                        }
                        check_div_reg_change(r16[SP]->reg);
                        check_div_reg_change(r16[SP]->reg+1);

                        //machine cycles update

                        num_of_machine_cycles(4);

                        break;

                    case(0xC6): //ADD A, d8
                        tmp = (*r8[A]); //backup for flag calculation
                        (*r8[A]) = (*r8[A]) + mem[PC];
                        //PC++;
                        //NOTE that I'm delaying the PC increment (although it should be immediately after) so I can calculate the flags beforehand

                        //FLAG_C
                        if ((tmp + mem[PC]) > UCHAR_MAX)
                            set_C_flag_status(1); //should turn on FLAG_CARRY
                        else
                            set_C_flag_status(0); //should turn OFF FLAG_CARRY
                        //FLAG_H
                        if ( ( (((tmp) & 0x0F)+((mem[PC]) & 0x0F) ) & half_carry_8bit) == half_carry_8bit)
                            set_H_flag_status(1); //should turn on FLAG_HALF
                        else
                            set_H_flag_status(0); //should turn OFF FLAG_HALF
                        PC++;
                        //flags
                        set_N_flag_status(0); //OFF
                        set_Z_flag_status((*r8[A]));

                        //machine cycles update

                        num_of_machine_cycles(2);

                        break;

                    case(0xCE): //ADC A,d8
                        operand_1 = (*r8[A]);
                        operand_2 = mem[PC];
                        (*r8[A]) = operand_1 + operand_2 + get_C_flag_status();
                        PC++;

                        //flags
                        set_N_flag_status(0);
                        set_Z_flag_status(*r8[A]);
                        //FLAG_H
                        if ( ( ( ( (operand_1 & 0x0F) + get_C_flag_status() + (operand_2 & 0x0F) ) & half_carry_8bit) == half_carry_8bit))
                            set_H_flag_status(1);
                        else
                            set_H_flag_status(0);
                        //FLAG_C
                        if (operand_1 + operand_2 + get_C_flag_status() > UCHAR_MAX)
                            set_C_flag_status(1);
                        else
                            set_C_flag_status(0);

                        //machine cycles update

                        num_of_machine_cycles(2);

                        break;

                    case(0xDE): //SBC A,d8
                        operand_1 = (*r8[A]);
                        operand_2 = mem[PC];
                        (*r8[A]) = operand_1 - operand_2 - get_C_flag_status();
                        PC++;

                        //flags
                        //FLAG_H
                        if ( ( ((operand_1 & 0x0F) - get_C_flag_status() - (operand_2 & 0x0F) ) & half_carry_8bit) == half_carry_8bit)
                            set_H_flag_status(1);
                        else
                            set_H_flag_status(0);
                        //FLAG_C
                        if ( operand_1 < operand_2 + get_C_flag_status() )
                            set_C_flag_status(1);
                        else
                            set_C_flag_status(0);

                        set_N_flag_status(1);
                        set_Z_flag_status(*r8[A]);

                        //machine cycles update

                        num_of_machine_cycles(2);

                        break;

                    case(0xD6): //SUB A, d8
                        operand_1 = (*r8[A]); //backup for flag calculation
                        operand_2 = mem[PC];
                        (*r8[A]) = operand_1 - operand_2;
                        PC++;

                        //flags
                        //FLAG_C
                        if ( operand_1 < operand_2) // credit: https://stackoverflow.com/questions/31409444/what-is-the-behavior-of-the-carry-flag-for-cp-on-a-game-boy/31415312
                            set_C_flag_status(1);
                        else
                            set_C_flag_status(0);
                        //FLAG_H
                        if (( (((operand_1) & 0x0F) - (operand_2 & 0x0F) ) & half_carry_8bit) == half_carry_8bit)
                            set_H_flag_status(1);
                        else
                            set_H_flag_status(0);

                        set_N_flag_status(1);
                        set_Z_flag_status(*r8[A]);

                        //machine cycles update

                        num_of_machine_cycles(2);

                        break;

                    case(0xE6): //AND A, d8
                        (*r8[A]) = (*r8[A]) & mem[PC];
                        PC++;
                        //flags
                        set_N_flag_status(0); //OFF
                        //FLAG_C
                        set_C_flag_status(0);
                        //FLAG_H
                        set_H_flag_status(1); //ON
                        set_Z_flag_status(*r8[A]);

                        //machine cycles update

                        num_of_machine_cycles(2);

                        break;

                    case(0xEE): //XOR A, d8
                        (*r8[A]) = (*r8[A]) ^ mem[PC];
                        PC++;
                        //FLAGS
                        //FLAG_C
                        set_C_flag_status(0);
                        set_H_flag_status(0);
                        set_N_flag_status(0);
                        set_Z_flag_status(*r8[A]);

                        //machine cycles update

                        num_of_machine_cycles(2);

                        break;

                    case(0xF6): //OR A, d8
                        (*r8[A]) = (*r8[A]) | mem[PC];
                        PC++;
                        //flags
                        set_Z_flag_status(*r8[A]);
                        set_C_flag_status(0);
                        set_H_flag_status(0);
                        set_N_flag_status(0);

                        //machine cycles update

                        num_of_machine_cycles(2);

                        break;


                    case(0xFE): //CP A, d8
                        operand_1 = *r8[A];
                        operand_2 = mem[PC];
                        tmp_uChar = operand_1 - operand_2;
                        PC++;

                        //flags
                        //FLAG_C
                        if ( operand_1 < operand_2 )
                            set_C_flag_status(1);
                        else
                            set_C_flag_status(0);
                        //FLAG_H
                        if ( ( ((operand_1 & 0x0F) - (operand_2 & 0x0F) ) & half_carry_8bit) == half_carry_8bit)
                            set_H_flag_status(1); //should turn on FLAG_H
                        else
                            set_H_flag_status(0); //should turn OFF FLAG_H

                        set_Z_flag_status(tmp_uChar);
                        set_N_flag_status(1);

                        //machine cycles update

                        num_of_machine_cycles(2);

                        break;




                    case(0xB8): case(0xB9): case(0xBA):case(0xBB): case(0xBC): case(0xBD): case(0xBE): case(0xBF): //CP A,r8
                        operand_1 = *r8[A];
                        operand_2 = *r8[(OPCODE & 0x07)];
                        tmp_uChar = (*r8[A]) - (*r8[(OPCODE & 0x07)]);

                        //flags
                        set_Z_flag_status(tmp_uChar);
                        set_N_flag_status(1);
                        //FLAG_C
                        if ( operand_1 < operand_2)
                            set_C_flag_status(1);
                        else
                            set_C_flag_status(0);
                        //FLAG_H
                        if ( ( ((operand_1 & 0x0F) - (operand_2 & 0x0F) ) & half_carry_8bit) == half_carry_8bit)
                            set_H_flag_status(1);
                        else
                            set_H_flag_status(0);

                        //machine cycles update
                        if(OPCODE == 0xBE)
                        {
                            num_of_machine_cycles(2);
                        }
                        else
                        {
                            num_of_machine_cycles(1);
                        }

                        break;


                        //16 bit commands
                        //BIT command
                    case(0xCB40): case(0xCB41): case(0xCB42): case(0xCB43): case(0xCB44): case(0xCB45): case(0xCB46): case(0xCB47): case(0xCB48): case(0xCB49): case(0xCB4A): case(0xCB4B): case(0xCB4C): case(0xCB4D): case(0xCB4E): case(0xCB4F):
                    case(0xCB50): case(0xCB51): case(0xCB52): case(0xCB53): case(0xCB54): case(0xCB55): case(0xCB56): case(0xCB57): case(0xCB58): case(0xCB59): case(0xCB5A): case(0xCB5B): case(0xCB5C): case(0xCB5D): case(0xCB5E): case(0xCB5F):
                    case(0xCB60): case(0xCB61): case(0xCB62): case(0xCB63): case(0xCB64): case(0xCB65): case(0xCB66): case(0xCB67): case(0xCB68): case(0xCB69): case(0xCB6A): case(0xCB6B): case(0xCB6C): case(0xCB6D): case(0xCB6E): case(0xCB6F):
                    case(0xCB70): case(0xCB71): case(0xCB72): case(0xCB73): case(0xCB74): case(0xCB75): case(0xCB76): case(0xCB77): case(0xCB78): case(0xCB79): case(0xCB7A): case(0xCB7B): case(0xCB7C): case(0xCB7D): case(0xCB7E): case(0xCB7F):
                        tmp_uChar = ((OPCODE & 0x38)>>3);
                        ///SHOULD BE THE Z-FLAG OF THE PROGRAM STATUS REGISTER / STAT REGISTER. UNSURE WHAT IT IS FOR NOW, TO-CHECK!~~~~~
                        if((0x01 & (*r8[(OPCODE & 0x07)] >> tmp_uChar)) == 0) //input the complement of the given bit into the Z_FLAG
                            set_Z_flag_status(0); //ON
                        else
                            set_Z_flag_status(1); //OFF
                        set_H_flag_status(1);
                        set_N_flag_status(0);

                        //machine cycles update
                        if(OPCODE == 0xCB4E || OPCODE == 0xCB5E || OPCODE == 0xCB6E || OPCODE == 0xCB7E || OPCODE == 0xCB46 || OPCODE == 0xCB56 || OPCODE == 0xCB66 || OPCODE == 0xCB76)
                        {
                            num_of_machine_cycles(3);
                        }
                        else
                        {
                            num_of_machine_cycles(2);
                        }

                        break;

                        //RES (reset)
                    case(0xCB80): case(0xCB81): case(0xCB82): case(0xCB83): case(0xCB84): case(0xCB85): case(0xCB86): case(0xCB87): case(0xCB88): case(0xCB89): case(0xCB8A): case(0xCB8B): case(0xCB8C): case(0xCB8D): case(0xCB8E): case(0xCB8F):
                    case(0xCB90): case(0xCB91): case(0xCB92): case(0xCB93): case(0xCB94): case(0xCB95): case(0xCB96): case(0xCB97): case(0xCB98): case(0xCB99): case(0xCB9A): case(0xCB9B): case(0xCB9C): case(0xCB9D): case(0xCB9E): case(0xCB9F):
                    case(0xCBA0): case(0xCBA1): case(0xCBA2): case(0xCBA3): case(0xCBA4): case(0xCBA5): case(0xCBA6): case(0xCBA7): case(0xCBA8): case(0xCBA9): case(0xCBAA): case(0xCBAB): case(0xCBAC): case(0xCBAD): case(0xCBAE): case(0xCBAF):
                    case(0xCBB0): case(0xCBB1): case(0xCBB2): case(0xCBB3): case(0xCBB4): case(0xCBB5): case(0xCBB6): case(0xCBB7): case(0xCBB8): case(0xCBB9): case(0xCBBA): case(0xCBBB): case(0xCBBC): case(0xCBBD): case(0xCBBE): case(0xCBBF):

                        tmp = ((OPCODE & 0x38)>>3);
                        *r8[(OPCODE & 0x07)] = (*r8[(OPCODE & 0x07)] & (BYTE)(~(1 << tmp)));

                        //machine cycles update
                        if(OPCODE == 0xCB8E || OPCODE == 0xCB9E || OPCODE == 0xCBAE || OPCODE == 0xCBBE || OPCODE == 0xCB86 || OPCODE == 0xCB96 || OPCODE == 0xCBA6 || OPCODE == 0xCBB6)
                        {
                            num_of_machine_cycles(4);
                        }
                        else
                        {
                            num_of_machine_cycles(2);
                        }

                        break;

                        //SET
                    case(0xCBC0): case(0xCBC1): case(0xCBC2): case(0xCBC3): case(0xCBC4): case(0xCBC5): case(0xCBC6): case(0xCBC7): case(0xCBC8): case(0xCBC9): case(0xCBCA): case(0xCBCB): case(0xCBCC): case(0xCBCD): case(0xCBCE): case(0xCBCF):
                    case(0xCBD0): case(0xCBD1): case(0xCBD2): case(0xCBD3): case(0xCBD4): case(0xCBD5): case(0xCBD6): case(0xCBD7): case(0xCBD8): case(0xCBD9): case(0xCBDA): case(0xCBDB): case(0xCBDC): case(0xCBDD): case(0xCBDE): case(0xCBDF):
                    case(0xCBE0): case(0xCBE1): case(0xCBE2): case(0xCBE3): case(0xCBE4): case(0xCBE5): case(0xCBE6): case(0xCBE7): case(0xCBE8): case(0xCBE9): case(0xCBEA): case(0xCBEB): case(0xCBEC): case(0xCBED): case(0xCBEE): case(0xCBEF):
                    case(0xCBF0): case(0xCBF1): case(0xCBF2): case(0xCBF3): case(0xCBF4): case(0xCBF5): case(0xCBF6): case(0xCBF7): case(0xCBF8): case(0xCBF9): case(0xCBFA): case(0xCBFB): case(0xCBFC): case(0xCBFD): case(0xCBFE): case(0xCBFF):

                        tmp = ((OPCODE & 0x38)>>3);
                        *r8[(OPCODE & 0x07)] = (*r8[(OPCODE & 0x07)] | (BYTE)((1 << tmp)));

                        //machine cycles update
                        if(OPCODE == 0xCBCE || OPCODE == 0xCBDE || OPCODE == 0xCBEE || OPCODE == 0xCBFE || OPCODE == 0xCBC6 || OPCODE == 0xCBD6 || OPCODE == 0xCBE6 || OPCODE == 0xCBF6)
                        {
                            num_of_machine_cycles(4);
                        }
                        else
                        {
                            num_of_machine_cycles(2);
                        }

                        break;


                    case(0xCB00): case(0xCB01): case(0xCB02): case(0xCB03): case(0xCB04): case(0xCB05): case(0xCB06): case(0xCB07): //RLC(rotate left carry) - bits 0 1 2 - operand
                        tmp = 0;
                        tmp = (*r8[(OPCODE & 0x07)] & 0x80) >> 7; //msb - but shifted to lsb
                        *r8[(OPCODE & 0x07)] = (*r8[(OPCODE & 0x07)] << 1); //shift all bits to the left
                        *r8[(OPCODE & 0x07)] = *r8[(OPCODE & 0x07)] | (tmp); //shift msb into lsb

                        //flags
                        set_Z_flag_status(*r8[(OPCODE & 0x07)]);
                        set_N_flag_status(0);
                        set_H_flag_status(0);
                        set_C_flag_status(tmp); //tmp is either 1 or 0 at this point

                        //machine cycles update
                        if(OPCODE == 0xCB06)
                        {
                            num_of_machine_cycles(4);
                        }
                        else
                        {
                            num_of_machine_cycles(2);
                        }

                        break;

                    case(0xCB08): case(0xCB09): case(0xCB0A): case(0xCB0B): case(0xCB0C): case(0xCB0D): case(0xCB0E): case(0xCB0F): //RRC(rotate right carry) - bits 0 1 2 - operand
                        tmp = 0;
                        tmp = (*r8[(OPCODE & 0x07)] & 0x01); //lsb
                        *r8[(OPCODE & 0x07)] = (*r8[(OPCODE & 0x07)] >> 1); //shift all bits to the right
                        *r8[(OPCODE & 0x07)] = *r8[(OPCODE & 0x07)] | ((tmp) << 7); //shift lsb into msb

                        //flags
                        set_Z_flag_status(*r8[(OPCODE & 0x07)]);
                        set_N_flag_status(0);
                        set_H_flag_status(0);
                        set_C_flag_status(tmp); //tmp is either 1 or 0 at this point

                        //machine cycles update
                        if(OPCODE == 0xCB0E)
                        {
                            num_of_machine_cycles(4);
                        }
                        else
                        {
                            num_of_machine_cycles(2);
                        }

                        break;

                    case(0xCB10): case(0xCB11): case(0xCB12): case(0xCB13): case(0xCB14): case(0xCB15): case(0xCB16): case(0xCB17): //RL (rotate left) - bits 0 1 2 - operand
                        tmp = 0;
                        tmp = (*r8[(OPCODE & 0x07)] & 0x80); //msb
                        *r8[(OPCODE & 0x07)] = (*r8[(OPCODE & 0x07)] << 1); //shift all bits to the left
                        *r8[(OPCODE & 0x07)] = *r8[(OPCODE & 0x07)] | get_C_flag_status(); //shift carry flag into lsb

                        //flags
                        set_Z_flag_status(*r8[(OPCODE & 0x07)]);
                        set_N_flag_status(0);
                        set_H_flag_status(0);
                        set_C_flag_status(tmp); //tmp is either 0x80 or 0 at this point (we only care if its 0 or not)

                        //machine cycles update
                        if(OPCODE == 0xCB16)
                        {
                            num_of_machine_cycles(4);
                        }
                        else
                        {
                            num_of_machine_cycles(2);
                        }

                        break;

                    case(0xCB18): case(0xCB19): case(0xCB1A): case(0xCB1B): case(0xCB1C): case(0xCB1D): case(0xCB1E): case(0xCB1F): //RR (rotate right) - bits 0 1 2 - operand
                        tmp = 0;
                        tmp = (*r8[(OPCODE & 0x07)] & 0x01); //lsb
                        *r8[(OPCODE & 0x07)] = (*r8[(OPCODE & 0x07)] >> 1); //shift all bits to the right
                        *r8[(OPCODE & 0x07)] = *r8[(OPCODE & 0x07)] | (get_C_flag_status()<<7); //shift carry flag into lsb

                        //flags
                        set_Z_flag_status(*r8[(OPCODE & 0x07)]);
                        set_N_flag_status(0);
                        set_H_flag_status(0);
                        set_C_flag_status(tmp); //tmp is either 1 or 0 at this point

                        //machine cycles update
                        if(OPCODE == 0xCB1E)
                        {
                            num_of_machine_cycles(4);
                        }
                        else
                        {
                            num_of_machine_cycles(2);
                        }

                        break;

                    case(0xCB20): case(0xCB21): case(0xCB22): case(0xCB23): case(0xCB24): case(0xCB25): case(0xCB26): case(0xCB27): //SLA - shift left arithmetic
                        tmp = 0;
                        tmp = (*r8[(OPCODE & 0x07)] & 0x80); //msb
                        *r8[(OPCODE & 0x07)] = (*r8[(OPCODE & 0x07)] << 1); //shift all bits to the left
                        *r8[(OPCODE & 0x07)] = *r8[(OPCODE & 0x07)] & 0xFE; //reset bit 0

                        //flags
                        set_Z_flag_status(*r8[(OPCODE & 0x07)]);
                        set_N_flag_status(0);
                        set_H_flag_status(0);
                        set_C_flag_status(tmp); //tmp is either 1 or 0 at this point

                        //machine cycles update
                        if(OPCODE == 0xCB26)
                        {
                            num_of_machine_cycles(4);
                        }
                        else
                        {
                            num_of_machine_cycles(2);
                        }

                        break;

                    case(0xCB28): case(0xCB29): case(0xCB2A): case(0xCB2B): case(0xCB2C): case(0xCB2D): case(0xCB2E): case(0xCB2F): //SRA - shift right arithmetic
                        tmp = 0;
                        tmp_uChar = (*r8[(OPCODE & 0x07)] & 0x80); //msb
                        tmp = (*r8[(OPCODE & 0x07)] & 0x01); //lsb
                        *r8[(OPCODE & 0x07)] = ((char)(*r8[(OPCODE & 0x07)]) >> 1); //shift all bits to the right


//                        if(tmp_uChar == 0)
//                        {
//                            *r8[(OPCODE & 0x07)] = (*r8[(OPCODE & 0x07)] & (BYTE)(~(1 << 7))); //should turn off bit 7
//                        }
//                        else
//                        {
//                            *r8[(OPCODE & 0x07)] = (*r8[(OPCODE & 0x07)] | (BYTE)(1 << 7)); //should turn on bit 7
//                        }

                        //new bit 7 is unchanged as per the instructions, although it should automatically default to 0 since unsigned

                        //flags
                        set_Z_flag_status(*r8[(OPCODE & 0x07)]);
                        set_N_flag_status(0);
                        set_H_flag_status(0);
                        set_C_flag_status(tmp); //tmp is either 1 or 0 at this point

                        //machine cycles update
                        if(OPCODE == 0xCB2E)
                        {
                            num_of_machine_cycles(4);
                        }
                        else
                        {
                            num_of_machine_cycles(2);
                        }

                        break;

                    case(0xCB30): case(0xCB31): case(0xCB32): case(0xCB33): case(0xCB34): case(0xCB35): case(0xCB36): case(0xCB37): //SWAP
                        tmp = 0;
                        tmp = (BYTE)(*r8[(OPCODE & 0x07)] << 4) & 0x00F0; //shift lower nibble to upper, and erase everything else
                        *r8[(OPCODE & 0x07)] = (*r8[(OPCODE & 0x07)] >> 4);//shift four times to make the upper nibble - low
                        *r8[(OPCODE & 0x07)] = *r8[(OPCODE & 0x07)] | tmp;
                        //flags
                        set_Z_flag_status(*r8[(OPCODE & 0x07)]);
                        set_N_flag_status(0);
                        set_H_flag_status(0);
                        set_C_flag_status(0); //tmp is either 1 or 0 at this point

                        //machine cycles update
                        if(OPCODE == 0xCB36)
                        {
                            num_of_machine_cycles(4);
                        }
                        else
                        {
                            num_of_machine_cycles(2);
                        }

                        break;

                    case(0xCB38): case(0xCB39): case(0xCB3A): case(0xCB3B): case(0xCB3C): case(0xCB3D): case(0xCB3E): case(0xCB3F): //SRL
                        tmp = 0;
                        tmp = (*r8[(OPCODE & 0x07)] & 0x01); //lsb
                        *r8[(OPCODE & 0x07)] = (*r8[(OPCODE & 0x07)] >> 1); //shift all bits to the right
                        *r8[(OPCODE & 0x07)] = *r8[(OPCODE & 0x07)] & 0x7F; //this is what differentiates SRL and SRA, explicit 7th bit reset

                        //flags
                        set_Z_flag_status(*r8[(OPCODE & 0x07)]);
                        set_N_flag_status(0);
                        set_H_flag_status(0);
                        set_C_flag_status(tmp); //tmp is either 1 or 0 at this point

                        //machine cycles update
                        if(OPCODE == 0xCB3E)
                        {
                            num_of_machine_cycles(4);
                        }
                        else
                        {
                            num_of_machine_cycles(2);
                        }

                        break;

                    default:
                        //cout << std::hex  << OPCODE << std::dec << '\n';
                        //cout << "Loop Counter: " << loop_counter << '\n';
                        break;


                }
            }
            void main_loop()
            {
                //read_from_file("../test.bin");

                memset(mem,0,sizeof(mem));
                //tester, gameboy cartridge, 0x100 offset and all
                //checklist of tests
                // 01-special.gb - VV
                // 02-interrupts.gb
                // 03-op sp,hl.gb - V
                // 04-op r,imm.gb - VV
                // 05-op rp.gb - VV
                // 06-ld r,r.gb - VV
                // 07-jr,jp,call,ret,rst.gb - VV
                // 08-misc instrs.gb - VV
                // 09-op r,r.gb
                // 10-bit ops.gb - VV
                // 11-op a,(hl).gb
                read_from_file("../TESTS/02-interrupts.gb");


                //bootstrap rom, 0x0 offset
                //read_from_file("../TESTS/DMG_ROM.bin");
                init();
                if(testing_mode)
                {
//                    init_register_file();
                    gbdoctor_init_register_file();
                    init_memory_file();
                }

                //for testing
                BYTE test_output_SB = mem[SB_reg];
                BYTE test_output_SC = mem[SC_reg];



                chrono::duration<double> time_span = chrono::milliseconds(0);
                while(true)
                {
                    chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
                    if(testing_mode && (is_halted != 2) || (( mem[IF_reg] & mem[IE_reg] ) != 0)) { //either we are not halted, or we are about to exit halt_mode and the opcode will run
//                        print_registers_r8(); //for testing
                        gbdoctor_print_registers_r8();
                    }

                    //interrupt_mode ? post_interrupt(): check_interrupts(); //if interrupt mode is on, we return to normal with post_interrupt(), otherwise we check for interrupts

                    if(gb_machine_cycles < max_machine_cycles_val) //1 million microseconds = 1 second
                    {
                        unsigned int gb_machine_cycles_prev = gb_machine_cycles;
                        check_interrupts();
                        if(is_halted > 0) //if HALT command was executed, first cycle we are in intermittent mode, and the cycle after that halts execution fully
                        {
                            if(is_halted == 2) //is halt in full effect yet
                            {
                                update_timers(1,1); //paused, 1 machine clock is placeholder
                                continue;
                            }
                            else //if it wasn't, it will be now
                                is_halted = 2;


                        }
                        fetch();
                        decode_execute();
                        //ought to fix the value so it doesnt become negative at any point
                        unsigned int machine_cycle_cost_iter = gb_machine_cycles - gb_machine_cycles_prev;

                        //sanity check
                        if(machine_cycle_cost_iter < 0)
                        {
                            cerr << "Error: Negative cycle cost this iteration\n" << endl;
                            exit(1);
                        }
                        update_timers(machine_cycle_cost_iter,0); //regular

                        //reset machine cycles every second
                        chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
                        time_span += duration_cast<chrono::duration<double>>(t2 - t1);
                        if(time_span >= chrono::milliseconds (1000))
                        {
                            //cout << "hi!";
                            time_span -= chrono::seconds(1);
                            gb_machine_cycles = 0; //may change to max(0,curr_val-max_val)
                        }

                        loop_counter++;
                    }
                    else //wait until the remainder of the second passes
                    {
                        //cout << "hi!";
                        this_thread::sleep_for((chrono::seconds(1)-time_span));
                        gb_machine_cycles = 0; //may change to max(0,curr_val-max_val)

                    }

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
