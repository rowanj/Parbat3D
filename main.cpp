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
#include "console.h"

using namespace std;

ImageHandler::ImageHandler* image_handler = NULL;	// Instance handle ptr

/* Unique class names for our main windows */
char szMainWindowClassName[] = "Parbat3D Main Window";
char szOverviewWindowClassName[] = "Parbat3D Overview Window";
char szImageWindowClassName[] = "Parbat3D Image Window";
char szToolWindowClassName[] = "Parbat3D Tool Window";
char szDisplayClassName[] = "Parbat3D Display Window";
/* pre-defined class names for controls */
char szStaticControl[] = "static";  /* static text control */

/* a handle that identifies our process */
HINSTANCE hThisInstance;

/* global variables to store handles to our windows */
HWND hRed, hBlue, hGreen;
HWND hMainWindow=NULL;
HWND hOverviewWindow=NULL;
HWND hImageWindow=NULL;
HWND hToolWindow=NULL;
HWND hDesktop;

/* global variables to store handles to our window controls */
HMENU hMainMenu;
HWND hToolWindowTabControl;
HWND hToolWindowDisplayTabContainer;
HWND hToolWindowQueryTabContainer;
HWND hToolWindowImageTabContainer;
HWND hToolWindowCurrentTabContainer;
HWND hToolWindowDisplayTabHeading;
HWND hToolWindowImageTabHeading;
HWND hToolWindowQueryTabHeading;
HWND hToolWindowScrollBar;
HWND hImageWindowDisplay;
HWND hOverviewWindowDisplay;
HWND *redRadiobuttons;                // band radio buttons
HWND *greenRadiobuttons;
HWND *blueRadiobuttons;
HWND *imageBandValues;                // values displayed under query tab
HWND cursorXPos, cursorYPos;          // position of cursor in image
HWND hupdate;
int bands = 0;                        // for use with radio buttons


/* Define id numbers for the tab's in the tool window */
enum {DISPLAY_TAB_ID,QUERY_TAB_ID,IMAGE_TAB_ID};

/* Used for loading and saving window position and sizes */
settings winPos ("settings.ini");

/* objects used for painting/drawing */
HFONT hBoldFont,hNormalFont,hHeadingFont;
HPEN hTabPen;
HBRUSH hTabBrush;

/* variables used to store addresses to window procedures */
WNDPROC oldTabControlProc,oldDisplayTabContainerProc,oldQueryTabContainerProc,oldImageTabContainerProc,oldScrollBarContainerProc;

char *filename=NULL;                    // currently open image filename

/* constants */
const int OVERVIEW_WINDOW_WIDTH=250;    /* width of the overview window in pixels */

/* program entry point */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)
{
 	MSG messages;     /* Here messages to the application is saved */
  

    InitCommonControls();           /* load window classes for common controls */    
    
    hThisInstance=hInstance;        /* record this process's instance handle */
    hDesktop=GetDesktopWindow();    /* record handle to desktop window */    
    
    Console::open();
    Console::write("testing1!\n");
    Console::write(new string("testing2!\n"));

    
    /* Register window classes */
    if ((!registerMainWindow()) || (!registerToolWindow()) || (!registerImageWindow()) || (!registerOverviewWindow()))
    {
        /* report error if window classes could not be registered */
        MessageBox(0,"Unable to register window class","Parbat3D Error",MB_OK);
        return 0;
    }
        
    /* Setup main & image windows */
    //  note: image window must be created before main window
    //  note: tool window is only setup when an image is loaded
    if ((!setupMainWindow()) || (!setupImageWindow()) || (!setupOverviewWindow()))
    {
        /* report error if windows could not be setup (note: unlikely to happen) */
        MessageBox(0,"Unable to create window","Parbat3D Error",MB_OK);
        return 0;
    }
  
    /* Execute the message loop. It will run until GetMessage( ) returns 0 */
    while(GetMessage(&messages, NULL, 0, 0))
    {
        /* Send message to the associated window procedure */
        DispatchMessage(&messages);
    }

    /* End program */
    return messages.wParam;
}


/* ------------------------------------------------------------------------------------------------------------------------ */
/* Main Window Functions */

int registerMainWindow()
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
int setupMainWindow()
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
LRESULT CALLBACK MainWindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
                        ShowWindow(hImageWindow,imageWindowState);
                    if (toolWindowState!=0)
                        ShowWindow(hToolWindow,toolWindowState);
                    ShowWindow(hOverviewWindow,SW_SHOW);
                    break;

                case SIZE_MINIMIZED:
                    /* record the current state of the child windows */
                    if (hImageWindow!=NULL)
                    {
                        if (!IsWindowVisible(hImageWindow))
                            imageWindowState=0;
                        else
                            imageWindowState=SW_SHOW;
                        
                    }
                    else
                    {
                        imageWindowState=0;
                    }

                    if (hToolWindow!=NULL)
                    {
                        if (!IsWindowVisible(hToolWindow))
                            toolWindowState=0;
                        else
                            toolWindowState=SW_SHOW;                        
                    }
                    else
                    {
                        toolWindowState=0;
                    }

                    /* hide the child windows */     
                    ShowWindow(hImageWindow,SW_HIDE);            
                    ShowWindow(hToolWindow,SW_HIDE);                    
                    ShowWindow(hOverviewWindow,SW_HIDE);                    
                    break;
		    }    
  	       return 0;
  	       
        default:                   /* for messages that we don't deal with */
            /* let windows peform the default operation based on the message */
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0; 
}


