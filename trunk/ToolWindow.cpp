#include <Windows.h>
#include <Commctrl.h>
#include "ToolWindow.h"
#include "main.h"
#include "OverviewWindow.h"
#include "ImageWindow.h"
#include "console.h"
#include "SnappingWindow.h"

HWND hToolWindow=NULL;

HWND hToolWindowTabControl;
HWND hToolWindowDisplayTabContainer;
HWND hToolWindowQueryTabContainer;
HWND hToolWindowImageTabContainer;
HWND hToolWindowCurrentTabContainer;
HWND hToolWindowDisplayTabHeading;
HWND hToolWindowImageTabHeading;
HWND hToolWindowQueryTabHeading;
HWND hToolWindowScrollBar;

HWND hRed, hBlue, hGreen;
HWND *redRadiobuttons;                // band radio buttons
HWND *greenRadiobuttons;
HWND *blueRadiobuttons;
HWND *imageBandValues;                // values displayed under query tab

HWND cursorXPos, cursorYPos;          // position of cursor in image
HWND hupdate;
int bands = 0;                        // for use with radio buttons

/* objects used for painting/drawing */
HFONT hBoldFont,hNormalFont,hHeadingFont;
HPEN hTabPen;
HBRUSH hTabBrush;

/* variables used to store addresses to window procedures */
WNDPROC oldTabControlProc,oldDisplayTabContainerProc,oldQueryTabContainerProc,oldImageTabContainerProc,oldScrollBarContainerProc;


char szToolWindowClassName[] = "Parbat3D Tool Window";


/* ------------------------------------------------------------------------------------------------------------------------ */
/* Tools Window Functions */

int registerToolWindow()
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
    GetWindowRect(OverviewWindow::hOverviewWindow,&rect);
    
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
            snapWindowByMoving(OverviewWindow::hOverviewWindow,(RECT*)lParam);
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
                CheckMenuItem(OverviewWindow::hMainMenu,IDM_TOOLSWINDOW,MF_CHECKED|MF_BYCOMMAND);            
            else
                CheckMenuItem(OverviewWindow::hMainMenu,IDM_TOOLSWINDOW,MF_UNCHECKED|MF_BYCOMMAND);
            return 0;
                
        /* WM_CLOSE: system or user has requested to close the window/application */              
        case WM_CLOSE:
            /* don't destory this window, but make it invisible */            
            ShowWindow(hwnd,SW_HIDE);
            return 0;

        /* WM_DESTORY: system is destroying our window */                
        case WM_DESTROY:
            CheckMenuItem(OverviewWindow::hMainMenu,IDM_TOOLSWINDOW,MF_UNCHECKED|MF_BYCOMMAND);            
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

