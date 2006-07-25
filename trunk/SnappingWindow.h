

class SnappingWindow
{
    public:
    static void moveWindowByOffset(HWND hwnd,RECT *rect,int leftOffset,int topOffset);
    static void moveSnappedWindows(RECT *newRect,RECT *oldRect,RECT *prevImageWindowRect,RECT *prevToolWindowRect,int moveImageWindow,int moveToolWindow);
    static int snapWindowByMoving(HWND snapToWin,RECT *rect);
    static int snapWindowBySizing(HWND snapToWin,RECT *rect,int);
    static int snapInsideWindowByMoving(HWND snapToWin,RECT *rect);
    static int snapInsideWindowBySizing(HWND snapToWin,RECT *rect,int);
    static void getMouseWindowOffset(HWND hwnd,int mx,int my,POINT *mouseOffset);
    static void setNewWindowSize(RECT* newPos,RECT* oldPos,POINT* oldMouse,int whichDirection);
    static void setNewWindowPosition(RECT* newPos,POINT *mouseOffset);
    static int isWindowSnapped(HWND main,HWND sticky);      
    static int isWindowInNormalState(HWND hwnd);
};


