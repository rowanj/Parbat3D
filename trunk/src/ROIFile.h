#ifndef _ROI_FILE_H
#define _ROI_FILE_H

#include "BatchIniFile.h"
#include "ROI.h"
#include "ROISet.h"
#include "StringUtils.h"

class ROIFile {
    private:
        BatchIniFile openFile;  // reference to the ini file, all file operations go through this
        
    public:
        ROIFile ();
        
        ~ROIFile ();
        
        
        void open (string);
        
        void close ();
        
        
        void saveSetToFile (string, ROISet*, bool);
        
        void saveSetToFile (ROISet*, bool);
        
        
        ROISet* loadSetFromFile (string);
        
        ROISet* loadSetFromFile ();
        
        
        void saveRegionToFile (string, ROI*);
        
        void saveRegionToFile (ROI*);
        
        
        ROI* loadRegionFromFile (string, string);
        
        ROI* loadRegionFromFile (string);
};

#endif
