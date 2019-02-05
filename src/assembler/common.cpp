#include "common.hpp"
using std::string;
strToBoolMap keywordMap;
strToInsMap insMap;
strToIntMap regMap;
charToIntMap digitMap;

#define NEW_INS(NAME,OP,FUNC,KIND,CLASS) ( Instruction ( string (#NAME), OP, FUNC, Instruction::KIND, Instruction::CLASS) ) 
#define ADD_INS(NAME,OP,FUNC,KIND,CLASS) insMap[#NAME] = NEW_INS(NAME,OP,FUNC,KIND,CLASS) 
#define ADD_INSR(NAME,FUNC,KIND) insMap[#NAME] = NEW_INS(NAME,0,FUNC,KIND,RTYPE); keywordMap[#NAME] = true 
#define ADD_INSI(NAME,OP,KIND) insMap[#NAME] = NEW_INS(NAME,OP,0,KIND,ITYPE); keywordMap[#NAME] = true 
#define ADD_INS_JMP(NAME,OP,KIND) insMap[#NAME] = NEW_INS(NAME,OP,0,KIND,JTYPE); keywordMap[#NAME] = true 
#define ADD_INS_BR(NAME,OP,KIND) insMap[#NAME] = NEW_INS(NAME,OP,0,KIND,BRTYPE); keywordMap[#NAME] = true 
#define ADD_INSP(NAME,OP,FUNC,KIND) insMap[#NAME] = NEW_INS(NAME,OP,FUNC,KIND,PTYPE); keywordMap[#NAME] = true;
void initIns( ){
    ADD_INSR(sll,0,AL);
    ADD_INSR(addr,32,AL);
    ADD_INSR(addu,33,AL);
    ADD_INSR(subr,34,AL);
    ADD_INSR(subu,35,AL);
    ADD_INSR(and,36,AL);
    ADD_INSR(or,37,AL);
    ADD_INSR(xor,38,AL);
    ADD_INSR(nor,39,AL);
    ADD_INSR(slt,42,AL);
    ADD_INSR(sltu,43,AL);
 
    ADD_INSP(jr,0,8,JUMP);   
    
    ADD_INSI(addi,8,AL);
    ADD_INSI(addiu,9,AL);
    ADD_INSI(slti,10,AL);
    ADD_INSI(sltiu,11,AL);
    ADD_INSI(andi,12,AL);
    ADD_INSI(ori,13,AL);
    ADD_INSI(xori,14,AL);

    ADD_INSI(lb,32,LS);
    ADD_INSI(lh,33,LS);
    ADD_INSI(lw,35,LS);
    ADD_INSI(lbu,36,LS);
    ADD_INSI(lhu,37,LS);

    ADD_INSI(sb,40,LS);
    ADD_INSI(sh,41,LS);
    ADD_INSI(sw,43,LS);

    ADD_INS_BR(beq,4,BRANCH);
    ADD_INS_JMP(jmp,2,JUMP);
    ADD_INS_JMP(jal,3,JUMP);
    ADD_INS_BR(bne,5,BRANCH);
}
#undef NEW_INS
#undef ADD_INS
#undef ADD_INSR
#undef ADD_INSI
void initKeyword(){
    #define ADD_KEYWORD(X) ( keywordMap[#X] = true )
    ADD_KEYWORD(addr);
    ADD_KEYWORD(addu);
    ADD_KEYWORD(sll);
    ADD_KEYWORD(subr);
    ADD_KEYWORD(subu);
    ADD_KEYWORD(and);
    ADD_KEYWORD(or);
    ADD_KEYWORD(xor);
    ADD_KEYWORD(xnor);
    ADD_KEYWORD(slt);
    ADD_KEYWORD(sltu);
    ADD_KEYWORD(addi);
    ADD_KEYWORD(addiu);
    ADD_KEYWORD(slti);
    ADD_KEYWORD(sltiu);
    ADD_KEYWORD(andi);
    ADD_KEYWORD(ori);
    ADD_KEYWORD(xori);

    ADD_KEYWORD(lb);
    ADD_KEYWORD(lh);
    ADD_KEYWORD(lw);
    ADD_KEYWORD(lbu);
    ADD_KEYWORD(lhu);
    ADD_KEYWORD(sb);
    ADD_KEYWORD(sh);
    ADD_KEYWORD(sw);
    #undef ADD_KEYWORD
}

