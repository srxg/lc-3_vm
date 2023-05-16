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
