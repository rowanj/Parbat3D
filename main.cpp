#include <Windows.h>
#include <Commctrl.h>
#include <iostream>
#include "config.h"
#include "Settings.h"
#include "ImageHandler.h"
#include "ImageProperties.h"
#include "BandInfo.h"
#include "StringUtils.h"
#include "main.h"
#include "SnappingWindow.h"


using namespace std;

ImageHandler::ImageHandler* image_handler = NULL;	// Instance handle ptr

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
HWND hToolWindowScrollBar;
HWND hImageWindowDisplay;
HWND hMainWindowDisplay;
HWND *redRadiobuttons;                // band radio buttons
HWND *greenRadiobuttons;
HWND *blueRadiobuttons;
HWND *imageBandValues;                // values displayed under query tab
HWND hupdate;
int bands = 0;                        // for use with radio buttons

/* Variables to record when the windows have snapped to main wndow */
int imageWindowIsSnapped=false;
int toolWindowIsSnapped=false;

/* Define id numbers for the tab's in the tool window */
enum {DISPLAY_TAB_ID,QUERY_TAB_ID,IMAGE_TAB_ID};

/* Used for loading and saving window position and sizes */
settings winPos ("settings.ini");

/* objects used for painting/drawing */
HFONT hBoldFont,hNormalFont,hHeadingFont;
HPEN hTabPen;
HBRUSH hTabBrush;
WNDPROC oldTabControlProc,oldDisplayTabContainerProc,oldQueryTabContainerProc,oldImageTabContainerProc;

int fileIsOpen=false;   /* indicates if an image file is currently loaded */

