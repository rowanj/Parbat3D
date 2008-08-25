#include "PchApp.h"

#include "Window.h"

#include "config.h"

#include "resources.h"
#include "main.h"
#include "console.h"

#include <shellapi.h>

/* define static variables */
WNDPROC Window::stPrevWindowProcedure=NULL;
HINSTANCE Window::hInstance=NULL;
HICON Window::hIcon=NULL;
int Window::keyboardShortcutKeys[NUMBER_OF_SHORTCUTS];

/* setup object variables */
Window::Window()
{
    hwindow=NULL;
    prevWindowProcedure=NULL; 
    hInstance=NULL;
	zValue=0;
	threadId=0;
}

/* setup static variables */
void Window::Init(HINSTANCE hInst)
{
    hInstance=hInst;
    hIcon=LoadIcon(hInst,MAKEINTRESOURCE(ID_MAIN_ICON));
}

// Display an error mesage as a message box, then exit
// Copied from Microsoft's MSDN documentation
void ErrorExit(LPTSTR lpszFunction) 
{ 
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;

    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)lpszFunction)+40)*sizeof(TCHAR)); 
    wsprintf((LPTSTR)lpDisplayBuf, 
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw); 
}


int Window::CreateWin(DWORD dwExStyle,LPCTSTR lpClassName,LPCTSTR lpWindowName,DWORD dwStyle,int x,int y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu)
{
    
    // if window already created, destroy it & re-create it
    if (hwindow!=NULL)
        DestroyWindow(hwindow);
        
    hwindow=CreateWindowEx(dwExStyle,lpClassName,lpWindowName,dwStyle,x,y,nWidth,nHeight,hWndParent,hMenu,hInstance,this);
    if (hwindow==NULL)
    {
        // attempt to register class (incase not already registered)
        WNDCLASSEX wincl;
        HBRUSH hbrush=CreateSolidBrush(GetSysColor(COLOR_3DFACE));
        wincl.cbSize=sizeof(wincl);
        wincl.style = CS_DBLCLKS;
        wincl.lpfnWndProc=&WindowProcedure;        
        wincl.cbClsExtra = 0;
        wincl.cbWndExtra = 0;
        wincl.hInstance=hInstance;
        wincl.hIcon = hIcon; //LoadIcon(NULL, IDI_APPLICATION);
        wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
        wincl.hbrBackground = (HBRUSH) hbrush; //GetStockObject(LTGRAY_BRUSH);
        wincl.lpszMenuName = NULL;
        wincl.lpszClassName=lpClassName;
        wincl.hIconSm = hIcon; //LoadIcon(NULL, IDI_APPLICATION);
        
        if (RegisterClassEx(&wincl)==0)
        {
            ErrorExit("RegisterClassEx");
            Console::write("Window::CreateWin RegisterClassEx failed\n");            
            return false;
        }

        //DeleteObject(hbrush);
        
        // re-try creating window
        hwindow=CreateWindowEx(dwExStyle,lpClassName,lpWindowName,dwStyle,x,y,nWidth,nHeight,hWndParent,hMenu,hInstance,this);
        if (hwindow==NULL)    
        {
            Console::write("Window::CreateWin CreateWindowEx failed second time time\n");
            ErrorExit("CreateWindowEx");
            return false;
        }
        
    }

    // add "this" object pointer to hwindow's internal structure
    SetWindowObject(hwindow,this);
    assert (GetWindowObject(hwindow)==this);

    // create fonts
    HDC hdc=GetDC(GetHandle());
    hBackgroundBrush=(HBRUSH)GetClassLong(hwindow,GCL_HBRBACKGROUND);
    DeleteObject(hdc);

    // set window procedure
    prevWindowProcedure=NULL;
    stPrevWindowProcedure=NULL;
    if (GetWindowLongPtr(hwindow,GWLP_WNDPROC) != reinterpret_cast<uintptr_t>(&Window::WindowProcedure))
    {
        prevWindowProcedure=(WNDPROC)SetWindowLongPtr(hwindow,GWLP_WNDPROC,reinterpret_cast<uintptr_t>(&Window::WindowProcedure));
        stPrevWindowProcedure=prevWindowProcedure;
    }
    
    // record the id of the thread that ran this function
    threadId=::GetCurrentThreadId();
    
    return true;
}

