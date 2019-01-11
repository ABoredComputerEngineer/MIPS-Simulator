#include <vector>
#include <cstdint>
#include <cassert>
// TODO : Maybe not use std::vector?
typedef int32_t Code;
#define ORIGIN 80000 
enum Limits {
    RS_LEN = 5,
    RT_LEN = 5,
    RD_LEN = 5,
    SHAMT_LEN = 5,
    FUNC_LEN = 6,
    IMM_LEN = 16,
    ADDRESS_LEN = 26,
    FIVE_BIT_MAX = 0x1fu,
    SIX_BIT_MAX = 0x3fu,
    SIXTEEN_BIT_MAX = 0xffffu,
    TWENTY_SIX_BIT_MAX = 0x3ffffffu,
    FIFTEEN_BIT_MAX = 0x7fffu,
    //INT16_MAX = 0x7fff, // 32768
    //INT16_MIN = -0x8000, // -32768
};
#define UINT32_CAST( x ) ( static_cast<uint32_t>( x ) )
#define ABS(x) ( ( (x) > 0 ) ? (x) : ( -(x) ) )
#define IS_UNSIGNED_5(x) ( !( (x) & ~FIVE_BIT_MAX ) )
#define IS_UNSIGNED_6( x ) ( !( (x) & ~SIX_BIT_MAX ) )
#define IS_UNSIGNED_16( x ) ( !( (x) & ~SIXTEEN_BIT_MAX ) )
#define IS_UNSIGNED_26( x ) ( !( (x) & ~TWENTY_SIX_BIT_MAX ) )
#define IS_SIGNED_16(x) ( ( ( x ) >= INT16_MIN ) && ( ( x ) <= INT16_MAX ) )
using std::vector;
class Generator {
    vector <Code> prog;
    vector <ParseObj *> objs;
    const char *file;
    bool parseSuccess;
    size_t totalIns;
    Code encodeRtype(const ParseObj *);
    Code encodeItype( const ParseObj *);
    Code encodeBranch( const ParseObj *);
    Code encodeJump( const ParseObj *);
    Code encodeObj( const ParseObj * );
    bool parseFile();
    void encode();
    void displayError(const ParseObj *,const char *fmt,...);
    bool resolveBranch(const ParseObj *);
    bool resolveJump(const ParseObj *);
    ~Generator () {
        for ( auto iter = objs.begin(); iter != objs.end(); iter++ ){
            delete *iter;
        }
    }
    public:
    Generator (const char *f ):file(f),parseSuccess(0),totalIns(0){
        prog.reserve( 100 );
        objs.reserve( 100 );
    }
    static void test();
};


/*
 * -------------------------------------------------
 * | BRANCH OFFSET AND JUMP ADDRESS CALCULATION |
 * -------------------------------------------------
 * We calculate the word offset here.
 * The word offset is a signed 16-bit integer
 * Positive offsets denote a forward jump
 * Negative offsets denote a backward jump
 * When calculating we calculate the offset relative to the next instruction
 * The word offset is the number of words we have to pass 
 * from the END of the current instruction to the instruction after branching Label
 * which is the reason why there is no extra -1 
 * 
 * Jump addresses are absolute 
 * The address is an unsigned sixteen bit number
 * Similar to branch they are word addresses i.e they always point to a begining of a valid instruction
 * i.e The address is later multiplied by 4 to get the actual address of the instruction 
 */

bool Generator:: resolveBranch(const ParseObj *p) {
    if ( p->props.Branch.label ){
        auto search = labelMap.find( string (p->props.Branch.label) );
        if ( search == labelMap.end() ){ // Label not found
            displayError( p, "Label %s not found!", p->props.Jump.label );
            return false;
        } else {
            p->props.Branch.offset = ( search->second - p->insNumber );
        }
    }
    if ( !IS_SIGNED_16( p->props.Branch.offset ) ){
        displayError(p, "Branch offset address is too large/small for a 16-bit field( min %d, max %d ).", INT16_MIN, INT16_MAX );
        return false; 
    }
    return true;
}

