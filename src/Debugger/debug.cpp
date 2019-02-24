// Contains code related to the debugger only
#include "debug.hpp"

void FileException :: display() {
     std::cerr << str << std::endl;
}


char* Debug::loadFile(const char *path, size_t *buffSize){
     // loads a program into a buffer and returns a pointer to that buffer
     assert( path );
     std::ifstream inFile(path,std::ifstream::binary|std::ifstream::in);
     if( !inFile.is_open() || !inFile.good() ){
          std::ostringstream stream;
          const char *s = strerror( errno );
          stream << "Unable to open file!" << std::endl;
          stream << path << ":" << s;
          throw OpenException( stream.str() );
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

BreakPoint :: BreakPoint ( DebugMachine *m,size_t addr):machine(m),insAddress(addr){
     enable();
}

void BreakPoint::enable(){
     original = machine->getWordAt( insAddress );
     // Set the first six bits to 0x3f
     modified = original | ( 0x3fu << ( sizeof(Word)*8 - 6 ) );
     machine->setWordAt( insAddress, modified);
     enabled = true;
}

void BreakPoint::disable(){
     enabled = false;
//     word |= ( 0x3f << ( sizeof(Word)*8 - 6 ) );
     machine->setWordAt( insAddress, original );
}


Debugger :: Debugger() : machine( 1024 ), currentLine( ){
     //srcCode.reserve( 1000 );
     breakPointList.reserve( 100 );
}

RegisterInfo Debugger :: getRegisters(){
     return machine.getRegisterInfo();
}

void Debugger :: display( const char *str ){
     std::cout << str << std::endl;
}

void display( const char *str ){
     std::cout << str << std::endl;
}

const std::string *getExceptionString( DebugMachine::ExceptionType type ){
#define PROCESS(x)  case x :  s = #x; break;
     static std::string s;
     switch ( type ){
        PROCESS( Machine::ExceptionType::OVERFLOW ) 
        PROCESS( Machine::ExceptionType::INVALID )
        PROCESS( Machine::ExceptionType::TRAP ) 
        PROCESS( Machine::ExceptionType::MEM_OUT_OF_RANGE ) 
        PROCESS( Machine::ExceptionType::MEM_INVALID )
        PROCESS( Machine::ExceptionType::INVALID_WRITE )
        PROCESS( Machine::ExceptionType::INVALID_READ )
        PROCESS( Machine::ExceptionType::MEM_UNALIGNED_READ )
        PROCESS( Machine::ExceptionType::MEM_UNALIGNED_WRITE )
     }
     return &s;
#undef PROCESS
}


void Debugger :: setBreakPoint( size_t x ){
     auto insNumber = lineToInsNumberMap[ x ];
     // Calculate the memory address of the instruction
     assert( insNumber );
     auto base = machine.getBasePC();
     // i.e, instruction 0 lies at address pointed to by base
     // instruction 1 lies at address pointed to by base + 1*4
     auto address = base + ( insNumber - 1 )*4;
     BreakPoint bp( &machine, address );
     breakPointList.push_back( bp );
     breakPointMap[ x ] = bp;     
}

ProgramException :: ProgramException () {}
ProgramException :: ProgramException ( size_t addr, size_t ln, size_t code, const std::string *t, const std::string *inf ):\
          pc( addr ),\
          line( ln ),\
          insCode( code ),\
          type ( t ),\
          exceptInfo( inf )
{}

void ProgramException :: set( size_t addr, size_t ln, size_t code, const std::string *t, const std::string *inf ){
     pc = addr;
     line = ln;
     insCode = code;
     type = t;
     exceptInfo = inf;
}

     
#if 0
void Debugger :: printExceptionInfo(Machine::ExceptionType type){
     pBuffer.clearBuff();
     size_t num = machine.getCurrentInsNum();
     size_t line = insNumberToLineMap[ num ];
     assert( line );
     pBuffer.append("Exception raised by instruction at address 0x%lx.\n", machine.getPC() );
     pBuffer.append("Instrucion: %s\n",srcCode[line -1].c_str() );
     pBuffer.append("Instruction Code: %llx\n",lineToInsMap[ line ] );
     pBuffer.append("Exception Type: %s\n", getExceptionString( type ) );
     pBuffer.append("Exception Info: %s\n", exceptStr[ type ].c_str() ); 
     display( pBuffer.getBuff() );
}
#endif

ProgramException &Debugger :: getExceptionInfo( ){
     return exceptionInfo;
}

void Debugger :: printHaltedMessage(){
     pBuffer.clearBuff();
     pBuffer.append("Program has been halted!\n");
     display( pBuffer.getBuff() );
}

void Debugger :: singleStep(){
     hasException = false;
     // check if the instruction to be executed has a breakpoint
     if ( breakPointMap.count( currentLine ) ){
          // there is a breakpoint
          BreakPoint &breakpoint = breakPointMap[ currentLine ];
          breakpoint.disable();
          Word w = machine.getPC();
          w -= 4;
          machine.setPC( w );
          machine.executeSingle();
          breakpoint.enable();
     } else{
          // there is no break point
          machine.executeSingle();
     }
     if ( machine.isHalted() ){
          printHaltedMessage();
     }
     if ( machine.isExceptionRaised() ){
          auto sr = machine.getSR();
          for ( size_t i = 1; i <= ( size_t )( Machine::ExceptionType::MEM_UNALIGNED_WRITE );i <<= 1 ){
               if ( sr & i ){
                    Machine::ExceptionType type = static_cast< Machine::ExceptionType >( i );
                    size_t num = machine.getCurrentInsNumber();
                    size_t line = insNumberToLineMap[ num ];
                    size_t code = lineToInsMap[ line ];
                    exceptionInfo.set( machine.getPC(),\
                              line,\
                              code,\
                              getExceptionString( type ),\
                              &exceptStr[type] );
                    hasException = true;
#if 0
                    pBuffer.append("Exception raised by instruction at address 0x%lx.\n", machine.getPC() );
                    pBuffer.append("Instrucion: %s\n",srcCode[line -1].c_str() );
                    pBuffer.append("Instruction Code: %llx\n",lineToInsMap[ line ] );
                    pBuffer.append("Exception Type: %s\n", getExceptionString( type ) );
                    pBuffer.append("Exception Info: %s\n", exceptStr[ type ].c_str() ); 
#endif
               }
          }
          machine.clearSR();
          machine.setExceptionFlag(false);
     }
     currentLine = insNumberToLineMap[machine.getNextInsNumber()];
}

void Debugger :: continueExecution (){
     hasException = false;
     machine.executeDebug();
     if ( machine.isHalted() ){
          return;
     }
     if ( machine.isExceptionRaised() ){
          hasException = true;

          size_t num = machine.getCurrentInsNumber();
          size_t line = insNumberToLineMap[ num ];
          size_t code = lineToInsMap[ line ];
          auto type = machine.getException();
          if ( type == Machine::ExceptionType :: TRAP ){
               currentLine = line;
          } 
          exceptionInfo.set( machine.getPC(),\
                    line,\
                    code,\
                    getExceptionString( type ),\
                    &exceptStr[type] );
          machine.clearSR();
          machine.setExceptionFlag(false);
     }
}

void Debugger :: generateMaps( const char *buff ){
     const char *start = buff + mheader.dbgOffset + sizeof(DebugSection);
     LineMapEntry entry;
     for ( size_t i = 0; i < dbgSection.lineMapCount; i++ ){
          memcpy( &entry, start, sizeof(LineMapEntry) );
          lineToInsMap[ entry.lineNum ] = entry.ins; 
          lineToInsNumberMap[ entry.lineNum ] = entry.insNum;
          insNumberToLineMap[ entry.insNum ] = entry.lineNum;
          start += sizeof( LineMapEntry );
     }
}

void Debugger :: parseDebugInfo(const char *buff){
     //std::ifstream inFile( dbgSection.srcPath, std::ios::binary | std::ios::in );
     //if ( !inFile.is_open() || !inFile.good() ){
     //     std::ostringstream stream;
     //     stream << dbgSection.srcPath<< ":" << strerror( errno ) << std::endl;
     //     stream << "Failed to load the source program" << std::endl;
     //     throw OpenException( stream.str() ); 
     //}
     //std::string s; 
     //while ( getline( inFile, s ) ){
     //     srcCode.push_back( s );
     //}
     generateMaps(buff);
}


void Debugger :: loadProgram(const char *buff, size_t size ){
     if ( size <= sizeof(mheader) ){
          throw InvalidFileError("File is not large enough. You probably opened the wrong file");
     }
     memcpy( &mheader, buff, sizeof(mheader) );
     if ( !mheader.dbgOffset ){
          throw InvalidFileError( "File is not in Debug Mode" );
     } else if ( mheader.dbgOffset > size ){
          throw InvalidFileError( "Invalid File Opened." );
     }
     memcpy( &pheader, buff + mheader.phOffset , sizeof(ProgramHeader) );
     try {
          machine.loadProgram( buff + mheader.textOffset, pheader.progSize ); 
          memcpy( &dbgSection, buff + mheader.dbgOffset, sizeof(DebugSection) );
          parseDebugInfo(buff);
     } catch ( MachineException &m ){
          throw;
     } catch ( OpenException &e ){
          throw;
     }
     currentLine = insNumberToLineMap[ machine.getNextInsNumber() ];
     
}


//struct RegisterInfo {
//    Word zero;
//    Word at;
//    Word v0, v1;
//    Word a0, a1, a2, a3;
//    Word t0, t1, t2, t3, t4, t5, t6, t7;
//    Word s0, s1, s2, s3, s4, s5, s6, s7;
//    Word t8, t9;
//    Word k0,k1;
//    Word gp, sp, fp, ra;
//};
//


// Print 'bytes' number of byte into 'buff' from 'address' of the memory
char* Debugger :: getMem(AppendBuffer &buff, size_t address, size_t bytes ){
     byte *start = machine.getMemAt( address );
     int count = 1;
     for ( size_t i = 0; i < bytes; i+=4 ){
         buff.append("0x%-5lx: ",address + i);
         for ( size_t k = 0; k < 4 ; k++ ){
             buff.append("%02x ",start[i + k] & 0xff );
         }
         buff.append("\t");
         count++;
         if ( count % 2 ) { count = 1; buff.append("\n");}
     }
     buff.append("\n"); 
     return buff.getBuff();
}