/* ------------------------------------------------------------------------------------------------------------------------ */
/* Overview Window Functions */

/* register overview window's class */
int registerOverviewWindow()
{
    WNDCLASSEX wincl;        /* Datastructure for the windowclass */
        
    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szOverviewWindowClassName;
    wincl.lpfnWndProc = OverviewWindowProcedure; /* This function is called by windows */
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
    return RegisterClassEx(&wincl);
}


/* create main window */
int setupOverviewWindow()
{

    RECT rect;
    
    /* get co-ords of image window */
    GetWindowRect(hImageWindow,&rect);
        
    /* Create main window */
    hOverviewWindow = CreateWindowEx(0, szOverviewWindowClassName, "Parbat3D",
		WS_OVERLAPPED+WS_CAPTION+WS_SYSMENU+WS_MINIMIZEBOX, rect.left-OVERVIEW_WINDOW_WIDTH, rect.top, OVERVIEW_WINDOW_WIDTH, 296,
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
    GetClientRect(hOverviewWindow,&rect);

    /* setup font objects */
    HDC hdc=GetDC(hOverviewWindow);      
    hNormalFont=CreateFont(-MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72),0,0,0,400,false,false,false,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,"Tahoma"); //"MS Sans Serif" //Tahoma    
    hBoldFont=CreateFont(-MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72),0,0,0,600,false,false,false,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,"Tahoma"); //"MS Sans Serif" //Tahoma
    hHeadingFont=CreateFont(-MulDiv(9, GetDeviceCaps(hdc, LOGPIXELSY), 72),0,0,0,600,false,false,false,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,"Tahoma"); //"MS Sans Serif" //Tahoma
    ReleaseDC(hOverviewWindow,hdc);    

    /* create a child window that will be used by OpenGL */
    hOverviewWindowDisplay=CreateWindowEx( 0, szDisplayClassName, NULL, WS_CHILD|WS_VISIBLE,
		rect.left, rect.top, rect.right, rect.bottom, hOverviewWindow, NULL, hThisInstance, NULL);
   
    /* Make the window visible on the screen */
    ShowWindow(hOverviewWindow, SW_SHOW);
    
    return true;
}

/* This function is called by the Windows function DispatchMessage( ) */
/* All messages/events related to the main window (or it's controls) are sent to this procedure */
LRESULT CALLBACK OverviewWindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
            GetClientRect(hOverviewWindow,&rect);
            MoveWindow(hOverviewWindowDisplay,rect.left,rect.top,rect.right,rect.bottom,true);
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
                if (MessageBox(hOverviewWindow,"Are you sure you wish quit?\nAn image is currently open.","Parbat3D",MB_YESNO|MB_ICONQUESTION)!=IDYES)
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
            if (image_handler) {
                /* Re-size OpenGL stuff */
                image_handler->resize_window();
            } else {
                /* re-position message */
                GetClientRect(hwnd,&rect);
                textPos.x=(rect.right-rect.left)/2 - textSize.cx/2;
                textPos.y=(rect.bottom-rect.top)/2 - textSize.cy/2;            
            }    
            break;

        case WM_MOUSEMOVE:
            {
                if (image_handler) {
                    /* Get mouse screen position */
                    int mx = (short)LOWORD(lParam);
                    int my = (short)HIWORD(lParam);
                    
                    /* Convert screen position to image position */
                    int vx = image_handler->get_viewport_x(); /* get top right corner of screen position in image */
                    int vy = image_handler->get_viewport_y();
                    int lod = (int)pow((double)2,(double)image_handler->get_LOD());
                    int ix = (mx + vx) * lod;
                    int iy = (my + vy) * lod;
                    
                    /* Get band values */
                    int* bv = image_handler->get_pixel_values(ix, iy);
                    
                    string leader = "";
                    
                    /* Update display of cursor position */
                    SetWindowText(cursorXPos, (char *) makeMessage(leader, ix));
                    SetWindowText(cursorYPos, (char *) makeMessage(leader, iy));

                    /* Update display of pixel values under query tab */                    
                    if (bv) { /* make sure the band values were returned */
                        for (int i=1; i<=bands; i++)
                            SetWindowText(imageBandValues[i], (char *) makeMessage(leader, bv[i-1]));
                    }
                    
                    delete[] bv;
                }
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
    RECT rect;
    int mx,my;
    const int IMAGE_WINDOW_WIDTH=700;
    const int IMAGE_WINDOW_HEIGHT=600;

    /* Get the width & height of the desktop window */
    GetWindowRect(hDesktop,&rect);

    /* Get the stored window position or use defaults if there's a problem */
    mx = atoi(winPos.getSetting("OverviewX").c_str());
	my = atoi(winPos.getSetting("OverviewY").c_str());
	if ((mx <= 0) || (mx >= rect.right))
        mx = (rect.right /2) - ((IMAGE_WINDOW_WIDTH+OVERVIEW_WINDOW_WIDTH) /2);         /* default x position to center windows */
    mx+=OVERVIEW_WINDOW_WIDTH;                                                          /* leave room for overview window */
	       
	if ((my <= 0) || (my >= rect.bottom))
       my = (rect.bottom /2) - (IMAGE_WINDOW_HEIGHT/2);                                 /* default y position to center windows */

    /* create image window */
    hImageWindow =CreateWindowEx(0, szImageWindowClassName, "Image Window",
	     WS_POPUP+WS_SYSMENU+WS_CAPTION+WS_MAXIMIZEBOX+WS_VSCROLL+WS_HSCROLL+WS_SIZEBOX,
	     mx, my, IMAGE_WINDOW_WIDTH, IMAGE_WINDOW_HEIGHT, hMainWindow, NULL, hThisInstance, NULL);
    if (!hImageWindow)
        return false;  

    /* get client area of image window */
    GetClientRect(hImageWindow,&rect);

    /* create a child window that will be used by OpenGL */
    hImageWindowDisplay=CreateWindowEx( 0, szDisplayClassName, NULL, WS_CHILD+WS_VISIBLE,
		rect.left, rect.top, rect.right, rect.bottom, hImageWindow, NULL, hThisInstance, NULL);

    if (hImageWindow==NULL)
        return false;


    return true;
}

