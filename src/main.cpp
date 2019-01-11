#include <cstdarg>
#include <iostream>
#include <string>
#include <cctype>
#include <map>
#include "lex.cpp"
#include "parse.cpp"
#include "gen.cpp"
int main( ){
    init();
    Lexer::test();
    Parser::test();
    Generator::test();
}