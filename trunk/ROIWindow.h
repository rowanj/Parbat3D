#ifndef _PARBAT_ROIWINDOW_H
#define _PARBAT_ROIWINDOW_H

#include "Window.h"

class ROIWindow:public Window
{
    private:
    WNDPROC prevProc;
    HWND roiBG;
    HWND roiTick;
    HWND roiGroup;
    HWND roiToolBar;
    
        
    public:
    //DisplayWindow roiWindowDisplay;

    int Create(HWND parent);
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
    //void updateImageWindowTitle();
    //void updateImageScrollbar();
    //void scrollImageX(int scrollMsg);
    //void scrollImageY(int scrollMsg);    
    //void zoomImage(int nlevels);
};

#endif
