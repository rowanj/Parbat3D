//note: do not build this file yet

#include <vector>

class MovingStickyWindow
{
    protected:
        RECT *originalRect;
        StickyWindow *win;
    public:
        MovingStickyWindow(StickyWindow*,RECT*);
        void moveBy(int,int);
};

class StickyWindow
{
    protected:
        HWND  hwnd;
        vector<StickyWindow*> stickTo;
        vector<StickyWindow*> attract;
        vector<StickyWindow*> moving;
        POINT moveMouseOffset;     /* mouse offset relative to window, used for snapping */
        POINT sizeMousePosition;   /* mouse position, used for sizing window */
        RECT  sizeWindowPosition;   /* window position, used for sizing window */
    public:
        StickyWindow(HWND hwin);
        void stickToWindow(StickyWindow stickyWin);
        void attractWindow(StickyWindow stickyWin);        
        void onStartMove();
        void onMoving();
        void onSize();    
        void onStartSize();
        void onSizing(); 
        HWND getHWND() {return hwnd};       
};


StickyWindow::StickyWindow(HWND win)
{
    this->hwnd=win;
}

StickyWindow::addStickyWindow(HWND stickyWin)
{
    stickywins.push_back(stickyWin);
    isSnapped.push_back(false);
    prevRect.push_back(new RECT);
}

StickyWindow::removeStickyWindow(HWND stickyWin)
{
    //todo
}

StickyWindow::onStartMove(int x,int y)
{
    this->getMouseWindowOffset(x,y);     /* record mouse position relative to window position */
      
    if (!attract.isEmpty())
    {
                moving.clear();
                for (int i=0;i<attract.size();i++)
                {
                    if (isSnapped[i])
                    {
                        RECT *rect=new RECT;
                        GetWindowRect(attract[i].getHWND(),rect);        /* record current window position */                        
                        MovingStickyWindow movingWin=new MovingStickyWindow(attract[i],rect);                        
                        moving.push_back(MovingStickyWindow);
                    }
                    else
                    {
                        isMoving[i]=false;                                      /* indicate the window should not be moved */
                    }
                }              
    }
}


StickyWindow::onMoving()
{
    /* set new window position based on position of mouse */
    this->setNewWindowPosition((RECT*)lParam);

    /* snap main window to edge of desktop */           
    //snapWindowByMoving(hDesktop,(RECT*)lParam);
   
    if (!attract.isEmpty())    
    {
            for (int i=0;i<stickTo.size();i++)
            {
                // only attempt to snap to the window is it's not already stuck to this one
                //if (not in moving list)
                {
                    isSnapped[i]=stickTo[i]->snapWindowByMoving((RECT*)lParam); 
                }
            }  
            
            /* move the windows that are stuck to / moving with this one */
            SIZE *distance=this->getDistanceMoved((RECT*)lParam);
            for (int i=0;i<moving.size();i++)
            {
                moving[i].moveBy(distance.cx,distance.cy);
            }
    }
    else if (!stickTo.isEmpty()
    {       
            
            for (int i=0;i<stickTo.size();i++)
            {
                isSnapped[i]=stickTo[i]->snapWindowByMoving((RECT*)lParam); 
            }  
    }                   
}

StickyWindow:onStartSize()
{
}
