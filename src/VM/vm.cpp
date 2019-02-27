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

#include "vm.hpp"
#include "printBuffer.hpp"


#define DEBUG 1
#if 0
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
#endif
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


typedef std::unordered_map< size_t , std::string > exceptionToStrMap;
exceptionToStrMap exceptionMap;

void MachineException::display() const {
    for ( size_t i = 1; i <= 0x20; i <<= 1 ){
        if ( type & i ){
            std::cerr << exceptionMap[i] << std::endl;
        }
    }
    std::cerr << str << std::endl;
}
MachineException :: MachineException( const char *s, byte fields ):str( s ? s : "" ),type(fields){ }
#if 0
// Remove if decided that there will be no external debugging help
// Lets the machine be a 'truer' representation of an actual MIPS machine
std::ostream& operator << ( std::ostream &out, Machine::ExceptionType type ){
    #define PROCESS(x) case x : out << #x; break;
    // eg. PROCESS (Machine::OVERFLOW ) expands to 
    // case Machine::OVERFLOW: out << "Machine::OVERFLOW"; break;
    switch ( type ){
        PROCESS( Machine::ExceptionType::OVERFLOW ) 
        PROCESS( Machine::ExceptionType::INVALID )
        PROCESS( Machine::ExceptionType::TRAP ) 
        PROCESS( Machine::ExceptionType::MEM_OUT_OF_RANGE ) 
        PROCESS( Machine::ExceptionType::MEM_INVALID )
        PROCESS( Machine::ExceptionType::INVALID_WRITE )
        PROCESS( Machine::ExceptionType::INVALID_READ )
        PROCESS( Machine::ExceptionType::MEM_UNALIGNED_READ )
        PROCESS( Machine::ExceptionType::MEM_UNALIGNED_WRITE )
    }
    return out;
    #undef PROCESS
}
#endif
void Machine::setException(Machine::ExceptionType type){
    sr  |= static_cast<unsigned int>( type );
    epc = pc;
    //std::cerr<< "Exception name : " << type << std::endl;
    //std::cerr << "Exception info: " << exceptStr[type] << std::endl;
    //std::cerr << "Exception thrown by instruction: ";
    //if ( isDebug ){
    //    // if we are in debug mode show corresponding line
    //    size_t lineNum = insNumMap[ currentInsNumber ];
    //    std::cerr << srcCode[ lineNum ] << std::endl;
    //    std::cerr << "At line : " << std::dec << lineNum << std::endl; 
    //    
    //} else {
    //    std::cerr<< "0x" << std::hex << currentIns << std::endl;
    //}
    //std::cerr << std::endl;
}


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

Memory::Memory ( size_t bytes, size_t wordSize ):\
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

Memory :: ~Memory(){
    delete []vals;
}

Word Memory::endWord(){ 
    /* 
     * Here, ( byteCount - 1 ) is the array index of the final byte
     * in the memory array. ( Think about your 'normal' arrays )
     * Then we align the index down to an integer multiple of 4
     * i.e, we find the largest number divisible by 4 which is less than ( byteCount -  1 )
     * This gives us the position of the starting byte of the last word in the memory
     * 
     * ALIGN_DOWN  casts to a uint64. So we lower the result down to a 32-bit number
     */
    return  ( ALIGN_DOWN( byteCount - 1 , 4 ) ) & ( WORD_MAX );
}



inline void Memory::set(byte b ){
    memset( reinterpret_cast<void *>(vals), b, byteCount);
}



/*
 * ========================================
 * | The Endianess Circus |
 * ========================================
 */

Word getWord( byte *b ){
   #if 1 
    Word w = b[3];
    w = ( w << 8 ) | b[2];
    w = ( w << 8 ) | b[1];
    w = ( w << 8 ) | b[0];
    return w;
    #else 
    return *( reinterpret_cast<Word *>(b) );
    #endif
}

