#ifndef _PARBAT_MAIN_H
#define _PARBAT_MAIN_H

#define ID_MENU             501

#define IDM_FILEOPEN        201
#define IDM_FILESAVE        202
#define IDM_FILESAVEAS      203
#define IDM_FILECLOSE       205
#define IDM_FILEEXIT        207

#define IDM_IMAGEWINDOW     210
#define IDM_TOOLSWINDOW     211

#define IDM_HELPCONTENTS    215
#define IDM_HELPABOUT       218

#define IDC_RADIO_BUTTON_1 10001



/* global variable declarations
    -added these so main.cpp can be split up into multiple files -shane */
extern char szMainWindowClassName[];
extern char szImageWindowClassName[];
extern char szToolWindowClassName[];
extern char szDisplayClassName[];
extern char szStaticControl[];
extern HINSTANCE hThisInstance;
extern HWND hRed, hBlue, hGreen;
extern HWND hMainWindow,hImageWindow, hToolWindow, hDesktop;
extern HMENU hMainMenu;
extern HWND hToolWindowTabControl, hToolWindowDisplayTabContainer;
extern HWND hToolWindowQueryTabContainer, hToolWindowImageTabContainer;
extern HWND hRedRadioButton1, hRedRadioButton2;
extern HWND hGreenRadioButton1, hGreenRadioButton2;
extern HWND hBlueRadioButton1, hBlueRadioButton2;
extern HWND hImageWindowDisplay;
extern HWND hMainWindowDisplay;
extern HWND hupdate;

extern int bands;
extern HWND *redRadiobuttons;
extern HWND *greenRadiobuttons;
extern HWND *blueRadiobuttons;
extern int imageWindowIsSnapped;
extern int toolWindowIsSnapped;

/* Define id numbers for the tab's in the tool window */
//enum {DISPLAY_TAB_ID,QUERY_TAB_ID,IMAGE_TAB_ID};

/* Used for loading and saving window position and sizes */
extern settings winPos;

/* objects used for painting/drawing */
extern HFONT hBoldFont,hNormalFont,hHeadingFont;
extern HPEN hTabPen;
extern HBRUSH hTabBrush;
extern WNDPROC oldTabControlProc,oldDisplayTabContainerProc,oldQueryTabContainerProc,oldImageTabContainerProc;


/* function declarations */

void loadFile();
void closeFile();

LRESULT CALLBACK MainWindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DisplayWindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ImageWindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolWindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolWindowTabControlProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolWindowDisplayTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolWindowQueryTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolWindowImageTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);

int registerToolWindow();
int registerImageWindow();
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

int isWindowInNormalState(HWND hwnd);
int toggleMenuItemTick(HMENU hMenu,int itemId);

#endif
