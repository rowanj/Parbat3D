#ifndef _PARBAT_CONTSWINDOW_H
#define _PARBAT_CONTSWINDOW_H

#include "Window.h"

class ContrastWindow:public Window
{
    private:
    WNDPROC prevProc;
    
	HWND hCSOKButton;
	HWND hCSCancelButton;
	HWND hPreview;
	
    int brightnessCancel;
    int contrastCancel;
   	
    public:
    HWND hPerChannel;
    HWND hBrightnessTrackbar;
	HWND hContrastTrackbar;

    int Create(HWND parent);
    void resetSliders();
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
};

#endif
