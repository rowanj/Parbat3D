#ifndef _PARBAT_MAINWINDOW_H
#define _PARBAT_MAINWINDOW_H

#include "Window.h"
#include <vector>

using namespace std;

class MainWindow:public Window
{
    private:
    WNDPROC prevProc;
    //vector<Window*> windows;
    vector<HWND> savedWindows;
    vector<int> restoreStates;
    int *windowStates; 
    
    static BOOL CALLBACK MainWindow::SaveAndHideWindow(HWND hwnd, LPARAM lparam);
       
    public:
    virtual int Create();        
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
    void RestoreAll();      /* restore all windows back to their original state */
    void MinimizeAll();     /* hide all windows owned by the current thread */
    void DestroyAll();		/* destroy all windows owned by current thread */
};


#endif
