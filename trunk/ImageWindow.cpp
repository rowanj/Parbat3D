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

char* imageWindowTitle;

int ImageWindow::Create(HWND parent) {
    RECT desktopRect;
    int mx, my, mwidth, mheight;
    const int IMAGE_WINDOW_WIDTH = 700;
    const int IMAGE_WINDOW_HEIGHT = 665;
    imageWindowTitle = new char[256];
    
    /* Get the width & height of the desktop window */
    GetWindowRect(hDesktop,&desktopRect);
    
    /* Get the stored window position or use defaults if there's a problem */
    mx = atoi(settingsFile->getSetting("image window", "x").c_str());
    my = atoi(settingsFile->getSetting("image window", "y").c_str());
    mwidth = atoi(settingsFile->getSetting("image window", "width").c_str());
    mheight = atoi(settingsFile->getSetting("image window", "height").c_str());
    
    if ((mx <= 0) || (mx > (desktopRect.right-50))) {
        mx = (desktopRect.right /2) - ((IMAGE_WINDOW_WIDTH+OverviewWindow::WIDTH) /2);  // default x position to center windows
        mx += OverviewWindow::WIDTH;                             // leave room for overview window
    }
    
    if ((my <= 0) || (my > (desktopRect.bottom-50))) {
        my = (desktopRect.bottom /2) - (IMAGE_WINDOW_HEIGHT/2);  // default y position to center windows
    }
    
    // use defaults if window size is too small
    if (mwidth <= 50)                   // check width
        mwidth = IMAGE_WINDOW_WIDTH;
    if (mheight <= 50)                  // check height
        mheight = IMAGE_WINDOW_HEIGHT;
	
	// if image window is too big to fit on screen, set it to the default size
	if ((mwidth>desktopRect.right)||(mheight>desktopRect.bottom)) {
		mwidth=IMAGE_WINDOW_WIDTH;
		mheight=IMAGE_WINDOW_HEIGHT;		
	}
	
    /* create image window */
    if (!CreateWin(0, "Parbat3D Image Window", "Image Window",
	     WS_POPUP+WS_SYSMENU+WS_CAPTION+WS_MAXIMIZEBOX+WS_VSCROLL+WS_HSCROLL+WS_SIZEBOX,
	     mx, my, mwidth, mheight, parent, NULL))
	    return false;
	
    prevProc=SetWindowProcedure(&WindowProcedure);
    
    
    // create a child window that will be used by OpenGL
    imageWindowDisplay.Create(GetHandle());
	
    return true;
}


