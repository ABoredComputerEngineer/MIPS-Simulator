#include "common.hpp"
#include "lex.hpp"
#include "parse.hpp"
strToIndexMap labelMap; 
using std::string;
#define UNIT 0
ParseObj::~ParseObj (){
    if ( ins.insClass == Instruction :: BRTYPE ){
        if ( props.Branch.label )
            free( (char *)(props.Branch.label) );
    } else if ( ins.insClass == Instruction :: JTYPE){
        if ( props.Jump.label ){
            free( (char *)( props.Jump.label ) );
        }
    }
}
void ParseObj :: setJr ( Integer rs ){
    props.Jr.rs = rs;
}
void ParseObj::setItype ( Integer rs, Integer rt, Integer addr ){
    props.Itype.rs = rs;
    props.Itype.rt = rt;
    props.Itype.addr = addr;
}

void ParseObj::setBranch( Integer rs, Integer rt, const char *s ){
    props.Branch.rs = rs;
    props.Branch.rt = rt;
    props.Branch.label = s;
    props.Branch.offset = 0;
}

void ParseObj::setBranch( Integer rs, Integer rt, Integer off ){
    props.Branch.rs = rs;
    props.Branch.rt = rt;
    props.Branch.offset = off;
    props.Branch.label = nullptr;
}

void ParseObj::setJump( Integer address ){
    props.Jump.addr = address;
}

void ParseObj::setJump( const char *s ){
    props.Jump.label = s;
}
#define print(x) ( std::cout << x )
#define printl(x) ( std::cout << x << std::endl )
#define print2l( x, y ) ( std::cout << ( x ) << ( y ) << std::endl )
void ParseObj :: display () const {
    std::cout << std:: dec ;
    print2l( "Instruction : " , insString );
    print2l( "op = ", ins.opcode );
    if ( ins.insClass == Instruction::RTYPE  ){
        print2l("rs = ", props.Rtype.rs );
        print2l("rt = ", props.Rtype.rt );
        print2l("rd = ", props.Rtype.rd);
        print2l("shamt = ", props.Rtype.shamt);
        print2l("func = ", ins.func );
    } else if ( ins.insClass == Instruction :: ITYPE ){
        print2l("rs = ", props.Itype.rs );
        print2l("rt = ", props.Itype.rt );
        print2l("immediate = ", props.Itype.addr );
    } else if ( ins.insClass == Instruction :: BRTYPE ){
        print2l("rs = ",props.Branch.rs );
        print2l( "rt = ", props.Branch.rt );
        print2l( "Word offset = ", props.Branch.offset );
        if ( props.Branch.label ){
            print2l( "Label name = ", props.Branch.label );
        }
    } else if ( ins.insClass == Instruction :: JTYPE ) {
        print2l("Word address = ",  props.Jump.addr );
        if( props.Jump.label ){ print2l("Label name = ", props.Jump.label ); }
    } else if ( ins.insClass == Instruction:: PTYPE ){
        if ( ins.opcode == 0 ){
            switch( ins.func ){
                case 8: // jr
                    print2l("rs = ", props.Jr.rs);
                    print2l("func = ", ins.func );
                    break;
                default:
                    break;
            }
        }
    }
    printl("");
}

void ParseObj::dumpToBuff( AppendBuffer &buff ){
    buff.append( "Instruction: %s\n" , insString.c_str() );
    buff.append( "op = %d\n", ins.opcode );
    if ( ins.insClass == Instruction::RTYPE  ){
        buff.append("rs = %ld\n", props.Rtype.rs );
        buff.append("rt = %ld\n", props.Rtype.rt );
        buff.append("rd = %ld\n", props.Rtype.rd);
        buff.append("shamt = %ld\n", props.Rtype.shamt);
        buff.append("func = %ld\n", ins.func );
    } else if ( ins.insClass == Instruction :: ITYPE ){
        buff.append("rs = %ld\n", props.Itype.rs );
        buff.append("rt = %ld\n", props.Itype.rt );
        buff.append("immediate = %llx\n", props.Itype.addr );
    } else if ( ins.insClass == Instruction :: BRTYPE ){
        buff.append("rs = %ld\n",props.Branch.rs );
        buff.append( "rt = %ld\n", props.Branch.rt );
        buff.append( "Word offset = 0x%llx\n", props.Branch.offset );
        if ( props.Branch.label ){
            buff.append( "Label name = %s\n", props.Branch.label );
        }
    } else if ( ins.insClass == Instruction :: JTYPE ) {
        buff.append("Word address = 0x%llx\n",  props.Jump.addr );
        if( props.Jump.label ){
            buff.append("Label name = %s", props.Jump.label );
        }
    } else if ( ins.insClass == Instruction:: PTYPE ){
        if ( ins.opcode == 0 ){
            switch( ins.func ){
                case 8: // jr
                    buff.append("rs = %ld\n", props.Jr.rs);
                    buff.append("func = %ld\n", ins.func );
                    break;
                default:
                    break;
            }
        }
    }
    buff.append("\n");
}
#undef print
#undef printl
#undef print2l
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

