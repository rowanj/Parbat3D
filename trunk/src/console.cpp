#include "PchApp.h"

#include "console.h"

#if USE_CONSOLE


using namespace std;

char* console_buffer;
const int console_buffer_length = 512;

// display console window
void Console::open()
{
    AllocConsole();
    COORD mySize;
    mySize.X = 80;
    mySize.Y = 10000;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), mySize);
    console_buffer = new char[512];
}

// hide console window    
void Console::close()
{
    FreeConsole();
    delete[] console_buffer;
}

// write string to console window
void Console::write(const char *msg_format, ...)
{
	va_list	ap;
	
	assert(msg_format != NULL);
	
	va_start(ap, msg_format);
    vsnprintf(console_buffer, console_buffer_length, msg_format, ap);
	va_end(ap);


	int written;
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),console_buffer,strlen(console_buffer),(DWORD*)&written,NULL);
}

// write string to console window
void Console::write(string *msg)
{
    int len,written;
    len=msg->length();
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),msg->c_str(),len,(DWORD*)&written,NULL);           
}

// write a RECT structure to console window
void Console::writeRECT(RECT *rect)
{
    Console::write("top: %d, left: %d, right: %d, bottom: %d\n", rect->top, rect->left, rect->right, rect->bottom);
}

// write integer to console window
void Console::write(int msg)
{
    int written;
    itoa(msg,console_buffer,10);
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),console_buffer,strlen(console_buffer),(DWORD*)&written,NULL);
}

// wait for user to press a enter in the console window
void Console::waitForEnter()
{
	DWORD bytesRead;
	ReadConsole(GetStdHandle(STD_INPUT_HANDLE),console_buffer,1,&bytesRead,0);
}

#endif // USE_CONSOLE
