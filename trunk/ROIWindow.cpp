#include <Windows.h>
#include "console.h"
#include "main.h"
#include "OverviewWindow.h"
#include "MainWindow.h"
#include "DisplayWindow.h"
#include "SnappingWindow.h"
#include "ToolWindow.h"
#include "Settings.h"
#include "ImageHandler.h"
#include "ImageWindow.h"
#include "ROIWindow.h"


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
	
    roiScrollbox =CreateWindowEx( 0, szStaticControl, NULL, 
		WS_VSCROLL | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 0,
		0, 243, 230, GetHandle(), NULL,
		Window::GetAppInstance(), NULL);

		roiTick=new HWND[20];	

    for (int i=0; i<20; i++)  
    {
		roiTick[i] =CreateWindowEx( 0, "BUTTON", "ROI name", 
		BS_CHECKBOX | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 10,
		10 + (20 * i), 100, 16, roiScrollbox, NULL,
		Window::GetAppInstance(), NULL);
	}
		
	roiToolBar =CreateWindowEx( 0, szStaticControl, "Buttons go here", 
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 0,
		230, 250, 70, GetHandle(), NULL,
		Window::GetAppInstance(), NULL);
	
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
        /* WM_NCLBUTTONDOWN: mouse button was pressed down in a non client area of the window */        
        case WM_NCLBUTTONDOWN:

            switch(wParam)
            {
                /* HTCAPTION: mouse button was pressed down on the window title bar */                            
                case HTCAPTION:
                    /* get the mouse co-ords relative to the window */
                    SnappingWindow::getMouseWindowOffset(hwnd,(int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam),&moveMouseOffset);               
                    break;

                /* HTLEFT...HTBOTTOMRIGHT: mouse button was pressed down on the sizing border of window */  
                /*case HTLEFT:
                case HTRIGHT:
                case HTTOP:
                case HTBOTTOM:
                case HTTOPLEFT:
                case HTTOPRIGHT:
                case HTBOTTOMLEFT:
                case HTBOTTOMRIGHT:*/
                    /* record current window & mouse positions */
                 /*   GetWindowRect(hwnd,&sizeWindowPosition);
                    sizeMousePosition.x=(int)(short)LOWORD(lParam);
                    sizeMousePosition.y=(int)(short)HIWORD(lParam);        */           
                    break;                   
            }

            /* also let windows handle this event */
            return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    

        /* WM_MOVING: the window is about to be moved to a new location */
        case WM_MOVING:

            /* set new window position based on position of mouse */
            SnappingWindow::setNewWindowPosition((RECT*)lParam,&moveMouseOffset);

            /* snap the window to other windows if in range */
            SnappingWindow::snapInsideWindowByMoving(hDesktop,(RECT*)lParam);      
            SnappingWindow::snapWindowByMoving(overviewWindow.GetHandle(),(RECT*)lParam);
            SnappingWindow::snapWindowByMoving(toolWindow.GetHandle(),(RECT*)lParam);            
            break;
    
        /* WM_SIZING: the window size is about to change */
        //case WM_SIZING:

            /* set new window size based on position of mouse */
            //SnappingWindow::setNewWindowSize((RECT*)lParam,&sizeWindowPosition,&sizeMousePosition,(int)wParam);

            /* prevent window from being resized too small */
            //if ( (((RECT*)lParam)->bottom) - (((RECT*)lParam)->top)  <100)
            //    (((RECT*)lParam)->bottom) = (((RECT*)lParam)->top) + 100;
            //if ( (((RECT*)lParam)->right) - (((RECT*)lParam)->left)  <100)
             //   (((RECT*)lParam)->right) = (((RECT*)lParam)->left) + 100;
            
            /* snap the window to the edge of the desktop (if near it) */
            SnappingWindow::snapInsideWindowBySizing(hDesktop,(RECT*)lParam,(int)wParam);   
                        
            /* snap the window the main window (if near it) */
            SnappingWindow::snapWindowBySizing(overviewWindow.GetHandle(),(RECT*)lParam,(int)wParam);           
            break;

        /* WM_SIZE: the window has been resized, minimized, or maximizsed, etc. */            
        case WM_SIZE:
           
            /* resize display/opengl window to fit new size */            
            GetClientRect(hwnd,&rect);
            //MoveWindow(win->roiWindowDisplay.GetHandle(),rect.left,rect.top,rect.right,rect.bottom,true);
            
            /* update scroll bar settings 
            if (roi_handler)
                win->updateROIScrollbar();
            return 0;
           */
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
            return 0;
            
        default:
            /* let windows handle any unknown messages */
            return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
    }
    /* return 0 to indicate that we have processed the message */       
    return 0;
}
