#include "vm.hpp"

void Machine::addr( ){
#if DEBUG 
    size_t rs = getBits(currentIns,25,21);
    size_t rt = getBits(currentIns,20,16);
    size_t rd = getBits(currentIns,15,11);
    reg[rd] = reg[rs] + reg[rt];
#else
    reg[RD(currentIns)] = reg[RS(currentIns)] + reg[RT(currentIns)];
#endif
}
void Machine::addi(){
#if DEBUG
    size_t rs = getBits(currentIns,25,21);
    size_t rt = getBits(currentIns,20,16);
    size_t imm = getBits(currentIns,15,0);
    reg[rt] = reg[rs] + imm;
#else
    reg[RT(currentIns)] = reg[RS(currentIns)] + IMM(currentIns);
#endif
}

/*
 * ==============================
 * | Overflow handling |
 * ==============================
 * 
 * ------------------------------
 * | Shift Instructions |
 * ------------------------------
 * Overflow check for left shifting:
 * 
 * Assume n is a B-bit number. n is always treated as an unsigned number as we
 * are only concerned about the bit pattern and not the actual value.
 * Also suppose that UINTB_MAX is the maximun value that can be represented by a 
 * B-bit number.
 * Then, to check if n overflows when it is left shifted by s ( 0 =< s < B )
 * we check if the following statement is true 
 * ( n << s ) > UINT32_MAX
 * Changing notations for the lshift operation into exponentiation, ( assume ^ is the exponentiation operator )
 * and using some elementary school math we get 
 * => n * ( 2 ^ s ) > UINT32_MAX 
 * => n > UINT32_MAX / ( 2 ^ s )
 * => n > ( UINT32_MAX >> s )
 * which is our final expression for checking for overflows during left shifting.
 * In our machine, overflow in left shift is DEFINED by setting all the bits to zero,
 * i.e, the result of an overflowing left shift operation is simply zeroed out
 *  so an expression like 
 *  UINTB_MAX << 1 evaluates to 0
 * 
 * Unlike the left shift operation, we have to consider a few cases with the rshift operation
 * Assuming a B-bit number 'n' is rshifted by a number 's'
 * if n == 0, then the result of rshift operation is 0 regardless of the value of s
 * if UINTB_MAX >= n > 0 and B > s >= 0, then the rshift follows as usual
 * Else if s >= B then the result of the rshift is zero, regardless of the value of n
 *   
 */


void Machine::sll( ){
    Word rt = RT(currentIns);
    Word shamt = SHAMT(currentIns);
    if ( reg[rt] > ( WORD_MAX >> shamt ) ){
        reg[RD(currentIns)] = 0;
    } else {
        reg[RD(currentIns)] = reg[rt] << shamt;
    }
}

void Machine::addu(){ addr(); } // the instructions addr and addu are equivalent
void Machine::subr(){
    reg[RD(currentIns)] = reg[RS(currentIns)] - reg[RT(currentIns)];
}
void Machine::subu(){ subr(); }

void Machine::andr(){
    reg[RD(currentIns)] = reg[RS(currentIns)] & reg[RT(currentIns)];
}
void Machine::orr(){
    reg[RD(currentIns)] = reg[RS(currentIns)] | reg[RT(currentIns)];
}
void Machine::xorr(){
    reg[RD(currentIns)] = reg[RS(currentIns)] ^ reg[RT(currentIns)];
}
void Machine::nor(){
    reg[RD(currentIns)] = ~( reg[RS(currentIns)] | reg[RT(currentIns)] );
}

void Machine::slt(){
/*
 * slt does a signed comparison between the values of the registers.
 * We have to explicitly cast them into signed numbers as signed comparison is different from unsigned 
 */
    int32_t rs = static_cast<int32_t>( reg[RS(currentIns)] );
    int32_t rt = static_cast<int32_t>( reg[RT(currentIns)] );
    reg[RD(currentIns)] = ( rs < rt );
}
void Machine::sltu(){
    reg[RD(currentIns)] = ( reg[RS(currentIns)] < reg[RT(currentIns)] );
}

