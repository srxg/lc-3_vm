#include "lc3_vm_win.h"

int main(int argc, const char* argv[]) {

    // load the argument

    // setup

    
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