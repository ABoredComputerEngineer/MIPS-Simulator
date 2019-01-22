#include <cstddef>
#include <vector>
#include <cstdarg>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
using std::vector;
using std::string;
#include "common.hpp"
#include "lex.hpp"
#include "parse.hpp"
#include "gen.hpp"

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



char *loadProgram(const char *path){
    assert( path );
    std::ifstream inFile(path,std::ifstream::binary|std::ifstream::in);
    if( !inFile.is_open() || !inFile.good() ){
        const char *s = strerror( errno );
        throw FileException(formatErr("%s : %s",path,s));
        return  NULL;
    }
    inFile.seekg(0,std::ios_base::end); 
    size_t len = inFile.tellg();
    char *in = new char[len+1];
    inFile.seekg(0);
    inFile.read(in, len );
    in[len] = 0;
    inFile.close();    
    return in;
}


bool assemble(const char *inPath, const char *outPath){
    struct stat s;
    stat( outPath, &s );
    string outFile( outPath );
    if ( S_ISDIR(s.st_mode) ){
         outFile += "test.bin";
    }
    char *content;
    try {
         content = loadProgram( inPath );
    } catch (FileException &f){
        std:: cerr << f.getError() << std::endl;
        return false;
    }
    Generator gen(content);
    gen.parseFile();
    gen.encode();
    gen.displayObjs();
    try {
        gen.generateFile(outFile.c_str());
    } catch ( FileException &f ){
        std:: cerr << f.getError() << std::endl;
        return false;
    }
    delete []content;
    return true;
}


int main( int argc, char *argv[] ){
    if ( argc < 2 ){
        std::cout << "Please input a location" << std::endl;
    }
    errBuff = new char[ERR_BUFF_SIZE];
    init();
    Lexer::test();
    Parser::test();
    Generator::test();
    assemble(argv[1],argv[2]);
    delete []errBuff;
}
