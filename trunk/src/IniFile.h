#ifndef _INI_FILE_H
#define _INI_FILE_H

#include <fstream>

using namespace std;

/**
    Handles access to ini files. Once a file name has been specified in the
    constructor, you can parse (search) the file for values, read values or
    update values. The update function can also be used to add values to the
    file.
*/
class iniFile {
    protected:
        bool gotFileName;  // true if a filename has been specified
        string fileName;   // stores the filename
        
        /**
            Returns the key part of a line in the file.
        */
        string getKeyFromLine (string);
        
        /**
            Returns the data (value) part of a line in the file.
        */
        string getDataFromLine (string);
        
        
        /**
            Returns the line number of where the section starts in the file.
        */
        int sectionStartsAt (string section);
        
        
    public:
        /**
            Stores the filename to be used in future calls.
        */
        void open (string);
        
        /**
            Deletes the filename so file calls cannot complete.
        */
        void close ();
        
        /**
            Returns the name of the file that is being used.
        */
        string getFileName ();
        
        /**
            Searches to find if the key provided as a parameter exists in the
            file. If the key is in the file then true is returned, otherwise it
            is false.
        */
        bool parse (string, string);
        
        /**
            Returns the value of the specified key in the file. If the key does
            not exist, then an empty string is returned.
        */
        string read (string, string);
        
        /**
            Updates the key in the file with the specified value. If the key
            does not exist, then it is added to the file with the value.
        */
        void update (string, string, string);
        
        
        /**
            Returns true if the section exists in the file.
        */
        bool sectionExists (string);
        
        
        /**
            Removes the entire section (and all its data) from the file.
        */
        void removeSection (string);
        
        /**
            Removes the section data from the file but lets the user specify if
            the section title is kept. If the second argument is true then the
            section title is kept, but if it is false it is removed. This
            allows the position of the section to be kept in the file if all
            the data is being replaced.
        */
        void removeSection (string, bool);
        
        
        /**
            Empties the file.
        */
        void clearFileContents ();
};

#endif