/* display image information in image window's title bar */
void updateImageWindowTitle()
{
    /* Display the file name & zoom level on the image window title bar */
    string leader = "Image - ";
    string title  = makeMessage(leader,filename);
    title+=makeString(" (",int(100.0 / pow((double)2,(double)image_handler->get_LOD())));
    title+="%)";
	SetWindowText(hImageWindow, (char*) title.c_str());    
}

/* update image window's scroll bar display settings  */
void updateImageScrollbar()
{
    int LOD_width,LOD_height;

    SCROLLINFO info_x,info_y;

    /* get window size and image size at current zoom level */   
    info_x.nPage=image_handler->get_viewport_width();
    info_y.nPage=image_handler->get_viewport_height();    
    LOD_width=image_handler->get_LOD_width();
    LOD_height=image_handler->get_LOD_height();
    
    /* set scroll range */
    info_x.nMin=0;
    info_y.nMin=0;
    info_x.nMax=LOD_width;// - info_x.nPage;
    info_y.nMax=LOD_height;// - info_y.nPage;
    
    /* set scroll position */
    info_x.nPos=image_handler->get_viewport_x();
    info_y.nPos=image_handler->get_viewport_y();
    
    Console::write("updateScrollbarSettings():\n");
    Console::write("LOD_width=");
    Console::write(LOD_width);
    Console::write("\nLOD_height=");
    Console::write(LOD_height);
    Console::write("\nviewport_width=");
    Console::write(info_x.nPage);
    Console::write("\nviewport_height=");
    Console::write(info_y.nPage);
    Console::write("\nscroll_width=");
    Console::write(info_x.nMax);
    Console::write("\nscroll_height=");
    Console::write(info_y.nMax);
    Console::write("\nscroll_x=");
    Console::write(info_x.nPos);
    Console::write("\nscroll_y=");
    Console::write(info_y.nPos);
    
    /* set scrollbar info */
    info_x.cbSize=sizeof(SCROLLINFO);
    info_x.fMask=SIF_ALL;
    info_y.cbSize=sizeof(SCROLLINFO);
    info_y.fMask=SIF_ALL;    
    SetScrollInfo(hImageWindow,SB_HORZ,&info_x,true);
    SetScrollInfo(hImageWindow,SB_VERT,&info_y,true);    
      
}

