#include <Windows.h>
#include <Commctrl.h>
#include <iostream>
#include "main.h"
#include "config.h"
#include "Settings.h"
#include "ImageHandler.h"
#include "ImageProperties.h"
#include "BandInfo.h"
#include "StringUtils.h"


ImageHandler::ImageHandler* image_handler;	// Instance handle

using namespace std;

/* Unique class names for our main windows */
char szMainWindowClassName[] = "Parbat3D Main Window";
char szImageWindowClassName[] = "Parbat3D Image Window";
char szToolWindowClassName[] = "Parbat3D Tool Window";
char szDisplayClassName[] = "Parbat3D Display Window";
/* pre-defined class names for controls */
char szStaticControl[] = "static";  /* static text control */

/* a handle that identifies our process */
HINSTANCE hThisInstance;

/* global variables to store handles to our windows */
HWND hRed, hBlue, hGreen;
HWND hMainWindow;
HWND hImageWindow;
HWND hToolWindow;
HWND hDesktop;

/* global variables to store handles to our window controls */
HMENU hMainMenu;
HWND hToolWindowTabControl;
HWND hToolWindowDisplayTabContainer;
HWND hToolWindowQueryTabContainer;
HWND hToolWindowImageTabContainer;
HWND hRedRadioButton1, hRedRadioButton2;
HWND hGreenRadioButton1, hGreenRadioButton2;
HWND hBlueRadioButton1, hBlueRadioButton2;
HWND hImageWindowDisplay;
HWND hMainWindowDisplay;

/* radio button globals */
int bands = 6;
HWND *redRadiobuttons;
HWND *greenRadiobuttons;
HWND *blueRadiobuttons;

/* update button global */
HWND hupdate;

/* Variables to record when the windows have snapped to main wndow */
int imageWindowIsSnapped=false;
int toolWindowIsSnapped=false;

/* Define id numbers for the tab's in the tool window */
enum {DISPLAY_TAB_ID,QUERY_TAB_ID,IMAGE_TAB_ID};

/* Used for loading and saving window position and sizes */
settings winPos ("settings.ini");

/* objects used for painting/drawing */
HFONT hBoldFont,hNormalFont;
HPEN hTabPen;
HBRUSH hTabBrush;
WNDPROC oldTabControlProc,oldDisplayTabContainerProc,oldQueryTabContainerProc,oldImageTabContainerProc;


/* program entry point */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)
{
 	MSG messages;            /* Here messages to the application is saved */
 
    /* load window classes for common controls */
    InitCommonControls();
    
    // store this process's instance handle
    hThisInstance=hInstance;
    
    registerToolWindow();
    
    // get desktop window handle
    hDesktop=GetDesktopWindow();
    
    // create & show main window
    if (!setupMainWindow())
        return 0;
  
    /* Run the messageloop. It will run until GetMessage( ) returns 0 */
    while(GetMessage(&messages, NULL, 0, 0))
    {
        /* Send message to associated WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program returvalue is 0 - The value that PostQuitMessage( ) gave */
    return messages.wParam;
}

/* ------------------------------------------------------------------------------------------------------------------------ */
/* Main Window Functions */

int setupMainWindow()
{
    WNDCLASSEX wincl;        /* Datastructure for the windowclass */
    RECT rect;

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szMainWindowClassName;
    wincl.lpfnWndProc = MainWindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Ctach double-clicks */
    wincl.cbSize = sizeof(WNDCLASSEX);

    /* Use default icon and mousepointer */
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL; /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use lightgray as the background of the window */
    wincl.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);

    /* Register the window class, if fails return false */
    if(!RegisterClassEx(&wincl)) return false;
    
    
    /* Create window class for the display control windows */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szDisplayClassName;
    wincl.lpfnWndProc = DisplayWindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Ctach double-clicks */
    wincl.cbSize = sizeof(WNDCLASSEX);
    /* Use default icon and mousepointer */
    wincl.hIcon = NULL;
    wincl.hIconSm = NULL;
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL; /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    wincl.hbrBackground = NULL;
    /* Register the window class, if fails return false */
    if(!RegisterClassEx(&wincl)) return false;    
    
    /* Get the stored window position or use defaults if there's a problem */
    int mx = atoi(winPos.getSetting("OverviewX").c_str());
	int my = atoi(winPos.getSetting("OverviewY").c_str());
	if (mx <= 0) mx = CW_USEDEFAULT;
	if (my <= 0) my = CW_USEDEFAULT;
    
    /* The class is registered, lets create a window based on it*/
    hMainWindow = CreateWindowEx(
           0,                   /* Extended possibilites for variation */
           szMainWindowClassName,         /* Classname */
           "Parbat3D",          /* Title Text */
           WS_OVERLAPPED+WS_CAPTION+WS_SYSMENU+WS_MINIMIZEBOX, /* window styles */
           mx,                  /* x position */
           my,                  /* y position */
           250,                 /* The program's width */
           296,                 /* and height in pixels */
           hMainWindow,         /* The window is a childwindow to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );
    if (!hMainWindow)
        return false;                        

    hMainMenu = LoadMenu(hThisInstance, MAKEINTRESOURCE(ID_MENU));
    SetMenu(hMainWindow, hMainMenu);    
    
    /* Disable Window menu items */
    EnableMenuItem(hMainMenu,IDM_IMAGEWINDOW,true);     /* note: true appears to disable & false enables */
    EnableMenuItem(hMainMenu,IDM_TOOLSWINDOW,true);

    /* get client area of image window */
    GetClientRect(hMainWindow,&rect);

    /* setup font objects */
    HDC hdc=GetDC(hMainWindow);      
    hNormalFont=CreateFont(-MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72),0,0,0,400,false,false,false,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,"Tahoma"); //"MS Sans Serif" //Tahoma    
    hBoldFont=CreateFont(-MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72),0,0,0,600,false,false,false,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,"Tahoma"); //"MS Sans Serif" //Tahoma
    ReleaseDC(hMainWindow,hdc);    

    /* create a child window that will be used by OpenGL */
    hMainWindowDisplay=CreateWindowEx(
           0,                   /* Extended possibilites for variation */
           szDisplayClassName,         /* Classname */
           NULL,         /* Title Text */
           WS_CHILD|WS_VISIBLE, /* styles */
           rect.left,       /* x position based on parent window */
           rect.top,         /* y position based on parent window */
           rect.right,                 /* The programs width */
           rect.bottom,                 /* and height in pixels */
           hMainWindow,        /* The window's parent window */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );
   
    
    /* Make the window visible on the screen */
    ShowWindow(hMainWindow, SW_SHOW);
    
    return true;
}

