#ifndef GEN_HPP

#define GEN_HPP
#include <vector>
#include <fstream>
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

class Generator {
    std::vector <Code> prog;
    std::vector <ParseObj *> objs;
    const char *file;
    bool parseSuccess;
    bool genSuccess;
    size_t totalIns;
    Code encodeRtype(const ParseObj *);
    Code encodeItype( const ParseObj *);
    Code encodeBranch( const ParseObj *);
    Code encodeJump( const ParseObj *);
    Code encodeObj( const ParseObj * );
    void displayError(const ParseObj *,const char *fmt,...);
    bool resolveBranch(const ParseObj *);
    bool resolveJump(const ParseObj *);
    public:
    Generator (const char * );
    ~Generator () ;
    bool parseFile();
    bool encode();
    static void test();
    void generateFile(const char *path);
    void generateFile(const std::string &path); // TODO
    void displayObjs();
};

extern  char *errBuff; // defined in main.cpp
extern char *formatErr(const char *,...); // defined in main.cpp
extern strToIndexMap labelMap; // defined in parse.cpp
#endif