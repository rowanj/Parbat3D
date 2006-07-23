#ifndef _PARBAT_MAIN_H
#define _PARBAT_MAIN_H

#include "resources.h"
#include "Settings.h"
#include "ImageHandler.h"

extern ImageHandler::ImageHandler* image_handler;
extern char szStaticControl[];
extern HINSTANCE hThisInstance;
extern HWND hDesktop;
extern settings winPos;
extern char *filename;
enum {DISPLAY_TAB_ID,QUERY_TAB_ID,IMAGE_TAB_ID};



/* function declarations */

void loadFile();
void closeFile();

LRESULT CALLBACK OverviewWindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK MainWindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DisplayWindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ImageWindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolWindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolWindowTabControlProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolWindowDisplayTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolWindowQueryTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolWindowImageTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolWindowScrollBarProcedure(HWND, UINT, WPARAM, LPARAM);

int registerToolWindow();
int registerImageWindow();
int registerMainWindow();
int registerOverviewWindow();
int setupImageWindow();
int setupMainWindow();
int setupToolWindow();
int setupOverviewWindow();
void orderWindows();
void showToolWindowTabContainer(int);

extern void moveWindowByOffset(HWND hwnd,RECT *rect,int leftOffset,int topOffset);
extern void moveSnappedWindows(RECT *newRect,RECT *oldRect,RECT *prevImageWindowRect,RECT *prevToolWindowRect,int moveImageWindow,int moveToolWindow);
extern int snapWindowByMoving(HWND snapToWin,RECT *rect);
extern int snapWindowBySizing(HWND snapToWin,RECT *rect,int);
extern void getMouseWindowOffset(HWND hwnd,int mx,int my,POINT *mouseOffset);
extern void setNewWindowSize(RECT* newPos,RECT* oldPos,POINT* oldMouse,int whichDirection);
extern void setNewWindowPosition(RECT* newPos,POINT *mouseOffset);

int isWindowInNormalState(HWND hwnd);
int toggleMenuItemTick(HMENU hMenu,int itemId);

void updateImageScrollbar();
void updateImageWindowTitle();
void updateToolWindowScrollbar();
void scrollToolWindowToTop();

#endif
