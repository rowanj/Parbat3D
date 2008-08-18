#ifndef _PARBAT_MAINWINDOW_H
#define _PARBAT_MAINWINDOW_H

#include "Window.h"

class MainWindow:public Window
{
    private:
    WNDPROC prevProc;
    std::vector<HWND> savedWindows;
    std::vector<int> restoreStates;
    int *windowStates; 
    
    static BOOL CALLBACK SaveAndHideWindow(HWND hwnd, LPARAM lparam);
	static BOOL CALLBACK EnableWindowCallback(HWND hwnd,LPARAM lparam);
	static BOOL CALLBACK DisableWindowCallback(HWND hwnd,LPARAM lparam);
       
    public:
    virtual int Create();        
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
    void RestoreAll();      /* restore all windows back to their original state */
    void MinimizeAll();     /* hide all windows owned by the current thread */
    void DestroyAll();		/* destroy all windows owned by current thread */
    void DisableAll();		/* disable all windows owned by main thread */
    void DisableAll(HWND);	/* disable all windows except for a particular window */
    void EnableAll();		/* enable all windows owned by main thread */
};


#endif