bool Generator :: resolveJump(const ParseObj *p){
    if ( p->props.Jump.label ){
        auto search = labelMap.find( string ( p->props.Jump.label) );
        if ( search == labelMap.end() ){
            displayError( p , "Label %s not found!", p->props.Jump.label );
            return false;
        } else {
            size_t tmp = search->second  + ( ORIGIN >> 2 ) ; 
            /* 
             * The word address is calculated as follows:
             * search->second is the instruction number of the instruction imediately followed by the loop.
             * The number is there even if there is no instruction after the loop ( in this case the program simply terminates )
             * Then we add the memory address of the first instruction of the program ( ORIGIN ).
             * We also divide by 4 ( rshift by 2 ) to get the correct word address of the instruction. 
             * The machine running it will multiply by 4 to get the correct address ( hopefully ).
             */
            p->props.Jump.addr = tmp;
        }
    }
    if ( !IS_UNSIGNED_26( p->props.Jump.addr ) ){
        displayError( p, "Jump address is too large!");
        return false;
    }
    return true;
}


void Generator:: displayError(const ParseObj *p,const char *fmt, ... ){
    parseSuccess = false;
    enum { BUFFER_SIZE = 1024 };
    va_list args;
    va_start( args, fmt );
    char buffer[ BUFFER_SIZE ];
    size_t len = snprintf(buffer,BUFFER_SIZE,"At line %zu:\n"\
                        "Instruction : %s \nError: ", p->line, p->insString.c_str() );
    try {
        if ( len > BUFFER_SIZE ){
            throw "Allocated buffer is smaller than the error message !";
        }
    } catch (const char *msg){
        std::cerr << msg << std::endl;
    }
    len = vsnprintf(buffer+len,BUFFER_SIZE-(len+1),fmt,args);
    try {
        if ( len > BUFFER_SIZE-(len+1) ){
            throw "Allocated buffer is smaller than the error message !";
        }
    } catch (const char *msg){
        std::cerr << msg << std::endl;
    }
    va_end(args);
    std::cerr << buffer << std::endl << std::endl;
}

Code Generator::encodeItype( const ParseObj *p ){
    Code x = 0;
    /* 
     * Register values, opcodes and func fields are all guranteed to be at most 5-bits long.
     * So we skip the error checking for these values
     * shamt is given by the user so we have to check
     */
    x =  ( UINT32_CAST(p->ins.opcode) ) << RS_LEN;
    x  = ( x | UINT32_CAST(p->props.Itype.rs) ) << RT_LEN;
    x = ( x | UINT32_CAST(p->props.Itype.rt) ) << IMM_LEN;
    x  |= ( UINT32_CAST(p->props.Itype.addr) & SIXTEEN_BIT_MAX );
    return x;
}

Code Generator::encodeRtype( const ParseObj *p ){
    Code x = 0;
    /* 
     * Register values are all guranteed to be at most 5-bits long.
     * So we skip the error checking for these values
     */
    x =  ( UINT32_CAST(p->ins.opcode) ) << RS_LEN ;
    x  = ( x | UINT32_CAST(p->props.Rtype.rs) ) << RT_LEN;
    x = ( x | UINT32_CAST(p->props.Rtype.rt) ) << RD_LEN;
    x = ( x | UINT32_CAST(p->props.Rtype.rd) ) << SHAMT_LEN;
    x = ( x | ( UINT32_CAST(p->props.Rtype.shamt) & FIVE_BIT_MAX ) ) << FUNC_LEN;
    x |= p->ins.func;
    return x;
}

Code Generator::encodeBranch( const ParseObj *p ){
    Code x = 0;
    /* 
     * Register values are all guranteed to be at most 5-bits long.
     * So we skip the error checking for these values
     */
    x =  ( UINT32_CAST(p->ins.opcode) ) << RS_LEN ;
    x  = ( x | UINT32_CAST(p->props.Branch.rs) ) << RT_LEN;
    x = ( x | UINT32_CAST(p->props.Branch.rt) ) << IMM_LEN;
    x |= ( UINT32_CAST(p->props.Branch.offset) & SIXTEEN_BIT_MAX ) ;
    return x;
}

