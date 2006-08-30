#ifndef _PARBAT_ROIWINDOW_H
#define _PARBAT_ROIWINDOW_H

#include <vector>

#include "Window.h"

class ROIWindow:public Window
{
    private:
        WNDPROC prevProc;
        HWND hBG;
        HWND *hROITick;
        HWND roiToolBar;
        
        HWND hOpenButton;
        HICON hOpenIcon;
        HWND hSaveButton;
        HICON hSaveIcon;
        
        HWND hNewButton;
        HICON hNewIcon;
        HWND hDeleteButton;
        HICON hDeleteIcon;
        
        HWND hPolyButton;
        HICON hPolyIcon;
        HWND hRectButton;
        HICON hRectIcon;
        HWND hSingleButton;
        HICON hSingleIcon;
                
        static vector<HWND> roiCheckboxList;
        
        
        /**
            Returns the number of ROI checkboxes that have been checked.
        */
        int getROICheckedCount ();
        
        /**
            Creates a new Region of Interest and, if specified, creates a new
            entity inside the new region. The second parameter accepts a string
            specifying the type of entity to add to the region. If the second
            parameter is left blank then no new entity is added.
        */
        void newROI (ROIWindow*, const char*);
        
        /**
            Contains the procedure for loading a set of Regions of Interest
            from a file. This includes opening a dialog box, getting the
            filename and loading the set from the file.
        */
        void loadROI (ROIWindow*);
        
        /**
            Contains the procedure for saving a set of Regions of Interest to a
            file. This includes opening a dialog box, getting the filename and
            saving the set to the file.
        */
        void saveROI (ROIWindow*);
        
        /**
            Contains the procedure for adding a new entity to the set of Regions
            of Interest. If no region is selected in the list then a new region
            is created, with the entity inside it. The second parameter
            specifies the type of entity to create.
        */
        void newEntity (ROIWindow*, const char*);
        
        void deleteROI (ROIWindow*);
        
        
        void updateButtons (ROIWindow*);
        
        
    public:
        int Create(HWND parent);
        
        static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
};

#endif
