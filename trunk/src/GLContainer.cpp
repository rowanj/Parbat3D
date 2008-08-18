#include "PchApp.h"

#include "main.h"
#include "Window.h"
#include "GLContainer.h"
#include "Config.h"
#include "console.h"


GLContainer::GLContainer(HWND parent,GLContainerHandler *eventHandler,int x,int y,int width,int height)
{
    // setup variables
    handler=eventHandler; 
    
    // create container control
    int createSuccess=CreateWin(WS_EX_CLIENTEDGE, "Parbat3D GLContainer", NULL,
	                       WS_CHILD|WS_VISIBLE, x, y, width, height, parent, NULL);
    
    assert(createSuccess==TRUE);
    
    prevProc=SetWindowProcedure(&WindowProcedure);    
}
 
 
/* handle events related to the main window */
LRESULT CALLBACK GLContainer::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{   
    PAINTSTRUCT ps;
    HDC hdc;
    GLContainer* win=(GLContainer*)Window::GetWindowObject(hwnd);
   
    switch (message)   
    {			
		case WM_LBUTTONDOWN:
			Console::write("GLContainer WM_LBUTTONDOWN\n");
            if (win->handler!=NULL)
                win->handler->OnGLContainerLeftMouseDown(LOWORD(lParam),HIWORD(lParam));					
			break;

		case WM_MOUSEMOVE:
            if (win->handler!=NULL)
                win->handler->OnGLContainerMouseMove(wParam,LOWORD(lParam),HIWORD(lParam));
			break;
			
        case WM_PAINT:
            hdc=BeginPaint(hwnd,&ps);
            if (win->handler!=NULL)
                win->handler->PaintGLContainer();
            EndPaint(hwnd,&ps);                
            return 0;
    }
    /* call the next procedure in the chain */       
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}
