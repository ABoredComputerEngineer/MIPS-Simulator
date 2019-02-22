#ifndef DEBUG_MACHINE_HPP
#define DEBUG_MACHINE_HPP
#include <VM/vm.hpp>
struct RegisterInfo {
    Word zero;
    Word at;
    Word v0, v1;
    Word a0, a1, a2, a3;
    Word t0, t1, t2, t3, t4, t5, t6, t7;
    Word s0, s1, s2, s3, s4, s5, s6, s7;
    Word t8, t9;
    Word k0,k1;
    Word gp, sp, fp, ra;
};

struct RequestException {
    enum ExceptionType {
        MEM_OUT_OF_RANGE, // Request for memory out of the valid memory range
    };
    ExceptionType type;
    std::string err;

    RequestException( ExceptionType kind,const char *s); 
};
class DebugMachine : public Machine {
     private:
     bool exceptionRaised;
     bool halted;
     ExceptionType type;
     public:
     DebugMachine ( int size );
     virtual void setException( ExceptionType ); // overridden function 
     void executeSingle(); // for single steping
     void executeDebug();
     Word getWordAt(size_t addr);
     void setWordAt(size_t addr, Word);
     inline Word getPC() { return pc; }
     inline void setPC(Word w) { pc = w ; }
     inline Word getEPC(){ return epc; }  
     inline Word getSR() { return sr; }
     inline size_t getInsNum(){ return currentInsNumber; }
     inline void setExceptionFlag( bool val ){ exceptionRaised = val; }
     inline void clearSR() { sr = 0 ;};
     inline bool isExceptionRaised(){ return exceptionRaised; }
     inline bool isHalted(){ return halted; }
     inline size_t getCurrentInsNumber( ){ return currentInsNumber; } 
     size_t getNextInsNumber();
     inline Word getBasePC(){ return basePC; }
     RegisterInfo getRegisterInfo();
     inline ExceptionType getException(){ return type;}
     inline byte* getMemAt( size_t address ) { return memory.vals + address; };
};
extern std::unordered_map < Machine::ExceptionType , std::string, ExceptionClassHash > exceptStr; // defined in debugMachine.cpp
#endif