// creates a simple window for testing
// usually over-written by subclasses
int Window::Create()
{
    int r=CreateWin(0,"test class name","Parbat test win",WS_OVERLAPPED+WS_CAPTION+WS_SYSMENU+WS_MINIMIZEBOX, 10,10,200,200,NULL,NULL);
    if (r)
    	ShowWindow(hwindow,SW_SHOW);    
    return r;
}


/* Set the function that will a handle a window's messages/events.
   Returns the address of the previous function.
*/
WNDPROC Window::SetWindowProcedure(WNDPROC newproc)
{
  return (WNDPROC)SetWindowLongPtr(hwindow,GWLP_WNDPROC,reinterpret_cast<uintptr_t>(newproc));
}

WNDPROC Window::SetWindowProcedure(HWND hwnd,WNDPROC newproc)
{
  return (WNDPROC)SetWindowLongPtr(hwnd,GWLP_WNDPROC,reinterpret_cast<uintptr_t>(newproc));
}

// associate a Window object with a window
void Window::SetWindowObject(HWND hwnd,Window* obj)
{
	SetProp(hwnd,"Parbat3D_Window",obj);
}

// get the Window object associated with a window
Window* Window::GetWindowObject(HWND hwnd)
{
    Window* win=(Window*)GetProp(hwnd,"Parbat3D_Window");
    return win;
}

// now same as SetFont, left here to prevent breaking old code that uses it
void Window::SetStaticFont(HWND hwnd,int font)
{
    SetFont(hwnd,font);
}

// set the default font of a window/control
void Window::SetFont(HWND hwnd,int font)
{
	HDC hdc;
    HFONT hfont;
    
    hdc=GetDC(hwnd);
    switch(font)
    {
        case STATIC_FONT_BOLD:
            hfont=CreateFont(-MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72),0,0,0,600,false,false,false,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,"Tahoma"); 
            break;
        case STATIC_FONT_HEADING:
            hfont=CreateFont(-MulDiv(9, GetDeviceCaps(hdc, LOGPIXELSY), 72),0,0,0,600,false,false,false,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,"Tahoma");
            break;
        default:
            hfont=CreateFont(-MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72),0,0,0,400,false,false,false,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,"Tahoma");
    }
	SendMessage(hwnd,WM_SETFONT,(WPARAM)hfont,0);
	DeleteObject(hdc);
}


// draw static text control in default font
void Window::drawStatic(DRAWITEMSTRUCT *dis)
{   
    char str[255];
    int len,x,y;
    SIZE size;  
    int font;

    GetWindowText(dis->hwndItem,(LPSTR)str,(int)255);
    len=strlen(str);

    SetTextColor(dis->hDC,0);                                                                   // set text colour to black
    TextOut(dis->hDC,dis->rcItem.left,dis->rcItem.top,(char*)str,len);                          // display text
}

