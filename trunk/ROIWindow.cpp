#include <Windows.h>
#include "console.h"
#include "main.h"
#include "OverviewWindow.h"
#include "MainWindow.h"
#include "DisplayWindow.h"
#include "ToolWindow.h"
#include "Settings.h"
#include "ImageHandler.h"
#include "ImageWindow.h"
#include "ROIWindow.h"
#include "ScrollBox.h"

ScrollBox scrollBox;

int ROIWindow::Create(HWND parent)
{
    
    RECT rect;
    int mx,my;
    const int ROI_WINDOW_WIDTH=250;
    const int ROI_WINDOW_HEIGHT=300;

    /* Get Main Window Location for ROI window alignment*/
    GetWindowRect(overviewWindow.GetHandle(),&rect);

    /* create ROI window */
    if (!CreateWin(0, "Parbat3D ROI Window", "Regions of Interest",
	     WS_POPUP+WS_SYSMENU+WS_CAPTION,
	     rect.left, rect.bottom+30, 250, 300, parent, NULL))
	    return false;
	    
    prevProc=SetWindowProcedure(&WindowProcedure);
	
	RECT rect2;
   	rect2.top=5;
   	rect2.left=5;
   	rect2.right=235;
   	rect2.bottom=220;                 	
	scrollBox.Create(GetHandle(),&rect2);

	// temporary ROI list with check boxes
	hROITick=new HWND[5];	
    for (int i=0; i<5; i++)  
    {
		hROITick[i] =CreateWindowEx( 0, "BUTTON", "ROI name", 
		BS_AUTOCHECKBOX | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 10,
		10 + (20 * i), 100, 16, scrollBox.GetHandle(),NULL,
		Window::GetAppInstance(), NULL);
	}
	
	//Create Open button
	hOpenButton =CreateWindowEx( 0, "BUTTON", NULL, 
		BS_ICON | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 5,
		230, 36, 36, GetHandle(), (HMENU) 1,
		Window::GetAppInstance(), NULL);
		
	hOpenIcon=(HICON)LoadImage(NULL,IDI_EXCLAMATION,IMAGE_ICON,0,0,LR_SHARED);
	SendMessage (hOpenButton, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadImage (Window::GetAppInstance(), "open.ico", IMAGE_ICON, 32, 32,LR_LOADFROMFILE));
	// this also works: SendMessage(openButton,BM_SETIMAGE,IMAGE_ICON,(LPARAM)hIcon);
	
	//Create Save button
	hSaveButton =CreateWindowEx( 0, "BUTTON", NULL, 
		BS_ICON | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 45,
		230, 36, 36, GetHandle(),  (HMENU) 2,
		Window::GetAppInstance(), NULL);
		
	hSaveIcon=(HICON)LoadImage(NULL,IDI_EXCLAMATION,IMAGE_ICON,0,0,LR_SHARED);
	SendMessage (hSaveButton, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadImage (Window::GetAppInstance(), "save.ico", IMAGE_ICON, 32, 32,LR_LOADFROMFILE));
	
	//Create Polygon selection button
	hPolyButton =CreateWindowEx( 0, "BUTTON", NULL, 
		BS_ICON | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 100,
		230, 36, 36, GetHandle(),  (HMENU) 3,
		Window::GetAppInstance(), NULL);
		
	hPolyIcon=(HICON)LoadImage(NULL,IDI_EXCLAMATION,IMAGE_ICON,0,0,LR_SHARED);
	SendMessage (hPolyButton, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadImage (Window::GetAppInstance(), "poly.ico", IMAGE_ICON, 32, 32,LR_LOADFROMFILE));
	
	//Create Rectangle selection button
	hRectButton =CreateWindowEx( 0, "BUTTON", NULL, 
		BS_ICON | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 140,
		230, 36, 36, GetHandle(),  (HMENU) 4,
		Window::GetAppInstance(), NULL);
		
	hRectIcon=(HICON)LoadImage(NULL,IDI_EXCLAMATION,IMAGE_ICON,0,0,LR_SHARED);
	SendMessage (hRectButton, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadImage (Window::GetAppInstance(), "rect.ico", IMAGE_ICON, 32, 32,LR_LOADFROMFILE));
	
	//Create Trash button
	hTrashButton =CreateWindowEx( 0, "BUTTON", NULL, 
		BS_ICON | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 200,
		230, 36, 36, GetHandle(),  (HMENU) 5,
		Window::GetAppInstance(), NULL);
		
	hTrashIcon=(HICON)LoadImage(NULL,IDI_EXCLAMATION,IMAGE_ICON,0,0,LR_SHARED);
	SendMessage (hTrashButton, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadImage (Window::GetAppInstance(), "trash.ico", IMAGE_ICON, 32, 32,LR_LOADFROMFILE));
	
    scrollBox.UpdateScrollBar();
	
	return true;
}

