#ifndef LEX_HPP

#define LEX_HPP
struct Position {
    size_t row, col;
    Position ( size_t a, size_t b ): row(a), col(b){}
};


struct Lexer{
    enum TokenKind {
        NONE ,
        TOKEN_BAD,
        TOKEN_REGISTER,
        TOKEN_INSTRUCTION,
        TOKEN_INT,
        TOKEN_NAME,
        TOKEN_LPAREN,
        TOKEN_RPAREN,
        TOKEN_COMMA,
        TOKEN_COLON,
        TOKEN_END,
        TOKEN_NEWLINE,
        TOKEN_ADD,
        TOKEN_SUB,
        TOKEN_MUL,
        TOKEN_DIV,
        TOKEN_LSHIFT,
        TOKEN_RSHIFT,
        TOKEN_BAND,
        TOKEN_BOR,
        TOKEN_BXOR,
        TOKEN_COMPLEMENT,
        TOKEN_NOT,

    };
    TokenKind kind;
    const char *stream;
    const char * line_start; // Points to the start of the line
    const char *start; // Points to the start of the current token
    const char *previous; // Points to the previous instruction
    size_t prev_line;
    Integer int_val;
    std::string str; // string of the current object
    size_t line; // the line number 

    Lexer ( const char *x );
    void init(const char *str);
    void next( char *buff );
    void scanInt(char *buff);
    inline bool isIns();
    bool match(TokenKind, char *);
    bool expect(TokenKind, char *);
    inline int getInt(){ return int_val;}
    inline bool isToken(TokenKind);
    inline bool isAddOp();
    inline bool isMulOp();
    inline bool isUnaryOp();
    Position currentPos();
    void nextInstruction(char *buff);
    static void test();
    std::string instructionString();
    void previousIns(char *);
    std::string insString();
    inline size_t getLineNum( ){ return line; }
};
inline bool Lexer::isIns(){ return kind == TOKEN_INSTRUCTION; }
inline bool Lexer::isToken( TokenKind k ){ return kind == k; }
inline bool Lexer::isAddOp(){
    return ( kind == TOKEN_ADD || kind == TOKEN_SUB );
}
inline bool Lexer::isMulOp(){
    return (kind >= TOKEN_MUL && kind <= TOKEN_BXOR );
}

inline bool Lexer::isUnaryOp(){
    return ( kind == TOKEN_COMPLEMENT || kind == TOKEN_NOT || kind == TOKEN_SUB );
}
extern charToIntMap digitMap; // defined in common.cpp 
extern strToBoolMap keywordMap; // defined in common.cpp
#endif