// handle global shortcut key events
// this is called by WindowProcedure
void Window::OnKeyDown(int virtualKey)
{
	const char *helpPath; // Used for accessing the help folder
	
	int keyMatch = -1;
	
	for (int i=0; i<Window::NUMBER_OF_SHORTCUTS; i++) {
        if (virtualKey == Window::keyboardShortcutKeys[i]) {
            keyMatch = i;
            break;
        }
    }
    
    switch (keyMatch) {
        // open/close windows --------------------------------------------------
        
        // toggle Image window
        case Window::KEY_IMAGE_WINDOW:
			if (overviewWindow.toggleMenuItemTick(overviewWindow.hMainMenu,IDM_IMAGEWINDOW))
		        ShowWindow(imageWindow.GetHandle(),SW_SHOW);
		    else
		        ShowWindow(imageWindow.GetHandle(),SW_HIDE);
		    break;
		
		// toggle tool window
		case Window::KEY_TOOLS_WINDOW:
			if (overviewWindow.toggleMenuItemTick(overviewWindow.hMainMenu,IDM_TOOLSWINDOW))
		        toolWindow.Show();
		    else
		        toolWindow.Hide();
		    break;
		
		// toggle ROI window
		case Window::KEY_ROI_WINDOW:
			if (overviewWindow.toggleMenuItemTick(overviewWindow.hMainMenu,IDM_ROIWINDOW))
		    	roiWindow.Show();
		    else
		        roiWindow.Hide();
		    break;
		
		// toggle Preferences window
		case Window::KEY_PREFERENCES_WINDOW:
			if (overviewWindow.toggleMenuItemTick(overviewWindow.hMainMenu,IDM_PREFSWINDOW))
		        prefsWindow.Show();
		    else
		        prefsWindow.Hide();
		    break;
		
		// toggle Contrast window
        case Window::KEY_CONTRAST_WINDOW:
            if (overviewWindow.toggleMenuItemTick(overviewWindow.hMainMenu,IDM_CONTSWINDOW))
                contrastWindow.Show();
            else
                contrastWindow.Hide();
                contrastAdvWindow.Hide();
            break;
        
        
		// help ----------------------------------------------------------------
        case Window::KEY_HELP:
            // get path to help folder
            helpPath = catcstrings( (char*) modulePath, (char*) "\\help\\index.html");
            
            // launch default browser
            ShellExecute(NULL, "open", helpPath, NULL, "help", SW_SHOWNORMAL);
            break;
        
        
        // Regions of Interest -------------------------------------------------
        
        // create point entity
        case Window::KEY_ROI_POINT:
            if (!regionsSet->editing())
                roiWindow.newEntity(&roiWindow, ROI_POINT);
            break;
        
        // create rect entity
        case Window::KEY_ROI_RECT:
            if (!regionsSet->editing())
                roiWindow.newEntity(&roiWindow, ROI_RECT);
            break;
        
        // create poly entity
        case Window::KEY_ROI_POLY:
            if (!regionsSet->editing())
                roiWindow.newEntity(&roiWindow, ROI_POLY);
            break;
        
        
        // Misc ----------------------------------------------------------------
        
        // set zoom level (not implemented)
        case Window::KEY_SET_ZOOM_LEVEL:
//            MessageBox(NULL, "Set zoom", "Parbat3D", MB_YESNO|MB_ICONQUESTION);
            break;
    }
}


/* General window event/message handler. 
   This procedure receives messages for all windows created by CreateWin(..) (by default)
   Any message handling that needs to occur for all windows is handled here (eg. global keyboard shortcuts)
*/
LRESULT Window::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    unsigned int flags;
    Window* win=Window::GetWindowObject(hwnd);
    WINDOWPOS *windowPos;
    
    char buffer[256];
    if (win!=NULL)
    {   
        switch (message)
        {
            case WM_CLOSE:
                DestroyWindow(hwnd);
                return 0;
                
            case WM_DRAWITEM:
				// draw an owner drawn static text control
                if (((DRAWITEMSTRUCT*)lParam)->CtlType==ODT_STATIC)
                {
                    win->drawStatic((DRAWITEMSTRUCT*)lParam);
                }
                return 0;

			// handle moving other snapped windows when this one is dragged, if required
            case WM_NCLBUTTONDOWN:
                switch(wParam)
                {
                    case HTCAPTION:
                        stickyWindowManager.BeginMoving(win);
                        break;
                }
                break;
            case WM_MOVING:
                stickyWindowManager.OnMoving(win,(RECT*)lParam);
                return TRUE;

			// handle keyboard shortcuts				
			case WM_KEYDOWN:
				win->OnKeyDown(wParam);
				break;
			
			// free any resources used by window
            case WM_DESTROY: 
                //DeleteObject(win->hNormalFont);
                //DeleteObject(win->hBoldFont);      
				//DeleteObject(win->hHeadingFont);
                win->hwindow=NULL;	// set window handle to invalid
                break;
        }
    
        // let someone else handle the stuff we haven't
        if (win->prevWindowProcedure!=NULL)
        {
            return CallWindowProc(win->prevWindowProcedure,hwnd,message,wParam,lParam);
        }
    }
    else
    {
        // let someone else handle the messages while the window is still being created
        if (stPrevWindowProcedure!=NULL)
            return CallWindowProc(stPrevWindowProcedure,hwnd,message,wParam,lParam);        
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}


