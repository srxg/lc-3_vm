## Simple Hello World in assembly for LC-3 for reference

```assembly
.ORIG x3000
LEA R0, HELLO_STR
PUTs
HALT
HELLO_STR .STRINGZ "Hello World!
.END
```