#ifndef _INI_FILE_H
#define _INI_FILE_H

#include <fstream>

using namespace std;

class iniFile {
        bool fileIsOpen;
        fstream filePtr;
        string fileName;
        
    public:
        void open (string);
        void close (string);

        bool parse (string);        
        string read (string);
        void update (string, string);
        
        string getFileName () { return fileName; }
};

#endif
