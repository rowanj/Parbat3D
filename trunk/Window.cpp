#include "Window.h"
#include "Console.h"
#include "resources.h"
#include "main.h"
#include "config.h"
#include <commctrl.h>


WNDPROC Window::stPrevWindowProcedure=NULL;
HINSTANCE Window::hInstance=NULL;
HICON Window::hIcon=NULL;
int Window::keyboardShortcutKeys[NUMBER_OF_SHORTCUTS];


Window::Window()
{
    hwindow=NULL;
    prevWindowProcedure=NULL; 
    hInstance=NULL;
	zValue=0;
	threadId=0;
}

void Window::Init(HINSTANCE hInst)
{
    hInstance=hInst;
    hIcon=LoadIcon(hInst,MAKEINTRESOURCE(ID_MAIN_ICON));
}

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
            return false;  //(may fail if already registered)
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
    SetWindowLong(hwindow,GWL_USERDATA,(int)this);
    if (GetWindowObject(hwindow)!=this)
        Console::write("Window::CreateWin this object not set properly\n");

    // create fonts
    HDC hdc=GetDC(GetHandle());
    hNormalFont=CreateFont(-MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72),0,0,0,400,false,false,false,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,"Tahoma"); //"MS Sans Serif" //Tahoma    
    hBoldFont=CreateFont(-MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72),0,0,0,600,false,false,false,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,"Tahoma"); //"MS Sans Serif" //Tahoma
    hHeadingFont=CreateFont(-MulDiv(9, GetDeviceCaps(hdc, LOGPIXELSY), 72),0,0,0,600,false,false,false,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,"Tahoma"); //"MS Sans Serif" //Tahoma   
    hBackgroundBrush=(HBRUSH)GetClassLong(hwindow,GCL_HBRBACKGROUND);
    DeleteObject(hdc);

    // set window procedure
    prevWindowProcedure=NULL;
    stPrevWindowProcedure=NULL;
    if (GetWindowLongPtr(hwindow,GWLP_WNDPROC)!=(LONG)&Window::WindowProcedure)
    {
        prevWindowProcedure=(WNDPROC)SetWindowLongPtr(hwindow,GWLP_WNDPROC,(LONG)&Window::WindowProcedure);
        stPrevWindowProcedure=prevWindowProcedure;
    }
    
    threadId=::GetCurrentThreadId();
    
    return true;
}

int Window::Create()
{
    int r=CreateWin(0,"test class name","Parbat test win",WS_OVERLAPPED+WS_CAPTION+WS_SYSMENU+WS_MINIMIZEBOX, 10,10,200,200,NULL,NULL);
    if (r)
    	ShowWindow(hwindow,SW_SHOW);    
    return r;
}



WNDPROC Window::SetWindowProcedure(WNDPROC newproc)
{
    return (WNDPROC)SetWindowLongPtr(hwindow,GWLP_WNDPROC,(LONG)newproc);
}


WNDPROC Window::SetWindowProcedure(HWND hwnd,WNDPROC newproc)
{
	return (WNDPROC)SetWindowLongPtr(hwnd,GWLP_WNDPROC,(LONG)newproc);
}

void Window::SetWindowObject(HWND hwnd,Window* obj)
{
    SetWindowLong(hwnd,GWL_USERDATA,(long)obj);
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

Window* Window::GetWindowObject(HWND hwnd)
{
    Window* win=(Window*)GetWindowLong(hwnd,GWL_USERDATA);
    return win;
}

// draw static text control in choosen font
void Window::drawStatic(DRAWITEMSTRUCT *dis)
{   
    char str[255];
    int len,x,y;
    SIZE size;  
    int font;

    GetWindowText(dis->hwndItem,(LPSTR)str,(int)255);
    len=strlen(str);

   /* font=(int)GetWindowObject(dis->hwndItem);
    HFONT hfont;
    switch(font)
    {
        case STATIC_FONT_BOLD:
            hfont=hBoldFont;
            break;
        case STATIC_FONT_HEADING:
            hfont=hHeadingFont;
            break;
        default:
            hfont=hNormalFont;
    }
    SelectObject(dis->hDC,hfont);*/
    SetTextColor(dis->hDC,0);                                                                   // set text colour to black
    TextOut(dis->hDC,dis->rcItem.left,dis->rcItem.top,(char*)str,len);                          // display text
}

// handle global shortcut key events
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
            helpPath = catcstrings( (char*) modulePath, (char*) "\\help\\index.htm");
            
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
        
        // set zoom level
        case Window::KEY_SET_ZOOM_LEVEL:
//            MessageBox(NULL, "Set zoom", "Parbat3D", MB_YESNO|MB_ICONQUESTION);
            break;
    }
}


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
                if (((DRAWITEMSTRUCT*)lParam)->CtlType==ODT_STATIC)
                {
                    win->drawStatic((DRAWITEMSTRUCT*)lParam);
                }
                return 0;
				         
            case WM_NCLBUTTONDOWN:
                switch(wParam)
                {
                    case HTCAPTION:
                        stickyWindowManager.BeginMoving(win);
                        break;
                }
                break;

				
			case WM_KEYDOWN:
				win->OnKeyDown(wParam);
				break;
			
            case WM_MOVING:
                stickyWindowManager.OnMoving(win,(RECT*)lParam);
                return TRUE;
            case WM_DESTROY: 
                DeleteObject(win->hNormalFont);
                DeleteObject(win->hBoldFont);      
				DeleteObject(win->hHeadingFont);
                win->hwindow=NULL;
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
    
    Window::keyboardShortcutKeys[Window::KEY_SET_ZOOM_LEVEL] = 'Z';
}


int Window::getKeyboardShortcut (int key) {
    return Window::keyboardShortcutKeys[key];
}

void Window::setKeyboardShortcut (int key, int value) {
    Window::keyboardShortcutKeys[key] = value;
}


