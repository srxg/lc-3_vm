# VM for the LC-3 architecture
## Follows jmeiner's lc3-vm tutorial
This VM simulates the fictional computer "LC-3"
- Has a simplified instruction set compared to x86
- still makes use of the main ideas in modern CPUs.
# To-Do
1. Set up the hardware components for the VM
   - [x] Memory
   - [ ] Registers
   - [ ] Instruction Set
   - [ ] Condition flags

### Memory
65,536 memory locations storing 16-bit values (2^16 : maximum accessible locations using a 16-bit unsigned int)
So, our memory is limited to 128KB. Pretty small.
Use a simple array for this.