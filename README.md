# VM for the LC-3 architecture
## Follows jmeiner's lc3-vm tutorial
This VM simulates the fictional computer "LC-3"
- Has a simplified instruction set compared to x86
- still makes use of the main ideas in modern CPUs.
# To-Do
1. Set up the hardware components for the VM
   - [x] Memory
   - [x] Registers
   - [x] Instruction Set
   - [x] Condition flags

### Memory
65,536 memory locations storing 16-bit values (2^16 : maximum accessible locations using a 16-bit unsigned int)
So, our memory is limited to 128KB. Pretty small.
Use a simple array for this.

### Registers
We'll use 10 registers. 8 are general purpose, 1 as the program counter (R_PC), and 1 for the condition flags (R_COND).
These will be stored in an array too.

### Instruction set
Here, instructions are exactly what you'd think: commands to the CPU to do a fundamental task.
Remember theory! Instructions have an **op*code*** and an **op*erand***!
The *opcode* just represents some (one) task the CPU knows how to do.
In LC-3, **there are 16 *opcodes***. So, everything LC-3 can calculate is some sequence
of these 16 opcodes. Each instruction is **16 bits** long, but the
*left 4 bits* store the **opcode** while the rest of the bits store the **operand** (the parameters, in this context).

**Note**: x86 has way more instructions (hundreds). ARM and LC-3 have very few - i.e., they have RISCs.
          larger instruction sets, i.e. that of x86, are called CISCs.
          While larger ones don't necessarily provide fundamentally new possibilties,
          they often just make it a little easier/convenient to write assembly for.
          So, for example, a single instruction in a CISC might take the place of several in RISC.
          However, they tend be more complex and expensive to design and manufacture - this and other trade offs cause the designs
          to come "in and out of style".

### Condition Flags
Condition flags will be stored in R_COND. Condition Flags just provide the give us information about the most recently executed
calculation, allowing programs to, e.g., check logical conditions e.g. ```if (x < 0) {...}```

Various situations are signalled to the CPU via a condition flag. Each CPU has a variety of these condition flags.
In our case, the LC-3 uses only 3 condition flags which indicate the sign of the previous calculation (positive, 0, or negative)

### See "Assembly_LC3.md" for some Assembly examples.

### Procedure
1. load an instruction from memory, the address of that instruction in memory is in the R_PC register
2. Increment the R_PC register
3. Look at the ***opcode*** to determine the type of instruction it should perform
4. Perform the instruction using the parameters (*operands*) in the instruction.
5. Go back to step 1.

"You may be wondering, “if the loop keeps incrementing the PC, and we don’t have if or while, won’t it quickly run out of instructions?” No. As we mentioned before [Assembly_LC3.md], some goto-like instructions change the execution flow by jumping the PC around."

### The ADD instruction
In LC-3 Assembly language, the `ADD` instruction has two forms:
1. **Register Mode**: `ADD DR, SR1, SR2`
2. **Immediate Mode**: `ADD DR, SR1, imm5`

### Register Mode
In the Register Mode, the `ADD` instruction takes two source registers (`SR1` and `SR2`), adds their contents together, and stores the result in a destination register (`DR`).  
The corresponding binary encoding for this format is as follows:
```
15 14 13 12   11   10   9     8 7 6    5   4  3    2  1  0
0  0  0  1  |      DR      |   SR1   | 0 | 0  0  |   SR2   |
```
Bits 3 and 4 are unused.
For example, the command `ADD R2, R3, R4 ; R2 <- R3 + R4`.

### Immediate Mode
```
15 14 13 12   11   10   9     8 7 6    5    4  3  2  1  0
0  0  0  1  |      DR      |   SR1   | 1 |    i m m 5     |
```
In the Immediate Mode, the `ADD` instruction takes one source register (`SR1`) and a 5-bit immediate value (`imm5`), adds them together, and stores the result in a destination register (`DR`).
This mode is primarily useful for incrementing and decrementing as imm5 is limited to 5 bits, hence 
can only hold a small number, up to 2^5 = 32 (unsigned). An example of a command in this mode is `ADD R0 R0 1`, which adds 1 to `R0` and stores it back in `R0`.

### In Both:
* we start with 4 bits: 0 0 0 1
* this is the opcode value for OP_ADD
               
* the next 3 bits are marked DR - 
* the destination register, WHERE the added sum will be stored
                 
* the next 3 bits are SR1. This is the register containing the first number to add
                 
* So, in both, we know where we want to store the result and know where the first number to add is

### Difference in the Immediate Mode
The 6th bit (bit index 5) is used to indicate immediate or register mode. Immediate mode is indicated
by a 1. 

### Sign-Extending
In immediate mode, the second source operand is obtained by sign-extending the imm5 field to 16
bits.

The immediate mode value only has 5 bits, but it needs to be added to a 16 bit number. Thus, those 5 bits
need to be extended to 16 to match the other number.

**For positive numbers**: we can just fill in 0's for those additional bits.
---
**For negative numbers**, this causes a problem. -1 in 5 bits is `1  1111`.
If we just extend it with 0s, this is `0000 0000 0001 1111` which is equal to 31.

**So, sign extension corrects this problem by filling in 0's for positive numbers and 1's for negative numbers, so that original values are preserved.**
*Remember Two's Complement:*
The leftmost bit is used as a sign bit. If the sign bit is 1, the number is *negative*. If it is 0, the number is positive.
A negative number's value is found by flipping the bits and adding 1.
So, in the case of -1:
1. Bin of 1 is `00001` in 5 bits.
2. Flip the bits : `11110`.
3. Add 1 to the result : `11111`
