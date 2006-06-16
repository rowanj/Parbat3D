



void moveWindowByOffset(HWND hwnd,RECT *rect,int leftOffset,int topOffset);
void moveSnappedWindows(RECT *newRect,RECT *oldRect,RECT *prevImageWindowRect,RECT *prevToolWindowRect,int moveImageWindow,int moveToolWindow);
int snapWindowByMoving(HWND snapToWin,RECT *rect);
int snapWindowBySizing(HWND snapToWin,RECT *rect,int);
int snapInsideWindowByMoving(HWND snapToWin,RECT *rect);
int snapInsideWindowBySizing(HWND snapToWin,RECT *rect,int);
void getMouseWindowOffset(HWND hwnd,int mx,int my,POINT *mouseOffset);
void setNewWindowSize(RECT* newPos,RECT* oldPos,POINT* oldMouse,int whichDirection);
void setNewWindowPosition(RECT* newPos,POINT *mouseOffset);
int isWindowSnapped(HWND main,HWND sticky);