void initRegisters(){
    regMap["$zero"] = 0;
    regMap["$at"] = 1;
    regMap["$v0"] = 2;
    regMap["$v1"] = 3;
    regMap["$a0"] = 4;
    regMap["$a1"] = 5;
    regMap["$a2"] = 6;
    regMap["$a3"] = 7;

    regMap["$t0"] = 8;
    regMap["$t1"] = 9;
    regMap["$t2"] = 10;
    regMap["$t3"] = 11;
    regMap["$t4"] = 12;
    regMap["$t5"] = 13;
    regMap["$t6"] = 14;
    regMap["$t7"] = 15;

    regMap["$s0"] = 16;
    regMap["$s1"] = 17;
    regMap["$s2"] = 18;
    regMap["$s3"] = 19;
    regMap["$s4"] = 20;
    regMap["$s5"] = 21;
    regMap["$s6"] = 22;
    regMap["$s7"] = 23;

    regMap["$t8"] = 24;
    regMap["$t9"] = 25;
    regMap["$k0"] = 26;
    regMap["$k1"] = 27;

    regMap["$gp"] = 28;
    regMap["$sp"] = 29;
    regMap["$fp"] = 30;
    regMap["$ra"] = 31;
}
void initialize_assembler( ){
    initIns();
    initRegisters();
    digitMap['0'] = 0;
    digitMap['1'] = 1;
    digitMap['2'] = 2;
    digitMap['3'] = 3;
    digitMap['4'] = 4;
    digitMap['5'] = 5;
    digitMap['6'] = 6;
    digitMap['7'] = 7;
    digitMap['8'] = 8;
    digitMap['9'] = 9;
    digitMap['A'] = 10;
    digitMap['B'] = 11;
    digitMap['C'] = 12;
    digitMap['D'] = 13;
    digitMap['E'] = 14;
    digitMap['F'] = 15;
    digitMap['a'] = 10;
    digitMap['b'] = 11;
    digitMap['c'] = 12;
    digitMap['d'] = 13;
    digitMap['e'] = 14;
    digitMap['f'] = 15;
    
}


void *xmalloc( size_t size ){
    void *x = malloc( sizeof(char) * size );
    if ( x ){
        return x;
    } else {
        perror("Malloc failed!");
        return NULL;
    }
}

void *xcalloc( size_t num ,size_t bytes ){
    void *x = calloc( num ,bytes );
    if ( x ){
        return x;
    } else {
        perror("Calloc Failed!");
        return NULL;
    }
}

void *xrealloc( void *prev, size_t newLen ){
    void *x = realloc( prev, newLen );
    if ( x ){
        return x;
    } else {
        perror("Realloc failed\n");
        return NULL;
    }
}

AppendBuffer::AppendBuffer () :buff(NULL), len(0), cap(0) {}

AppendBuffer::~AppendBuffer(){
    free( buff );
    len = 0; cap = 0;
}


AppendBuffer::AppendBuffer (size_t x) : len(0), cap(x){
    buff = (char * )xmalloc( sizeof(char) * x );
    assert( buff );
}

void AppendBuffer::alloc( ){
    len = 0; cap = DEFAULT_SIZE ;
    buff = ( char * )malloc( sizeof ( char ) * DEFAULT_SIZE);
}

void AppendBuffer::grow( size_t size ){
    char *newBuff = ( char * )xrealloc(buff,size);
    if ( newBuff ){
        buff = newBuff;
        cap = size;
    } else {
        std::cerr << "Unable to allocate memory" << std::endl;
    }
}

char *AppendBuffer::append( const char *fmt , ... ){
    if ( !buff || !cap ){
        alloc( );
    }
    assert(buff);
    char *end = buff + len ; // position of the NULL terminator in the string
    va_list args;
    va_start( args, fmt );
    size_t printLen = vsnprintf(NULL,0,fmt,args);
    if ( len + printLen + 1 >= cap ){
        grow(2 * ( len + printLen + 1 ) );
        //buff = (char * )realloc(buff, 2 * ( len + printLen + 1 ) );
        //assert( buff );
        //cap = 2 * ( len + printLen  + 1 );
    }
    assert( len + printLen < cap );
    size_t freeSpace = cap-len;
    va_start(args,fmt);
    printLen = vsnprintf(end,freeSpace,fmt,args);
    if ( printLen >= freeSpace ){
        std::cerr << "Unable to print buffer " << std::endl;
        return NULL;
    }
    len += printLen;
    va_end(args);
    return buff; 
}

char *AppendBuffer::appendn( size_t size, const char *x ){ 
    // print size bytes from x into the buffer
    // The buffer is not zero terminated by this function
    if ( !buff || !cap ){
        alloc();
    } else 
    assert(buff);
    if ( len + size >= cap ){
        size_t s = ( len + size > 2 * cap ) ? ( len  + size ) : ( 2 * cap );
        grow(s);
    }
    assert( len + size < cap );
    char *end = buff + len;
    snprintf(end,size,"%s",x);
    len += size;
    return buff;
}