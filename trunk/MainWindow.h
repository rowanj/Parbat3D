#ifndef _PARBAT_MAINWINDOW_H
#define _PARBAT_MAINWINDOW_H

#include "Window.h"
#include <vector>

using namespace std;

class MainWindow:public Window
{
    private:
    WNDPROC prevProc;
    vector<Window*> windows;
    int *windowStates;    
    public:
    virtual int Create();        
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
};


#endif