/* scroll image window horizontally */
void scrollImageX(int scrollMsg)
{
    SCROLLINFO info;

    /* get current scroll position & range */    
    info.cbSize=sizeof(SCROLLINFO);
    info.fMask=SIF_ALL;
    GetScrollInfo(hImageWindow,SB_HORZ,&info);
     
    Console::write("scrollImageX() ");
    switch(LOWORD(scrollMsg))
    {
        case SB_LINEUP:
            info.nPos--;
            break;
        case SB_LINEDOWN:
            info.nPos++;
            break;
        case SB_PAGEUP:
            info.nPos-=image_handler->get_viewport_width();
            break;
        case SB_PAGEDOWN:
            info.nPos+=image_handler->get_viewport_width();                
            break;
        case SB_THUMBTRACK:
            info.nPos=info.nTrackPos;
            break;
        default:
            return;
    }

    // check new position is within scroll range
    if (info.nPos<info.nMin)
        info.nPos=info.nMin;
    else if (info.nPos>(info.nMax-info.nPage))
        info.nPos=info.nMax-info.nPage;

    Console::write("xpos=");
    Console::write(info.nPos);
    Console::write("\n");

    // update scroll position
    info.fMask=SIF_POS;
    SetScrollInfo(hImageWindow,SB_HORZ,&info,true);
    image_handler->set_viewport_x(info.nPos);
}

/* scroll image window vertically */
void scrollImageY(int scrollMsg)
{
    SCROLLINFO info;

    /* get current scroll position & range */    
    info.cbSize=sizeof(SCROLLINFO);
    info.fMask=SIF_ALL;
    GetScrollInfo(hImageWindow,SB_VERT,&info);
     
    Console::write("scrollImageY() ");
    switch(LOWORD(scrollMsg))
    {
        case SB_LINEUP:
            info.nPos--;
            break;
        case SB_LINEDOWN:
            info.nPos++;
            break;
        case SB_PAGEUP:
            info.nPos-=image_handler->get_viewport_height();
            break;
        case SB_PAGEDOWN:
            info.nPos+=image_handler->get_viewport_height();                
            break;
        case SB_THUMBTRACK:
            info.nPos=info.nTrackPos;
            break;
        default:
            return;
    }

    // check new position is within scroll range
    if (info.nPos<info.nMin)
        info.nPos=info.nMin;
    else if (info.nPos>(info.nMax-info.nPage))
        info.nPos=info.nMax-info.nPage;

    Console::write("ypos=");
    Console::write(info.nPos);
    Console::write("\n");

    // update scroll position
    info.fMask=SIF_POS;
    SetScrollInfo(hImageWindow,SB_VERT,&info,true);
    image_handler->set_viewport_y(info.nPos);
}

/* zoom the image in/out */
void zoomImage(int nlevels)
{
    const int MAX_LOD=6;
    int LOD;
 
    Console::write("zoomImage() nlevels=");   
    Console::write(nlevels);
    Console::write("\n");
    LOD=image_handler->get_LOD();
    LOD+=nlevels;
    if (LOD<0)
        LOD=0;
    else if (LOD>MAX_LOD)
        LOD=MAX_LOD;
    image_handler->set_LOD(LOD);
    updateImageWindowTitle();
    updateImageScrollbar();
}

/* This function is called by the Windowsfunction DispatchMessage( ) */
/* All messages/events related to the image window (or it's controls) are sent to this procedure */
LRESULT CALLBACK ImageWindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static POINT moveMouseOffset;     /* mouse offset relative to window, used for snapping */
    static POINT sizeMousePosition;   /* mouse position, used for sizing window */
    static RECT  sizeWindowPosition;  /* window position, used for sizing window */
    static RECT rect;                 /* for general use */
    
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

            /* snap the window to other windows if in range */
            snapInsideWindowByMoving(hDesktop,(RECT*)lParam);      
            snapWindowByMoving(hOverviewWindow,(RECT*)lParam);
            snapWindowByMoving(hToolWindow,(RECT*)lParam);            
            break;
    
        /* WM_SIZING: the window size is about to change */
        case WM_SIZING:

            /* set new window size based on position of mouse */
            setNewWindowSize((RECT*)lParam,&sizeWindowPosition,&sizeMousePosition,(int)wParam);

            /* snap the window to the edge of the desktop (if near it) */
            snapInsideWindowBySizing(hDesktop,(RECT*)lParam,(int)wParam);   
                        
            /* snap the window the main window (if near it) */
            snapWindowBySizing(hOverviewWindow,(RECT*)lParam,(int)wParam);           
            break;

        /* WM_SIZE: the window has been resized, minimized, or maximizsed, etc. */            
        case WM_SIZE:
           
            /* resize display/opengl window to fit new size */            
            GetClientRect(hImageWindow,&rect);
            MoveWindow(hImageWindowDisplay,rect.left,rect.top,rect.right,rect.bottom,true);
            
            /* update scroll bar settings */
            if (image_handler)
                updateImageScrollbar();
            return 0;
           
        case WM_SHOWWINDOW:
            /* update window menu item depending on whether window is shown or hidden */
            if (wParam)
                CheckMenuItem(hMainMenu,IDM_IMAGEWINDOW,MF_CHECKED|MF_BYCOMMAND);            
            else
                CheckMenuItem(hMainMenu,IDM_IMAGEWINDOW,MF_UNCHECKED|MF_BYCOMMAND);
            return 0;

        case WM_HSCROLL:
            scrollImageX(wParam);
            return 0;
            
        case WM_VSCROLL:
            scrollImageY(wParam);
            return 0;
            
        case WM_MOUSEWHEEL:
            zoomImage(GET_WHEEL_DELTA_WPARAM(wParam)/WHEEL_DELTA);
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
    return RegisterClassEx(&wincl);
}    

