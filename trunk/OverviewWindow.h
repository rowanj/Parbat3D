#ifndef _PARBAT_OVERVIEWWINDOW_H
#define _PARBAT_OVERVIEWWINDOW_H

#include "Window.h"
#include "DisplayWindow.h"

class OverviewWindow:public Window
{
    private:
        WNDPROC prevProc;
        
        //bool moving_viewport;
        //int prev_viewport_x, prev_viewport_y;
        
    public:
        HMENU hMainMenu;
        DisplayWindow overviewWindowDisplay;
        static const int WIDTH=250;  // width of the overview window in pixels
        static const int HEIGHT=296;
        
        virtual int Create(HWND);        
        static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
        int toggleMenuItemTick(HMENU hMenu,int itemId);
        
        
        int getWidth ();
        int getHeight ();
};

#endif
