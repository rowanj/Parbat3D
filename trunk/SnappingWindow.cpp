
#include <Windows.h>
#include "main.h"
#include "SnappingWindow.h"
#include "ImageWindow.h"
#include "ToolWindow.h"
#include "ROIWindow.h"
//#include "console.h"

/* calculates window's new position based on the location of the mouse cursor */
void SnappingWindow::setNewWindowPosition(RECT* newPos,POINT *mouseOffset)
{
    POINT mouse;    /* mouse co-ords */
    
    /* get current mouse co-ords */
    GetCursorPos(&mouse);

    /* get width & height of window */
    newPos->bottom-=newPos->top;
    newPos->right-=newPos->left;
    
    /* set window position based on the cursor position */
    newPos->top=mouse.y-mouseOffset->y;
    newPos->left=mouse.x-mouseOffset->x;
    newPos->bottom+=newPos->top;
    newPos->right+=newPos->left;                
}

/* calculates window's new size based on location of the mouse cursor */
void SnappingWindow::setNewWindowSize(RECT* newPos,RECT* oldPos,POINT* oldMouse,int whichDirection)
{
    POINT newMouse;    /* mouse co-ords */
    SIZE size;
    
    /* get current mouse co-ords */
    GetCursorPos(&newMouse);

    /* calculate how much mouse has moved */
    int my=newMouse.y-oldMouse->y;
    int mx=newMouse.x-oldMouse->x;

    /* set window position based on the cursor position & which border is being dragged */
    switch (whichDirection)
    {
        case WMSZ_BOTTOM:
            newPos->bottom=oldPos->bottom+my;
            break;
        case WMSZ_TOP:
            newPos->top=oldPos->top+my;
            break;
        case WMSZ_LEFT:
            newPos->left=oldPos->left+mx;
            break;
        case WMSZ_RIGHT:
            newPos->right=oldPos->right+mx;
            break;
        case WMSZ_TOPLEFT:
            newPos->top=oldPos->top+my;
            newPos->left=oldPos->left+mx;
            break;
        case WMSZ_TOPRIGHT:
            newPos->top=oldPos->top+my;
            newPos->right=oldPos->right+mx;
            break;
        case WMSZ_BOTTOMLEFT:
            newPos->bottom=oldPos->bottom+my;
            newPos->left=oldPos->left+mx;
            break;
        case WMSZ_BOTTOMRIGHT:
            newPos->bottom=oldPos->bottom+my;
            newPos->right=oldPos->right+mx;
            break;                        

    }   
}    

/* move a window by a certain amount of pixels */
void SnappingWindow::moveWindowByOffset(HWND hwnd,RECT *rect,int leftOffset,int topOffset)
{
    MoveWindow(hwnd,rect->left+leftOffset,rect->top+topOffset,rect->right-rect->left,rect->bottom-rect->top,true);
}

/* move snapped windows with the main window */
void SnappingWindow::moveSnappedWindows(RECT *newRect,RECT *oldRect,RECT *prevImageWindowRect,RECT *prevToolWindowRect,RECT *prevROIWindowRect,int moveImageWindow,int moveToolWindow,int moveROIWindow)
{
    int moveLeftOffset=newRect->left-oldRect->left;
    int moveTopOffset=newRect->top-oldRect->top;
    if (moveImageWindow)
        moveWindowByOffset(imageWindow.GetHandle(),prevImageWindowRect,moveLeftOffset,moveTopOffset);
    if (moveToolWindow)
        moveWindowByOffset(toolWindow.GetHandle(),prevToolWindowRect,moveLeftOffset,moveTopOffset);
	if (moveROIWindow)
        moveWindowByOffset(roiWindow.GetHandle(),prevROIWindowRect,moveLeftOffset,moveTopOffset);
}

/* checks whether a window is in normal state (not hidden, minimized, or maximized) */
int SnappingWindow::isWindowInNormalState(HWND hwnd)
{
    /* check if window is hidden,  maximized, or minimized */
    if ((!IsWindowVisible(hwnd)) || (IsZoomed(hwnd)) || (IsIconic(hwnd)))
        return false;

    return true;
}    

int SnappingWindow::snapInsideWindowByMoving(HWND snapToWin,RECT *rect)
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
int SnappingWindow::snapWindowByMoving(HWND snapToWin,RECT *rect)
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

