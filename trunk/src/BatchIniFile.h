#ifndef _BATCH_INI_FILE_H
#define _BATCH_INI_FILE_H

#include "IniFile.h"
#include "StringUtils.h"

/**
    Extends the iniFile class to allow for data to be sent to or read from the
    file in blocks (batch requests). This reduces the amount of file accesses
    and speeds up the actions.
*/
class BatchIniFile: public iniFile {
    private:
        string *contentBuffer;  // stores content between reads and writes
        
    public:
        /**
            Returns the contents of the buffer.
        */
        string getBuffer ();
        
        
        /**
            Clears the entire contents of the buffer. This must be called before
            anything is added to the buffer the first time.
        */
        void clearBuffer ();
        
        
        /**
            Adds data to the buffer with a specified key. The first parameter is
            the key and the second is the data to be associated with the key.
        */
        void updateBuffer (string, string);
        
        
        /**
            Writes the buffer to a specified section, removing all previous
            data in the section.
        */
        void writeBufferToSection (string);
        
        /**
            Writes the buffer to a specified section but allows the previous
            data to be kept. If the second parameter is true then the previous
            data is kept, if it is false then it is removed.
        */
        void writeBufferToSection (string, bool);
        
        
        /**
            Reads a section from the open file and stores it in the buffer.
            Returns the buffer that contains the section content.
        */
        string readSectionContent (string);
        
        
        /**
            Returns a value from the buffer. The value is chosen by the key
            that is specified in the parameter list.
        */
        string readFromBuffer (string);
};

#endif
