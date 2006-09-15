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
#include "ImageViewport.h"

#include "config.h"

int ImageWindow::Create(HWND parent)
{
    
    RECT rect;
    int mx,my;
    const int IMAGE_WINDOW_WIDTH=700;
    const int IMAGE_WINDOW_HEIGHT=600;

    /* Get the width & height of the desktop window */
    GetWindowRect(hDesktop,&rect);

    /* Get the stored window position or use defaults if there's a problem */
    mx = atoi(settingsFile->getSetting("overview window", "x").c_str());
	my = atoi(settingsFile->getSetting("overview window", "y").c_str());
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
	
	RegisterHotKey(GetHandle(), 100, NULL, VK_UP);
	RegisterHotKey(GetHandle(), 110, MOD_SHIFT, VK_UP);
	
	RegisterHotKey(GetHandle(), 200, NULL, VK_DOWN);
	RegisterHotKey(GetHandle(), 210, MOD_SHIFT, VK_DOWN);
	
	RegisterHotKey(GetHandle(), 300, NULL, VK_LEFT);
	RegisterHotKey(GetHandle(), 310, MOD_SHIFT, VK_LEFT);
	
	RegisterHotKey(GetHandle(), 400, NULL, VK_RIGHT);
	RegisterHotKey(GetHandle(), 410, MOD_SHIFT, VK_RIGHT);
	
	
    return true;
}

/* display image information in image window's title bar */
void ImageWindow::updateImageWindowTitle()
{
    /* Display the file name & zoom level on the image window title bar */
    string leader = "Image - ";
    string title  = makeMessage(leader,filename);
    title+=makeString(" (",image_handler->get_image_viewport()->get_zoom_level()*100.0);
    title+="%)";
	SetWindowText(GetHandle(), (char*) title.c_str());
}

