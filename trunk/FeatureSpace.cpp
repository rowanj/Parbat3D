#include <Windows.h>
#include "main.h"
#include "Window.h"
#include "FeatureSpace.h"
#include "GLContainer.h"
#include "Config.h"
#include "Console.h"
#include <gl/gl.h>
#include <gl/glu.h>

/* ------------------------------------------------------------------------------------------------------------------------ */
/* Prefs Window Functions */

int FeatureSpace::numFeatureSpaces=0;

FeatureSpace::FeatureSpace(int LOD, bool only_ROIs)
{
    assert(Create());
    glview=new GLView(glContainer->GetHandle());
    numFeatureSpaces++;
        
}
 
int FeatureSpace::Create()
{
    RECT rect;
    int mx,my;
    const int FEATURE_WINDOW_WIDTH=650;
    const int FEATURE_WINDOW_HEIGHT=550;
    const int TOOLBAR_HEIGHT=50;
   
    // Get Overview Window Location for Prefs window alignment
    GetWindowRect(overviewWindow.GetHandle(),&rect);

    // create feature space window
    const char *title=makeMessage("Feature Space ",numFeatureSpaces+1);
    if (!CreateWin(0, "Parbat3D Feature Window", title,
	     WS_POPUP+WS_SYSMENU+WS_CAPTION+WS_SIZEBOX+WS_MAXIMIZEBOX+WS_MINIMIZEBOX,
	     rect.right, rect.top, FEATURE_WINDOW_WIDTH, FEATURE_WINDOW_HEIGHT, NULL, NULL))
        return false;
    delete(title);
    
    glContainer=new GLContainer(GetHandle(),this,0,0,FEATURE_WINDOW_WIDTH,FEATURE_WINDOW_HEIGHT-TOOLBAR_HEIGHT);       

    stickyWindowManager.AddStickyWindow(this);

    prevProc=SetWindowProcedure(&WindowProcedure);	
    Sleep(5000);    
    Show();       
    return true;    
}
 
void FeatureSpace::PaintGLContainer()
{
	glview->make_current();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glview->GLswap();
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
