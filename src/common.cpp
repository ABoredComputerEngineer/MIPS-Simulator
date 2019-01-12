#include "../include/common.hpp"
using std::string;
strToBoolMap keywordMap;
strToInsMap insMap;
strToIntMap regMap;
charToIntMap digitMap;

#define NEW_INS(NAME,OP,FUNC,KIND,CLASS) ( Instruction ( string (#NAME), OP, FUNC, Instruction::KIND, Instruction::CLASS) ) 
#define ADD_INS(NAME,OP,FUNC,KIND,CLASS) insMap[#NAME] = NEW_INS(NAME,OP,FUNC,KIND,CLASS) 
#define ADD_INSR(NAME,FUNC,KIND) insMap[#NAME] = NEW_INS(NAME,0,FUNC,KIND,RTYPE); keywordMap[#NAME] = true 
#define ADD_INSI(NAME,OP,KIND) insMap[#NAME] = NEW_INS(NAME,OP,0,KIND,ITYPE); keywordMap[#NAME] = true 
#define ADD_INS_JMP(NAME,OP,KIND) insMap[#NAME] = NEW_INS(NAME,OP,0,KIND,JTYPE); keywordMap[#NAME] = true 
#define ADD_INS_BR(NAME,OP,KIND) insMap[#NAME] = NEW_INS(NAME,OP,0,KIND,BRTYPE); keywordMap[#NAME] = true 
void initIns( ){
    ADD_INSR(sll,0,AL);
    ADD_INSR(addr,32,AL);
    ADD_INSR(addu,33,AL);
    ADD_INSR(subr,34,AL);
    ADD_INSR(subu,35,AL);
    ADD_INSR(and,36,AL);
    ADD_INSR(or,37,AL);
    ADD_INSR(xor,38,AL);
    ADD_INSR(xnor,39,AL);
    ADD_INSR(slt,42,AL);
    ADD_INSR(sltu,43,AL);

    ADD_INSI(addi,8,AL);
    ADD_INSI(addiu,9,AL);
    ADD_INSI(slti,10,AL);
    ADD_INSI(sltiu,11,AL);
    ADD_INSI(andi,12,AL);
    ADD_INSI(ori,13,AL);
    ADD_INSI(xori,14,AL);

    ADD_INSI(lb,32,LS);
    ADD_INSI(lh,33,LS);
    ADD_INSI(lw,35,LS);
    ADD_INSI(lbu,36,LS);
    ADD_INSI(lhu,37,LS);

    ADD_INSI(sb,40,LS);
    ADD_INSI(sh,41,LS);
    ADD_INSI(sw,43,LS);

    ADD_INS_BR(beq,4,BRANCH);
    ADD_INS_JMP(jmp,2,JUMP);
    ADD_INS_BR(bne,5,BRANCH);
}
#undef NEW_INS
#undef ADD_INS
#undef ADD_INSR
#undef ADD_INSI
void initKeyword(){
    #define ADD_KEYWORD(X) ( keywordMap[#X] = true )
    ADD_KEYWORD(addr);
    ADD_KEYWORD(addu);
    ADD_KEYWORD(sll);
    ADD_KEYWORD(subr);
    ADD_KEYWORD(subu);
    ADD_KEYWORD(and);
    ADD_KEYWORD(or);
    ADD_KEYWORD(xor);
    ADD_KEYWORD(xnor);
    ADD_KEYWORD(slt);
    ADD_KEYWORD(sltu);
    ADD_KEYWORD(addi);
    ADD_KEYWORD(addiu);
    ADD_KEYWORD(slti);
    ADD_KEYWORD(sltiu);
    ADD_KEYWORD(andi);
    ADD_KEYWORD(ori);
    ADD_KEYWORD(xori);

    ADD_KEYWORD(lb);
    ADD_KEYWORD(lh);
    ADD_KEYWORD(lw);
    ADD_KEYWORD(lbu);
    ADD_KEYWORD(lhu);
    ADD_KEYWORD(sb);
    ADD_KEYWORD(sh);
    ADD_KEYWORD(sw);
    #undef ADD_KEYWORD
}

void initRegisters(){
    regMap["$t0"] = 8;
    regMap["$t1"] = 9;
    regMap["$t2"] = 10;
    regMap["$t3"] = 11;
    regMap["$t4"] = 12;
    regMap["$t5"] = 13;
    regMap["$t6"] = 14;
    regMap["$t7"] = 15;
;
    regMap["$s0"] = 16;
    regMap["$s1"] = 17;
    regMap["$s2"] = 18;
    regMap["$s3"] = 19;
    regMap["$s4"] = 20;
    regMap["$s5"] = 21;
    regMap["$s6"] = 22;
    regMap["$s7"] = 23;
}
void init( ){
    initIns();
    initRegisters();
    digitMap['0'] = 0;
    digitMap['1'] = 1;
    digitMap['2'] = 2;
    digitMap['3'] = 3;
    digitMap['4'] = 4;
    digitMap['5'] = 5;
    digitMap['6'] = 6;
    digitMap['7'] = 7;
    digitMap['8'] = 8;
    digitMap['9'] = 9;
    digitMap['A'] = 10;
    digitMap['B'] = 11;
    digitMap['C'] = 12;
    digitMap['D'] = 13;
    digitMap['E'] = 14;
    digitMap['F'] = 15;
    digitMap['a'] = 10;
    digitMap['b'] = 11;
    digitMap['c'] = 12;
    digitMap['d'] = 13;
    digitMap['e'] = 14;
    digitMap['f'] = 15;
    
}