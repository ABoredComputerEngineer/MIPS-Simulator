#include <cstdarg>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include "printBuffer.hpp"

void *xmalloc( size_t size ){
    void *x = malloc( sizeof(char) * size );
    if ( x ){
        return x;
    } else {
        perror("Malloc failed!");
        return NULL;
    }
}

void *xcalloc( size_t num ,size_t bytes ){
    void *x = calloc( num ,bytes );
    if ( x ){
        return x;
    } else {
        perror("Calloc Failed!");
        return NULL;
    }
}

void *xrealloc( void *prev, size_t newLen ){
    void *x = realloc( prev, newLen );
    if ( x ){
        return x;
    } else {
        perror("Realloc failed\n");
        return NULL;
    }
}

AppendBuffer::AppendBuffer () :buff(NULL), len(0), cap(0) {}

AppendBuffer::~AppendBuffer(){
    free( buff );
    len = 0; cap = 0;
}


AppendBuffer::AppendBuffer (size_t x) : len(0), cap(x){
    buff = (char * )xmalloc( sizeof(char) * x );
    assert( buff );
}

void AppendBuffer::alloc( ){
    len = 0; cap = DEFAULT_SIZE ;
    buff = ( char * )malloc( sizeof ( char ) * DEFAULT_SIZE);
}

char *AppendBuffer::append( const char *fmt , ... ){
    if ( !buff || !cap ){
        alloc( );
    }
    assert(buff);
    char *end = buff + len ; // position of the NULL terminator in the string
    va_list args;
    va_start( args, fmt );
    size_t printLen = vsnprintf(NULL,0,fmt,args);
    if ( len + printLen + 1 >= cap ){ 
         // Grow the buffer
        char *x = (char * )realloc(buff, 2 * ( len + printLen + 1 ) );
        assert( x );
        buff = x;
        cap = 2 * ( len + printLen  + 1 );
    }
    end = buff + len;
    assert( len + printLen < cap );
    size_t freeSpace = cap-len;
    va_start(args,fmt);
    printLen = vsnprintf(end,freeSpace,fmt,args);
    if ( printLen >= freeSpace ){
        std::cerr << "Unable to print buffer " << std::endl;
        return NULL;
    }
    len += printLen;
    va_end(args);
    return buff; 
}
#if 0
void bufferTest(){
    AppendBuffer p(1024*1024);
    p.append("%s","fuck this shit");
    p.append(" %d %d ",23,32);
    std::cout << p.buff << std::endl;
    return 0;
}
#endif
