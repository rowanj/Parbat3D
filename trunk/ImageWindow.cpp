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

HWND hImageWindow=NULL;
HWND hImageWindowDisplay;

char szImageWindowClassName[] = "Parbat3D Image Window";

/* ------------------------------------------------------------------------------------------------------------------------ */
/* Image Window Functions */

/* register image window class */
int registerImageWindow()
{
    WNDCLASSEX wincl;  /* Datastructure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;                /* process's instance handle */
    wincl.lpszClassName = szImageWindowClassName;   /* our unique name for this class */
    wincl.lpfnWndProc = ImageWindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS; /* Ctach double-clicks */
    wincl.cbSize = sizeof(WNDCLASSEX);

    /* Use default icon and mousepointer */
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL; /* No menu */
    wincl.cbClsExtra = 0; /* No extra bytes after the window class */
    wincl.cbWndExtra = 0; /* structure or the window instance */
    /* Use lightgray as the background of the window */
    wincl.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);

    /* Register the window class, if fails return false */
    return RegisterClassEx(&wincl);
}

/* create the image window */
int setupImageWindow()
{   
    RECT rect;
    int mx,my;
    const int IMAGE_WINDOW_WIDTH=700;
    const int IMAGE_WINDOW_HEIGHT=600;

    /* Get the width & height of the desktop window */
    GetWindowRect(hDesktop,&rect);

    /* Get the stored window position or use defaults if there's a problem */
    mx = atoi(winPos.getSetting("OverviewX").c_str());
	my = atoi(winPos.getSetting("OverviewY").c_str());
	if ((mx <= 0) || (mx >= rect.right))
        mx = (rect.right /2) - ((IMAGE_WINDOW_WIDTH+OverviewWindow::OVERVIEW_WINDOW_WIDTH) /2);         /* default x position to center windows */
    mx+=OverviewWindow::OVERVIEW_WINDOW_WIDTH;                                                          /* leave room for overview window */
	       
	if ((my <= 0) || (my >= rect.bottom))
       my = (rect.bottom /2) - (IMAGE_WINDOW_HEIGHT/2);                                 /* default y position to center windows */

    /* create image window */
    hImageWindow =CreateWindowEx(0, szImageWindowClassName, "Image Window",
	     WS_POPUP+WS_SYSMENU+WS_CAPTION+WS_MAXIMIZEBOX+WS_VSCROLL+WS_HSCROLL+WS_SIZEBOX,
	     mx, my, IMAGE_WINDOW_WIDTH, IMAGE_WINDOW_HEIGHT, hMainWindow, NULL, hThisInstance, NULL);
    if (!hImageWindow)
        return false;  

    /* get client area of image window */
    GetClientRect(hImageWindow,&rect);

    /* create a child window that will be used by OpenGL */
    hImageWindowDisplay=CreateWindowEx( 0, DisplayWindow::szDisplayClassName, NULL, WS_CHILD+WS_VISIBLE,
		rect.left, rect.top, rect.right, rect.bottom, hImageWindow, NULL, hThisInstance, NULL);

    if (hImageWindow==NULL)
        return false;


    return true;
}

/* display image information in image window's title bar */
void updateImageWindowTitle()
{
    /* Display the file name & zoom level on the image window title bar */
    string leader = "Image - ";
    string title  = makeMessage(leader,filename);
    title+=makeString(" (",int(100.0 / pow((double)2,(double)image_handler->get_LOD())));
    title+="%)";
	SetWindowText(hImageWindow, (char*) title.c_str());    
}

