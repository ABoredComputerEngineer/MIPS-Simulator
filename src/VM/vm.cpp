/*
 *------------------------------------------
 *| POINTS TO REMEMBER |
 *------------------------------------------
 * All the multiplication and divisions are ( mostly ) powers of two
 * So we use bit-shifting all over the place.
 * REMEMBER: 
 * x << n is the same as x*(2^n) ( where ^  is the exponential operator )
 * x >> n is the same as x/(2^n)
 */

#include <unordered_map>
#include <string>
#include <functional>
#include <cassert>
#include <cinttypes>
#include <cstring>
#include <fstream>
#include <iostream>
#include <climits>
#include <cstdarg>
#include <cstdio>
typedef uint8_t byte;
typedef uint32_t Word;
char *errBuff;
#define ERR_BUFF_SIZE 1024
char *formatErr( const char *fmt, ... ){
    if ( !errBuff ){
        std::cerr << "Error Buffer not initialized!" << std::endl;
        exit(1);
    } 
    va_list args;
    va_start(args,fmt);
    size_t len = vsnprintf(errBuff,ERR_BUFF_SIZE,fmt,args);
    if ( len >= ERR_BUFF_SIZE ){
        std:: cerr << "Error Buffer is not long enough!" << std::endl;
        exit(1);
    }
    va_end(args);
    return errBuff;
}
class Machine;
enum IntegerLimits {
    MAX_32 = 0xffffffff, // Equal to UINT32_MAX
};
struct FileException {
    private:
    std::string str;
    public:
    FileException ( const char *s ): str( s ){}
    FileException ( const std::string &s ): str( s ){}
    const char *getError( ){
        return str.c_str();
    }  
};


enum ExceptionType {
    LDERR = 0x1, // Signals error during loading of program
    OPNERR= 0x2, // Signals error during opening of the file
    RDERR = 0x4, // Signals error during reading from the file
    MEMFULL = 0x8, // Signals if the memory is full
    LARGEPROG = 0x10, // Signals if the program is too large to fit in the memory
    MEMEND = 0x20, // Signals if trying to write/read past the end of allocated memory
};
typedef std::unordered_map< size_t , std::string > exceptionToStrMap;
exceptionToStrMap exceptionMap;
struct MachineException {
    private:
    std::string str;
    byte type; // a bit-field denoting what type of errors were caused

    public:
    MachineException ( const char *, byte );
    void display() const {
        for ( size_t i = 1; i <= 0x20; i <<= 1 ){
            if ( type & i ){
                std::cerr << exceptionMap[i] << std::endl;
            }
        }
        std::cerr << str << std::endl;
    }
};

MachineException :: MachineException( const char *s, byte fields ):str( s ? s : "" ),type(fields){ }

/*
 * 'aligns' the number down to the nearest integer multiple of given number, which must be a power of two
 * ALIGN_DOWN(33,32) = 32 ,
 * ALIGN_DOWN(65,32) = 64, 
 * ALIGN_DOWN( 97, 32) = 96

 * We achieve this by simply setting the lower lg(p) ( log base 2 ) bits of x to zero
 */


