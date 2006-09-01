#define WINVER 0x0500
#include <Windows.h>
#include "main.h"
#include "ImageWindow.h"
#include "OverviewWindow.h"
#include "ToolWindow.h"
#include "ROIWindow.h"
#include "Window.h"
#include "MainWindow.h"

/* ------------------------------------------------------------------------------------------------------------------------ */
/* Main Window Functions */


/* create main window */
int MainWindow::Create()
{
    // Get height of screen
    RECT rect;
    HWND hDesktop=GetDesktopWindow();
    GetWindowRect(hDesktop,&rect);
    
    // Create main window positioned off the screen
    if (!CreateWin(WS_EX_NOACTIVATE, "parbat3d main win class", "Parbat3D",
		0, -150, rect.bottom+50, 1, 1,  //|WS_SYSMENU|WS_MINIMIZEBOX
		NULL, NULL))
        return false;          
        
                

    prevProc=SetWindowProcedure(&MainWindow::WindowProcedure);
    Show();
    return true;
}

// save a window's visibility state & hide it
BOOL CALLBACK MainWindow::SaveAndHideWindow(HWND hwnd, LPARAM lparam)
{
    MainWindow *win=(MainWindow*)lparam;
    
    char buffer[256];
    GetWindowText(hwnd,buffer,255);
    if ((hwnd!=win->GetHandle()) && (hwnd!=overviewWindow.GetHandle()))
    {
        win->savedWindows.push_back(hwnd);
        if (!IsWindowVisible(hwnd))
            win->restoreStates.push_back(0);
        else
            win->restoreStates.push_back(SW_SHOW);

        /* hide the window */                                                
        ShowWindow(hwnd,SW_HIDE);                                                           
        //AnimateWindow(hwnd,1000,AW_HIDE|AW_CENTER);
    }
    return true;
}


void MainWindow::RestoreAll()
{
    int i;
    for (i=0;i<mainWindow.savedWindows.size();i++)
    {
        if (mainWindow.restoreStates.at(i)!=0)
            ShowWindow(mainWindow.savedWindows.at(i),mainWindow.restoreStates.at(i));
    }
    
}

void MainWindow::MinimizeAll()
{
    mainWindow.savedWindows.clear();
    mainWindow.restoreStates.clear();
    EnumThreadWindows(GetCurrentThreadId(),(WNDENUMPROC)&SaveAndHideWindow,(LPARAM)&mainWindow);
}

/* handle events related to the main window */
LRESULT CALLBACK MainWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    MainWindow* win=(MainWindow*)Window::GetWindowObject(hwnd);
    int i;

    switch (message) 
    {
//		case WM_SIZE:
        /* handle minimizing and restoring the child windows */            
//		    switch(wParam)
//		    {
                
//                case SIZE_RESTORED:
//                    // restore windows to their previous state 
//                    win->RestoreAll();
//                    return 0;

//                 case SIZE_MINIMIZED:
//                    // record whether windows are currently visible & then hide them all
//                    win->MinimizeAll(); 
//					return 0;
//		    }    
//  	        return 0;
       
    }
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}
