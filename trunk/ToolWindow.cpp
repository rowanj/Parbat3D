#include <Windows.h>
#include <Commctrl.h>
#include "Window.h"
#include "ScrollBox.h"
#include "ToolWindow.h"
#include "main.h"
#include "OverviewWindow.h"
#include "ImageWindow.h"
#include "console.h"
#include "SnappingWindow.h"
#include "ToolTab.h"
#include "DisplayTab.h"
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
           250, 300, imageWindow.GetHandle(), NULL))
        return false;
    setupDrawingObjects(GetHandle());
    prevProc=SetWindowProcedure(&ToolWindow::WindowProcedure);


      /* get width & height of tool window's client area (ie. inside window's border) */
    GetClientRect(GetHandle(),&rect);

    /* create tab control */
    hToolWindowTabControl =CreateWindowEx(0, WC_TABCONTROL, "Tools",
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | TCS_OWNERDRAWFIXED, 0, 0,
		rect.right, rect.bottom, GetHandle(), NULL, Window::GetAppInstance(), NULL);

    /* force Windows to send us messages/events related to the tab control */
    SetWindowObject(hToolWindowTabControl,(Window*)this);
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
    rect.left=SPACING_FOR_BOARDER; /**** new ****/
    rect.top=SPACING_FOR_TAB_HEIGHT; /**** new ****/    
    rect.bottom-=SPACING_FOR_TAB_HEIGHT+SPACING_FOR_BOARDER;
    rect.right-=SPACING_FOR_BOARDER+SPACING_FOR_BOARDER;
    
    /* create tab containers for each tab (a child window that will be shown/hidden when user clicks on a tab) */
    /* Display tab container */
    
    displayTab.Create(hToolWindowTabControl,&rect);
   
           
	/* Query tab container */
    hToolWindowQueryTabContainer =CreateWindowEx(0, szStaticControl, "", 
		WS_CHILD | WS_CLIPSIBLINGS | SS_OWNERDRAW, SPACING_FOR_BOARDER,             /* left position relative to tab control */
           SPACING_FOR_TAB_HEIGHT, rect.right-SCROLLBAR_WIDTH, 80 + (20 * (bands+1)), hToolWindowTabControl,           /* The window is a childwindow of the tab control */
           NULL, Window::GetAppInstance(), NULL);

	hToolWindowQueryTabHeading =CreateWindowEx( 0, szStaticControl, "Band Values", 
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_OWNERDRAW, 0,
		0, rect.right-SCROLLBAR_WIDTH, 20, hToolWindowQueryTabContainer, NULL,
		Window::GetAppInstance(), NULL); 		
                      
    /* Image tab container */
    hToolWindowImageTabContainer =CreateWindowEx(0, szStaticControl, "",                            
           WS_CHILD | WS_CLIPSIBLINGS | SS_OWNERDRAW, SPACING_FOR_BOARDER,             /* left position relative to tab control */
           SPACING_FOR_TAB_HEIGHT, rect.right-SCROLLBAR_WIDTH, rect.bottom, hToolWindowTabControl, NULL,                            /* No menu */
           Window::GetAppInstance(), NULL); 

	hToolWindowImageTabHeading =CreateWindowEx( 0, szStaticControl, "Image Properties", 
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_OWNERDRAW, 0,
		0, rect.right-SCROLLBAR_WIDTH, 20, hToolWindowImageTabContainer, NULL,
		Window::GetAppInstance(), NULL); 		

           
    /* show display tab container */
    showToolWindowTabContainer(DISPLAY_TAB_ID);

    /* create scroll bar */
    GetClientRect(GetHandle(),&rect);
    hToolWindowScrollBar=CreateWindowEx(0, "SCROLLBAR", NULL,
		WS_CHILD | WS_VISIBLE | SBS_RIGHTALIGN | SBS_VERT, 0, SCROLLBAR_TOP,
		rect.right, rect.bottom-SCROLLBAR_TOP, GetHandle(), NULL, Window::GetAppInstance(), NULL);           
    
    /* force Windows to notify us of messages/events related to these controls */
    SetWindowObject(hToolWindowQueryTabContainer,(Window*)this);    
    oldQueryTabContainerProc=(WNDPROC)SetWindowLong(hToolWindowQueryTabContainer,GWL_WNDPROC,(long)&ToolWindowQueryTabContainerProcedure);
    SetWindowObject(hToolWindowImageTabContainer,(Window*)this);    
    oldImageTabContainerProc=(WNDPROC)SetWindowLong(hToolWindowImageTabContainer,GWL_WNDPROC,(long)&ToolWindowImageTabContainerProcedure);
    SetWindowObject(hToolWindowScrollBar,(Window*)this);    
    oldScrollBarContainerProc=(WNDPROC)SetWindowLong(hToolWindowScrollBar,GWL_WNDPROC,(long)&ToolWindowScrollBarProcedure);
    
	/* Create container for band values */
	HWND queryValueContainer = CreateWindowEx(0, "BUTTON", "Values",
		WS_CHILD | BS_GROUPBOX | WS_VISIBLE, 138, 45, 66, 20 + (20 * (bands-1)),
		hToolWindowQueryTabContainer, NULL, Window::GetAppInstance(), NULL);
	
	/* Dynamically add image band values */
	imageBandValues = new HWND[bands];


    for (int i=1; i<bands; i++)  
    {

		const char* name;
		// add band names to radio buttons
		name = "";
		//name = image_handler->get_band_info(i)->getColourInterpretationName();
		
		// If Colour name unknown change band name
		//const char *altName ="No colour name";
		//if (strcmp(name, "Unknown")==0)
         // name = altName;
            
        // Add band number to band name
        //name = catcstrings( (char*) " - ", (char*) name);
        name = catcstrings( (char*) inttocstring(i), (char*) name);
        name = catcstrings( (char*) "Band ", (char*) name);

        // add channel names under the query tab
        CreateWindowEx(0, szStaticControl, name,
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 20, 40 + (20 * i), 100, 18,
			GetHandle(), NULL, Window::GetAppInstance(), NULL);

		// add the band values to the value container under the query tab
        char tempBandValue[4] = "0"; // temporary storage for the band value
        imageBandValues[i] = CreateWindowEx(0, szStaticControl, tempBandValue, WS_CHILD | WS_VISIBLE, 5, 15 + (20 * (i-1)),
			50, 18, queryValueContainer, NULL, Window::GetAppInstance(), NULL);
    }

	
	/* display cursor position under query tab */
	CreateWindowEx(0, szStaticControl, "X", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW,
                 20, 60 + (20 * bands), 100, 18, hToolWindowQueryTabContainer, NULL, Window::GetAppInstance(), NULL);
	CreateWindowEx(0, szStaticControl, "Y", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW,
                 20, 60 + (20 * (bands+1)), 100, 18, hToolWindowQueryTabContainer, NULL, Window::GetAppInstance(), NULL);
    cursorXPos = CreateWindowEx(0, szStaticControl, "-", WS_CHILD | WS_VISIBLE,
                 143, 60 + (20 * bands), 50, 18, hToolWindowQueryTabContainer, NULL, Window::GetAppInstance(), NULL);			
    cursorYPos = CreateWindowEx(0, szStaticControl, "-", WS_CHILD | WS_VISIBLE,
                 143, 60 + (20 * (bands+1)), 50, 18, hToolWindowQueryTabContainer, NULL, Window::GetAppInstance(), NULL);			
	
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
	    
	string drivername = ip->getDriverLongName();
	string finaldrivname;
	if (drivername.length() > 25) {
		fname = drivername.substr(0, 12);
		bname = drivername.substr(drivername.length()-12, drivername.length()-1);
		finaldrivname = fname + "…" + bname;
	} else
	    finaldrivname = drivername;
	
	    
	n[0]="File Name"; v[0]=makeMessage(leader, (char*) finalname.c_str());
	n[1]="File Type"; v[1]=makeMessage(leader, (char*) finaldrivname.c_str());
	n[2]="Width"; v[2]=makeMessage(leader, ip->getWidth());
	n[3]="Height"; v[3]=makeMessage(leader, ip->getHeight());
	n[4]="Bands"; v[4]=makeMessage(leader, ip->getNumBands());
	
	for (int i=0; i<ipItems; i++) {
		CreateWindowEx(0, szStaticControl, (char*) n[i].c_str(),
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 20, 40+(i*20), 50, 18,
			hToolWindowImageTabContainer, NULL, Window::GetAppInstance(), NULL);
		
		CreateWindowEx(0, szStaticControl, (char*) v[i].c_str(),
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 78, 40+(i*20), 160, 18,
			hToolWindowImageTabContainer, NULL, Window::GetAppInstance(), NULL);
    }
    
    /* setup tool window scrollbar */
    updateToolWindowScrollbar();
    
    return true;
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


