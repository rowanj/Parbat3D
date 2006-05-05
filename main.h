#ifndef _PARBAT_MAIN_H
#define _PARBAT_MAIN_H

#define ID_MENU 501

#define IDM_FILEOPEN        201
#define IDM_FILESAVE        202
#define IDM_FILESAVEAS      203
#define IDM_FILEEXIT        207

#define IDM_TOOL            210

#define IDM_HELPCONTENTS    215
#define IDM_HELPABOUT       218


/* function declarations */

void loadFile();

LRESULT CALLBACK MainWindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ImageWindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolWindowProcedure(HWND, UINT, WPARAM, LPARAM);

int setupImageWindow();

int setupMainWindow();

int setupToolWindow();

void moveWindowByOffset(HWND hwnd,RECT *rect,int leftOffset,int topOffset);
void moveSnappedWindows(RECT *newRect,RECT *oldRect,RECT *prevImageWindowRect,RECT *prevToolWindowRect,int moveImageWindow,int moveToolWindow);
int snapWindow(HWND snapToWin,RECT *rect);
void getMouseWindowOffset(HWND hwnd,int mx,int my,POINT *mouseOffset);
void setNewWindowPosition(RECT* newPos,POINT *mouseOffset);

#endif
