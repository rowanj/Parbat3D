#include <Windows.h>
#include <string>

using namespace std;

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
};
