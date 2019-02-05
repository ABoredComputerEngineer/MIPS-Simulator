#ifndef GEN_HPP

#define GEN_HPP
#include "common.hpp"
#include <vector>
#include <fstream>
#include <climits>
typedef uint32_t Code;
#define ORIGIN 00000 
enum Limits {
    RS_LEN = 5,
    RT_LEN = 5,
    RD_LEN = 5,
    SHAMT_LEN = 5,
    FUNC_LEN = 6,
    IMM_LEN = 16,
    ADDRESS_LEN = 26,
    FIVE_BIT_MAX = 0x1fu,
    SIX_BIT_MAX = 0x3fu,
    SIXTEEN_BIT_MAX = 0xffffu,
    TWENTY_SIX_BIT_MAX = 0x3ffffffu,
    FIFTEEN_BIT_MAX = 0x7fffu,
    //INT16_MAX = 0x7fff, // 32768
    //INT16_MIN = -0x8000, // -32768
};

#define UINT32_CAST( x ) ( static_cast<uint32_t>( x ) )
#define ABS(x) ( ( (x) > 0 ) ? (x) : ( -(x) ) )
#define IS_UNSIGNED_5(x) ( !( (x) & ~FIVE_BIT_MAX ) )
#define IS_UNSIGNED_6( x ) ( !( (x) & ~SIX_BIT_MAX ) )
#define IS_UNSIGNED_16( x ) ( !( (x) & ~SIXTEEN_BIT_MAX ) )
#define IS_UNSIGNED_26( x ) ( !( (x) & ~TWENTY_SIX_BIT_MAX ) )
#define IS_SIGNED_16(x) ( ( ( x ) >= INT16_MIN ) && ( ( x ) <= INT16_MAX ) )

struct MainHeader{
    char  isa[16]; // string containing the isa that generated the file
    size_t version; // the version of the assembler that generated the binary file
    size_t textOffset; // the number of bytes from the begining from which the actual program code starts
    size_t phOffset; // the number of bytes after which the program header begins
    size_t dbgOffset; // the number of bytes after which the debug section begins
    size_t secOffset; // the number of bytes after which section information is stored, wiil be added in later version

    void setHeader(const char *s);
};


struct ProgHeader {
    size_t progSize; // the total size of only the machine code in bytes
    size_t origin; // the offset from the start of text segment in  memory where the code should be stored

    ProgHeader (size_t size, size_t offset);
};

struct DebugHeader{
    char srcPath[PATH_MAX+1]; // the absolute path of the source file from which the binary file was generated
    size_t lineMapCount; // the number of entires in the line to instruction map
};

struct LineMapEntry {
    size_t lineNum;
    size_t ins;
};
class Generator {
    std::vector <Code> prog;
    std::vector <ParseObj *> objs;
    const char *file;
    const char *srcPath;
    bool parseSuccess;
    bool genSuccess;
    bool debugMode;
    size_t totalIns;
    AppendBuffer dumpBuff;
    Code encodeRtype(const ParseObj *);
    Code encodeItype( const ParseObj *);
    Code encodeBranch( const ParseObj *);
    Code encodeJump( const ParseObj *);
    Code encodeObj( const ParseObj * );
    Code encodePtype( const ParseObj * );
    Code encodeJr( const ParseObj * );
    void displayError(const ParseObj *,const char *fmt,...);
    bool resolveBranch(const ParseObj *);
    bool resolveJump(const ParseObj *);
    void genHeader(AppendBuffer &);
    size_t genLineInfo(std::vector<LineMapEntry> &);
    void genDebugInfo(AppendBuffer &);
    void genMainHeader(std::ofstream &outFile);
    void genProgHeader(std::ofstream &outFile, size_t progSize);
    void genDebugSection( std::ofstream &outFile );
    public:
    Generator (const char *content, const char *src, bool debug );
    ~Generator () ;
    bool parseFile(); // returns true if the parsing completed without any errors
    bool encode();
    static void test();
    void generateFile(const char *path);
    void generateFile(const std::string &path); // TODO
    void displayObjs();
    void dumpObjs();
    void dumpToFile( const char *);
    inline bool isParseSuccess(){ return parseSuccess; }
    inline bool isGenSuccess(){ return genSuccess; }
};


extern  char *errBuff; // defined in main.cpp
extern char *formatErr(const char *,...); // defined in main.cpp
extern strToIndexMap labelMap; // defined in parse.cpp
#endif