/* create tool window */
int setupToolWindow()
{
    TCITEM tie;  /* datastructure for tabs */
    RECT rect;

    const int SCROLLBAR_WIDTH=13;
    const int SCROLLBAR_TOP=25;

    
    /* Get Main Window Location for image window alignment*/
    GetWindowRect(hOverviewWindow,&rect);
    
    /* The class is registered, lets create the program*/
    hToolWindow =CreateWindowEx(0, szToolWindowClassName, "Tools",
           WS_POPUP+WS_CAPTION+WS_SYSMENU, rect.left, rect.bottom,
           250, 300, hImageWindow, NULL, hThisInstance, NULL);

    if (hToolWindow==NULL)
        return false;
    

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

	/* Assign number of image bands to global variable */
    bands = image_handler->get_image_properties()->getNumBands() + 1;

    /* calculate the width & height for our tab container windows */
    const int SPACING_FOR_TAB_HEIGHT=30;    /* the height of the tabs + a bit of spacing */
    const int SPACING_FOR_BOARDER=5;        /* left & right margain + spacing for tab control's borders */
    rect.bottom-=SPACING_FOR_TAB_HEIGHT+SPACING_FOR_BOARDER;
    rect.right-=SPACING_FOR_BOARDER+SPACING_FOR_BOARDER;
    
    /* create tab containers for each tab (a window that will be shown/hidden when user clicks on a tab) */
    /* Display tab container */
	hToolWindowDisplayTabContainer =CreateWindowEx( 0, szStaticControl, "", 
		WS_CHILD | WS_CLIPSIBLINGS | SS_OWNERDRAW, SPACING_FOR_BOARDER,
		SPACING_FOR_TAB_HEIGHT, rect.right-SCROLLBAR_WIDTH, 90 + (20 * bands), hToolWindowTabControl, NULL,
		hThisInstance, NULL); 

	hToolWindowDisplayTabHeading =CreateWindowEx( 0, szStaticControl, "Channel Selection", 
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_OWNERDRAW, 0,
		0, rect.right-SCROLLBAR_WIDTH, 20, hToolWindowDisplayTabContainer, NULL,
		hThisInstance, NULL); 		
           
	/* Query tab container */
    hToolWindowQueryTabContainer =CreateWindowEx(0, szStaticControl, "", 
		WS_CHILD | WS_CLIPSIBLINGS | SS_OWNERDRAW, SPACING_FOR_BOARDER,             /* left position relative to tab control */
           SPACING_FOR_TAB_HEIGHT, rect.right-SCROLLBAR_WIDTH, 80 + (20 * (bands+1)), hToolWindowTabControl,           /* The window is a childwindow of the tab control */
           NULL, hThisInstance, NULL);

	hToolWindowQueryTabHeading =CreateWindowEx( 0, szStaticControl, "Band Values", 
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_OWNERDRAW, 0,
		0, rect.right-SCROLLBAR_WIDTH, 20, hToolWindowQueryTabContainer, NULL,
		hThisInstance, NULL); 		
                      
    /* Image tab container */
    hToolWindowImageTabContainer =CreateWindowEx(0, szStaticControl, "",                            
           WS_CHILD | WS_CLIPSIBLINGS | SS_OWNERDRAW, SPACING_FOR_BOARDER,             /* left position relative to tab control */
           SPACING_FOR_TAB_HEIGHT, rect.right-SCROLLBAR_WIDTH, rect.bottom, hToolWindowTabControl, NULL,                            /* No menu */
           hThisInstance, NULL); 

	hToolWindowImageTabHeading =CreateWindowEx( 0, szStaticControl, "Image Properties", 
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_OWNERDRAW, 0,
		0, rect.right-SCROLLBAR_WIDTH, 20, hToolWindowImageTabContainer, NULL,
		hThisInstance, NULL); 		

           
    /* show display tab container */
    showToolWindowTabContainer(DISPLAY_TAB_ID);

    /* create scroll bar */
    GetClientRect(hToolWindow,&rect);
    hToolWindowScrollBar=CreateWindowEx(0, "SCROLLBAR", NULL,
		WS_CHILD | WS_VISIBLE | SBS_RIGHTALIGN | SBS_VERT, 0, SCROLLBAR_TOP,
		rect.right, rect.bottom-SCROLLBAR_TOP, hToolWindow, NULL, hThisInstance, NULL);           
    
    /* modify tab containers' & scrollbar's window procedure address */
    oldDisplayTabContainerProc=(WNDPROC)SetWindowLong(hToolWindowDisplayTabContainer,GWL_WNDPROC,(long)&ToolWindowDisplayTabContainerProcedure);
    oldQueryTabContainerProc=(WNDPROC)SetWindowLong(hToolWindowQueryTabContainer,GWL_WNDPROC,(long)&ToolWindowQueryTabContainerProcedure);
    oldImageTabContainerProc=(WNDPROC)SetWindowLong(hToolWindowImageTabContainer,GWL_WNDPROC,(long)&ToolWindowImageTabContainerProcedure);
    oldScrollBarContainerProc=(WNDPROC)SetWindowLong(hToolWindowScrollBar,GWL_WNDPROC,(long)&ToolWindowScrollBarProcedure);
    
   
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
            char tempBandValue[4] = "0"; // temporary storage for the band value
            imageBandValues[i] = CreateWindowEx(0, szStaticControl, tempBandValue, WS_CHILD | WS_VISIBLE, 5, 15 + (20 * (i-1)),
    			50, 18, queryValueContainer, NULL, hThisInstance, NULL);
        } 			
	}
	
	/* display cursor position under query tab */
	CreateWindowEx(0, szStaticControl, "X", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW,
                 20, 60 + (20 * bands), 100, 18, hToolWindowQueryTabContainer, NULL, hThisInstance, NULL);
	CreateWindowEx(0, szStaticControl, "Y", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW,
                 20, 60 + (20 * (bands+1)), 100, 18, hToolWindowQueryTabContainer, NULL, hThisInstance, NULL);
    cursorXPos = CreateWindowEx(0, szStaticControl, "-", WS_CHILD | WS_VISIBLE,
                 143, 60 + (20 * bands), 50, 18, hToolWindowQueryTabContainer, NULL, hThisInstance, NULL);			
    cursorYPos = CreateWindowEx(0, szStaticControl, "-", WS_CHILD | WS_VISIBLE,
                 143, 60 + (20 * (bands+1)), 50, 18, hToolWindowQueryTabContainer, NULL, hThisInstance, NULL);			
	
	/* Default radio button selection */
	if (bands == 1) {
    	SendMessage(redRadiobuttons[1],BM_SETCHECK,BST_CHECKED,0);
    	SendMessage(greenRadiobuttons[1],BM_SETCHECK,BST_CHECKED,0);
    	SendMessage(blueRadiobuttons[1],BM_SETCHECK,BST_CHECKED,0);
    } else if (bands == 2) {
        SendMessage(redRadiobuttons[1],BM_SETCHECK,BST_CHECKED,0);
    	SendMessage(greenRadiobuttons[2],BM_SETCHECK,BST_CHECKED,0);
    	SendMessage(blueRadiobuttons[2],BM_SETCHECK,BST_CHECKED,0);
    } else {
        SendMessage(redRadiobuttons[1],BM_SETCHECK,BST_CHECKED,0);
    	SendMessage(greenRadiobuttons[2],BM_SETCHECK,BST_CHECKED,0);
    	SendMessage(blueRadiobuttons[3],BM_SETCHECK,BST_CHECKED,0);
    }
	
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
    
    /* setup tool window scrollbar */
    updateToolWindowScrollbar();
    
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
                if (((DRAWITEMSTRUCT*)lParam)->hwndItem==hToolWindowDisplayTabHeading)
                    drawStatic((DRAWITEMSTRUCT*)lParam,hBoldFont);                
                else
                    drawStatic((DRAWITEMSTRUCT*)lParam,hNormalFont);
            break; 
        case WM_COMMAND:
            //if(hupdate==(HWND)lParam)
            {

                // find out which bands are selected
				int r, g, b;
				for (int i=0; i<bands; i++)
   				{
					LRESULT state = SendMessageA(redRadiobuttons[i], BM_GETCHECK, 0, 0);
					if(state==BST_CHECKED)
						r = i;
					
					state = SendMessageA(greenRadiobuttons[i], BM_GETCHECK, 0, 0);
					if(state==BST_CHECKED)
						g = i;
					
					state = SendMessageA(blueRadiobuttons[i], BM_GETCHECK, 0, 0);
					if(state==BST_CHECKED)
						b = i;
				}

	    		// Temporary message
	    		/*const char *butNum = "selected bands are: "; 
				butNum = catcstrings( (char*) butNum, (char*) inttocstring(r) );
				butNum = catcstrings( (char*) butNum, (char*) ", " );
				butNum = catcstrings( (char*) butNum, (char*) inttocstring(g) );
				butNum = catcstrings( (char*) butNum, (char*) ", " );
				butNum = catcstrings( (char*) butNum, (char*) inttocstring(b) );
				MessageBox( hwnd, (LPSTR) butNum,
                    (LPSTR) szOverviewWindowClassName,
        			MB_ICONINFORMATION | MB_OK );*/
        		// #define DEBUG_IMAGE_HANDLER to see these values printed in console window.
				
				// !! Insert band numbers (bands start at 1, not 0) here. - Rowan
				// 0 now equals none - Damian
            	if (image_handler) image_handler->set_bands(r,g,b);
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
                if (((DRAWITEMSTRUCT*)lParam)->hwndItem==hToolWindowQueryTabHeading)
                    drawStatic((DRAWITEMSTRUCT*)lParam,hBoldFont);                
                else
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
                if (((DRAWITEMSTRUCT*)lParam)->hwndItem==hToolWindowImageTabHeading)
                    drawStatic((DRAWITEMSTRUCT*)lParam,hBoldFont);                
                else
                    drawStatic((DRAWITEMSTRUCT*)lParam,hNormalFont);
            break; 
                        
        default:
            break;
    }        
    return CallWindowProc(oldImageTabContainerProc,hwnd,message,wParam,lParam);
}

