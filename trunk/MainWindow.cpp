#include <Windows.h>
#include "main.h"
#include "ImageWindow.h"
#include "OverviewWindow.h"
#include "ToolWindow.h"
#include "ROIWindow.h"
#include "Window.h"
#include "MainWindow.h"

/* ------------------------------------------------------------------------------------------------------------------------ */
/* Main Window Functions */


/* create main window */
int MainWindow::Create()
{
    /* Create main window */
    if (!CreateWin(0, "parbat3d main win class", "Parbat3D",
		WS_OVERLAPPED+WS_CAPTION+WS_SYSMENU+WS_MINIMIZEBOX, -50, -50, 1, 1,
		NULL, NULL))
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
    static WINDOWPLACEMENT roiWindowPlacement;         // recorded state of roi window when minimised
    static int imageWindowState=0;                      // recorded visibility state of image window
    static int toolWindowState=0;                       // recorded visibility state of tool window
    static int roiWindowState=0;                       // recorded visibility state of tool window
    
    
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
                        ShowWindow(imageWindow.GetHandle(),imageWindowState);
                    if (toolWindowState!=0)
                        ShowWindow(toolWindow.GetHandle(),toolWindowState);
                    if (roiWindowState!=0)
                        ShowWindow(roiWindow.GetHandle(),roiWindowState);    
                        
                    ShowWindow(overviewWindow.GetHandle(),SW_SHOW);
                    return 0;

                case SIZE_MINIMIZED:
                    /* record the current state of the child windows */
                    if (imageWindow.GetHandle()!=NULL)
                    {
                        if (!IsWindowVisible(imageWindow.GetHandle()))
                            imageWindowState=0;
                        else
                            imageWindowState=SW_SHOW;
                    }
                    else
                    {
                        imageWindowState=0;
                    }

                    if (toolWindow.GetHandle()!=NULL)
                    {
                        if (!IsWindowVisible(toolWindow.GetHandle()))
                            toolWindowState=0;
                        else
                            toolWindowState=SW_SHOW;                        
                    }
                    else
                    {
                        toolWindowState=0;
                    }
                    
                    if (roiWindow.GetHandle()!=NULL)
                    {
                        if (!IsWindowVisible(roiWindow.GetHandle()))
                            roiWindowState=0;
                        else
                            roiWindowState=SW_SHOW;                        
                    }
                    else
                    {
                        roiWindowState=0;
                    }
                    

                    /* hide the child windows */     
                    ShowWindow(imageWindow.GetHandle(),SW_HIDE);            
                    ShowWindow(toolWindow.GetHandle(),SW_HIDE);                    
                    ShowWindow(overviewWindow.GetHandle(),SW_HIDE);
                    ShowWindow(roiWindow.GetHandle(),SW_HIDE);
                    return 0;
		    }    
  	       return 0;
       
    }
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}
