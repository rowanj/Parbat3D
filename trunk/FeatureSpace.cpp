#include <Windows.h>
#include "main.h"
#include "Window.h"
#include "FeatureSpace.h"
#include "Config.h"
#include <time.h>

/* ------------------------------------------------------------------------------------------------------------------------ */
/* Prefs Window Functions */

FeatureSpace::FeatureSpace(int LOD, bool only_ROIs)
{
    
    assert(Create(imageWindow.GetHandle()));
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
    const int FEATURE_WINDOW_WIDTH=600;
    const int FEATURE_WINDOW_HEIGHT=500;
   
    // Get Overview Window Location for Prefs window alignment
    GetWindowRect(overviewWindow.GetHandle(),&rect);

    // create feature space window
    if (!CreateWin(0, "Parbat3D Feature Window", "Feature Space",
	     WS_POPUP+WS_SYSMENU+WS_CAPTION,
	     rect.right, rect.top, FEATURE_WINDOW_WIDTH, FEATURE_WINDOW_HEIGHT, parent, NULL))
        return false;
    sleep(5000);    

    stickyWindowManager.AddStickyWindow(this);

    prevProc=SetWindowProcedure(&WindowProcedure);	
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
			
        /* WM_CLOSE: system or user has requested to close the window/application */             
        //case WM_CLOSE:
            /* don't destroy this window, but make it invisible */
            //ShowWindow(hwnd,SW_HIDE);
            //return 0;

        /* WM_DESTORY: system is destroying our window */
        case WM_DESTROY:
            stickyWindowManager.RemoveStickyWindow(win);
            break;
            
    }
    /* call the next procedure in the chain */       
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}
