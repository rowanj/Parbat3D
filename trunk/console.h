#ifndef _CONSOLE_H
#define _CONSOLE_H
#include <Windows.h>
#include <string>
#include <stdarg.h>

using namespace std;

typedef unsigned int uint;

class Console
{
    private:
    //static HANDLE hstdout;
    public:

    static void open();
    static void close();
    static void write(const char *msg_format, ...);
    static void write(string *msg);
    static void write(int msg);
    static void writeRECT(uint *rect);
    static void waitForEnter();
};


#endif
