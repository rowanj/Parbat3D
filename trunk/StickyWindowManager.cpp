
#include "Window.h"
#include "StickyWindowManager.h"
#include "console.h"
#include "config.h"
#include "main.h"
#include <queue>
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


void StickyWindowManager::RemoveStickyWindow(Window *sticky)
{
    vector<Window*>::iterator i;
    for (i=stickyWindows.begin();i<stickyWindows.end();i++)
    {
        if (*i==sticky)
        {
             stickyWindows.erase(i);
             break;
        }
    }

}

void StickyWindowManager::BeginMoving(Window *win)
{

    #if DEBUG_STICKY_WINDOW_MANAGER
    Console::write("StickyWindowManager::BeginMoving\n");
    #endif
        
    GetCursorPos(&prevMousePosition);           // save current mouse position
    
    if (controller==win)
    {
        // calculate which windows are directly connected to each other &
        char *connection=(char*)malloc(sizeof(char)*stickyWindows.size()*stickyWindows.size());
        int i,j,k;
        for (i=0;i<stickyWindows.size();i++)
        {
            for (j=i+1;j<stickyWindows.size();j++)
            {
                char isCon;
                isCon=isConnected(stickyWindows.at(i),stickyWindows.at(j));
                connection[(j*stickyWindows.size())+i]=isCon;
                connection[(i*stickyWindows.size())+j]=isCon;
            }
        }
        
        // calculate which windows are directly connected to the controller
        char *snapped=(char*)malloc(sizeof(char)*stickyWindows.size());
        queue<int> snappedIdx;
        for (i=0;i<stickyWindows.size();i++)
        {
            int isCon;
            isCon=isConnected(stickyWindows.at(i),controller);
            snapped[i]=isCon;
            if (isCon==true)
                snappedIdx.push(i);
        }
        
        // calculate which windows are indirectly connected to the controller
        while (!snappedIdx.empty())
        {
            int idx=(int)snappedIdx.front();
            snappedIdx.pop();
            for (j=0;j<stickyWindows.size();j++)
            {
                if ((j!=idx) && (connection[(idx*stickyWindows.size())+j]) && (snapped[j]==false))
                {
                    snapped[j]=true;
                    snappedIdx.push(j);
                }
            }
        }
        snappedWindows.clear();
        nonSnappedWindows.clear();        
        for (i=0;i<stickyWindows.size();i++)
        {
            if (snapped[i])
            {
                #if DEBUG_STICKY_WINDOW_MANAGER
                char *name="Unknown";
                if (stickyWindows.at(i)==&prefsWindow)
                {
                    name="prefsWindow";
                }
                else if (stickyWindows.at(i)==&overviewWindow)
                {
                    name="overviewWindow";
                }                
                else if (stickyWindows.at(i)==&imageWindow)
                {
                    name="imageWindow";
                }                                
                    Console::write(name);
                    Console::write(" is snapped\n");                    
                #endif
                
                snappedWindows.push_back(stickyWindows.at(i));
            }
            else
            {
                nonSnappedWindows.push_back(stickyWindows.at(i));
            }
        }
        
        free(snapped);        
        free(connection);
    }

}

void StickyWindowManager::OnMoving(Window *win, RECT *rect)
{   
    POINT mouse,mouseOffset;
    POINT originalPos;
    
    // get current mouse co-ords
    GetCursorPos(&mouse);
    
    // calculate how much the mouse has moved
    mouseOffset.x=mouse.x-prevMousePosition.x;
    mouseOffset.y=mouse.y-prevMousePosition.y;   
    
    // move window position based on the cursor position
    GetWindowRect(win->GetHandle(),rect);
    rect->left+=mouseOffset.x;
    rect->top+=mouseOffset.y;    
    rect->right+=mouseOffset.x;
    rect->bottom+=mouseOffset.y;    
    originalPos.x=rect->left;
    originalPos.y=rect->top;
       
    // snap to other windows
    moveToInsideOfWindow(hDesktop,rect); 
    if (win==controller)
    {
        // todo: only snap to windows that were not connected to the controller when BeginMoving was called
        // todo: snap to desktop
                
        // move windows that were connected to the controller when BeginMoving was called
        int i;
        for (i=0;i<snappedWindows.size();i++)
        {
            moveWindowByOffset(snappedWindows.at(i)->GetHandle(),mouseOffset.x+(rect->left-originalPos.x),mouseOffset.y+(rect->top-originalPos.y));
        }
        
    }
    else
    {
        int id=GetStickyWindowId(win);
        if (id!=-1)
        {
            moveToOutsideOfWindow(controller->GetHandle(),rect);
            int i;
            for  (i=0;i<stickyWindows.size();i++)
            {
                if (i!=id)
                {
                    if ((stickyWindows.at(i)->GetHandle()!=NULL) && (isWindowInNormalState(stickyWindows.at(i)->GetHandle())))
                        moveToOutsideOfWindow(stickyWindows.at(i)->GetHandle(),rect);
                }
            }
        }
    }
    
    // record current mouse position for use next time
    prevMousePosition.x=mouse.x-(originalPos.x-rect->left);
    prevMousePosition.y=mouse.y-(originalPos.y-rect->top);
}


void StickyWindowManager::moveWindowByOffset(HWND hwnd,int xOffset,int yOffset)
{
    RECT rect;
    GetWindowRect(hwnd,&rect);
    MoveWindow(hwnd,rect.left+xOffset,rect.top+yOffset,rect.right-rect.left,rect.bottom-rect.top,true);
}

// calculates whether a window is connected to another by its border
char StickyWindowManager::isConnected(Window *win1,Window *win2)
{
    RECT r1,r2;
    
    if ( (!isWindowInNormalState(win1->GetHandle())) || (!isWindowInNormalState(win2->GetHandle())) )
        return false;
    
    GetWindowRect(win1->GetHandle(),&r1);
    GetWindowRect(win2->GetHandle(),&r2);    

    // check if windows are on top of each other
    if ((r1.top>r2.bottom) || (r1.bottom<r2.top) ||
            ((r1.left>r2.right) || (r1.right<r2.left)) )
        return false;    
    
    // check if the borders of the two windows are tuching
    if ((r1.right==r2.left) || (r1.left==r2.right) ||
            (r1.top==r2.bottom) || (r1.bottom==r2.top))
        return true;
    return false;
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

                #if DEBUG_STICKY_WINDOW_MANAGER
                  Console::write("StickyWindowManager::moveToOutsideOfWindow");
                  Console::write("\n");
                #endif


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


int StickyWindowManager::isWindowInNormalState(HWND hwnd)
{
    /* check if window is hidden,  maximized, or minimized */
    if ((!IsWindowVisible(hwnd)) || (IsZoomed(hwnd)) || (IsIconic(hwnd)))
        return false;

    return true;
}    
