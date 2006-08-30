#include <Windows.h>
#include "main.h"
#include "Window.h"
#include "FeatureSpace.h"
#include "Config.h"
#include <time.h>

/* ------------------------------------------------------------------------------------------------------------------------ */
/* Prefs Window Functions */

int FeatureSpace::numFeatureSpaces=0;

FeatureSpace::FeatureSpace(int LOD, bool only_ROIs)
{
    assert(Create(imageWindow.GetHandle()));
    glview=new GLView(hglcontainer);
    numFeatureSpaces++;
}
 
void sleep(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

int FeatureSpace::Create(HWND parent)
{
    RECT rect;
    int mx,my;
    const int FEATURE_WINDOW_WIDTH=650;
    const int FEATURE_WINDOW_HEIGHT=550;
    const int TOOLBAR_HEIGHT=40;
   
    // Get Overview Window Location for Prefs window alignment
    GetWindowRect(overviewWindow.GetHandle(),&rect);

    // create feature space window
    const char *title=makeMessage("Feature Space - ",numFeatureSpaces+1);
    if (!CreateWin(0, "Parbat3D Feature Window", title,
	     WS_POPUP+WS_SYSMENU+WS_CAPTION+WS_SIZEBOX+WS_MAXIMIZEBOX,
	     rect.right, rect.top, FEATURE_WINDOW_WIDTH, FEATURE_WINDOW_HEIGHT, parent, NULL))
        return false;
    delete(title);
        
    hglcontainer=CreateWindowEx(0,"static","opengl graphics goes here", 
                        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, 
                        FEATURE_WINDOW_WIDTH, FEATURE_WINDOW_HEIGHT-TOOLBAR_HEIGHT,
                        GetHandle(), NULL, Window::GetAppInstance(), NULL);
        

    stickyWindowManager.AddStickyWindow(this);

    prevProc=SetWindowProcedure(&WindowProcedure);	
    sleep(5000);    
    Show();       
    return true;    
}
 
/* handle events related to the main window */
LRESULT CALLBACK FeatureSpace::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{   
    static POINT moveMouseOffset;     /* mouse offset relative to window, used for snapping */
    static RECT rect;                 /* for general use */

    FeatureSpace* win=(FeatureSpace*)Window::GetWindowObject(hwnd);
    
    
    switch (message)                  /* handle the messages */
    {			
        /* WM_DESTORY: system is destroying our window */
        case WM_DESTROY:
            stickyWindowManager.RemoveStickyWindow(win);
            break;
    }
    /* call the next procedure in the chain */       
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}
