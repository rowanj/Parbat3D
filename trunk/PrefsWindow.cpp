#include <Windows.h>
#include "main.h"
#include "Window.h"
#include "PrefsWindow.h"

/* ------------------------------------------------------------------------------------------------------------------------ */
/* Prefs Window Functions */


/* create main window */
int PrefsWindow::Create(HWND parent)
{
    RECT rect;
    int mx,my;
    const int PREFS_WINDOW_WIDTH=250;
    const int PREFS_WINDOW_HEIGHT=300;

    /* Get Main Window Location for ROI window alignment*/
    GetWindowRect(overviewWindow.GetHandle(),&rect);

    /* create ROI window */
    if (!CreateWin(0, "Parbat3D Prefs Window", "Preferences",
	     WS_POPUP+WS_SYSMENU+WS_CAPTION,
	     rect.left, rect.bottom+30, 250, 300, parent, NULL))
	    return false;

    prevProc=SetWindowProcedure(&WindowProcedure);	

	return true;
}

/* handle events related to the main window */
LRESULT CALLBACK PrefsWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{   
    static POINT moveMouseOffset;     /* mouse offset relative to window, used for snapping */
    static RECT rect;                 /* for general use */

    PrefsWindow* win=(PrefsWindow*)Window::GetWindowObject(hwnd);
    
    switch (message)                  /* handle the messages */
    {
        /* WM_NCLBUTTONDOWN: mouse button was pressed down in a non client area of the window */        
        case WM_NCLBUTTONDOWN:

            /* let windows handle this event */
            return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    

        case WM_SHOWWINDOW:
            /* update window menu item depending on whether window is shown or hidden */
            if (wParam)
                CheckMenuItem(overviewWindow.hMainMenu,IDM_PREFSWINDOW,MF_CHECKED|MF_BYCOMMAND);            
            else
                CheckMenuItem(overviewWindow.hMainMenu,IDM_PREFSWINDOW,MF_UNCHECKED|MF_BYCOMMAND);
            return 0;

        /* WM_CLOSE: system or user has requested to close the window/application */             
        case WM_CLOSE:
            /* don't destroy this window, but make it invisible */
            ShowWindow(hwnd,SW_HIDE);
            return 0;

        /* WM_DESTORY: system is destroying our window */
        case WM_DESTROY:
            CheckMenuItem(overviewWindow.hMainMenu,IDM_PREFSWINDOW,MF_UNCHECKED|MF_BYCOMMAND);            
            return 0;
            
        default:
            /* let windows handle any unknown messages */
            return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
    }
    /* return 0 to indicate that we have processed the message */       
    return 0;  
}