Word getHalfWord(byte *b){
    /*
     * Gives the sign extened 32-bit representation of the 
     * half word ( 16 -bit number ) stored in the memory pointed by b.
     * We assume that the number is stored in little endian format
     */
   #if DEBUG 
    Word w = static_cast<char>( b[1] );
    w = ( w << 8 ) | b[0];
    return w;
    #else 
    return static_cast<Word>( ( static_cast<char>( b[1] ) << 8 ) | b[0] ) ; 
    #endif
}


/* 
 * NOTE : We don't bother checking whether the numbers are 
 * signed or unsigned because addition ( and also subtraction )
 * are the same whether the numbers are signed or unsigned. i.e the bit patterns remains same
 * We use unsiged int32. We also dont check for overflows beacuse
 * unsigned integer overflow is properply defined in the C++ standard
 * as the unsigned addition is a modulo of ( UINT_MAX + 1)
 * i.e for any two unisgned 32 bit numbers a and b,
 * ( a + b ) is acutally calculated as ( a + b ) % ( UINT32_MAX + 1 )
 * which is a modulo of 2^32
 */


Machine::Machine (size_t bytes):\
    memory(bytes,WORD_SIZE),\
    pc(memory.textStart),\
    basePC(pc),\
    isDebug(false)\
    {
    addFunctions();
}

#define ADD_FUNCTION(opcode,func) ( functions[( opcode )] = &Machine::func )
void Machine::addFunctions( ){
    // For R-type instructions with opcode == 0, the function
    // is added into the map with the key 64 + func
    ADD_FUNCTION(64+0,sll);
    ADD_FUNCTION(64+32,addr);
    ADD_FUNCTION(64+33,addu);
    ADD_FUNCTION(64+34,subr);
    ADD_FUNCTION(64+35,subu);
    ADD_FUNCTION(64+36,andr);
    ADD_FUNCTION(64+37,orr);
    ADD_FUNCTION(64+38,xorr);
    ADD_FUNCTION(64+39,nor);
    ADD_FUNCTION(64+42,slt);
    ADD_FUNCTION(64+43,sltu);
    ADD_FUNCTION(64+8,jr);

    ADD_FUNCTION(2,jmp);
    ADD_FUNCTION(3,jal);
    ADD_FUNCTION(4,beq);
    ADD_FUNCTION(5,bne);
    ADD_FUNCTION(8,addi);
    ADD_FUNCTION(9,addiu);
    ADD_FUNCTION(10,slti);
    ADD_FUNCTION(11,sltiu);
    ADD_FUNCTION(12,andi);
    ADD_FUNCTION(13,ori);
    ADD_FUNCTION(14,xori);
    ADD_FUNCTION(32,lb);
    ADD_FUNCTION(33,lh);
    ADD_FUNCTION(35,lw);
    ADD_FUNCTION(36,lbu);
    ADD_FUNCTION(37,lhu);
    ADD_FUNCTION(40,sb);
    ADD_FUNCTION(41,sh);
    ADD_FUNCTION(43,sw);
#undef ADD_FUNCTION
}



void Machine::reset(){
    memset( reg, 0, sizeof(Word) * REG_COUNT );
    reg[SP] = memory.endWord();
    reg[GP] = memory.staticStart;
    pc = memory.textStart;
    auto start = memory.vals + memory.staticStart;
    auto bytes = memory.byteCount  - memory.staticStart;
    memset( start, 0, bytes );
//    memory.set(0);
}

