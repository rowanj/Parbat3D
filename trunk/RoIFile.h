#ifndef _ROI_FILE_H
#define _ROI_FILE_H

#include <vector>
#include <string>
#include <sstream>

#include "BatchIniFile.h"
#include "ROI.h"
#include "ROISet.h"
#include "StringUtils.h"

using namespace std;


class ROIFile {
    private:
        BatchIniFile openFile;  // reference to the ini file, all file operations go through this
        
    public:
        ROIFile ();
        
        ~ROIFile ();
        
        
        void open (string);
        
        void close ();
        
        
        void saveSetToFile (string, ROISet*);
        
        void saveSetToFile (ROISet*);
        
        
        ROISet* loadSetFromFile (string);
        
        ROISet* loadSetFromFile ();
        
        
        void saveRegionToFile (string, ROI*);
        
        void saveRegionToFile (ROI*);
        
        
        ROI* loadRegionFromFile (string, string);
        
        ROI* loadRegionFromFile (string);
};

#endif
