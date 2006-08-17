#include <Windows.h>
#include "main.h"
#include "ImageWindow.h"
#include "OverviewWindow.h"
#include "ToolWindow.h"
#include "Window.h"
#include "MainWindow.h"

/* ------------------------------------------------------------------------------------------------------------------------ */
/* Main Window Functions */


/* create main window */
int MainWindow::Create(HINSTANCE hInstance)
{
    /* Create main window */
    if (!CreateWin(0, "parbat3d main win class", "Parbat3D",
		WS_OVERLAPPED+WS_CAPTION+WS_SYSMENU+WS_MINIMIZEBOX, -50, -50, 100, 100,
		NULL, NULL, hInstance))
        return false;                        

    prevProc=SetWindowProcedure(&MainWindow::WindowProcedure);
    Show();
    return true;
}

/* handle events related to the main window */
LRESULT CALLBACK MainWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static WINDOWPLACEMENT imageWindowPlacement;        // recorded state of image window when minimised
    static WINDOWPLACEMENT toolWindowPlacement;         // recorded state of tool window when minimised
    static int imageWindowState=0;                      // recorded visibility state of image window
    static int toolWindowState=0;                       // recorded visibility state of tool window
    
    
    MainWindow* win=(MainWindow*)Window::GetWindowObject(hwnd);
    
    switch (message) 
    {
            
		case WM_SIZE:
        /* handle minimizing and restoring the child windows */            
		    switch(wParam)
		    {
                case SIZE_RESTORED:
                    /* restore child windows to their previous state */
                    if (imageWindowState!=0)
                        ShowWindow(ImageWindow::hImageWindow,imageWindowState);
                    if (toolWindowState!=0)
                        ShowWindow(ToolWindow::hToolWindow,toolWindowState);
                    ShowWindow(OverviewWindow::hOverviewWindow,SW_SHOW);
                    return 0;

                case SIZE_MINIMIZED:
                    /* record the current state of the child windows */
                    if (ImageWindow::hImageWindow!=NULL)
                    {
                        if (!IsWindowVisible(ImageWindow::hImageWindow))
                            imageWindowState=0;
                        else
                            imageWindowState=SW_SHOW;
                    
                    }
                    else
                    {
                        imageWindowState=0;
                    }

                    if (ToolWindow::hToolWindow!=NULL)
                    {
                        if (!IsWindowVisible(ToolWindow::hToolWindow))
                            toolWindowState=0;
                        else
                            toolWindowState=SW_SHOW;                        
                    }
                    else
                    {
                        toolWindowState=0;
                    }

                    /* hide the child windows */     
                    ShowWindow(ImageWindow::hImageWindow,SW_HIDE);            
                    ShowWindow(ToolWindow::hToolWindow,SW_HIDE);                    
                    ShowWindow(OverviewWindow::hOverviewWindow,SW_HIDE);                    
                    return 0;
		    }    
  	       return 0;
       
    }
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}