/* handle query tab container's messages/events */
LRESULT CALLBACK ToolWindow::ToolWindowQueryTabContainerProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ToolWindow* win=(ToolWindow*)Window::GetWindowObject(hwnd);
        
    switch (message)
    {
        /* draw window's owner-drawn static text controls using our custom fonts */
        case WM_DRAWITEM:
            if (((DRAWITEMSTRUCT*)lParam)->CtlType==ODT_STATIC)
                if (((DRAWITEMSTRUCT*)lParam)->hwndItem==win->hToolWindowQueryTabHeading)
                    win->drawStatic((DRAWITEMSTRUCT*)lParam,win->hBoldFont);                
                else
                    win->drawStatic((DRAWITEMSTRUCT*)lParam,win->hNormalFont);
            break; 
                        
        default:
            break;
    }        
    // let Windows perform the default operation for the message recevied
    return CallWindowProc(win->oldQueryTabContainerProc,hwnd,message,wParam,lParam);
}

/* handle query tab container's messages/events */
LRESULT CALLBACK ToolWindow::ToolWindowImageTabContainerProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ToolWindow* win=(ToolWindow*)Window::GetWindowObject(hwnd);
        
    switch (message)                  /* handle the messages */
    {
        /* draw window's owner-drawn static text controls using our custom fonts */
        case WM_DRAWITEM:
            if (((DRAWITEMSTRUCT*)lParam)->CtlType==ODT_STATIC)
                if (((DRAWITEMSTRUCT*)lParam)->hwndItem==win->hToolWindowImageTabHeading)
                    win->drawStatic((DRAWITEMSTRUCT*)lParam,win->hBoldFont);                
                else
                    win->drawStatic((DRAWITEMSTRUCT*)lParam,win->hNormalFont);
            break; 
                        
        default:
            break;
    }        
    // let Windows perform the default operation for the message recevied    
    return CallWindowProc(win->oldImageTabContainerProc,hwnd,message,wParam,lParam);
}