void Machine::readDebugInfo(std::ifstream &inFile ){
    DebugSection debug;
    inFile.read( reinterpret_cast<char*>( &debug ), sizeof(DebugSection));
    LineMap *temp = new LineMap[ debug.lineMapCount * debug.lineMapSize ];
    inFile.read( reinterpret_cast<char*>(temp), debug.lineMapCount * debug.lineMapSize );
    for ( size_t i = 0; i < debug.lineMapCount; i++ ){
        LineMap &l = temp[i];
        codeMap[ l.ins ] = l.lineNum;
        insNumMap[ l.insNum ] = l.lineNum;
    }
    
    std::ifstream srcFile( debug.srcPath, std::ifstream::binary | std::ifstream::in );    
    if ( !inFile.good() || !inFile.is_open() ){
        const char *s = strerror( errno );
        std::cout << s << std::endl;
#if 0
        std::cerr << formatErr("%s:%s",debug.srcPath,s) << std::endl;
        std::cerr << "Execution will continue without any source information for display!" << std::endl;
#endif
    }
    std::string line;
    srcCode.push_back(""); // store a garbage value at position 0 ( which corresponds to line number 0)
    while ( getline( srcFile, line ) ){
        srcCode.push_back( line );
    }
    if ( srcFile.bad() ){
        perror("Unable to read the soure file : ");
    }
    delete temp;
}

#if 0

void Machine::loadProgram(const char *path){
    assert( path );
    std::ifstream inFile(path,std::ifstream::binary|std::ifstream::in);
    if( !inFile.is_open() || !inFile.good() ){
        const char *s = strerror( errno );
        throw MachineException(formatErr("%s : %s",path,s), LDERR|OPNERR );
        return ;
    }
    MainHeader mheader;
    ProgramHeader pheader;
    inFile.read((char *)&mheader, sizeof(MainHeader));
    inFile.read( (char *)&pheader, sizeof(ProgHeader));
    size_t len = pheader.progSize;
    /* 
     * textSize is number of words
     * Convert it to number of bytes by multiplying with 4
     */
    if ( len > ( memory.textSize << 2 ) ){
        throw MachineException(path,LDERR|LARGEPROG);
        return;
    }
    inFile.read(reinterpret_cast<char *>( memory.vals +  memory.textStart ), len );
    if ( mheader.dbgOffset ){
        // a debug offset has been specified i.e. program is 
        // in debug mode, so we run in debug mode
        isDebug = true;
        debugOffset = mheader.dbgOffset;
        readDebugInfo( inFile );
    }
    inFile.close();    
}

bool Machine::load( const char *path ){
    try {
        loadProgram(path);
    } catch ( MachineException &m ){
        m.display();
        return false;
    }
    memset( reg, 0, sizeof(Word) * REG_COUNT );
    reg[SP] = memory.endWord(); 
    reg[GP] = memory.staticStart;
    pc = memory.textStart;        
    return true;
}
#endif


#define NEW_EXCEPTION(a,b) ( exceptionMap[a] = std::string (b) )
void init(){
    NEW_EXCEPTION(LDERR,"Error while loading the file.");
    NEW_EXCEPTION(RDERR,"Unable to read file.");
    NEW_EXCEPTION(OPNERR,"Unable to open file.");
    NEW_EXCEPTION(MEMEND,"Trying to read/write past the memory");
    NEW_EXCEPTION(LARGEPROG,"Program is too large to load into the memory.");
}



void Machine::executeIns(Word w){
    currentIns = w;
    size_t op = getBits(w,31,26);
    if ( !op ){
        op = 64 + FUNC(w); // if opcode is zero, use 64 + func field as the key
    }
    if ( functions.count(op) ){ // equivalent to searching for op in the list of available function
        InsPointer fp = functions[op];
        ( this->*fp )();
    } else if ( op == 0x3fu ){ 
        // 0x3f is the breakpoint/trap signal
        setException( ExceptionType:: TRAP); // set the trap flag in exception register
        return;
    } else {
        setException( ExceptionType :: INVALID ); // set invalid instruction flag in exception register
        return;
    }

}
void Machine::execute(){
    currentIns = getWord( memory.vals + pc ); 
    while ( currentIns ){
        currentInsNumber = 1 + ( pc - basePC ) / 4;
        pc += 4;
        size_t op = getBits(currentIns,31,26);
        if ( !op ){
            op = 64 + FUNC(currentIns);
        }
        if ( functions.count(op) ){ // equivalent to searching for op in the list of available function
            InsPointer fp = functions[op];
            ( this->*fp )();
        } else if ( op == 0x3fu ){ 
            // 0x3f is the breakpoint/trap signal
            setException( ExceptionType:: TRAP); // set the trap flag in exception register
            return;
        } else {
            setException( ExceptionType :: INVALID ); // set invalid instruction flag in exception register
            return;
        }
        currentIns = getWord( memory.vals + pc ); 
    }
}



