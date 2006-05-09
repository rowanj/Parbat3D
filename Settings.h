#ifndef _SETTINGS_H
#define _SETTINGS_H

#include "IniFile.h"

using namespace std;

class settings {
        iniFile openFile;
        
    public:
        settings (string filename) { openFile.open(filename); }
        ~settings () { openFile.close(); }
        
        void setSetting (string key, string data);
        string getSetting (string key);
};

#endif
