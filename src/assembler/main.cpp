#include <cstddef>
#include <cstdarg>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
using std::string;
#include "common.hpp"
#include "lex.hpp"
#include "parse.hpp"
#include "gen.hpp"

#define ERR_BUFF_SIZE 1024
#define DEBUG 1

#ifdef __linux__

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#endif
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

bool isDirectory ( const char *path ){
    #ifdef __linux__
    struct stat s;
    stat( path, &s );
    return S_ISDIR(s.st_mode);
    #else
    std::cout << "Skipping directory checks as the OS is not Linux " << std::endl;
    return false;
    #endif
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


bool assemble(const char *inPath, const char *outPath, bool isDump, const char *dumpPath){
    string outFile( outPath );
    labelMap.clear();
    if ( isDirectory(outPath) ){
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
    if ( !gen.isParseSuccess() ){
        return false;
    }
    gen.encode();
    if ( !gen.isGenSuccess() ){
        return false;
    }
    if ( isDump ){
        gen.dumpObjs();
        gen.dumpToFile(dumpPath);
    }
    try {
        gen.generateFile(outFile.c_str());
    } catch ( FileException &f ){
        std:: cerr << f.getError() << std::endl;
        return false;
    }
    delete []content;
    return true;
}


/*
 *  The program arguments are specified as:
 *  <prog_name> <input_path> -o <output_path> -d <dump_path>
 *  The prog_name and input_path are fixed
 *  The -o & -d are always followed by their repective paths
 */

const char *splitPath(const char *fullPath, char delim ){
  // fullPath = ~/random/random/zz.c
  // output =  pointer to the character '/' before 'zz.c'
    size_t len = strlen(fullPath);
    const char *s = nullptr;
    for ( s = fullPath + len - 1; s != fullPath && *s!=delim ; s-- );
    return s;    
}


int main(int argc, char *argv[] ){
    initialize_assembler();
    Lexer::test();
    Parser::test();
    Generator::test();
    if ( argc < 2 ){
        std::cout << "Please input a file" << std::endl;
        return 1;
    }
    errBuff = new char[ERR_BUFF_SIZE];
    /*
     * We assume the arguments to be in following format
     * ./vm <file_path> -[args] <value related to the arg>
     * so, argv[1] is always the file path
     */
    const char *filePath = argv[1];
    const char *split  = splitPath(filePath,'/');
    std::string fileName( ( split == filePath )?filePath:(split+1) );
    std::string fileDirectory("");
    if ( split != filePath ){
        for ( const char *x = filePath; x != (split+1) ; x++ ){
            fileDirectory += (*x);
        }
    } 
    const char *outPath = nullptr; 
    const char *dumpPath = nullptr;
    bool isDump = false;
    for ( int i = 2; i < argc ; i+=2  ){
        std::string str(argv[i]);
        if ( i + 1 >= argc ){
            std::cerr << "Expected location after " << str << " argument." << std::endl;
            return 0;
        }
        if ( str == "-o" ){
            outPath = argv[i+1];
        } else if ( str == "-d" ){
            isDump = true;
            dumpPath = argv[i+1];
        }
    }
    string outFile;
    if ( !outPath ){ // output file path is not given
        const char *sp = splitPath( fileName.c_str(), '.' );
        if ( sp == fileName.c_str() ){ // file has no extension
            outFile = fileName;
            outFile += ".bin";
        } else {
            for ( const char *x = fileName.c_str(); (intptr_t)x != (intptr_t)sp ; x++ ){ // ignores the delim
                outFile += (*x);
            }
            outFile+=".bin";
        }
    } else {
        outFile = outPath;
    }
    assemble( filePath, outFile.c_str(), isDump, dumpPath );
    delete []errBuff;
}