/* This function is called by the Windows function DispatchMessage( ) */
/* All messages/events related to the main window (or it's controls) are sent to this procedure */
LRESULT CALLBACK MainWindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    /* static variables used for snapping windows */
    static POINT snapMouseOffset;           /* mouse co-ords relative to position of window */
    static RECT prevMainWindowRect;         /* main window position before it was moved */
    static RECT prevImageWindowRect;        /* image window position before it was moved */
    static RECT prevToolWindowRect;         /* tool window position before it was moved */
    static int toolWindowIsMovingToo;       /* whether or not the tool window should be moved with the main window */
    static int imageWindowIsMovingToo;      /* whether or not the image window should be moved with the main window */
    static RECT rect;                       /* for general use */
    
    switch (message)                  /* handle the messages */
    {
			
        /* WM_CREATE: window has been created */
        case WM_CREATE:                
            hImageWindow=NULL;
            hToolWindow=NULL;
            break;

        /* WM_COMMAND: some command has been preformed by user, eg. menu item clicked */            
        case WM_COMMAND:
            /* if menu item clicked, wParam==resource id number of menu item (defined in rsrc.rc) */
            switch( wParam )
            {
                case IDM_FILEOPEN:
                    loadFile();
                    return 0;
                  
                case IDM_FILESAVE:
                case IDM_FILESAVEAS:
					MessageBox( hwnd, (LPSTR) "This will be greyed out if no file open, else will say \"Where the #$%@ do you want to store such a big file?\"",
                              (LPSTR) szMainWindowClassName,
                              MB_ICONINFORMATION | MB_OK );
                    return 0;
                          
             	case IDM_IMAGEWINDOW:
                    ShowWindow(hImageWindow,SW_SHOW);

                    /*MessageBox( hwnd, (LPSTR) "This will be greyed out if no file open, else will reopen image window",
                              (LPSTR) szMainWindowClassName,
                              MB_ICONINFORMATION | MB_OK );*/
                    return 0;
             
                case IDM_TOOLSWINDOW:
                     ShowWindow(hToolWindow,SW_SHOW);
                    
                    /*MessageBox( hwnd, (LPSTR) "This will be greyed out if no file open, else will reopen image window",
                              (LPSTR) szMainWindowClassName,
                              MB_ICONINFORMATION | MB_OK );*/
                    return 0;

                case IDM_HELPCONTENTS:
                    WinHelp( hwnd, (LPSTR) "PARBAT3D.HLP",
                           HELP_CONTENTS, 0L );
                    return 0;

                case IDM_FILEEXIT:
                    SendMessage( hwnd, WM_CLOSE, 0, 0L );
                    return 0;

                case IDM_HELPABOUT:
                    MessageBox (NULL, "Created by Team Imagery, 2006" , "Parbat3D", 0);
                    return 0;
           }
           break;

        /* WM_NCLBUTTONDOWN: mouse button was pressed down in a non client area of the window */
        case WM_NCLBUTTONDOWN:
            /* wParam=the area of the window where the mouse button was pressed */

            /* HTCAPTION: mouse button was pressed down on the window title bar
                         (occurs when user starts to move the window)              */
            if(wParam == HTCAPTION)
            {
                /* record mouse position relative to window position */
                getMouseWindowOffset(hwnd,(int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam),&snapMouseOffset);
                     
                /* record current window positions */
                GetWindowRect(hwnd,&prevMainWindowRect);  
                GetWindowRect(hImageWindow,&prevImageWindowRect); 
                GetWindowRect(hToolWindow,&prevToolWindowRect);
               
                /* record which windows are currently snapped (which ones should move with the main window) */
                imageWindowIsMovingToo=((imageWindowIsSnapped)&&(isWindowInNormalState(hImageWindow)));
                toolWindowIsMovingToo=((toolWindowIsSnapped)&&(isWindowInNormalState(hToolWindow)));  
           }
           return DefWindowProc(hwnd, message, wParam, lParam); 

        /* WM_MOVING: the window is about to be moved to a new location */
        case WM_MOVING:
            /* lParam=the new position, which can be modified before the window is moved */

            /* set new window position based on position of mouse */
            setNewWindowPosition((RECT*)lParam,&snapMouseOffset);            
            
            /* snap main window to edge of desktop */           
            snapWindowByMoving(hDesktop,(RECT*)lParam);

            /* bug: when calling snapWindow the second time the main window does not visually snap to the tool window, but snapWindow still returns true */
            /* snap main window to image window, if near it, if it's not already snapped */
            if (!imageWindowIsMovingToo)
                imageWindowIsSnapped=snapWindowByMoving(hImageWindow,(RECT*)lParam); 

            /* snap main window to tool window, if near it, if it's not already snapped */
            if (!toolWindowIsMovingToo)
                toolWindowIsSnapped=snapWindowByMoving(hToolWindow,(RECT*)lParam);
            
            /* move the snapped windows relative to main window's new position */
            /* only moves the windows that were already snapped to the main window */
            moveSnappedWindows((RECT*)lParam,&prevMainWindowRect,&prevImageWindowRect,&prevToolWindowRect,imageWindowIsMovingToo,toolWindowIsMovingToo);
            break;

        /* WM_SIZE: the window has been re-sized, minimized, maximised or restored */
        case WM_SIZE:
            /* resize display/opengl window to fit new size */
            GetClientRect(hMainWindow,&rect);
            MoveWindow(hMainWindowDisplay,rect.left,rect.top,rect.right,rect.bottom,true);
            break;   
                     
        /* WM_CLOSE: system or user has requested to close the window/application */
        case WM_CLOSE:
			// Shut down the image file and OpenGL
			if (image_handler) { // Was instantiated
				delete image_handler;
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
            break;
        
        default:                   /* for messages that we don't deal with */
            /* let windows peform the default operation based on the message */
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    /* return true to indicate that we have processed the message */    
    return 0; 
}

/* This function is called by the Windows function DispatchMessage( ) */
/* All messages/events related to one of the display windows are sent to this procedure */
LRESULT CALLBACK DisplayWindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static PAINTSTRUCT ps;
    static HDC hdc;
    static RECT rect;
    static HBRUSH hbrush;
    static SIZE textSize;
    static POINT textPos;
    static char text[]="No Image Loaded";
    const int textLen=16;
    
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            /* init variables used for painting "No Image Loaded" message */
            hbrush=CreateSolidBrush(0);
            hdc=GetDC(hwnd);                                                                      /* get device context (drawing) object */
            SelectObject(hdc,hNormalFont);                                                   /* set font that will be used for drawing text */    
            GetTextExtentPoint32(hdc,text,textLen,&textSize);     /* get width & height of string in pixels */   
            ReleaseDC(hwnd,hdc);
            
            break;            
        case WM_SIZE:           
            if (image_handler) 
            {
                /* Re-size OpenGL stuff */
                image_handler->resize_window();
            }    
            else
            {
                /* re-position message */
                GetClientRect(hwnd,&rect);
                textPos.x=(rect.right-rect.left)/2 - textSize.cx/2;
                textPos.y=(rect.bottom-rect.top)/2 - textSize.cy/2;            
            }    
            break;
            
		case WM_PAINT:
            hdc=BeginPaint(hwnd,&ps);
			if (image_handler)
    			/* Re-draw OpenGL stuff */
                image_handler->redraw();
            else
            {
                /* Display "No Image Loaded" on black background */
                SelectObject(hdc,hbrush);
                Rectangle(hdc,0,0,rect.right,rect.bottom);
                SelectObject(hdc,hNormalFont);
                SetTextColor(hdc,RGB(255,255,255));
                SetBkColor(hdc,0);
                TextOut(hdc,textPos.x,textPos.y,text,textLen);
            }
			EndPaint(hwnd,&ps);
			break;

    default:                   /* for messages that we don't deal with */
            /* let windows peform the default operation based on the message */
            return DefWindowProc(hwnd, message, wParam, lParam);
    }			
    /* return true to indicate that we have processed the message */    
    return 0; 
}


/* ------------------------------------------------------------------------------------------------------------------------ */
/* Image Window Functions */

