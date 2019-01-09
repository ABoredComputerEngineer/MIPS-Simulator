using std::string;
namespace Assembler {
       
} // name Assembler  
typedef int Integer;
enum OpCodes{
    ADD = 0,
    SUB = 0,
    ADDI = 8,
    LW = 35, 
};

struct Position {
    size_t row, col;
    Position ( size_t a, size_t b ): row(a), col(b){}
};

struct Instruction {
    enum InstructionKind {
        AL, // Arithmetic and logical
        BRANCH,
        JUMP,
        LS // Load and store type
    };
    enum InstructionClass {
        RTYPE,
        ITYPE,
        JTYPE,
        BRTYPE,
    };
    string str;  
    Integer opcode;
    Integer func;
    InstructionKind kind;
    InstructionClass insClass;
    public:
    Instruction () {}
    Instruction ( string s, Integer op,Integer f, InstructionKind k, InstructionClass c):\
    str(s), opcode(op),func(f), kind(k), insClass(c) {}
    inline bool isShift();
    inline bool isBranch();
    inline bool isJump();
};

inline bool Instruction :: isShift ( ){
    return ( func == 0 || func == 2 || func == 4 || func == 6 );
}
inline bool Instruction::isBranch(){
    return ( opcode == 4 || opcode == 5 );
}

inline bool Instruction::isJump(){
    return opcode == 2;
}
typedef std::map< string, bool> strToBoolMap;
typedef std::map< string, OpCodes> strToOpCodeMap;
typedef std::map< string , Instruction > strToInsMap;
typedef std::map< string ,Integer > strToIntMap;
typedef std::map< char , int > charToIntMap;
strToBoolMap keywordMap;
strToOpCodeMap opCodeMap;
strToInsMap insMap;
strToIntMap regMap;
charToIntMap digitMap;

struct Lexer{
    enum TokenKind {
        NONE,
        TOKEN_REGISTER,
        TOKEN_INSTRUCTION,
        TOKEN_INT,
        TOKEN_NAME,
        TOKEN_LPAREN,
        TOKEN_RPAREN,
        TOKEN_COMMA,
        TOKEN_COLON,
        TOKEN_END,
        TOKEN_NEWLINE
    };
    TokenKind kind;
    const char *stream;
    const char * line_start; // Points to the start of the line
    const char *start; // Points to the start of the current token
    const char *previous; // Points to the previous instruction
    size_t prev_line;
    Integer int_val;
    string str; // string of the current object
    size_t line; // the line number 

    Lexer ( const char *x ): stream( x ),line_start(x),line(1){str.reserve(256);};
    void init(const char *str);
    void next( char *buff );
    void scanInt(char *buff);
    inline bool isIns();
    bool match(TokenKind, char *);
    bool expect(TokenKind, char *);
    inline int getInt(){ return int_val;}
    inline bool isToken(TokenKind);
    Position currentPos();
    void nextInstruction(char *buff);
    static void test();
    string instructionString();
    void previousIns(char *);
    string insString();
    inline size_t getLineNum( ){ return line; }
};

string Lexer :: insString(){
    string str;
    str.reserve( 256 );
    for ( const char *s = line_start; *s && *s != '\n' ; s++ ){
        str += *s;
    }
    return str;
}

string Lexer :: instructionString (){
    //need to handle it more efficiently
    const char *s = line_start;
    string str;
    while ( *s != '\n'){
        str += *s++;
    }
    return str;
}

Position Lexer::currentPos(){
    return Position ( line, (size_t)( stream - start ) );
}

inline bool Lexer::isIns(){ return kind == TOKEN_INSTRUCTION; }


bool Lexer::isToken( TokenKind k ){
    return kind == k;
}
bool Lexer::match( TokenKind k , char *buff){
    if ( kind == k ){
        next(buff);
        return true;
    } else {
        return false;
    }
}

bool Lexer::expect(TokenKind k , char *buff ){
    return match(k,buff);
}
void Lexer :: init ( const char *str){
    line = 1;
    stream = str;
}



void Lexer::scanInt( char *buff ){
    int base  = 10, val = 0;
    if ( *stream == '0' ){
        *buff++ = *stream++;
        if ( *stream == 'x' || *stream == 'b' ){
            base = ( *stream == 'x' )?16:2;
            *buff++ = *stream++;
        } else {
            base = 8;
        }
    }
    while ( digitMap.find(*stream) != digitMap.end() ){//While there is a valid digit
        int digit = digitMap[*stream];
        if ( digit >= base ){
            std::cerr << "Invalid digit for the given base!\n" << std::endl;
            break;
        }
        val = val * base + digitMap[*stream];
        *buff++ = *stream++;
    }
    int_val = val; 
    *buff = 0;
}

void Lexer::nextInstruction(char *buff){
    while ( !isToken(TOKEN_INSTRUCTION) && !isToken(TOKEN_END) ){
        next(buff);
    }
}

