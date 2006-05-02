/*
modifications as of 1/5/06 7:00PM:
    fixed tab control
    other minor things
modifications 1/5/06:
    fixed snapped windows, but a bug still exists
    changed tool & image windows to be a child of main window (changed where they appear when minimized)
    disabled mimimizing/restoring tool & image windows on main window minimize/resize (now done automatically due to line above)
    put function declartions in main.h & moved functions into seperate sections in same file
    changed tool window style
    add tab control & test tab containers - tab change notification not working
*/

#include <Windows.h>
#include <Commctrl.h>
#include <iostream>
#include "main.h"

using namespace std;

/* Make the classname into a global variable */
char szMainWindowClassName[] = "Parabat3D Main Window";
char szImageWindowClassName[] = "Parabat3D Image Window";
char szToolWindowClassName[] = "Parabat3D Tool Window";
char szLabelClassName[] = "Tool Window channels";

HINSTANCE hThisInstance;

/* Create global variables for Window handles */
HWND hMainWindow;
HWND hImageWindow;
HWND hToolWindow;
HWND hDesktop;

/* create global variables for window controls */
HWND hToolWindowTabControl;
HWND hToolWindowDisplayTabContainer;
HWND hToolWindowQueryTabContainer;

/* Variables to record when the windows have snapped to main wndow */
int imageWindowIsSnapped=false;
int toolWindowIsSnapped=false;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)

{
    MSG messages;            /* Here messages to the application is saved */

    /* open console for debugging messages (doesn't work - cout messages don't appear) */
    /*HANDLE hConsole;
    AllocConsole();
    hConsole=CreateConsoleScreenBuffer(GENERIC_WRITE+GENERIC_READ,FILE_SHARE_READ,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
    SetStdHandle(STD_OUTPUT_HANDLE,hConsole);
    SetConsoleActiveScreenBuffer(hConsole);
    cout << "test";*/
    
    /* load window classes for controls */
    InitCommonControls();
    
    // store (this program's) instance handle
    hThisInstance=hInstance;
    
    // get desktop window
    hDesktop=GetDesktopWindow();
    
    // create & show main window
    hMainWindow=setupMainWindow(hThisInstance);
   
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
HWND setupMainWindow(HINSTANCE hThisInstance)
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

    /* Register the window class, if fail quit the program */
    if(!RegisterClassEx(&wincl)) return 0;
    
    /* The class is registered, lets create the program*/
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

    menu = LoadMenu(hThisInstance, MAKEINTRESOURCE(ID_MENU));
    SetMenu(hMainWindow, menu);    

    /* Make the window visible on the screen */
    ShowWindow(hMainWindow, SW_SHOW);
    
    return hMainWindow;
}

/* This function is called by the Windowsfunction DispatchMessage( ) */
LRESULT CALLBACK MainWindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static POINT snapMouseOffset;
    static RECT prevMainWindowRect;
    static RECT prevImageWindowRect;
    static RECT prevToolWindowRect;
    static int toolWindowIsMovingToo;
    static int imageWindowIsMovingToo;    
        
    switch (message)                  /* handle the messages */
    {
       case WM_CREATE:
            hImageWindow=NULL;
            hToolWindow=NULL;
            break;
       case WM_COMMAND:
           switch( wParam )
           {
             case IDM_FILEOPEN:
                  loadFile(hwnd);
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

      case WM_NCLBUTTONDOWN:
           if(wParam == HTCAPTION)  
           {
               getMouseWindowOffset(hwnd,(int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam),&snapMouseOffset);
                     
               /* record current window positions before moving */
               GetWindowRect(hwnd,&prevMainWindowRect);  
               GetWindowRect(hImageWindow,&prevImageWindowRect); 
               GetWindowRect(hToolWindow,&prevToolWindowRect);
               
               /* move the currently snapped windows with this one */
               imageWindowIsMovingToo=imageWindowIsSnapped;
               toolWindowIsMovingToo=toolWindowIsSnapped;                  
           }
           return DefWindowProc(hwnd, message, wParam, lParam); 

      case WM_MOVING:
           /* snap to windows and/or move snapped windows as the main window is moved */
           snapWindow(hDesktop,(RECT*)lParam,&snapMouseOffset);           

           /* bug: when calling snapWindow the second time the main window does not visually snap to the tool window, but snapWindow still returns true */
           if (!imageWindowIsMovingToo)
              imageWindowIsSnapped=snapWindow(hImageWindow,(RECT*)lParam,&snapMouseOffset); 

           if ((!toolWindowIsMovingToo))
              toolWindowIsSnapped=snapWindow(hToolWindow,(RECT*)lParam,&snapMouseOffset);
              
           moveSnappedWindows((RECT*)lParam,&prevMainWindowRect,&prevImageWindowRect,&prevToolWindowRect,imageWindowIsMovingToo,toolWindowIsMovingToo);
           break;
      
      case WM_SIZE:
           WINDOWPLACEMENT wp;
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
      case WM_CLOSE:
           DestroyWindow( hwnd );
           return 0;

      case WM_DESTROY:
           PostQuitMessage (0);
           return 0;

      break;
      default:                   /* for messages that we don't deal with */
      return DefWindowProc(hwnd, message, wParam, lParam);
   }
  return 0;
}


