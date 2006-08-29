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
    const int PREFS_WINDOW_WIDTH=190;
    const int PREFS_WINDOW_HEIGHT=120;
   
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
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_OWNERDRAW, 6,
		6, 120, 20, GetHandle(), NULL,
		GetAppInstance(), NULL);
	
	SetStaticFont(cacheLabel, STATIC_FONT_NORMAL);

	cacheEntry =CreateWindowEx( WS_EX_CLIENTEDGE, "EDIT", (settingsFile->getSetting("preferences", "cachesize")).c_str(), 
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | ES_LEFT | SS_OWNERDRAW, 138,
		6, 40, 20, GetHandle(), NULL,
		GetAppInstance(), NULL);

	texSizeLabel =CreateWindowEx( 0, szStaticControl, "Texture dimension (pixels):", 
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_OWNERDRAW, 6,
		29, 130, 20, GetHandle(), NULL,
		GetAppInstance(), NULL);
	
	SetStaticFont(texSizeLabel, STATIC_FONT_NORMAL);

	texSizeEntry =CreateWindowEx( WS_EX_CLIENTEDGE, "EDIT", (settingsFile->getSetting("preferences", "texsize")).c_str(), 
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | ES_LEFT | SS_OWNERDRAW, 138,
		29, 40, 20, GetHandle(), NULL,
		GetAppInstance(), NULL);
		
	okButton = CreateWindowEx(0, "BUTTON", "OK", WS_CHILD | WS_VISIBLE | WS_BORDER, 60,
			55, 55, 25, GetHandle(), (HMENU) 1, Window::GetAppInstance(), NULL);
			
	cancelButton = CreateWindowEx(0, "BUTTON", "Cancel", WS_CHILD | WS_VISIBLE | WS_BORDER, 120,
			55, 55, 25, GetHandle(), (HMENU) 2, Window::GetAppInstance(), NULL);		

    prevProc=SetWindowProcedure(&WindowProcedure);	

    //stickyWindowManager.AddStickyWindow(this);  // make the window stick to others 

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
				char* entryCBuffer = new char[255];
				entryCBuffer[0] = (char)255;
				entryCBuffer[1] = (char)0;
				
				LRESULT theResult = SendMessage(win->cacheEntry, EM_GETLINE, 0,(LPARAM) entryCBuffer);				
				settingsFile->setSetting("preferences", "cachesize", atoi(entryCBuffer));
				
				entryCBuffer[0] = (char)255;
				entryCBuffer[1] = (char)0;
				theResult = SendMessage(win->texSizeEntry, EM_GETLINE, 0,(LPARAM) entryCBuffer);
				settingsFile->setSetting("preferences", "texsize", atoi(entryCBuffer));
				
				MessageBox( hwnd, (LPSTR) "Preferences saved successfully.\nThis will take effect after restarting Parbat.",(LPSTR) "Parbat3D",
				MB_ICONINFORMATION | MB_OK );
				ShowWindow(hwnd,SW_HIDE);
				delete entryCBuffer;
	        } 
			if (LOWORD(wParam) == 2 && HIWORD(wParam) == BN_CLICKED)
	        {
				ShowWindow(hwnd,SW_HIDE);
				LRESULT theResult = SendMessage(win->cacheEntry, WM_SETTEXT, 0, (LPARAM)(settingsFile->getSetting("preferences", "cachesize")).c_str());
				theResult = SendMessage(win->texSizeEntry, WM_SETTEXT, 0, (LPARAM)(settingsFile->getSetting("preferences", "texsize")).c_str());
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
