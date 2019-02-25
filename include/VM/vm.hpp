#ifndef VM_HPP

#define VM_HPP
#include <unordered_map>
#include <string>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cinttypes>
#include <cstddef>
#include <climits>
#include <vector>
#include "printBuffer.hpp"
typedef uint8_t byte;
typedef uint32_t Word;

typedef std::unordered_map<size_t,size_t> NumToNumMap;
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

    void dump(AppendBuffer &);
    inline void set(byte); // sets the memory contents to the given  word (uint32)
    ~Memory();
    friend class Machine;
};

struct MainHeader{
    char  isa[16]; // string containing the isa that generated the file
    size_t version; // the version of the assembler that generated the binary file
    size_t textOffset; // the number of bytes from the begining from which the actual program code starts
    size_t phOffset; // the number of bytes after which the program header begins
    size_t dbgOffset; // the number of bytes from the begining of file after which the debug section begins
    size_t secOffset; // the number of bytes after which section information is stored, wiil be added in later version
};


struct ProgramHeader {
    size_t progSize; // the total size of only the machine code in bytes
    size_t origin; // the offset from the start of text segment in  memory where the code should be stored
    ProgramHeader(){}
};

#define SRC_PATH_MAX 4096

struct DebugSection{
    char srcPath[SRC_PATH_MAX+1]; // the absolute path of the source file from which the binary file was generated
    size_t lineMapCount;
    size_t lineMapSize;
};

struct LineMap {
    size_t lineNum;
    size_t ins;
    size_t insNum;
};


class Machine {
//    typedef  std::function< void (Word) > InstructionFunc;
    public:
    typedef void (Machine::*InsPointer)(void); 
    typedef std::unordered_map< size_t , InsPointer > CodeToFunctionMap; // your array of function pointers
    // General purpose registers
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

    // The exception Type denotes a bit field. The exception occured if the bit at the position is set
    enum class ExceptionType : unsigned int {
        OVERFLOW = 0x1,   // Arithmeic overflow 
        INVALID = 0x2, // Invalid Instruction
        TRAP = 0x4, // Trap signal , used for debugging
        MEM_OUT_OF_RANGE  = 0x8, // Memory access is outside the range of the memory we currently have
        MEM_INVALID = 0x10, // Write in invaild portion of memory
        INVALID_WRITE = 0X20,
        INVALID_READ = 0x40, // Read from an invalid alignment, i.e reading from word which is not 4 aligned, half word from an uneven memory address
        MEM_UNALIGNED_READ = 0x80,
        MEM_UNALIGNED_WRITE = 0x100
    };
    protected:
    Word reg[REG_COUNT]; //These contain the general purpose registers. Registers are not byte addressable so we declare them as static array
    Memory memory; // the primary memory
    Word pc; // The program counter
    Word epc; // The exception program counter, is largely unused, stores the addres of the most recent instruction that caused an exception
    Word sr; // The status register whose value is the type of exception 
    const Word basePC; // the base  program counter, equal to memory.textStart
    size_t currentInsNumber;
    Word currentIns;
    bool isDebug; // set if the program is in debug mode
    size_t debugOffset; // only has value if program is in debug mode
    NumToNumMap lineMap; // Maps line number --> Instruction Code
    NumToNumMap codeMap; // Maps Instruction Code --> LineNumber
    NumToNumMap insNumMap; // Maps instrution number --> line Number
    std::vector < std::string > srcCode; // The souce code stored as srcCode[lineNumber] = Code at that line
    CodeToFunctionMap functions;
    void dumpRegister( AppendBuffer &);    
    void executeIns(Word);
    virtual void setException(ExceptionType type);
    static void arithmeticTest(Machine &);
    static void testBranch(Machine &);
    static void testProcedure(Machine &);
    public:
    static void test();
    void loadProgram(const char *buff, size_t size);
    bool load(const char *);
    void reset();
    void addFunctions();
    void execute();
    void dumpMem(const char *);
    void readDebugInfo( std::ifstream &file );
    // Following functions serve as interface for debugger
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
#define IMM(x) ( signExtend16( getBits(x,15,0) & 0xffff ) ) // get last sixteen bits and reduce to 16-bits
#define SHAMT(w) ( getBits(w,10,6) )

#ifndef ABS
#define ABS(x) ( ( ( x ) < 0 )?( -( x ) ):0 )
#endif
#define FUNC(w) ( getBits(w,5,0) )
inline size_t getBits( size_t x, size_t end, size_t start){
    return ( x >> (start) ) & ~( ~0 << ( end-start + 1 ) );
}


Word getWord(byte *);
Word getHalfWord(byte *);

struct ExceptionClassHash {
    template < typename T >
    std::size_t operator() ( T t ) const {
        return static_cast<size_t>(t);
    }
};

/*
 * The Request Exception class is used for exception handling whenever
 * there is an invalid request, or when a reques could not be completed
 * Generally only used by the debugger class
 */


struct MachineException {
    private:
    std::string str;
    byte type; // a bit-field denoting what type of errors were caused

    public:
    MachineException ( const char *, byte );
    void display() const;
};


enum ErrType{
    LDERR = 0x1, // Signals error during loading of program
    OPNERR= 0x2, // Signals error during opening of the file
    RDERR = 0x4, // Signals error during reading from the file
    MEMFULL = 0x8, // Signals if the memory is full
    LARGEPROG = 0x10, // Signals if the program is too large to fit in the memory
    MEMEND = 0x20, // Signals if trying to write/read past the end of allocated memory
};
void printRange(const byte *str, AppendBuffer &buff, size_t start, size_t end);

#define ERR_BUFF_SIZE 1024
#endif