/* show the correct tab container window for the selected tab */
void showToolWindowTabContainer(int selectedTabId)
{
    /* hide previously visible tab container window */
    if (hToolWindowCurrentTabContainer!=NULL)
    {
        scrollToolWindowToTop();
        ShowWindow(hToolWindowCurrentTabContainer,SW_HIDE);
    }

    /* update the handle to the current tab container window */        
    switch(selectedTabId)
    {
        case DISPLAY_TAB_ID:
                hToolWindowCurrentTabContainer=hToolWindowDisplayTabContainer;
                break;
        case QUERY_TAB_ID:
                hToolWindowCurrentTabContainer=hToolWindowQueryTabContainer;
                break;
        case IMAGE_TAB_ID:
                hToolWindowCurrentTabContainer=hToolWindowImageTabContainer;                
                break;
    }       

    /* show new tab container window */
    ShowWindow(hToolWindowCurrentTabContainer,SW_SHOW);
    
    /* update scroll bar settings for the current tab */
    updateToolWindowScrollbar();
}    

/* scrolls a window back to its orginal state */
void scrollToolWindowToTop()
{
    SCROLLINFO info;
    RECT rect;
    int amount;
    
    /* get scroll bar settings */
    info.cbSize=sizeof(SCROLLINFO);
    info.fMask=SIF_POS;
    GetScrollInfo(hToolWindowScrollBar,SB_VERT,&info);
    
    GetClientRect(hToolWindowCurrentTabContainer,&rect);
    amount=info.nPos;
    ScrollWindowEx(hToolWindowCurrentTabContainer,0,amount,NULL,NULL,NULL,&rect,SW_ERASE|SW_INVALIDATE|SW_SCROLLCHILDREN);    
}    

