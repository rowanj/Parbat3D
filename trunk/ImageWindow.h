#ifndef _PARBAT_IMAGEWINDOW_H
#define _PARBAT_IMAGEWINDOW_H

#include "Window.h"

class ImageWindow:public Window
{
    private:
    WNDPROC prevProc;
        
    public:
    DisplayWindow imageWindowDisplay;

    int Create(HWND parent);
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
    void updateImageWindowTitle();
    void updateImageScrollbar();
    void scrollImageX(int scrollMsg);
    void scrollImageY(int scrollMsg);    
    void zoomImage(int nlevels);
};

#endif
