#ifndef _PARBAT_ROIWINDOW_H
#define _PARBAT_ROIWINDOW_H

#include "Window.h"

class ROIWindow:public Window
{
    private:
    WNDPROC prevProc;
    HWND hBG;
    HWND hROIScrollbox;
    HWND *hROITick;
    HWND roiToolBar;
    
    HWND hOpenButton;
    HICON hOpenIcon;
    HWND hSaveButton;
    HICON hSaveIcon;
    HWND hPolyButton;
    HICON hPolyIcon;
    HWND hRectButton;
    HICON hRectIcon;
    HWND hTrashButton;
    HICON hTrashIcon;
        
    public:
    

    int Create(HWND parent);
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

};

#endif
