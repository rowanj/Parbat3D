#ifndef _INI_FILE_H
#define _INI_FILE_H

#include <fstream>

using namespace std;

class iniFile {
        bool gotFileName;
        string fileName;
        
        string getKeyFromLine (string);
        string getDataFromLine (string);
        
    public:
        void open (string);
        void close ();

        bool parse (string);        
        string read (string);
        void update (string, string);
        
        string getFileName () { return fileName; }
};

#endif