/* update image window's scroll bar display settings  */
void updateImageScrollbar()
{
    int LOD_width,LOD_height;

    SCROLLINFO info_x,info_y;

    /* get window size and image size at current zoom level */   
    info_x.nPage=image_handler->get_viewport_width();
    info_y.nPage=image_handler->get_viewport_height();    
    LOD_width=image_handler->get_LOD_width();
    LOD_height=image_handler->get_LOD_height();
    
    /* set scroll range */
    info_x.nMin=0;
    info_y.nMin=0;
    info_x.nMax=LOD_width;// - info_x.nPage;
    info_y.nMax=LOD_height;// - info_y.nPage;
    
    /* set scroll position */
    info_x.nPos=image_handler->get_viewport_x();
    info_y.nPos=image_handler->get_viewport_y();
    
    Console::write("updateScrollbarSettings():\n");
    Console::write("LOD_width=");
    Console::write(LOD_width);
    Console::write("\nLOD_height=");
    Console::write(LOD_height);
    Console::write("\nviewport_width=");
    Console::write(info_x.nPage);
    Console::write("\nviewport_height=");
    Console::write(info_y.nPage);
    Console::write("\nscroll_width=");
    Console::write(info_x.nMax);
    Console::write("\nscroll_height=");
    Console::write(info_y.nMax);
    Console::write("\nscroll_x=");
    Console::write(info_x.nPos);
    Console::write("\nscroll_y=");
    Console::write(info_y.nPos);
    
    /* set scrollbar info */
    info_x.cbSize=sizeof(SCROLLINFO);
    info_x.fMask=SIF_ALL;
    info_y.cbSize=sizeof(SCROLLINFO);
    info_y.fMask=SIF_ALL;    
    SetScrollInfo(hImageWindow,SB_HORZ,&info_x,true);
    SetScrollInfo(hImageWindow,SB_VERT,&info_y,true);    
      
}

/* scroll image window horizontally */
void scrollImageX(int scrollMsg)
{
    SCROLLINFO info;

    /* get current scroll position & range */    
    info.cbSize=sizeof(SCROLLINFO);
    info.fMask=SIF_ALL;
    GetScrollInfo(hImageWindow,SB_HORZ,&info);
     
    Console::write("scrollImageX() ");
    switch(LOWORD(scrollMsg))
    {
        case SB_LINEUP:
            info.nPos--;
            break;
        case SB_LINEDOWN:
            info.nPos++;
            break;
        case SB_PAGEUP:
            info.nPos-=image_handler->get_viewport_width();
            break;
        case SB_PAGEDOWN:
            info.nPos+=image_handler->get_viewport_width();                
            break;
        case SB_THUMBTRACK:
            info.nPos=info.nTrackPos;
            break;
        default:
            return;
    }

    // check new position is within scroll range
    if (info.nPos<info.nMin)
        info.nPos=info.nMin;
    else if (info.nPos>(info.nMax-info.nPage))
        info.nPos=info.nMax-info.nPage;

    Console::write("xpos=");
    Console::write(info.nPos);
    Console::write("\n");

    // update scroll position
    info.fMask=SIF_POS;
    SetScrollInfo(hImageWindow,SB_HORZ,&info,true);
    image_handler->set_viewport_x(info.nPos);
}

/* scroll image window vertically */
void scrollImageY(int scrollMsg)
{
    SCROLLINFO info;

    /* get current scroll position & range */    
    info.cbSize=sizeof(SCROLLINFO);
    info.fMask=SIF_ALL;
    GetScrollInfo(hImageWindow,SB_VERT,&info);
     
    Console::write("scrollImageY() ");
    switch(LOWORD(scrollMsg))
    {
        case SB_LINEUP:
            info.nPos--;
            break;
        case SB_LINEDOWN:
            info.nPos++;
            break;
        case SB_PAGEUP:
            info.nPos-=image_handler->get_viewport_height();
            break;
        case SB_PAGEDOWN:
            info.nPos+=image_handler->get_viewport_height();                
            break;
        case SB_THUMBTRACK:
            info.nPos=info.nTrackPos;
            break;
        default:
            return;
    }

    // check new position is within scroll range
    if (info.nPos<info.nMin)
        info.nPos=info.nMin;
    else if (info.nPos>(info.nMax-info.nPage))
        info.nPos=info.nMax-info.nPage;

    Console::write("ypos=");
    Console::write(info.nPos);
    Console::write("\n");

    // update scroll position
    info.fMask=SIF_POS;
    SetScrollInfo(hImageWindow,SB_VERT,&info,true);
    image_handler->set_viewport_y(info.nPos);
}

/* zoom the image in/out */
void zoomImage(int nlevels)
{
    const int MAX_LOD=6;
    int LOD;
 
    Console::write("zoomImage() nlevels=");   
    Console::write(nlevels);
    Console::write("\n");
    LOD=image_handler->get_LOD();
    LOD+=nlevels;
    if (LOD<0)
        LOD=0;
    else if (LOD>MAX_LOD)
        LOD=MAX_LOD;
    image_handler->set_LOD(LOD);
    updateImageWindowTitle();
    updateImageScrollbar();
}

