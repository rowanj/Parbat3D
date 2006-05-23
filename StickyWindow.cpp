//note: do not use these classes yet

#include <vector>
#include <set>
#include <algorithm>
#include <Windows.h>

using std::set;
using std::vector;

class MovingStickyWindow;

class StickyWindow
{
    protected:
        HWND  hwnd;
        set<StickyWindow*> stickTo;
        set<StickyWindow*> attract;
        vector<MovingStickyWindow*> moving;
        set<StickyWindow*> snapped;
        POINT mouseOffset;     /* mouse offset relative to window, used for snapping */
        RECT startMovePosition;
        //POINT sizeMousePosition;   /* mouse position, used for sizing window */
        //RECT  sizeWindowPosition;   /* window position, used for sizing window */
    public:
        StickyWindow(HWND hwin);
        void stickToWindow(StickyWindow *stickyWin);
        void attractWindow(StickyWindow *stickyWin);    
        void stopStickingTo(StickyWindow *stickyWin);
        void stopAttracting(StickyWindow *stickyWin);
        void removeStickyWindow(StickyWindow *stickyWin);
        void onStartMove(int x,int y);
        void onMoving(RECT* rect);
        void onSize();    
        void onStartSize();
        void onSizing(); 
        HWND getHWND() {return hwnd;};  
        void setNewWindowPosition(RECT* newPos);     
        int snapWindowByMoving(HWND snapToWin,RECT *rect);
        void getMouseWindowOffset(int mx,int my);
        //void moveBy(int x,int y);
        SIZE* getDistanceMoved(RECT *rect);
};

class MovingStickyWindow
{
    protected:
        RECT *originalRect;
        StickyWindow *win;
    public:
        MovingStickyWindow(StickyWindow* sticky,RECT* rect) {win=sticky;originalRect=rect;};
        void moveBy(int,int);
        StickyWindow* getStickyWindow() {return win;};
};

StickyWindow::StickyWindow(HWND win)
{
    this->hwnd=win;
}

void StickyWindow::stickToWindow(StickyWindow *stickyWin)
{
    stickTo.insert(stickyWin);
}

void StickyWindow::attractWindow(StickyWindow *stickyWin)
{
    attract.insert(stickyWin);
    //stickyWin.stickToWindow(this);
}

void StickyWindow::stopStickingTo(StickyWindow *stickyWin)
{
    stickTo.erase(stickyWin);
}

void StickyWindow::stopAttracting(StickyWindow *stickyWin)
{
    attract.erase(stickyWin);
    //stickyWin.stopStickingTo(this);
}

void StickyWindow::onStartMove(int x,int y)
{
    this->getMouseWindowOffset(x,y);     /* record mouse position relative to window position */
      
    if (attract.size()!=0)
    {              
                // begin to move all sticky windows that are currently snapped to this one
                moving.clear();
                set<StickyWindow*>::iterator isticky;
                for (isticky=snapped.begin();isticky!=snapped.end();++isticky)
                {
                    RECT *rect=new RECT;
                    GetWindowRect((*isticky)->getHWND(),rect);        /* record current window position */                        
                    MovingStickyWindow *movingWin=new MovingStickyWindow(*isticky,rect);                        
                    moving.push_back(movingWin);
                }
                
                // store current window position
                GetWindowRect(hwnd,&startMovePosition);
    }
}


void StickyWindow::onMoving(RECT* rect)
{
    /* set new window position based on position of mouse */
    this->setNewWindowPosition(rect);

    /* snap main window to edge of desktop */           
    //snapWindowByMoving(hDesktop,(RECT*)lParam);
   
    if (attract.size()!=0)    
    {
            set<StickyWindow*>::iterator isticky;
            for (isticky=stickTo.begin();isticky!=stickTo.end();++isticky)
            {
                /* only attempt to snap to windows that arn't already being moved with this window */
                int found=false;
                vector<MovingStickyWindow*>::iterator imsticky;
                for (imsticky=moving.begin();imsticky!=moving.end();++imsticky)
                {
                    if ((*imsticky)->getStickyWindow()==*isticky)
                    {
                        found=true;
                        break;
                    }
                }
                if (!found)
                {
                    if (this->snapWindowByMoving((*isticky)->getHWND(),rect))
                        snapped.insert(*isticky);
                    else
                        snapped.erase(*isticky);
                }
            }  
            
            /* move the windows that are stuck to this one */
            SIZE *distance=this->getDistanceMoved(rect);
            vector<MovingStickyWindow*>::iterator imsticky;
            for (imsticky=moving.begin();imsticky!=moving.end();++imsticky)
            {
                (*imsticky)->moveBy(distance->cx,distance->cy);
            }
    }
    else if (stickTo.size()!=0)
    {       
            /* attempt to snap to all stickTo windows */
            set<StickyWindow*>::iterator isticky;            
            for (isticky=stickTo.begin();isticky!=stickTo.end();++isticky)
            {
                    if (this->snapWindowByMoving((*isticky)->getHWND(),rect))
                        snapped.insert(*isticky);
                    else
                        snapped.erase(*isticky);
            }  
    }                   
}

void StickyWindow::onStartSize()
{
}


void StickyWindow::onSizing()
{
}


/* calculates window's new position based on the location of the mouse cursor */
void StickyWindow::setNewWindowPosition(RECT* newPos)
{
    POINT mouse;    /* mouse co-ords */
    
    /* get current mouse co-ords */
    GetCursorPos(&mouse);

    /* get width & height of window */
    newPos->bottom-=newPos->top;
    newPos->right-=newPos->left;
    
    /* set window position based on the cursor position */
    newPos->top=mouse.y-this->mouseOffset.y;
    newPos->left=mouse.x-this->mouseOffset.x;
    newPos->bottom+=newPos->top;
    newPos->right+=newPos->left;                
}

/* snap a window to another window (by moving it) if it is range */
/* snapToWin=handle of window to snap to, rect=cords of window to be moved */
/* returns: true if window has been snapped, false if not */
int StickyWindow::snapWindowByMoving(HWND snapToWin,RECT *rect)
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
    
    // snap (right) to left
    difference=((rect->left+winsize.cx)-(snapToRect.left));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->left=snapToRect.left-winsize.cx;
       rect->right=rect->left+winsize.cx;       
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
    
    // snap (left) to right
    difference=(rect->left-(snapToRect.right));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->left=snapToRect.right;
       rect->right=rect->left+winsize.cx;       
       isSnapped=true;       
    }        

    // snap (top) to top
    difference=(rect->top-(snapToRect.top));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->top=snapToRect.top;
       rect->bottom=rect->top+winsize.cy;
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


void StickyWindow::getMouseWindowOffset(int mx,int my)
{
    RECT win;
    GetWindowRect(this->hwnd,&win);
    this->mouseOffset.x=mx-win.left;
    this->mouseOffset.y=my-win.top;
}

SIZE* StickyWindow::getDistanceMoved(RECT *rect)
{
    SIZE *sz=new SIZE;
    sz->cx=rect->left-startMovePosition.left;
    sz->cy=rect->top-startMovePosition.top;
    return sz;
}

void MovingStickyWindow::moveBy(int x,int y)
{
    RECT rect;
    HWND hwnd=this->win->getHWND();
    GetWindowRect(hwnd,&rect);
    rect.left+=x;
    rect.top+=y;
    rect.right+=x;
    rect.bottom+=y;
    MoveWindow(hwnd,rect.left,rect.top,rect.right,rect.bottom,true);
}
