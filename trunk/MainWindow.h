#ifndef _PARBAT_MAINWINDOW_H
#define _PARBAT_MAINWINDOW_H

#include "Window.h"

class MainWindow:public Window
{
    private:
    WNDPROC prevProc;
    public:
    virtual int Create();        
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
};


#endif