/* create the image window */
int setupImageWindow()
{
    WNDCLASSEX wincl;        /* Datastructure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;                /* process's instance handle */
    wincl.lpszClassName = szImageWindowClassName;   /* our unique name for this class */
    wincl.lpfnWndProc = ImageWindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Ctach double-clicks */
    wincl.cbSize = sizeof(WNDCLASSEX);

    /* Use default icon and mousepointer */
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL; /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use lightgray as the background of the window */
    wincl.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);

    /* Register the window class, if fails return false */
    if(!RegisterClassEx(&wincl)) return false;
    
    /* Get Main Window coords for Image Window alignment*/
    RECT rect;
    GetWindowRect(hMainWindow, &rect);
    
    /* Create a window based on the class we just registered */
    hImageWindow =CreateWindowEx(
           WS_EX_OVERLAPPEDWINDOW,                   /* Extended possibilites for variation */
           szImageWindowClassName,         /* Classname */
           "Image Window",         /* Title Text */
           WS_POPUP+WS_SYSMENU+WS_CAPTION+WS_MINIMIZEBOX+WS_MAXIMIZEBOX+WS_VSCROLL+WS_HSCROLL, /* defaultwindow */
           rect.right,       /* x position based on main window */
           rect.top,         /* y position based on main window */
           700,                 /* The programs width */
           600,                 /* and height in pixels */
           0,        /* The window is a childwindow to main window */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* get client area of image window */
    GetClientRect(hImageWindow,&rect);

    /* create a child window that will be used by OpenGL */
    hImageWindowDisplay=CreateWindowEx(
           0,                   /* Extended possibilites for variation */
           szDisplayClassName,         /* Classname */
           NULL,         /* Title Text */
           WS_CHILD+WS_VISIBLE, /* styles */
           rect.left,       /* x position based on parent window */
           rect.top,         /* y position based on parent window */
           rect.right,                 /* The programs width */
           rect.bottom,                 /* and height in pixels */
           hImageWindow,        /* The window's parent window */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    if (hImageWindow==NULL)
        return false;

    imageWindowIsSnapped=true;

    return true;
}

/* This function is called by the Windowsfunction DispatchMessage( ) */
/* All messages/events related to the image window (or it's controls) are sent to this procedure */
LRESULT CALLBACK ImageWindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static POINT moveMouseOffset;     /* mouse offset relative to window, used for snapping */
    static POINT sizeMousePosition;   /* mouse position, used for sizing window */
    static RECT  sizeWindowPosition;   /* window position, used for sizing window */
    static RECT rect;                   /* for general use */
    
    switch (message)                  /* handle the messages */
    {
        /* WM_NCLBUTTONDOWN: mouse button was pressed down in a non client area of the window */        
        case WM_NCLBUTTONDOWN:
            /* wParam=the area of the window where the mouse button was pressed */

            switch(wParam)
            {
                /* HTCAPTION: mouse button was pressed down on the window title bar */                            
                case HTCAPTION:
                    /* get the mouse co-ords relative to the window */
                    getMouseWindowOffset(hwnd,(int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam),&moveMouseOffset);               
                    break;

                /* HTLEFT...HTBOTTOMRIGHT: mouse button was pressed down on the sizing border of window */  
                case HTLEFT:
                case HTRIGHT:
                case HTTOP:
                case HTBOTTOM:
                case HTTOPLEFT:
                case HTTOPRIGHT:
                case HTBOTTOMLEFT:
                case HTBOTTOMRIGHT:
                    /* record current window & mouse positions */
                    GetWindowRect(hwnd,&sizeWindowPosition);
                    sizeMousePosition.x=(int)(short)LOWORD(lParam);
                    sizeMousePosition.y=(int)(short)HIWORD(lParam);                   
                    break;                   
            }

            /* also let windows handle this event */
            return DefWindowProc(hwnd, message, wParam, lParam); 
        
            
        /* WM_MOVING: the window is about to be moved to a new location */
        case WM_MOVING:

            /* set new window position based on position of mouse */
            setNewWindowPosition((RECT*)lParam,&moveMouseOffset);

            /* snap the window to the edge of the desktop (if near it) */
            snapWindowByMoving(hDesktop,(RECT*)lParam);      
            
            /* snap the window the main window (if near it) */
            imageWindowIsSnapped=snapWindowByMoving(hMainWindow,(RECT*)lParam);
            break;
    
        /* WM_SIZING: the window size is about to change */
        case WM_SIZING:

            /* set new window size based on position of mouse */
            setNewWindowSize((RECT*)lParam,&sizeWindowPosition,&sizeMousePosition,(int)wParam);

            /* snap the window to the edge of the desktop (if near it) */
            snapWindowBySizing(hDesktop,(RECT*)lParam,(int)wParam);   
                        
            /* snap the window the main window (if near it) */
            imageWindowIsSnapped=snapWindowBySizing(hMainWindow,(RECT*)lParam,(int)wParam);           
            break;

        /* WM_SIZE: the window has been resized, minimized, or maximizsed, etc. */            
        case WM_SIZE:
            if (imageWindowIsSnapped)
            {
                // re-snap window in case the main window has now moved away
                GetWindowRect(hwnd,&rect);
                imageWindowIsSnapped=snapWindowByMoving(hwnd,&rect);
            }    
           
            /* resize display/opengl window to fit new size */            
            GetClientRect(hImageWindow,&rect);
            MoveWindow(hImageWindowDisplay,rect.left,rect.top,rect.right,rect.bottom,true);
            break;

        /* WM_CLOSE: system or user has requested to close the window/application */             
        case WM_CLOSE:
            /* don't destroy this window, but make it invisible */
            ShowWindow(hwnd,SW_HIDE);
            //DestroyWindow(hwnd);
            //hImageWindow=NULL;
            return 0;

        /* WM_DESTORY: system is destroying our window */
        case WM_DESTROY:
			/* send a message that will cause WinMain to exit the message loop */
            PostQuitMessage (0);
            return 0;
            
            
        default:
            /* let windows handle any unknown messages */
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    /* return 0 to indicate that we have processed the message */       
    return 0;
}

/* ------------------------------------------------------------------------------------------------------------------------ */
/* Tools Window Functions */

int registerToolWindow()
{
    WNDCLASSEX wincl;       /* Datastructure for the windowclass */
    
    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szToolWindowClassName;
    wincl.lpfnWndProc = ToolWindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Ctach double-clicks */
    wincl.cbSize = sizeof(WNDCLASSEX);

    /* Use default icon and mousepointer */
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL; /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use lightgray as the background of the window */
    wincl.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);

    /* Register the window class, if it fails return false */
    if(!RegisterClassEx(&wincl)) return false;    
}    

