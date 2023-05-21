#include "lc3_vm_win.h"

/**
 * Sign-extend a `bit_count`-bit number, `x` to a 16-bit number.
 * We only need to sign extend if the number is negative
 * x >> (bit_count - 1)     ?     Shifts x to the right by `bit_count-1`
*                                 places.
*                                 Why? To isolate the sign bit of the
*                                      `bit_count`-bit number (leftmost bit)
* In this particular context, where the size of the number (bit_count) is
* known and fixed, the & 1 operation is indeed redundant. The sign bit is the
* left-most bit, and all bits to the left of it would be zero after right-shifting
* (bit_count - 1) times, so you don't actually need the & 1.
* However, the & 1 operation is often included out of habit or to make the code
* more robust and self-explanatory. It makes it clear that we are interested in the
* value of a single bit, and it ensures that any extraneous bits, if they were present
* for some reason, would not affect the result.
* 
* if ((x >> (bit_count-1)) & 1) : checks if the sign bit is 1 (negative number)
* x |= (0xFFFF << bit_count); :
            0xFFFF << bit_count
            basically gets a 16 bit number
            with the left bit_count number of
            bits as 0s and the rest on the right as 1s
            1111 1111 1110 0000
            we then OR this with x and assign the result to x
            e.g. if x is representative of -2 , i.e.
             1 0010
             then:
             1111 1111 1110 0000
                          1 0010
             1111 1111 1111 0010
             we get our result, a sign-extended version of x
             1111 1111 1111 0010

*/
uint16_t sign_extend(uint16_t x, int bit_count) {
    if ((x >> (bit_count - 1)) & 1) {
        x |= (0xFFFF << bit_count);
    }
    return x;
}

/*Any time a value is written to a register
 we need to update the flags to indicate its sign.
We will write a function so that this can be reused:*/
void update_flags(uint16_t r) { // we COULD use a char because we only have 10 registers but uint16_t is used for consistency
    if(reg[r] == 0) reg[R_COND] = FL_ZRO;
    /* a 1 in the left-most bit indicates negative */
    else if (reg[r] >> (BIT_LENGTH-1)) {
        reg[R_COND] = FL_NEG;
    } else {
        reg[R_COND] = FL_POS;
    }
}

// **The first 16 bits of the program file specify the address in memory where the program should start**
// This address is called the **origin**
// the first 16 bits of the program file specify the address in memory
// where the program should start
void read_img_file(FILE* file) {
    uint16_t origin; // address in LC-3's memory where the image file should be loaded
    // read into origin, the first (hence 1) element from "file" of size sizeof(origin) - sizeof(origin)
    // is 1 byte
    fread(&origin, sizeof(origin), 1, file);
    // swap to little endian - LC-3 uses big endian (so the image file is big-endian)
    // whereas most modern computers are little endian.
    origin = swap16(origin);
    
    // maximum number of 16 bit words that can be read
    // from the file is determined by the distance from the origin
    // to the end of the LC-3's memory.
    uint16_t max_read = MEM_MAX - origin;
    
    // point at the memory location
    // where the image should be loaded.
    /**
     * So memory + origin is a pointer to the location in memory where you want to begin
     * writing the program. If you were to use just origin, it would be a number, not a pointer,
     * and wouldn't point to the correct place in memory.
     * In simple terms, memory is the start of your memory space, and origin is how far into that
     * memory space you need to go. memory + origin gives you a pointer to that location in memory.
    */
    uint16_t* p = memory + origin;
    
    // read into p max_read number of things 
    // of size 1 byte (sizeof(uint16_t) = 1 byte) from file.
    // read then contains the number of elements read in
    size_t read = fread(p, sizeof(uint16_t), max_read, file);

    // change to little endian
    while(read-- > 0) {
        *p = swap16(*p);
        ++p;
    }

}

uint16_t swap16(uint16_t x) {
    return (x << 8) | (x >> 8);
}

// takes a string of the path of the image file
// and calls read_img_file 
int read_img(const char* img_path) {
    FILE* file = fopen(img_path, "rb");
    if (!file) return 0;
    read_img_file(file);
    fclose(file);
    return 1;
}

