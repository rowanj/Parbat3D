#ifndef _PARBAT_IMAGEWINDOW_H
#define _PARBAT_IMAGEWINDOW_H

#include "Window.h"
#include "DisplayWindow.h"

extern char* imageWindowTitle;

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
        void scrollImage(bool vert, int amount);
        
        void zoomImage(float nlevels);
    	void onKeyDown(int virtualKey);
    	
        
        int getWidth ();
        int getHeight ();
};

#endif
