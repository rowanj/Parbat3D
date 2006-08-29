#include <Windows.h>
#include <Commctrl.h>
#include "Window.h"
#include "ScrollBox.h"
#include "ToolWindow.h"
#include "main.h"
#include "OverviewWindow.h"
#include "ImageWindow.h"
#include "console.h"
#include "ToolTab.h"
#include "DisplayTab.h"
#include "QueryTab.h"
#include "ImageTab.h"
#include "FeatureTab.h"
#include "config.h"


/* create tool window */
int ToolWindow::Create(HWND)
{
    TCITEM tie;  /* datastructure for tabs */
    RECT rect;

    const int SCROLLBAR_WIDTH=13;
    const int SCROLLBAR_TOP=25;

  
    /* Get Main Window Location for image window alignment*/
    GetWindowRect(overviewWindow.GetHandle(),&rect);
    
    /* The class is registered, lets create the program*/
    if (!CreateWin(0, "Parbat3D Tool Window", "Tools",
           WS_POPUP+WS_CAPTION+WS_SYSMENU, rect.left, rect.bottom,
           250, 340, imageWindow.GetHandle(), NULL))
        return false;
    setupDrawingObjects(GetHandle());
    prevProc=SetWindowProcedure(&ToolWindow::WindowProcedure);
    
    stickyWindowManager.AddStickyWindow(this);  // make the window stick to others    

      /* get width & height of tool window's client area (ie. inside window's border) */
    GetClientRect(GetHandle(),&rect);

    /* create tab control */
    hToolWindowTabControl =CreateWindowEx(0, WC_TABCONTROL, "Tools",
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | TCS_OWNERDRAWFIXED, 0, 0,
		rect.right, rect.bottom, GetHandle(), NULL, Window::GetAppInstance(), NULL);

    /* force Windows to send us messages/events related to the tab control */
    SetWindowObject(hToolWindowTabControl,(Window*)this);
    oldTabControlProc=(WNDPROC)SetWindowLong(hToolWindowTabControl,GWL_WNDPROC,(long)&ToolWindowTabControlProcedure);

    /* add tabs to list */
    tabs.push_back(&displayTab);
    tabs.push_back(&queryTab);
    tabs.push_back(&imageTab);  
    tabs.push_back(&featureTab);      
  
    /* get size of tab control's client area (the area inside the tab control) */
    GetClientRect(hToolWindowTabControl,&rect);   

	/* Assign number of image bands to global variable */
    bands = image_handler->get_image_properties()->getNumBands() + 1;

    /* calculate the width & height for our tab container windows */
    const int SPACING_FOR_TAB_HEIGHT=30;    /* the height of the tabs + a bit of spacing */
    const int SPACING_FOR_BOARDER=5;        /* left & right margain + spacing for tab control's borders */
    rect.left=SPACING_FOR_BOARDER; /**** new ****/
    rect.top=SPACING_FOR_TAB_HEIGHT; /**** new ****/    
    rect.bottom-=SPACING_FOR_TAB_HEIGHT+SPACING_FOR_BOARDER;
    rect.right-=SPACING_FOR_BOARDER+SPACING_FOR_BOARDER;
    
    /* create tab & tab containers */

    for (int i=0;i<tabs.size();i++)
    {
        ToolTab *tab=tabs.at(i);
        tie.mask=TCIF_TEXT+TCIF_PARAM;;
        tie.pszText=tab->GetTabName();
        tie.lParam=(DWORD)tie.pszText;
        TabCtrl_InsertItem(hToolWindowTabControl, i, &tie);
        tab->Create(hToolWindowTabControl,&rect);
        tab->Hide();
    }
                             
    /* show display tab container */
    showToolWindowTabContainer(0);

    /* create scroll bar */
    GetClientRect(GetHandle(),&rect);
    
    return true;
}

// update cursor position on query tab
void ToolWindow::SetCursorPosition(int x,int y)
{
    string leader = "";
    queryTab.SetCursorX((char *) makeMessage(leader, x));
    queryTab.SetCursorY((char *) makeMessage(leader, y));
}

void ToolWindow::SetImageBandValue(int band,int value)
{
    string leader = "";
    queryTab.SetImageBandValue(band, (char *) makeMessage(leader, value));
}

