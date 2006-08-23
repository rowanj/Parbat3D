
#include "Window.h"
#include "StickyWindowManager.h"
#include <vector>

using namespace std;

StickyWindowManager::StickyWindowManager()
{
    controller=NULL;
    hDesktop=GetDesktopWindow();
}


void StickyWindowManager::SetController(Window *controllerWin)
{
    controller=controllerWin;
}


void StickyWindowManager::AddStickyWindow(Window *sticky)
{
    stickyWindows.push_back(sticky);
}

void StickyWindowManager::beginMoving(Window *win)
{
    POINT cursor;
    RECT rect;
    
    GetCursorPos(&cursor);
    
    GetWindowRect(win->GetHandle(),&rect);
    mouseOffset.x=cursor.x-rect.left;
    mouseOffset.y=cursor.y-rect.top;    

}

void StickyWindowManager::onMoving(Window *win, RECT *rect)
{
    /*SnappingWindow::setNewWindowPosition((RECT*)lParam,&moveMouseOffset);
    
    SnappingWindow::snapInsideWindowByMoving(hDesktop,(RECT*)lParam);      
    SnappingWindow::snapWindowByMoving(overviewWindow.GetHandle(),(RECT*)lParam);
    SnappingWindow::snapWindowByMoving(toolWindow.GetHandle(),(RECT*)lParam); */
    
    POINT mouse; 
    
    /* get current mouse co-ords */
    GetCursorPos(&mouse);

    /* get width & height of window */
    rect->bottom-=rect->top;
    rect->right-=rect->left;
    
    /* set window position based on the cursor position */
    rect->top=mouse.y-mouseOffset.y;
    rect->left=mouse.x-mouseOffset.x;
    rect->bottom+=rect->top;
    rect->right+=rect->left;
    
    /* snap to other windows */
    moveToInsideOfWindow(hDesktop,rect); 
    int id=GetStickyWindowId(win);
    if (id!=-1)
    {
        int i;
        for  (i=0;i<stickyWindows.size();i++)
        {
            if (i!=id)
            {
                moveToOutsideOfWindow(stickyWindows.at(i)->GetHandle(),rect);
            }
        }
    }
}

int StickyWindowManager::GetStickyWindowId(Window *win)
{
    int i;
    for  (i=0;i<stickyWindows.size();i++)
    {
        if (stickyWindows.at(i)==win)
            return i;
    }  
    return -1;
}

int StickyWindowManager::moveToInsideOfWindow(HWND snapToWin,RECT *rect)
{
    const int SNAP_PIXEL_RANGE=10;
    RECT snapToRect;
    SIZE winsize={rect->right-rect->left,rect->bottom-rect->top};
    int difference;
    int isSnapped=false;
    int winState;
    
    // get position of the window that we may snap to
    GetWindowRect(snapToWin,&snapToRect);
    
    //check if window is close enough to snap to
    if ( (rect->left+winsize.cx<=(snapToRect.left-SNAP_PIXEL_RANGE)) || (rect->left>=(snapToRect.right+SNAP_PIXEL_RANGE)) )
        return false;

    if ( (rect->top+winsize.cy<=(snapToRect.top-SNAP_PIXEL_RANGE)) || (rect->top>=(snapToRect.bottom+SNAP_PIXEL_RANGE)) )
        return false;
            
    // snap (top) to top
    difference=(rect->top-(snapToRect.top));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->top=snapToRect.top;
       rect->bottom=rect->top+winsize.cy;
       isSnapped=true;       
    }      

    // snap (right) to right
    difference=(rect->left+winsize.cx-(snapToRect.right));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->left=snapToRect.right-winsize.cx;
       rect->right=rect->left+winsize.cx;       
       isSnapped=true;       
    }    
         
    // snap (bottom) to bottom
    difference=((rect->top+winsize.cy)-(snapToRect.bottom));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->top=snapToRect.bottom-winsize.cy;
       rect->bottom=rect->top+winsize.cy;
       isSnapped=true;
    } 
    
    // snap (left) to left
    difference=(rect->left-(snapToRect.left));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->left=snapToRect.left;
       rect->right=rect->left+winsize.cx;
       isSnapped=true;
    }        
    
}

/* snap a window to another window (by moving it) if it is range */
/* snapToWin=handle of window to snap to, rect=cords of window to be moved */
/* returns: true if window has been snapped, false if not */
int StickyWindowManager::moveToOutsideOfWindow(HWND snapToWin,RECT *rect)
{   
    const int SNAP_PIXEL_RANGE=10;
    RECT snapToRect;
    SIZE winsize={rect->right-rect->left,rect->bottom-rect->top};
    int difference;
    int isSnapped=false;
    int winState;

    // get position of the window that we may snap to
    GetWindowRect(snapToWin,&snapToRect);
    
    //check if window is close enough to snap to
    if ( (rect->left+winsize.cx<=(snapToRect.left-SNAP_PIXEL_RANGE)) || (rect->left>=(snapToRect.right+SNAP_PIXEL_RANGE)) )
        return false;

    if ( (rect->top+winsize.cy<=(snapToRect.top-SNAP_PIXEL_RANGE)) || (rect->top>=(snapToRect.bottom+SNAP_PIXEL_RANGE)) )
        return false;

    //snap (top) to bottom
    difference=(rect->top-(snapToRect.bottom));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->top=snapToRect.bottom;
       rect->bottom=rect->top+winsize.cy;
       isSnapped=true;
    }
       
    // snap (right) to left
    difference=((rect->left+winsize.cx)-(snapToRect.left));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->left=snapToRect.left-winsize.cx;
       rect->right=rect->left+winsize.cx;       
       isSnapped=true;       
    }     
  
    // snap (left) to right
    difference=(rect->left-(snapToRect.right));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->left=snapToRect.right;
       rect->right=rect->left+winsize.cx;       
       isSnapped=true;       
    }        

    // snap (bottom) to top
    difference=((rect->top+winsize.cy)-(snapToRect.top));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->top=snapToRect.top-winsize.cy;
       rect->bottom=rect->top+winsize.cy;
       isSnapped=true;
    }      
           
    return isSnapped;
}
