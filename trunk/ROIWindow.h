#ifndef _PARBAT_ROIWINDOW_H
#define _PARBAT_ROIWINDOW_H

#include "Window.h"

class ROIWindow:public Window
{
    private:
    WNDPROC prevProc;
    HWND roiScrollbox;
    HWND *roiTick;
    HWND roiToolBar;
    
        
    public:
    //DisplayWindow roiWindowDisplay;

    int Create(HWND parent);
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

};

#endif