/* draw a static text control on the screen */
void ToolWindow::drawStatic(DRAWITEMSTRUCT *dis, HFONT hfont)
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
void ToolWindow::drawTab(DRAWITEMSTRUCT *dis)
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
void ToolWindow::measureTab(MEASUREITEMSTRUCT *mis)
{
    const int TEXT_MARGIN=5;
    SIZE size;
    HDC hdc=GetDC(GetHandle());                                              /* get device context (drawing) object */
    SelectObject(hdc,hBoldFont);                                                 /* set font that will be used for drawing text */    
    GetTextExtentPoint32(hdc,(char*)mis->itemData,strlen((char*)mis->itemData),&size);     /* get width of string in pixels */   
    ReleaseDC(GetHandle(),hdc);                                              /* free device context handle */
    
    mis->itemWidth=size.cx+2*TEXT_MARGIN;                                    /* set width of tab */
    mis->itemHeight=size.cy+2*TEXT_MARGIN;                                   /* set height of tab */
}

/* setup up tool window's fonts & brushes for drawing */
void ToolWindow::setupDrawingObjects(HWND hwnd)
{
    HDC hdc;
    hdc=GetDC(hwnd);                 //hToolWindowTabControl
    hNormalFont=CreateFont(-MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72),0,0,0,400,false,false,false,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,"Tahoma"); //"MS Sans Serif" //Tahoma    
    hBoldFont=CreateFont(-MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72),0,0,0,600,false,false,false,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,"Tahoma"); //"MS Sans Serif" //Tahoma
    hHeadingFont=CreateFont(-MulDiv(9, GetDeviceCaps(hdc, LOGPIXELSY), 72),0,0,0,600,false,false,false,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,"Tahoma"); //"MS Sans Serif" //Tahoma   
    hTabPen=CreatePen(PS_SOLID,1,GetSysColor(COLOR_3DFACE));          
    hTabBrush=CreateSolidBrush(GetSysColor(COLOR_3DFACE));
    ReleaseDC(hwnd,hdc);            
}    

/* free all drawing objects */
void ToolWindow::freeDrawingObjects()
{
    DeleteObject(hNormalFont);
    DeleteObject(hBoldFont);
    DeleteObject(hHeadingFont);
    DeleteObject(hTabBrush);
    DeleteObject(hTabPen);    
}  

/* handle tab control messages/events */
LRESULT CALLBACK ToolWindow::ToolWindowTabControlProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ToolWindow* win=(ToolWindow*)Window::GetWindowObject(hwnd);
    
    switch (message)                  /* handle the messages */
    {
        case WM_DRAWITEM:
            /* draw window's owner-drawn static text controls using our custom font */
            if (((DRAWITEMSTRUCT*)lParam)->CtlType==ODT_STATIC)
                win->drawStatic((DRAWITEMSTRUCT*)lParam,win->hHeadingFont);
            break; 
                        
        default:
            break;
    }        
    // let Windows perform the default operation for the message recevied
    return CallWindowProc(win->oldTabControlProc,hwnd,message,wParam,lParam);
}


/* show the correct tab container window for the selected tab */
void ToolWindow::showToolWindowTabContainer(int selectedTabId)
{
    /* hide previously visible tab container window */
    if (hToolWindowCurrentTabContainer!=NULL)
    {
//        scrollToolWindowToTop();
        ShowWindow(hToolWindowCurrentTabContainer,SW_HIDE);
    }

    /* record the new active tab container window */        
    hToolWindowCurrentTabContainer=tabs.at(selectedTabId)->GetHandle();

    /* show new tab container window */
    ShowWindow(hToolWindowCurrentTabContainer,SW_SHOW);
    
    /* update scroll bar settings for the current tab */
//    updateToolWindowScrollbar();
}    

/* scrolls a window back to its orginal state 
void ToolWindow::scrollToolWindowToTop()
{
    SCROLLINFO info;
    RECT rect;
    int amount;
    
    // get scroll bar settings
    info.cbSize=sizeof(SCROLLINFO);
    info.fMask=SIF_POS;
    GetScrollInfo(hToolWindowScrollBar,SB_VERT,&info);
    
    GetClientRect(hToolWindowCurrentTabContainer,&rect);
    amount=info.nPos;
    ScrollWindowEx(hToolWindowCurrentTabContainer,0,amount,NULL,NULL,NULL,&rect,SW_ERASE|SW_INVALIDATE|SW_SCROLLCHILDREN);    
}    */

