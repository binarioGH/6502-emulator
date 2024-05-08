#include <stdio.h>
#include <stdlib.h>

// We need to define a 16bit and a 8bit type
using Byte = unsigned char;
using Word = unsigned short;
using u32 = unsigned int;

struct Mem{
    static constexpr u32 MAX_MEM = 1024 * 64;
    Byte Data[MAX_MEM];

    void Initialise(){
        // Clear memory
        for(u32 i = 0; i < MAX_MEM; i++){
            Data[i] = 0;
        }
    } 

    // read 1 byte
    Byte operator[](u32 Address) const {
        // assert here Address is < MAX_MEM
        // This is so you can do memory[PC] instead of memory.Data[PC]
        return Data[Address];
    }

    // Write 1 byte
    Byte& operator[](u32 Address) {
        // assert here Address is < MAX_MEM
        // This is so you can write into Data by doing mem[address] = value
        return Data[Address];
    }

    // Write 2 bytes
    void WriteWord(Word Value, u32 Address, u32& Cycles){
        Data[Address]       = Value & 0xFF;
        Data[Address + 1]   = (Value >> 8);
        Cycles -= 2;
    }
};


struct CPU {
    
    // We need a Program counter
    Word PC;
    // Stack Pointer
    Word SP;
    
    //registers
    Byte A, X, Y;

    // Processor Status flags
    Byte C : 1; // carry flag
    Byte Z : 1; // zero flag
    Byte I : 1; // Interrupt Disable
    Byte D : 1; // Decimal mode
    Byte B : 1; // Break Command
    Byte V : 1; // Overflow flag
    Byte N : 1; // Negative flag

    void Reset(Mem& memory){
        // First thing: reset the CPU

        PC = 0xFFFC;
        SP = 0x0100;
        // A real computer would do some stuff after this, but since this is emulated we skip ahead.

        // Clear all flags and registers
        C = Z = I = D = B = V = N = 0;
        A = X = Y = 0;

        memory.Initialise();
    }

    Byte FetchByte(u32& Cycles, Mem& memory){
        // Fetching takes one cycle, and takes the part of memory that the PC is currently at.
        Byte Data = memory[PC];
        PC++;
        Cycles--;
        return Data;

    }
    Byte ReadByte(u32& Cycles, Byte Address,Mem& memory){
        // Reads data from memory which is at the specified Address but since it doesnt execute it doesnt increment the PC
        Byte Data = memory[Address];
        Cycles--;
        return Data;
    }

    Word FetchWord(u32& Cycles, Mem& memory){

        //6502 is little endian
        Word Data = memory[PC];
        PC++;
        Cycles--;

        Data |= (memory[PC] << 8); // Shift by 8 bytes
        PC++;
        Cycles--;

        // If you wanted to handle endianess you would have to swap bytes here.
        /*
        if(PLATFORM_BIG_ENDIAN){
            SwapBytesInWord(DAta);
        }
        */
        return Data;
    }

    // opcodes
    static constexpr Byte
        INS_LDA_IM = 0xA9,
        INS_LDA_ZP = 0xA5,
        INS_LDA_ZPX = 0xB5,
        INS_JSR = 0x20;
    


    void LDASetStatus(){
        // This is a instruction that is very repetitve, so i will just put it in a function
        Z = (A == 0);
        N = (A & 0b10000000) > 0;
    }

    void Execute(u32 Cycles,Mem& memory){

        while(Cycles  > 0){
            Byte Ins = FetchByte(Cycles, memory);
            switch(Ins){
                case INS_LDA_IM:{
                    /*
                    LDA - Load accumulator
                    A,Z,N = M

                    Loads a  byte of memory into the accumulator setting 0 and negative flags as appropriate
                    C,I,D,B,V = not affected
                    Z =  Set if A == 0

                    N = Set if bit 7 of A is set
                    */
                    Byte Value = FetchByte(Cycles, memory);
                    A = Value;
                    LDASetStatus();
                
                } break;

                case INS_LDA_ZP:{
                   Byte ZeroPageAddr = FetchByte(Cycles, memory);
                   A = ReadByte(Cycles, ZeroPageAddr,memory);
                
                   LDASetStatus();

                } break;

                case INS_LDA_ZPX:{
                    /*
                        The address to be read is taking an 8 bit zeropageaddress + the value of the X register
                    
                    */
                   Byte ZeroPageAddr = FetchByte(Cycles, memory);
                   ZeroPageAddr += X;
                   // We reduce one cycle because we executed zeropageaddress += X
                   Cycles--;
                   A = ReadByte(Cycles, ZeroPageAddr,memory);
                
                   LDASetStatus();

                } break;

                case INS_JSR: {
                    /*
                    pushes an addres (minus one) of the return point on to the stack and then sets the program counter to the target memory address
                    No process status flags are affected
                    */
                   Word SubAddr = FetchWord(Cycles, memory);
                   memory.WriteWord(PC - 1, SP, Cycles);
                   PC = SubAddr;
                   printf("Jumping to %d\n", PC);
                   SP += 1;
                   Cycles--;
                } break;

                default: {
                    printf("Instruction not handled %d\n", Ins);
                }
                break;
            }
            
            
        }

    }

};

int main(void){
    Mem mem;
    CPU cpu;
    cpu.Reset( mem );
    // This is for testing, we are going to put a program in memory
    mem[0xFFFC] = CPU::INS_JSR;
    mem[0xFFFD] = 0x42;
    mem[0xFFFE] = 0x42;
    mem[0x4242] = CPU::INS_LDA_IM;
    mem[0x4243] = 0x84;
    //End little endian inline program
    cpu.Execute(8, mem );
    return 0;
}