void Lexer :: next (char *buff){
    switch ( *stream ){
        case '\n': case '\t': case '\r': case ' ':
            while ( isspace( *stream ) ){
                if ( *stream == '\n' ){
                    kind = TOKEN_NEWLINE;
                    prev_line = line;
                    line++; 
                    previous = line_start;
                    line_start = stream + 1;
                }
                stream++;
            }
            if ( kind == TOKEN_NEWLINE ){
                *buff = '\n';
                *buff = 0;
            } else {
                next(buff);
            }
            break;
        case 'A': case 'B': case 'C': case 'D': case 'E': 
        case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': 
        case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': 
        case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': 
        case 'X': case 'Y': case 'Z': case 'a': case 'b': case 'c': 
        case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': 
        case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': 
        case 'p': case 'q': case 'r': case 's': case 't': case 'u': 
        case 'v': case 'w': case 'x': case 'y': case 'z': {
            start = stream;
            kind = TOKEN_NAME;
            char *str = buff;
            while ( isalnum(*stream) ){
                *buff = *stream;
                stream++;
                buff++;
            }
            *buff = 0;
            string s ( str );
            if ( ::keywordMap[s] ){
                kind = TOKEN_INSTRUCTION;
            }
            break;
        }
        case '(': case ')':
            kind = ( *stream == '(' )? TOKEN_LPAREN:TOKEN_RPAREN;
            *buff = *stream++;
            *buff = 0;
            break;
        case '$':
            kind = TOKEN_REGISTER;
            *buff++ = *stream++;
            while ( isalnum( *stream ) ){
                *buff++ = *stream++;
            }
            *buff = 0;
            break;
        case '0': case '1': case '2': case '3': case '4': 
        case '5': case '6': case '7': case '8': case '9':{
            start = stream;
            kind =   TOKEN_INT;
            scanInt(buff);
            break;
        }
        case ',': case ':':
            kind = ( *stream == ',' )?TOKEN_COMMA:TOKEN_COLON;
            *buff++ = *stream++;
            *buff = 0;
            break;
        case 0:
            kind = TOKEN_END;
            break;
        default:
            std::cerr << "Unidentified token !" << std::endl;
            break;
    }
}

void Lexer :: previousIns(char *buff){
    stream = previous;
    line_start = previous;
    line = prev_line;
    next(buff);
}
#define NEW_INS(NAME,OP,FUNC,KIND,CLASS) ( Instruction ( string (#NAME), OP, FUNC, Instruction::KIND, Instruction::CLASS) ) 
#define ADD_INS(NAME,OP,FUNC,KIND,CLASS) insMap[#NAME] = NEW_INS(NAME,OP,FUNC,KIND,CLASS) 
#define ADD_INSR(NAME,FUNC,KIND) insMap[#NAME] = NEW_INS(NAME,0,FUNC,KIND,RTYPE); keywordMap[#NAME] = true 
#define ADD_INSI(NAME,OP,KIND) insMap[#NAME] = NEW_INS(NAME,OP,0,KIND,ITYPE); keywordMap[#NAME] = true 
#define ADD_INS_JMP(NAME,OP,KIND) insMap[#NAME] = NEW_INS(NAME,OP,0,KIND,JTYPE); keywordMap[#NAME] = true 
#define ADD_INS_BR(NAME,OP,KIND) insMap[#NAME] = NEW_INS(NAME,OP,0,KIND,BRTYPE); keywordMap[#NAME] = true 
void initIns( ){
    ADD_INSR(sll,0,AL);
    ADD_INSR(addr,32,AL);
    ADD_INSR(addu,33,AL);
    ADD_INSR(subr,34,AL);
    ADD_INSR(subu,35,AL);
    ADD_INSR(and,36,AL);
    ADD_INSR(or,37,AL);
    ADD_INSR(xor,38,AL);
    ADD_INSR(xnor,39,AL);
    ADD_INSR(slt,42,AL);
    ADD_INSR(sltu,43,AL);

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
void init( ){
    opCodeMap["addr"] = ADD;
    opCodeMap["addi"] = ADDI;
    regMap["$s0"] = 16;
    regMap["$s1"] = 17;
    regMap["$s2"] = 18;
    regMap["$s3"] = 19;
    initIns();
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


#define TEST_EQ(x,y,msg)\
    do {\
        try {\
            if ( ( x ) != ( y ) ) throw msg;\
        } catch ( const char *m ){\
            std::cerr << msg << std::endl;\
        }\
    } while ( 0 )

void Lexer :: test (){
    const char str [] =  "addi $r0,$r1,123";
    char buffer[ 256 ];
    Lexer n( str );
    n.next(buffer);
    TEST_EQ(n.kind,TOKEN_INSTRUCTION,"Failed to find the type of instruction");
    n.next(buffer);
    TEST_EQ(n.kind,TOKEN_REGISTER,"Failed to lex register!");
    n.next(buffer);
    TEST_EQ( n.kind , TOKEN_COMMA, "Failed to lex COMMA (',')" );
    n.next(buffer);
    TEST_EQ(n.kind,TOKEN_REGISTER,"Failed to lex register!");
    n.next(buffer);
    TEST_EQ( n.kind , TOKEN_COMMA, "Failed to lex COMMA (',')" );
    n.next(buffer);
    TEST_EQ(n.kind,TOKEN_INT,"Failed to lex integer!");
    TEST_EQ(n.int_val,123,"Failed to get integer value!");
    string s("1234 0xa2b4 0b1111 012 0");
    const char *newStr = s.c_str();
    n.init( newStr );
    n.next(buffer);
    TEST_EQ(n.int_val,1234,"Failed to scan decimal integer");
    n.next(buffer);
    TEST_EQ(n.int_val,0xa2b4,"Failed to scan hexadecimal integer");
    n.next(buffer);
    TEST_EQ(n.int_val,15,"Failed to scan binary integer");
    n.next(buffer);
    TEST_EQ(n.int_val,10,"Failed to scan octal integer");
    n.next(buffer);
    TEST_EQ(n.int_val,0,"Failed to scan zero value!");
}
#undef TEST_EQ