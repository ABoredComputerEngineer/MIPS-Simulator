/*
 * common.hpp includes all the class definitions, types and map 
 * definitions common to all the parts of the assembler
 */

#ifndef ASM_COMMON_HPP

#define ASM_COMMON_HPP
#include <iostream>
#include <string>
#include <unordered_map>
#include <cstdarg>
#include <cstdint>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <VM/printBuffer.hpp>
typedef long int Integer;

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
        PTYPE, // Denotes pseudo instructions which dosen't follow above rules
    };
    std::string str;  
    Integer opcode;
    Integer func;
    InstructionKind kind;
    InstructionClass insClass;
    public:
    Instruction () {}
    Instruction ( std::string s, Integer op,Integer f, InstructionKind k, InstructionClass c):\
    str(s), opcode(op),func(f), kind(k), insClass(c) {}
    inline bool isShift() const;
    inline bool isShiftLogical() const;
    inline bool isShiftVariable() const;
    inline bool isBranch() const ;
    inline bool isJump() const;
    inline InstructionClass type() const { return insClass; };
};
inline bool Instruction :: isShift ( ) const{
    return ( opcode == 0 ) && ( func == 0 || func == 3 || func == 2 || func == 4 || func == 6 );
}

inline bool Instruction :: isShiftLogical() const {
    return ( func == 0 || func == 2 || func == 3 );
}

inline bool Instruction :: isShiftVariable() const {
    return (opcode == 0) && ( func == 4 || func == 6 || func == 7 );
}
inline bool Instruction::isBranch() const {
    return ( opcode == 4 || opcode == 5 );
}

inline bool Instruction::isJump() const{
    return opcode == 2;
}

typedef std::unordered_map< std::string, bool> strToBoolMap;
typedef std::unordered_map< std::string , Instruction > strToInsMap;
typedef std::unordered_map< std::string ,Integer > strToIntMap;
typedef std::unordered_map< char , int > charToIntMap;
typedef std::unordered_map<std::string,size_t> strToIndexMap;

namespace Assembler {
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

}

void initialize_assembler();
#if 0
struct AppendBuffer {
    enum { DEFAULT_SIZE = 1024 };
    char *buff;
    size_t len;
    size_t cap;

    AppendBuffer (size_t buffSize);

    AppendBuffer ();

    ~AppendBuffer();

    void alloc();
    void grow(size_t size);
    char *append(const char *fmt, ... );
    char *appendn(size_t size, const char *s );
    inline void clear(){ len = 0 ;}
    inline char *getBuff() { return buff; }
};
#endif

struct ErrorInfo {
	enum ErrorLocation {
		ERR_LEXER,
		ERR_PARSER,
		ERR_GENERATOR,
	};
	ErrorLocation location;
	size_t lineNumber;
	std::string lineStr;
	std::string errInfo;
	
	ErrorInfo ();	
	ErrorInfo ( ErrorLocation loc, size_t lineNum, const char *line, const char *err );	
	ErrorInfo ( ErrorLocation loc, size_t lineNum, const std::string &str, const char *err );	
};


#endif // COMMON_HPP