/* ------------------------------------------------------------------------------------------------------------------------ */
/* Image Window Functions */
HWND setupImageWindow(HINSTANCE hThisInstance)
{
    WNDCLASSEX wincl;        /* Datastructure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szImageWindowClassName;
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

    /* Register the window class, if fail quit the program */
    if(!RegisterClassEx(&wincl)) return 0;
    
    /* Get Main Window coords for Image Window alignment*/
    RECT rect;
    GetWindowRect(hMainWindow, &rect);
    
    /* The class is registered, lets create the program*/
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

    imageWindowIsSnapped=true;

    return hImageWindow;
}

/* This function is called by the Windowsfunction DispatchMessage( ) */
LRESULT CALLBACK ImageWindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static POINT snapMouseOffset;     // mouse offset relative to window, used for snapping
    
    switch (message)                  /* handle the messages */
    {
       case WM_CREATE:
            break;
            
       case WM_COMMAND:
           switch( wParam )
           {

           }
           break;

      case WM_NCLBUTTONDOWN:
           if(wParam == HTCAPTION)  /* user has clicked down mouse button on title bar of window */
           {              
               getMouseWindowOffset(hwnd,(int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam),&snapMouseOffset);
           }
           /* also let windows perform default operation (or it stuffs up) */
           return DefWindowProc(hwnd, message, wParam, lParam); 
            
      case WM_MOVING:  /* window is being dragged by user */
           snapWindow(hDesktop,(RECT*)lParam,&snapMouseOffset);      
           imageWindowIsSnapped=snapWindow(hMainWindow,(RECT*)lParam,&snapMouseOffset);
           break;

      case WM_MOVE:    /* window has been moved */
           break;
                             
      case WM_CLOSE:   /* user/software requests to close window */
           ShowWindow(hwnd,SW_HIDE);
           //DestroyWindow(hwnd);
           //hImageWindow=NULL;
           return 0;

      case WM_DESTROY:
           PostQuitMessage (0);
           return 0;

      break;
      default:                   /* for messages that we don't deal with */
      return DefWindowProc(hwnd, message, wParam, lParam);
   }
  return 0;
}

