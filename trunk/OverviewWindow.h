#ifndef _PARBAT_OVERVIEWWINDOW_H
#define _PARBAT_OVERVIEWWINDOW_H

#include "Window.h"
#include "DisplayWindow.h"

class OverviewWindow:public Window
{
    private:
    WNDPROC prevProc;
        
    public:
    HMENU hMainMenu;
    DisplayWindow overviewWindowDisplay;
    static const int OVERVIEW_WINDOW_WIDTH=250;    /* width of the overview window in pixels */

    virtual int Create(HWND);        
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
    int toggleMenuItemTick(HMENU hMenu,int itemId);  
};

#endif