/* display image information in image window's title bar */
void ImageWindow::updateImageWindowTitle() {
    /* Display the file name & zoom level on the image window title bar */
    sprintf(imageWindowTitle, "Image - %s (%.2f%%)", filename, image_handler->get_image_viewport()->get_zoom_level()*100.0);
    SetWindowText(GetHandle(), imageWindowTitle);
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
     
    //Console::write("scrollImageX() ");
    //Console::write("\n");
    //Console::write(scrollMsg);
    
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
void ImageWindow::scrollImageY(int scrollMsg) {
    SCROLLINFO info;
	ImageViewport* viewport;
    viewport = image_handler->get_image_viewport();
    
    /* get current scroll position & range */    
    info.cbSize=sizeof(SCROLLINFO);
    info.fMask=SIF_ALL;
    GetScrollInfo(GetHandle(),SB_VERT,&info);
     
    //Console::write("scrollImageY() ");
    //Console::write("\n");
    //Console::write(scrollMsg);
    
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

void ImageWindow::scrollImage(bool vert, int amount) {
    SCROLLINFO info;
	ImageViewport* viewport;
    viewport = image_handler->get_image_viewport();
    
    // get current scroll position & range
    info.cbSize=sizeof(SCROLLINFO);
    info.fMask=SIF_ALL;
    if (vert) GetScrollInfo(GetHandle(),SB_VERT,&info);
    else GetScrollInfo(GetHandle(),SB_HORZ,&info);
    
    // move the desired amount
    info.nPos += amount;
    
    // check new position is within scroll range
    if (info.nPos<info.nMin)
        info.nPos=info.nMin;
    else if (info.nPos>(info.nMax-info.nPage))
        info.nPos=info.nMax-info.nPage;
    
    // update scroll position
    info.fMask=SIF_POS;
    if (vert) {
        SetScrollInfo(GetHandle(),SB_VERT,&info,true);
        viewport->set_zoom_y(info.nPos);
    } else {
        SetScrollInfo(GetHandle(),SB_HORZ,&info,true);
        viewport->set_zoom_x(info.nPos);
    }
}


/* zoom the image in/out */
void ImageWindow::zoomImage(float nlevels)
{
    float zoom=image_handler->get_image_viewport()->get_zoom_level();
    float increment = ((float)nlevels * zoom) / 50.0;

    Console::write("zoomImage() nlevels=%d, increment=%f, new_zoom=%f\n", nlevels, increment, zoom);
    image_handler->get_image_viewport()->set_zoom_level(zoom+increment);

    updateImageWindowTitle();
    updateImageScrollbar();
}

// handle shortcut key events
void ImageWindow::onKeyDown(int virtualKey)
{
	// check whether shift is currently down
	bool shift_pressed=(bool)(GetKeyState(VK_SHIFT)&128);
	
	// scroll up small
	if (virtualKey == VK_UP)
	{
		scrollImageY(SB_LINEUP);
	}
	// scroll up big
	if (virtualKey == VK_UP && shift_pressed)
	{
		scrollImageY(SB_PAGEUP);
	}
	// scroll down small
	if (virtualKey == VK_DOWN)
	{
		scrollImageY(SB_LINEDOWN);		
	}
	// scroll down big
	if (virtualKey == VK_DOWN && shift_pressed)
	{
		scrollImageY(SB_PAGEDOWN);		
	}
	// scroll left small
	if (virtualKey == VK_LEFT)
	{
		scrollImageX(SB_LINEUP);
	}
	
	//scroll left big
	if (virtualKey == VK_LEFT && shift_pressed)
	{
		scrollImageX(SB_PAGEUP);
	}
	
	// scroll right small
	if (virtualKey == VK_RIGHT)
	{
		scrollImageX(SB_LINEDOWN);
	}
	// scroll right big
	if (virtualKey == VK_RIGHT && shift_pressed)
	{
		scrollImageX(SB_PAGEDOWN);
	}
	
	// Zoom in
	if (virtualKey == VK_PRIOR)
	{
		if (shift_pressed)
			zoomImage(0.1);
		else
			zoomImage(1);
	}
	
	// zoom out
	if (virtualKey == VK_NEXT)
	{
		if (shift_pressed)
			zoomImage(-0.1);
		else
			zoomImage(-1);
	}	
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
            
        case WM_KEYDOWN:
			win->onKeyDown(wParam);
			break;

        /* WM_CLOSE: system or user has requested to close the window/application */             
        case WM_CLOSE:
            /* remove the check in the menu */
            //CheckMenuItem(overviewWindow.hMainMenu,IDM_IMAGEWINDOW,MF_UNCHECKED|MF_BYCOMMAND);
            
            /* don't destroy this window, but make it invisible */
            ShowWindow(hwnd,SW_HIDE);
            return 0;
            
        /* WM_DESTORY: system is destroying our window */
        case WM_DESTROY:
//			Console::write("ImageWindow WM_DESTROY");
            /* save the image window attributes */
            RECT image_window_rect;
            GetWindowRect(hwnd, &image_window_rect);
            settingsFile->setSetting("image window", "x", image_window_rect.left);
            settingsFile->setSetting("image window", "y", image_window_rect.top);
            settingsFile->setSetting("image window", "width", (image_window_rect.right-image_window_rect.left));
            settingsFile->setSetting("image window", "height", (image_window_rect.bottom-image_window_rect.top));
    //            settingsFile->setSetting("image window", "maximised", 0);
    //            settingsFile->setSetting("image window", "snapped", 0);
            delete[] imageWindowTitle;
            break;
            
        default:
            /* let windows handle any unknown messages */
            return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
    }
    
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);
}

        
int ImageWindow::getWidth () {
    RECT rect;
    GetWindowRect(GetHandle(), &rect);
    return (rect.right - rect.left);
}

int ImageWindow::getHeight () {
    RECT rect;
    GetWindowRect(GetHandle(), &rect);
    return (rect.bottom - rect.top);
}
