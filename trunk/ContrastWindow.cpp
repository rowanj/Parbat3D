#include <Windows.h>
#include "main.h"
#include "Window.h"
#include "ContrastWindow.h"
#include "Settings.h"
#include <string>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <commctrl.h>
#include "OverviewWindow.h"

/* ------------------------------------------------------------------------------------------------------------------------ */
/* Prefs Window Functions */


/* create main window */
int ContrastWindow::Create(HWND parent)
{
    RECT rect;
    int mx,my;
    const int CONTS_WINDOW_WIDTH=330;
    const int CONTS_WINDOW_HEIGHT=250;
    
    // Use ImageHandler to get current values;
    int previous_brightness = 250;
	int previous_contrast = 250;
    assert(image_handler != NULL);
    if(image_handler != NULL) {
    	image_handler->get_brightness_contrast(&previous_brightness, &previous_contrast);
	}
   
    /* Get Overview Window Location for Prefs window alignment*/
    GetWindowRect(overviewWindow.GetHandle(),&rect);

    /* create Contrast Brightness window */
    if (!CreateWin(0, "Parbat3D Contrast & Brightness Window", "Contrast Stretch",
	     WS_POPUP+WS_SYSMENU+WS_CAPTION,
	     rect.left+50, rect.bottom-150, CONTS_WINDOW_WIDTH, CONTS_WINDOW_HEIGHT, parent, NULL))
	    return false;

	/* Brightness Trackbar */

	//Brightness Trackbar title
	HWND hBtitle=CreateWindowEx(0, szStaticControl, "Brightness",
	WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 16, 8, 100, 18,
	GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetStaticFont(hBtitle,STATIC_FONT_NORMAL);
	
	// Create Trackbar (slider) for Brightness
	hBrightnessTrackbar = CreateWindowEx( 
    0,                             // no extended styles 
    "msctls_trackbar32",                // class name TRACKBAR_CLASS
    "Contrast",            // title (caption) 
    WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,  // style  | TBS_AUTOTICKS | TBS_ENABLESELRANGE
    5, 27,                        // position 
    310, 35,                      // size 
    GetHandle(),                       // parent window 
    NULL, //(HMENU) ID_TRACKBAR,             // control identifier  ID_TRACKBAR
    Window::GetAppInstance(),                       // instance 
    NULL                           // no WM_CREATE parameter 
    ); 
	
    // Trackbar range - number of positions
    SendMessage(hBrightnessTrackbar, TBM_SETRANGE, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) MAKELONG(1, 500));  // min. & max. positions
        
    // Set tick mark frequency
	SendMessage(hBrightnessTrackbar, TBM_SETTICFREQ, 
        (WPARAM) 25,				// wFreq
		0);  
    
    // Trackbar steps taken when clicking to the side of the pointer or scroll wheel
    SendMessage(hBrightnessTrackbar, TBM_SETPAGESIZE, 
        0, (LPARAM) 1);                  // new page size 
        
	//Trackbar - sets initial position
    SendMessage(hBrightnessTrackbar, TBM_SETPOS, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) previous_brightness); 
	
	//Trackbar tick values
	HWND hBValues=CreateWindowEx(0, szStaticControl, "0                                             50                                         100",
	WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 16, 62, 310, 18,
	GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetStaticFont(hBValues,STATIC_FONT_NORMAL);
	
	/* Contrast Trackbar */
	
	//Contrast Trackbar title
	HWND hCtitle=CreateWindowEx(0, szStaticControl, "Contrast",
	WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 16, 85, 100, 18,
	GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetStaticFont(hCtitle,STATIC_FONT_NORMAL);
	
	// Create Trackbar (slider) for Contrast
	hContrastTrackbar = CreateWindowEx( 
    0,                             // no extended styles 
    "msctls_trackbar32",                // class name TRACKBAR_CLASS
    "Contrast",            // title (caption) 
    WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,  // style  | TBS_AUTOTICKS | TBS_ENABLESELRANGE
    5, 104,                        // position 
    310, 35,                      // size 
    GetHandle(),                       // parent window 
    NULL, //(HMENU) ID_TRACKBAR,             // control identifier  ID_TRACKBAR
    Window::GetAppInstance(),                       // instance 
    NULL                           // no WM_CREATE parameter 
    ); 
	
    // Trackbar range - number of positions
    SendMessage(hContrastTrackbar, TBM_SETRANGE, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) MAKELONG(1, 500));  // min. & max. positions
        
    // Set tick mark frequency
	SendMessage(hContrastTrackbar, TBM_SETTICFREQ, 
        (WPARAM) 25,				// wFreq
		0);  
    
    // Trackbar steps taken when clicking to the side of the pointer or scroll wheel
    SendMessage(hContrastTrackbar, TBM_SETPAGESIZE, 
        0, (LPARAM) 1);                  // new page size 
        
	//Trackbar - sets initial position
    SendMessage(hContrastTrackbar, TBM_SETPOS, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) previous_contrast); 
	
	//Trackbar tick values
	HWND hCValues=CreateWindowEx(0, szStaticControl, "0                                             50                                         100",
	WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 16, 139, 310, 18,
	GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetStaticFont(hCValues,STATIC_FONT_NORMAL);
	
	hPerChannel = CreateWindowEx( 0, "BUTTON", "Per Channel", 
    BS_AUTOCHECKBOX | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 18, 160, 100, 16,
	GetHandle(),(HMENU) 9, Window::GetAppInstance(), NULL);
	
	hPreview = CreateWindowEx( 0, "BUTTON", "Preview", 
    BS_AUTOCHECKBOX | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 18, 180, 100, 16,
	GetHandle(),NULL, Window::GetAppInstance(), NULL);
	
	// Insert 'OK' button 
	hCSOKButton =  CreateWindowEx(0, "BUTTON", "OK", WS_CHILD | WS_VISIBLE | BS_CHECKBOX  | BS_PUSHLIKE,
		140, 180, 80, 25, GetHandle(), (HMENU) 1, Window::GetAppInstance(), NULL);
		
	// Insert 'Cancel' button 
	hCSCancelButton =  CreateWindowEx(0, "BUTTON", "Cancel", WS_CHILD | WS_VISIBLE | BS_CHECKBOX  | BS_PUSHLIKE,
		228, 180, 80, 25, GetHandle(), (HMENU) 2, Window::GetAppInstance(), NULL);

	prevProc=SetWindowProcedure(&WindowProcedure);	

    return true;
}