/*
 * Print bytes at str[start] to str[end] into the buffer buff
 */
void printRange(const byte *str, AppendBuffer &buff, size_t start, size_t end){
    int count = 1;
    for ( size_t i = start; i < end; i+=4 ){
        buff.append("0x%-5lx: ",i);
        for ( size_t k = 0; k < 4 ; k++ ){
            buff.append("%02x ",str[i + k] & 0xff );
        }
        buff.append("\t");
        count++;
        if ( count % 2 ) { count = 1; buff.append("\n");}
    }
    buff.append("\n\n"); 
}

#define HEADER(x) "\n=============================================\n| %s |\n=============================================\n",x

void Memory::dump(AppendBuffer &buffer){
    buffer.append(HEADER( "Reserved Segment" ));
    printRange( vals, buffer, 0, textStart );
    buffer.append(HEADER("Text Segment"));
    printRange( vals, buffer, textStart, staticStart );
    buffer.append(HEADER( "Static Data Segment" ));
    printRange( vals, buffer,staticStart,staticSize << 4 ); // the size is in words so we convert to bytes
    buffer.append(HEADER( "Heap Data segment" ));
    printRange( vals,buffer,heapStart, byteCount );
    buffer.append("\n");
}

char registerNames[][8] = {
    "zero","at",
    "v0","v1",
    "a0","a1","a2","a3",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", 
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8","t9",
    "k0","k1",
    "gp","sp","fp","ra" 
};

void dumpWord( AppendBuffer &buff, Word w ){
    buff.append("%02x %02x %02x %02x", w & 0xff, ( w >> 8 ) & 0xff , ( w >> 16 ) & 0xff , ( w >> 24 ) & 0xff );
}

void Machine::dumpRegister(AppendBuffer &buff){
    buff.append(HEADER("General Purpose Registers"));
    for ( size_t i = 0 ; i < 32 ; i++ ){
        buff.append("Register $%-5s: ",registerNames[i]);
        dumpWord(buff,reg[i]);
        buff.append("\n");
    }
}

void Machine::dumpMem(const char *path){
    AppendBuffer memBuff(1024 * 1024 ); // ~ 1 mb
    dumpRegister(memBuff);
    memory.dump( memBuff );
    std::ofstream outFile(path,std::ofstream::out|std::ofstream::binary );
    if ( !outFile.is_open() || !outFile.good() ){
        const char *s = strerror( errno );
        std::cout << "Unable to open file memDump" << std::endl;
        std::cout << s << std::endl;
        return ;
    }
    outFile.write( memBuff.buff, memBuff.len );
    outFile.close();
}

#define TEST_EQ(x,y,...)\
    do {\
        if ( ( x ) != ( y ) ) {\
            fprintf(stderr,__VA_ARGS__);\
            fprintf(stderr,"\n");\
        }\
    } while ( 0 )

