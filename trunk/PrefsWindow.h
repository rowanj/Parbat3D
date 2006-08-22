#ifndef _PARBAT_PREFSWINDOW_H
#define _PARBAT_PREFSWINDOW_H

#include "Window.h"

class PrefsWindow:public Window
{
    private:
    WNDPROC prevProc;
        
    public:
    //DisplayWindow prefsWindowDisplay;

    int Create(HWND parent);
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
};

#endif