/* create tool window */
int setupToolWindow()
{
    TCITEM tie;             /* datastructure for tabs */
    RECT rect;
    
    /* Get Main Window Location for image window alignment*/
    GetWindowRect(hMainWindow,&rect);
    
    /* The class is registered, lets create the program*/
    hToolWindow =CreateWindowEx(
           WS_EX_TOOLWINDOW+WS_EX_TOPMOST,                              /* Extended styles */
           szToolWindowClassName,         /* Classname */
           "Tools",                         /* Title Text */
           WS_POPUP+WS_CAPTION+WS_SYSMENU+WS_MINIMIZEBOX+WS_VSCROLL, /* defaultwindow */
           rect.left,       /* Windows decides the position */
           rect.bottom,       /* where the window end up on the screen */
           250,                 /* The programs width */
           300,                 /* and height in pixels */
           hImageWindow,        /* The window is a childwindow to main window */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    if (hToolWindow==NULL)
        return false;
    
    /* indicate tool window as snapped to the main window */
    toolWindowIsSnapped=true;

    /* get width & height of tool window's client area (ie. inside window's border) */
    GetClientRect(hToolWindow,&rect);

    /* create tab control */
    hToolWindowTabControl =CreateWindowEx(
           0,                       /* Extended possibilites for variation */
           WC_TABCONTROL,           /* Classname */
           "Tools",                 /* Title Text */
           WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | TCS_OWNERDRAWFIXED,
           0,                       /* Top co-ord relative to tool window */
           0,                       /* Left co-ord relative to tool window */
           rect.right,              /* Set width to tool window's client area width */
           rect.bottom,             /* Set height to tool window's client area height */
           hToolWindow,             /* The window is a childwindow to tool window */
           NULL,                    /* No menu */
           hThisInstance,           /* Process's Instance handle */
           NULL                     /* No Window Creation data */
           );

    /* modify tab control's window procedure address */
    oldTabControlProc=(WNDPROC)SetWindowLong(hToolWindowTabControl,GWL_WNDPROC,(long)&ToolWindowTabControlProcedure);

    /* add tabs to tab-control */
    tie.mask=TCIF_TEXT+TCIF_PARAM;;
    tie.pszText="Display";
    tie.lParam=(DWORD)tie.pszText;
    TabCtrl_InsertItem(hToolWindowTabControl, DISPLAY_TAB_ID, &tie);
    
    tie.mask=TCIF_TEXT+TCIF_PARAM;;   
    tie.pszText="Query";
    tie.lParam=(DWORD)tie.pszText;    
    TabCtrl_InsertItem(hToolWindowTabControl, QUERY_TAB_ID, &tie);
    
    tie.mask=TCIF_TEXT+TCIF_PARAM;;   
    tie.pszText="Image";
    tie.lParam=(DWORD)tie.pszText;    
    TabCtrl_InsertItem(hToolWindowTabControl, IMAGE_TAB_ID, &tie);

    /* get size of tab control's client area (the area inside the tab control) */
    GetClientRect(hToolWindowTabControl,&rect);   

    /* calculate the width & height for our tab container windows */
    const int SPACING_FOR_TAB_HEIGHT=30;    /* the height of the tabs + a bit of spacing */
    const int SPACING_FOR_BOARDER=5;        /* left & right margain + spacing for tab control's borders */
    rect.bottom-=SPACING_FOR_TAB_HEIGHT+SPACING_FOR_BOARDER;
    rect.right-=SPACING_FOR_BOARDER+SPACING_FOR_BOARDER;
    
    /* create tab containers for each tab (a window that will be shown/hidden when user clicks on a tab) */
    hToolWindowDisplayTabContainer =CreateWindowEx(
           0,                               /* Extended styles */
           szStaticControl,                 /* pre-defined static text control classname */
           "Channel Selection",         /* text to be displayed */
           WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_OWNERDRAW, /* window styles */
           SPACING_FOR_BOARDER,             /* left position relative to tab control */
           SPACING_FOR_TAB_HEIGHT,          /* top position relative to tab control */
           rect.right,                      /* the width of the container */
           rect.bottom,                     /* the height of the container */
           hToolWindowTabControl,           /* The window is a childwindow of the tab control */
           NULL,                            /* No menu */
           hThisInstance,                   /* Program's Instance handle */
           NULL                             /* No Window Creation data */
           ); 
           
    hToolWindowQueryTabContainer =CreateWindowEx(
           0,                               /* Extended styles */
           szStaticControl,                 /* pre-defined classname for static text control */
           "Band Values",                   /* text to display */
           WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_OWNERDRAW, /* styles */
           SPACING_FOR_BOARDER,             /* left position relative to tab control */
           SPACING_FOR_TAB_HEIGHT,          /* top position relative to tab control */
           rect.right,                      /* the width of the container */
           rect.bottom,                     /* the height of the container */
           hToolWindowTabControl,           /* The window is a childwindow of the tab control */
           NULL,                            /* No menu */
           hThisInstance,                   /* Program Instance handler */
           NULL                             /* No Window Creation data */
           );
           
    hToolWindowImageTabContainer =CreateWindowEx(
           0,                               /* Extended styles */
           szStaticControl,                 /* pre-defined classname for static text control */
           "Image Properties",              /* text to display */
           WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_OWNERDRAW, /* styles */
           SPACING_FOR_BOARDER,             /* left position relative to tab control */
           SPACING_FOR_TAB_HEIGHT,          /* top position relative to tab control */
           rect.right,                      /* the width of the container */
           rect.bottom,                     /* the height of the container */
           hToolWindowTabControl,           /* The window is a childwindow of the tab control */
           NULL,                            /* No menu */
           hThisInstance,                   /* Program Instance handler */
           NULL                             /* No Window Creation data */
           ); 
    

    /* modify tab containers' window procedure address 
        note: assumming proc addr is the same for all three */
    oldDisplayTabContainerProc=(WNDPROC)SetWindowLong(hToolWindowDisplayTabContainer,GWL_WNDPROC,(long)&ToolWindowDisplayTabContainerProcedure);
    oldQueryTabContainerProc=(WNDPROC)SetWindowLong(hToolWindowQueryTabContainer,GWL_WNDPROC,(long)&ToolWindowQueryTabContainerProcedure);
    oldImageTabContainerProc=(WNDPROC)SetWindowLong(hToolWindowImageTabContainer,GWL_WNDPROC,(long)&ToolWindowImageTabContainerProcedure);


    bands = image_handler->get_image_properties()->getNumBands();
    
    /* Create group for RED radio buttons based on band number */
    hRed = CreateWindowEx(
	    	0,
			"BUTTON",
			"R", //title
			WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
			138, //int x,
			25, //int y, CW_USEDEFAULT
			26, //int nWidth,
			20 + (20 * bands), //int nHeight
			hToolWindowDisplayTabContainer, //parent window     
			NULL, //no menu
			hThisInstance, //HINSTANCE hInstance,
			NULL //pointer not needed
			);


		
	/* Create group for GREEN radio buttons based on band number */
    hGreen = CreateWindowEx(
	    	0,
			"BUTTON",
			"G", //title
			WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
			164, //int x,
			25, //int y, CW_USEDEFAULT
			26, //int nWidth,
			20 + (20 * bands), //int nHeight
			hToolWindowDisplayTabContainer, //parent window     
			NULL, //no menu
			hThisInstance, //HINSTANCE hInstance,
			NULL //pointer not needed
			);
		
	/* Create group for BLUE radio buttons based on band number */
    hBlue = CreateWindowEx(
	    	0,
			"BUTTON",
			"B", //title
			WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
			190, //int x,
			25, //int y, CW_USEDEFAULT
			26, //int nWidth,
			20 + (20 * bands), //int nHeight
			hToolWindowDisplayTabContainer, //parent window     
			NULL, //no menu
			hThisInstance, //HINSTANCE hInstance,
			NULL //pointer not needed
			);
	
	/* Create container for band values */
	HWND queryValueContainer = CreateWindowEx(
	    	0,
			"BUTTON",
			"Values", //title
			WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
			138, //int x,
			25, //int y, CW_USEDEFAULT
			66, //int nWidth,
			20 + (20 * bands), //int nHeight
			hToolWindowQueryTabContainer, //parent window     
			NULL, //no menu
			hThisInstance, //HINSTANCE hInstance,
			NULL //pointer not needed
			);

	/* Dynamically add Radio buttons */
	redRadiobuttons=new HWND[bands];
	greenRadiobuttons=new HWND[bands];
	blueRadiobuttons=new HWND[bands];
	
    for (int i=0; i<bands; i++)
    {
		redRadiobuttons[i] = CreateWindowEx(
	    	0,
			"BUTTON",
			NULL, //title
			WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
			5, //int x,
			15 + (20 * i), //int y, CW_USEDEFAULT
			18, //int nWidth,
			18, //int nHeight
			hRed, //parent window     
			NULL, //no menu
			hThisInstance, //HINSTANCE hInstance,
			NULL //pointer not needed
			);
			
		greenRadiobuttons[i] = CreateWindowEx(
	    	0,
			"BUTTON",
			NULL, //title
			WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
			5, //int x,
			15 + (20 * i), //int y, CW_USEDEFAULT
			18, //int nWidth,
			18, //int nHeight
			hGreen, //parent window     
			NULL, //no menu
			hThisInstance, //HINSTANCE hInstance,
			NULL //pointer not needed
			);
			
		blueRadiobuttons[i] = CreateWindowEx(
	    	0,
			"BUTTON",
			NULL, //title
			WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
			5, //int x,
			15 + (20 * i), //int y, CW_USEDEFAULT
			18, //int nWidth,
			18, //int nHeight
			hBlue, //parent window     
			NULL, //no menu
			hThisInstance, //HINSTANCE hInstance,
			NULL //pointer not needed
			);
		
		/* add band names to radio buttons*/
		const char *name = image_handler->get_band_info(i+1)->getColourInterpretationName();
		
		/* If Colour name unknown change band name*/
		const char *altName ="No colour name";
		if (strcmp(name, "Unknown")==0)
          name = altName;
            
        /* Add band number to band name */
        name = catcstrings( (char*) "). ", (char*) name);
        name = catcstrings( (char*) inttocstring(i+1), (char*) name);
		
        /* Display band name in tool windo */
		CreateWindowEx(
           0,                   /* Extended possibilites for variation */
           szStaticControl,     /* Classname */
           name,        		/* Title Text */
           WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, /* defaultwindow */
           20,       			/* Windows decides the position */
           40 + (20 * i),       /* where the window end up on the screen */
           100,                 /* The programs width */
           18,                  /* and height in pixels */
           hToolWindowDisplayTabContainer,        /* The window is a childwindow to main window */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );
           
        /* add channel names under the query tab */
        CreateWindowEx(
	    	0,
			szStaticControl,
			name, //title
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, /* defaultwindow */
			20, //int x,
			40 + (20 * i), //int y, CW_USEDEFAULT
			100, //int nWidth,
			18, //int nHeight
			hToolWindowQueryTabContainer, //parent window     
			NULL, //no menu
			hThisInstance, //HINSTANCE hInstance,
			NULL //pointer not needed
			);
			
			
		/* Insert 'Update' button under radio buttons. Location based on band number */
		hupdate =  CreateWindowEx(
			0,
			"BUTTON",
			"Update",
			WS_CHILD | WS_VISIBLE,
			136,50 + (20 * bands), // x y location
			80,25, // x y dimensions
			hToolWindowDisplayTabContainer, //parent window     
			NULL, //no menu
			hThisInstance, //HINSTANCE hInstance,
			NULL //pointer not needed
			);     

			
		/* add the band values to the value container under the query tab */
        char tempBandValue[4] = "128"; // temporary storage for the band value
        CreateWindowEx(
	    	0,
			szStaticControl,
			tempBandValue, //title
			WS_CHILD | WS_VISIBLE,
			5, //int x,
			15 + (20 * i), //int y, CW_USEDEFAULT
			50, //int nWidth,
			18, //int nHeight
			queryValueContainer, //parent window     
			NULL, //no menu
			hThisInstance, //HINSTANCE hInstance,
			NULL //pointer not needed
			);
	}
	
	/* add the image property information under the image tab */
	ImageProperties* ip=image_handler->get_image_properties();
	string leader;
	int ipItems=5;
	string n[ipItems];
	string v[ipItems];
	n[0]="File Name"; v[0]="Unknown";
	n[1]="File Type"; v[1]=makeMessage(leader, (char*) ip->getDriverLongName());
	n[2]="Width"; v[2]=makeMessage(leader, ip->getWidth());
	n[3]="Height"; v[3]=makeMessage(leader, ip->getHeight());
	n[4]="Bands"; v[4]=makeMessage(leader, ip->getNumBands());
	
	for (int i=0; i<ipItems; i++) {
		CreateWindowEx(
	    	0,
			szStaticControl,
			(char*) n[i].c_str(),          //title
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, /* defaultwindow */
			20,                             //int x,
			40+(i*20),                     //int y, CW_USEDEFAULT
			100,                           //int nWidth,
			18,                            //int nHeight
			hToolWindowImageTabContainer,  //parent window     
			NULL,                          //no menu
			hThisInstance,                 //HINSTANCE hInstance,
			NULL                           //pointer not needed
			);
		
		CreateWindowEx(
	    	0,
			szStaticControl,
			(char*) v[i].c_str(),          //title
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, /* defaultwindow */
			138,                           //int x,
			40+(i*20),                     //int y, CW_USEDEFAULT
			100,                           //int nWidth,
			18,                            //int nHeight
			hToolWindowImageTabContainer,  //parent window     
			NULL,                          //no menu
			hThisInstance,                 //HINSTANCE hInstance,
			NULL                           //pointer not needed
			);
    }	
    
    return true;
}

/* draw a static text control on the screen */
inline void drawStatic(DRAWITEMSTRUCT *dis)
{   
    char str[255];
    int len,x,y;
    SIZE size;  

    GetWindowText(dis->hwndItem,(LPSTR)str,(int)255);
    len=strlen(str);
    
    SelectObject(dis->hDC,hTabPen);                                                             // set border
    SelectObject(dis->hDC,hNormalFont);                                                         // set font
    SetTextColor(dis->hDC,0);                                                                   // set text colour to black
    SelectObject(dis->hDC,hTabBrush);                                                           // set background fill
    GetTextExtentPoint32(dis->hDC,str,len,&size);                                               // get size of string
    Rectangle(dis->hDC,dis->rcItem.left,dis->rcItem.top,dis->rcItem.right,dis->rcItem.bottom);  // display background rectangle
    TextOut(dis->hDC,dis->rcItem.left,dis->rcItem.top,(char*)str,len);                          // display text
}

/* draw a tab on the screen */
inline void drawTab(DRAWITEMSTRUCT *dis)
{   
    char *str;
    int len,x,y;
    SIZE size;  
    //GetWindowTextA(dis->hwndItem,(LPSTR)str,(int)255);

    str=(char*)dis->itemData;
    len=strlen(str);
   
    /* set font (based on whether the tab is selected) */
    if (dis->itemState==ODS_SELECTED)
        SelectObject(dis->hDC,hBoldFont);
    else
        SelectObject(dis->hDC,hNormalFont);

    SelectObject(dis->hDC,hTabPen);                                     /* set border style/colour */   
    SelectObject(dis->hDC,hTabBrush);                                   /* set background fill brush */    
    
    /* calculate position based on size of string in pixels */
    GetTextExtentPoint32(dis->hDC,str,len,&size); 
    x=dis->rcItem.left+(dis->rcItem.right-dis->rcItem.left)/2-size.cx/2;
    y=dis->rcItem.top+(dis->rcItem.bottom-dis->rcItem.top)/2-size.cy/2;

    /* display background rectangle & text on screen */
    Rectangle(dis->hDC,dis->rcItem.left,dis->rcItem.top,dis->rcItem.right,dis->rcItem.bottom);
    TextOut(dis->hDC,x,y,(char*)str,len);
}

/* calculate the size of a tab */
inline void measureTab(MEASUREITEMSTRUCT *mis)
{
    const int TEXT_MARGIN=5;
    SIZE size;
    HDC hdc=GetDC(hToolWindow);                                              /* get device context (drawing) object */
    SelectObject(hdc,hBoldFont);                                                 /* set font that will be used for drawing text */    
    GetTextExtentPoint32(hdc,(char*)mis->itemData,strlen((char*)mis->itemData),&size);     /* get width of string in pixels */   
    ReleaseDC(hToolWindow,hdc);                                              /* free device context handle */
    
    mis->itemWidth=size.cx+2*TEXT_MARGIN;                                    /* set width of tab */
    mis->itemHeight=size.cy+2*TEXT_MARGIN;                                   /* set height of tab */
}


/* setup up tool window's fonts & brushes for drawing */
inline void setupDrawingObjects(HWND hwnd)
{
    HDC hdc;
    
    hTabPen=CreatePen(PS_SOLID,1,GetSysColor(COLOR_3DFACE));          
    hdc=GetDC(hToolWindowTabControl);             
    hTabBrush=CreateSolidBrush(GetSysColor(COLOR_3DFACE));
    ReleaseDC(hToolWindowTabControl,hdc);            
}    

/* This function is called by the Windowsfunction DispatchMessage( ) */
LRESULT CALLBACK ToolWindowTabControlProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        /* WM_DRAWITEM: an ownerdraw control owned by this window needs to be drawn */
        case WM_DRAWITEM:
            if (((DRAWITEMSTRUCT*)lParam)->CtlType==ODT_STATIC)
                drawStatic((DRAWITEMSTRUCT*)lParam);
            break; 
                        
        default:
            break;
    }        
    return CallWindowProc(oldTabControlProc,hwnd,message,wParam,lParam);
}


