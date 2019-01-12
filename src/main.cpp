#include <cstddef>
#include <vector>
#include <cstdarg>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
using std::vector;
using std::string;
#include "../include/common.hpp"
#include "../include/lex.hpp"
#include "../include/parse.hpp"
#include "../include/gen.hpp"

#define ERR_BUFF_SIZE 1024

char *errBuff;
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

bool assemble(){
    string s("addi $s0,$s1,12\n"\
             "addr $s0,$s1,$s3\n"\
             "lw $s0,0($s1)\n");
    Generator gen(s.c_str());
    gen.parseFile();
    gen.encode();
    try {
        gen.generateFile(nullptr);
    } catch ( FileException &f ){
        std:: cerr << f.getError() << std::endl;
        return false;
    }
    return true;
}


int main( ){
    errBuff = new char[ERR_BUFF_SIZE];
    init();
    Lexer::test();
    Parser::test();
    Generator::test();
    assemble();
    delete errBuff;
}