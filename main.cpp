#include <Windows.h>
#include <Commctrl.h>
#include <iostream>
#include "main.h"
#include "config.h"
#include "Settings.h"

#if TMP_USE_IMAGE_MANIPULATION
#include "ImageHandler.h"
ImageHandler::ImageHandler* image_handler;	// Instance handle
#endif

using namespace std;

/* Unique class names for our main windows */
char szMainWindowClassName[] = "Parbat3D Main Window";
char szImageWindowClassName[] = "Parbat3D Image Window";
char szToolWindowClassName[] = "Parbat3D Tool Window";
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

/* Variables to record when the windows have snapped to main wndow */
int imageWindowIsSnapped=false;
int toolWindowIsSnapped=false;

/* Define id numbers for the tab's in the tool window */
enum {DISPLAY_TAB_ID,QUERY_TAB_ID};

/* Used for loading and saving window position and sizes */
settings winPos ("settings.ini");

/* program entry point */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)

{
 	MSG messages;            /* Here messages to the application is saved */
 
    /* load window classes for common controls */
    InitCommonControls();
    
    // store this process's instance handle
    hThisInstance=hInstance;
    
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
           300,                 /* and height in pixels */
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

    /* create a child window that will be used by OpenGL */
    hMainWindowDisplay=CreateWindowEx(
           0,                   /* Extended possibilites for variation */
           szStaticControl,         /* Classname */
           NULL,         /* Title Text */
           WS_CHILD+WS_VISIBLE, /* styles */
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
                imageWindowIsMovingToo=imageWindowIsSnapped;
                toolWindowIsMovingToo=toolWindowIsSnapped;                  
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
            WINDOWPLACEMENT wp;
            /* wParam=how the window size has changed */
            switch(wParam)
            {
                case SIZE_MINIMIZED:
                   /* hide other windows when main window is minized */
                   /*  (now done automatically)
                   if ((hImageWindow)&&(hToolWindow))
                   {
                       GetWindowPlacement(hImageWindow,&wp);
                       wp.showCmd=SW_MINIMIZE;
                       SetWindowPlacement(hImageWindow,&wp);
                       
                       GetWindowPlacement(hToolWindow,&wp);
                       wp.showCmd=SW_MINIMIZE;
                       SetWindowPlacement(hToolWindow,&wp);
                   }*/
                    break;
                case SIZE_RESTORED:
                   /* restore other windows when main window is restored */
                   /* (now done automatically)
                   if ((hImageWindow)&&(hToolWindow))
                   {
                   
                       GetWindowPlacement(hImageWindow,&wp);
                       if ((wp.showCmd==SW_MINIMIZE)||(wp.showCmd==SW_SHOWMINIMIZED))
                       {
                          wp.showCmd=SW_RESTORE;
                          SetWindowPlacement(hImageWindow,&wp);                                                                                     
                       }
                       GetWindowPlacement(hToolWindow,&wp);
                       if ((wp.showCmd==SW_MINIMIZE)||(wp.showCmd==SW_SHOWMINIMIZED))
                       {
                          wp.showCmd=SW_RESTORE;
                          SetWindowPlacement(hToolWindow,&wp);                                                                                                                                                                          
                       }
                   
                   }
                   */
                    break;
            }
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
            // !! Shutdown image file and OpenGL
            PostQuitMessage (0);
            break;
        
        default:                   /* for messages that we don't deal with */
            /* let windows peform the default operation based on the message */
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    /* return 0 to indicate that we have processed the message */    
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
           0,                   /* Extended possibilites for variation */
           szImageWindowClassName,         /* Classname */
           "Image Window",         /* Title Text */
           WS_OVERLAPPEDWINDOW+WS_VSCROLL+WS_HSCROLL, /* defaultwindow */
           rect.right,       /* x position based on main window */
           rect.top,         /* y position based on main window */
           700,                 /* The programs width */
           600,                 /* and height in pixels */
           hMainWindow,        /* The window is a childwindow to main window */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* get client area of image window */
    GetClientRect(hImageWindow,&rect);

    /* create a child window that will be used by OpenGL */
    hImageWindowDisplay=CreateWindowEx(
           0,                   /* Extended possibilites for variation */
           szStaticControl,         /* Classname */
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

/* create tool window */
int setupToolWindow()
{
    WNDCLASSEX wincl;        /* Datastructure for the windowclass */
    TCITEM tie;             /* datastructure for tabs */
    RECT rect;
    
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
    
    /* Get Main Window Location for image window alignment*/
    GetWindowRect(hMainWindow,&rect);
    
    /* The class is registered, lets create the program*/
    hToolWindow =CreateWindowEx(
           // WS_EX_TOOLWINDOW? -shane
           0,                              /* Extended styles */
           szToolWindowClassName,         /* Classname */
           "Tools",                         /* Title Text */
           WS_OVERLAPPED+WS_CAPTION+WS_SYSMENU+WS_MINIMIZEBOX+WS_VSCROLL, /* defaultwindow */
           rect.left,       /* Windows decides the position */
           rect.bottom,       /* where the window end up on the screen */
           250,                 /* The programs width */
           300,                 /* and height in pixels */
           hMainWindow,        /* The window is a childwindow to main window */
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
           WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
           0,                       /* Top co-ord relative to tool window */
           0,                       /* Left co-ord relative to tool window */
           rect.right,              /* Set width to tool window's client area width */
           rect.bottom,             /* Set height to tool window's client area height */
           hToolWindow,             /* The window is a childwindow to tool window */
           NULL,                    /* No menu */
           hThisInstance,           /* Process's Instance handle */
           NULL                     /* No Window Creation data */
           );

    /* add tabs to tab-control */
    tie.mask=TCIF_TEXT;
    tie.pszText="Display";
    TabCtrl_InsertItem(hToolWindowTabControl, DISPLAY_TAB_ID, &tie);
    tie.mask=TCIF_TEXT;
    tie.pszText="Query";
    TabCtrl_InsertItem(hToolWindowTabControl, QUERY_TAB_ID, &tie);

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
           WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, /* window styles */
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
           0,                                /* Extended styles */
           szStaticControl,                  /* pre-defined classname for static text control */
           "Query tab container",            /* text to display */
           WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, /* styles */
           SPACING_FOR_BOARDER,             /* left position relative to tab control */
           SPACING_FOR_TAB_HEIGHT,          /* top position relative to tab control */
           rect.right,                      /* the width of the container */
           rect.bottom,                     /* the height of the container */
           hToolWindowTabControl,           /* The window is a childwindow of the tab control */
           NULL,                            /* No menu */
           hThisInstance,                   /* Program Instance handler */
           NULL                             /* No Window Creation data */
           ); 
    
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
			
			/* temporary label */ 
			char name[100] = "Channel";
			
		CreateWindowEx(
           0,                   /* Extended possibilites for variation */
           szStaticControl,     /* Classname */
           name,        		/* Title Text */
           WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, /* defaultwindow */
           20,       			/* Windows decides the position */
           40 + (20 * i),       /* where the window end up on the screen */
           100,                 /* The programs width */
           18,                  /* and height in pixels */
           hToolWindowDisplayTabContainer,        /* The window is a childwindow to main window */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );     
		
	}
    
    return true;
}

/* This function is called by the Windowsfunction DispatchMessage( ) */
LRESULT CALLBACK ToolWindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static POINT snapMouseOffset;     /* mouse offset relative to window, used for snapping */
    NMHDR *nmhdr;                     /* structure used for WM_NOTIFY events */
            
    switch (message)                  /* handle the messages */
    {    
        
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
                            break;
                        case QUERY_TAB_ID:
                            ShowWindow(hToolWindowDisplayTabContainer,SW_HIDE);
                            ShowWindow(hToolWindowQueryTabContainer,SW_SHOW);                                                        
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
      
        /* WM_CLOSE: system or user has requested to close the window/application */              
        case WM_CLOSE:
            /* don't destory this window, but make it invisible */            
            ShowWindow(hwnd,SW_HIDE);
            //DestroyWindow(hwnd);
            //hToolWindow=NULL;           
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

    // get position of the window that we may snap to
    GetWindowRect(snapToWin,&snapToRect);
    
    //check if window is close enough to snap to
    if ( (rect->left+winsize.cx<(snapToRect.left-SNAP_PIXEL_RANGE)) || (rect->left>(snapToRect.right+SNAP_PIXEL_RANGE)) )
        return false;

    if ( (rect->top+winsize.cy<(snapToRect.top-SNAP_PIXEL_RANGE)) || (rect->top>(snapToRect.bottom+SNAP_PIXEL_RANGE)) )
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


/* main functions */
/* ------------------------------------------------------------------------------------------------------------------------ */
// !! why does this need a window handle argument? - Rowan
//      answer: to make the main window modal when file dialog box opens. I have changed it 
//              to use the global variable instead. - Shane
void loadFile()
{
    OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW - which note?
    ofn.hwndOwner = hMainWindow;
    ofn.lpstrFilter = "JPG Files (*.jpg)\0*.jpg\0All Files (*.*)\0*.*\0";
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
        
        // create tool window and display
        if (!hToolWindow)
            setupToolWindow();
        ShowWindow(hToolWindow,SW_SHOW);
        
        // enable Window menu items
        EnableMenuItem(hMainMenu,IDM_IMAGEWINDOW,false);
        EnableMenuItem(hMainMenu,IDM_TOOLSWINDOW,false);        
        
        #if TMP_USE_IMAGE_MANIPULATION
    	// !! These NULL args need to be replaced with the HWNDs of the GL drawable areas
    	//	of the overview window and main window respectively. 
        //      - Replace with hMainWindow and hImageWindow. - Shane
        //          Will GL draw over entire window surface? Or can you give it co-ords? If it draws over entire surface,
        //          we may need to give it child windows instead or it will draw over the menus, etc. - Shane
        //		- Child windows the best solution, that way the GL code, etc. doesn't need to know about
		//			the windowing system.  Have used hMainWindow and hImageWindow for interim testing.. - Rowan
		//      - Have changed to child windows - Shane
	    image_handler = new ImageHandler::ImageHandler(hMainWindowDisplay, hImageWindowDisplay, ofn.lpstrFile);
	    if (image_handler) {
			if (image_handler->status != 0) {
				// An error occurred instantiaing the image handler class.
				MessageBox (NULL, image_handler->error_text , "Parbat3D :: ImageHandler", 0);
			} else {
				// Continue initialization
				image_handler->post_init();
			}
		} else { // Object wasn't created - this is probably terminal
			MessageBox (NULL, "Could not instantiate ImageHandler." , "Parbat3D :: ImageHandler", 0);
		}

		#endif	// TMP_USE_IMAGE_MANIPULATION
    }
}






