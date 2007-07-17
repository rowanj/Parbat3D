#ifndef _PARBAT_PREFSWINDOW_H
#define _PARBAT_PREFSWINDOW_H

#include "Window.h"

class PrefsWindow:public Window
{
    private:
    WNDPROC prevProc;
    HWND cacheLabel;
    HWND cacheEntry;
    HWND texSizeLabel;
    HWND texSizeEntry;
    HWND displayCloseConfirmLabel;
    HWND displayCloseConfirmCheckbox;
    HWND okButton;
    HWND cancelButton;
        
    public:

    int Create(HWND parent);
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
};

#endif