/* This function is called by the Windowsfunction DispatchMessage( ) */
LRESULT CALLBACK ToolWindowDisplayTabContainerProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        /* WM_DRAWITEM: an ownerdraw control owned by this window needs to be drawn */
        case WM_DRAWITEM:
            if (((DRAWITEMSTRUCT*)lParam)->CtlType==ODT_STATIC)
                drawStatic((DRAWITEMSTRUCT*)lParam);
            break; 
        case WM_COMMAND:
            //if(hupdate==(HWND)lParam)
            {
            MessageBox( hwnd, (LPSTR) "Updating Image",
            (LPSTR) szMainWindowClassName,
            MB_ICONINFORMATION | MB_OK );
            }                
            break;
        
                     
        default:
            break;
    }        
    return CallWindowProc(oldDisplayTabContainerProc,hwnd,message,wParam,lParam);
}

/* This function is called by the Windowsfunction DispatchMessage( ) */
LRESULT CALLBACK ToolWindowQueryTabContainerProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        /* WM_DRAWITEM: an ownerdraw control owned by this window needs to be drawn */
        case WM_DRAWITEM:
            if (((DRAWITEMSTRUCT*)lParam)->CtlType==ODT_STATIC)
                drawStatic((DRAWITEMSTRUCT*)lParam);
            break; 
                        
        default:
            break;
    }        
    return CallWindowProc(oldQueryTabContainerProc,hwnd,message,wParam,lParam);
}

