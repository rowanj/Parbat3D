#include <Windows.h>
#include "main.h"
#include "Window.h"
#include "ContrastAdvWindow.h"
#include "Settings.h"
#include <string>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <commctrl.h>

/* ------------------------------------------------------------------------------------------------------------------------ */
/* Prefs Window Functions */


/* create main window */
int ContrastAdvWindow::Create(HWND parent)
{
    RECT rect;
    int mx,my;
    const int CONTSADV_WINDOW_WIDTH=330;
    const int CONTSADV_WINDOW_HEIGHT=570;
   
    /* Get Overview Window Location for Prefs window alignment*/
    GetWindowRect(overviewWindow.GetHandle(),&rect);

    /* create Advanced Contrast Brightness window */
    if (!CreateWin(0, "Parbat3D Advanced Contrast & Brightness Window", "Advanced Contrast Stretch",
	     WS_POPUP+WS_SYSMENU+WS_CAPTION,
	     rect.left+50, rect.bottom-150, CONTSADV_WINDOW_WIDTH, CONTSADV_WINDOW_HEIGHT, parent, NULL))
	    return false;
	
	////////////////////////////////////////
	
	/* Red Brightness Trackbar */

	//Red Brightness Trackbar title
	HWND hRedBtitle=CreateWindowEx(0, szStaticControl, "Red Brightness",
	WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 16, 8, 100, 18,
	GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetStaticFont(hRedBtitle,STATIC_FONT_NORMAL);
	
	// Create Trackbar (slider) for Red Brightness
	hRedBrightnessTrackbar = CreateWindowEx( 
    0,                             // no extended styles 
    "msctls_trackbar32",                // class name TRACKBAR_CLASS
    "Red Brightness",            // title (caption) 
    WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,  // style  | TBS_AUTOTICKS | TBS_ENABLESELRANGE
    5, 27,                        // position 
    310, 35,                      // size 
    GetHandle(),                       // parent window 
    NULL, //(HMENU) ID_TRACKBAR,             // control identifier  ID_TRACKBAR
    Window::GetAppInstance(),                       // instance 
    NULL                           // no WM_CREATE parameter 
    ); 
	
    // Red Brightness Trackbar range - number of positions
    SendMessage(hRedBrightnessTrackbar, TBM_SETRANGE, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) MAKELONG(1, 255));  // min. & max. positions
        
    // Set tick mark frequency for Red Brightness
	SendMessage(hRedBrightnessTrackbar, TBM_SETTICFREQ, 
        (WPARAM) 128,				// wFreq
		0);  
    
    // Trackbar steps taken when clicking to the side of the pointer or scroll wheel for Red Brightness
    SendMessage(hRedBrightnessTrackbar, TBM_SETPAGESIZE, 
        0, (LPARAM) 1);                  // new page size 
        
	//Trackbar - sets initial position for Red Brightness
    SendMessage(hRedBrightnessTrackbar, TBM_SETPOS, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) 129); 
	
	//Trackbar tick values for Red Brightness
	HWND hRedBValues=CreateWindowEx(0, szStaticControl, "0                                             50                                         100",
	WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 16, 62, 310, 18,
	GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetStaticFont(hRedBValues,STATIC_FONT_NORMAL);
	
	/* Red Contrast Trackbar */
	
	//Red Contrast Trackbar title
	HWND hRedCtitle=CreateWindowEx(0, szStaticControl, "Red Contrast",
	WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 16, 85, 100, 18,
	GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetStaticFont(hRedCtitle,STATIC_FONT_NORMAL);
	
	// Create Trackbar (slider) for Red Contrast
	hRedContrastTrackbar = CreateWindowEx( 
    0,                             // no extended styles 
    "msctls_trackbar32",                // class name TRACKBAR_CLASS
    "Red Contrast",            // title (caption) 
    WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,  // style  | TBS_AUTOTICKS | TBS_ENABLESELRANGE
    5, 104,                        // position 
    310, 35,                      // size 
    GetHandle(),                       // parent window 
    NULL, //(HMENU) ID_TRACKBAR,             // control identifier  ID_TRACKBAR
    Window::GetAppInstance(),                       // instance 
    NULL                           // no WM_CREATE parameter 
    ); 
	
    // Trackbar range - number of positions for Red Contrast
    SendMessage(hRedContrastTrackbar, TBM_SETRANGE, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) MAKELONG(1, 255));  // min. & max. positions
        
    // Set tick mark frequency for Red Contrast
	SendMessage(hRedContrastTrackbar, TBM_SETTICFREQ, 
        (WPARAM) 128,				// wFreq
		0);  
    
    // Trackbar steps taken when clicking to the side of the pointer or scroll wheel for Red Contrast
    SendMessage(hRedContrastTrackbar, TBM_SETPAGESIZE, 
        0, (LPARAM) 1);                  // new page size 
        
	//Trackbar - sets initial position for Red Contrast
    SendMessage(hRedContrastTrackbar, TBM_SETPOS, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) 129); 
	
	//Trackbar tick values for Red Contrast
	HWND hRedCValues=CreateWindowEx(0, szStaticControl, "0                                             50                                         100",
	WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 16, 139, 310, 18,
	GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetStaticFont(hRedCValues,STATIC_FONT_NORMAL);
	
	/////////////////////////////////
	
		/* Green Brightness Trackbar */

	//Red Brightness Trackbar title
	HWND hGreenBtitle=CreateWindowEx(0, szStaticControl, "Green Brightness",
	WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 16, 168, 100, 18,
	GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetStaticFont(hGreenBtitle,STATIC_FONT_NORMAL);
	
	// Create Trackbar (slider) for Green Brightness
	hGreenBrightnessTrackbar = CreateWindowEx( 
    0,                             // no extended styles 
    "msctls_trackbar32",                // class name TRACKBAR_CLASS
    "Green Brightness",            // title (caption) 
    WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,  // style  | TBS_AUTOTICKS | TBS_ENABLESELRANGE
    5, 187,                        // position 
    310, 35,                      // size 
    GetHandle(),                       // parent window 
    NULL, //(HMENU) ID_TRACKBAR,             // control identifier  ID_TRACKBAR
    Window::GetAppInstance(),                       // instance 
    NULL                           // no WM_CREATE parameter 
    ); 
	
    // Green Brightness Trackbar range - number of positions
    SendMessage(hGreenBrightnessTrackbar, TBM_SETRANGE, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) MAKELONG(1, 255));  // min. & max. positions
        
    // Set tick mark frequency for Green Brightness
	SendMessage(hGreenBrightnessTrackbar, TBM_SETTICFREQ, 
        (WPARAM) 128,				// wFreq
		0);  
    
    // Trackbar steps taken when clicking to the side of the pointer or scroll wheel for Green Brightness
    SendMessage(hGreenBrightnessTrackbar, TBM_SETPAGESIZE, 
        0, (LPARAM) 1);                  // new page size 
        
	//Trackbar - sets initial position for Green Brightness
    SendMessage(hGreenBrightnessTrackbar, TBM_SETPOS, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) 129); 
	
	//Trackbar tick values for Green Brightness
	HWND hGreenBValues=CreateWindowEx(0, szStaticControl, "0                                             50                                         100",
	WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 16, 222, 310, 18,
	GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetStaticFont(hGreenBValues,STATIC_FONT_NORMAL);
	
	/* Green Contrast Trackbar */
	
	//Green Contrast Trackbar title
	HWND hGreenCtitle=CreateWindowEx(0, szStaticControl, "Green Contrast",
	WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 16, 245, 100, 18,
	GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetStaticFont(hGreenCtitle,STATIC_FONT_NORMAL);
	
	// Create Trackbar (slider) for Green Contrast
	hGreenContrastTrackbar = CreateWindowEx( 
    0,                             // no extended styles 
    "msctls_trackbar32",                // class name TRACKBAR_CLASS
    "Green Contrast",            // title (caption) 
    WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,  // style  | TBS_AUTOTICKS | TBS_ENABLESELRANGE
    5, 264,                        // position 
    310, 35,                      // size 
    GetHandle(),                       // parent window 
    NULL, //(HMENU) ID_TRACKBAR,             // control identifier  ID_TRACKBAR
    Window::GetAppInstance(),                       // instance 
    NULL                           // no WM_CREATE parameter 
    ); 
	
    // Trackbar range - number of positions for Green Contrast
    SendMessage(hGreenContrastTrackbar, TBM_SETRANGE, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) MAKELONG(1, 255));  // min. & max. positions
        
    // Set tick mark frequency for Green Contrast
	SendMessage(hGreenContrastTrackbar, TBM_SETTICFREQ, 
        (WPARAM) 128,				// wFreq
		0);  
    
    // Trackbar steps taken when clicking to the side of the pointer or scroll wheel for Green Contrast
    SendMessage(hGreenContrastTrackbar, TBM_SETPAGESIZE, 
        0, (LPARAM) 1);                  // new page size 
        
	//Trackbar - sets initial position for Green Contrast
    SendMessage(hGreenContrastTrackbar, TBM_SETPOS, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) 129); 
	
	//Trackbar tick values for Green Contrast
	HWND hGreenCValues=CreateWindowEx(0, szStaticControl, "0                                             50                                         100",
	WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 16, 299, 310, 18,
	GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetStaticFont(hGreenCValues,STATIC_FONT_NORMAL);
	
	/////////////////////////////////////////
	
	
		/* Blue Brightness Trackbar */

	//Blue Brightness Trackbar title
	HWND hBlueBtitle=CreateWindowEx(0, szStaticControl, "Green Brightness",
	WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 16, 328, 100, 18,
	GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetStaticFont(hBlueBtitle,STATIC_FONT_NORMAL);
	
	// Create Trackbar (slider) for Blue Brightness
	hBlueBrightnessTrackbar = CreateWindowEx( 
    0,                             // no extended styles 
    "msctls_trackbar32",                // class name TRACKBAR_CLASS
    "Blue Brightness",            // title (caption) 
    WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,  // style  | TBS_AUTOTICKS | TBS_ENABLESELRANGE
    5, 347,                        // position 
    310, 35,                      // size 
    GetHandle(),                       // parent window 
    NULL, //(HMENU) ID_TRACKBAR,             // control identifier  ID_TRACKBAR
    Window::GetAppInstance(),                       // instance 
    NULL                           // no WM_CREATE parameter 
    ); 
	
    // Blue Brightness Trackbar range - number of positions
    SendMessage(hBlueBrightnessTrackbar, TBM_SETRANGE, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) MAKELONG(1, 255));  // min. & max. positions
        
    // Set tick mark frequency for Blue Brightness
	SendMessage(hBlueBrightnessTrackbar, TBM_SETTICFREQ, 
        (WPARAM) 128,				// wFreq
		0);  
    
    // Trackbar steps taken when clicking to the side of the pointer or scroll wheel for Blue Brightness
    SendMessage(hBlueBrightnessTrackbar, TBM_SETPAGESIZE, 
        0, (LPARAM) 1);                  // new page size 
        
	//Trackbar - sets initial position for Blue Brightness
    SendMessage(hBlueBrightnessTrackbar, TBM_SETPOS, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) 129); 
	
	//Trackbar tick values for Blue Brightness
	HWND hBlueBValues=CreateWindowEx(0, szStaticControl, "0                                             50                                         100",
	WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 16, 382, 310, 18,
	GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetStaticFont(hBlueBValues,STATIC_FONT_NORMAL);
	
	/* Blue Contrast Trackbar */
	
	//Blue Contrast Trackbar title
	HWND hBlueCtitle=CreateWindowEx(0, szStaticControl, "Green Contrast",
	WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 16, 405, 100, 18,
	GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetStaticFont(hBlueCtitle,STATIC_FONT_NORMAL);
	
	// Create Trackbar (slider) for Blue Contrast
	hBlueContrastTrackbar = CreateWindowEx( 
    0,                             // no extended styles 
    "msctls_trackbar32",                // class name TRACKBAR_CLASS
    "Blue Contrast",            // title (caption) 
    WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,  // style  | TBS_AUTOTICKS | TBS_ENABLESELRANGE
    5, 424,                        // position 
    310, 35,                      // size 
    GetHandle(),                       // parent window 
    NULL, //(HMENU) ID_TRACKBAR,             // control identifier  ID_TRACKBAR
    Window::GetAppInstance(),                       // instance 
    NULL                           // no WM_CREATE parameter 
    ); 
	
    // Trackbar range - number of positions for Blue Contrast
    SendMessage(hBlueContrastTrackbar, TBM_SETRANGE, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) MAKELONG(1, 255));  // min. & max. positions
        
    // Set tick mark frequency for Blue Contrast
	SendMessage(hBlueContrastTrackbar, TBM_SETTICFREQ, 
        (WPARAM) 128,				// wFreq
		0);  
    
    // Trackbar steps taken when clicking to the side of the pointer or scroll wheel for Blue Contrast
    SendMessage(hBlueContrastTrackbar, TBM_SETPAGESIZE, 
        0, (LPARAM) 1);                  // new page size 
        
	//Trackbar - sets initial position for Blue Contrast
    SendMessage(hBlueContrastTrackbar, TBM_SETPOS, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) 129); 
	
	//Trackbar tick values for Blue Contrast
	HWND hBlueCValues=CreateWindowEx(0, szStaticControl, "0                                             50                                         100",
	WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 16, 459, 310, 18,
	GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetStaticFont(hBlueCValues,STATIC_FONT_NORMAL);
	
	//////////////////////////////////////////
	
	hPerChannelAdv = CreateWindowEx( 0, "BUTTON", "Per Channel", 
    BS_AUTOCHECKBOX | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 18, 480, 100, 16,
	GetHandle(),(HMENU) 9, Window::GetAppInstance(), NULL);
	
	hPreviewAdv = CreateWindowEx( 0, "BUTTON", "Preview", 
    BS_AUTOCHECKBOX | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 18, 500, 100, 16,
	GetHandle(),NULL, Window::GetAppInstance(), NULL);
	
	// Insert 'OK' button 
	hAdvCSOKButton =  CreateWindowEx(0, "BUTTON", "OK", WS_CHILD | WS_VISIBLE | BS_CHECKBOX  | BS_PUSHLIKE,
		140, 500, 80, 25, GetHandle(), (HMENU) 1, Window::GetAppInstance(), NULL);
		
	// Insert 'Cancel' button 
	hAdvCSCancelButton =  CreateWindowEx(0, "BUTTON", "Cancel", WS_CHILD | WS_VISIBLE | BS_CHECKBOX  | BS_PUSHLIKE,
		228, 500, 80, 25, GetHandle(), (HMENU) 2, Window::GetAppInstance(), NULL);
		
		
	
	
	prevProc=SetWindowProcedure(&WindowProcedure);	

    return true;
}

