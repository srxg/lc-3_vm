#include "lc3_vm_win.h"

HANDLE hStdin = INVALID_HANDLE_VALUE;
DWORD fdwMode, fdwOldMode;

void disable_input_buffering()
{
    hStdin = GetStdHandle(STD_INPUT_HANDLE);

    GetConsoleMode(hStdin, &fdwOldMode); // save the old mode

    fdwMode = fdwOldMode ^ ENABLE_ECHO_INPUT ^ ENABLE_LINE_INPUT;

    SetConsoleMode(hStdin, fdwMode); // start new mode
    FlushConsoleInputBuffer(hStdin); // clear buffer
}

void restore_input_buffering()
{
    SetConsoleMode(hStdin, fdwOldMode);
}

uint16_t check_key()
{
    return WaitForSingleObject(hStdin, 1000) == WAIT_OBJECT_0 && _kbhit();
}

void handle_interrupt(int sig)
{
    restore_input_buffering();
    printf("\n");
    exit(-2);
}

uint16_t sign_extend(uint16_t x, int bit_count)
{
    if ((x >> (bit_count - 1)) & 1) {
        x |= (0xFFFF << bit_count);
    }
    return x;
}

uint16_t swap16(uint16_t x)
{
    return (x << 8) | (x >> 8);
}

/*Any time a value is written to a register
 we need to update the flags to indicate its sign.
We will write a function so that this can be reused:*/
void update_flags(uint16_t r)
{ // we COULD use a char because we only have 10 registers but uint16_t is used for consistency
    if (reg[r] == 0)
        reg[R_COND] = FL_ZRO;
    /* a 1 in the left-most bit indicates negative */
    else if (reg[r] >> (BIT_LENGTH - 1))
    {
        reg[R_COND] = FL_NEG;
    }
    else
    {
        reg[R_COND] = FL_POS;
    }
}


// The first 16 bits of the program file specify the address in memory where the program should start
// This address is called the **origin**
// the first 16 bits of the program file specify the address in memory
// where the program should start
void read_img_file(FILE *file)
{
    uint16_t origin;
    fread(&origin, sizeof(origin), 1, file);
    origin = swap16(origin);

    uint16_t max_read = MEMORY_MAX - origin;

    uint16_t *p = memory + origin;
    size_t read = fread(p, sizeof(uint16_t), max_read, file);

    // change to little endian
    while (read-- > 0)
    {
        *p = swap16(*p);
        ++p;
    }
}

// takes a string of the path of the image file
// and calls read_img_file on the file
int read_img(const char* img_path)
{
    FILE *file = fopen(img_path, "rb");
    if (!file) return 0;
    read_img_file(file);
    fclose(file);
    return 1;
}

// "Memory mapped registers make memory access a bit more
// complicated. We can't read and write to the memory array directly,
// but must instead call setter and getter functions".
// When memory is read from KBSR, the getter will check the keyboard
// and update both memory locations
void mem_write(uint16_t addr, uint16_t val)
{
    memory[addr] = val;
}

uint16_t mem_read(uint16_t addr)
{
    if (addr == MR_KBSR)
    {
        if (check_key())
        {
            memory[MR_KBSR] = (1 << 15); // set to 1 when a key has been pressed
            memory[MR_KBDR] = getchar();
        }
        else
        {
            memory[MR_KBSR] = 0;
        }
    }
    return memory[addr];
}