/* This function is called by the Windowsfunction DispatchMessage( ) */
LRESULT CALLBACK ToolWindowImageTabContainerProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        /* WM_DRAWITEM: an ownerdraw control owned by this window needs to be drawn */
        case WM_DRAWITEM:
            if (((DRAWITEMSTRUCT*)lParam)->CtlType==ODT_STATIC)
                drawStatic((DRAWITEMSTRUCT*)lParam);
            break; 
                        
        default:
            break;
    }        
    return CallWindowProc(oldImageTabContainerProc,hwnd,message,wParam,lParam);
}


/* This function is called by the Windowsfunction DispatchMessage( ) */
LRESULT CALLBACK ToolWindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static POINT snapMouseOffset;     /* mouse offset relative to window, used for snapping */
    NMHDR *nmhdr;                     /* structure used for WM_NOTIFY events */
    RECT rect;
            
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            setupDrawingObjects(hwnd);    /* setup fonts, brushes, etc */
            break;                
        
        /* WM_NOTIFY: notification of certain events related to controls */
        case WM_NOTIFY:
            nmhdr=(NMHDR*)lParam;
            /* nmhdr->code=the type of event that occured */
            switch(nmhdr->code)
            {
                /* TCN_SELCHANGE: the user has selected a tab within a tab control */
                case TCN_SELCHANGE:
                    
                    /* display the container that is associated with the selected tab
                       and hide the remaining tab containers */
                    switch(TabCtrl_GetCurSel(hToolWindowTabControl))
                    {
                        case DISPLAY_TAB_ID:
                            ShowWindow(hToolWindowDisplayTabContainer,SW_SHOW);
                            ShowWindow(hToolWindowQueryTabContainer,SW_HIDE);                            
                            ShowWindow(hToolWindowImageTabContainer,SW_HIDE);
                            break;
                        case QUERY_TAB_ID:
                            ShowWindow(hToolWindowDisplayTabContainer,SW_HIDE);
                            ShowWindow(hToolWindowQueryTabContainer,SW_SHOW);
                            ShowWindow(hToolWindowImageTabContainer,SW_HIDE);                                                        
                            break;
                        case IMAGE_TAB_ID:
                            ShowWindow(hToolWindowDisplayTabContainer,SW_HIDE);
                            ShowWindow(hToolWindowQueryTabContainer,SW_HIDE);
                            ShowWindow(hToolWindowImageTabContainer,SW_SHOW);
                            break;
                    }    
                    break;
            }    
            break;

        /* WM_NCLBUTTONDOWN: mouse button was pressed down in a non client area of the window */        
        case WM_NCLBUTTONDOWN:
            /* wParam=the area of the window where the mouse button was pressed */

            /* HTCAPTION: mouse button was pressed down on the window title bar
                         (occurs when user starts to move the window)              */            
            if(wParam == HTCAPTION)
            {
               /* get the mouse co-ords relative to the window */
                getMouseWindowOffset(hwnd,(int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam),&snapMouseOffset);
            }
            /* also let windows handle this message */
            return DefWindowProc(hwnd, message, wParam, lParam); 
            
        /* WM_MOVING: the window is about to be moved to a new location */
        case WM_MOVING:
            /* lParam=the new position, which can be modified before the window is moved */

            /* set new window position based on position of mouse */
            setNewWindowPosition((RECT*)lParam,&snapMouseOffset);

            /* if new position is near desktop edge, snap to it */
            snapWindowByMoving(hDesktop,(RECT*)lParam);  
            
            /* if new position is near main window, snap to it */    
            toolWindowIsSnapped=snapWindowByMoving(hMainWindow,(RECT*)lParam);
            break;
        
        /* WM_DRAWITEM: an ownerdraw control owned by this window needs to be drawn */
        case WM_DRAWITEM:
            if (((DRAWITEMSTRUCT*)lParam)->CtlType==ODT_TAB)
                drawTab((DRAWITEMSTRUCT*)lParam);
            else
                drawStatic((DRAWITEMSTRUCT*)lParam);
            break;           

        /* WM_MEASUREITEM: an ownerdraw control needs to be measured */        
        case WM_MEASUREITEM:
            if (((DRAWITEMSTRUCT*)lParam)->CtlType==ODT_TAB)
                measureTab((MEASUREITEMSTRUCT*)lParam);
            break;

        /* WM_SIZE: the window has been resized, minimized, or maximizsed, etc. */            
        case WM_SIZE:
            if (toolWindowIsSnapped)
            {
                // re-snap window in case the main window has now moved away
                GetWindowRect(hwnd,&rect);
                toolWindowIsSnapped=snapWindowByMoving(hMainWindow,&rect);
            }    
            break;
      
        /* WM_CLOSE: system or user has requested to close the window/application */              
        case WM_CLOSE:
            /* don't destory this window, but make it invisible */            
            ShowWindow(hwnd,SW_HIDE);
            return 0;

        /* WM_DESTORY: system is destroying our window */                
        case WM_DESTROY:
            return 0;

        default: 
            /* let windows handle any unknown messages */            
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    /* return 0 to indicate that we have processed the message */          
    return 0;
}


