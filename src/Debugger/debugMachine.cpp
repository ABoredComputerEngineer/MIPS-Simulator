// contains code related to the debuggin machine used by the debugger
#include <debugMachine.hpp>
std::ostream& operator << ( std::ostream &out, Machine::ExceptionType type ){
    #define PROCESS(x) case x : out << #x; break;
    // eg. PROCESS (Machine::OVERFLOW ) expands to 
    // case Machine::OVERFLOW: out << "Machine::OVERFLOW"; break;
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
    return out;
    #undef PROCESS
}

std::unordered_map < Machine::ExceptionType , std::string, ExceptionClassHash > exceptStr = {
    { Machine::ExceptionType::OVERFLOW, "Overflow occured" }, 
    { Machine::ExceptionType::INVALID, "Invalid Instruction" }, 
    { Machine::ExceptionType::TRAP, "Software Breakpoint occured" }, 
    { Machine::ExceptionType::MEM_OUT_OF_RANGE, "Memory access out of valid range" }, 
    { Machine::ExceptionType::INVALID_WRITE, "Writing into an invalid memory address" },
    { Machine::ExceptionType::INVALID_READ, "Reading from an invalid memory address" }, 
    { Machine::ExceptionType::MEM_UNALIGNED_READ, "Unaligned memory read"},
    { Machine::ExceptionType::MEM_UNALIGNED_WRITE, "Unaligned memory write"},
};


size_t DebugMachine::getNextInsNumber(){
     return 1 + ( pc - basePC ) /4; 
}



DebugMachine :: DebugMachine ( int size ):Machine ( size ), exceptionRaised(false), halted( false ){}

void DebugMachine::setException(DebugMachine::ExceptionType t){
     exceptionRaised = true;
     sr  |= static_cast<unsigned int>( t );
     type = t;
     epc = pc;
#if 0
     std::cerr<< "Exception name : " << type << std::endl;
     std::cerr << "Exception info: " << exceptStr[type] << std::endl;
     std::cerr << "Exception thrown by instruction: ";
     if ( isDebug ){
         // if we are in debug mode show corresponding line
         size_t lineNum = insNumMap[ currentInsNumber ];
         std::cerr << srcCode[ lineNum ] << std::endl;
         std::cerr << "At line : " << std::dec << lineNum << std::endl; 
         
     } else {
         std::cerr<< "0x" << std::hex << currentIns << std::endl;
     }
     std::cerr << std::endl;
#endif
}


void DebugMachine::executeSingle(){
     // The first two are just sanity checks and __should__ never evaluate to true ( the first one is more likely to evaluate to true 
     if ( halted ) return;
     currentIns = getWord( memory.vals + pc ); 
     if ( !currentIns ){ // we have reached the end of the program
          halted = true;
          return;
     }
     currentInsNumber = 1 + ( pc - basePC ) / 4;
     pc += 4;
     size_t op = getBits(currentIns,31,26);
     if ( !op ){
         op = 64 + FUNC(currentIns);
     }
     if ( functions.count(op) ){ // equivalent to searching for op in the list of available function
         InsPointer fp = functions[op];
         ( this->*fp )();
     } else if ( op == 0x3fu ){ 
         // 0x3f is the breakpoint/trap signal
         setException( ExceptionType:: TRAP); // set the trap flag in exception register
         return;
     } else {
         setException( ExceptionType :: INVALID ); // set invalid instruction flag in exception register
         return;
     }
     if ( !getWord( memory.vals + pc ) ){
          // Finished executing all the instructions
          halted = true;
     }
}

void DebugMachine::executeDebug(){
     while ( !halted && !exceptionRaised ){
          executeSingle();
     }
}

Word DebugMachine::getWordAt( size_t addr ){
     if ( addr > memory.byteCount - 3 ){
         throw RequestException(RequestException::MEM_OUT_OF_RANGE,"Trying to read memory out of bounds");
         return 0;
     }
    return getWord( memory.vals + addr ); 
}

void DebugMachine::setWordAt(size_t addr, Word word ){
    memory.vals[addr] = static_cast<uint8_t>( word & (0xff) );
    memory.vals[addr + 1] = static_cast<uint8_t>( ( word >> 8 ) & (0xff) );
    memory.vals[addr + 2] = static_cast<uint8_t>( ( word >> 16 ) & (0xff) );
    memory.vals[addr + 3] = static_cast<uint8_t>( ( word >> 24 ) & (0xff) );
}

RegisterInfo DebugMachine::getRegisterInfo(){
    RegisterInfo regInfo;
    memcpy( &regInfo, reg, sizeof(reg) );
    return regInfo;
}

RequestException :: RequestException( RequestException::ExceptionType t, const char *s ):\
    type(t), err(s){}