int main(int argc, const char *argv[])
{
    // load the argument
    if (argc < 2)
    {
        printf("lc3_vm_win [image-file1] ...\n");
        exit(2);
    }

    for (int i = 1; i < argc; ++i)
    {
        if (!read_img(argv[i]))
        {
            printf("Failed to load image: %s\n", argv[i]);
            exit(1);
        }
    }

    // setup
    signal(SIGINT, handle_interrupt);
    disable_input_buffering();
    // Exactly one condition flag should be set at any given time
    // so, seems like it would be best to just set the Z(ero) flag.
    reg[R_COND] = FL_ZRO;

    /**
     * Set the PC to the starting position -
     * 0x3000 is the default, starting address for user programs.
     */

    // i don't understand why an enum's being used here? seems unusual/a little redundant?
    enum
    {
        PC_START = 0x3000 // DEFAULT_START_ADDR
    };

    // ask yourself what's the pc doing? it contains
    // the mem address of the next instruction to execute
    reg[R_PC] = PC_START;

    int running = 1;
    while (running)
    {
        // fetch
        uint16_t instruction = mem_read(reg[R_PC]++);
        uint16_t opcode = instruction >> 12;

        switch (opcode)
        {
            case OP_ADD:
                {
                    // the destination register!
                    /**
                     * move the 3-bits specifying the destination register
                     * down to bits 0, 1 and 2
                     * isolate them by anding with 111 (7)
                     * do the same for sr1!
                     */
                    uint16_t dr = (instruction >> 9) & 0x7;
                    uint16_t sr1 = (instruction >> 6) & 0x7;

                    // are we in immediate mode?
                    uint16_t imm_flag = (instruction >> 5) & 0x1;
                    if (imm_flag)
                    {
                        // we're in immediate mode
                        uint16_t imm5 = sign_extend(instruction & 0x1F, 5);
                        reg[dr] = reg[sr1] + imm5;
                    }
                    else
                    {
                        // we're in register mode
                        uint16_t sr2 = instruction & 0x7;
                        reg[dr] = reg[sr1] + reg[sr2];
                    }

                    update_flags(dr);
                }
                break;
            case OP_AND:
                {
                    uint16_t dr = (instruction >> 9) & 0x7;
                    uint16_t sr1 = (instruction >> 6) & 0x7;
                    uint16_t imm_flag = (instruction >> 5) & 0x1;

                    if (imm_flag)
                    {
                        uint16_t imm5 = sign_extend(instruction & 0x1F, 5);
                        reg[dr] = (reg[sr1] & imm5);
                    }
                    else
                    {
                        uint16_t sr2 = instruction & 0x7;
                        reg[dr] = (reg[sr1] & reg[sr2]);
                    }
                    update_flags(dr);
                }
                break;
            case OP_NOT:
                {
                    uint16_t dr = (instruction >> 9) & 0x7;
                    uint16_t sr = (instruction >> 6) & 0x7;
                    reg[dr] = ~(reg[sr]);
                    update_flags(dr);
                }
                break;
            case OP_BR:
                {
                    uint16_t pc_offset_16 = sign_extend(instruction & 0x1FF, 9);
                    uint16_t cond = (instruction >> 9) & 0x7; // 3 bits nzp

                    if (reg[R_COND] & cond) reg[R_PC] += pc_offset_16;
                }
                break;
            case OP_JMP:
                {
                    uint16_t base_r = (instruction >> 6) & 0x7;
                    reg[R_PC] = reg[base_r];
                }
                break;
            case OP_JSR:
                {
                    reg[R_R7] = reg[R_PC]; // linkage back to the calling routine
                    if ((instruction >> 11) & 1)
                    { // jsr
                        uint16_t pc_offset_16 = sign_extend(instruction & 0x7FF, 11);
                        reg[R_PC] += pc_offset_16;
                    }
                    else
                    {
                        uint16_t sr1 = (instruction >> 6) & 0x7;
                        reg[R_PC] = reg[sr1]; // jssr
                    }
                }
                break;
            case OP_LD:
                {
                    uint16_t dr = (instruction >> 9) & 0x7;
                    uint16_t pc_offset_16 = sign_extend(instruction & 0x1FF, 9);
                    reg[dr] = mem_read(reg[R_PC] + pc_offset_16);
                    update_flags(dr);
                }
                break;
            case OP_LDI:
                {
                    uint16_t dr = (instruction >> 9) & 0x7;
                    uint16_t pc_offset_16b = sign_extend(instruction & 0x1FF, 9);
                    // In memory it may be layed out like this:

                    // Address Label      Value
                    // 0x123:  far_data = 0x456
                    // ...
                    // 0x456:  string   = 'a'
                    // if PC was at 0x100
                    // LDI R0 0x023
                    // would load 'a' into R0
                    reg[dr] = mem_read(mem_read(reg[R_PC] + pc_offset_16b));
                    update_flags(dr);
                }
                break;
            case OP_LDR:
                {
                    uint16_t dr = (instruction >> 9) & 0x7;
                    uint16_t base_r = (instruction >> 6) & 0x7;
                    uint16_t offset_16 = sign_extend(instruction & 0x3F, 6);
                    reg[dr] = mem_read(reg[base_r] + offset_16);
                    update_flags(dr);
                }
                break;
            case OP_LEA:
                {
                    uint16_t dr = (instruction >> 9) & 0x7;
                    uint16_t pc_offset_16 = sign_extend(instruction & 0x1FF, 9);
                    reg[dr] = reg[R_PC] + pc_offset_16;
                    // a memory address can be negative in LC-3 apparently...?
                    update_flags(dr);
                }
                break;
            case OP_ST:
                {
                    uint16_t sr = (instruction >> 9) & 0x7;
                    uint16_t pc_offset_16 = sign_extend(instruction & 0x1FF, 9);

                    mem_write(reg[R_PC] + pc_offset_16, reg[sr]);
                }
                break;
            case OP_STI:
                {
                    uint16_t sr = (instruction >> 9) & 0x7;
                    uint16_t pc_offset_16 = sign_extend(instruction & 0x1FF, 9);

                    mem_write(mem_read(reg[R_PC] + pc_offset_16), reg[sr]);
                }
                break;
            case OP_STR:
                {
                    uint16_t sr = (instruction >> 9) & 0x7;
                    uint16_t base_r = (instruction >> 6) & 0x7;
                    uint16_t offset_16 = sign_extend(instruction & 0x3F, 6);

                    mem_write(reg[base_r] + offset_16, reg[sr]);
                }
                break;
            case OP_TRAP:
                reg[R_R7] = reg[R_PC];
                switch (instruction & 0xFF)
                {
                    case TRAP_GETC:
                        reg[R_R0] = (uint16_t)getchar();
                        update_flags(R_R0);
                        break;
                    case TRAP_OUT:
                        // when you cast a larger integer to a char, the
                        // higher bits get truncated and only the lowest 8 bits are kept
                        //putc((char)(reg[R_R0] & 0xFF), stdout);
                        putc((char) reg[R_R0], stdout);
                        fflush(stdout);
                        break;
                    case TRAP_PUTS:
                        {
                            uint16_t *c = memory + reg[R_R0];
                            while (*c)
                            {
                                putc((char)*c, stdout);
                                ++c;
                            }
                            fflush(stdout);
                        }
                        break;
                    case TRAP_IN:
                        {
                            printf("Enter char: ");
                            char c = getchar();
                            putc(c, stdout);
                            fflush(stdout);
                            reg[R_R0] = (uint16_t) c;
                            update_flags(R_R0);
                        }
                        break;
                    case TRAP_PUTSP:
                        {
                            // the characters are contained in consecutive
                            // memory locations.
                            // two characters per memory location
                            // starting the address specified by R0
                            // char is 1 byte
                            uint16_t *c = memory + reg[R_R0];
                            while (*c)
                            {
                                char c1  = (*c) & 0xFF;
                                putc(c1, stdout);
                                char c2 = (*c) >> 8;
                                if (c2) {
                                    putc(c2, stdout);
                                }
                                ++c;
                            }
                            fflush(stdout);
                        }
                        break;
                    case TRAP_HALT:
                        puts("Halt!");
                        fflush(stdout);
                        running = 0;
                        break;
                }
                break;
            case OP_RES:
            case OP_RTI:
            default:
                abort();
                break;
        }
    }
    restore_input_buffering();
}