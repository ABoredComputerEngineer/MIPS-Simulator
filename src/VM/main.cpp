#include "vm.hpp"

extern void init();
const char *splitPath(const char *fullPath ){
  // fullPath = ~/random/random/zz.c
  // output = ~/random/random
#if defined(_WIN32)
    char delim = '\\';
#else
    char delim = '/';
#endif
    size_t len = strlen(fullPath);
    const char *s = nullptr;
    for ( s = fullPath + len - 1; s != fullPath && *s!=delim ; s-- );
    return s;    
}
char *loadFile(const char *path, size_t *buffSize){
    // loads a program into a buffer and returns a pointer to that buffer
    assert( path );
    std::ifstream inFile(path,std::ifstream::binary|std::ifstream::in);
    if( !inFile.is_open() || !inFile.good() ){
        const char *s = strerror( errno );
        std::cout << s << std::endl;
        return nullptr;
    }
    inFile.seekg(0,std::ios_base::end);
    size_t size = inFile.tellg();
    inFile.seekg(0,std::ios_base::beg);    
    char *buff = new char[size+1];
    inFile.read(buff,size);
    buff[size] = 0;
    *buffSize = size;
    return buff;
}

void directorTest(){
    const char d[] = "$PROJECT/DUMP/dump/.zz.c";
    const char *s = splitPath(d);
    std::string fileName( (s == d )?d:(s+1) );
    std::string directory("");
    if ( s != d ){
        for ( const char *x = d; x != ( s + 1 ) ; x++ ){
            directory += (*x);
        }
    } else {
        directory = "./";
    }
    assert( directory == "$PROJECT/DUMP/dump/");
    assert( fileName == ".zz.c" );
}



int main(int argc, char *argv[] ){
    Machine :: test();
    assert( ALIGN_UP(4,32) == 32 );
    assert( ALIGN_DOWN(127,32) == 96 );
    assert( ALIGN_DOWN(31,4) == 28 );
    directorTest();
    if ( argc < 2 ){
        std::cout << "Please input a file" << std::endl;
        return 1;
    }
    /*
     * We assume the arguments to be in following format
     * ./vm <file_path> -[args] <value related to the arg>
     * so, argv[1] is always the file path
     */
    const char *filePath = argv[1];
    const char *split  = splitPath(filePath);
    const char *dumpPath = nullptr;
    bool isDump = false;
    std::string dump("");
    std::string fileName( ( split == filePath )?filePath:(split+1) );
    std::string fileDirectory("");
    if ( split != filePath ){
        for ( const char *x = filePath; x != (split+1) ; x++ ){
            fileDirectory += (*x);
        }
    } 
    
    for ( int i = 2; i < argc ; i++  ){
        std::string str(argv[i]);
        if ( str == "-d" ){
            if ( i + 1 < argc ){
                dumpPath = argv[i+1];
                i++;
            }
        } else {
            std::cerr << "Unrecognized argument \'" << argv[i+1] << "\'" << std::endl;
        }
    }

    if ( dumpPath ){
        isDump = true;
        dump = dumpPath;
        struct stat s;
        stat(dumpPath,&s);
        if ( S_ISDIR(s.st_mode) ){
            dump += fileName;
        }
    }
    init();
    Machine m(1024 * sizeof(Word) );
    char *buff = nullptr;
    size_t size = 0;
    try {
        buff = loadFile(filePath, &size );
    } catch ( MachineException &m ){
        m.display();
        std::cerr << "Terminating..." << std::endl;
        delete []buff;
        return -1;
    }
    MainHeader mheader;
    ProgramHeader pheader;
    assert( size > sizeof(MainHeader) );
    memcpy( &mheader, buff, sizeof(MainHeader) );
    std::string s(mheader.isa);
    if ( s != "MIPS-32" ){
        std::cerr << "Invalid binary file!" << std::endl;
        return -1;
    }
    memcpy( &pheader, buff + mheader.phOffset, sizeof(ProgramHeader) ); 
    assert( pheader.progSize <= size );
    #if 0
    if ( m.load(filePath) ){
        m.execute();
        if ( isDump ){
            m.dumpMem(dump.c_str());
        }
    }
    #endif
    try {
        m.loadProgram(buff+mheader.textOffset, pheader.progSize );
    } catch ( MachineException &m ){
        m.display();
        std::cerr << "Terminating..." << std::endl;
        delete []buff;
        return -1;
    }
    m.execute();
    if ( isDump ){
        m.dumpMem(dump.c_str());
    }
    delete []buff;
}
