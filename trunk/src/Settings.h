#ifndef _SETTINGS_H
#define _SETTINGS_H

#include "IniFile.h"

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
        settings (string s);
        
        /**
            Ends access to the file.
        */
        virtual ~settings ();
        
        
        /**
            Sets up access to the file using the filename given as a parameter.
        */
        void open (string filename);
        
        
        /**
            Returns a string containing the value of the key. If the key does
            not exist in the file then an empty string is returned.
        */
        string getSetting (string section, string key);
        
        /**
            Returns a string containing the value of the key. If the key does
            not exist in the file then the third parameter is set as the new
            value in the file. In this case the third parameter is also
            returned.
        */
        string getSetting (string section, string key, string defaultSetting);
        
        
        int getSettingi(string section, string key);
        
        int getSettingi (string section, string key, int defaultSetting);
        
        
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
extern settings* settingsFile;


#endif
