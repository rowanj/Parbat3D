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



int ImageWindow::Create(HWND parent)
{
    
    RECT rect;
    int mx,my;
    const int IMAGE_WINDOW_WIDTH=700;
    const int IMAGE_WINDOW_HEIGHT=600;

    /* Get the width & height of the desktop window */
    GetWindowRect(hDesktop,&rect);

    /* Get the stored window position or use defaults if there's a problem */
    mx = atoi(settingsFile.getSetting("overview window", "x").c_str());
	my = atoi(settingsFile.getSetting("overview window", "y").c_str());
	if ((mx <= 0) || (mx >= rect.right))
        mx = (rect.right /2) - ((IMAGE_WINDOW_WIDTH+OverviewWindow::OVERVIEW_WINDOW_WIDTH) /2);         /* default x position to center windows */
    mx+=OverviewWindow::OVERVIEW_WINDOW_WIDTH;                                                          /* leave room for overview window */
	       
	if ((my <= 0) || (my >= rect.bottom))
       my = (rect.bottom /2) - (IMAGE_WINDOW_HEIGHT/2);                                 /* default y position to center windows */

    /* create image window */
    if (!CreateWin(0, "Parbat3D Image Window", "Image Window",
	     WS_POPUP+WS_SYSMENU+WS_CAPTION+WS_MAXIMIZEBOX+WS_VSCROLL+WS_HSCROLL+WS_SIZEBOX,
	     mx, my, IMAGE_WINDOW_WIDTH, IMAGE_WINDOW_HEIGHT, parent, NULL))
	    return false;

    prevProc=SetWindowProcedure(&WindowProcedure);

    /* create a child window that will be used by OpenGL */
    imageWindowDisplay.Create(GetHandle());

    return true;
}

/* display image information in image window's title bar */
void ImageWindow::updateImageWindowTitle()
{
    /* Display the file name & zoom level on the image window title bar */
    string leader = "Image - ";
    string title  = makeMessage(leader,filename);
    title+=makeString(" (",int(100.0 / pow((double)2,(double)image_handler->get_LOD())));
    title+="%)";
	SetWindowText(GetHandle(), (char*) title.c_str());
}

/* update image window's scroll bar display settings  */
void ImageWindow::updateImageScrollbar()
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
    SetScrollInfo(GetHandle(),SB_HORZ,&info_x,true);
    SetScrollInfo(GetHandle(),SB_VERT,&info_y,true);    
      
}

/* scroll image window horizontally */
void ImageWindow::scrollImageX(int scrollMsg)
{
    SCROLLINFO info;

    /* get current scroll position & range */    
    info.cbSize=sizeof(SCROLLINFO);
    info.fMask=SIF_ALL;
    GetScrollInfo(GetHandle(),SB_HORZ,&info);
     
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
    SetScrollInfo(GetHandle(),SB_HORZ,&info,true);
    image_handler->set_viewport_x(info.nPos);
}

/* scroll image window vertically */
void ImageWindow::scrollImageY(int scrollMsg)
{
    SCROLLINFO info;

    /* get current scroll position & range */    
    info.cbSize=sizeof(SCROLLINFO);
    info.fMask=SIF_ALL;
    GetScrollInfo(GetHandle(),SB_VERT,&info);
     
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
    SetScrollInfo(GetHandle(),SB_VERT,&info,true);
    image_handler->set_viewport_y(info.nPos);
}

/* zoom the image in/out */
void ImageWindow::zoomImage(int nlevels)
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

/* All messages/events related to the image window (or it's controls) are sent to this procedure */
LRESULT CALLBACK ImageWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static POINT moveMouseOffset;     /* mouse offset relative to window, used for snapping */
    static POINT sizeMousePosition;   /* mouse position, used for sizing window */
    static RECT  sizeWindowPosition;  /* window position, used for sizing window */
    static RECT rect;                 /* for general use */

    ImageWindow* win=(ImageWindow*)Window::GetWindowObject(hwnd);
    
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
        case WM_SIZING:

            /* set new window size based on position of mouse */
            SnappingWindow::setNewWindowSize((RECT*)lParam,&sizeWindowPosition,&sizeMousePosition,(int)wParam);

            /* prevent window from being resized too small */
            if ( (((RECT*)lParam)->bottom) - (((RECT*)lParam)->top)  <100)
                (((RECT*)lParam)->bottom) = (((RECT*)lParam)->top) + 100;
            if ( (((RECT*)lParam)->right) - (((RECT*)lParam)->left)  <100)
                (((RECT*)lParam)->right) = (((RECT*)lParam)->left) + 100;
            
            /* snap the window to the edge of the desktop (if near it) */
            SnappingWindow::snapInsideWindowBySizing(hDesktop,(RECT*)lParam,(int)wParam);   
                        
            /* snap the window the main window (if near it) */
            SnappingWindow::snapWindowBySizing(overviewWindow.GetHandle(),(RECT*)lParam,(int)wParam);           
            break;

        /* WM_SIZE: the window has been resized, minimized, or maximizsed, etc. */            
        case WM_SIZE:
           
            /* resize display/opengl window to fit new size */            
            GetClientRect(hwnd,&rect);
            MoveWindow(win->imageWindowDisplay.GetHandle(),rect.left,rect.top,rect.right,rect.bottom,true);
            
            /* update scroll bar settings */
            if (image_handler)
                win->updateImageScrollbar();
            return 0;
           
        case WM_SHOWWINDOW:
            /* update window menu item depending on whether window is shown or hidden */
            if (wParam)
                CheckMenuItem(overviewWindow.hMainMenu,IDM_IMAGEWINDOW,MF_CHECKED|MF_BYCOMMAND);            
            else
                CheckMenuItem(overviewWindow.hMainMenu,IDM_IMAGEWINDOW,MF_UNCHECKED|MF_BYCOMMAND);
            return 0;

        case WM_HSCROLL:
            win->scrollImageX(wParam);
            return 0;
            
        case WM_VSCROLL:
            win->scrollImageY(wParam);
            return 0;
            
        case WM_MOUSEWHEEL:
            win->zoomImage(GET_WHEEL_DELTA_WPARAM(wParam)/WHEEL_DELTA);
            return 0;


        /* WM_CLOSE: system or user has requested to close the window/application */             
        case WM_CLOSE:
            /* don't destroy this window, but make it invisible */
            ShowWindow(hwnd,SW_HIDE);
            return 0;

        /* WM_DESTORY: system is destroying our window */
        case WM_DESTROY:
            CheckMenuItem(overviewWindow.hMainMenu,IDM_IMAGEWINDOW,MF_UNCHECKED|MF_BYCOMMAND);            
            return 0;
            
        default:
            /* let windows handle any unknown messages */
            return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
    }
    /* return 0 to indicate that we have processed the message */       
    return 0;
}
