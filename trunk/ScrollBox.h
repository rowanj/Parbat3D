#ifndef _PARBAT_SCROLLABLEWINDOW_H
#define _PARBAT_SCROLLABLEWINDOW_H

#include "Window.h"

class ScrollBox:public Window
{
    private:
    WNDPROC prevProc;
    protected:
    void UpdateScrollBar();
    public:
    int Create(RECT *rect,HWND parentHandle);
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
};


#endif

