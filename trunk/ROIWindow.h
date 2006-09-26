#ifndef _PARBAT_ROIWINDOW_H
#define _PARBAT_ROIWINDOW_H

#include <vector>
#include "Window.h"
#include <Commctrl.h>

class ROIWindow:public Window
{
    private:
        WNDPROC prevProc;
        static WNDPROC prevListViewProc;
        HWND hBG;
        HWND *hROITick;
        HWND roiToolBar;
        HWND hROIListBox;
        
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
        
        HWND hwndTT;
                
        static vector<HWND> roiColourButtonList;
        
        static COLORREF customColours[];
        
        static string *editingROIName;
        
        /**
            Returns the number of ROI checkboxes that have been checked.
        */
        int getROICheckedCount ();
        
        
        /**
            Returns the index of the selected item in the list. Returns -1 if
            nothing is selected.
        */
        int getSelectedItemIndex ();
        
        /**
            Returns the text of the specified item in the ROI list.
        */
        string getItemText (int i);
        
        /**
            Returns the text of the selected item in the list. The text is the
            name of the ROI that the list item represents. Returns a blank
            string ("") if nothing is selected.
        */
        string getSelectedItemText ();
        
        
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
        
        
        /**
            Removes an ROI from the set. Removes the ROI from the set, the item
            from the list and the ROIs colour picker. Finally, moves all the
            other colour pickers up one position.
        */
        void deleteROI (ROIWindow*);
               
        void updateROIList (ROIWindow*);
        
        
        static LRESULT CALLBACK ROIListViewProcedure(HWND, UINT, WPARAM, LPARAM);
        static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
        void OnListItemChanged(NMLISTVIEW* pnml);        
        
        static void ChangeROIColour(HWND hbutton);

        static void DrawColourButton(LPDRAWITEMSTRUCT di);
        
        void addNewRoiToList(ROI *rCur,int newId);
        
    public:
        int Create(HWND parent);
        
        
        void updateButtons (ROIWindow*);
};

#endif