#define ALIGN_DOWN(x,p) ( static_cast<uint64_t>(x) & ~( ( p ) - 1 ) )
#define ALIGN_UP(x,p) ( ALIGN_DOWN( static_cast<uint64_t>( x ) + ( p ) - 1, ( p )) )
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

    /*
     *==================================================
     *| ABOUT THE MEMORY CONSTRUCTION | 
     *==================================================
     * All the segments of the memory are calculated relative to the total memory that is allocated
     * 
     * byteCount is simply the size of the entire memory in bytes. We align it up to 32 bytes so that 
     * there will always be an integer number of words 
     *
     * --------------------
     * | Text Segment |
     * --------------------
     * textStart is the starting byte index of the text segment in the memory array.
     * We set the text segment to start at ( 1/16 )th index of the total byte index
     * Calculation is as follows:
     *      1. The total number of bytes is first divided by 16 to get the offset from the starting point
     *      2. We decrease it by 1 to get the index in the 'vals' array from which the segment will start
     *      3. We then 'align' the calculated index down to 4 bytes so that the text segment always begins at
     *          the begining of a word 
     *          ( i.e it prevents the segment lower than it from having a non-integer number of words ) 
     *  textSize is the total number of words allocated for the text segment.
     *  It is set to ( 1 / 4 )th of the total number of words available in the memory
     *  Calculation is as follows:
     *      1. First, we calculate the total number of words present in the entire memory ( byteCount / 4 )
     *      2. Then , we calculate ( 1/4 )th of the word count
     *      3. The entire expression reduces to 
     *         ( ( byteCount/ 4 )/4 ) => ( byteCount/16 ) <=> ( byteCount << 4 ) => ( byteCount << ( 2 + 2 ) ) 
     * 
     * --------------------
     * | Data Segment |
     * --------------------
     * For the data segment we only set the boundaries for static Data segment.
     * The heap data segment is set to begin immediately after the static data segment
     * 
     * staticStart is the starting byte index of the static data segment in the memory array
     * Calculation is as follows:
     *      1. First, we get the size of the text segment in bytes. The textSize is in terms of words.
     *         So, we multiply by 4 to get the proper number of bytes
     *      2. We then add it to the starting index of the text segment to get the starting index
     * The ALIGN_UP is generally not need because textStart is 4-byte(32-bit) aligned and the 
     * textSize is in word count which means that the number of bytes is also going to a muliple of 4
     * However, we just add it 
     *  
     * The size of the static segment is set at ( 1 / 8 )th the total number of words in the memory
     * The calculation is similar to that of textSize calculation
     * 
     * HeapStart is the starting byte index of the Heap ( dynamic data segment )
     * It is unbounded in size
     * heapStart is immediately set to the next byte after the static data segment
     * The calculation is similar to staticStart
     */
    Memory ( size_t bytes, size_t wordSize ):\
        byteCount(ALIGN_UP(bytes,wordSize)),\
        textStart( ALIGN_DOWN( ( byteCount >> 4 ) - 1,4 ) ),\
        textSize( byteCount >> ( 2 + 2 ) ),\
        staticStart( ALIGN_UP(textStart + ( textSize << 2 ),4) ),\
        staticSize( (byteCount >> ( 2 + 3 ) ) ),\
        heapStart( ALIGN_UP( staticStart + ( staticSize << 2 ) , 4) )\
    {
        vals = new byte[ byteCount ]; 
        memset(vals,0,byteCount);

        assert( byteCount );
        assert( textStart );
        assert( textSize );
        assert( staticStart );
        assert( staticSize );
        assert( heapStart );
    }

    Word endWord(){ 
        /* 
         * Here, ( byteCount - 1 ) is the array index of the final byte
         * in the memory array. ( Think about your 'normal' arrays )
         * Then we align the index down to an integer multiple of 4
         * i.e, we find the largest number divisible by 4 which is less than ( byteCount -  1 )
         * This gives us the position of the starting byte of the last word in the memory
         * 
         * ALIGN_DOWN  casts to a uint64. So we lower the result down to a 32-bit number
         */
        return  ( ALIGN_DOWN( byteCount - 1 , 4 ) ) & ( MAX_32 );
    }
    ~Memory( ){
        free( vals );
    }
    inline void set(byte); // sets the memory contents to the given  word (uint32)
    friend class Machine;
};

inline void Memory::set(byte b ){
    memset( reinterpret_cast<void *>(vals), b, byteCount);
}



typedef std::function< void (Word) > InstructionFunc;
typedef std::unordered_map< size_t , InstructionFunc > CodeToFunctionMap; // your array of function pointers



