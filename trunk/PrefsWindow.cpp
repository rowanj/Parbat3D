#include <Windows.h>
#include "main.h"
#include "Window.h"
#include "PrefsWindow.h"
#include "Settings.h"
#include <string>
#include <cstdio>
#include <iostream>
#include <cstring>

/* ------------------------------------------------------------------------------------------------------------------------ */
/* Prefs Window Functions */


/* create main window */
int PrefsWindow::Create(HWND parent)
{
    RECT rect;
    int mx,my;
    const int PREFS_WINDOW_WIDTH=400;
    const int PREFS_WINDOW_HEIGHT=500;
   
    /* Get Overview Window Location for Prefs window alignment*/
    GetWindowRect(overviewWindow.GetHandle(),&rect);

    /* create prefs window */
    if (!CreateWin(0, "Parbat3D Prefs Window", "Preferences",
	     WS_POPUP+WS_SYSMENU+WS_CAPTION,
	     rect.left+50, rect.bottom-150, PREFS_WINDOW_WIDTH, PREFS_WINDOW_HEIGHT, parent, NULL))
	    return false;

	HBRUSH backBrush=CreateSolidBrush(GetSysColor(COLOR_3DFACE));
    SetBackgroundBrush(backBrush);

    cacheLabel =CreateWindowEx( 0, szStaticControl, "Cache size (MB):", 
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_OWNERDRAW, 3,
		3, 120, 20, GetHandle(), NULL,
		GetAppInstance(), NULL);
	
	SetStaticFont(cacheLabel, STATIC_FONT_NORMAL);

	cacheEntry =CreateWindowEx( WS_EX_CLIENTEDGE, "EDIT", (settingsFile->getSetting("preferences", "cachesize")).c_str(), 
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | ES_LEFT | SS_OWNERDRAW, 125,
		3, 40, 20, GetHandle(), NULL,
		GetAppInstance(), NULL);
		
	okButton = CreateWindowEx(0, "BUTTON", "OK", WS_CHILD | WS_VISIBLE | WS_BORDER, 270,
			435, 55, 25, GetHandle(), (HMENU) 1, Window::GetAppInstance(), NULL);
			
	cancelButton = CreateWindowEx(0, "BUTTON", "Cancel", WS_CHILD | WS_VISIBLE | WS_BORDER, 330,
			435, 55, 25, GetHandle(), (HMENU) 2, Window::GetAppInstance(), NULL);		

    prevProc=SetWindowProcedure(&WindowProcedure);	

    stickyWindowManager.AddStickyWindow(this);  // make the window stick to others 

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
		case WM_COMMAND:
			
			if (LOWORD(wParam) == 1 && HIWORD(wParam) == BN_CLICKED)
	        {
				char* cacheSizeCBuffer = new char[255];
				cacheSizeCBuffer[0] = (char)255;
				cacheSizeCBuffer[1] = (char)0;
				std::string cacheSizeString;
				std::stringstream convertorStream;
				
				LRESULT theResult = SendMessage(win->cacheEntry, EM_GETLINE, 0,(LPARAM) cacheSizeCBuffer);
				Console::write("Got ");
				Console::write(theResult);
				Console::write(" TCHARs of data\n");
				convertorStream << cacheSizeCBuffer;
				convertorStream >> cacheSizeString;
				Console::write("Got cache entry - C string = ");
				Console::write(cacheSizeCBuffer);
				Console::write("\n");
				Console::write("C++ string = ");
				Console::write(&cacheSizeString);
				Console::write("\n");
				
				settingsFile->setSetting("preferences", "cachesize", cacheSizeString);
				MessageBox( hwnd, (LPSTR) "Preferences saved successfully.",(LPSTR) "Parbat3D",
				MB_ICONINFORMATION | MB_OK );
	        } 
			if (LOWORD(wParam) == 2 && HIWORD(wParam) == BN_CLICKED)
	        {
				ShowWindow(hwnd,SW_HIDE);
	        } 
			return 0;
			
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
            break;
            
    }
    /* call the next procedure in the chain */       
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}
