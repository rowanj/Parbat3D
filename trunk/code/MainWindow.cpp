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

// save a window's visibility state & hide it (called by windows API)
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

// disable a particular window as long as it is not a particular window (called by windows API for every window)
BOOL CALLBACK MainWindow::DisableWindowCallback(HWND hwnd,LPARAM lparam)
{
	// check that this window is not the one we want to keep enabled
	if (hwnd!=(HWND)lparam)
	{
		Console::write("MainWindow::DisableWindowCallback\n");
		::EnableWindow(hwnd,false);
	}
	return true;
}

// enable a particular window (called by windows API for every window)
BOOL CALLBACK MainWindow::EnableWindowCallback(HWND hwnd,LPARAM lparam)
{
	// check that this window is not the one we want to keep enabled
	Console::write("MainWindow::EnableWindowCallback\n");	
	EnableWindow(hwnd,true);
	return true;
}

// destroy all windows owned by the main window
void MainWindow::DestroyAll()
{
    DestroyWindow(GetHandle());		// destory main window, which will destroy all others
}

// disable all windows, except for window with handle hKeepEnabled (if not NULL)
void MainWindow::DisableAll(HWND hKeepEnabled)
{
	// call callback function for every window owned by the main window's thread
	EnumThreadWindows(Window::GetThreadId(),(WNDENUMPROC)&DisableWindowCallback,(LPARAM)hKeepEnabled);
}

// disable all windows
void MainWindow::DisableAll()
{
	// call callback function for every window owned by the main window's thread
	EnumThreadWindows(Window::GetThreadId(),(WNDENUMPROC)&DisableWindowCallback,(LPARAM)0);
}

// enable all windows
void MainWindow::EnableAll()
{
	// call callback function for every window owned by the main window's thread
	EnumThreadWindows(Window::GetThreadId(),(WNDENUMPROC)&EnableWindowCallback,(LPARAM)0);	
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

    // call callback function for every window owned by current thread
    EnumThreadWindows(GetCurrentThreadId(),(WNDENUMPROC)&SaveAndHideWindow,(LPARAM)&mainWindow);
}



/* handle events related to the main window */
LRESULT CALLBACK MainWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    MainWindow* win=(MainWindow*)Window::GetWindowObject(hwnd);
    int i;

    switch (message) 
    {

       
    }
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}
