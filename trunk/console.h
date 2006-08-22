#ifndef _CONSOLE_H
#define _CONSOLE_H
#include <Windows.h>
#include <string>

using namespace std;

typedef unsigned int uint;

class Console
{
    private:
    //static HANDLE hstdout;
    public:

    static void open();
    static void close();
    static void write(char *msg);
    static void write(string *msg);
    static void write(int msg);
    static void writeRECT(uint *rect);
};


#endif
