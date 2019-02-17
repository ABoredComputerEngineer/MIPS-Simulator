#ifndef PRINT_BUFFER_HPP

#define PRINT_BUFFER_HPP

struct AppendBuffer {
    enum { DEFAULT_SIZE = 1024 };
    char *buff;
    size_t len;
    size_t cap;

    AppendBuffer (size_t buffSize);

    AppendBuffer ();

    ~AppendBuffer();

    void alloc();
    char *append(const char *fmt, ... );
    inline char *getBuff() { return buff; }
    inline void clearBuff(){ len = 0; }
};

#endif