/* update image window's scroll bar display settings  */
void ImageWindow::updateImageScrollbar()
{
    int LOD_width,LOD_height;
    ImageViewport* viewport;
    viewport = image_handler->get_image_viewport();

    SCROLLINFO info_x,info_y;

    /* get window size and image size at current zoom level */   
    info_x.nPage=viewport->get_window_width();
    info_y.nPage=viewport->get_window_height();    
    LOD_width=viewport->get_zoom_image_width();
    LOD_height=viewport->get_zoom_image_height();
    
    /* set scroll range */
    info_x.nMin=0;
    info_y.nMin=0;
    info_x.nMax=LOD_width;// - info_x.nPage;
    info_y.nMax=LOD_height;// - info_y.nPage;
    
    /* set scroll position */
    info_x.nPos=viewport->get_zoom_x();
    info_y.nPos=viewport->get_zoom_y();
    
    #if DEBUG_SCROLLING
    Console::write("updateScrollbarSettings():\n");
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
    #endif
    
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
	ImageViewport* viewport;
    viewport = image_handler->get_image_viewport();
    
    /* get current scroll position & range */    
    info.cbSize=sizeof(SCROLLINFO);
    info.fMask=SIF_ALL;
    GetScrollInfo(GetHandle(),SB_HORZ,&info);
     
    Console::write("scrollImageX() ");
    Console::write("\n");
    Console::write(scrollMsg);
    
    switch(LOWORD(scrollMsg))
    {
        case SB_LINEUP:
            info.nPos--;
            break;
        case SB_LINEDOWN:
            info.nPos++;
            break;
        case SB_PAGEUP:
            info.nPos-=viewport->get_viewport_width();
            break;
        case SB_PAGEDOWN:
            info.nPos+=viewport->get_viewport_width();                
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

    // update scroll position
    info.fMask=SIF_POS;
    SetScrollInfo(GetHandle(),SB_HORZ,&info,true);
    viewport->set_zoom_x(info.nPos);
}

/* scroll image window vertically */
void ImageWindow::scrollImageY(int scrollMsg)
{
    SCROLLINFO info;
	ImageViewport* viewport;
    viewport = image_handler->get_image_viewport();
    
    /* get current scroll position & range */    
    info.cbSize=sizeof(SCROLLINFO);
    info.fMask=SIF_ALL;
    GetScrollInfo(GetHandle(),SB_VERT,&info);
     
    Console::write("scrollImageY() ");
    Console::write("\n");
    Console::write(scrollMsg);
    
    switch(LOWORD(scrollMsg))
    {
        case SB_LINEUP:
            info.nPos--;
            break;
        case SB_LINEDOWN:
            info.nPos++;
            break;
        case SB_PAGEUP:
            info.nPos-=viewport->get_viewport_height();
            break;
        case SB_PAGEDOWN:
            info.nPos+=viewport->get_viewport_height();                
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
    Console::write(info.nPos);
    Console::write("\n");
    viewport->set_zoom_y(info.nPos);
}

/* zoom the image in/out */
void ImageWindow::zoomImage(int nlevels)
{
    float zoom;
 
    Console::write("zoomImage() nlevels=");   
    Console::write(nlevels);
    Console::write("\n");
    zoom=image_handler->get_image_viewport()->get_zoom_level();
    // Sometimes the zoom should be a little more sensitive
    if (zoom >= 0.5) nlevels = nlevels * 2;
    if (zoom >= 1.0) nlevels = nlevels * 2;
    zoom+=nlevels/100.0;
    if (zoom>2.0)
        zoom=2.0;
    else if (zoom<0.01) // a per-image minimum is calculated in ImageViwport
        zoom=0.01;
    image_handler->get_image_viewport()->set_zoom_level(zoom);
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
        /* WM_NCLBUTTONDOWN: mouse button was pressed down in a non client area of the window         
        case WM_NCLBUTTONDOWN:

            switch(wParam)
            {
                // HTCAPTION: mouse button was pressed down on the window title bar                             
                case HTCAPTION:
                    // get the mouse co-ords relative to the window
                    SnappingWindow::getMouseWindowOffset(hwnd,(int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam),&moveMouseOffset);               
                    break;

                // HTLEFT...HTBOTTOMRIGHT: mouse button was pressed down on the sizing border of window
                case HTLEFT:
                case HTRIGHT:
                case HTTOP:
                case HTBOTTOM:
                case HTTOPLEFT:
                case HTTOPRIGHT:
                case HTBOTTOMLEFT:
                case HTBOTTOMRIGHT:
                    // record current window & mouse positions //
                    GetWindowRect(hwnd,&sizeWindowPosition);
                    sizeMousePosition.x=(int)(short)LOWORD(lParam);
                    sizeMousePosition.y=(int)(short)HIWORD(lParam);                   
                    break;                   
            }

            // also let windows handle this event
            return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
        */

        /* WM_SIZING: the window size is about to change */
        case WM_SIZING:

            // set new window size based on position of mouse
            //SnappingWindow::setNewWindowSize((RECT*)lParam,&sizeWindowPosition,&sizeMousePosition,(int)wParam);

            /* prevent window from being resized too small */
            if ( (((RECT*)lParam)->bottom) - (((RECT*)lParam)->top)  <100)
                (((RECT*)lParam)->bottom) = (((RECT*)lParam)->top) + 100;
            if ( (((RECT*)lParam)->right) - (((RECT*)lParam)->left)  <100)
                (((RECT*)lParam)->right) = (((RECT*)lParam)->left) + 100;
            
            /* snap the window to the edge of the desktop (if near it) */
           // SnappingWindow::snapInsideWindowBySizing(hDesktop,(RECT*)lParam,(int)wParam);   
                        
            /* snap the window the main window (if near it) */
            //SnappingWindow::snapWindowBySizing(overviewWindow.GetHandle(),(RECT*)lParam,(int)wParam);           
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
            
        case WM_HOTKEY:
			// scroll up small
			if (LOWORD(wParam) == 100)
			{
				win->scrollImageY(0);
				win->scrollImageY(8);
			}
			// scroll up big
			if (LOWORD(wParam) == 110)
			{
				win->scrollImageY(2);
				win->scrollImageY(8);
			}
			// scroll down small
			if (LOWORD(wParam) == 200)
			{
				win->scrollImageY(1);
				win->scrollImageY(8);
			}
			// scroll down big
			if (LOWORD(wParam) == 210)
			{
				win->scrollImageY(3);
				win->scrollImageY(8);
			}
			// scroll left small
			if (LOWORD(wParam) == 300)
			{
				win->scrollImageX(0);
				win->scrollImageX(8);
			}
			
			//scroll left big
			if (LOWORD(wParam) == 310)
			{
				win->scrollImageX(2);
				win->scrollImageX(8);
			}
			
			// scroll right small
			if (LOWORD(wParam) == 400)
			{
				win->scrollImageX(1);
				win->scrollImageX(8);
			}
			// scroll right big
			if (LOWORD(wParam) == 410)
			{
				win->scrollImageX(3);
				win->scrollImageX(8);
			}
			return 0;


        /* WM_CLOSE: system or user has requested to close the window/application */             
        case WM_CLOSE:
            /* remove the check in the menu */
            //CheckMenuItem(overviewWindow.hMainMenu,IDM_IMAGEWINDOW,MF_UNCHECKED|MF_BYCOMMAND);
            
            /* don't destroy this window, but make it invisible */
            ShowWindow(hwnd,SW_HIDE);
            return 0;
            
        /* WM_DESTORY: system is destroying our window */
        case WM_DESTROY:
            /* save the image window attributes */
            RECT *image_window_rect;
            GetWindowRect(hwnd, image_window_rect);
            if (image_window_rect != NULL) {
                settingsFile->setSetting("image window", "x", image_window_rect->left);
                settingsFile->setSetting("image window", "y", image_window_rect->top);
                settingsFile->setSetting("image window", "width", (image_window_rect->right-image_window_rect->left));
                settingsFile->setSetting("image window", "height", (image_window_rect->bottom-image_window_rect->top));
    //            settingsFile->setSetting("image window", "maximised", 0);
    //            settingsFile->setSetting("image window", "snapped", 0);
            }
            break;
            
        default:
            /* let windows handle any unknown messages */
            return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
    }
    
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);
}
