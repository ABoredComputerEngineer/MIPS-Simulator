#ifndef DEBUG_HPP

#define DEBUG_HPP
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <linenoise.h>
#include <unordered_map>
#include <VM/printBuffer.hpp>
#include <debugMachine.hpp>

namespace Debug{
     char *loadFile( const char *path, size_t *size );
}
class FileException{
     protected:
     std::string str;
     public:
     FileException (const char *s):str(s){}
     FileException (const std::string &s):str(s){}
     void display();
};

class OpenException : public FileException {
     public:
          OpenException ( const char *s ): FileException( s ){}
          OpenException ( const std::string &s ): FileException( s ){}
};

class InvalidFileError : public FileException {
     public:
         InvalidFileError ( const char *s ): FileException(s){}
         InvalidFileError ( const std::string &s ): FileException(s){}
};
struct LineMapEntry {
     size_t lineNum;
     size_t ins;
     size_t insNum;
};


class BreakPoint {
     private:
          DebugMachine *machine;
          bool enabled;
          Word original; // the actual instruction the machine will execute
          Word modified; // the instruction with the first six bits modified to 0x3f ( which signifies a breakpoint )
          size_t insAddress; // value of pc of the address where we have to set a breakpoint
          byte saved; // byte is typdeffed as uint8_t in vm.hpp, stores a 6-bit opcode value
     public:
          void enable();
          void disable();
          inline bool isEnabled(){ return enabled; }
          BreakPoint ( DebugMachine *m,size_t line );
          BreakPoint ( ){}
          void display();
};

struct ProgramException {
     size_t pc;
     size_t line;
     size_t insCode;
     const std::string *type;
     const std::string *exceptInfo;
     
     ProgramException ();
     ProgramException( size_t , size_t , size_t, const std::string *, const std::string *);
     void set( size_t , size_t , size_t, const std::string*, const std::string *);
};



class Debugger {
     friend class BreakPoint;
     public:
          enum {
               SRCLINES = 10, // always display 10 lines of source code
          };
     private:
     DebugMachine machine; // initialize with number of bytes
     size_t currentLine;
     MainHeader mheader;
     ProgramHeader pheader;
     DebugSection dbgSection;
     ProgramException exceptionInfo;
     std::unordered_map< size_t , size_t > lineToInsMap; // maps line number to instruction code
     std::unordered_map< size_t , size_t > lineToInsNumberMap; // maps line number to instruction  number
     std::unordered_map< size_t , size_t > insNumberToLineMap; // maps line number to instruction  number
     //std::vector < std::string > srcCode;
     AppendBuffer pBuffer; // the print Buffer for displaying stuff like messages
     std::vector < BreakPoint > breakPointList; // stores a list representation of break points
     std::unordered_map< size_t, BreakPoint > breakPointMap; // Maps line number --> Breakpoint
     bool hasException;
     public:
     Debugger ();
     void loadProgram(const char *buff, size_t size);
     void parseDebugInfo( const char *buff );
     void generateMaps( const char *buff ); 
     void displayCode( size_t line );
     void printExceptionInfo(Machine::ExceptionType type );
     void run();
     void singleStep();
     void printHaltedMessage();
     inline bool isHalted() { return machine.isHalted(); }
     void setBreakPoint(size_t line );
     void continueExecution();
     RegisterInfo getRegisters();
     void display(const char *buff);
     char* getMem(AppendBuffer &buff,size_t address, size_t bytes);
     inline std::string getSrcPath() { return dbgSection.srcPath; }
     inline bool isExceptionRaised() { return hasException; }
     inline size_t getLineNumber( ) { return currentLine; }
     ProgramException &getExceptionInfo( );
};

#endif
