#include <Windows.h>
#include <Commctrl.h>
#include <iostream>
#include "main.h"
#include "config.h"

#if TMP_USE_IMAGE_MANIPULATION
#include "ImageHandler.h"
ImageHandler::ImageHandler* image_handler;	// Instance handle
int	image_handler_status;
#endif

using namespace std;

/* Unique class names for our main windows */
char szMainWindowClassName[] = "Parabat3D Main Window";
char szImageWindowClassName[] = "Parabat3D Image Window";
char szToolWindowClassName[] = "Parabat3D Tool Window";
/* pre-defined class names for controls */
char szStaticControl[] = "static";  /* static text control */

/* a handle that identifies our process */
HINSTANCE hThisInstance;

/* global variables to store handles to our windows */
HWND hMainWindow;
HWND hImageWindow;
HWND hToolWindow;
HWND hDesktop;

/* global variables to store handles to our window controls */
HWND hToolWindowTabControl;
HWND hToolWindowDisplayTabContainer;
HWND hToolWindowQueryTabContainer;

/* Variables to record when the windows have snapped to main wndow */
int imageWindowIsSnapped=false;
int toolWindowIsSnapped=false;

/* Define id numbers for the tab's in the tool window */
enum {DISPLAY_TAB_ID,QUERY_TAB_ID};

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
    HMENU menu;              /* Handle of the menu */
    WNDCLASSEX wincl;        /* Datastructure for the windowclass */

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
    
    /* The class is registered, lets create a window based on it*/
    hMainWindow = CreateWindowEx(
           0,                   /* Extended possibilites for variation */
           szMainWindowClassName,         /* Classname */
           "Parbat3D",         /* Title Text */
           WS_OVERLAPPEDWINDOW, /* defaultwindow */
           50, //CW_USEDEFAULT,       /* x position */
           50, //CW_USEDEFAULT,       /* y position */
           250,                 /* The programs width */
           300,                 /* and height in pixels */
           hMainWindow,        /* The window is a childwindow to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );
    if (!hMainWindow)
        return false;           

    menu = LoadMenu(hThisInstance, MAKEINTRESOURCE(ID_MENU));
    SetMenu(hMainWindow, menu);    

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
             
                case IDM_TOOL:
                    MessageBox( hwnd, (LPSTR) "Function Not Yet Implemented.",
                              (LPSTR) szMainWindowClassName,
                              MB_ICONINFORMATION | MB_OK );
                    return 0;

                case IDM_HELPCONTENTS:
                    WinHelp( hwnd, (LPSTR) "HELPFILE.HLP",
                           HELP_CONTENTS, 0L );
                    return 0;

                case IDM_FILEEXIT:
                    SendMessage( hwnd, WM_CLOSE, 0, 0L );
                    return 0;

                case IDM_HELPABOUT:
                    MessageBox (NULL, "Created by Team Imagery 2006" , "Parbat3D", 0);
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
            /* destroy this window */
            DestroyWindow( hwnd );
            return 0;

        /* WM_DESTROY: window is being destroyed */
        case WM_DESTROY:
            /* post a message that will cause WinMain to exit from the message loop */
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

            /* HTCAPTION: mouse button was pressed down on the window title bar */            
            /* HTSIZE: mouse button was pressed down on the sizing border of window */                                    
            if ((wParam == HTCAPTION))
            {
                /* get the mouse co-ords relative to the window */
                getMouseWindowOffset(hwnd,(int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam),&moveMouseOffset);               
            }    
            else if (wParam == HTSIZE)
            {
                GetWindowRect(hwnd,&sizeWindowPosition);
                sizeMousePosition.x=(int)(short)LOWORD(lParam);
                sizeMousePosition.y=(int)(short)HIWORD(lParam);                
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
            //setNewWindowSize((RECT*)lParam,&sizeWindowPosition,&sizeMousePosition,(int)wParam);

            /* snap the window to the edge of the desktop (if near it) */
            //snapWindowBySizing(hDesktop,(RECT*)lParam,(int)wParam);   
                        
            /* snap the window the main window (if near it) */
            //imageWindowIsSnapped=snapWindowBySizing(hMainWindow,(RECT*)lParam,(int)wParam);           
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
           WS_EX_TOOLWINDOW,              /* Extended styles */
           szToolWindowClassName,         /* Classname */
           "Tools",                         /* Title Text */
           WS_OVERLAPPEDWINDOW+WS_VSCROLL, /* defaultwindow */
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
           "Display tab container",         /* text to be displayed */
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
           
     /* create test control inside a container (remove later) */
     CreateWindowEx(
           0,                   /* Extended possibilites for variation */
           szStaticControl,         /* Classname */
           "Test control inside query container",        /* Title Text */
           WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, /* defaultwindow */
           50,       /* Windows decides the position */
           50,       /* where the window end up on the screen */
           100,                 /* The programs width */
           100,                 /* and height in pixels */
           hToolWindowQueryTabContainer,        /* The window is a childwindow to main window */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );            
               
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
            newPos->bottom=oldPos->bottom;
            break;
        case WMSZ_TOP:
            newPos->top=oldPos->top;
            break;
        case WMSZ_LEFT:
            newPos->left=oldPos->left;
            break;
        case WMSZ_RIGHT:
            newPos->right=oldPos->right;
            break;
        case WMSZ_TOPLEFT:
            newPos->top=my;
            newPos->left=mx;
            break;
        case WMSZ_TOPRIGHT:
            newPos->top=my;
            newPos->right=mx;
            break;
        case WMSZ_BOTTOMLEFT:
            newPos->bottom=my;
            newPos->left=mx;
            break;
        case WMSZ_BOTTOMRIGHT:
            newPos->bottom=my;
            newPos->right=mx;
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
        
        #if TMP_USE_IMAGE_MANIPULATION
    	// !! These NULL args need to be replaced with the HWNDs of the GL drawable areas
    	//	of the overview window and main window respectively. 
        //      - Replace with hMainWindow and hImageWindow. - Shane
        //          Will GL draw over entire window surface? Or can you give it co-ords? If it draws over entire surface,
        //          we may need to give it child windows instead or it will draw over the menus, etc. - Shane
	    image_handler = new ImageHandler::ImageHandler(NULL, NULL, ofn.lpstrFile, &image_handler_status);
	    if (image_handler_status != 0) {
			// An error occurred initializing the image handler class.
			// Print image_handler_status for `useful' debug info.
		}
		#endif
    }
}