/* ------------------------------------------------------------------------------------------------------------------------ */
/* Tools Window Functions */
HWND setupToolWindow(HINSTANCE hThisInstance)
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

    /* Register the window class, if fail quit the program */
    if(!RegisterClassEx(&wincl)) return 0;
    
    /* Get Main Window Location for image window alignment*/
    GetWindowRect(hMainWindow,&rect);
    
    /* The class is registered, lets create the program*/
    hToolWindow =CreateWindowEx(
           WS_EX_TOOLWINDOW,                   /* Extended possibilites for variation */
           szToolWindowClassName,         /* Classname */
           "Tools",             /* Title Text */
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
    
    /* indicate tool window as snapped to the main window */
    toolWindowIsSnapped=true;

    /* get position & size based on parent window */
    GetClientRect(hToolWindow,&rect);

    /* create tab control */
    hToolWindowTabControl =CreateWindowEx(
           0,                   /* Extended possibilites for variation */
           WC_TABCONTROL,         /* Classname */
           "Tools",             /* Title Text */
           WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, /* defaultwindow */
           rect.left,       /* Windows decides the position */
           rect.top,       /* where the window end up on the screen */
           rect.right,                 /* The programs width */
           rect.bottom,                 /* and height in pixels */
           hToolWindow,        /* The window is a childwindow to main window */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* add tabs */
    tie.mask=TCIF_TEXT;
    tie.pszText="Display";
    TabCtrl_InsertItem(hToolWindowTabControl, 0, &tie);
    tie.mask=TCIF_TEXT;
    tie.pszText="Query";
    TabCtrl_InsertItem(hToolWindowTabControl, 1, &tie);

    /* get position & size based on parent window */
    const int SPACING_FOR_TAB_HEIGHT=30;
    const int SPACING_FOR_BOARDER=5;
    GetClientRect(hToolWindowTabControl,&rect);   
    rect.bottom-=SPACING_FOR_TAB_HEIGHT+SPACING_FOR_BOARDER;
    rect.right-=SPACING_FOR_BOARDER+SPACING_FOR_BOARDER;
    
    /* create tab containers */
    hToolWindowDisplayTabContainer =CreateWindowEx(
           0,                   /* Extended possibilites for variation */
           "static",         /* Classname */
           "Display tab container",        /* Title Text */
           WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, /* defaultwindow */
           SPACING_FOR_BOARDER,       /* Windows decides the position */
           SPACING_FOR_TAB_HEIGHT,       /* where the window end up on the screen */
           rect.right,                 /* The programs width */
           rect.bottom,                 /* and height in pixels */
           hToolWindowTabControl,        /* The window is a childwindow to main window */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           ); 
           
    hToolWindowQueryTabContainer =CreateWindowEx(
           0,                   /* Extended possibilites for variation */
           "static",         /* Classname */
           "Query tab container",        /* Title Text */
           WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, /* defaultwindow */
           SPACING_FOR_BOARDER,       /* Windows decides the position */
           SPACING_FOR_TAB_HEIGHT,       /* where the window end up on the screen */
           rect.right,                 /* The programs width */
           rect.bottom,                 /* and height in pixels */
           hToolWindowTabControl,        /* The window is a childwindow to main window */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           ); 
           
     /* create test control inside a container (remove later) */
     CreateWindowEx(
           0,                   /* Extended possibilites for variation */
           "static",         /* Classname */
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
               
    return hToolWindow;
}

/* This function is called by the Windowsfunction DispatchMessage( ) */
LRESULT CALLBACK ToolWindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static POINT snapMouseOffset;     /* mouse offset relative to window, used for snapping */
    NMHDR *nmhdr;                     /* structure used for WM_NOTIFY events */
            
    switch (message)                  /* handle the messages */
    {
          
        
        case WM_NOTIFY:
            nmhdr=(NMHDR*)lParam;
            switch(nmhdr->code)
            {
                case TCN_SELCHANGE: /* check if tab selection has changed */
                    /* display container associated with tab selection */
                    switch(TabCtrl_GetCurSel(hToolWindowTabControl))
                    {
                        case 0:
                            ShowWindow(hToolWindowDisplayTabContainer,SW_SHOW);
                            ShowWindow(hToolWindowQueryTabContainer,SW_HIDE);                            
                            break;
                        case 1:
                            ShowWindow(hToolWindowDisplayTabContainer,SW_HIDE);
                            ShowWindow(hToolWindowQueryTabContainer,SW_SHOW);                                                        
                            break;
                    }    
                    break;
            }    
            break;

      case WM_NCLBUTTONDOWN:
           if(wParam == HTCAPTION)    /* check if user has clicked down mouse button on title bar of window */
           {
               getMouseWindowOffset(hwnd,(int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam),&snapMouseOffset);
           }
           //also let windows perform default operation (or it stuffs up)
           return DefWindowProc(hwnd, message, wParam, lParam); 
            
      case WM_MOVING:  /* window is being dragged by user */
           snapWindow(hDesktop,(RECT*)lParam,&snapMouseOffset);      
           toolWindowIsSnapped=snapWindow(hMainWindow,(RECT*)lParam,&snapMouseOffset);
           break;
            
      case WM_CLOSE:   /* user/software requests to close window */
           ShowWindow(hwnd,SW_HIDE);
           //DestroyWindow(hwnd);
           //hToolWindow=NULL;           
           return 0;

      case WM_DESTROY:
           PostQuitMessage (0);
           return 0;

      break;
      default:                   /* for messages that we don't deal with */
      return DefWindowProc(hwnd, message, wParam, lParam);
   }
  return 0;
}


/* ------------------------------------------------------------------------------------------------------------------------ */
/* general window functions */
 

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

// snap a window to another window while it is being moved
// returns: true if window has been snapped, false if not
int snapWindow(HWND snapToWin,RECT *rect,POINT *mouseOffset)
{
    const int SNAP_PIXEL_RANGE=10;
    RECT snapToRect;
    SIZE winsize={rect->right-rect->left, rect->bottom-rect->top};
    POINT mouse;
    int difference;
    int isSnapped=false;
    
    GetCursorPos(&mouse);        
    GetWindowRect(snapToWin,&snapToRect);
    rect->top=mouse.y-mouseOffset->y;
    rect->left=mouse.x-mouseOffset->x;
    rect->bottom=rect->top+winsize.cy;
    rect->right=rect->left+winsize.cx;   

    //check if window is too far away to snap to
    if ( (rect->left+winsize.cx<(snapToRect.left-SNAP_PIXEL_RANGE)) || (rect->left>(snapToRect.right+SNAP_PIXEL_RANGE)) )
        return false;

    if ( (rect->top+winsize.cy<(snapToRect.top-SNAP_PIXEL_RANGE)) || (rect->top>(snapToRect.bottom+SNAP_PIXEL_RANGE)) )
        return false;


    //snap (top) to bottom
    difference=(rect->top-(snapToRect.bottom));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->top=snapToRect.bottom;
       isSnapped=true;
    }
    
    // snap (bottom) to bottom
    difference=((rect->top+winsize.cy)-(snapToRect.bottom));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->top=snapToRect.bottom-winsize.cy;
       isSnapped=true;
    }    
    
    // snap (left) to left
    difference=(rect->left-(snapToRect.left));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->left=snapToRect.left;
       isSnapped=true;
    }
    
    // snap (right) to left
    difference=((rect->left+winsize.cx)-(snapToRect.left));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->left=snapToRect.left-winsize.cx;
       isSnapped=true;       
    }    
    
    // snap (right) to right
    difference=(rect->left+winsize.cx-(snapToRect.right));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->left=snapToRect.right-winsize.cx;
       isSnapped=true;       
    }    
    
    // snap (left) to right
    difference=(rect->left-(snapToRect.right));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->left=snapToRect.right;
       isSnapped=true;       
    }        

    // snap (top) to top
    difference=(rect->top-(snapToRect.top));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->top=snapToRect.top;
       isSnapped=true;       
    }      

    // snap (bottom) to top
    difference=((rect->top+winsize.cy)-(snapToRect.top));
    if ((difference<SNAP_PIXEL_RANGE)&&(difference>-SNAP_PIXEL_RANGE))
    {
       rect->top=snapToRect.top-winsize.cy;
       isSnapped=true;
    }      
    
    // restore width & height
    rect->bottom=rect->top+winsize.cy;
    rect->right=rect->left+winsize.cx;   
    return isSnapped;
}

//calculate mouse co-ords relative to position of window
void getMouseWindowOffset(HWND hwnd,int mx,int my,POINT *mouseOffset)
{
    RECT win;
    GetWindowRect(hwnd,&win);
    int a=mx-win.left;
    mouseOffset->x=mx-win.left;
    mouseOffset->y=my-win.top;
}


/* main functions */
/* ------------------------------------------------------------------------------------------------------------------------ */

void loadFile(HWND hwnd)
{
     OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
    ofn.hwndOwner = hwnd;
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
            hImageWindow=setupImageWindow(hThisInstance);
        ShowWindow(hImageWindow,SW_SHOW);
        
        // create tool window and display
        if (!hToolWindow)
            hToolWindow=setupToolWindow(hThisInstance);
        ShowWindow(hToolWindow,SW_SHOW);
    }
}






