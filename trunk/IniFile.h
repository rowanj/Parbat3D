#ifndef _INI_FILE_H
#define _INI_FILE_H

#include <fstream>

using namespace std;

/**
    Handles access to ini files. Once a file name has been specified in the constructor,
    you can parse (search) the file for values, read values or update values. The update
    function can also be used to add values to the file.
*/
class iniFile {
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
            Returns the line number of where the section starts in the file
        */
        int sectionStartsAt (string section);
        
        
    public:
        /**
            Stores the filename to be used in future calls.
            This was hopefully going to open the file once so it didn't have to be opened
            again in each of the other calls but I could not get it to work. - Matt
        */
        void open (string);
        
        /**
            Deletes the filename so file calls cannot complete.
            If open() worked correctly, this call would have closed the file as well as
            deleting the filename. - Matt
        */
        void close ();


        /**
            Searches to find if the key provided as a parameter exists in the file. If the
            key is in the file then true is returned, otherwise it is false.
        */
        bool parse (string, string);
        
        /**
            Returns the value of the specified key in the file. If the key does not exist,
            then an empty string is returned.
            If open() worked correctly, if read occured after parse then the file would not
            have to be searched again. - Matt
        */
        string read (string, string);
        
        /**
            Updates the key in the file with the specified value. If the key does not exist,
            then it is added to the file with the value.
        */
        void update (string, string, string);
        
        
        /**
            Returns the name of the file that is being used.
        */
        string getFileName () { return fileName; }
};

#endif
