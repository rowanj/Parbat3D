#ifndef _PARBAT_DISPLAYWINDOW_H
#define _PARBAT_DISPLAYWINDOW_H

#include "Window.h"
class DisplayWindow: public Window
{
    private:
    WNDPROC prevProc;
    HBRUSH hbrush;
    HFONT hNormalFont;    
    SIZE textSize;
    POINT textPos;
    char *text;
    int textLen;    
    
    public:
    DisplayWindow();
    virtual int Create(HINSTANCE hInstance,HWND parent);        
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);};

#endif
