
#include <Windows.h>
#include <string>
#include <stdlib.h>
//#include <stringstream>

#include "console.h"
#include "config.h"

using namespace std;

void Console::open()
{
    #if TMP_USE_CONSOLE           
    AllocConsole();
    //hstdout=GetStdHandle(STD_OUTPUT_HANDLE);        
    #endif
}
    
void Console::close()
{
    #if TMP_USE_CONSOLE         
    FreeConsole();
    #endif
}
    
void Console::write(char *msg)
{
    #if TMP_USE_CONSOLE        
    int len,written;
    len=strlen(msg);
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),msg,len,(DWORD*)&written,NULL);           
    #endif
}

void Console::write(string *msg)
{
    #if TMP_USE_CONSOLE        
    int len,written;
    len=msg->length();
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),msg->c_str(),len,(DWORD*)&written,NULL);           
    #endif
}

void Console::write(int msg)
{
    #if TMP_USE_CONSOLE        
    int written;
    char message[33];
    itoa(msg,message,10);
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),message,strlen(message),(DWORD*)&written,NULL);
    #endif
}
