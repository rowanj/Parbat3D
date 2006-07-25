#include <Windows.h>
#include "main.h"
#include "ImageWindow.h"
#include "OverviewWindow.h"
#include "ToolWindow.h"
#include "MainWindow.h"

HWND MainWindow::hMainWindow=NULL;

char MainWindow::szMainWindowClassName[] = "Parbat3D Main Window";

/* ------------------------------------------------------------------------------------------------------------------------ */
/* Main Window Functions */


int MainWindow::registerMainWindow()
{
    WNDCLASSEX wincl;        /* Datastructure for the windowclass */
        
    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szMainWindowClassName;
    wincl.lpfnWndProc = MainWindowProcedure; /* This function is called by windows */
    wincl.style = CS_DBLCLKS;  /* Ctach double-clicks */
    wincl.cbSize = sizeof(WNDCLASSEX);

    /* Use default icon and mousepointer */
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL; /* No menu */
    wincl.cbClsExtra = 0; /* No extra bytes after the window class */
    wincl.cbWndExtra = 0; /* structure or the window instance */
    /* Use lightgray as the background of the window */
    wincl.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);

    /* Register the window class, if fails return false */
    return RegisterClassEx(&wincl);
}

/* create main window */
int MainWindow::setupMainWindow()
{
    /* Create main window */
    hMainWindow = CreateWindowEx(0, szMainWindowClassName, "Parbat3D",
		WS_OVERLAPPED+WS_CAPTION+WS_SYSMENU+WS_MINIMIZEBOX, -50, -50, 1, 1,
		NULL, NULL, hThisInstance, NULL);
    if (!hMainWindow)
        return false;                        

    ShowWindow(hMainWindow, SW_SHOW);
    
    return true;
}

/* handle events related to the main window */
LRESULT CALLBACK MainWindow::MainWindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static WINDOWPLACEMENT imageWindowPlacement;        // recorded state of image window when minimised
    static WINDOWPLACEMENT toolWindowPlacement;         // recorded state of tool window when minimised
    static int imageWindowState;                      // recorded visibility state of image window
    static int toolWindowState;                       // recorded visibility state of tool window
    
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            /* indicate that the window states have not yet been recorded */
            imageWindowState=0;
            toolWindowState=0;            
            break;
            
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
                    break;

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
                    break;
		    }    
  	       return 0;
  	       
        default:                   /* for messages that we don't deal with */
            /* let windows peform the default operation based on the message */
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0; 
}