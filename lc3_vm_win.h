#ifndef LC3_VM_WIN_H
#define LC3_VM_WIN_H
#include <stdint.h>

#ifndef MEM_MAX
#define MEM_MAX (1 << 16)
#endif

uint16_t memory[MEM_MAX];

/**
 * Registers
 * LC-3 has 10 registers, 16 bits each
 * The majority of these will be general purpose
 * 
*/

#endif