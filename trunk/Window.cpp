#include "Window.h"
#include "Console.h"
#include "resources.h"
#include "config.h"

WNDPROC Window::stPrevWindowProcedure=NULL;
HINSTANCE Window::hInstance=NULL;
HICON Window::hIcon=NULL;

Window::Window()
{
    hwindow=NULL;
    prevWindowProcedure=NULL; 
    hInstance=NULL;
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

void Window::SetWindowObject(HWND hwnd,Window* obj)
{
    SetWindowLong(hwnd,GWL_USERDATA,(long)obj);
}

void Window::SetStaticFont(HWND hwnd,int font)
{
    SetWindowLong(hwnd,GWL_USERDATA,(long) font);
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

    font=(int)GetWindowObject(dis->hwndItem);
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
    SelectObject(dis->hDC,hfont);
    SetTextColor(dis->hDC,0);                                                                   // set text colour to black
//    GetTextExtentPoint32(dis->hDC,str,len,&size);                                               // get size of string
//    SelectObject(dis->hDC,hTabPen);                                                             // set border
//    SelectObject(dis->hDC,hTabBrush);                                                           // set background fill
//    Rectangle(dis->hDC,dis->rcItem.left,dis->rcItem.top,dis->rcItem.right,dis->rcItem.bottom);  // display background rectangle
    TextOut(dis->hDC,dis->rcItem.left,dis->rcItem.top,(char*)str,len);                          // display text
}

LRESULT Window::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Window* win=Window::GetWindowObject(hwnd);
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
            case WM_DESTROY: 
                DeleteObject(win->hNormalFont);
                DeleteObject(win->hBoldFont);                
                win->hwindow=NULL;
                return 0;
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


