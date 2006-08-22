#include <Windows.h>
#include "main.h"
#include "Window.h"
#include "ScrollBox.h"
#include "Console.h"

/* create scrollable window as a child of another window, with rect set to top-left & bottom-right corners of parent window */
int ScrollBox::Create(RECT *rect,HWND parentHandle)
{
    /* Create main window */
    if (!CreateWin(0, "parbat3d scrollable win class", "Parbat3D",
		WS_CHILD | WS_VSCROLL | WS_CLIPSIBLINGS, rect->top, rect->left, rect->right, rect->bottom,
		parentHandle, NULL))
        return false;
    prevProc=SetWindowProcedure(&ScrollBox::WindowProcedure);
    Show();
    return true;
}

void ScrollBox::UpdateScrollBar()
{
    RECT rwscrollbox,rcscrollbox;
    SCROLLINFO info;    
   
    /* get position of current container & scrollbar */
    GetWindowRect(GetHandle(),&rwscrollbox);
    GetClientRect(GetHandle(),&rcscrollbox);    

    /* set height of visible scroll area */   
    info.nPage=100;
    
    /* set scroll range */
    info.nMin=0;
    info.nMax=100;
    
    /* set scroll position */
    info.nPos=0;
    
    Console::write("ScrollBox::UpdateScrollBar()\n");
    Console::write("  wrect= ");
    Console::writeRECT((uint*)&rwscrollbox);
    Console::write("\n");
    Console::write("  crect= ");
    Console::writeRECT((uint*)&rcscrollbox);
    Console::write("\n");    
       
    /* set scrollbar info */
    info.cbSize=sizeof(SCROLLINFO);
    info.fMask=SIF_ALL;
    SetScrollInfo(GetHandle(),SB_VERT,&info,true);     
}

/* handle events related to the main window */
LRESULT CALLBACK ScrollBox::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    
    ScrollBox* win=(ScrollBox*)Window::GetWindowObject(hwnd);
    switch (message)
    {
        case WM_SHOWWINDOW:
            win->UpdateScrollBar();
            break;
        case WM_VSCROLL:
            
            return 0;
    }
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}
