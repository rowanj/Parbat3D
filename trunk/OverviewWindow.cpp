#include <Windows.h>
#include "main.h"
#include "OverviewWindow.h"
#include "SnappingWindow.h"
#include "ImageWindow.h"
#include "ToolWindow.h"
#include "DisplayWindow.h"
#include "MainWindow.h"

HMENU OverviewWindow::hMainMenu;
HWND OverviewWindow::hOverviewWindowDisplay;
HWND OverviewWindow::hOverviewWindow=NULL;

char szOverviewWindowClassName[] = "Parbat3D Overview Window";

/* ------------------------------------------------------------------------------------------------------------------------ */
/* Overview Window Functions */

/* register overview window's class */
int OverviewWindow::registerWindow()
{
    WNDCLASSEX wincl;        /* Datastructure for the windowclass */
        
    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szOverviewWindowClassName;
    wincl.lpfnWndProc = OverviewWindow::WindowProcedure; /* This function is called by windows */
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


/* setup overview window */
int OverviewWindow::setupWindow()
{

    RECT rect;
    
    /* get co-ords of image window */
    GetWindowRect(hImageWindow,&rect);
        
    /* Create overview window */
    OverviewWindow::hOverviewWindow = CreateWindowEx(0, szOverviewWindowClassName, "Parbat3D",
		WS_OVERLAPPED+WS_CAPTION+WS_SYSMENU+WS_MINIMIZEBOX, rect.left-OverviewWindow::OVERVIEW_WINDOW_WIDTH, rect.top, OVERVIEW_WINDOW_WIDTH, 296,
		hImageWindow, NULL, hThisInstance, NULL);
    if (!hOverviewWindow)
        return false;                        

    /* set menu to main windo */
	hMainMenu = LoadMenu(hThisInstance, MAKEINTRESOURCE(ID_MENU));
    SetMenu(hOverviewWindow, hMainMenu);    
    
    /* Disable Window menu items. note: true appears to disable & false enables */
    EnableMenuItem(hMainMenu,IDM_IMAGEWINDOW,true);
    EnableMenuItem(hMainMenu,IDM_TOOLSWINDOW,true);
    EnableMenuItem(hMainMenu,IDM_FILECLOSE,true);    

    /* get client area of image window */
    GetClientRect(OverviewWindow::hOverviewWindow,&rect);

    /* setup font objects */
    HDC hdc=GetDC(OverviewWindow::hOverviewWindow);      
    hNormalFont=CreateFont(-MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72),0,0,0,400,false,false,false,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,"Tahoma"); //"MS Sans Serif" //Tahoma    
    hBoldFont=CreateFont(-MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72),0,0,0,600,false,false,false,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,"Tahoma"); //"MS Sans Serif" //Tahoma
    hHeadingFont=CreateFont(-MulDiv(9, GetDeviceCaps(hdc, LOGPIXELSY), 72),0,0,0,600,false,false,false,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,"Tahoma"); //"MS Sans Serif" //Tahoma
    ReleaseDC(OverviewWindow::hOverviewWindow,hdc);    

    /* create a child window that will be used by OpenGL */
    hOverviewWindowDisplay=CreateWindowEx( 0, DisplayWindow::szDisplayClassName, NULL, WS_CHILD|WS_VISIBLE,
		rect.left, rect.top, rect.right, rect.bottom, hOverviewWindow, NULL, hThisInstance, NULL);
   
    /* Make the window visible on the screen */
    ShowWindow(OverviewWindow::hOverviewWindow, SW_SHOW);
    
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
                              (LPSTR) szOverviewWindowClassName,
                              MB_ICONINFORMATION | MB_OK );
                    return 0;
                
                case IDM_FILECLOSE:
                    closeFile();
                    return 0;                    
                          
             	case IDM_IMAGEWINDOW:
                    if (toggleMenuItemTick(hMainMenu,IDM_IMAGEWINDOW))
                        ShowWindow(hImageWindow,SW_SHOW);                        
                    else
                        ShowWindow(hImageWindow,SW_HIDE);
                    return 0;
             
                case IDM_TOOLSWINDOW:
                    if (toggleMenuItemTick(hMainMenu,IDM_TOOLSWINDOW))
                        ShowWindow(hToolWindow,SW_SHOW);                        
                    else
                        ShowWindow(hToolWindow,SW_HIDE);
                    return 0;

                case IDM_HELPCONTENTS:
                     ShellExecute(NULL, "open", "\\help\\index.htm", NULL, "help", SW_SHOW);
					
					//WinHelp( hwnd, (LPSTR) "help/index.htm",
                           //HELP_CONTENTS, 0L );
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
                getMouseWindowOffset(hwnd,(int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam),&snapMouseOffset);
                     
                /* record current window positions */
                GetWindowRect(hwnd,&prevOverviewWindowRect);  
                GetWindowRect(hImageWindow,&prevImageWindowRect); 
                GetWindowRect(hToolWindow,&prevToolWindowRect);
               
                /* find out which windows are connected & which are in a normal state */
                imageNormalState=isWindowInNormalState(hImageWindow);
                toolNormalState=isWindowInNormalState(hToolWindow);
                imageAndMainSnapped=(isWindowSnapped(hOverviewWindow,hImageWindow));
                toolAndMainSnapped=(isWindowSnapped(hOverviewWindow,hToolWindow));
                toolAndImageSnapped=(isWindowSnapped(hToolWindow,hImageWindow));

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
            setNewWindowPosition((RECT*)lParam,&snapMouseOffset);            
            
            /* snap main window to edge of desktop */           
            snapInsideWindowByMoving(hDesktop,(RECT*)lParam);

            if (!moveImageWindow)
                snapWindowByMoving(hImageWindow,(RECT*)lParam); 

            /* snap main window to tool window, if near it, if it's not already snapped */
            if (!moveToolWindow)
                snapWindowByMoving(hToolWindow,(RECT*)lParam);
            
            /* move the snapped windows relative to main window's new position */
            /* only moves the windows that were already snapped to the main window */
            moveSnappedWindows((RECT*)lParam,&prevOverviewWindowRect,&prevImageWindowRect,&prevToolWindowRect,moveImageWindow,moveToolWindow);
            return 0;

        /* WM_SIZE: the window has been re-sized, minimized, maximised or restored */
        case WM_SIZE:
                
            /* resize display/opengl window to fit new size */
            GetClientRect(OverviewWindow::hOverviewWindow,&rect);
            MoveWindow(OverviewWindow::hOverviewWindowDisplay,rect.left,rect.top,rect.right,rect.bottom,true);
            return 0;
                   
        /* WM_SYSCOMMAND: a system-related command associated with window needs to be executed */    
        case WM_SYSCOMMAND:
            /* check if user has tried to minimize the overview window */
            if (wParam==SC_MINIMIZE)
            {
                /* cause the main window to minimised instead */
                ShowWindow(hMainWindow,SW_MINIMIZE);
                return 0;
            }            
            return DefWindowProc(hwnd, message, wParam, lParam);
                     
        /* WM_CLOSE: system or user has requested to close the window/application */
        case WM_CLOSE:
            // Shut down the image file and OpenGL
			if (image_handler) { // Was instantiated
                if (MessageBox(OverviewWindow::hOverviewWindow,"Are you sure you wish quit?\nAn image is currently open.","Parbat3D",MB_YESNO|MB_ICONQUESTION)!=IDYES)
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
            winPos.setSetting("OverviewX", r.left);
            winPos.setSetting("OverviewY", r.top);
            
            /* post a message that will cause WinMain to exit from the message loop */
            PostQuitMessage (0);
            return 0;
        
        default:                   /* for messages that we don't deal with */
            /* let windows peform the default operation based on the message */
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0; 
}
