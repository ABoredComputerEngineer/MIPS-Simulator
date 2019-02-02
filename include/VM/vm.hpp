#ifndef VM_HPP

#define VM_HPP
#include <cinttypes>
#include <cstddef>
#include <unordered_map>
#include "printBuffer.hpp"
typedef uint8_t byte;
typedef uint32_t Word;

enum IntegerLimits {
    BYTE_MAX = 0xffu,
    WORD_MAX = 0xffffffffu, // Equal to UINT32_MAX
    UMAX_16 = 0xffffu,// Equal to UINT16_MAX
};

class Machine;
struct Memory{
    /* 
     * Memory is a static array i.e, its  size dosen't change during
     * the program execution.
     * The size of the memory ( number of bytes ) is always an integral multiple of the word size
     * We force this by the help of the ALIGN_UP and ALIGN_DOWN macros
     */
    byte *vals;
    const size_t byteCount; // The total amount of bytes allocated i.e. size of the vals array
    const size_t textStart; // the byte index at which the text segment start
    const size_t textSize; // the size of the text segment in words
    const size_t staticStart;
    const size_t staticSize;
    const size_t heapStart;

    Memory (size_t bytes, size_t wordSize);
    Word endWord();
    void dealloc();

    void dump(AppendBuffer &);
    inline void set(byte); // sets the memory contents to the given  word (uint32)
    friend class Machine;
};


class Machine {
//    typedef  std::function< void (Word) > InstructionFunc;
    public:
    typedef void (Machine::*InsPointer)(void); 
    typedef std::unordered_map< size_t , InsPointer > CodeToFunctionMap; // your array of function pointers
    enum Registers{
        ZERO = 0,
        AT,
        V0 = 2, V1,
        A0, A1, A2, A3,
        T0, T1, T2, T3, T4, T5, T6, T7,
        S0, S1, S2, S3, S4, S5, S6, S7,
        T8, T9,
        K0,K1,
        GP, SP, FP, RA,
    };
    enum MachineSpec {
        REG_COUNT = 32,
        WORD_SIZE = 32,
    };

    private:
    Word reg[REG_COUNT]; // Registers are not byte addressable so we declare them as static array
    Memory memory; // the primary memory
    Word pc; // The program counter
    Word currentIns;
    CodeToFunctionMap functions;
    void dumpRegister( AppendBuffer &);    
    void executeIns(Word);
    static void arithmeticTest(Machine &);
    static void testBranch(Machine &);
    public:
    static void test();
    void loadProgram(const char *);
    bool load(const char *);
    void reset();
    void addFunctions();
    void execute();
    void dumpMem(const char *);
    Machine (size_t bytes );


    private:
    void sll();
    void addr();
    void addu();
    void subr();
    void subu();
    void ori();
    void xorr();
    void nor();
    void slt();
    void sltu();
    void andr();
    void andi(); 
    void orr(); 
    void addi( );
    void addiu();
    void slti();
    void sltiu();
    void xori();

    void lb();
    void lh();
    void lw();
    void lbu();
    void lhu();
    void sb();
    void sh();
    void sw();

    void beq();
    void jmp();
    void bne();
    void jr();
    void jal();
};


/*
 * 'aligns' the number down to the nearest integer multiple of given number, which must be a power of two
 * ALIGN_DOWN(33,32) = 32 ,
 * ALIGN_DOWN(65,32) = 64, 
 * ALIGN_DOWN( 97, 32) = 96

 * We achieve this by simply setting the lower lg(p) ( log base 2 ) bits of x to zero
 */


#define ALIGN_DOWN(x,p) ( static_cast<uint64_t>(x) & ~( ( p ) - 1 ) )
#define ALIGN_UP(x,p) ( ALIGN_DOWN( static_cast<uint64_t>( x ) + ( p ) - 1, ( p )) )

#define RS(x) ( getBits(x,25,21) )
#define RT(x) ( getBits(x,20,16) )
#define RD(x) ( getBits(x,15,11) )
#define IMM(x) ( getBits(x,15,0) )
#define SHAMT(w) ( getBits(w,10,6) )
#define ABS(x) ( ( ( x ) < 0 )?( -( x ) ):0 )

inline size_t getBits( size_t x, size_t end, size_t start){
    return ( x >> (start) ) & ~( ~0 << ( end-start + 1 ) );
}

Word getWord(byte *);
Word getHalfWord(byte *);

#endif