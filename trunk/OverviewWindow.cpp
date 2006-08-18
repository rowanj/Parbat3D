#include <Windows.h>
#include "main.h"
#include "OverviewWindow.h"
#include "SnappingWindow.h"
#include "ImageWindow.h"
#include "ToolWindow.h"
#include "DisplayWindow.h"
#include "MainWindow.h"
#include "console.h"


// Used for accessing the help folder
const char *helpPath;

DisplayWindow overviewWindowDisplay;

/* setup overview window */
int OverviewWindow::Create(HINSTANCE hThisInstance)
{

    RECT rect;
    
    /* get co-ords of image window */
    GetWindowRect(imageWindow.GetHandle(),&rect);
        
    /* Create overview window */
    if (!CreateWin(0, "Parbat3D Overview Window", "Parbat3D",
		WS_OVERLAPPED+WS_CAPTION+WS_SYSMENU+WS_MINIMIZEBOX, rect.left-OverviewWindow::OVERVIEW_WINDOW_WIDTH, rect.top, OVERVIEW_WINDOW_WIDTH, 296,
		imageWindow.GetHandle(), NULL, hThisInstance))
        return false;
    prevProc=SetWindowProcedure(&OverviewWindow::WindowProcedure);

    /* set menu to main windo */
	hMainMenu = LoadMenu(hThisInstance, MAKEINTRESOURCE(ID_MENU));
    SetMenu(GetHandle(), hMainMenu);
    
    /* Disable Window menu items. note: true appears to disable & false enables */
    EnableMenuItem(hMainMenu,IDM_IMAGEWINDOW,true);
    EnableMenuItem(hMainMenu,IDM_TOOLSWINDOW,true);
    EnableMenuItem(hMainMenu,IDM_FILECLOSE,true);

    /* create a child window that will be used by OpenGL */
    overviewWindowDisplay.Create(hThisInstance,GetHandle());
   
    /* Make the window visible on the screen */
    Show();
    
    return true;
}

/* toggles a menu item between a checked & unchecked state */
/* returns true if new state is checked, else false if unchecked */
int OverviewWindow::toggleMenuItemTick(HMENU hMenu,int itemId)
{
    int menuState=GetMenuState(hMenu,itemId,MF_BYCOMMAND);
    if (menuState&MF_CHECKED)
    {
        CheckMenuItem(hMenu,itemId,MF_UNCHECKED|MF_BYCOMMAND);
        return false;
    }
    else
    {
        CheckMenuItem(hMenu,itemId,MF_CHECKED|MF_BYCOMMAND);
        return true;
    }
}


