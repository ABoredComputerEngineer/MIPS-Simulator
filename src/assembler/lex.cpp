#include "common.hpp"
#include "lex.hpp"
extern std::vector < ErrorInfo > errorInfo; // defined in common.cpp
using std::string;
Lexer :: Lexer ( const char *x ): stream( x ),line_start(x),line(1){
    str.reserve(256);
}
string Lexer :: insString(){
    string str;
    str.reserve( 256 );
    for ( const char *s = line_start; *s && *s != '\n' ; s++ ){
        str += *s;
    }
    return str;
}

string Lexer :: instructionString (){
    //need to handle it more efficiently
    const char *s = line_start;
    string str;
    while ( *s != '\n'){
        str += *s++;
    }
    return str;
}

Position Lexer::currentPos(){
    return Position ( line, (size_t)( stream - start ) );
}



bool Lexer::match( TokenKind k , char *buff){
    if ( kind == k ){
        next(buff);
        return true;
    } else {
        return false;
    }
}

bool Lexer::expect(TokenKind k , char *buff ){
    return match(k,buff);
}

bool Lexer::matchInsEnd(char *buff ){
    return match(TOKEN_NEWLINE,buff) || match( TOKEN_END,buff);
}
void Lexer :: init ( const char *str){
    line = 1;
    stream = str;
}



void Lexer::scanInt( char *buff ){
    int base  = 10, val = 0;
    if ( *stream == '0' ){
        *buff++ = *stream++;
        if ( *stream == 'x' || *stream == 'b' ){
            base = ( *stream == 'x' )?16:2;
            *buff++ = *stream++;
        } else {
            base = 8;
        }
    }
    while ( digitMap.find(*stream) != digitMap.end() ){//While there is a valid digit
        int digit = digitMap[*stream];
        if ( digit >= base ){
			errorInfo.push_back( ErrorInfo (\
			ErrorInfo::ErrorLocation::ERR_LEXER,\
		    line,insString(),\
			"Invalid digit for the given base" ) ); 
//            std::cerr << "Invalid digit for the given base!\n" << std::endl;
            break;
        }
        val = val * base + digitMap[*stream];
        *buff++ = *stream++;
    }
    int_val = val; 
    *buff = 0;
}

void Lexer::nextInstruction(char *buff){
    while ( !isToken(TOKEN_INSTRUCTION) && !isToken(TOKEN_END) ){
        next(buff);
    }
}
#define ADD_SIMPLE_TOKEN(k)\
                    do {\
                      kind = (k);\
                      *buff++ = *stream++;\
                      *buff = 0;\
                    }while( 0 )

void Lexer :: next (char *buff){
	std::string s;
    switch ( *stream ){
        case '#': // '#' is going to denote start of a line comment
            while ( *stream && *stream != '\n' ){
                stream++;
            }
            if ( *stream == '\n' ){
                line++;
                line_start = ++stream;
            }
            next(buff);
            break;
        case '\n': case '\t': case '\r': case ' ':
            while ( isspace( *stream ) ){
                if ( *stream == '\n' ){
                    kind = TOKEN_NEWLINE;
                    prev_line = line;
                    line++; 
                    previous = line_start;
                    line_start = stream + 1;
                }
                stream++;
            }
            if ( kind == TOKEN_NEWLINE ){
                *buff = '\n';
                *buff = 0;
            } else {
                next(buff);
            }
            break;
        case 'A': case 'B': case 'C': case 'D': case 'E': 
        case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': 
        case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': 
        case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': 
        case 'X': case 'Y': case 'Z': case 'a': case 'b': case 'c': 
        case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': 
        case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': 
        case 'p': case 'q': case 'r': case 's': case 't': case 'u': 
        case 'v': case 'w': case 'x': case 'y': case 'z': {
            start = stream;
            kind = TOKEN_NAME;
            char *str = buff;
            while ( isalnum(*stream) ){
                *buff = *stream;
                stream++;
                buff++;
            }
            *buff = 0;
            string s ( str );
            if ( ::keywordMap[s] ){
                kind = TOKEN_INSTRUCTION;
            }
            break;
        }
        case '(': case ')':
            kind = ( *stream == '(' )? TOKEN_LPAREN:TOKEN_RPAREN;
            *buff = *stream++;
            *buff = 0;
            break;
        case '$':
            kind = TOKEN_REGISTER;
            *buff++ = *stream++;
            while ( isalnum( *stream ) ){
                *buff++ = *stream++;
            }
            *buff = 0;
            break;
        case '0': case '1': case '2': case '3': case '4': 
        case '5': case '6': case '7': case '8': case '9':{
            start = stream;
            kind =   TOKEN_INT;
            scanInt(buff);
            break;
        }
        case ',': case ':':
            kind = ( *stream == ',' )?TOKEN_COMMA:TOKEN_COLON;
            *buff++ = *stream++;
            *buff = 0;
            break;
        case '+':
            ADD_SIMPLE_TOKEN(TOKEN_ADD);
            break;
        case '-':
            ADD_SIMPLE_TOKEN(TOKEN_SUB);
            break;
        case '*':
            ADD_SIMPLE_TOKEN(TOKEN_MUL);
            break;
        case '/':
            ADD_SIMPLE_TOKEN(TOKEN_DIV);
            break;
        case '&':
            ADD_SIMPLE_TOKEN(TOKEN_BAND);
            break;
        case '|':
            ADD_SIMPLE_TOKEN(TOKEN_BOR);
            break;
        case '^':
            ADD_SIMPLE_TOKEN(TOKEN_BXOR);
            break;
        case '!':
            ADD_SIMPLE_TOKEN(TOKEN_NOT);
            break;
        case '~':
            ADD_SIMPLE_TOKEN(TOKEN_COMPLEMENT);
            break;
        case '>':
            *buff++ = *stream++; 
            if ( *stream != '>' ){
				errorInfo.push_back (ErrorInfo (ErrorInfo::\
				ErrorLocation::ERR_LEXER,\
				line, insString(),\
				"Undefined token") );
                //std::cerr << "At line: " << currentPos().row << std::endl;
                //std::cerr << "Unidentified token \'" << *stream <<"\'"  << std::endl;
                while ( !isspace(*stream) ){ // ingnore all the non whitespace characters
                    stream++;
                }
                kind = TOKEN_BAD;
                return;
            }
            *buff++ = *stream++; *buff = 0;
            kind = TOKEN_RSHIFT;
            break;
        case '<':
            *buff++ = *stream++; 
            if ( *stream != '<' ){
                errorInfo.push_back (ErrorInfo (ErrorInfo::\
				ErrorLocation::ERR_LEXER,\
				line, insString(),\
				"Undefined token") );
				//std::cerr << "At line: " << currentPos().row << std::endl;
                //std::cerr << "Unidentified token \'" << *stream <<"\'"  << std::endl;
                while ( !isspace(*stream) ){ // ingnore all the non whitespace characters
                    stream++;
                }
                kind = TOKEN_BAD;
                return;
            } else {
                *buff++ = *stream++; *buff = 0;
                kind = TOKEN_LSHIFT;
            }
            break;
        case 0:
            kind = TOKEN_END;
            break;
        default:
			errorInfo.push_back (ErrorInfo (ErrorInfo::\
			ErrorLocation::ERR_LEXER,\
			line, insString(),\
			"Undefined token") );
            //std::cerr << "At line: " << currentPos().row << std::endl;
            //std::cerr << "Unidentified token \'" << *stream <<"\'"  << std::endl;
            kind = TOKEN_BAD;
            while ( !isspace(*stream) ){ // ingnore all the non whitespace characters
                stream++;
            }
            return;
            break;
    }
}

