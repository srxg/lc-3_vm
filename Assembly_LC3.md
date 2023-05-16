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