int main(int argc, const char* argv[]) {
    // load the argument
    if(argc < 2) {
        printf("lc3vm [image-file1] ...\n");
        exit(2);
    }

    for(int i = 1; i < argc; ++i) {
        if(!read_image(argv[i])) {
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
   enum {
    PC_START = DEFAULT_START_ADDR
   };
   
   // ask yourself what's the pc doing? it contains
   // the mem address of the next instruction to execute
   reg[R_PC] = PC_START;

    int running = 1;
    while(running) {
        // fetch
        uint16_t instruction = mem_read(reg[R_PC]++);
        uint16_t opcode = instruction >> 12;

        switch(opcode) {
            case OP_ADD:
                // the destination register!
                /**
                 * move the 3-bits specifying the destination register
                 * down to bits 0, 1 and 2
                 * isolate them by anding with 111 (7)
                 * do the same for sr1!
                */
                uint16_t dr = (opcode >> 9) & 0x7;
                uint16_t sr1 = (opcode >> 6) & 0x7;
                
                // are we in immediate mode?
                uint16_t imm_flag = (opcode >> 5) & 0x1;
                if(imm_flag) {
                    // we're in immediate mode
                    uint16_t imm5 = sign_extend(opcode & 0x1F, 5);
                    reg[dr] = reg[sr1] + imm5;
                } else {
                    // we're in register mode
                    uint16_t sr2 = opcode & 0x7;
                    reg[dr] = reg[sr1] + reg[sr2];
                }

                update_flags(dr);

                break;
            case OP_AND:
                uint16_t dr = (opcode >> 9) & 0x7;
                uint16_t sr1 = (opcode >> 6) & 0x7;
                uint16_t imm_flag = (opcode >> 5) & 0x1;

                if(imm_flag) {
                    uint16_t imm5 = sign_extend(opcode & 0x1F, 5);
                    reg[dr] = (reg[sr1] & imm5);
                } else {
                    uint16_t sr2 = opcode & 0x7;
                    reg[dr] = (reg[sr1] & reg[sr2]);
                }
                update_flags(dr);
                break;
            case OP_NOT:
                uint16_t dr = (opcode >> 9) & 0x7;
                uint16_t sr = (opcode >> 6) & 0x7;
                reg[dr] = ~(reg[sr]);
                update_flags(dr);
                break;
            case OP_BR:
                uint16_t pc_offset_16 = sign_extend(opcode & 0x1FF, 9);
                uint16_t cond = (opcode >> 9) & 0x7; // 3 bits nzp

                if(reg[R_COND] & cond) reg[R_PC] += pc_offset_16;

                break;
            case OP_JMP:
                uint16_t base_r = (opcode >> 6) & 0x7;
                reg[R_PC] = reg[base_r];
                break;
            case OP_JSR:
                reg[R_R7] = reg[R_PC]; // linkage back to the calling routine
                if((opcode >> 11) & 0x1) { // jsr
                    uint16_t pc_offset_16 = sign_extend((opcode & 0x7FF), 11);
                    reg[R_PC] += pc_offset_16;
                } else { // jssr
                    reg[R_PC] = (opcode >> 6) & 0x7;
                }
                break;
            case OP_LD:
                uint16_t dr = (opcode >> 9) & 0x7;
                uint16_t pc_offset_16 = sign_extend(opcode & 0x1FF, 9);
                reg[dr] = mem_read(reg[R_PC] + pc_offset_16);
                update_flags(dr);
                break;
            case OP_LDI:
                // load indirect - load a value from some location in memory
                // into a register
                // LDI DR, LABEL
                // opcode is bits [15:12] = 1010 i.e. 10 (see spec)
                // bits[11:9] = 3 dit destination register
                // bits[8:0] = pcoffset9
                // an immediate value embedded in the instruction
                // since this loads from memory, we can guess that this value
                // is some kind of address (to load from)
                // in the spec: "An address is computed
                // by sign extending bits [8:0] to 16 bits and adding this value
                // to the incremented PC". What is stored in memory at this address
                // is the address of the data to be loaded into DR.
                uint16_t dr = (opcode >> 9) & 0x7;
                uint16_t pc_offset_16b = sign_extend(opcode & 0x1FF, 9);
                // "The resulting sum is an address to a location in memory, and that
                // address contains, yet another value which is the address of the value to load."
                // why? LD instruction is limited to address offsets that are 9 bits.
                // whereas the memory requires 16 bits to address.
                // LDI is useful for loading values that are stored
                // in locations far away from the current PC, but to use it,
                // the address of the final location needs to be stored in a neighbourhood
                // nearby. Think of it like a local variable in C which is a pointer.
                // the value of far_data is an address
                // of course far_data itself (the location in memory containing the address) has an address
                //char* far_data = "apple";

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
                break;
            case OP_LDR:
                uint16_t dr = (opcode >> 9) & 0x7;
                uint16_t base_r = (opcode >> 6) & 0x7;
                uint16_t offset_16 = sign_extend(opcode & 0x3F, 6);
                reg[dr] = mem_read(reg[base_r] + offset_16);
                update_flags(dr);
                break;
            case OP_LEA:
                uint16_t dr = (opcode >> 9) & 0x7;
                uint16_t pc_offset_16 = sign_extend(opcode & 0x1FF, 9);
                reg[dr] = reg[R_PC] + pc_offset_16;
                // a memory address can be negative in LC-3 apparently...?
                update_flags(dr);
                break;
            case OP_ST:
                uint16_t sr = (opcode >> 9) & 0x7;
                uint16_t pc_offset_16 = sign_extend(opcode & 0x1FF, 9);
                uint16_t mem_addr = reg[R_PC] + pc_offset_16;
                
                mem_write(mem_addr, reg[sr]);
                break;
            case OP_STI:
                uint16_t sr = (opcode >> 9) & 0x7;
                uint16_t pc_offset_16 = sign_extend(opcode & 0x1FF,9);
                
                mem_write(mem_read(reg[R_PC] + pc_offset_16), reg[sr]);
                break;
            case OP_STR:
                uint16_t sr = (opcode >> 9) & 0x7;
                uint16_t base_r = (opcode >> 6) & 0x7;
                uint16_t offset_16 = sign_extend((opcode & 0x3F), 6);

                memw_write(reg[base_r]+offset_16, reg[sr]);
                break;
            case OP_TRAP:
                switch(opcode & 0xFF) {
                    case TRAP_GETC:
                        reg[R_R0] = (uint16_t) getchar();
                        update_flags(R_R0);
                        break;
                    case TRAP_OUT:
                        // when you cast a larger integer to a char, the
                        // higher bits get truncated and only the lowest 8 bits are kept
                        putc((char) (reg[R_R0] & 0xFF), stdout);
                        fflush(stdout);
                        break;
                    case TRAP_PUTS:
                        uint16_t* c = memory + reg[R_R0];
                        while (*c) {
                            putc((char)*c, stdout);
                            ++c;
                        }
                        fflush(stdout);
                        break;
                    case TRAP_IN:
                        printf("Enter char: ");
                        char c = getchar();
                        putc(c, stdout);
                        fflush(stdout);
                        reg[R_R0] = (uint16_t) c;
                        update_flags(R_R0);
                        break;
                    case TRAP_PUTSP:
                        // the characters are contained in consecutive
                        // memory locations.
                        // two characters per memory location
                        // starting the address specified by R0
                        // char is 1 byte
                        uint16_t* c = memory + reg[R_R0];
                        while(*c) {
                            putchar(((char) *c), stdout);
                            char c2;
                            if((c2 = (*c) >> 8)) putchar(c2, stdout);
                            ++c;
                        }
                        fflush(stdout);
                        break;
                    case TRAP_HALT:
                        printf("Halting");
                        fflush(stdout);
                        running = 0;
                        break;
                }
                break;
            case OP_RES:
                abort();
                break;
            case OP_RTI:
                abort();
                break;
            default:
                // bad opcode
                break;
        }
        // decode
        // execute
    }

    // shutdown

}