Code Generator::encodeJump( const ParseObj *p ){
    Code x = 0;
    /* 
     * Register values are all guranteed to be at most 5-bits long.
     * So we skip the error checking for these values
     */
    x =  ( UINT32_CAST(p->ins.opcode) ) << ADDRESS_LEN;

    /* 
     * Jump address are unsigned 26 bit numbers
     * You can't jump to a negative address
     */
    x |= ( UINT32_CAST(p->props.Jump.addr) & TWENTY_SIX_BIT_MAX ) ;
    return x;

}
Code Generator::encodeObj( const ParseObj *p){
    if ( p->ins.type() == Instruction::ITYPE ){
        return encodeItype(p);
    } else if ( p->ins.type() == Instruction::RTYPE ){
        return encodeRtype(p);
    } else if ( p->ins.type() == Instruction::BRTYPE){
        return encodeBranch(p);
    } else if ( p->ins.type() == Instruction :: JTYPE){
        return encodeJump(p);
    }
    std::cerr << "Invalid Object!" << std::endl;
    return 0;
}
void Generator :: encode () {
    for ( auto iter = objs.begin(); iter != objs.end() ; iter ++ ){
        Code code;
        ParseObj *p = *iter; 
        if ( p->ins.insClass == Instruction::RTYPE ){
            if ( p->ins.isShiftLogical() && !IS_UNSIGNED_5(p->props.Rtype.shamt) ){
                    displayError(p, "Value for shamt field exceeds 5-bits ( max %d ).", FIVE_BIT_MAX );
            }
            code = encodeRtype( p );
        } else if ( p->ins.insClass == Instruction :: ITYPE ){
            if ( !IS_SIGNED_16(p->props.Itype.addr) ){
                displayError(p, "Value for Immediate field exceeds 16-bits ( min %d, max %d ).", INT16_MIN, INT16_MAX );
            }
            code = encodeItype(p);
        } else if ( p->ins.insClass == Instruction :: BRTYPE){
            resolveBranch(p);
            code = encodeBranch(p);
        } else if ( p->ins.insClass == Instruction :: JTYPE ){
            resolveJump(p);
            code = encodeJump(p);
        }
        prog.push_back(code);
    }
}

bool Generator :: parseFile () {
    try {
        if ( !file ){ throw "Generator has no file initialized!";}
    } catch ( const char *msg) {
        std::cerr << msg << std::endl;
    }
    Parser p(file);
    ParseObj *obj = p.parse();
    while ( obj != nullptr ){
        objs.push_back( obj );
        obj = p.parse();
    }
    totalIns = p.getInsCount();
    return p.isSuccess();
}

void Generator :: test (){
    assert( IS_SIGNED_16( -8 ) );
    assert( IS_SIGNED_16( 123 ) );
    assert( !IS_SIGNED_16(-56789) );
    assert( !IS_SIGNED_16(56789) );
    string s ("Loop:sll $t1,$s3,2\n"\
             "      addr $t1,$t1,$s6\n"\
             "      lw $t0,0($t1)\n"\
             "      bne $t0,$s5,Exit\n"\
             "      addi $s3,$s3,1\n"\
             "      jmp Loop\n"\
             "Exit: beq $t0,$s5,Loop\n"\
             "      beq $t1,$t2,Exit\n"\
                    "jmp Exit2\n"\
             "Exit2: ");
    Generator g( s.c_str() );
    bool x = g.parseFile( );
    g.encode();
    for ( size_t i = 0; i < g.objs.size() ; i++ ){
        std::cout<<"Instruction Code: "<< std::endl << "0x"<< std::hex << g.prog[i] << std::endl;
        g.objs[i]->display();
    }
    (void)x;
}