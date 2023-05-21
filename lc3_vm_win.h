#ifndef LC3_VM_WIN_H
#define LC3_VM_WIN_H
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <Windows.h> // windows only
#include <conio.h> // _kbhit
#ifndef MEMORY_MAX
#define MEMORY_MAX (1 << 16)
#endif
#ifndef DEFAULT_START_ADDR
#define DEFAULT_START_ADDR 0x3000
#endif
#ifndef BIT_LENGTH
#define BIT_LENGTH 16
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

enum {
    FL_POS = 1 << 0,
    FL_ZRO = 1 << 1,
    FL_NEG = 1 << 2,
};

enum {
    OP_BR = 0,  // branch
    OP_ADD,     // add
    OP_LD,      // load
    OP_ST,      // store
    OP_JSR,     // jump register
    OP_AND,     // bitwise and
    OP_LDR,     // load register
    OP_STR,     // store register
    OP_RTI,     // unused
    OP_NOT,     // bitwise not
    OP_LDI,     // load indirect
    OP_STI,     // store indirect
    OP_JMP,     // jump
    OP_RES,     // reserved (unused)
    OP_LEA,     // load effective address
    OP_TRAP     // execute trap
};

enum {
    MR_KBSR = 0xFE00, // keyboard status
    MR_KBDR = 0xFE02  // keyboard data
};

enum {
    TRAP_GETC = 0x20,    // get char from keyboard, not echoed to terminal
    TRAP_OUT = 0x21,    // output a character
    TRAP_PUTS = 0x22,   // output a word string
    TRAP_IN = 0x23,     // get char from keyboard, echo to the terminal
    TRAP_PUTSP = 0x24,  // output a byte string
    TRAP_HALT = 0x25    // halt program
};



uint16_t memory[MEMORY_MAX];
uint16_t reg[R_COUNT];

uint16_t sign_extend(uint16_t x, int bit_count);
/*
    The condition codes are set, based on whether
    the result is negative, zero, or positive. (Pg. 526)
*/
void update_flags(uint16_t r); 

void read_img_file(FILE* FILE);
int read_img(const char* img_path);

uint16_t swap16(uint16_t x);

void mem_write(uint16_t addr, uint16_t val);
uint16_t mem_read(uint16_t addr);

void disable_input_buffering();

void restore_input_buffering();

uint16_t check_key();

void handle_interrupt(int sig);

#endif