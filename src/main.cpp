#include <cstdarg>
#include <iostream>
#include <string>
#include <cctype>
#include <map>
#include "lex.cpp"
#include "parse.cpp"

int main( ){
    init();
    Lexer::test();
    Parser::test();
}