ParseObj :: ParseObj ( const Instruction &i, string const &s, size_t x,size_t y ): ins(i),insString(s),line(x),insNumber(y) {
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


/*
 * PARSER CODE BEGINS HERE
 */

Parser:: Parser ( const char *p ): instructions( p ),lex(p),err(false),parseSuccess(true),insCount(0){
    currentStr.reserve(256);
    lex.next(buff);
}

void Parser::init(const char *p){
    instructions = p;
    lex.init(p);
    lex.next(buff);
    err = false;
}
void Parser :: displayError(const char *fmt, ... ){
    parseSuccess = false;
    enum { BUFFER_SIZE = 1024 };
    va_list args;
    va_start( args, fmt );
    char buffer[ BUFFER_SIZE ];
    size_t len = snprintf(buffer,BUFFER_SIZE,"At line %zu:\n"\
                        "Instruction : %s \nError: ", currentLine, currentStr.c_str() );
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
        auto regs = regMap.find( string (buff) );
        if ( regs != regMap.end() ){
            i = regs->second;
        } else {
            err = true;
            displayError("Invalid register %s.",buff);            
        }
        lex.next(buff);
    }
    return i;
}


int Parser::parseInt(){
    return parseExpr();
    #if 0
    int i = 0;
    if ( lex.isToken(Lexer::TOKEN_INT) ){
        i = lex.getInt(); 
        lex.next(buff);
    } else if ( !err ){
        err = true;
        displayError("Expected integer value as an argument. Got \'%s\' instead.",buff);
    }
    return i;
    #endif
}
ParseObj *Parser :: parseRtype ( ){
    Integer reg1,reg2,reg3;

    /* 
     * Only valid instructions are counted by the parser.
     * i.e The instruction count is only incremented after the parser ( parse() function )
     * confirms that there is no error.
     * So, we increase insCount by 1 in advance.
     * The parseObj p is discarded by the parser if there is an error 
     */
    ParseObj *p = new ParseObj( current, currentStr, currentLine,insCount+1 );
    reg1 = parseRegister();
    /* 
     * Sets error to true if we dont get the expected TOKEN.
     * expect() returns true if we match with the given token,
     * which sets the err to false
     */
    err = !lex.expect( Lexer:: TOKEN_COMMA, buff );
    reg2 = parseRegister();
    err = !lex.expect( Lexer :: TOKEN_COMMA ,buff );

    /* There are two types of shifting operations
     * The logical shift takes an integer value as an argument for its 'shamt' field
     * The variable shift, like other R-type instructions take a register for its 'rs' field, the shamt field is ignored
     */
    if ( current.isShiftLogical() ){
        reg3 = parseInt();
    } else {
        reg3 = parseRegister();
    }
    if ( err ){
        displayError("Invalid arguments to the instruction \'%s\'.",current.str.c_str());
    } else {
        parseInsEnd();
    }
    p->setRtype( reg1,reg2,reg3 );
    return p;
}

ParseObj *Parser :: parseItype ( ){
    Integer rs,rt,addr;
    ParseObj *p =  new ParseObj (current, currentStr, currentLine, insCount + 1);
    rt = parseRegister();
    lex.expect(Lexer::TOKEN_COMMA, buff);
    rs = parseRegister();
    lex.expect(Lexer::TOKEN_COMMA,buff); 
    addr = parseInt();
    parseInsEnd();
    p->setItype( rs,rt,addr );
    return p;
}

ParseObj *Parser::parseLS(){
    Integer rs,rt,off;
    ParseObj *p = new ParseObj ( current, currentStr , currentLine, insCount + 1);
    rt = parseRegister();
    lex.expect(Lexer::TOKEN_COMMA,buff);
    off = parseInt();
    lex.expect(Lexer::TOKEN_LPAREN,buff);
    rs = parseRegister();
    lex.expect(Lexer::TOKEN_RPAREN,buff);
    parseInsEnd();
    p->setItype(rs,rt,off);
    return p;
}