/* change the tool window's scrollbar settings based on the currently visible tab container */
void updateToolWindowScrollbar()
{
    RECT rcontainer,rscrollbar;
    SCROLLINFO info;    
   
    /* get position of current container & scrollbar */
    GetWindowRect(hToolWindowCurrentTabContainer,&rcontainer);
    GetWindowRect(hToolWindowScrollBar,&rscrollbar);

    /* set height of visible scroll area */   
    info.nPage=rscrollbar.bottom-rscrollbar.top;
    
    /* set scroll range */
    info.nMin=0;
    info.nMax=rcontainer.bottom-rcontainer.top;
    
    /* set scroll position */
    info.nPos=0;
    
    Console::write("updateToolWindowScrollbar() ");
    Console::write("\ninfo.nMin=");
    Console::write(info.nMin);
    Console::write(" info.nMax=");
    Console::write(info.nMax);
    Console::write("\ninfo.nPage=");
    Console::write(info.nPage);
    Console::write("\n");
       
    /* set scrollbar info */
    info.cbSize=sizeof(SCROLLINFO);
    info.fMask=SIF_ALL;
    SetScrollInfo(hToolWindowScrollBar,SB_VERT,&info,true);  
    
}    


void scrollToolWindow(int msg)
{
    SCROLLINFO info;
    RECT rect;
    int prevPos;
    int amount;
    
    /* get scroll bar settings */
    info.cbSize=sizeof(SCROLLINFO);
    info.fMask=SIF_POS|SIF_RANGE|SIF_TRACKPOS|SIF_PAGE;
    GetScrollInfo(hToolWindowScrollBar,SB_VERT,&info);
    prevPos=info.nPos;

    /* calculate new scroll bar position */
    switch (LOWORD(msg))
    {
        case SB_LINEUP:
            info.nPos--;
            break;
            
        case SB_LINEDOWN:
            info.nPos++;
            break;
            
        case SB_PAGEUP:
            info.nPos+=info.nPage;
            break;
            
        case SB_PAGEDOWN:
            info.nPos-=info.nPage;
            break;
            
        case SB_THUMBTRACK:
            info.nPos=info.nTrackPos;
            break;
        default:
            return;
    }
    if (info.nPos<info.nMin)
        info.nPos=info.nMin;
    if (info.nPos>info.nMax)
        info.nPos=info.nMax;
    
    // update scroll bar settings    
    info.fMask=SIF_POS;
    SetScrollInfo(hToolWindowScrollBar,SB_VERT,&info,true);
    
    // scroll window
    GetClientRect(hToolWindowCurrentTabContainer,&rect);
    amount=prevPos-info.nPos;
    ScrollWindowEx(hToolWindowCurrentTabContainer,0,amount,NULL,NULL,NULL,&rect,SW_ERASE|SW_INVALIDATE|SW_SCROLLCHILDREN);

    //if (amount<0)
    //    rect.bottom=rect.top+amount;
    //else
     //   rect.top=rect.bottom-amount;    

    //InvalidateRect(hToolWindowCurrentTabContainer,&rect,true);
    UpdateWindow(hToolWindowCurrentTabContainer);
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
                case TCN_SELCHANGE:
                   /* display the tab container window that is associated with the selected tab */                                   
                   showToolWindowTabContainer(TabCtrl_GetCurSel(hToolWindowTabControl));
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
            snapInsideWindowByMoving(hDesktop,(RECT*)lParam);  
            
            /* if new position is near main window, snap to it */    
            snapWindowByMoving(hOverviewWindow,(RECT*)lParam);
            snapWindowByMoving(hImageWindow,(RECT*)lParam);
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

/* This function is called by the Windows function DispatchMessage( ) */
/* All messages/events related to one of the display windows are sent to this procedure */
LRESULT CALLBACK ToolWindowScrollBarProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    /* handle the messages */    
    switch (message)
    {
        case WM_VSCROLL:          
            scrollToolWindow(wParam);
            return 0;
                        
        default:
            break;
    }        
    return CallWindowProc(oldTabControlProc,hwnd,message,wParam,lParam);
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
    //SetWindowPos(hOverviewWindow,hToolWindow,0,0,0,0,SWP_NOMOVE+SWP_NOSIZE);        
    SetWindowPos(hImageWindow,hToolWindow,0,0,0,0,SWP_NOMOVE+SWP_NOSIZE+SWP_NOACTIVATE+SWP_NOSENDCHANGING);        
    SetWindowPos(hImageWindow,hOverviewWindow,0,0,0,0,SWP_NOMOVE+SWP_NOSIZE+SWP_NOACTIVATE+SWP_NOSENDCHANGING);        


}    

