#ifndef _PARBAT_CONTSWINDOW_H
#define _PARBAT_CONTSWINDOW_H

#include "Window.h"

class ContrastWindow:public Window
{
    private:
    WNDPROC prevProc;
    HWND hBrightnessTrackbar;
	HWND hContrastTrackbar;
	HWND hCSOKButton;
	HWND hCSCancelButton;
	
	HWND hPreview;
	
    
    public:
    HWND hPerChannel;
    
    int Create(HWND parent);
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
};

#endif
