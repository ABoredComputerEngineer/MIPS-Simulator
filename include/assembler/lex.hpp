#ifndef LEX_HPP

#define LEX_HPP
struct Position {
    size_t row, col;
    Position ( size_t a, size_t b ): row(a), col(b){}
};


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
extern charToIntMap digitMap; // defined in common.cpp 
extern strToBoolMap keywordMap; // defined in common.cpp
#endif