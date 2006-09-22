#include <Windows.h>
#include "main.h"
#include "Window.h"
#include "ScrollBox.h"
#include "config.h"

/* create scrollable window as a child of another window, with rect set to top-left & bottom-right corners of parent window */
int ScrollBox::Create(HWND parentHandle,RECT *rect)
{
    /* Create main window */
    if (!CreateWin(WS_EX_CLIENTEDGE, "parbat3d scrollable win class", "Parbat3D",
		 WS_CHILD | WS_VSCROLL | WS_CLIPSIBLINGS, rect->top, rect->left, rect->right, rect->bottom,
		parentHandle, NULL))
        return false;

    prevProc=SetWindowProcedure(&ScrollBox::WindowProcedure);
    Show();
    return true;
}


BOOL CALLBACK ScrollBox::GetMaxScrollHeight(HWND hwnd, LPARAM lparam)
{
    RECT rect;
    int height;
    int cb;
    
    ScrollBox *sbox=(ScrollBox*)lparam;
    GetWindowRect(hwnd,&rect);
    cb=rect.bottom;
    
    GetWindowRect(sbox->GetHandle(),&rect);
    
    height=cb-rect.top;
    if (height > sbox->maxScrollHeight)
        sbox->maxScrollHeight=height;
    return true;
}

// re-calculate amount of content that needs to be scrolled & update scrollbars
void ScrollBox::UpdateScrollBar()
{
    RECT rcscrollbox;
    SCROLLINFO info;    

    /* get height of content within scroll box */
    maxScrollHeight=0;
    EnumChildWindows(GetHandle(),&GetMaxScrollHeight,(LPARAM)this);
     
    /* get position of current container & scrollbar */
    GetClientRect(GetHandle(),&rcscrollbox);    

    /* set scroll amount per unit of scroll position */   
    info.nPage=rcscrollbox.bottom;
   
    #if DEBUG_SCROLLING
    Console::write("ScrollBox::UpdateScrollBar() max scroll height: ");
    Console::write(maxScrollHeight);
    Console::write(" page size: ");
    Console::write(info.nPage);
    Console::write("\n");
    #endif
    
    /* set scroll range */
    info.nMin=0;
    info.nMax=maxScrollHeight;
    
    /* set scroll position */
    info.nPos=0;
    info.nTrackPos=0;
    pixelPosition=0;
           
    /* set scrollbar info */
    info.cbSize=sizeof(SCROLLINFO);
    info.fMask=SIF_ALL;
    SetScrollInfo(GetHandle(),SB_VERT,&info,true);     
}

/* scroll the window */
void ScrollBox::Scroll(int msg)
{
    SCROLLINFO info;
    RECT rect;
    int prevPos;
    int amount;
    
    /* get scroll bar settings */
    info.cbSize=sizeof(SCROLLINFO);
    info.fMask=SIF_POS|SIF_RANGE|SIF_TRACKPOS|SIF_PAGE;
    GetScrollInfo(GetHandle(),SB_VERT,&info);
    prevPos=info.nPos;

    /* calculate new scroll bar position */
    switch (LOWORD(msg))
    {
        case SB_LINEUP:
            info.nPos-=10;
            break;
            
        case SB_LINEDOWN:
            info.nPos+=10;
            break;
            
        case SB_PAGEUP:
            info.nPos-=info.nPage;
            break;
            
        case SB_PAGEDOWN:
            info.nPos+=info.nPage;
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
    
    if (info.nPos!=prevPos)
    {
    
        // update scroll bar settings    
        info.fMask=SIF_POS;
        SetScrollInfo(GetHandle(),SB_VERT,&info,true);
    
        // scroll window
        GetClientRect(GetHandle(),&rect);
        amount=(prevPos-info.nPos);        
        ScrollWindowEx(GetHandle(),0,amount,NULL,NULL,NULL,&rect,SW_ERASE|SW_INVALIDATE|SW_SCROLLCHILDREN);

        //InvalidateRect(hToolWindowCurrentTabContainer,&rect,true);
        UpdateWindow(GetHandle());
    }
}

/* handle events related to the main window */
LRESULT CALLBACK ScrollBox::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    
    ScrollBox* win=(ScrollBox*)Window::GetWindowObject(hwnd);
    switch (message)
    {
		case WM_NOTIFY:
			// forward notification messages to scrollbox's parent window
			static HWND parent=GetParentHandle(hwnd);
			if (parent!=NULL)
			{
				SendMessage(parent,message,wParam,lParam);
			}
			break;
			
        case WM_SHOWWINDOW:
            win->UpdateScrollBar();
            break;
        case WM_VSCROLL:
            win->Scroll(wParam);
            return 0;
    }
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}
