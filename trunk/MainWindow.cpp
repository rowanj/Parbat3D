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

BOOL CALLBACK MainWindow::SaveChildState(HWND hwnd, LPARAM lparam)
{
    MainWindow *win=(MainWindow*)lparam;
    
//    if (hwnd!=overviewWindow.GetHandle())
//    {
        win->savedChildren.push_back(hwnd);
        if (!IsWindowVisible(hwnd))
            win->restoreStates.push_back(0);
        else
            win->restoreStates.push_back(SW_SHOW);

        /* hide the window */                                                
        ShowWindow(hwnd,SW_HIDE);                                                           
 //   }
    return true;
}


/* handle events related to the main window */
LRESULT CALLBACK MainWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

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
                    
                   /* for (i=0;i<win->windows.size();i++)
                    {
                        if (win->windowStates[i]!=0)
                            ShowWindow(win->windows.at(i)->GetHandle(),win->windowStates[i]);
                    }*/                   
                    for (i=0;i<win->savedChildren.size();i++)
                    {
                        if (win->restoreStates.at(i)!=0)
                            ShowWindow(win->savedChildren.at(i),win->restoreStates.at(i));
                    }
                       
                    ShowWindow(overviewWindow.GetHandle(),SW_SHOW);                    
                    return 0;

                case SIZE_MINIMIZED:

                    /*
                    for (i=0;i<win->windows.size();i++)
                    {
                        // get window state
                        if (win->windows.at(i)->GetHandle()!=NULL)
                        {
                            if (!IsWindowVisible(win->windows.at(i)->GetHandle()))
                                win->windowStates[i]=0;
                            else
                                win->windowStates[i]=SW_SHOW;                        

                            // hide the window                                                
                            ShowWindow(win->windows.at(i)->GetHandle(),SW_HIDE);                                                           
                        }
                        else
                        {
                            win->windowStates[i]=0;
                        }
                        
                        
                    }*/
                    win->savedChildren.clear();
                    win->restoreStates.clear();
//                    EnumChildWindows(hwnd,&SaveChildState,(LPARAM)win);

                    ShowWindow(overviewWindow.GetHandle(),SW_HIDE);
 
                    /*
                    ShowWindow(imageWindow.GetHandle(),SW_HIDE);            
                    ShowWindow(toolWindow.GetHandle(),SW_HIDE);                    
                    ShowWindow(roiWindow.GetHandle(),SW_HIDE);
                    ShowWindow(prefsWindow.GetHandle(), SW_HIDE);
                    */
					return 0;
		    }    
  	       return 0;
       
    }
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}
