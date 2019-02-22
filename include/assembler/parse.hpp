#ifndef PARSE_HPP

#define PARSE_HPP
#include "common.hpp"
#include "lex.hpp"
#include <cstring>

class InstructionException{
    enum Type { 
        INVALID_ARGUMENTS,
    };
    Type type;
    InstructionException ( Type type );
};

class RegisterException{
    public:
    enum Type {
        INVALID, // register name is invalid e.g. $x0
        OVERFLOW, // more registers than required
        EXPECTED, // expected register but got something else
    };
    Type type;
    RegisterException ( Type  type );
};

class InstructionEndException { };

class ExpressionException {
    public:
    enum Type {
        INVALID_OP,
        INVALID_TOKEN,
    };
    Type type;
    Lexer::TokenKind kind;
    ExpressionException (Type,Lexer::TokenKind k);
};

class LabelError{
};


struct ParseObj {
    Instruction ins;
    std::string insString;
    size_t line;
    size_t insNumber;
    union {
        struct {
            Integer rs,rt,rd,shamt;
        } Rtype;
        struct {
            Integer rs,rt,addr;
        } Itype;
        struct {
            Integer rs,rt;
            mutable Integer offset; // The value is either given by the user or is set during code generation
            char label[32];
        } Branch;
        struct {
            mutable Integer addr;
            char label[32];
        } Jump;
        struct {
            Integer rs; // for 'jr' instructions. this might be merged with other structure in future versions
        } Jr;
    } props;
    ParseObj(){}
    ParseObj ( const Instruction & , std::string const &, size_t x,size_t y);
    ParseObj ( Instruction i, int a , int b = 0, int c = 0, int d = 0 );
    bool validateObj( ParseObj * );
    bool validateObj( const ParseObj &);
    void setRtype(Integer, Integer, Integer);
    void display() const ;
    ~ ParseObj ();
    void setItype ( Integer rs, Integer rt, Integer addr );
    void setBranch( Integer rs, Integer rt, const char *s );
    void setBranch( Integer rs, Integer rt, Integer off );
    void setJump( Integer address );
    void setJump( const char *s );
    void setJr( Integer rs );
    void dumpToBuff(AppendBuffer &);
};

class Parser {
    private:
    const char *instructions;
    Lexer lex;
    char buff[256];
    Instruction current;
    std::string currentStr;
    size_t currentLine;
    bool err;
    bool parseSuccess;
    size_t insCount;
    bool end; // true if done parsing
    AppendBuffer errorBuffer;
    ParseObj parseIns( );
    ParseObj parseRtype();
    ParseObj parseItype();
    ParseObj parseLS();
    ParseObj parseBranch();
    ParseObj parseJump();
    ParseObj parseJr();
    int parseRegister();
    int parseInt();
    int parseExpr();
    int parseAddExpr();
    int parseMulExpr();
    int parseUnaryExpr();
    int parseBaseExpr();
    void genParseError();
    void displayError(const char *fmt,...);
    void parseInsEnd();
    void genRegisterError( RegisterException & );
    void genLexerMatchError( LexerMatchException &);
    void genInstructionEndException();
    void genExpressionError( ExpressionException &);
    void genlabelError();
    public:
    Parser ( const char *p );
    inline bool isSuccess(){ return parseSuccess;} 
    void init(const char *p);
    ParseObj parse();
    static void test();
    static void exprTest();
    inline size_t getInsCount(){ return insCount; }
    inline bool isEnd(){ return end; }
};

extern strToIntMap regMap; // defined in common.cpp
extern strToInsMap insMap;  // defined in common.cpp
extern std::vector < ErrorInfo > errorList; // defined in common.cpp
#endif
