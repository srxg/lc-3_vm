# VM for the LC-3 architecture
## Follows jmeiner's lc3-vm tutorial
This VM simulates the fictional computer "LC-3"
- Has a simplified instruction set compared to x86
- still makes use of the main ideas in modern CPUs.
# To-Do
1. Set up the hardware components for the VM
   - [x] Memory
   - [x] Registers
   - [ ] Instruction Set
   - [ ] Condition flags

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

# See "Assembly_LC3.md" for some Assembly examples.

### Procedure
1. load an instruction from memory, the address of that instruction in memory is in the R_PC register
2. Increment the R_PC register
3. Look at the ***opcode*** to determine the type of instruction it should perform
4. Perform the instruction using the parameters (*operands*) in the instruction.
5. Go back to step 1.

"You may be wondering, “if the loop keeps incrementing the PC, and we don’t have if or while, won’t it quickly run out of instructions?” No. As we mentioned before [Assembly_LC3.md], some goto-like instructions change the execution flow by jumping the PC around."
