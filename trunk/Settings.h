#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <sstream>

#include "IniFile.h"

using namespace std;

/**
    Allows settings to be saved to and retrieved from an ini file. Access to the
    file is handled by IniFile.cpp and its functions.
*/
class settings {
        iniFile openFile;  // reference to the ini file, all file operations go through this
        
    public:
        /**
            Creates a new settings object that can save settings to a file.
        */
        settings () {}
        
        /**
            Ends access to the file.
        */
        ~settings () { openFile.close(); }
        
        
        /**
            Sets up access to the file using the filename given as a parameter.
        */
        void open (string filename) { openFile.open(filename); }
        
        
        /**
            Returns a string containing the value of the key. If the key does
            not exist in the file then an empty string is returned.
        */
        string getSetting (string, string);
        
        /**
            Adds the setting to the file.
        */
        void setSetting (string, string, string);
        
        /**
            Adds the setting to the file. Since the file can only be updated
            using text, the integer is first converted into a string using
            stringstream before updating the file.
        */
        void setSetting (string, string, int);
};

#endif
