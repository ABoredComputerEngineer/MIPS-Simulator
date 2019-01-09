#include <cstring>
typedef std::map<string,size_t> strToIndexMap;
strToIndexMap labelMap; 
struct ParseObj {
    Instruction ins;
    union {
        struct {
            Integer rs,rt,rd,shamt;
        } Rtype;
        struct {
            Integer rs,rt,addr;
        } Itype;
        struct {
            Integer rs,rt,offset;
            const char *label;
        } Branch;
        struct {
            Integer addr;
            const char *label;
        } Jump;
    } props;
    ParseObj(){}
    ParseObj ( const Instruction & );
    ParseObj ( Instruction i, int a , int b = 0, int c = 0, int d = 0 );
    bool validateObj( ParseObj * );
    bool validateObj( const ParseObj &);
    void setRtype(Integer, Integer, Integer);
    void setItype ( Integer rs, Integer rt, Integer addr ){
        props.Itype.rs = rs;
        props.Itype.rt = rt;
        props.Itype.addr = addr;
    }
    void setBranch( Integer rs, Integer rt, const char *s ){
        props.Branch.rs = rs;
        props.Branch.rt = rt;
        props.Branch.label = s;
        props.Branch.offset = 0;
    }
    void setBranch( Integer rs, Integer rt, Integer off ){
        props.Branch.rs = rs;
        props.Branch.rt = rt;
        props.Branch.offset = off;
        props.Branch.label = nullptr;
    }
    void setJump( Integer address ){
        props.Jump.addr = address;
    }
    void setJump( const char *s ){
        props.Jump.label = s;
    }

};

void ParseObj::setRtype( Integer reg1, Integer reg2, Integer reg3 ){
    /* 
     * For shift instructions ( sll, srl , sllv, srlv )
     * The input registers are interpreted as 
     * rd = reg1, rt = reg2, shamt = reg3, rs = 0
     * For other instructions they are interpreted as:
     * rd = reg1, rs = reg2, rt = reg3, shamt = 0
     */
    if ( ins.isShift() ){
        props.Rtype.rd = reg1; 
        props.Rtype.rt = reg2; 
        props.Rtype.shamt = reg3;
        props.Rtype.rs = 0;
    } else {
        props.Rtype.rd = reg1; 
        props.Rtype.rs = reg2; 
        props.Rtype.rt = reg3; 
        props.Rtype.shamt = 0;
    }
}

ParseObj :: ParseObj ( const Instruction &i ): ins(i) {
    if ( ins.insClass == Instruction::ITYPE ){
        setItype(0,0,0);
    } else if ( ins.insClass == Instruction :: RTYPE ){
        setRtype( 0, 0 , 0);
    } else if ( ins.insClass == Instruction :: BRTYPE ){
        props.Branch.offset = 0; props.Branch.label = nullptr;
    } else if ( ins.insClass == Instruction:: JTYPE ){
        props.Jump.addr = 0; props.Jump.label = nullptr;
    }
}
class Parser {
    const char *instructions;
    Lexer lex;
    char buff[256];
    Instruction current;
    bool err;
    size_t insCount;
    Parser ( const char *p ): instructions( p ),lex(p),err(false),insCount(0){
        lex.next(buff);
    }
    ParseObj *parse();
    ParseObj *parseIns( );
    ParseObj *parseRtype();
    ParseObj *parseItype();
    ParseObj *parseLS();
    ParseObj *parseBranch();
    ParseObj *parseJump();
    int parseRegister();
    int parseInt();
    void init(const char *p){
        instructions = p;
        lex.init(p);
        lex.next(buff);
        err = false;
    }
    void genParseError();
    void displayError(const char *fmt,...);
    public:
    static void test();
};

ParseObj::ParseObj ( Instruction ins, int a, int b, int c , int d){
    if ( ins.insClass == Instruction :: RTYPE ){
        props.Rtype.rs = a;
        props.Rtype.rt = b;
        props.Rtype.rd = c;
        props.Rtype.shamt = d;
    } else if ( ins.insClass == Instruction :: ITYPE ){
        props.Itype.rs = a;
        props.Itype.rt = b;
        props.Itype.addr = c;
    } else if ( ins.insClass == Instruction:: JTYPE ){
        props.Jump.addr = a;
    } else {
        props.Branch.rs = a;
        props.Branch.rt = b;
        props.Branch.offset = c;
    }
}