/* change the tool window's scrollbar settings based on the currently visible tab container 
void ToolWindow::updateToolWindowScrollbar()
{
    RECT rcontainer,rscrollbar;
    SCROLLINFO info;    
   
    // get position of current container & scrollbar 
    GetWindowRect(hToolWindowCurrentTabContainer,&rcontainer);
    GetWindowRect(hToolWindowScrollBar,&rscrollbar);

    // set height of visible scroll area   
    info.nPage=rscrollbar.bottom-rscrollbar.top;
    
    // set scroll range 
    info.nMin=0;
    info.nMax=rcontainer.bottom-rcontainer.top;
    
    // set scroll position
    info.nPos=0;
    
    Console::write("updateToolWindowScrollbar() ");
    Console::write("\ninfo.nMin=");
    Console::write(info.nMin);
    Console::write(" info.nMax=");
    Console::write(info.nMax);
    Console::write("\ninfo.nPage=");
    Console::write(info.nPage);
    Console::write("\n");
       
    // set scrollbar info 
    info.cbSize=sizeof(SCROLLINFO);
    info.fMask=SIF_ALL;
    SetScrollInfo(hToolWindowScrollBar,SB_VERT,&info,true);  
    
} */   


/*
void ToolWindow::scrollToolWindow(int msg)
{
    SCROLLINFO info;
    RECT rect;
    int prevPos;
    int amount;
    
    // get scroll bar settings 
    info.cbSize=sizeof(SCROLLINFO);
    info.fMask=SIF_POS|SIF_RANGE|SIF_TRACKPOS|SIF_PAGE;
    GetScrollInfo(hToolWindowScrollBar,SB_VERT,&info);
    prevPos=info.nPos;

    // calculate new scroll bar position
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

    //InvalidateRect(hToolWindowCurrentTabContainer,&rect,true);
    UpdateWindow(hToolWindowCurrentTabContainer);
}   */  


/* handles messages/events related to the tool window */
LRESULT CALLBACK ToolWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static POINT snapMouseOffset;     /* mouse offset relative to window, used for snapping */
    NMHDR *nmhdr;                     /* structure used for WM_NOTIFY events */
    RECT rect;

    ToolWindow* win=(ToolWindow*)Window::GetWindowObject(hwnd);
            
    switch (message)                  /* handle the messages */
    {
        
        case WM_NOTIFY:
            /* handle change in tab selection  */
            nmhdr=(NMHDR*)lParam;
            switch(nmhdr->code)
            {                   
                case TCN_SELCHANGE:
                   /* display the tab container window that is associated with the selected tab */                                   
                   win->showToolWindowTabContainer(TabCtrl_GetCurSel(win->hToolWindowTabControl));
            }    
            break;

       
        /* WM_DRAWITEM: an ownerdraw control owned by this window needs to be drawn */
        case WM_DRAWITEM:
            if (((DRAWITEMSTRUCT*)lParam)->CtlType==ODT_TAB)
                win->drawTab((DRAWITEMSTRUCT*)lParam);
            else
                win->drawStatic((DRAWITEMSTRUCT*)lParam,win->hNormalFont);
            return 0;          

        /* WM_MEASUREITEM: an ownerdraw control needs to be measured */        
        case WM_MEASUREITEM:
            if (((DRAWITEMSTRUCT*)lParam)->CtlType==ODT_TAB)
                win->measureTab((MEASUREITEMSTRUCT*)lParam);
            return 0;

        case WM_SHOWWINDOW:
            /* update window menu item depending on whether window is shown or hidden */
            if (wParam)
                CheckMenuItem(overviewWindow.hMainMenu,IDM_TOOLSWINDOW,MF_CHECKED|MF_BYCOMMAND);            
            else
                CheckMenuItem(overviewWindow.hMainMenu,IDM_TOOLSWINDOW,MF_UNCHECKED|MF_BYCOMMAND);
            return 0;
                
        /* WM_CLOSE: system or user has requested to close the window/application */              
        case WM_CLOSE:
            /* don't destory this window, but make it invisible */            
            ShowWindow(hwnd,SW_HIDE);
            return 0;

        /* WM_DESTORY: system is destroying our window */                
        case WM_DESTROY:
            CheckMenuItem(overviewWindow.hMainMenu,IDM_TOOLSWINDOW,MF_UNCHECKED|MF_BYCOMMAND);            
            win->freeDrawingObjects();           
            stickyWindowManager.RemoveStickyWindow(win);
            break;

    }
    /* return 0 to indicate that we have processed the message */          
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);
}

/* handle events related to tool window's scroll bar 
LRESULT CALLBACK ToolWindow::ToolWindowScrollBarProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ToolWindow* win=(ToolWindow*)Window::GetWindowObject(hwnd);
        
    switch (message)
    {
        //case WM_VSCROLL:          
            //win->scrollToolWindow(wParam);
          //  return 0;
                        
        default:
            break;
    }        
    // let Windows perform the default operation for the message recevied        
    return CallWindowProc(win->oldTabControlProc,hwnd,message,wParam,lParam);
}
*/
