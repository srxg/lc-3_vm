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
void update_flags(uint16_t r) {
    if(reg[r] == 0) reg[R_COND] = FL_ZRO;
    /* a 1 in the left-most bit indicates negative */
    else if (reg[r] >> (BIT_LENGTH-1)) {
        reg[R_COND] = FL_NEG;
    } else {
        reg[R_COND] = FL_POS;
    }
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
                // the ADD instruction...
                // takes two numbers
                // adds them together
                // stores the result in a register
                
                break;
            case OP_AND:
                break;
            case OP_NOT:
                break;
            case OP_BR:
                break;
            case OP_JMP:
                break;
            case OP_JSR:
                break;
            case OP_LD:
                break;
            case OP_LDI:
                break;
            case OP_LDR:
                break;
            case OP_LEA:
                break;
            case OP_ST:
                break;
            case OP_STI:
                break;
            case OP_STR:
                break;
            case OP_TRAP:
                break;
            case OP_RES:
                break;
            case OP_RTI:
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