void Machine::arithmeticTest(Machine &tm){
    tm.reset();
    tm.executeIns(0x20107fff);// Instruction : addi $s0,$zero,32767
    tm.executeIns(0x22100001);// Instruction : addi $s0,$s0,1
    TEST_EQ(tm.reg[S0],32767 + 1,"Instruction failed: \naddi $s0,$zero,32767\naddi $s0,$s0,1" );
    tm.reg[S0] =  WORD_MAX;
    tm.executeIns(0x22100001);// Instruction : addi $s0,$s0,1
    TEST_EQ(tm.reg[S0],0,"Failed overflow check for instruction addi");
    tm.reg[S0] = -5; 
    tm.executeIns(0x22100001);// Instruction : addi $s0,$s0,1
    TEST_EQ(static_cast<int32_t>( tm.reg[S0] ), -5 + 1 , "Instruction Failed: addi $s0,$s0,1 ( $s0 = %d )",(int32_t)(-5));
    tm.reg[S0] = 1; 
    tm.executeIns(0x108100); // Instruction : sll  $s0,$s0,4
    TEST_EQ(tm.reg[S0],1 << 4,"Failed Instruction : sll  $s0,$s0,4 " );
    tm.reg[S1] = 333;
    tm.executeIns(0x1187c0);// Instruction ::  sll  $s0,$s1,31
    TEST_EQ(tm.reg[S0],0,"Failed Overflow check for instruction sll");

    tm.reg[S2] = -213; tm.reg[S3] = 232;
    tm.executeIns(0x2534020); // Instruction :: addr $t0,$s1,$s2
    TEST_EQ(tm.reg[T0],-213+232,"Failed Instruction :addr $t0,$s1,$s2 ( $s1 = %d, $s2 = %d )",(int32_t)(-213), (int32_t)232);
    tm.reg[S3] = 12;
    tm.executeIns(0x2534020); // Instruction :: addr $t0,$s1,$s2
    TEST_EQ(static_cast<int32_t>( tm.reg[T0] ),-213+12,"Failed Instruction :addr $t0,$s1,$s2 ( $s1 = %d, $s2 = %d )",(int32_t)-213,(int32_t)232);

    tm.reg[S1] = -213; tm.reg[S2] = 12;
    tm.executeIns(0x2324021); // Instruction :: addu $t0,$s2,$s3
    TEST_EQ(tm.reg[T0],(Word)(-213)+12,"Failed Instruction :addu $t0,$s1,$s2 ( $s2 = %u, $s3 = %u )",-213,12);
    tm.executeIns(0x2324022); // Instruction :: subr $t0,$s1,$s2
    TEST_EQ((int32_t)tm.reg[T0],-213-12,"Failed instruction: subr $t0,$s1,$s2( $s1 = %d, $s2 = %d )", (int32_t)-213, (int)232 );    
    tm.executeIns(0x2324023); // Instruction :: subu $t0,$s1,$s2
    TEST_EQ(tm.reg[T0],(Word)(-213)-12,"Failed instruction: subr $t0,$s1,$s2( $s1 = %u, $s2 = %u )", -213, 232 );    
    
    tm.reg[T2] = 0xff34; tm.reg[T3] = 0x3f3f;
    tm.executeIns(0x14b4024); // Instruction 
    TEST_EQ(tm.reg[T0],tm.reg[T2]&tm.reg[T3],"Failed instruction: and $t0,$t2,$s3( $t1 = %u, $t2 = %u )",tm.reg[T2], tm.reg[T3]);    
    tm.executeIns(0x14b4025); // Instruction or $t0,$t2,$t3
    TEST_EQ(tm.reg[T0],tm.reg[T2]|tm.reg[T3],"Failed instruction: or $t0,$t2,$s3( $t1 = %u, $t2 = %u )", tm.reg[T2], tm.reg[T3] );    
    tm.executeIns(0x14b4026); // Instruction xor $t0,$t2,$t3
    TEST_EQ(tm.reg[T0],tm.reg[T2]^tm.reg[T3],"Failed instruction: xor $t0,$t2,$s3( $t1 = %u, $t2 = %u )", tm.reg[T2], tm.reg[T3] );    
//    tm.executeIns(); // Instruction nor $t0,$t2,$t3
//    TEST_EQ(tm.reg[T0],~(tm.reg[T2]|tm.reg[T3]),"Failed instruction: nor $t0,$t2,$s3( $t1 = %u, $t2 = %u )", tm.reg[T2], tm.reg[T3] );    
    
    tm.reg[T5] = 23; tm.reg[T6] = -34;
    tm.executeIns(0x1ae402a); // Instruction slt $t0,$t5,$t6
    TEST_EQ(tm.reg[T0],0,"Failed Instruction: slt $t0,$t5,$t6 ( $t5 = 23, $t6=-34 )");
    tm.executeIns(0x1cd402a); // Instruction slt $t0,$t6,$t5
    TEST_EQ(tm.reg[T0],1,"Failed Instruction: slt $t0,$t5,$t6 ( $t5 = 23, $t6=-34 )");
    tm.executeIns(0x1cd402b); // Instruction sltu $t0,$t6,$t5
    TEST_EQ(tm.reg[T0],0,"Failed Instruction: sltu $t0,$t5,$t6 ( $t5 = 23, $t6=-34 )");
    tm.executeIns(0x29c8000d); // Instruction slti $t0,$t6,13
    TEST_EQ(tm.reg[T0],1,"Failed Instruction: slti $t0,$t6,13 ( $t6=-34 )");
    tm.executeIns(0x2dc8000d); // Instruction sltiu $t0,$t6,13
    TEST_EQ(tm.reg[T0],0,"Failed Instruction: sltiu $t0,$t6,13 ( $t6=-34 )");

    tm.reg[S0] = 0x37c;
    tm.memory.vals[ 0x37c ] = 0xf2;
    tm.memory.vals[ 0x37c + 1 ] = 0x1e;
    tm.memory.vals[ 0x37c + 2 ] = 0xab;
    tm.memory.vals[ 0x37c + 3 ] = 0xfd;


    tm.executeIns(0x82090001); // Instruction lb $t1,1($s0)
    TEST_EQ(tm.reg[T1],0x1e,"Failed Instruction: lb $t1,0($s0)");
    tm.executeIns(0x82090000); // Instruction lb $t1,0($s0)
    TEST_EQ(tm.reg[T1],0xfffffff2,"Failed Instruction: lb $t1,0($s0)");
    tm.executeIns(0x92090000); // Instruction lbu $t1,0($s0)
    TEST_EQ(tm.reg[T1],0xf2,"Failed Instruction: lbu $t1,0($s0)");
    tm.executeIns(0x92090003); // Instruction lbu $t1,3($s0)
    TEST_EQ(tm.reg[T1],0xfd,"Failed Instruction: lbu $t1,3($s0)");


    tm.executeIns(0x86090002); // Instruction lh $t1,2($s0):
    TEST_EQ(tm.reg[T1],0xfffffdab,"Failed Instruction: lh $t1,2($s0)"); 
    tm.executeIns(0x86090000); // Instruction lh $t1,0($s0):
    TEST_EQ(tm.reg[T1],0x1ef2,"Failed Instruction: lh $t1,0($s0)"); 
    tm.executeIns(0x96090000); // Instruction lhu $t1,0($s0):
    TEST_EQ(tm.reg[T1],0x1ef2,"Failed Instruction: lh $t1,0($s0)"); 
    tm.executeIns(0x8e090000); // Instruction lw $t1,0($s0)
    TEST_EQ(tm.reg[T1],0xfdab1ef2,"Failed Instruction: lh $t1,0($s0)");

    tm.reg[T1] = 0xabcdef12;
    tm.executeIns(0xa2090004); // Instruction sb $t1,4($s0)
    TEST_EQ(tm.memory.vals[0x37c + 4 ],0x12,"Failed Instruction sb $t1,4($s0)");
    tm.executeIns(0xa6090006); // Instruction sh $t1,6($s0)
    TEST_EQ(getHalfWord(tm.memory.vals+0x37c+6) & UINT16_MAX, 0xef12u, "Failed Instruction sh $t1,4($s0)" );
    tm.executeIns(0xae090008); // Instruction sw $t1,8($s0)
    TEST_EQ(getWord(tm.memory.vals + 0x37c + 8), 0xabcdef12u, "Failed Instruction sw $t1,8($s0)");
    
    tm.executeIns(0x960d0006); // Instruction lhu $t5,6($s0)
    TEST_EQ(tm.reg[T5],0xef12u,"Failed to load the saved half word" );
    tm.executeIns(0x8e0e0008); // Instruction lw $t6,8($s0)
    TEST_EQ(tm.reg[T6],0xabcdef12u,"Failed to load the saved word" );



}


