#include <Windows.h>
#include "main.h"
#include "Window.h"
#include "FeatureSpace.h"
#include "GLContainer.h"
#include "Config.h"
#include "Console.h"
#include <gl/gl.h>
#include <gl/glu.h>

int FeatureSpace::numFeatureSpaces=0;

const int FeatureSpace::FEATURE_WINDOW_WIDTH=650;       // inital width of feature space window
const int FeatureSpace::FEATURE_WINDOW_HEIGHT=550;      // inital height of feature space window
const int FeatureSpace::TOOLBAR_HEIGHT=40;              // height of toolbar


// timer used for testing
#include <time.h>   
void sleep(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

// create & display new feature space window
FeatureSpace::FeatureSpace(int LOD, bool only_ROIs)
{
    int createSuccess;
    
    createSuccess=Create();
    assert(createSuccess);
    
    glview=new GLView(glContainer->GetHandle());
    numFeatureSpaces++;
    
    // todo: setup feature space's data
    Console::write("FeatureSpace::FeatureSpace Sleeping...\n");
    sleep(5000);    
}

// draw contents of GLContainer with opengl
void FeatureSpace::PaintGLContainer()
{
	glview->make_current();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glview->GLswap();
} 
 
// create feature space window 
int FeatureSpace::Create()
{
    RECT rect;

    // get position of overview window for alignment   
    GetWindowRect(overviewWindow.GetHandle(),&rect);

    // create feature space window
    const char *title=makeMessage("Feature Space ",numFeatureSpaces+1);
    if (!CreateWin(0, "Parbat3D Feature Window", title,
	     WS_POPUP+WS_SYSMENU+WS_CAPTION+WS_SIZEBOX+WS_MAXIMIZEBOX+WS_MINIMIZEBOX,
	     rect.right, rect.top, FEATURE_WINDOW_WIDTH, FEATURE_WINDOW_HEIGHT, NULL, NULL))
        return false;
    delete(title);
    
    // create child windows
    glContainer=new GLContainer(GetHandle(),this,0,0,FEATURE_WINDOW_WIDTH,FEATURE_WINDOW_HEIGHT-TOOLBAR_HEIGHT);       

    //todo: create toolbar
    

    // make this window snap to others
    stickyWindowManager.AddStickyWindow(this);

    // handle events for the window
    prevProc=SetWindowProcedure(&WindowProcedure);	
    
    
    OnResize();    
    Show();       
    return true;    
}
 
// resize GLContainer to fit the feature space window
void FeatureSpace::OnResize()
{
    RECT rect;
    GetClientRect(GetHandle(),&rect);
    MoveWindow(glContainer->GetHandle(),rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top-TOOLBAR_HEIGHT, true);
}
 
/* handle events related to the feature space window */
LRESULT CALLBACK FeatureSpace::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{   

    FeatureSpace* win=(FeatureSpace*)Window::GetWindowObject(hwnd);
    
    
    switch (message)                  /* handle the messages */
    {			
        /* WM_DESTORY: system is destroying our window */
        case WM_SIZE:
            win->OnResize();
            break;
        case WM_DESTROY:
            stickyWindowManager.RemoveStickyWindow(win);
            break;
    }
    /* call the next procedure in the chain */       
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}