class Machine {
    enum Registers{
        ZERO = 0,
        V0 = 2, V1,
        A0, A1, A2, A3,
        T0, T1, T2, T3, T4, T5, T6, T7,
        S0, S1, S2, S3, S4, S5, S6, S7,
        T8,T9,
        GP, SP, FP, RA
    };
    enum MachineSpec {
        REG_COUNT = 32,
        WORD_SIZE = 32,
    };
    private:
    Word reg[REG_COUNT]; // Registers are not byte addressable so we declare them as static array
    Memory memory; // the primary memory
    Word pc; // The program counter
    CodeToFunctionMap functions;
    public: 
    void loadProgram(const char *);
    void load(const char *);
    void reset();
    inline void addFunction(size_t, InstructionFunc );
    Machine (size_t bytes ):memory(bytes,WORD_SIZE){}
    void execute();
};

inline void Machine::addFunction( size_t opcode, InstructionFunc f ){
    functions[opcode] = f;
}

void Machine::reset(){
    memset( reg, 0, sizeof(Word) * REG_COUNT );
    memory.set(0);
}

void Machine::loadProgram(const char *path){
    assert( path );
    std::ifstream inFile(path,std::ifstream::binary|std::ifstream::in);
    if( !inFile.is_open() || !inFile.good() ){
        const char *s = strerror( errno );
        throw MachineException(formatErr("%s : %s",path,s), LDERR|OPNERR );
        return ;
    }
    inFile.seekg(0,std::ios_base::end); 
    size_t len = inFile.tellg();
    /* 
     * textSize is number of words
     * Convert it to number of bytes by multiplying with 4
     */
    if ( len > ( memory.textSize << 2 ) ){
        throw MachineException(path,LDERR|LARGEPROG);
        return;
    }
    inFile.seekg(0);
    inFile.read(reinterpret_cast<char *>( memory.vals +  memory.textStart ), len );
    inFile.close();    
}

void Machine::load( const char *path ){
    try {
        loadProgram(path);
    } catch ( MachineException &m ){
        m.display();
        return;
    }
    memset( reg, 0, sizeof(Word) * REG_COUNT );
    reg[SP] = memory.endWord(); 
    pc = memory.textStart;        
}

Word getWord( byte *b ){
   #if 1 
    Word w = b[3];
    w = ( w << 8 ) | b[2];
    w = ( w << 8 ) | b[1];
    w = ( w << 8 ) | b[0];
    return w;
   #endif 
//    return *( reinterpret_cast<Word *>(b) );
}

size_t getBits( size_t x, size_t start, size_t end ){
    return ( x >> (start) ) & ~( ~0 << ( end-start + 1 ) );
}

void Machine::execute(){
    Word instruction = getWord( memory.vals + pc );
    size_t op = getBits(instruction,26,31);
    size_t rs = getBits(instruction,21,25);
    size_t rt = getBits(instruction,16,20);
    (void)op;(void)rs;(void)rt;
}


#define NEW_EXCEPTION(a,b) ( exceptionMap[a] = std::string (b) )
void init(){
    NEW_EXCEPTION(LDERR,"Error while loading the file.");
    NEW_EXCEPTION(RDERR,"Unable to read file.");
    NEW_EXCEPTION(OPNERR,"Unable to open file.");
    NEW_EXCEPTION(MEMEND,"Trying to read/write past the memory");
    NEW_EXCEPTION(LARGEPROG,"Program is too large to load into the memory.");
}

void addi(Word w){
    size_t rs = getBits(w,21,25);
    (void)rs;
}


int main(int argc, char *argv[] ){
    if ( argc < 2 ){
        std::cout << "Please input a file" << std::endl;
        return 0;
    }
    errBuff = new char[ERR_BUFF_SIZE];
    init();
    assert( ALIGN_UP(4,32) == 32 );
    assert( ALIGN_DOWN(127,32) == 96 );
    assert( ALIGN_DOWN(31,4) == 28 );
    Machine m(1024 * sizeof(Word) );
    m.load(argv[1]);
    m.execute();
    delete errBuff;
    (void)m;
}