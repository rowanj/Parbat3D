#ifndef _SETTINGS_H
#define _SETTINGS_H

#include "IniFile.h"

using namespace std;

class settings {
        iniFile openFile;
        
    public:
        settings (string fn) { openFile.open(fn); }
        ~settings () { openFile.close(); }
        
        void setSetting (string, string);
        string getSetting (string);
};

#endif