/* program entry point */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)
{
 	MSG messages;     /* Here messages to the application is saved */
  
    /* load window classes for common controls */
    InitCommonControls();
    
    /* store this process's instance handle */
    hThisInstance=hInstance;
    
    /* register window classes */
    if ((!registerToolWindow())||(!registerImageWindow()))
        return 0;
    
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
    if(!RegisterClassEx(&wincl)) return false;
    
    /* Create window class for the display control windows */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szDisplayClassName;
    wincl.lpfnWndProc = DisplayWindowProcedure;  /* This function is called by windows */
    wincl.style = CS_DBLCLKS;  /* Ctach double-clicks */
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
    
    /* Create main window */
    hMainWindow = CreateWindowEx(0, szMainWindowClassName, "Parbat3D",
		WS_OVERLAPPED+WS_CAPTION+WS_SYSMENU+WS_MINIMIZEBOX, mx, my, 250, 296,
		hMainWindow, NULL, hThisInstance, NULL);
    if (!hMainWindow)
        return false;                        

    /* set menu to main windo */
	hMainMenu = LoadMenu(hThisInstance, MAKEINTRESOURCE(ID_MENU));
    SetMenu(hMainWindow, hMainMenu);    
    
    /* Disable Window menu items. note: true appears to disable & false enables */
    EnableMenuItem(hMainMenu,IDM_IMAGEWINDOW,true);
    EnableMenuItem(hMainMenu,IDM_TOOLSWINDOW,true);
    EnableMenuItem(hMainMenu,IDM_FILECLOSE,true);    

    /* get client area of image window */
    GetClientRect(hMainWindow,&rect);

    /* setup font objects */
    HDC hdc=GetDC(hMainWindow);      
    hNormalFont=CreateFont(-MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72),0,0,0,400,false,false,false,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,"Tahoma"); //"MS Sans Serif" //Tahoma    
    hBoldFont=CreateFont(-MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72),0,0,0,600,false,false,false,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,"Tahoma"); //"MS Sans Serif" //Tahoma
    hHeadingFont=CreateFont(-MulDiv(9, GetDeviceCaps(hdc, LOGPIXELSY), 72),0,0,0,600,false,false,false,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,"Tahoma"); //"MS Sans Serif" //Tahoma
    ReleaseDC(hMainWindow,hdc);    

    /* create a child window that will be used by OpenGL */
    hMainWindowDisplay=CreateWindowEx( 0, szDisplayClassName, NULL, WS_CHILD|WS_VISIBLE,
		rect.left, rect.top, rect.right, rect.bottom, hMainWindow, NULL, hThisInstance, NULL);
   
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
            switch( wParam )
            {
                case IDM_FILEOPEN:
                    loadFile();
                    return 0;
                  
                case IDM_FILESAVE:
                case IDM_FILESAVEAS:
					MessageBox( hwnd, (LPSTR) "This will be greyed out if no file open, else will say \"Where do you want to store such a big file?\"",
                              (LPSTR) szMainWindowClassName,
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
                     ShellExecute(NULL, "open", "index.htm", NULL, "help", SW_SHOW);
					
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
           break;

        /* WM_NCLBUTTONDOWN: mouse button was pressed down in a non client area of the window */
        case WM_NCLBUTTONDOWN:

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

            /* set new window position based on position of mouse */
            setNewWindowPosition((RECT*)lParam,&snapMouseOffset);            
            
            /* snap main window to edge of desktop */           
            snapWindowByMoving(hDesktop,(RECT*)lParam);

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
                if (MessageBox(hMainWindow,"Are you sure you wish quit?\nAn image is currently open.","Parbat3D",MB_YESNO|MB_ICONQUESTION)!=IDYES)
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
    const int textLen=15;
    
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            /* init variables used for painting "No Image Loaded" message */
            hbrush=CreateSolidBrush(0);
            hdc=GetDC(hwnd);                                                                      /* get device context (drawing) object */
            SelectObject(hdc,hNormalFont);                                                   /* set font that will be used for drawing text */    
            GetTextExtentPoint32(hdc,text,textLen,&textSize); /* get width & height of string in pixels */   
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

/* register image window class */
int inline registerImageWindow()
{
    WNDCLASSEX wincl;  /* Datastructure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;                /* process's instance handle */
    wincl.lpszClassName = szImageWindowClassName;   /* our unique name for this class */
    wincl.lpfnWndProc = ImageWindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS; /* Ctach double-clicks */
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

/* create the image window */
int setupImageWindow()
{
    
    /* Get Main Window coords for Image Window alignment*/
    RECT rect;
    GetWindowRect(hMainWindow, &rect);
    
    /* Create a window based on the class we just registered */
    hImageWindow =CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, szImageWindowClassName, "Image Window",
	     WS_POPUP+WS_SYSMENU+WS_CAPTION+WS_MINIMIZEBOX+WS_MAXIMIZEBOX+WS_VSCROLL+WS_HSCROLL+WS_SIZEBOX,
	     rect.right, rect.top, 700, 600, 0, NULL, hThisInstance, NULL);

    /* get client area of image window */
    GetClientRect(hImageWindow,&rect);

    /* create a child window that will be used by OpenGL */
    hImageWindowDisplay=CreateWindowEx( 0, szDisplayClassName, NULL, WS_CHILD+WS_VISIBLE,
		rect.left, rect.top, rect.right, rect.bottom, hImageWindow, NULL, hThisInstance, NULL);

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
                /* re-snap window in case the main window has now moved away */
                GetWindowRect(hwnd,&rect);
                imageWindowIsSnapped=snapWindowByMoving(hwnd,&rect);
            }    
           
            /* resize display/opengl window to fit new size */            
            GetClientRect(hImageWindow,&rect);
            MoveWindow(hImageWindowDisplay,rect.left,rect.top,rect.right,rect.bottom,true);
            return 0;
           
        case WM_SHOWWINDOW:
            /* update window menu item depending on whether window is shown or hidden */
            if (wParam)
                CheckMenuItem(hMainMenu,IDM_IMAGEWINDOW,MF_CHECKED|MF_BYCOMMAND);            
            else
                CheckMenuItem(hMainMenu,IDM_IMAGEWINDOW,MF_UNCHECKED|MF_BYCOMMAND);
            return 0;

        /* WM_CLOSE: system or user has requested to close the window/application */             
        case WM_CLOSE:
            /* don't destroy this window, but make it invisible */
            ShowWindow(hwnd,SW_HIDE);
            return 0;

        /* WM_DESTORY: system is destroying our window */
        case WM_DESTROY:
            CheckMenuItem(hMainMenu,IDM_IMAGEWINDOW,MF_UNCHECKED|MF_BYCOMMAND);            
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

int inline registerToolWindow()
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
    TCITEM tie;  /* datastructure for tabs */
    RECT rect;

    const int SCROLLBAR_WIDTH=13;
    const int SCROLLBAR_TOP=25;

    
    /* Get Main Window Location for image window alignment*/
    GetWindowRect(hMainWindow,&rect);
    
    /* The class is registered, lets create the program*/
    hToolWindow =CreateWindowEx(WS_EX_TOPMOST, szToolWindowClassName, "Tools",
           WS_POPUP+WS_CAPTION+WS_SYSMENU+WS_MINIMIZEBOX, rect.left, rect.bottom,
           250, 300, NULL, NULL, hThisInstance, NULL);

    if (hToolWindow==NULL)
        return false;
    
    /* indicate tool window as snapped to the main window */
    toolWindowIsSnapped=true;

    /* get width & height of tool window's client area (ie. inside window's border) */
    GetClientRect(hToolWindow,&rect);

    /* create tab control */
    hToolWindowTabControl =CreateWindowEx(0, WC_TABCONTROL, "Tools",
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | TCS_OWNERDRAWFIXED, 0, 0,
		rect.right, rect.bottom, hToolWindow, NULL, hThisInstance, NULL);

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
    /* Display tab container */
	hToolWindowDisplayTabContainer =CreateWindowEx( 0, szStaticControl, "Channel Selection",
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_OWNERDRAW, SPACING_FOR_BOARDER,
		SPACING_FOR_TAB_HEIGHT, rect.right-SCROLLBAR_WIDTH, rect.bottom, hToolWindowTabControl, NULL,
		hThisInstance, NULL); 
           
	/* Query tab container */
    hToolWindowQueryTabContainer =CreateWindowEx(0, szStaticControl, "Band Values",
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_OWNERDRAW, SPACING_FOR_BOARDER,             /* left position relative to tab control */
           SPACING_FOR_TAB_HEIGHT, rect.right-SCROLLBAR_WIDTH, rect.bottom, hToolWindowTabControl,           /* The window is a childwindow of the tab control */
           NULL, hThisInstance, NULL);
           
    /* Image tab container */
    hToolWindowImageTabContainer =CreateWindowEx(0, szStaticControl, "Image Properties",              /* text to display */
           WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_OWNERDRAW, SPACING_FOR_BOARDER,             /* left position relative to tab control */
           SPACING_FOR_TAB_HEIGHT, rect.right-SCROLLBAR_WIDTH, rect.bottom, hToolWindowTabControl, NULL,                            /* No menu */
           hThisInstance, NULL); 

    /* create scroll bar */
    GetClientRect(hToolWindow,&rect);
    hToolWindowScrollBar=CreateWindowEx(0, "SCROLLBAR", NULL,
		WS_CHILD | WS_VISIBLE | SBS_RIGHTALIGN | SBS_VERT, 0, SCROLLBAR_TOP,
		rect.right, rect.bottom-SCROLLBAR_TOP, hToolWindow, NULL, hThisInstance, NULL);           
	EnableWindow(hToolWindowScrollBar,false);    
    
    /* modify tab containers' window procedure address 
        note: assumming proc addr is the same for all three */
    oldDisplayTabContainerProc=(WNDPROC)SetWindowLong(hToolWindowDisplayTabContainer,GWL_WNDPROC,(long)&ToolWindowDisplayTabContainerProcedure);
    oldQueryTabContainerProc=(WNDPROC)SetWindowLong(hToolWindowQueryTabContainer,GWL_WNDPROC,(long)&ToolWindowQueryTabContainerProcedure);
    oldImageTabContainerProc=(WNDPROC)SetWindowLong(hToolWindowImageTabContainer,GWL_WNDPROC,(long)&ToolWindowImageTabContainerProcedure);

	/* Assign number of image bands to global variable */
    bands = image_handler->get_image_properties()->getNumBands() + 1;
    
    /* Create group for R, G & B radio buttons based on band number */
    hRed = CreateWindowEx(0, "BUTTON", "R", WS_CHILD | BS_GROUPBOX | WS_VISIBLE, 138, 25,
		26, 20 + (20 * bands), hToolWindowDisplayTabContainer, NULL, hThisInstance, NULL);
	hGreen = CreateWindowEx(0, "BUTTON", "G", WS_CHILD | BS_GROUPBOX | WS_VISIBLE, 164, 25,
		26, 20 + (20 * bands), hToolWindowDisplayTabContainer, NULL, hThisInstance, NULL);
    hBlue = CreateWindowEx(0, "BUTTON", "B", WS_CHILD | BS_GROUPBOX | WS_VISIBLE, 190, 25,
		26, 20 + (20 * bands), hToolWindowDisplayTabContainer, NULL, hThisInstance, NULL);
	
	/* Create container for band values */
	HWND queryValueContainer = CreateWindowEx(0, "BUTTON", "Values",
		WS_CHILD | BS_GROUPBOX | WS_VISIBLE, 138, 45, 66, 20 + (20 * (bands-1)),
		hToolWindowQueryTabContainer, NULL, hThisInstance, NULL);

	/* Dynamically add Radio buttons */
	redRadiobuttons=new HWND[bands];
	greenRadiobuttons=new HWND[bands];
	blueRadiobuttons=new HWND[bands];
	
	/* Dynamically add image band values */
	imageBandValues = new HWND[bands];
	
    for (int i=0; i<bands; i++)
    {
		redRadiobuttons[i] = CreateWindowEx(0, "BUTTON", NULL,
			WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 5, 15 + (20 * i), 18, 18,
			hRed, NULL, hThisInstance, NULL);
			
		greenRadiobuttons[i] = CreateWindowEx(0, "BUTTON", NULL,
			WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 5, 15 + (20 * i), 18, 18,
			hGreen, NULL, hThisInstance, NULL);
			
		blueRadiobuttons[i] = CreateWindowEx(0, "BUTTON", NULL,
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 5, 15 + (20 * i), 18, 18, hBlue, NULL,
			hThisInstance, NULL);
		
		const char* name;
		if (i>0) {
    		/* add band names to radio buttons*/
    		name = image_handler->get_band_info(i)->getColourInterpretationName();
    		
    		/* If Colour name unknown change band name*/
    		const char *altName ="No colour name";
    		if (strcmp(name, "Unknown")==0)
              name = altName;
                
            /* Add band number to band name */
            name = catcstrings( (char*) " - ", (char*) name);
            name = catcstrings( (char*) inttocstring(i), (char*) name);
            name = catcstrings( (char*) "Band ", (char*) name);
        } else
               name = "NONE";

        /* Display band name in tool window */
		CreateWindowEx(0, szStaticControl, name,
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 20, 40 + (20 * i), 100, 18,
			hToolWindowDisplayTabContainer, NULL, hThisInstance, NULL);
           
		/* Insert 'Update' button under radio buttons. Location based on band number */
		hupdate =  CreateWindowEx(0, "BUTTON", "Update", WS_CHILD | WS_VISIBLE, 136,
			50 + (20 * bands), 80, 25, hToolWindowDisplayTabContainer, NULL, hThisInstance, NULL);     

        if (i>0) {
            /* add channel names under the query tab */
            CreateWindowEx(0, szStaticControl, name,
    			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 20, 40 + (20 * i), 100, 18,
    			hToolWindowQueryTabContainer, NULL, hThisInstance, NULL);
    
    		/* add the band values to the value container under the query tab */
            char tempBandValue[4] = "128"; // temporary storage for the band value
            imageBandValues[i] = CreateWindowEx(0, szStaticControl, tempBandValue, WS_CHILD | WS_VISIBLE, 5, 15 + (20 * (i-1)),
    			50, 18, queryValueContainer, NULL, hThisInstance, NULL);
        } 			
	}
	
	/* Default radio button selection */
	SendMessage(redRadiobuttons[1],BM_SETCHECK,BST_CHECKED,0);
	SendMessage(greenRadiobuttons[2],BM_SETCHECK,BST_CHECKED,0);
	SendMessage(blueRadiobuttons[3],BM_SETCHECK,BST_CHECKED,0);
	
	/* add the image property information under the image tab */
	ImageProperties* ip=image_handler->get_image_properties();
	string leader;
	int ipItems=5;
	string n[ipItems];
	string v[ipItems];
	
	/* If the filename is too long to be displayed, truncate it.
	Later on, a roll-over tooltip should be implemented to bring
	up the full name.*/
	string fullname = ip->getFileName();
	string fname, bname, finalname;
	if (fullname.length() > 25) {
		fname = fullname.substr(0, 12);
		bname = fullname.substr(fullname.length()-12, fullname.length()-1);
		finalname = fname + "…" + bname;
	} else
	    finalname = fullname;
	    
	n[0]="File Name"; v[0]=makeMessage(leader, (char*) finalname.c_str());
	n[1]="File Type"; v[1]=makeMessage(leader, (char*) ip->getDriverLongName());
	n[2]="Width"; v[2]=makeMessage(leader, ip->getWidth());
	n[3]="Height"; v[3]=makeMessage(leader, ip->getHeight());
	n[4]="Bands"; v[4]=makeMessage(leader, ip->getNumBands());
	
	for (int i=0; i<ipItems; i++) {
		CreateWindowEx(0, szStaticControl, (char*) n[i].c_str(),
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 20, 40+(i*20), 50, 18,
			hToolWindowImageTabContainer, NULL, hThisInstance, NULL);
		
		CreateWindowEx(0, szStaticControl, (char*) v[i].c_str(),
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 78, 40+(i*20), 160, 18,
			hToolWindowImageTabContainer, NULL, hThisInstance, NULL);
    }
    
    return true;
}

/* draw a static text control on the screen */
inline void drawStatic(DRAWITEMSTRUCT *dis, HFONT hfont)
{   
    char str[255];
    int len,x,y;
    SIZE size;  

    GetWindowText(dis->hwndItem,(LPSTR)str,(int)255);
    len=strlen(str);
    
    SelectObject(dis->hDC,hTabPen);                                                             // set border
    SelectObject(dis->hDC,hfont);                                                         // set font
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
                drawStatic((DRAWITEMSTRUCT*)lParam,hHeadingFont);
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
                drawStatic((DRAWITEMSTRUCT*)lParam,hNormalFont);
            break; 
        case WM_COMMAND:
            //if(hupdate==(HWND)lParam)
            {
            MessageBox( hwnd, (LPSTR) "Updating Image",
                        (LPSTR) szMainWindowClassName,
            			MB_ICONINFORMATION | MB_OK );
            // !! Insert band numbers (bands start at 1, not 0) here. - Rowan
            if (image_handler) image_handler->set_bands(3,1,2);
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
                drawStatic((DRAWITEMSTRUCT*)lParam,hNormalFont);
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
                drawStatic((DRAWITEMSTRUCT*)lParam,hNormalFont);
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
                drawStatic((DRAWITEMSTRUCT*)lParam,hNormalFont);
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

        case WM_SHOWWINDOW:
            /* update window menu item depending on whether window is shown or hidden */
            if (wParam)
                CheckMenuItem(hMainMenu,IDM_TOOLSWINDOW,MF_CHECKED|MF_BYCOMMAND);            
            else
                CheckMenuItem(hMainMenu,IDM_TOOLSWINDOW,MF_UNCHECKED|MF_BYCOMMAND);
            return 0;
      
        /* WM_CLOSE: system or user has requested to close the window/application */              
        case WM_CLOSE:
            /* don't destory this window, but make it invisible */            
            ShowWindow(hwnd,SW_HIDE);
            return 0;

        /* WM_DESTORY: system is destroying our window */                
        case WM_DESTROY:
            CheckMenuItem(hMainMenu,IDM_TOOLSWINDOW,MF_UNCHECKED|MF_BYCOMMAND);            
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

/* toggles a menu item between a checked & unchecked state */
/* returns true if new state is checked, else false if unchecked */
int toggleMenuItemTick(HMENU hMenu,int itemId)
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



void orderWindows()
{
    SetWindowPos(hMainWindow,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE+SWP_NOSIZE);
    SetWindowPos(hToolWindow,hMainWindow,0,0,0,0,SWP_NOMOVE+SWP_NOSIZE); 
    //SetWindowPos(hImageWindow,hToolWindow,0,0,0,0,SWP_NOMOVE+SWP_NOSIZE);    
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
    ofn.lpstrFilter =  "All Supported Images\0*.ecw;*.jpg;*.tif\0ERMapper Compressed Wavelets (*.ecw)\0*.ecw\0JPEG / JPEG 2000 (*.jpg)\0*.jpg\0TIFF / GeoTIFF (*.tif)\0*.tif\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "txt";

    if(GetOpenFileName(&ofn))
    {
        // Do something usefull with the filename stored in szFileName 

		// Clean up any previous instance
		closeFile();

		// create image window 
    	if (!hImageWindow)
    		setupImageWindow();        
	
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
                    // Image loaded succesfully
                    fileIsOpen=true;
                    
                    /* Display the file name at the top of the image window */
                    string leader = "Image - ";
					SetWindowText(hImageWindow,
								(char *) makeMessage(leader,
								(char *) image_handler->get_image_properties()->getFileName()));

                    // re-create tool window
                    setupToolWindow();

                    // show tool & image windows
                    ShowWindow(hToolWindow,SW_SHOW);
                    ShowWindow(hImageWindow,SW_SHOW);    
                    orderWindows();                    

                    // update opengl displays
                    //InvalidateRect(hMainWindowDisplay,0,true);
                    //UpdateWindow(hMainWindowDisplay);
                    //InvalidateRect(hImageWindowDisplay,0,true);
                    //UpdateWindow(hImageWindowDisplay);
                    RedrawWindow(hMainWindowDisplay,NULL,NULL,RDW_INTERNALPAINT);
                    RedrawWindow(hImageWindowDisplay,NULL,NULL,RDW_INTERNALPAINT);                

                    // enable window menu items
                    EnableMenuItem(hMainMenu,IDM_IMAGEWINDOW,false);
                    EnableMenuItem(hMainMenu,IDM_TOOLSWINDOW,false);
                    EnableMenuItem(hMainMenu,IDM_FILECLOSE,false);
                            
                }				
			}

		} else { // Object wasn't created - this is probably terminal
			MessageBox (NULL, "[error] Could not instantiate ImageHandler." , "Parbat3D :: ImageHandler", 0);
			// !! Should probably die gracefully at this point - Rowan
		}
    }
}

void closeFile()
{
    
	if (image_handler) delete image_handler;
    image_handler=NULL;
	
    if (hToolWindow)
    {
        DestroyWindow(hToolWindow);
        hToolWindow=NULL;
    }    
    
    if (hImageWindow)
    {
        DestroyWindow(hImageWindow);
        hImageWindow=NULL;
    }

    /* disable menu items */
    EnableMenuItem(hMainMenu,IDM_IMAGEWINDOW,true);
    EnableMenuItem(hMainMenu,IDM_TOOLSWINDOW,true);
    EnableMenuItem(hMainMenu,IDM_FILECLOSE,true);    
    
    InvalidateRect(hMainWindowDisplay,0,true);  /* repaint main window */		
    UpdateWindow(hMainWindowDisplay);
}
