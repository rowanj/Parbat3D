#ifndef _CONSOLE_H
#define _CONSOLE_H

#define USE_CONSOLE 1

#include <Windows.h>
#include <string>
#include <stdarg.h>

typedef unsigned int uint;

class Console
{
  public:

	#if USE_CONSOLE
    static void open();
    static void close();
    static void write(const char *msg_format, ...);
    static void write(std::string *msg);
    static void write(int msg);
    static void writeRECT(RECT *rect); 
    static void waitForEnter();
    #else
    static void open() {return;}
    static void close() {return;}
    static void write(const char *msg_format, ...) {return;}
    static void write(std::string *msg) {return;}
    static void write(int msg) {return;}
    static void writeRECT(uint *rect) {return;}
    static void waitForEnter() {return;}
    #endif
    
};


#endif