/* show the correct tab container window for the selected tab */
void ToolWindow::showToolWindowTabContainer(int selectedTabId)
{
    /* hide previously visible tab container window */
    if (hToolWindowCurrentTabContainer!=NULL)
    {
        scrollToolWindowToTop();
        ShowWindow(hToolWindowCurrentTabContainer,SW_HIDE);
    }

    /* record the new active tab container window */        
    switch(selectedTabId)
    {
        case DISPLAY_TAB_ID:
                hToolWindowCurrentTabContainer=displayTab.GetHandle();
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
void ToolWindow::scrollToolWindowToTop()
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
void ToolWindow::updateToolWindowScrollbar()
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


void ToolWindow::scrollToolWindow(int msg)
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

    //InvalidateRect(hToolWindowCurrentTabContainer,&rect,true);
    UpdateWindow(hToolWindowCurrentTabContainer);
}    


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

        case WM_NCLBUTTONDOWN:
            /* check if the mouse has been clicked on the title bar */
            if(wParam == HTCAPTION)
            {
               /* get the mouse co-ords relative to the window */
                SnappingWindow::getMouseWindowOffset(hwnd,(int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam),&snapMouseOffset);
            }
            /* also let windows handle this message */
            return DefWindowProc(hwnd, message, wParam, lParam); 
            
        case WM_MOVING:
            /* set new window position based on position of mouse */
            SnappingWindow::setNewWindowPosition((RECT*)lParam,&snapMouseOffset);

            /* if new position is near desktop edge, move to the egde */
            SnappingWindow::snapInsideWindowByMoving(hDesktop,(RECT*)lParam);  
            
            /* if new position is near another window, move to other the window */    
            SnappingWindow::snapWindowByMoving(overviewWindow.GetHandle(),(RECT*)lParam);
            SnappingWindow::snapWindowByMoving(imageWindow.GetHandle(),(RECT*)lParam);
            break;
        
        /* WM_DRAWITEM: an ownerdraw control owned by this window needs to be drawn */
        case WM_DRAWITEM:
            if (((DRAWITEMSTRUCT*)lParam)->CtlType==ODT_TAB)
                win->drawTab((DRAWITEMSTRUCT*)lParam);
            else
                win->drawStatic((DRAWITEMSTRUCT*)lParam,win->hNormalFont);
            break;           

        /* WM_MEASUREITEM: an ownerdraw control needs to be measured */        
        case WM_MEASUREITEM:
            if (((DRAWITEMSTRUCT*)lParam)->CtlType==ODT_TAB)
                win->measureTab((MEASUREITEMSTRUCT*)lParam);
            break;

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
            return 0;

        default: 
            return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);
    }
    /* return 0 to indicate that we have processed the message */          
    return 0;
}

/* handle events related to tool window's scroll bar */
LRESULT CALLBACK ToolWindow::ToolWindowScrollBarProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ToolWindow* win=(ToolWindow*)Window::GetWindowObject(hwnd);
        
    switch (message)
    {
        case WM_VSCROLL:          
            win->scrollToolWindow(wParam);
            return 0;
                        
        default:
            break;
    }        
    // let Windows perform the default operation for the message recevied        
    return CallWindowProc(win->oldTabControlProc,hwnd,message,wParam,lParam);
}