void Parser :: displayError(const char *fmt, ... ){
    enum { BUFFER_SIZE = 1024 };
    va_list args;
    va_start( args, fmt );
    char buffer[ BUFFER_SIZE ];
    Position p = lex.currentPos(); // TODO: Proper way of handling columns
    string ins = lex.instructionString();
    size_t len = snprintf(buffer,BUFFER_SIZE,"At line %zu:\n"\
                        "Instruction : %s \nError: ", p.row, ins.c_str() );
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

int Parser :: parseRegister(){
    int i = -1;
    if ( !err && !lex.isToken(Lexer::TOKEN_REGISTER) ){
        err = true;
        displayError("Expected register but instead got \'%s\'.",buff);
    } else if ( !err ){
        i = regMap[ string ( buff ) ]; 
        lex.next(buff);
    }
    return i;
}


int Parser::parseInt(){
    int i = 0;
    if ( lex.isToken(Lexer::TOKEN_INT) ){
        i = lex.getInt(); 
        lex.next(buff);
    } else if ( !err ){
        err = true;
        displayError("Expected integer value as an argument. Got \'%s\' instead.",buff);
    }
    return i;
}
ParseObj *Parser :: parseRtype ( ){
    Integer reg1,reg2,reg3;
    reg1 = parseRegister();
    /* 
     * Sets error to true if we dont get the expected TOKEN.
     * expect() returns true if we match with the given token,
     * which sets the err to false
     */
    err = !lex.expect( Lexer:: TOKEN_COMMA, buff );
    reg2 = parseRegister();
    err = !lex.expect( Lexer :: TOKEN_COMMA ,buff );
    reg3 = parseRegister();
    if ( err ){
        lex.previousIns(buff);
        displayError("Invalid arguments to the instruction \'%s\'.",current.str.c_str());
        lex.next(buff);
    }else if ( !err && !lex.isToken(Lexer::TOKEN_END) && !lex.match(Lexer::TOKEN_NEWLINE, buff) ){
        err = true;
        displayError("Too many arguments to instruction %s.", current.str.c_str() );
    }
    ParseObj *p = new ParseObj( current );
    p->setRtype( reg1,reg2,reg3 );
    return p;
}

ParseObj *Parser :: parseItype ( ){
    Integer rs,rt,addr;
    rt = parseRegister();
    lex.expect(Lexer::TOKEN_COMMA, buff);
    rs = parseRegister();
    lex.expect(Lexer::TOKEN_COMMA,buff); 
    addr = parseInt();
    ParseObj *p = new ParseObj( current );
    if ( !err && !lex.isToken(Lexer::TOKEN_END) && !lex.match(Lexer::TOKEN_NEWLINE, buff) ){
        err = true;
        displayError("Too many arguments to instruction %s.", current.str.c_str() );
    }
    p->setItype( rs,rt,addr );
    return p;
}

ParseObj *Parser::parseLS(){
    Integer rs,rt,off;
    rt = parseRegister();
    lex.expect(Lexer::TOKEN_COMMA,buff);
    off = parseInt();
    lex.expect(Lexer::TOKEN_LPAREN,buff);
    rs = parseRegister();
    lex.expect(Lexer::TOKEN_RPAREN,buff);
    ParseObj *p = new ParseObj( current );
    if ( !err && !lex.isToken(Lexer::TOKEN_END) && !lex.match(Lexer::TOKEN_NEWLINE,buff) ){
        err = true;
        displayError("Too many arguments to instruction %s.", current.str.c_str() );
    }
    p->setItype(rs,rt,off);
    return p;
}

ParseObj *Parser::parseBranch(){
    Integer rs,rt,offset;
    ParseObj *p = new ParseObj(current);
    rs = parseRegister();
    lex.expect(Lexer::TOKEN_COMMA,buff);
    rt = parseRegister();
    lex.expect(Lexer::TOKEN_COMMA,buff);
    if ( lex.isToken(Lexer::TOKEN_NAME) ){
        const char *str = strdup( buff );
        p->setBranch(rs,rt,str);
    } else if ( lex.isToken( Lexer::TOKEN_INT ) ){
        offset = lex.getInt();
        p->setBranch( rs, rt, offset );
    } else {
        err = true;
        displayError("Expected jump offset ( integer ) or Label name. Got \'%s\' instead.",buff);
    }
    lex.next(buff);
    if ( !err && !lex.isToken(Lexer::TOKEN_END) && !lex.match(Lexer::TOKEN_NEWLINE,buff) ){
        err = true;
        displayError("Too many arguments to instruction %s.", current.str.c_str() );
    }
    return p;
}

ParseObj *Parser::parseJump(){
    ParseObj *p = new ParseObj(current);
    if ( lex.isToken(Lexer::TOKEN_NAME) ){
        const char *str = strdup(buff);
        p->setJump(str);
    } else if ( lex.isToken(Lexer::TOKEN_INT) ){
        Integer address = lex.getInt();
        p->setJump(address);
    } else {
        err = true;
        displayError("Expected jump offset ( integer ) or Label name. Got \'%s\' instead.",buff);
    }
    lex.next(buff);
    if ( !err && !lex.isToken(Lexer::TOKEN_END) && !lex.match(Lexer::TOKEN_NEWLINE, buff) ){
        err = true;
        displayError("Too many arguments to instruction %s.", current.str.c_str() );
    }
    return p;
}

ParseObj *Parser::parseIns(  ){
    if ( current.kind == Instruction::AL ){
        if ( current.insClass == Instruction:: ITYPE ){
            // Parse itype version of instruction
            return parseItype();
        } else {
           // Parse rtype version of the instruction 
           return parseRtype( );
        }
    } else if ( current.kind == Instruction :: LS ){
        // for load/save type instructions
        return parseLS();
    } else if ( current.kind == Instruction:: BRANCH ){
        return parseBranch();
    } else if ( current.kind == Instruction :: JUMP ){
        return parseJump();
    }
    return nullptr;
}

ParseObj* Parser::parse( ){
    if ( lex.isToken( Lexer :: TOKEN_INSTRUCTION ) ){
        current = insMap[ string ( buff ) ];
        lex.next(buff);
        ParseObj *p = parseIns( );
        while ( err && !lex.isToken( Lexer :: TOKEN_END ) ){
            delete p;
            lex.nextInstruction(buff);
            current = insMap[ string (buff) ];
            lex.next(buff);
            err = false;
            p = parseIns();
        }
        if ( !err ){
            // We were able to find a valid instruction
            insCount++;
            return p;
        }
        // We reached end of string before finding a valid instruction
        return nullptr;
    } else if ( lex.isToken(Lexer::TOKEN_NAME) ) {
        // The token has to be a label, else we display error
        string str(buff);
        lex.next(buff);
        if ( !lex.match(Lexer::TOKEN_COLON,buff) ){
            displayError("Unidentified instruction \'%s\'.",str.c_str() );
            lex.nextInstruction(buff);
        } else {
            labelMap[str] = insCount+1;
        }
        return parse();
    } else if (lex.isToken(Lexer::TOKEN_END)){
        return nullptr;
    }
    displayError("Expected instruction or label name but got \'%s\' instead.",buff);
    lex.nextInstruction(buff);
    return parse();
}



#define TEST_EQ(x,y,msg)\
    do {\
        try {\
            if ( ( x ) != ( y ) ) {flag = false;throw msg;}\
        } catch ( const char *m ){\
            std::cerr << msg << std::endl;\
        }\
    } while ( 0 )


bool ParseObj :: validateObj( ParseObj *p ){
    bool flag = true;
    if ( ins.insClass == Instruction::RTYPE ){
        TEST_EQ(props.Rtype.rs,p->props.Rtype.rs,"Unable to correctly parse the field rs.");
        TEST_EQ(props.Rtype.rt,p->props.Rtype.rt,"Unable to correctly parse the field rt.");
        TEST_EQ(props.Rtype.rd,p->props.Rtype.rd,"Unable to correctly parse the field rd.");
        TEST_EQ(props.Rtype.shamt,p->props.Rtype.shamt,"Unable to correctly parse the field shamt.");
    }  else if ( ins.insClass== Instruction:: ITYPE ){
        TEST_EQ(props.Itype.rs,p->props.Itype.rs,"Unable to correctly parse the field rs.");
        TEST_EQ(props.Itype.rt,p->props.Itype.rt,"Unable to correctly parse the field rt.");
        TEST_EQ(props.Itype.addr,p->props.Itype.addr,"Unable to correctly parse the field addr.");
    } else if ( ins.insClass==Instruction:: JTYPE ){
        TEST_EQ(props.Jump.addr,p->props.Jump.addr,"Unable to correctly parse/calculate the field addr.");
    } else if( ins.insClass == Instruction::BRTYPE ){
        TEST_EQ(props.Branch.rs,p->props.Branch.rs,"Unable to correctly parse the field rs.");
        TEST_EQ(props.Branch.rt,p->props.Branch.rt,"Unable to correctly parse the field rt.");
        TEST_EQ(props.Branch.offset,p->props.Branch.offset,"Unable to correctly calculate the offset.");
    }
    return flag;
}
bool ParseObj :: validateObj( const ParseObj &p ){
    bool flag = true;
    if ( ins.insClass== Instruction::RTYPE ){
        TEST_EQ(props.Rtype.rs,p.props.Rtype.rs,"Unable to correctly parse the field rs.IN RTYPE");
        TEST_EQ(props.Rtype.rt,p.props.Rtype.rt,"Unable to correctly parse the field rt.IN RTYPE");
        TEST_EQ(props.Rtype.rd,p.props.Rtype.rd,"Unable to correctly parse the field rd.IN RTYPE");
        TEST_EQ(props.Rtype.shamt,p.props.Rtype.shamt,"Unable to correctly parse the field shamt.");
    }  else if ( ins.insClass==Instruction:: ITYPE ){
        TEST_EQ(props.Itype.rs,p.props.Itype.rs,"Unable to correctly parse the field rs in ITYPE.");
        TEST_EQ(props.Itype.rt,p.props.Itype.rt,"Unable to correctly parse the field rt in ITYPE.");
        TEST_EQ(props.Itype.addr,p.props.Itype.addr,"Unable to correctly parse the field addr in ITYPE.");
    } else if ( ins.insClass==Instruction:: JTYPE ){
        TEST_EQ(props.Jump.addr,p.props.Jump.addr,"Unable to correctly parse the field address in Jump.");
        if ( props.Jump.label ){
            TEST_EQ( string (props.Jump.label), string( p.props.Jump.label), "Unable to correctly parse label name in Jump");
        }
    } else if ( ins.insClass == Instruction :: BRTYPE ){
        TEST_EQ(props.Branch.rs,p.props.Branch.rs,"Unable to parse rs of Branches correctly");
        TEST_EQ(props.Branch.rt,p.props.Branch.rt,"Unable to parse rt of Branches correctly");
        TEST_EQ(props.Branch.offset,p.props.Branch.offset,"Unable to parse offset of Branches correctly");
        if ( props.Branch.label ){
            TEST_EQ( string (props.Branch.label), string (p.props.Branch.label), "Unable to correctly parse the label name in Branches.");
        }

    }
    return flag;
    
}
#define NEW_INS(NAME,OP,FUNC,KIND,CLASS) ( Instruction ( string (#NAME), OP, FUNC, Instruction::KIND, Instruction::CLASS) ) 
void Parser::test(){
    string str("addr $s0,$s1\n"\
                "Label:\n"\
               "addi $s0,$s1,ab\n"\
               "lw $s0,0(32)\n"\
               "addr $s0,$s1\n"\
               "sw $s0,0($s1)\n"\
               "garbage $s0,$s1,$s3\n"\
               "Exit:"\
               "beq $s0,$s1,Exit\n"\
               "beq $s0,$s1,0x10\n"\
               "jmp 0x10\n"\
               "jmp Exit");
#if 0
// Checks the validity of the parsed strings with the correct ParseObj struct for each instruction
    Parser p ( str.c_str() );
    ParseObj *obj = p.parse();
    obj->validateObj(ParseObj ( insMap["addr"],17,18,16,0 ));
    delete obj;
    obj = p.parse();
    obj->validateObj( ParseObj ( insMap["sw"],17,16,0) );
    delete obj;
    obj = p.parse();
    ParseObj beq = ParseObj ( insMap["beq"] );
    char exit[] = "Exit";
    beq.setBranch(16,17,exit);
    obj->validateObj( beq );
    delete obj;
    obj = p.parse();
    beq.setBranch( 16,17,0x10);
    obj->validateObj( beq );
    delete obj;
    obj = p.parse();
    ParseObj jmp = ParseObj ( insMap["jmp"] );
    jmp.setJump( 0x10 );
    obj->validateObj( jmp );
    delete obj;
    obj = p.parse();
    jmp.setJump(exit);
    obj->validateObj(jmp);
#endif 
#if 1
// Parses everything in the test string displaying appropiate errors
    Parser q(str.c_str());
    ParseObj *obj = q.parse();
    do {
        delete obj;
        obj = q.parse();
    } while ( obj != nullptr );
    return;
#endif
}
#undef NEW_INS
#undef TEST_EQ