/* snap a window to another window (by resizing it) if it is range */
/*  snapToWin=handle of window to snap to, rect=cords of window to be resized */
/* returns: true if window has been snapped, false if not */
int SnappingWindow::snapWindowBySizing(HWND snapToWin,RECT *rect,int whichDirection)
{
    const int SNAP_PIXEL_RANGE=10;
    RECT snapToRect;
    int difference;
    int isSnapped=false;

    // get position of the window that we may snap to
    GetWindowRect(snapToWin,&snapToRect);
    
    //check if window is close enough to snap to
    if ( (rect->right<(snapToRect.left-SNAP_PIXEL_RANGE)) || (rect->left>(snapToRect.right+SNAP_PIXEL_RANGE)) )
        return false;

    if ( (rect->bottom<(snapToRect.top-SNAP_PIXEL_RANGE)) || (rect->top>(snapToRect.bottom+SNAP_PIXEL_RANGE)) )
        return false;
        
    if ((whichDirection==WMSZ_TOP) || (whichDirection==WMSZ_TOPLEFT) || (whichDirection==WMSZ_TOPRIGHT))
    {
        //snap (top) to bottom
        difference=(rect->top-(snapToRect.bottom));
        if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
        {
           rect->top=snapToRect.bottom;
           isSnapped=true;
        }
    }
    
    if ((whichDirection==WMSZ_BOTTOM) || (whichDirection==WMSZ_BOTTOMLEFT) || (whichDirection==WMSZ_BOTTOMRIGHT))
    {    

        // snap (bottom) to top
        difference=((rect->bottom)-(snapToRect.top));
        if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
        {
           rect->bottom=snapToRect.top;
           isSnapped=true;
        }          
    }
    
    if ((whichDirection==WMSZ_LEFT) || (whichDirection==WMSZ_BOTTOMLEFT) || (whichDirection==WMSZ_TOPLEFT))
    {    

        // snap (left) to right
        difference=(rect->left-(snapToRect.right));
        if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
        {
           rect->left=snapToRect.right;
           isSnapped=true;       
        }           
    }
    

    if ((whichDirection==WMSZ_RIGHT) || (whichDirection==WMSZ_BOTTOMRIGHT) || (whichDirection==WMSZ_TOPRIGHT))
    { 
    
        // snap (right) to left
        difference=((rect->right)-(snapToRect.left));
        if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
        {
           rect->right=snapToRect.left;       
           isSnapped=true;       
        }    
    
        // snap (right) to right
        difference=(rect->right-(snapToRect.right));
        if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
        {
           rect->right=snapToRect.right;       
           isSnapped=true;       
        }    
    }
    return isSnapped;
}


int SnappingWindow::snapInsideWindowBySizing(HWND snapToWin,RECT *rect,int whichDirection)
{
    const int SNAP_PIXEL_RANGE=10;
    RECT snapToRect;
    int difference;
    int isSnapped=false;

    // get position of the window that we may snap to
    GetWindowRect(snapToWin,&snapToRect);
    
    //check if window is close enough to snap to
    if ( (rect->right<(snapToRect.left-SNAP_PIXEL_RANGE)) || (rect->left>(snapToRect.right+SNAP_PIXEL_RANGE)) )
        return false;

    if ( (rect->bottom<(snapToRect.top-SNAP_PIXEL_RANGE)) || (rect->top>(snapToRect.bottom+SNAP_PIXEL_RANGE)) )
        return false;
        
    if ((whichDirection==WMSZ_TOP) || (whichDirection==WMSZ_TOPLEFT) || (whichDirection==WMSZ_TOPRIGHT))
    {
        // snap (top) to top
        difference=(rect->top-(snapToRect.top));
        if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
        {
           rect->top=snapToRect.top;
           isSnapped=true;       
        }         
    }
    
    if ((whichDirection==WMSZ_BOTTOM) || (whichDirection==WMSZ_BOTTOMLEFT) || (whichDirection==WMSZ_BOTTOMRIGHT))
    {    
        // snap (bottom) to bottom
        difference=((rect->bottom)-(snapToRect.bottom));
        if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
        {
           rect->bottom=snapToRect.bottom;
           isSnapped=true;
        } 

    }
    
    if ((whichDirection==WMSZ_LEFT) || (whichDirection==WMSZ_BOTTOMLEFT) || (whichDirection==WMSZ_TOPLEFT))
    {    
        // snap (left) to left
        difference=(rect->left-(snapToRect.left));
        if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
        {
           rect->left=snapToRect.left;
           isSnapped=true;
        }
    }
    

    if ((whichDirection==WMSZ_RIGHT) || (whichDirection==WMSZ_BOTTOMRIGHT) || (whichDirection==WMSZ_TOPRIGHT))
    { 
    
        // snap (right) to right
        difference=(rect->right-(snapToRect.right));
        if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
        {
           rect->right=snapToRect.right;       
           isSnapped=true;       
        }    
    }
    return isSnapped;
}


//calculate mouse co-ords relative to position of window
void SnappingWindow::getMouseWindowOffset(HWND hwnd,int mx,int my,POINT *mouseOffset)
{
    RECT win;
    GetWindowRect(hwnd,&win);
    mouseOffset->x=mx-win.left;
    mouseOffset->y=my-win.top;
}


/* calculates whether a window is connected to another by its border */
int SnappingWindow::isWindowSnapped(HWND main,HWND sticky)
{
    RECT rmain,rsticky;
    

    GetWindowRect(sticky,&rsticky);
    GetWindowRect(main,&rmain);    

    // check if sticky window co-ords are outside of the main window's co-ords
    if ((rmain.top>rsticky.bottom) || (rmain.bottom<rsticky.top) ||
            ((rmain.left>rsticky.right) || (rmain.right<rsticky.left)) )
        return false;    
    
    // check if the borders of the two windows are tuching
    if ((rmain.right==rsticky.left) || (rmain.left==rsticky.right) ||
            (rmain.top==rsticky.bottom) || (rmain.bottom==rsticky.top))
        return true;
    return false;
}   
 