/* main functions */
/* ------------------------------------------------------------------------------------------------------------------------ */
void loadFile()
{
    OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW - which note?
    ofn.hwndOwner = hOverviewWindow;
    ofn.lpstrFilter =  "All Supported Images\0*.ecw;*.jpg;*.tif;*.j2k;*.jp2\0ERMapper Compressed Wavelets (*.ecw)\0*.ecw\0JPEG (*.jpg)\0*.jpg\0JPEG 2000 (*.j2k,*.jp2)\0*.j2k;*.jp2\0TIFF / GeoTIFF (*.tif)\0*.tif\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "txt";

    if(GetOpenFileName(&ofn))
    {
        // Do something usefull with the filename stored in szFileName 

		// Clean up any previous instance
		closeFile();

	    image_handler = new ImageHandler::ImageHandler(hOverviewWindowDisplay, hImageWindowDisplay, ofn.lpstrFile);
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
                    // update image window settings
                    filename=(char*)image_handler->get_image_properties()->getFileName();
                    updateImageWindowTitle();              
                    updateImageScrollbar();      

                    // re-create tool window
                    setupToolWindow();
                    
                    // show tool & image windows
                    ShowWindow(hToolWindow,SW_SHOW);
                    ShowWindow(hImageWindow,SW_SHOW);    
                    orderWindows();                    

                    // update opengl displays
                    //InvalidateRect(hOverviewWindowDisplay,0,true);
                    //UpdateWindow(hOverviewWindowDisplay);
                    //InvalidateRect(hImageWindowDisplay,0,true);
                    //UpdateWindow(hImageWindowDisplay);
                    RedrawWindow(hOverviewWindowDisplay,NULL,NULL,RDW_INTERNALPAINT);
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
        ShowWindow(hImageWindow,SW_HIDE);
        //DestroyWindow(hImageWindow);
        //hImageWindow=NULL;
    }

    /* disable menu items */
    EnableMenuItem(hMainMenu,IDM_IMAGEWINDOW,true);
    EnableMenuItem(hMainMenu,IDM_TOOLSWINDOW,true);
    EnableMenuItem(hMainMenu,IDM_FILECLOSE,true);    
    
    InvalidateRect(hOverviewWindowDisplay,0,true);  /* repaint main window */		
    UpdateWindow(hOverviewWindowDisplay);
}
