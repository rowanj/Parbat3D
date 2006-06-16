#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <sstream>

#include "IniFile.h"

using namespace std;

class settings {
        iniFile openFile;
        
    public:
        settings (string filename) { openFile.open(filename); }
        ~settings () { openFile.close(); }
        
        string getSetting (string);
        void setSetting (string, string);
        void setSetting (string, int);
};

#endif