/* ------------------------------------------------------------------------------------------------------------------------ */
/* general window functions */

/* calculates window's new position based on the location of the mouse cursor */
void setNewWindowPosition(RECT* newPos,POINT *mouseOffset)
{
    POINT mouse;    /* mouse co-ords */
    
    /* get current mouse co-ords */
    GetCursorPos(&mouse);

    /* get width & height of window */
    newPos->bottom-=newPos->top;
    newPos->right-=newPos->left;
    
    /* set window position based on the cursor position */
    newPos->top=mouse.y-mouseOffset->y;
    newPos->left=mouse.x-mouseOffset->x;
    newPos->bottom+=newPos->top;
    newPos->right+=newPos->left;                
}

/* calculates window's new size based on location of the mouse cursor */
void setNewWindowSize(RECT* newPos,RECT* oldPos,POINT* oldMouse,int whichDirection)
{
    POINT newMouse;    /* mouse co-ords */
    SIZE size;
    
    /* get current mouse co-ords */
    GetCursorPos(&newMouse);

    /* calculate how much mouse has moved */
    int my=newMouse.y-oldMouse->y;
    int mx=newMouse.x-oldMouse->x;

    /* set window position based on the cursor position & which border is being dragged */
    switch (whichDirection)
    {
        case WMSZ_BOTTOM:
            newPos->bottom=oldPos->bottom+my;
            break;
        case WMSZ_TOP:
            newPos->top=oldPos->top+my;
            break;
        case WMSZ_LEFT:
            newPos->left=oldPos->left+mx;
            break;
        case WMSZ_RIGHT:
            newPos->right=oldPos->right+mx;
            break;
        case WMSZ_TOPLEFT:
            newPos->top=oldPos->top+my;
            newPos->left=oldPos->left+mx;
            break;
        case WMSZ_TOPRIGHT:
            newPos->top=oldPos->top+my;
            newPos->right=oldPos->right+mx;
            break;
        case WMSZ_BOTTOMLEFT:
            newPos->bottom=oldPos->bottom+my;
            newPos->left=oldPos->left+mx;
            break;
        case WMSZ_BOTTOMRIGHT:
            newPos->bottom=oldPos->bottom+my;
            newPos->right=oldPos->right+mx;
            break;                        

    }   
}    

/* move a window by a certain amount of pixels */
inline void moveWindowByOffset(HWND hwnd,RECT *rect,int leftOffset,int topOffset)
{
    MoveWindow(hwnd,rect->left+leftOffset,rect->top+topOffset,rect->right-rect->left,rect->bottom-rect->top,true);
}

/* move snapped windows with the main window */
inline void moveSnappedWindows(RECT *newRect,RECT *oldRect,RECT *prevImageWindowRect,RECT *prevToolWindowRect,int moveImageWindow,int moveToolWindow)
{
    int moveLeftOffset=newRect->left-oldRect->left;
    int moveTopOffset=newRect->top-oldRect->top;

    if (moveImageWindow)
        moveWindowByOffset(hImageWindow,prevImageWindowRect,moveLeftOffset,moveTopOffset);
    if (moveToolWindow)
        moveWindowByOffset(hToolWindow,prevToolWindowRect,moveLeftOffset,moveTopOffset);
       
}


/* returns whether window is in normal position (ie. not minimized or maximised) */
int isWindowInNormalState(HWND hwnd)
{
    WINDOWPLACEMENT wp;
    RECT rect;
    wp.length=sizeof(WINDOWPLACEMENT);

    GetWindowPlacement(hwnd,&wp);
    GetWindowRect(hwnd,&rect);
    if (wp.rcNormalPosition.top!=rect.top)
        return false;
    if (wp.rcNormalPosition.left!=rect.left)
        return false;
    if (wp.rcNormalPosition.right!=rect.right)
        return false;
    if (wp.rcNormalPosition.bottom!=rect.bottom)
        return false;
    if (wp.showCmd==SW_HIDE)
        return false;
    return true;
}    

/* snap a window to another window (by moving it) if it is range */
/*  snapToWin=handle of window to snap to, rect=cords of window to be moved */
/* returns: true if window has been snapped, false if not */
int snapWindowByMoving(HWND snapToWin,RECT *rect)
{   
    const int SNAP_PIXEL_RANGE=10;
    RECT snapToRect;
    SIZE winsize={rect->right-rect->left,rect->bottom-rect->top};
    int difference;
    int isSnapped=false;
    int winState;

    // get position of the window that we may snap to
    GetWindowRect(snapToWin,&snapToRect);
    
    //check if window is close enough to snap to
    if ( (rect->left+winsize.cx<=(snapToRect.left-SNAP_PIXEL_RANGE)) || (rect->left>=(snapToRect.right+SNAP_PIXEL_RANGE)) )
        return false;

    if ( (rect->top+winsize.cy<=(snapToRect.top-SNAP_PIXEL_RANGE)) || (rect->top>=(snapToRect.bottom+SNAP_PIXEL_RANGE)) )
        return false;

    //snap (top) to bottom
    difference=(rect->top-(snapToRect.bottom));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->top=snapToRect.bottom;
       rect->bottom=rect->top+winsize.cy;
       isSnapped=true;
    }
    
    // snap (bottom) to bottom
    difference=((rect->top+winsize.cy)-(snapToRect.bottom));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->top=snapToRect.bottom-winsize.cy;
       rect->bottom=rect->top+winsize.cy;
       isSnapped=true;
    }    
    
    // snap (left) to left
    difference=(rect->left-(snapToRect.left));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->left=snapToRect.left;
       rect->right=rect->left+winsize.cx;
       isSnapped=true;
    }
    
    // snap (right) to left
    difference=((rect->left+winsize.cx)-(snapToRect.left));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->left=snapToRect.left-winsize.cx;
       rect->right=rect->left+winsize.cx;       
       isSnapped=true;       
    }    
    
    // snap (right) to right
    difference=(rect->left+winsize.cx-(snapToRect.right));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->left=snapToRect.right-winsize.cx;
       rect->right=rect->left+winsize.cx;       
       isSnapped=true;       
    }    
    
    // snap (left) to right
    difference=(rect->left-(snapToRect.right));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->left=snapToRect.right;
       rect->right=rect->left+winsize.cx;       
       isSnapped=true;       
    }        

    // snap (top) to top
    difference=(rect->top-(snapToRect.top));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->top=snapToRect.top;
       rect->bottom=rect->top+winsize.cy;
       isSnapped=true;       
    }      

    // snap (bottom) to top
    difference=((rect->top+winsize.cy)-(snapToRect.top));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->top=snapToRect.top-winsize.cy;
       rect->bottom=rect->top+winsize.cy;
       isSnapped=true;
    }      
    
    return isSnapped;
}