/* This function is called by the Windows function DispatchMessage( ) */
/* All messages/events related to the main window (or it's controls) are sent to this procedure */
LRESULT CALLBACK OverviewWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    /* static variables used for snapping windows */
    static POINT snapMouseOffset;           /* mouse co-ords relative to position of window */
    static RECT prevOverviewWindowRect;         /* main window position before it was moved */
    static RECT prevImageWindowRect;        /* image window position before it was moved */
    static RECT prevToolWindowRect;         /* tool window position before it was moved */
    static int moveToolWindow=false;       /* whether or not the tool window should be moved with the main window */
    static int moveImageWindow=false;      /* whether or not the image window should be moved with the main window */
    static int imageAndMainSnapped=false;
    static int toolAndMainSnapped=false;
    static int toolAndImageSnapped=false;
    static int imageNormalState=false;
    static int toolNormalState=false;
    static RECT rect;                       /* for general use */
    
    OverviewWindow* win=(OverviewWindow*)Window::GetWindowObject(hwnd);
        
    switch (message)                  /* handle the messages */
    {
			
        /* WM_COMMAND: some command has been preformed by user, eg. menu item clicked */            
        case WM_COMMAND:
            switch( wParam )
            {
                case IDM_FILEOPEN:
                    loadFile();
                    return 0;
                  
                case IDM_FILESAVE:
                case IDM_FILESAVEAS:
					MessageBox( hwnd, (LPSTR) "This will be greyed out if no file open, else will say \"Where do you want to store such a big file?\"",
                              (LPSTR) "Overview Window",
                              MB_ICONINFORMATION | MB_OK );
                    return 0;
                
                case IDM_FILECLOSE:
                    closeFile();
                    return 0;                    
                          
             	case IDM_IMAGEWINDOW:
                    if (win->toggleMenuItemTick(win->hMainMenu,IDM_IMAGEWINDOW))
                        ShowWindow(imageWindow.GetHandle(),SW_SHOW);
                    else
                        ShowWindow(imageWindow.GetHandle(),SW_HIDE);
                    return 0;
             
                case IDM_TOOLSWINDOW:
                    if (win->toggleMenuItemTick(win->hMainMenu,IDM_TOOLSWINDOW))
                        toolWindow.Show();
                    else
                        toolWindow.Hide();
                    return 0;

                case IDM_HELPCONTENTS:
					 // get path to help folder
                     helpPath = catcstrings( (char*) modulePath, (char*) "\\help\\index.htm");

				     // launch default browser
                     ShellExecute(NULL, "open", helpPath, NULL, "help", SW_SHOWNORMAL);
                     return 0;

                case IDM_FILEEXIT:
                    SendMessage( hwnd, WM_CLOSE, 0, 0L );
                    return 0;

                case IDM_HELPABOUT:
                    MessageBox (NULL, "Created by Team Imagery, 2006" , "Parbat3D", 0);
                    return 0;
           }
           return 0;

        /* WM_NCLBUTTONDOWN: mouse button was pressed down in a non client area of the window */
        case WM_NCLBUTTONDOWN:

            /* HTCAPTION: mouse button was pressed down on the window title bar
                         (occurs when user starts to move the window)              */
            if(wParam == HTCAPTION)
            {

                /* record mouse position relative to window position */
                SnappingWindow::getMouseWindowOffset(hwnd,(int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam),&snapMouseOffset);
                     
                /* record current window positions */
                GetWindowRect(hwnd,&prevOverviewWindowRect);  
                GetWindowRect(imageWindow.GetHandle(),&prevImageWindowRect); 
                GetWindowRect(toolWindow.GetHandle(),&prevToolWindowRect);
               
                /* find out which windows are connected & which are in a normal state */
                imageNormalState=SnappingWindow::isWindowInNormalState(imageWindow.GetHandle());
                toolNormalState=SnappingWindow::isWindowInNormalState(toolWindow.GetHandle());
                imageAndMainSnapped=(SnappingWindow::isWindowSnapped(win->GetHandle(),imageWindow.GetHandle()));
                toolAndMainSnapped=(SnappingWindow::isWindowSnapped(win->GetHandle(),toolWindow.GetHandle()));
                toolAndImageSnapped=(SnappingWindow::isWindowSnapped(toolWindow.GetHandle(),imageWindow.GetHandle()));

                /* calculate whether the image window should be moved */
                if (imageNormalState)
                {
                    if (imageAndMainSnapped)
                        moveImageWindow=true;
                    else if (toolAndImageSnapped && toolNormalState && toolAndMainSnapped)
                        moveImageWindow=true;
                    else
                        moveImageWindow=false;
                }  
                else
                    moveImageWindow=false;  

                /* calculate whether the tool window should be moved */
                if (toolNormalState)
                {
                    if (toolAndMainSnapped)
                        moveToolWindow=true;
                    else if (toolAndImageSnapped && moveImageWindow)
                        moveToolWindow=true;
                    else
                        moveToolWindow=false;
                } 
                else
                    moveToolWindow=false; 

           }
           return DefWindowProc(hwnd, message, wParam, lParam);


        /* WM_MOVING: the window is about to be moved to a new location */
        case WM_MOVING:

            /* set new window position based on position of mouse */
            SnappingWindow::setNewWindowPosition((RECT*)lParam,&snapMouseOffset);            
            
            /* snap main window to edge of desktop */           
            SnappingWindow::snapInsideWindowByMoving(hDesktop,(RECT*)lParam);

            if (!moveImageWindow)
                SnappingWindow::snapWindowByMoving(imageWindow.GetHandle(),(RECT*)lParam); 

            /* snap main window to tool window, if near it, if it's not already snapped */
            if (!moveToolWindow)
                SnappingWindow::snapWindowByMoving(toolWindow.GetHandle(),(RECT*)lParam);
            
            /* move the snapped windows relative to main window's new position */
            /* only moves the windows that were already snapped to the main window */
            SnappingWindow::moveSnappedWindows((RECT*)lParam,&prevOverviewWindowRect,&prevImageWindowRect,&prevToolWindowRect,moveImageWindow,moveToolWindow);
            return 0;

        /* WM_SIZE: the window has been re-sized, minimized, maximised or restored */
        case WM_SIZE:
                
            /* resize display/opengl window to fit new size */
            GetClientRect(win->GetHandle(),&rect);
            MoveWindow(win->overviewWindowDisplay.GetHandle(),rect.left,rect.top,rect.right,rect.bottom,true);
            return 0;
                   
        /* WM_SYSCOMMAND: a system-related command associated with window needs to be executed */    
        case WM_SYSCOMMAND:
            /* check if user has tried to minimize the overview window */
            if (wParam==SC_MINIMIZE)
            {
                /* cause the main window to minimised instead */
                ShowWindow(mainWindow.GetHandle(),SW_MINIMIZE);
                return 0;
            }            
            return DefWindowProc(hwnd, message, wParam, lParam);
                     
        /* WM_CLOSE: system or user has requested to close the window/application */
        case WM_CLOSE:
            // Shut down the image file and OpenGL
			if (image_handler) { // Was instantiated
                if (MessageBox(hwnd,"Are you sure you wish quit?\nAn image is currently open.","Parbat3D",MB_YESNO|MB_ICONQUESTION)!=IDYES)
                    return 0;
                closeFile();
			}
            /* destroy this window */
            DestroyWindow( hwnd );			
            return 0;
            

        /* WM_DESTROY: window is being destroyed */
        case WM_DESTROY:
            /* Save the window location */
            RECT r;
            GetWindowRect(hwnd,&r);
            settingsFile.setSetting("overview window","x", r.left);
            settingsFile.setSetting("overview window","y", r.top);
            
            /* post a message that will cause WinMain to exit from the message loop */
            PostQuitMessage (0);
            return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);
        
        default:                   /* for messages that we don't deal with */
            /* let windows peform the default operation based on the message */
              return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
    }
    return 0; 
}
