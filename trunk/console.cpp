
#include <Windows.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
//#include <stringstream>

#include "console.h"
#include "config.h"

using namespace std;

// display console window
void Console::open()
{
	//COORD myBufferSize()
    #if TMP_USE_CONSOLE           
    AllocConsole();
    COORD mySize;
    mySize.X = 80;
    mySize.Y = 10000;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), mySize);
    //hstdout=GetStdHandle(STD_OUTPUT_HANDLE);        
    #endif
}

// hide console window    
void Console::close()
{
    #if TMP_USE_CONSOLE         
    FreeConsole();
    #endif
}

// write string to console window
void Console::write(const char *msg_format, ...)
{
    #if TMP_USE_CONSOLE        
	char* text = new char[512];
	va_list	ap;
	
	assert(msg_format != NULL);
	
	va_start(ap, msg_format);
    vsprintf(text, msg_format, ap);
	va_end(ap);


    int len,written;
    len=strlen(text);
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),text,len,(DWORD*)&written,NULL);           
    #endif
}

// write string to console window
void Console::write(string *msg)
{
    #if TMP_USE_CONSOLE        
    int len,written;
    len=msg->length();
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),msg->c_str(),len,(DWORD*)&written,NULL);           
    #endif
}

// write a RECT structure to console window
void Console::writeRECT(uint *rect)
{
    #if TMP_USE_CONSOLE        
    Console::write("top: ");
    Console::write(rect[0]);
    Console::write(" left: ");
    Console::write(rect[1]);
    Console::write(" right: ");
    Console::write(rect[2]);
    Console::write(" bottom: ");
    Console::write(rect[3]);
    #endif
}

// write integer to console window
void Console::write(int msg)
{
    #if TMP_USE_CONSOLE        
    int written;
    char message[33];
    itoa(msg,message,10);
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),message,strlen(message),(DWORD*)&written,NULL);
    #endif
}

// wait for user to press a enter in the console window
void Console::waitForEnter()
{
	#if TMP_USE_CONSOLE
	char buffer[1];
	DWORD bytesRead;
	ReadConsole(GetStdHandle(STD_INPUT_HANDLE),buffer,1,&bytesRead,0);
	#endif
}