/* handle events related to the main window */
LRESULT CALLBACK ContrastWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{   
    static POINT moveMouseOffset;     /* mouse offset relative to window, used for snapping */
    static RECT rect;                 /* for general use */

    ContrastWindow* win=(ContrastWindow*)Window::GetWindowObject(hwnd);
    
    
    switch (message)                  /* handle the messages */
    {
		case WM_COMMAND:
             
			if (LOWORD(wParam) == 1 && HIWORD(wParam) == BN_CLICKED)
	        {
//				MessageBox( hwnd, (LPSTR) "Contrast Stretch complete", (LPSTR) "Contrast / brightness", MB_ICONINFORMATION | MB_OK );
				ShowWindow(hwnd,SW_HIDE);
				
				// Get Slider values
				DWORD Bstate = SendMessageA(win->hBrightnessTrackbar, TBM_GETPOS, 0, 0);
				// note: MessageBox expects char*, state is int
//				MessageBox( hwnd, (LPSTR) makeMessage("Bstate:",(int)Bstate), (LPSTR) "Title", MB_ICONINFORMATION | MB_OK );
				DWORD Cstate = SendMessageA(win->hContrastTrackbar, TBM_GETPOS, 0, 0);
//				MessageBox( hwnd, (LPSTR) makeMessage("Cstate:",(int)Cstate), (LPSTR) "Title", MB_ICONINFORMATION | MB_OK );
				assert(image_handler != NULL);
				if (image_handler != NULL) {
					image_handler->set_brightness_contrast((int)Bstate, (int)Cstate);
				}
			}
			
			if (LOWORD(wParam) == 2 && HIWORD(wParam) == BN_CLICKED)
	        {
				ShowWindow(hwnd,SW_HIDE);
			}
					
		    if (LOWORD(wParam) == 9 && HIWORD(wParam) == BN_CLICKED)
			{
                contrastWindow.Hide();
                SendMessage(contrastAdvWindow.hPerChannelAdv,BM_SETCHECK,BST_CHECKED,0);
                contrastAdvWindow.Show();
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
            CheckMenuItem(overviewWindow.hMainMenu,IDM_CONTSWINDOW,MF_UNCHECKED|MF_BYCOMMAND);            
            return 0;

        /* WM_DESTORY: system is destroying our window */
        case WM_DESTROY:
            break;
            
    }
    /* call the next procedure in the chain */       
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}