/* create a tooltip over a window control */
void Window::CreateTooltip (HWND hwnd,char *text)
{
                 // struct specifying control classes to register
    INITCOMMONCONTROLSEX iccex; 
    HWND hwndTT;                 // handle to the ToolTip control
          // struct specifying info about tool in ToolTip control
    TOOLINFO ti;
    unsigned int uid = 0;       // for ti initialization
    RECT rect;                  // for client area coordinates

    /* INITIALIZE COMMON CONTROLS */
    iccex.dwICC = ICC_WIN95_CLASSES;
    iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    InitCommonControlsEx(&iccex);

    /* CREATE A TOOLTIP WINDOW */
    hwndTT = CreateWindowEx(WS_EX_TOPMOST,
        TOOLTIPS_CLASS,
        NULL,
        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,		
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        hwnd,
        NULL,
        Window::GetAppInstance(),
        NULL
        );

    SetWindowPos(hwndTT,
        HWND_TOPMOST,
        0,
        0,
        0,
        0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    /* GET COORDINATES OF THE MAIN CLIENT AREA */
    GetClientRect (hwnd, &rect);

    /* INITIALIZE MEMBERS OF THE TOOLINFO STRUCTURE */
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS;
    ti.hwnd = hwnd;
    ti.hinst = Window::GetAppInstance();
    ti.uId = uid;
    ti.lpszText = text;
        // ToolTip control will cover the whole window
    ti.rect.left = rect.left;    
    ti.rect.top = rect.top;
    ti.rect.right = rect.right;
    ti.rect.bottom = rect.bottom;

    /* SEND AN ADDTOOL MESSAGE TO THE TOOLTIP CONTROL WINDOW */
    SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);	

} 


void Window::setDefaultKeyboardShortcuts () {
    Window::keyboardShortcutKeys[Window::KEY_IMAGE_WINDOW] = 'I';
    Window::keyboardShortcutKeys[Window::KEY_TOOLS_WINDOW] = 'T';
    Window::keyboardShortcutKeys[Window::KEY_ROI_WINDOW] = 'R';
    Window::keyboardShortcutKeys[Window::KEY_PREFERENCES_WINDOW] = 'P';
    Window::keyboardShortcutKeys[Window::KEY_CONTRAST_WINDOW] = 'C';
    
    Window::keyboardShortcutKeys[Window::KEY_HELP] = VK_F1;
    
    Window::keyboardShortcutKeys[Window::KEY_ROI_POINT] = 'D';
    Window::keyboardShortcutKeys[Window::KEY_ROI_RECT] = 'S';
    Window::keyboardShortcutKeys[Window::KEY_ROI_POLY] = 'A';
    
    //Window::keyboardShortcutKeys[Window::KEY_SET_ZOOM_LEVEL] = 'Z'; //not implemented
}


int Window::getKeyboardShortcut (int key) {
    return Window::keyboardShortcutKeys[key];
}

void Window::setKeyboardShortcut (int key, int value) {
    Window::keyboardShortcutKeys[key] = value;
}