/* All messages/events related to the ROI window (or it's controls) are sent to this procedure */
LRESULT CALLBACK ROIWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static POINT moveMouseOffset;     /* mouse offset relative to window, used for snapping */
    //static POINT sizeMousePosition;   /* mouse position, used for sizing window */
    //static RECT  sizeWindowPosition;  /* window position, used for sizing window */
    static RECT rect;                 /* for general use */

    ROIWindow* win=(ROIWindow*)Window::GetWindowObject(hwnd);
    
    switch (message)                  /* handle the messages */
    {
        case WM_COMMAND:
             
            // find out which ROIs are selected
            if((SendMessageA(win->hROITick[0], BM_GETCHECK, 0, 0))==BST_CHECKED)
            {
               MessageBox( hwnd, (LPSTR) "First ROI is checked",(LPSTR) "Action",
					MB_ICONINFORMATION | MB_OK );
            }

			
			// Handle actions for each button pressed
			if (LOWORD(wParam) == 1 && HIWORD(wParam) == BN_CLICKED)
            {
				MessageBox( hwnd, (LPSTR) "Open ROI",(LPSTR) "Action",
					MB_ICONINFORMATION | MB_OK );
            } 
			if (LOWORD(wParam) == 2 && HIWORD(wParam) == BN_CLICKED)
            {
				MessageBox( hwnd, (LPSTR) "Save ROI",(LPSTR) "Action",
					MB_ICONINFORMATION | MB_OK );
            } 
			if (LOWORD(wParam) == 3 && HIWORD(wParam) == BN_CLICKED)
            {
				MessageBox( hwnd, (LPSTR) "Create ROI Poly tool",(LPSTR) "Action",
					MB_ICONINFORMATION | MB_OK );
            } 
			if (LOWORD(wParam) == 4 && HIWORD(wParam) == BN_CLICKED)
            {
				MessageBox( hwnd, (LPSTR) "Create ROI Rect tool",(LPSTR) "Action",
					MB_ICONINFORMATION | MB_OK );
            } 
			if (LOWORD(wParam) == 5 && HIWORD(wParam) == BN_CLICKED)
            {
				MessageBox( hwnd, (LPSTR) "Delete it",(LPSTR) "Action",
					MB_ICONINFORMATION | MB_OK );
            } 
			
			return 0;
		
		

        case WM_SHOWWINDOW:
            /* update window menu item depending on whether window is shown or hidden */
            if (wParam)
                CheckMenuItem(overviewWindow.hMainMenu,IDM_ROIWINDOW,MF_CHECKED|MF_BYCOMMAND);            
            else
                CheckMenuItem(overviewWindow.hMainMenu,IDM_ROIWINDOW,MF_UNCHECKED|MF_BYCOMMAND);
            return 0;

        /* WM_CLOSE: system or user has requested to close the window/application */             
        case WM_CLOSE:
            /* don't destroy this window, but make it invisible */
            ShowWindow(hwnd,SW_HIDE);
            return 0;

        /* WM_DESTORY: system is destroying our window */
        case WM_DESTROY:
            CheckMenuItem(overviewWindow.hMainMenu,IDM_ROIWINDOW,MF_UNCHECKED|MF_BYCOMMAND);            
            break;
    }
    // call next window procedure in chain
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}
