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

    windows.push_back(&imageWindow);
    windows.push_back(&toolWindow);
    windows.push_back(&roiWindow);
    windows.push_back(&prefsWindow);
    windowStates=new int[windows.size()];

    prevProc=SetWindowProcedure(&MainWindow::WindowProcedure);
    Show();
    return true;
}

/* handle events related to the main window */
LRESULT CALLBACK MainWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //static int imageWindowState=0;                      // recorded visibility state of image window
//    static int toolWindowState=0;                       // recorded visibility state of tool window
//    static int roiWindowState=0;                       // recorded visibility state of tool window
//    static int prefsWindowState=0;						// recorded visibility state of prefs window
  
    
    MainWindow* win=(MainWindow*)Window::GetWindowObject(hwnd);
    int i;
    
    switch (message) 
    {
            
		case WM_SIZE:
        /* handle minimizing and restoring the child windows */            
		    switch(wParam)
		    {
                case SIZE_RESTORED:
                    /* restore child windows to their previous state */
                    
                    /*
                    if (imageWindowState!=0)
                        ShowWindow(imageWindow.GetHandle(),imageWindowState);
                    if (toolWindowState!=0)
                        ShowWindow(toolWindow.GetHandle(),toolWindowState);
                    if (roiWindowState!=0)
                        ShowWindow(roiWindow.GetHandle(),roiWindowState);
					if (prefsWindowState !=0)
						ShowWindow(prefsWindow.GetHandle(),prefsWindowState);    
                    */

                    for (i=0;i<win->windows.size();i++)
                    {
                        if (win->windowStates[i]!=0)
                            ShowWindow(win->windows.at(i)->GetHandle(),win->windowStates[i]);
                    }

                        
                    ShowWindow(overviewWindow.GetHandle(),SW_SHOW);
                    return 0;

                case SIZE_MINIMIZED:
                    /* record the current state of the child windows */

                    /*
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
                    
                    if (prefsWindow.GetHandle()!=NULL)
                    {
                        if (!IsWindowVisible(prefsWindow.GetHandle()))
                            prefsWindowState=0;
                        else
                            prefsWindowState=SW_SHOW;                        
                    }
                    else
                    {
                        prefsWindowState=0;
                    }
                    */


                    for (i=0;i<win->windows.size();i++)
                    {
                        /* get window state */
                        if (win->windows.at(i)->GetHandle()!=NULL)
                        {
                            if (!IsWindowVisible(win->windows.at(i)->GetHandle()))
                                win->windowStates[i]=0;
                            else
                                win->windowStates[i]=SW_SHOW;                        

                            /* hide the window */                                                
                            ShowWindow(win->windows.at(i)->GetHandle(),SW_HIDE);                                                           
                        }
                        else
                        {
                            win->windowStates[i]=0;
                        }
                        
                        
                    }
                    

 
                    /*
                    ShowWindow(imageWindow.GetHandle(),SW_HIDE);            
                    ShowWindow(toolWindow.GetHandle(),SW_HIDE);                    
                    ShowWindow(overviewWindow.GetHandle(),SW_HIDE);
                    ShowWindow(roiWindow.GetHandle(),SW_HIDE);
                    ShowWindow(prefsWindow.GetHandle(), SW_HIDE);
                    */
					return 0;
		    }    
  	       return 0;
       
    }
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}