void Machine::testBranch(Machine &test){
//    test.reset();
    Memory &mem = test.memory;
    Word ins[] = {
        0x20100001, // addi $s0,$zero,1
        0x2a110006,    // Start: slti $s1,$s0,5
        0x22100001,    // addi $s0,$s0,1
        0x1620fffd,    // bne  $s1,$zero,Start
        0x32090001, //  andi $t1,$s0,1
        0x11200007, //  beq  $t1,$zero,Even
        0x200a006f, // addi $t2,$zero,0x6f
        0xa38a0000, // sb   $t2,0($gp)
        0x200a0064, //  addi $t2,$zero,0x64
        0xa38a0001, // sb   $t2,1($gp)
        0x200a0064, // addi $t2,$zero,0x64
        0xa38a0002, // sb   $t2,2($gp)
        0x8000015 , //  jmp  Exit
        0x200a0065, // addi $t2,$zero,0x65
        0xa38a0000, // sb   $t2,0($gp)
        0x200a0076, // addi $t2,$zero,0x76
        0xa38a0001, // sb   $t2,1($gp)
        0x200a0065, // addi $t2,$zero,0x65
        0xa38a0002, // sb   $t2,2($gp)
        0x200a006e, // addi $t2,$zero,0x6e
        0xa38a0003, // sb   $t2,3($gp)
    };
    memcpy(mem.vals+mem.textStart,(char *)ins,sizeof(ins));
    test.execute();
    TEST_EQ(test.reg[S0],7,"Branching failed");
}

