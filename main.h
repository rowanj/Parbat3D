#define ID_MENU 501

#define IDM_FILEOPEN        201
#define IDM_FILESAVE        202
#define IDM_FILESAVEAS      203
#define IDM_FILEEXIT        207

#define IDM_TOOL            210

#define IDM_HELPCONTENTS    215
#define IDM_HELPABOUT       218

/* Make the classname into a global variable */


void loadFile(HWND hwnd);

/* Declare WindowsProcedure */
LRESULT CALLBACK MainWindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ImageWindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolWindowProcedure(HWND, UINT, WPARAM, LPARAM);

/* imageWindow.h declarations */
HWND setupImageWindow(HINSTANCE hThisInstance);

/* mainWindow.h declarations */
HWND setupMainWindow(HINSTANCE hThisInstance);

/* toolWindow.h declarations */
HWND setupToolWindow(HINSTANCE hThisInstance);

/* windowFunctions.h declarations */
void moveWindowByOffset(HWND hwnd,RECT *rect,int leftOffset,int topOffset);
void moveSnappedWindows(RECT *newRect,RECT *oldRect,RECT *prevImageWindowRect,RECT *prevToolWindowRect,int moveImageWindow,int moveToolWindow);
int snapWindow(HWND snapToWin,RECT *rect,POINT *mouseOffset);
void getMouseWindowOffset(HWND hwnd,int mx,int my,POINT *mouseOffset);