void Machine::addiu(){ addi(); }
void Machine::slti(){
    int32_t rs = static_cast<int32_t>( reg[RS(currentIns)] );    
    int32_t imm = static_cast<int32_t>( IMM(currentIns) );
    reg[RT(currentIns)] = ( rs < imm );
}
void Machine::sltiu(){
    Word imm = static_cast<Word>( IMM( currentIns ) );
    reg[RT(currentIns)] = reg[RS(currentIns)] < imm;
}

void Machine::andi(){
    Word imm = static_cast<Word>( IMM( currentIns ) );
    reg[RT(currentIns)] = reg[RS(currentIns)] & imm ;
}
void Machine::ori(){
    Word imm = static_cast<Word>( IMM( currentIns ) );
    reg[RT(currentIns)] = reg[RS(currentIns)] | imm ;
}
void Machine::xori(){
    Word imm = static_cast<Word>( IMM( currentIns ) );
    reg[RT(currentIns)] = reg[RS(currentIns)] ^ imm ;
}

void Machine::lb(){
/* 
 * A more verbose version:
 * 
 *   size_t rt = RT(currentIns);
 *   size_t rs = RS(currentIns); // rs is a 'pointer' variable, in this case just a normal word
 *   size_t offset = IMM(currentIns); // offset is byte indexed
 *   size_t index = offset + reg[rs]; 
 *   Word value = static_cast<Word>( static_cast<char>( memory.vals[index] ) ); 
 *   reg[rt] = value;
 * 
 *  -----------------------------------------
 *  | The Weird Type Casting |
 *  -----------------------------------------
 *  Remember, lb loads the byte as a signed byte.
 *  To load the byte into a register which is larger than the byte, we 
 *  first cast the byte ( unsigned char ) into signed type ( char ).
 *  So, when the char is converted into a unsigned type Word, The sign bit of the
 *  char will fill all the leftmost bits, which is the desired behaviour
 */
    reg[RT(currentIns)] = static_cast<Word>( static_cast<char>( memory.vals[ reg[RS(currentIns)] + IMM(currentIns)] ) );
    
}
void Machine::lh(){
    /*
     * Here, getHalfWord returns the sign extened 32-bit representation
     * Which is what we want
     */
    size_t rt = RT(currentIns);
    size_t rs = RS(currentIns);
    long int offset = static_cast<int32_t>( IMM(currentIns) );
    Word val = getHalfWord( memory.vals + ( offset + reg[rs] ) );
    reg[rt] = val;
}

void Machine::lw(){
    size_t rt = RT(currentIns);
    size_t rs = RS(currentIns);
    long int offset = static_cast<int32_t>( IMM(currentIns) );
    Word val = getWord( memory.vals + ( offset + reg[rs] ) );
    reg[rt] = val;
}

void Machine::lbu(){
    size_t rt = RT(currentIns);
    size_t rs = RS(currentIns); // rs is a 'pointer' variable, in this case just a normal word
    long int offset = static_cast<int32_t>( IMM(currentIns) ); // offset is byte indexed
    size_t index = offset + reg[rs]; 
    Word value = static_cast<Word>( memory.vals[index] ); 
    reg[rt] = value;

}
void Machine::lhu(){
    size_t rt = RT(currentIns);
    size_t rs = RS(currentIns);
    long int offset = static_cast<int32_t>( IMM(currentIns) );
    Word val = ( getHalfWord( memory.vals + ( offset + reg[rs] ) ) ) & ( UMAX_16 );
    reg[rt] = val;
}
void Machine::sb(){
   size_t rt = RT(currentIns);
   size_t rs = RS(currentIns);
   byte val =  reg[rt] & BYTE_MAX;
   long int offset = static_cast<int32_t>( IMM(currentIns) );
   memory.vals[reg[rs] + offset] = val;
}
void Machine::sh(){
   size_t rt = RT(currentIns);
   size_t rs = RS(currentIns);
   long int offset = static_cast<int32_t>( IMM(currentIns) );
   size_t val = reg[rt] & UMAX_16; // convert value at rt into a 16-bit value
   memory.vals[reg[ rs ] + offset] = static_cast<byte>( val & BYTE_MAX ); // set the lower byte of the value at the starting of the half word
   memory.vals[reg[ rs ] + offset + 1 ] = static_cast<byte>( ( val >> 8 ) & BYTE_MAX );
}
void Machine::sw(){
   size_t rt = RT(currentIns);
   size_t rs = RS(currentIns);
   long int offset = static_cast<int32_t>( IMM(currentIns) );
   size_t base = reg[rs];
   size_t val = reg[rt] & WORD_MAX; // convert value at rt into a 16-bit value
   memory.vals[base + offset] = static_cast<byte>( val & BYTE_MAX ); // set the lower byte of the value at the starting of the half word
   memory.vals[base + offset + 1 ] = static_cast<byte>( ( val >> 8 ) & BYTE_MAX );
   memory.vals[base + offset + 2] = static_cast<byte>( ( val >> 16 ) & BYTE_MAX );
   memory.vals[base + offset + 3 ] = static_cast<byte>( ( val >> 24 ) & BYTE_MAX );
}