/* snap a window to another window (by resizing it) if it is range */
/*  snapToWin=handle of window to snap to, rect=cords of window to be resized */
/* returns: true if window has been snapped, false if not */
int snapWindowBySizing(HWND snapToWin,RECT *rect,int whichDirection)
{
    const int SNAP_PIXEL_RANGE=10;
    RECT snapToRect;
    int difference;
    int isSnapped=false;

    // get position of the window that we may snap to
    GetWindowRect(snapToWin,&snapToRect);
    
    //check if window is close enough to snap to
    if ( (rect->right<(snapToRect.left-SNAP_PIXEL_RANGE)) || (rect->left>(snapToRect.right+SNAP_PIXEL_RANGE)) )
        return false;

    if ( (rect->bottom<(snapToRect.top-SNAP_PIXEL_RANGE)) || (rect->top>(snapToRect.bottom+SNAP_PIXEL_RANGE)) )
        return false;
        

    if ((whichDirection==WMSZ_TOP) || (whichDirection==WMSZ_TOPLEFT) || (whichDirection==WMSZ_TOPRIGHT))
    {
        //snap (top) to bottom
        difference=(rect->top-(snapToRect.bottom));
        if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
        {
           rect->top=snapToRect.bottom;
           isSnapped=true;
        }
        
        // snap (top) to top
        difference=(rect->top-(snapToRect.top));
        if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
        {
           rect->top=snapToRect.top;
           isSnapped=true;       
        }           
    }
    
    if ((whichDirection==WMSZ_BOTTOM) || (whichDirection==WMSZ_BOTTOMLEFT) || (whichDirection==WMSZ_BOTTOMRIGHT))
    {    
        // snap (bottom) to bottom
        difference=((rect->bottom)-(snapToRect.bottom));
        if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
        {
           rect->bottom=snapToRect.bottom;
           isSnapped=true;
        }    

        // snap (bottom) to top
        difference=((rect->bottom)-(snapToRect.top));
        if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
        {
           rect->bottom=snapToRect.top;
           isSnapped=true;
        }          
    }
    
    if ((whichDirection==WMSZ_LEFT) || (whichDirection==WMSZ_BOTTOMLEFT) || (whichDirection==WMSZ_TOPLEFT))
    {    
        // snap (left) to left
        difference=(rect->left-(snapToRect.left));
        if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
        {
           rect->left=snapToRect.left;
           isSnapped=true;
        }

        // snap (left) to right
        difference=(rect->left-(snapToRect.right));
        if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
        {
           rect->left=snapToRect.right;
           isSnapped=true;       
        }           
    }
    

    if ((whichDirection==WMSZ_RIGHT) || (whichDirection==WMSZ_BOTTOMRIGHT) || (whichDirection==WMSZ_TOPRIGHT))
    { 
    
        // snap (right) to left
        difference=((rect->right)-(snapToRect.left));
        if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
        {
           rect->right=snapToRect.left;       
           isSnapped=true;       
        }    
    
        // snap (right) to right
        difference=(rect->right-(snapToRect.right));
        if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
        {
           rect->right=snapToRect.right;       
           isSnapped=true;       
        }    
    }
    
    return isSnapped;
}


//calculate mouse co-ords relative to position of window
inline void getMouseWindowOffset(HWND hwnd,int mx,int my,POINT *mouseOffset)
{
    RECT win;
    GetWindowRect(hwnd,&win);
    mouseOffset->x=mx-win.left;
    mouseOffset->y=my-win.top;
}

void orderTheWindows()
{
    SetWindowPos(hMainWindow,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE+SWP_NOSIZE);
    SetWindowPos(hToolWindow,hMainWindow,0,0,0,0,SWP_NOMOVE+SWP_NOSIZE); 
    SetWindowPos(hImageWindow,hToolWindow,0,0,0,0,SWP_NOMOVE+SWP_NOSIZE);    
}    


/* main functions */
/* ------------------------------------------------------------------------------------------------------------------------ */
void loadFile()
{
    OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW - which note?
    ofn.hwndOwner = hMainWindow;
    ofn.lpstrFilter =  "All Supported Images\0*.ecw;*.jpg;*.tif\0ERMapper Compressed Wavelets (*.ecw)\0*.ecw\0JPG Files (*.jpg)\0*.jpg\0TIFF / GeoTIFF (*.tif)\0*.tif\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "txt";

    if(GetOpenFileName(&ofn))
    {
        // Do something usefull with the filename stored in szFileName 

        			// create image window and display
    				if (!hImageWindow)
            			setupImageWindow();
    				ShowWindow(hImageWindow,SW_SHOW);    
        
        // enable Window menu items
        EnableMenuItem(hMainMenu,IDM_IMAGEWINDOW,false);
        EnableMenuItem(hMainMenu,IDM_TOOLSWINDOW,false);        

		// Clean up any previous instance
		if (image_handler) delete image_handler;
	    image_handler = new ImageHandler::ImageHandler(hMainWindowDisplay, hImageWindowDisplay, ofn.lpstrFile);
	    if (image_handler) {
			if (image_handler->status > 0) {
				// An error occurred instantiaing the image handler class.
				MessageBox (NULL, image_handler->error_text , "[error] Parbat3D :: ImageHandler", 0);
			} else {
				if (image_handler->status < 0) { // Error occurred, but was non-fatal
					MessageBox (NULL, image_handler->error_text , "[warning] Parbat3D :: ImageHandler", 0);
				}
    			else
    			{
       
        			SetWindowText(hImageWindow,image_handler->get_info_string());
        			
                    // Image loaded succesfully, so update opengl displays
                    RedrawWindow(hMainWindowDisplay,NULL,NULL,RDW_INTERNALPAINT);
                    RedrawWindow(hImageWindowDisplay,NULL,NULL,RDW_INTERNALPAINT);                
                }				
			}

		} else { // Object wasn't created - this is probably terminal
			MessageBox (NULL, "[error] Could not instantiate ImageHandler." , "Parbat3D :: ImageHandler", 0);
			// !! Should probably die gracefully at this point - Rowan
		}
    }
    // create tool window and display
        
        if (hToolWindow)
        {
            DestroyWindow(hToolWindow);
            hToolWindow=NULL;
        }    
        setupToolWindow();
        ShowWindow(hToolWindow,SW_SHOW);
        
        orderTheWindows();
}