ParseObj *Parser::parseBranch(){
    Integer rs,rt,offset;
    ParseObj *p = new ParseObj(current, currentStr, currentLine, insCount + 1 );
    rs = parseRegister();
    lex.expect(Lexer::TOKEN_COMMA,buff);
    rt = parseRegister();
    lex.expect(Lexer::TOKEN_COMMA,buff);
    if ( lex.isToken(Lexer::TOKEN_NAME) ){
        const char *str = strdup( buff ); // TODO: replace strdup with something more useful
        p->setBranch(rs,rt,str);
    } else if ( lex.isToken( Lexer::TOKEN_INT ) ){
        offset = lex.getInt();
        p->setBranch( rs, rt, offset );
    } else if ( !err ) {
        err = true;
        displayError("Expected jump offset (integer) or Label name. Got \'%s\' instead.",buff);
    }
    lex.next(buff);
    parseInsEnd();
    return p;
}

ParseObj *Parser::parseJump(){
    ParseObj *p = new ParseObj(current,currentStr, currentLine, insCount + 1);
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
    parseInsEnd();
    return p;
}

void Parser::parseInsEnd(){
    if ( !err && !lex.matchInsEnd(buff) ){
        err = true;
        displayError("Too many arguments to instruction %s.", current.str.c_str() );
    }
}

ParseObj *Parser::parseJr(){
    ParseObj *p = new ParseObj( current, currentStr, currentLine, insCount + 1 );
    auto rs = parseRegister();
    parseInsEnd();
    p->setJr(rs);
    return p;
}