void Machine::testProcedure(Machine &test){
    // Test for jal and jr functionality
    test.reset();
    Memory &mem = test.memory;
    /* BELOW PROGRAM
     *   addi $a0,$zero,3
     *   addi $a1,$zero,4
     *   jal Add
     *   sw $v0,0($gp)
     *   jmp Exit
     *   Add:
     *   addr $v0, $a0,$a1
     *   jr $ra
     *   Exit:
     */
    Word ins[] = {
        0x20040003,
        0x20050004,
        0xc000005, 
        0xaf820000,
        0x8000007,
        0x851020,
        0x3e00008,
    };
    memcpy(mem.vals+mem.textStart,(char *)ins,sizeof(ins));
    test.execute();
    Word val = mem.vals[mem.staticStart];
    TEST_EQ(val,7,"Procedure Test Failed. Something wrong with jal and jr instructions.");
}

void Machine::test(){
    Machine testMachine(512* sizeof(Word));
    arithmeticTest(testMachine);
    testBranch(testMachine);
//    testMachine.dumpMem("./memDump.dump");
    testProcedure(testMachine);
}



void Machine::loadProgram(const char *text, size_t size ){
    // reads size byte from text, checking for errors
    // size is in bytes while textSize is in number of words
    // we convert size to number of words
    if ( (size >> 2 ) >= (memory.textSize ) ){
        throw MachineException("Program is too long for the allocated text Area.",LARGEPROG);
    }
    memcpy( memory.vals + memory.textStart, text, size );
    memset( reg, 0, sizeof(Word) * REG_COUNT );
    reg[SP] = memory.endWord(); 
    reg[GP] = memory.staticStart;
    pc = memory.textStart;        
}
