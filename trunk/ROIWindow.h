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
                
        static vector<HWND> roiCheckboxList;
        
        int getROICheckedCount ();
        
        void loadROI (ROIWindow*);
        
        void saveROI (ROIWindow*);
        
        void createNewROI (ROIWindow*, string);
        
        void deleteROI (ROIWindow*);
        
        
    public:
        int Create(HWND parent);
        static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
};

#endif