ParseObj *Parser::parseIns(  ){
    // Pseudo instructions are handled separately
    if ( current.insClass == Instruction::PTYPE ){
        if ( current.opcode == 0 ){
            switch ( current.func ){
                case 8:
                    return parseJr();
                    break;
                default:
                    break;
            }
        }
    } else if ( current.kind == Instruction::AL ){
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
        currentStr = lex.insString();
        currentLine = lex.getLineNum();
        lex.next(buff);
        ParseObj *p = parseIns( );
        while ( err && !lex.isToken( Lexer :: TOKEN_END ) ){
            delete p;
            lex.nextInstruction(buff);
            current = insMap[ string (buff) ];
            currentStr = lex.insString();
            currentLine = lex.getLineNum();
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
        delete p;
        return nullptr;
    } else if ( lex.isToken(Lexer::TOKEN_NAME) ) {
        // The token has to be a label, else we display error
        string str(buff);
        lex.next(buff);
        currentStr = lex.insString();
        currentLine = lex.getLineNum();
        if ( !lex.match(Lexer::TOKEN_COLON,buff) ){
            displayError("Unidentified instruction \'%s\'.",str.c_str() );
            lex.nextInstruction(buff);
        } else {
            labelMap[str] = insCount;
        }
        lex.match(Lexer::TOKEN_NEWLINE,buff);
        return parse();
    } else if (lex.isToken(Lexer::TOKEN_END)){
        return nullptr;
    }
    displayError("Expected instruction or label name but got \'%s\' instead.",buff);
    lex.nextInstruction(buff);
    return parse();
}


/*
 * =============================================
 * | Expression Parsing Code | 
 * =============================================
 * We only support following classes of operations:
 * addition, subtractions, multiply,
 * division, bitwise operations
 * The precedence order is similar to C.
 * Only difference is that bitwise - operations have
 * same precedence as that of muliply and division
 * So, 1 >> 2 + 3 == ( 1 >> 2 ) + 3
 *     1 + 2 & 3 == 1 + ( 2 & 3 )
 * and so on
 * The expression grammar is :
 * Expr =  add_expr
 * add_expr = mul_expr ( mul_expr add_op mul_expr )*
 * mul_expr = unary_expr ( unary_expr mul_op unary_expr )*
 * unary_expr = base_expr | unary_op unary_expr
 * base_expr = NUMBER | ( expr )
 * add_op = ( +, - )
 * mul_op = ( / , * , << , >> , | , & , ^ )
 * unary_op = ( -, !, ~)
 * Here, * denotes zero or more repititions
 */

int performBinaryOp( Lexer::TokenKind op, int a, int b = 0){
    /*
     * The default argument b allows us to use this function 
     * to evaluate unary operations without specifying two 
     * arguments. This function does NOT handle unary minus and 
     * so must be handled explicitly
     */
    switch ( op ){
        case Lexer::TOKEN_ADD:
            return a+b; break;
        case Lexer::TOKEN_SUB:
            return a-b; break;
        case Lexer::TOKEN_MUL:
            return a*b; break;
        case Lexer::TOKEN_DIV:
            return a/b; break;
        case Lexer::TOKEN_LSHIFT:
            return a<<b; break;
        case Lexer::TOKEN_RSHIFT:
            return a>>b; break;
        case Lexer::TOKEN_BAND:
            return a&b; break;
        case Lexer::TOKEN_BOR:
            return a|b; break;
        case Lexer::TOKEN_BXOR:
            return a^b; break;
        default:
            std::cerr << "Invalid Operator " << op << std::endl;
            return 0;
            break;

    }
}

int performUnaryOp( Lexer::TokenKind k , int a ){
    switch  ( k ){
        case Lexer::TOKEN_SUB:
            return -a; break;
        case Lexer::TOKEN_COMPLEMENT:
            return ~a; break;
        case Lexer::TOKEN_NOT:
            return !a; break;
        default:
            std::cerr << "Invlid operator " << k << std::endl;
            break;
    }
    return 0;
}

int Parser::parseBaseExpr(){
    if ( lex.isToken(Lexer::TOKEN_INT) ){
        int val = lex.int_val;
        lex.next(buff);
        return val;
    } else if ( lex.match(Lexer::TOKEN_LPAREN,buff) ){
        int val = parseExpr();
        lex.expect(Lexer::TOKEN_RPAREN,buff);
        return val;
    } else {
        err = true;
        displayError("Invalid token %s in an expression",buff);
        return 0;
    } 
}

int Parser::parseUnaryExpr(){
    if ( lex.isUnaryOp() && !err ){
        Lexer::TokenKind op = lex.kind;
        lex.next(buff);
        int val = parseUnaryExpr();
        return performUnaryOp(op,val);
    } else if ( !err ) {
        return parseBaseExpr();
    }
    return 0;
    
}
int Parser::parseMulExpr(){
    int val = parseUnaryExpr();
    while ( lex.isMulOp() && !err ){
        Lexer::TokenKind op = lex.kind;
        lex.next(buff);
        int rval = parseMulExpr();
        val = performBinaryOp(op,val,rval);
    }
    return val;
}
int Parser::parseAddExpr(){
    int val = parseMulExpr();
    while ( lex.isAddOp() && !err ){
        Lexer::TokenKind op = lex.kind;
        lex.next(buff);
        int rval = parseMulExpr();
        val = performBinaryOp(op,val,rval);
    }
    return val;
}
int Parser::parseExpr(){
    return parseAddExpr();
}

#define TEST_EQ(x,y,msg)\
    do {\
        try {\
            if ( ( x ) != ( y ) ) {flag = false;throw msg;}\
        } catch ( const char *m ){\
            std::cerr << msg << std::endl;\
        }\
    } while ( 0 )


void Parser::exprTest(){
    #define TEST_EXPR(x) \
        do {\
            Parser p(#x);\
            int result = p.parseExpr();\
            if ( (x) != result ){\
                std::cerr << "Incorrect expression evaluation for expression " << std::endl;\
            }\
            std::cerr<< #x << " == " << x << std::endl;\
        } while (0)
    
    TEST_EXPR(2+3);
    TEST_EXPR( 2 + 3 * 2 );
    TEST_EXPR( (2+3) * 2 );
    TEST_EXPR( -2 + 3 );
    TEST_EXPR( -2 - 3 - 4 );
    TEST_EXPR( !2 * 32 );
    TEST_EXPR( 2 + -3 * ~2 );
    #undef TEST_EXPR
}


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
    exprTest();
    string str("addr $s0,$s1\n"\
                "      Label:\n"\
               "addi $s0,$s1,ab\n"\
               "lw $s0,0(32)\n"\
               "     addr $s0,$s1\n"\
               "   sw $s0,0($s1)\n"\
               "garbage $s0,$s1,$s3\n"\
               "Exit:"\
               "addi $t0,$t1,0x3?b4\n"\
               "beq $s0,$s1,Exit\n"\
               "beq $s0,$s1,0x10\n"\
               "addi $s0,$s1,3+4*5\n"\
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
    obj->display();
    do {
        delete obj;
        obj = q.parse();
        if (obj){
            obj->display();
        }
    } while ( obj != nullptr );
    return;
#endif
}
#undef NEW_INS
#undef TEST_EQ

#if UNIT
int main(){
    Parser::test();
}
#endif