/* handle events related to the main window */
LRESULT CALLBACK ContrastAdvWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{   
    static POINT moveMouseOffset;     /* mouse offset relative to window, used for snapping */
    static RECT rect;                 /* for general use */

    ContrastAdvWindow* win=(ContrastAdvWindow*)Window::GetWindowObject(hwnd);
    
    
    switch (message)                  /* handle the messages */
    {
		case WM_COMMAND:
             
			if (LOWORD(wParam) == 1 && HIWORD(wParam) == BN_CLICKED)
	        {
				MessageBox( hwnd, (LPSTR) "Contrast Stretch complete", (LPSTR) "Contrast / brightness", MB_ICONINFORMATION | MB_OK );
				ShowWindow(hwnd,SW_HIDE);
				
				
			// Get Slider values
			DWORD RBstate = SendMessageA(win->hRedBrightnessTrackbar, TBM_GETPOS, 0, 0);
			MessageBox( hwnd, (LPSTR) makeMessage("RBstate:",(int)RBstate), (LPSTR) "Title", MB_ICONINFORMATION | MB_OK );
			DWORD RCstate = SendMessageA(win->hRedContrastTrackbar, TBM_GETPOS, 0, 0);
			MessageBox( hwnd, (LPSTR) makeMessage("RCstate:",(int)RCstate), (LPSTR) "Title", MB_ICONINFORMATION | MB_OK );
			
			DWORD GBstate = SendMessageA(win->hGreenBrightnessTrackbar, TBM_GETPOS, 0, 0);
			MessageBox( hwnd, (LPSTR) makeMessage("GBstate:",(int)GBstate), (LPSTR) "Title", MB_ICONINFORMATION | MB_OK );
			DWORD GCstate = SendMessageA(win->hGreenContrastTrackbar, TBM_GETPOS, 0, 0);
			MessageBox( hwnd, (LPSTR) makeMessage("GCstate:",(int)GCstate), (LPSTR) "Title", MB_ICONINFORMATION | MB_OK );
			
			DWORD BBstate = SendMessageA(win->hBlueBrightnessTrackbar, TBM_GETPOS, 0, 0);
			MessageBox( hwnd, (LPSTR) makeMessage("BBstate:",(int)BBstate), (LPSTR) "Title", MB_ICONINFORMATION | MB_OK );
			DWORD BCstate = SendMessageA(win->hBlueContrastTrackbar, TBM_GETPOS, 0, 0);
			MessageBox( hwnd, (LPSTR) makeMessage("BCstate:",(int)BCstate), (LPSTR) "Title", MB_ICONINFORMATION | MB_OK );
			}
			
			if (LOWORD(wParam) == 2 && HIWORD(wParam) == BN_CLICKED)
	        {
				ShowWindow(hwnd,SW_HIDE);
			}
			
			if (LOWORD(wParam) == 9 && HIWORD(wParam) == BN_CLICKED)
			{
                contrastAdvWindow.Hide();
                SendMessage(contrastWindow.hPerChannel,BM_SETCHECK,0,0);
                //SendMessage( hwnd, WM_CLOSE, 0, 0L );
                contrastWindow.Show();
            }
			
			return 0;
			
        case WM_SHOWWINDOW:
            /* update window menu item depending on whether window is shown or hidden */
            
            if (wParam)
                CheckMenuItem(overviewWindow.hMainMenu,IDM_CONTSWINDOW,MF_CHECKED|MF_BYCOMMAND);            
            else
                CheckMenuItem(overviewWindow.hMainMenu,IDM_CONTSWINDOW,MF_UNCHECKED|MF_BYCOMMAND);
            return 0;
            
        /* WM_CLOSE: system or user has requested to close the window/application */             
        case WM_CLOSE:
            /* don't destroy this window, but make it invisible */
            ShowWindow(hwnd,SW_HIDE);
            return 0;

        /* WM_DESTORY: system is destroying our window */
        case WM_DESTROY:
            CheckMenuItem(overviewWindow.hMainMenu,IDM_CONTSWINDOW,MF_UNCHECKED|MF_BYCOMMAND);            
            break;
            
    }
    /* call the next procedure in the chain */       
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}
