## Simple Hello World in assembly for LC-3 for reference

```assembly
.ORIG x3000         ; memory address of where the program will be loaded
LEA R0, HELLO_STR   ; load the address of HELLO_STR into R0
PUTS                ; output the string pointed to BY R0 to the console
HALT                ; hal the program 
HELLO_STR .STRINGZ "Hello World! ; store this string here in the program
.END                ; mark the end of the file
```

```.ORIG``` and ```.STRINGZ``` may look like instructions, but they aren't.
They're **assembler directives** which generate a piece of code or data
(like macros). E.g., ```.STRINGZ``` inserts a string of characters into
the program binary at the location where it is written.

"Every instruction is 16 bits though, right So how come there are different numbers
of characters in each line? How this inconsitency possible?"
Well, the code is just assembly code. It's the human-readable and writable form.
It's literally plain text. An **assembler** is used to convert each line
of text into the actual 16 bit binary instructions the VM can understand.
This binary form, which is essentially an array of 16-bit instructions, is the
machine code - this is what the VM will actually run.

"
**Note:** Although a compiler and an assembler play a similar role in development,
they are not the same. An assembler simply encodes what the programmer has written in
text into binary, replacing symbols with their binary representation and packing
them into instructions.
"

## Loops and Conditions?
We use the a goto-esque instruction for loops and instructions.
An example which counts to 10:
```
AND R0, R0, 0       ; clear R0
LOOP                ; just a label at the top of the loop
ADD R0, R0, 1       ; add 1 to R0 and store it back in R0.
ADD R1, R0, -10     ; SUBTRACT 10 from R0 and store the result in R1
BRn LOOP            ; go back to LOOP if the result was negative
... ; R0 is now 10!
```

## (Jumping ahead a little) Trap Routines
LC-3 provides a few predfined routines for performing common I/O interactions, e.g. getting input from the
keyboard and echoing to the console. These are called **trap routines**.
**Trap Routines** can be thought of as the Operating System or API for the LC-3. Each trap routine is
assigned a *trap code* which identifies it (similar to an opcode). To execute a trap routine, the ```TRAP```
instruction is called with the trap code of the desired routine.
---
*"Why are the trap codes not included in the instructions?"*
because they do not actually introduct new *functionality*. They just provide a convenient way to perform
a task (similar to OS system calls). In the official simulator for LC-3, trap routines are written in Assembly.
The ```PC``` is moved to the code of a trap routine whenever it's called. The CPU executes the routine's instructions,
and when it is complete, the ```PC``` is reset to the location following the initial call.
### Important Note:
This is why programs start at address ```0x3000``` - the lower addresses are left empty to leave space
for the trap routines.
---
Trap Routines are not defined by *how* they must be implemented, but by what they are supposed to do.
In our case, when a trap code is invoked, we will call a C function. When finished, execution will return
to the instructions.

"Even though the trap routines can be written in assembly and this is what a physical LC-3 computer would do, it isn’t the best fit for a VM. Instead of writing our own primitive I/O routines, we can take advantage of the ones available on our OS. This will make the VM run better on our computers, simplify the code, and provide a higher level of abstraction for portability."

## Loading Programs...
How do instructions get into memory in the first place?
Assembly -> Machine Code results in a file containing an array of instructions and data.
So, load it in by copying the contents right into an address in memory.

**The first 16 bits of the program file specify the address in memory where the program should start**
This address is called the **origin**

This MUST be read first, after which the rest of the data can be read from the file into memory
starting at the origin address.
---
### Endianness
How bytes of an integer are interpreted. Endianness only affects how the bytes of the number are arranged in memory.
Little-endian : the first byte in memory is the least significant digit.
Big-Endian    : the first byte in memory is the most significant digit.

e.g.
Imagine 0x0AB0C00D which is 179355661
In BIG-ENDIAN, the ***MOST* SIGNIFICANT BYTE** is stored
at the smallest byte address (first). So, it would be stored in memory
as:
Byte Address   +0    +1   +2    +3
Big Endian     0A    B0   C0    0D

In LITTLE-ENDIAN, the ***LEAST* SIGNIFICANT BYTE** is stored
at the smallest byte address (first). So, it would be stored in memory
Byte Address   +0    +1   +2    +3
Big Endian     0D    C0   B0    0A

If you read 0x0AB0C00D from a big-endian system, but interpret it as little-endian, you'd get 0x0DC0B00A instead.


## Memory Mapped Registers
Some special registers can't be accessed from the usual register table, and so a special
address is reserved for them in memory. To read and write to/from these registers, you just read and write
to their location in memory. These are called **memory mapped registers**. They are commonly used to interact
with special hardware devices.

LC-3 has two memory mapped registers that need to be implemented, the **keyboard status register** (``KBSR``) and
**keyboard data register** (``KBDR``).

``KBSR`` indicates whether a key has been pressed and the ``KBDR`` identifies which key was pressed.
---
Although you *can* request keyboard input using GETC, this *blocks execution until input is received*. 
This is in contrast to ``KBSR`` and ``KBDR`` which allow you to *poll the state* of the device and continue execution,
so the program can stay responsive while waiting for input.
