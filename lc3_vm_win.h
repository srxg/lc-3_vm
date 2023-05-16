#ifndef LC3_VM_WIN_H
#define LC3_VM_WIN_H
#include <stdint.h>

#ifndef MEM_MAX
#define MEM_MAX (1 << 16)
#endif

/**
 * Registers
 * LC-3 has 10 registers, 16 bits each
 * 8 of these will be general purpose, 1 for the program
 * counter (R_PC), and 1 for the condition flags (COND).
 * Theory reminders:
 *      - Program counter is just an unsigned
 *        integer which is the next instruction in memory to execute
 * 
 *      - The condition flags give us info about the previous calculation
*/

enum {
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC, // the program counter
    R_COND,
    R_COUNT // not a register
};

uint16_t memory[MEM_MAX];
uint16_t reg[R_COUNT];



#endif