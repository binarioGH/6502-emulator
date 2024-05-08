# CPU Emulator

I want to code a CPU emulator :3

https://www.youtube.com/watch?v=qJgsuQoy9bc

https://retrogamecoders.com/6502-processor/6502-instructions/

# 6502

* very simple processor
* ha  same principles as a modern processor
* memory, code, execute code, run trough instructions, registers, so on

* can only go trough 3 registers
* 16 bit
* little endian

```perl
perl -MConfig -e 'print "$Config{byteorder}\n";'
12345678
```
* if it starts with 1 you are little endian, 8 or 4 is big endian
* I am little endian in my box

* zero page
    * only 8 bit address operand
    * limits addressing only the first 256 bytes of memory ($0000 to $00FF)
    * the most significant byte of the address is always 0

* can skip clock cycles, so its very fast for the time it was created

* stack memory of 256 byte of memory


## Stack pointer implementation

the stack pointer is an address of the current part of the stack that the program is currently using

stack == small piece of reusable memory
when you are executing code you are pushing things into the stack and popping stuff out of it too

stack == small memory that is reused

Registers: (they are 3)

we have 3 8bit registers in the 6502 processor:


#### Index register X
* used to hold counters or offsets for accessing memory
* Has one special function: it can be used to get a copy of the stack pointer or change its value

#### Index register Y
* holds counters and offsets for accessing memory, same as X
* no special functions

#### Acumulator (index register A)
* Aritmethic and logical operations (except incrementions and decreses)
* the contents of the accumulator can be stored and retrieved either from memory or stack


#### Process status

* A single byte with a bunch of flags in it
    * This is used in modern processors
* depending on what instruction gets called, this flags will be set or cleared

[Status_flags](https://www.nesdev.org/wiki/Status_flags)



### Memory

* this processor needs memory, at the very least the zero page and the stack pointer + more memory to be able to execute anything



## Current status (15:19)

At this moment i have a struct of the CPU with the next variables:
A program counter
A Stack pointer
3 registers
7 process status flags
A function to reset the CPU

And a struct for memory, which has:
A variable for max memory: MAX_MEM = 1024 * 64;
An array called Data which is the length of MAX_MEM

A function to initialise memory which resets the Data array



A main function that instances the memory, and the CPU structs, then resets the CPU (sends mem as an argument)
and then sends mem as an argument to cpu.Execute() which i still dont define


## CPU ticks

the cpu has a clock, and each tick it will execute an instruction
ticks == cycles


## Current status (25:51)

Now I have coded a FetchByte function, Execute and overwritten operator[] for Memory struct.

Execute -> Its a while loop that goes trough the cycles of the CPU, and in each of them it fetches a byte, checks what instruction it has and executes it trough a switch, the instructions are optcodes that are weird fucking numbers.


operator[] -> Allows me to get `memory.Data[]` from just doing `memory[]`
FetchByte -> Uses `memory[PC]` to get the current byte the program counter is pointing at.


Compiles `B)`


## JSR

Jump to Subroutine

* pushes an addres (minus one) of the return point on to the stack and then sets the program counter to the target memory address
