#include "Window.h"
#include "Console.h"

WNDPROC Window::stPrevWindowProcedure=NULL;
HINSTANCE Window::hInstance=NULL;

Window::Window()
{
    hwindow=NULL;
    prevWindowProcedure=NULL; 
    hInstance=NULL;
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
        Console::write("Window::CreateWin CreateWindowEx failed first time\n");
        // attempt to register class (incase not already registered)
        WNDCLASSEX wincl;
        wincl.cbSize=sizeof(wincl);
        wincl.style = CS_DBLCLKS;
        wincl.lpfnWndProc=&WindowProcedure;        
        wincl.cbClsExtra = 0;
        wincl.cbWndExtra = 0;
        wincl.hInstance=hInstance;
        wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
        wincl.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH); //GetSysColor(COLOR_3DFACE);
        wincl.lpszMenuName = NULL;
        wincl.lpszClassName=lpClassName;
        wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
        
        if (RegisterClassEx(&wincl)==0)
        {
            ErrorExit("RegisterClassEx");
            Console::write("Window::CreateWin RegisterClassEx failed\n");            
            return false;  //(may fail if already registered)
        }
        
        // re-try creating window
        hwindow=CreateWindowEx(dwExStyle,lpClassName,lpWindowName,dwStyle,x,y,nWidth,nHeight,hWndParent,hMenu,hInstance,this);
        if (hwindow==NULL)    
        {
            Console::write("Window::CreateWin CreateWindowEx failed second time time\n");
            ErrorExit("CreateWindowEx");
            return false;
        }
        
    }
    Console::write("Window::CreateWin CreateWindowEx ok\n");    

    // add "this" object pointer to hwindow's internal structure
    SetWindowLong(hwindow,GWL_USERDATA,(int)this);
    if (GetWindowObject(hwindow)!=this)
        Console::write("Window::CreateWin this object not set properly\n");

    // set window procedure
    prevWindowProcedure=NULL;
    stPrevWindowProcedure=NULL;
    if (GetWindowLongPtr(hwindow,GWLP_WNDPROC)!=(LONG)&Window::WindowProcedure)
    {
        Console::write("Window::CreateWin: window proc incorrect, subclassing\n");
        prevWindowProcedure=(WNDPROC)SetWindowLongPtr(hwindow,GWLP_WNDPROC,(LONG)&Window::WindowProcedure);
        stPrevWindowProcedure=prevWindowProcedure;
    }
        
    Console::write("Window::CreateWin: ok\n");
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
    //todo: check for err
}

Window* Window::GetWindowObject(HWND hwnd)
{
    Window* win=(Window*)GetWindowLong(hwnd,GWL_USERDATA);
    if (win==NULL)
    {
        Console::write("Window::GetWindowObject win=NULL\n");
    }
   // Console::write("Window::GetWindowObject win=\n");
//    Console::write((int)win);
//    Console::write("\n");
    return win;
}


LRESULT Window::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Window* win=Window::GetWindowObject(hwnd);
    if (win!=NULL)
    {   
        switch (message)
        {
            case WM_CREATE:
                /*win=(Window*)((CREATESTRUCT*)lParam)->lpCreateParams;
                Window::SetWindowObject(hwnd,win);*/
                break;
            case WM_CLOSE:
                DestroyWindow(hwnd);
                return 0;
            case WM_DESTROY: 
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


