#include <vector>
#include <cstdint>
typedef uint32_t Code;
class Generator {
    vector <Code> prog;
    vector <ParseObj *> objs;
    Generator (){
        prog.reserve( 100 );
        objs.reserve( 100 );
    }
    Code encode( ParseObj * );
}

Code Genreator :: encode ( ParseObj *p ){
}

Code Generator :: generate ( ){
    for ( auto iter = objs.begin(); iter != objs.end(); iter++ ){
        if ( iter->ins.isBranch() || iter->ins.isJump() ){
            // resolve the label to respective jump/ branch addressses
        } 
        encode( iter );
    }
}