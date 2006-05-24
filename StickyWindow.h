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

