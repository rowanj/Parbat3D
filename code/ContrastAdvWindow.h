#ifndef _PARBAT_CONTSADVWINDOW_H
#define _PARBAT_CONTSADVWINDOW_H

#include "Window.h"

class ContrastAdvWindow:public Window
{
    private:
    WNDPROC prevProc;
    
    HWND hRedBrightnessTrackbar;
	HWND hRedContrastTrackbar;
	HWND hGreenBrightnessTrackbar;
	HWND hGreenContrastTrackbar;
	HWND hBlueBrightnessTrackbar;
	HWND hBlueContrastTrackbar;
	
	HWND hAdvCSOKButton;
	HWND hAdvCSCancelButton;
	
	HWND hPreviewAdv;
	
    
    public:
    HWND hPerChannelAdv;

    int Create(HWND parent);
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
};

#endif