/* This function is called by the Windowsfunction DispatchMessage( ) */
/* All messages/events related to the image window (or it's controls) are sent to this procedure */
LRESULT CALLBACK ImageWindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static POINT moveMouseOffset;     /* mouse offset relative to window, used for snapping */
    static POINT sizeMousePosition;   /* mouse position, used for sizing window */
    static RECT  sizeWindowPosition;  /* window position, used for sizing window */
    static RECT rect;                 /* for general use */
    
    switch (message)                  /* handle the messages */
    {
        /* WM_NCLBUTTONDOWN: mouse button was pressed down in a non client area of the window */        
        case WM_NCLBUTTONDOWN:

            switch(wParam)
            {
                /* HTCAPTION: mouse button was pressed down on the window title bar */                            
                case HTCAPTION:
                    /* get the mouse co-ords relative to the window */
                    getMouseWindowOffset(hwnd,(int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam),&moveMouseOffset);               
                    break;

                /* HTLEFT...HTBOTTOMRIGHT: mouse button was pressed down on the sizing border of window */  
                case HTLEFT:
                case HTRIGHT:
                case HTTOP:
                case HTBOTTOM:
                case HTTOPLEFT:
                case HTTOPRIGHT:
                case HTBOTTOMLEFT:
                case HTBOTTOMRIGHT:
                    /* record current window & mouse positions */
                    GetWindowRect(hwnd,&sizeWindowPosition);
                    sizeMousePosition.x=(int)(short)LOWORD(lParam);
                    sizeMousePosition.y=(int)(short)HIWORD(lParam);                   
                    break;                   
            }

            /* also let windows handle this event */
            return DefWindowProc(hwnd, message, wParam, lParam); 

        /* WM_MOVING: the window is about to be moved to a new location */
        case WM_MOVING:

            /* set new window position based on position of mouse */
            setNewWindowPosition((RECT*)lParam,&moveMouseOffset);

            /* snap the window to other windows if in range */
            snapInsideWindowByMoving(hDesktop,(RECT*)lParam);      
            snapWindowByMoving(OverviewWindow::hOverviewWindow,(RECT*)lParam);
            snapWindowByMoving(hToolWindow,(RECT*)lParam);            
            break;
    
        /* WM_SIZING: the window size is about to change */
        case WM_SIZING:

            /* set new window size based on position of mouse */
            setNewWindowSize((RECT*)lParam,&sizeWindowPosition,&sizeMousePosition,(int)wParam);

            /* snap the window to the edge of the desktop (if near it) */
            snapInsideWindowBySizing(hDesktop,(RECT*)lParam,(int)wParam);   
                        
            /* snap the window the main window (if near it) */
            snapWindowBySizing(OverviewWindow::hOverviewWindow,(RECT*)lParam,(int)wParam);           
            break;

        /* WM_SIZE: the window has been resized, minimized, or maximizsed, etc. */            
        case WM_SIZE:
           
            /* resize display/opengl window to fit new size */            
            GetClientRect(hImageWindow,&rect);
            MoveWindow(hImageWindowDisplay,rect.left,rect.top,rect.right,rect.bottom,true);
            
            /* update scroll bar settings */
            if (image_handler)
                updateImageScrollbar();
            return 0;
           
        case WM_SHOWWINDOW:
            /* update window menu item depending on whether window is shown or hidden */
            if (wParam)
                CheckMenuItem(OverviewWindow::hMainMenu,IDM_IMAGEWINDOW,MF_CHECKED|MF_BYCOMMAND);            
            else
                CheckMenuItem(OverviewWindow::hMainMenu,IDM_IMAGEWINDOW,MF_UNCHECKED|MF_BYCOMMAND);
            return 0;

        case WM_HSCROLL:
            scrollImageX(wParam);
            return 0;
            
        case WM_VSCROLL:
            scrollImageY(wParam);
            return 0;
            
        case WM_MOUSEWHEEL:
            zoomImage(GET_WHEEL_DELTA_WPARAM(wParam)/WHEEL_DELTA);
            return 0;


        /* WM_CLOSE: system or user has requested to close the window/application */             
        case WM_CLOSE:
            /* don't destroy this window, but make it invisible */
            ShowWindow(hwnd,SW_HIDE);
            return 0;

        /* WM_DESTORY: system is destroying our window */
        case WM_DESTROY:
            CheckMenuItem(OverviewWindow::hMainMenu,IDM_IMAGEWINDOW,MF_UNCHECKED|MF_BYCOMMAND);            
            return 0;
            
        default:
            /* let windows handle any unknown messages */
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    /* return 0 to indicate that we have processed the message */       
    return 0;
}
