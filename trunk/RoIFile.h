#ifndef _ROI_FILE_H
#define _ROI_FILE_H

#include <vector>
#include <string>

#include "IniFile.h"
#include "RoI.h"

using namespace std;


class RoIFile {
    private:
        iniFile openFile;  // reference to the ini file, all file operations go through this
        
    public:
        RoIFile ();
        
        virtual ~RoIFile ();
        
        
        void open (string);
        
        void close ();
        
        
        void saveSetToFile (string, RoISet*);
        
        void saveSetToFile (RoISet*);
        
        
        RoISet* loadSetFromFile (string);
        
        RoISet* loadSetFromFile ();
        
        
        void saveRegionToFile (string, RoI*);
        
        void saveRegionToFile (RoI*);
        
        
        RoI* loadRegionFromFile (string, string);
        
        RoI* loadRegionFromFile (string);
};

#endif