/*
 *
 * ==========================================================
 * | Sign Extension |
 * ==========================================================
 * The branch instruction depend on the value of their immediate feild,
 * which gives the offset by which the program counter must be changed
 * to get to the desired instruction.
 * 
 * Now, The immediate field is a signed 16-bit integer. The positive
 * value of this offset means jumping forward and the negative value 
 * means jumping backward.
 * 
 * The offset is extracted with the help of the IMM macro, which in turn
 * calls the getBits function to extract the relevant bits. Now, getBits()
 * returns a Word, which happens to be an unsigned 32 bit integer, with the 
 * exact bits that were present i.e, there is no sign extension done by this 
 * function ( and for a very good reason ).
 * 
 * Hence, we have to check the sign of the sixteen bit manually. 
 * It is achieved by the following steps:
 * 1. Check the sixteenth bit of the word. For this we shift bits to the
 *  right by 15 ( not 16 ), so that the sixteen bit is now shifted to the 
 *  least significant bit. We check this bit by anding with 1
 * 2. If the sign bit was found to be 1, then the offset if -ve  ( +ve otherwise )
 * 3. Now we extend the sign as follows
 *      offset = ( ~0  << 16  ) | x
 *  How does it work?
 *      ~0  => is a long list of bits of 1s
 *      ( ~0 << 16 ) => shift the list of 1s by 16 so that the last sixteen bits will be 0
 *      ( ~0 << 16 ) | x  => Set the lower sixteen bits ( which are all zeros ) to the bits of x
 *                          which also has sixteen bits
 * The resultant number is the required sign extened offset
 * 
 * For jumps, the address field is an unsigned integer so we don't have to check for its sign
 */ 
void Machine::beq(){
    size_t rtVal = reg[RT(currentIns)];
    size_t rsVal = reg[RS(currentIns)];
    Word imm = ( IMM(currentIns) );
    int32_t offset = imm;
    if ( ( imm >> 15) & 1 ){
        offset = (~0 << 15) | imm;
    }
    if ( rtVal == rsVal ){
        if ( offset < 0 ){
            pc -= ( ABS(offset) << 2 );
        } else {
            pc += ( offset << 2 ) ;
        }
    }
}
void Machine::jmp(){
    size_t addr = getBits(currentIns,25,0);
    pc = memory.textStart + ( addr << 2 );
}
void Machine::bne(){
    size_t rtVal = reg[RT(currentIns)];
    size_t rsVal = reg[RS(currentIns)];
    Word imm = ( IMM(currentIns) );
    int32_t offset = imm;
    if ( ( imm >> 15) & 1 ){
        offset = (~0 << 15) | imm;
    }
    if ( rtVal != rsVal ){
        if ( offset < 0 ){
            pc -= ( ABS(offset) << 2 );
        } else {
            pc += ( offset << 2 ) ;
        }
    }
}

void Machine::jal(){
    size_t addr = getBits(currentIns,25,0);
    // pc is the full address of the next instruction but we have to store the word number of the next instruction
    // and hence the rshift by 2    
    reg[RA] = pc ;
    pc = memory.textStart + ( addr << 2 );
}

void Machine::jr(){
    size_t rsVal = reg[RS(currentIns)];
    pc = ( rsVal );
}