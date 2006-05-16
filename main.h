#ifndef _PARBAT_MAIN_H
#define _PARBAT_MAIN_H

#define ID_MENU 501

#define IDM_FILEOPEN        201
#define IDM_FILESAVE        202
#define IDM_FILESAVEAS      203
#define IDM_FILEEXIT        207

#define IDM_IMAGEWINDOW     210
#define IDM_TOOLSWINDOW      211

#define IDM_HELPCONTENTS    215
#define IDM_HELPABOUT       218

#define IDC_RADIO_BUTTON_1 10001


/* function declarations */

void loadFile();

LRESULT CALLBACK MainWindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DisplayWindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ImageWindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolWindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolWindowTabControlProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolWindowDisplayTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolWindowQueryTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);

int setupImageWindow();

int setupMainWindow();

int setupToolWindow();

void moveWindowByOffset(HWND hwnd,RECT *rect,int leftOffset,int topOffset);
void moveSnappedWindows(RECT *newRect,RECT *oldRect,RECT *prevImageWindowRect,RECT *prevToolWindowRect,int moveImageWindow,int moveToolWindow);
int snapWindowByMoving(HWND snapToWin,RECT *rect);
int snapWindowBySizing(HWND snapToWin,RECT *rect,int);
void getMouseWindowOffset(HWND hwnd,int mx,int my,POINT *mouseOffset);
void setNewWindowSize(RECT* newPos,RECT* oldPos,POINT* oldMouse,int whichDirection);
void setNewWindowPosition(RECT* newPos,POINT *mouseOffset);

#endif