void Lexer :: previousIns(char *buff){
    stream = previous;
    line_start = previous;
    line = prev_line;
    next(buff);
}

#define TEST_EQ(x,y,msg)\
    do {\
        try {\
            if ( ( x ) != ( y ) ) throw msg;\
        } catch ( const char *m ){\
            std::cerr << msg << std::endl;\
        }\
    } while ( 0 )

void Lexer :: test (){
    const char str [] =  "addi $r0,$r1,123";
    char buffer[ 256 ];
    Lexer n( str );
    n.next(buffer);
    TEST_EQ(n.kind,TOKEN_INSTRUCTION,"Failed to find the type of instruction");
    n.next(buffer);
    TEST_EQ(n.kind,TOKEN_REGISTER,"Failed to lex register!");
    n.next(buffer);
    TEST_EQ( n.kind , TOKEN_COMMA, "Failed to lex COMMA (',')" );
    n.next(buffer);
    TEST_EQ(n.kind,TOKEN_REGISTER,"Failed to lex register!");
    n.next(buffer);
    TEST_EQ( n.kind , TOKEN_COMMA, "Failed to lex COMMA (',')" );
    n.next(buffer);
    TEST_EQ(n.kind,TOKEN_INT,"Failed to lex integer!");
    TEST_EQ(n.int_val,123,"Failed to get integer value!");
    string s("1234 0xa2b4 0b1111 012 0");
    const char *newStr = s.c_str();
    n.init( newStr );
    n.next(buffer);
    TEST_EQ(n.int_val,1234,"Failed to scan decimal integer");
    n.next(buffer);
    TEST_EQ(n.int_val,0xa2b4,"Failed to scan hexadecimal integer");
    n.next(buffer);
    TEST_EQ(n.int_val,15,"Failed to scan binary integer");
    n.next(buffer);
    TEST_EQ(n.int_val,10,"Failed to scan octal integer");
    n.next(buffer);
    TEST_EQ(n.int_val,0,"Failed to scan zero value!");
    s = "+ - >> << | & ~ / * !";
    n.init(s.c_str()); n.next(buffer);
    TEST_EQ(n.kind,TOKEN_ADD,"Failed to scan Add operator"); n.next(buffer);
    TEST_EQ(n.kind,TOKEN_SUB,"Failed to scan Subtract operator"); n.next(buffer);
    TEST_EQ(n.kind,TOKEN_RSHIFT,"Failed to scan rshift operator");n.next(buffer);
    TEST_EQ(n.kind,TOKEN_LSHIFT,"Failed to scan lshift operator");n.next(buffer);
    TEST_EQ(n.kind,TOKEN_BOR,"Failed to scan bor operator");n.next(buffer);
    TEST_EQ(n.kind,TOKEN_BAND,"Failed to scan band operator");n.next(buffer);
    TEST_EQ(n.kind,TOKEN_COMPLEMENT,"Failed to scan complement operator");n.next(buffer);
    TEST_EQ(n.kind,TOKEN_DIV,"Failed to scan division operator");n.next(buffer);
    TEST_EQ(n.kind,TOKEN_MUL,"Failed to scan multiply operator");n.next(buffer);
    TEST_EQ(n.kind,TOKEN_NOT,"Failed to scan not operator");n.next(buffer);
    s = "#1234591234\n 1234";
    n.init(s.c_str()); n.next(buffer);
    TEST_EQ(n.kind,TOKEN_INT,"Failed to scan comments");
}
#undef TEST_EQ
