#include "config.h"
#include <Windows.h>
#include <Commctrl.h>
#include <vector>


#include "console.h"
#include "main.h"
#include "OverviewWindow.h"
#include "MainWindow.h"
#include "DisplayWindow.h"
#include "ToolWindow.h"
#include "ImageHandler.h"
#include "ImageWindow.h"
#include "ROI.h"
#include "ROISet.h"
#include "ROIFile.h"
#include "ROIWindow.h"
#include "Settings.h"
#include "ScrollBox.h"
#include "StringUtils.h"


ScrollBox ROIscrollBox;

vector<HWND> ROIWindow::roiColourButtonList;

WNDPROC ROIWindow::prevListViewProc;

// array to store custom colour values as choosen by user (all initilised to grey)
COLORREF ROIWindow::customColours[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int ROIWindow::Create(HWND parent)
{
    
    RECT rect;
    int mx,my;
    const int ROI_WINDOW_WIDTH=250;
    const int ROI_WINDOW_HEIGHT=300;

    /* Get Main Window Location for ROI window alignment*/
    GetWindowRect(overviewWindow.GetHandle(),&rect);

    /* create ROI window */
    if (!CreateWin(0, "Parbat3D ROI Window", "Regions of Interest",
	     WS_POPUP | WS_SYSMENU | WS_CAPTION | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
	     rect.left, rect.bottom+30, 250, 300, parent, NULL))
	    return false;
	    
    prevProc=SetWindowProcedure(&WindowProcedure);

	// Create scroll box 	
	RECT rect2;
   	rect2.top=5;
   	rect2.left=5;
   	rect2.right=235;
   	rect2.bottom=227;                 	
	ROIscrollBox.Create(GetHandle(),&rect2);

	// Create listview control inside scroll box
	GetClientRect(ROIscrollBox.GetHandle(),&rect);
    hROIListBox = CreateWindowEx(0,WC_LISTVIEW, 
                                NULL, 
                                WS_CHILD | LVS_REPORT | LVS_EDITLABELS | WS_VISIBLE | LVS_NOCOLUMNHEADER | LVS_SINGLESEL | LVS_SORTASCENDING | LVS_SHOWSELALWAYS, 
                                0, 
                                0, 
                                rect.right - rect.left, 
                                rect.bottom - rect.top, 
                                ROIscrollBox.GetHandle(), 
                                (HMENU) 0, 
                                Window::GetAppInstance(), 
                                NULL); 	
	ListView_SetExtendedListViewStyle(hROIListBox,LVS_EX_CHECKBOXES);
	prevListViewProc=SetWindowProcedure(hROIListBox,&ROIListViewProcedure);
	
	LVCOLUMN lvc;                                
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
	lvc.iSubItem = 0;
	lvc.pszText = "Show";
	lvc.cx = rect.right-45;     // width of column in pixels
	lvc.fmt = LVCFMT_CENTER; 
	ListView_InsertColumn(hROIListBox, 0, &lvc);

	lvc.iSubItem = 1;
	lvc.pszText = "Colour";
	lvc.cx = 20;     // width of column in pixels
	lvc.fmt = LVCFMT_CENTER; 
	ListView_InsertColumn(hROIListBox,2, &lvc);







	// add ROI check boxes
/*	int roiInList = regionsSet->get_regions_count();
	vector<RoI> rList = regionsSet->get_regions();
	RoI rCur;
	hROITick=new HWND[roiInList];	
    for (int i=0; i<roiInList; i++) {
        rCur = rList.at(i);
		hROITick[i] = CreateWindowEx( 0, "BUTTON",
                      copyString(rCur.get_name().c_str()), 
		              BS_AUTOCHECKBOX | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 10,
		              10 + (20 * i), 100, 16, scrollBox.GetHandle(),NULL,
		              Window::GetAppInstance(), NULL);
	}*/
	
	//Create Open button
	hOpenButton =CreateWindowEx( 0, "BUTTON", NULL, 
		BS_ICON | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 5,
		237, 28, 28, GetHandle(), (HMENU) 1,
		Window::GetAppInstance(), NULL);
		
	hOpenIcon=(HICON)LoadImage(NULL,IDI_EXCLAMATION,IMAGE_ICON,0,0,LR_SHARED);
	SendMessage (hOpenButton, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadImage (Window::GetAppInstance(), "open.ico", IMAGE_ICON, 24, 24,LR_LOADFROMFILE));
	
	//Create Save button
	hSaveButton =CreateWindowEx( 0, "BUTTON", NULL, 
		BS_ICON | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 38,
		237, 28, 28, GetHandle(),  (HMENU) 2,
		Window::GetAppInstance(), NULL);
		
	hSaveIcon=(HICON)LoadImage(NULL,IDI_EXCLAMATION,IMAGE_ICON,0,0,LR_SHARED);
	SendMessage (hSaveButton, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadImage (Window::GetAppInstance(), "save.ico", IMAGE_ICON, 24, 24,LR_LOADFROMFILE));
	
	//Create New ROI button
	hNewButton =CreateWindowEx( 0, "BUTTON", NULL, 
		BS_ICON | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 75,
		237, 28, 28, GetHandle(),  (HMENU) 3,
		Window::GetAppInstance(), NULL);
		
	hNewIcon=(HICON)LoadImage(NULL,IDI_EXCLAMATION,IMAGE_ICON,0,0,LR_SHARED);
	SendMessage (hNewButton, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadImage (Window::GetAppInstance(), "new.ico", IMAGE_ICON, 24, 24,LR_LOADFROMFILE));
	
	//Create Delete ROI button
	hDeleteButton =CreateWindowEx( 0, "BUTTON", NULL, 
		BS_ICON | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 108,
		237, 28, 28, GetHandle(),  (HMENU) 4,
		Window::GetAppInstance(), NULL);
		
	hDeleteIcon=(HICON)LoadImage(NULL,IDI_EXCLAMATION,IMAGE_ICON,0,0,LR_SHARED);
	SendMessage (hDeleteButton, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadImage (Window::GetAppInstance(), "delete.ico", IMAGE_ICON, 24, 24,LR_LOADFROMFILE));
	
	//Create Polygon selection button
	hPolyButton =CreateWindowEx( 0, "BUTTON", NULL, 
		BS_ICON | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 146,
		237, 28, 28, GetHandle(),  (HMENU) 5,
		Window::GetAppInstance(), NULL);
		
	hPolyIcon=(HICON)LoadImage(NULL,IDI_EXCLAMATION,IMAGE_ICON,0,0,LR_SHARED);
	SendMessage (hPolyButton, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadImage (Window::GetAppInstance(), "poly.ico", IMAGE_ICON, 24, 24,LR_LOADFROMFILE));
	
	//Create Rectangle selection button
	hRectButton =CreateWindowEx( 0, "BUTTON", NULL, 
		BS_ICON | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 179,
		237, 28, 28, GetHandle(),  (HMENU) 6,
		Window::GetAppInstance(), NULL);
		
	hRectIcon=(HICON)LoadImage(NULL,IDI_EXCLAMATION,IMAGE_ICON,0,0,LR_SHARED);
	SendMessage (hRectButton, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadImage (Window::GetAppInstance(), "rect.ico", IMAGE_ICON, 24, 24,LR_LOADFROMFILE));
	
	//Create single point selection button
	hSingleButton =CreateWindowEx( 0, "BUTTON", NULL, 
		BS_ICON | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 212,
		237, 28, 28, GetHandle(),  (HMENU) 7,
		Window::GetAppInstance(), NULL);
		
	hSingleIcon=(HICON)LoadImage(NULL,IDI_EXCLAMATION,IMAGE_ICON,0,0,LR_SHARED);
	SendMessage (hSingleButton, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadImage (Window::GetAppInstance(), "single.ico", IMAGE_ICON, 24, 24,LR_LOADFROMFILE));

	
	
	// Tool Tips
	/* CREATE A TOOLTIP WINDOW */
    hwndTT = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, "tooltips_class32", "single point",
        WS_POPUP,		
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        hSingleButton,
        NULL,
        Window::GetAppInstance(),
        NULL);
        

        
    
    SetWindowPos(hwndTT,
        HWND_TOPMOST,
        0,
        0,
        0,
        0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);


    /* GET COORDINATES OF THE MAIN CLIENT AREA */
    GetClientRect (hSingleButton, &rect);

    /* INITIALIZE MEMBERS OF THE TOOLINFO STRUCTURE */
    /*
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS;
    ti.hwnd = hwnd;
    ti.hinst = ghThisInstance;
    ti.uId = uid;
    ti.lpszText = lptstr;
        // ToolTip control will cover the whole window
    ti.rect.left = rect.left;    
    ti.rect.top = rect.top;
    ti.rect.right = rect.right;
    ti.rect.bottom = rect.bottom;

    /* SEND AN ADDTOOL MESSAGE TO THE TOOLTIP CONTROL WINDOW */
    //SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
	
	
	
	
    //ROIscrollBox.UpdateScrollBar();
	
	return true;
}

// show colour dialog box allowing user to change an roi's colour
void ROIWindow::ChangeROIColour(HWND hbutton)
{
	CHOOSECOLOR cc;
	int r,g,b;	
	int colour;	
	ROI *roi;
	int i;

	// find ROI object that is associated with the button
	for (i=0;i<roiColourButtonList.size();i++)
	{
		if (roiColourButtonList.at(i)==hbutton)
		{
			LVITEM item;
			item.mask=LVIF_PARAM;
			item.iItem=i;
			item.iSubItem=0;
			ListView_GetItem(roiWindow.hROIListBox,&item);			
			roi=(ROI*)item.lParam;	
			
			roi->get_color(&r,&g,&b);
			colour=(b<<16)+(g<<8)+r;
			
			//  display colour dialog box
			cc.lStructSize=sizeof(CHOOSECOLOR);
			cc.hwndOwner=roiWindow.GetHandle();
			cc.hInstance=NULL;
			cc.rgbResult=colour;			// inital colour (format: 0xBBGGRR)
			cc.lpCustColors=customColours;	// pointer to array of 16 custom colours
			cc.Flags=CC_ANYCOLOR|CC_FULLOPEN|CC_RGBINIT|CC_SOLIDCOLOR;
			cc.lCustData=0;
			cc.lpfnHook=NULL;
			cc.lpTemplateName=NULL;
			if (ChooseColor(&cc)!=0)
			{
				// save new colour
				colour=cc.rgbResult;
				r=colour&0xFF;
				g=(colour>>8)&0xFF;
				b=(colour>>16)&0xFF;
				roi->set_colour(r,g,b);
				
				// re-paint colour button
				InvalidateRect(hbutton,NULL,TRUE);
				UpdateWindow(hbutton);
				// re-paint image window
				imageWindow.Repaint();
			}					
			
			break;
		}
	}
	
	
}

// draw an ROI's colour button in its own colour
void ROIWindow::DrawColourButton(LPDRAWITEMSTRUCT di)
{
	HBRUSH hbrush;
	RECT rect;
	ROI *roi;
	int colour;
	int r,g,b;	
	int i;

	// find the ROI object that is associated with the colour button
	for (i=0;i<roiColourButtonList.size();i++)
	{
		if (di->hwndItem==roiColourButtonList.at(i))
		{
			LVITEM item;
			item.mask=LVIF_PARAM;
			item.iItem=i;
			item.iSubItem=0;
			ListView_GetItem(roiWindow.hROIListBox,&item);			
			roi=(ROI*)item.lParam;
			roi->get_color(&r,&g,&b);
			colour=(b<<16)+(g<<8)+r;
			
			// draw box filled with ROI's colour
			hbrush=CreateSolidBrush(colour);
			SelectObject(di->hDC,hbrush);
			GetClientRect(di->hwndItem,&rect);
			Rectangle(di->hDC,rect.left,rect.top,rect.right,rect.bottom);
			DrawEdge(di->hDC,&rect,EDGE_RAISED,BF_RECT);
			DeleteObject(hbrush);
			break;
		}
	}	
}

/* handle events related to the list view control & its controls */
LRESULT CALLBACK ROIWindow::ROIListViewProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	ROI *roi;
	switch (message)
	{
		// handle button clicks
		case WM_COMMAND:
			if (HIWORD(wParam)==BN_CLICKED)
			{
				ChangeROIColour((HWND)lParam);
			}
			break;
		
		// draw colour buttons 
		case WM_DRAWITEM:
			DrawColourButton((LPDRAWITEMSTRUCT) lParam);
			break;		
	}
    return CallWindowProc(prevListViewProc,hwnd,message,wParam,lParam);	
}

// handle list item changed notification
void ROIWindow::OnListItemChanged(NMLISTVIEW* pnml)
{
	if ((pnml->uChanged&LVIF_STATE)!=0)		// check if state of item has changed
	{
		//Console::write("ROIWindow LVN_ITEMCHANGED\n\tstate changed, id=%d oldState=%d newState=%d uChanged=%d uChangedAnded=%d newStateAnded=%d\n",pnml->iItem,pnml->uOldState,pnml->uNewState,pnml->uChanged,pnml->uChanged&LVIF_STATE,pnml->uNewState&LVIS_STATEIMAGEMASK);
		
		// get check box state (1=unticked, 2=ticked)
		int checkBoxState=(pnml->uNewState&LVIS_STATEIMAGEMASK)>>12;			
	
		switch (checkBoxState)
		{
			
			case 2:
				// show ROI
				Console::write("List view item (id=%d) ticked\n",pnml->iItem);
				break;
				
			case 1:
				// hide ROI
				Console::write("List view item (id=%d) unticked\n",pnml->iItem);						
				break;							
		}
	
	}
}

/* handle messages/events related to the ROI window */
LRESULT CALLBACK ROIWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT rect;                 /* for general use */
	NMHDR *pnmh;
    
    ROIWindow* win=(ROIWindow*)Window::GetWindowObject(hwnd);
    
    switch (message)                  /* handle the messages */
    {
		case WM_NOTIFY:
			pnmh=(NMHDR*)lParam;
			switch(pnmh->code)
			{
				case LVN_ITEMCHANGED:
					win->OnListItemChanged((NMLISTVIEW*)pnmh);
					break;
			}
			break;
			
        case WM_COMMAND:
            /* Open Button */
			if (LOWORD(wParam) == 1 && HIWORD(wParam) == BN_CLICKED)
				win->loadROI(win);
            
            /* Save Button */
			if (LOWORD(wParam) == 2 && HIWORD(wParam) == BN_CLICKED)
                win->saveROI(win);
            
            /* New ROI Button */
			if (LOWORD(wParam) == 3 && HIWORD(wParam) == BN_CLICKED)
                win->newROI(win, NULL);
            
            /* Delete ROI Button */
			if (LOWORD(wParam) == 4 && HIWORD(wParam) == BN_CLICKED)
                win->deleteROI(win);
            
            /* Create Polygon ROI Button */
			if (LOWORD(wParam) == 5 && HIWORD(wParam) == BN_CLICKED)
                win->newEntity(win, ROI_POLY);
            
            /* Create Rectangle ROI Button */
			if (LOWORD(wParam) == 6 && HIWORD(wParam) == BN_CLICKED)
                win->newEntity(win, ROI_RECT);
            
            /* Create Single Point ROI Button */
			if (LOWORD(wParam) == 7 && HIWORD(wParam) == BN_CLICKED) {
                win->newEntity(win, ROI_POINT);
            }
            
            // turn off the buttons if an entity is being created
            win->updateButtons(win);
            
            ROIscrollBox.UpdateScrollBar();
            
			return 0;
				
        case WM_SHOWWINDOW:
            /* update window menu item depending on whether window is shown or hidden */
            if (wParam)
                CheckMenuItem(overviewWindow.hMainMenu,IDM_ROIWINDOW,MF_CHECKED|MF_BYCOMMAND);            
            else
                CheckMenuItem(overviewWindow.hMainMenu,IDM_ROIWINDOW,MF_UNCHECKED|MF_BYCOMMAND);
            return 0;

        /* WM_CLOSE: system or user has requested to close the window/application */             
        case WM_CLOSE:
            /* don't destroy this window, but make it invisible */
            ShowWindow(hwnd,SW_HIDE);
            CheckMenuItem(overviewWindow.hMainMenu,IDM_ROIWINDOW,MF_UNCHECKED|MF_BYCOMMAND);            
            return 0;

        /* WM_DESTORY: system is destroying our window */
        case WM_DESTROY:
            break;
    }
    // call next window procedure in chain
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}



int ROIWindow::getROICheckedCount () {
    int items_in_list = SendMessage(hROIListBox, LVM_GETITEMCOUNT, 0, 0);
    int i;
    int checked = 0;
    
    // find how many ROIs are selected
    for (i=0; i<items_in_list; i++) {
        if (ListView_GetCheckState(hROIListBox, i))
            checked++;
    }
    
    return checked;
}


int ROIWindow::getSelectedItemIndex () {
    int items_in_list = SendMessage(hROIListBox, LVM_GETITEMCOUNT, 0, 0);
    int selected = -1;
    
    for (int i=0; i<items_in_list; i++) {
        if (SendMessage(hROIListBox, LVM_GETITEMSTATE, i, LVIS_SELECTED) == LVIS_SELECTED) {
            selected = i;
            break;
        }
    }
    
    return selected;
}

string ROIWindow::getSelectedItemText () {
    int items_in_list = SendMessage(hROIListBox, LVM_GETITEMCOUNT, 0, 0);
    char buffer[256];
    string *name = new string("");
    
    for (int i=0; i<items_in_list; i++) {
        if (SendMessage(hROIListBox, LVM_GETITEMSTATE, i, LVIS_SELECTED) == LVIS_SELECTED) {
            LVITEM lvitem;
            lvitem.mask = LVIF_TEXT;
            lvitem.iItem = i;
            lvitem.pszText = buffer;
            lvitem.cchTextMax = 256;
            lvitem.iSubItem = 0;
            
            ListView_GetItem(hROIListBox, &lvitem);
            name = new string(buffer);
            
            break;
        }
    }
    
    return *name;
}


void ROIWindow::newROI (ROIWindow* win, const char* roiType) {
    int listSize = regionsSet->get_regions_count();
    int newId=listSize;
    
    Console::write("ROIWindow::newROI newId=");
    Console::write(newId);
    Console::write("\n");
    
    // create ROI with the default name
    ROI *rCur = regionsSet->new_region();
    if (roiType != NULL)
        regionsSet->new_entity(roiType);
    
    // add an item & colour box in the list for the ROI 
    addNewRoiToList(rCur,newId);

}

// add a new row in the ROI list
void ROIWindow::addNewRoiToList(ROI *rCur,int newId)
{
    // add row to listview control
    LVITEM item;
    item.mask=LVIF_TEXT|LVIF_PARAM;
    item.iItem=newId;
    item.iSubItem=0;
    item.pszText=copyString((rCur->get_name()).c_str());
    item.lParam=(LPARAM)rCur;
    ListView_InsertItem(hROIListBox,&item);    

	// get coords of new row in list box
	RECT rect;
	ListView_GetItemRect(hROIListBox,newId,&rect,LVIR_BOUNDS);
	int rowHeight=rect.bottom-rect.top;
	int boxSize=rowHeight-2;
	
    // create colour button for the ROI
    HWND hColourButton = CreateWindowEx(
                        0, "BUTTON",
                        NULL,
                		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW,
                        210-20, rect.top+1,
                        20, boxSize, hROIListBox, (HMENU)NULL,
                		Window::GetAppInstance(), NULL);

    roiColourButtonList.push_back(hColourButton);    // add the ROI colour button to the list	

	// get width & height of scrollbox client area
	GetClientRect(ROIscrollBox.GetHandle(),&rect);
	
	// re-size list view control to show all rows inside scroll box
	int newListViewHeight=(roiColourButtonList.size()*rowHeight)+5;
	if (newListViewHeight<rect.bottom-4)
		newListViewHeight=rect.bottom-4;
	MoveWindow(hROIListBox,0,0,rect.right-rect.left,newListViewHeight,true);
}

void ROIWindow::loadROI (ROIWindow* win) {
    OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";
    
    ZeroMemory(&ofn, sizeof(ofn));
    
    // open a dialog box to ask for the filename to load
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = win->GetHandle();//overviewWindow.GetHandle();
    ofn.lpstrFilter =  "All Supported Files\0*.roi;*.txt\0ROI Files (*.roi)\0*.roi\0Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "txt";
    
    
    if(GetOpenFileName(&ofn)) {
        // get a string version of the filename
		string fn_str (ofn.lpstrFile);
		
		// load the set from the file and combine it with the current set
        ROIFile *rf = new ROIFile();
        ROISet* rs = rf->loadSetFromFile(fn_str);
        regionsSet->combine(rs, true);
        
        // update the list of checkboxes with the new regions that were loaded
        win->updateROIList(win);
        
        // update the display
        imageWindow.Repaint();
    }
}


void ROIWindow::saveROI (ROIWindow* win) {
    OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";
    
    ZeroMemory(&ofn, sizeof(ofn));
    
    // open a dialog box to ask for the filename to load
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = win->GetHandle();//overviewWindow.GetHandle();
    ofn.lpstrFilter =  "ROI File (*.roi)\0*.roi\0Text File (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = "txt";
    
    
    if(GetSaveFileName(&ofn)) {
        // get a string version of the filename
		string fn_str (ofn.lpstrFile);
		
        // save all the ROIs
        ROIFile *rf = new ROIFile();
        rf->saveSetToFile(fn_str, regionsSet);
    }
}


void ROIWindow::newEntity (ROIWindow* win, const char* roiType) {
    string name = getSelectedItemText();
    
    // if no ROIs are selected then create a new one
    if (name == "") {
        win->newROI(win, roiType);
        
    // if an ROI is selected then create a new entity for it
    } else {
        regionsSet->set_current(name);
        regionsSet->new_entity(roiType);
    }
}


void ROIWindow::deleteROI (ROIWindow* win) {
    // get the index of the selected item in the ROI list
    int i = getSelectedItemIndex();

	RECT rect;
	ListView_GetItemRect(hROIListBox,1,&rect,LVIR_BOUNDS);
	int boxSize=rect.bottom-rect.top;
    
    if (i != -1) {
        // get the name of the selected item in the ROI list
        string name = getSelectedItemText();
        
        // remove the item from the list
        SendMessage(hROIListBox, LVM_DELETEITEM, i, 0);
        
        // remove the ROI colour picker
        HWND colBox = roiColourButtonList.at(i);
        DestroyWindow(colBox);
        roiColourButtonList.erase(roiColourButtonList.begin()+i, roiColourButtonList.begin()+i+1);
        
        // move all the colour boxes after the deleted item up one position
        for (int j=i; j<roiColourButtonList.size(); j++) {
            colBox = roiColourButtonList.at(j);
            MoveWindow(colBox, 190, 1+j*17, 20, 15, true);
            
            // better way of doing it - but doesn't currently work
            //GetWindowRect(colBox,&rect);
            //MoveWindow(colBox, 210-20, rect.top-boxSize+1, rect.right-rect.left, rect.bottom-rect.top, true);
        }

		// get height of each row
		ListView_GetItemRect(hROIListBox,1,&rect,LVIR_BOUNDS);
		int rowHeight=rect.bottom-rect.top;

		// get width & height of scrollbox client area
		GetClientRect(ROIscrollBox.GetHandle(),&rect);
		
		// re-size list view control to show all rows inside scroll box
		int newListViewHeight=(roiColourButtonList.size()*rowHeight)+5;
		if (newListViewHeight<rect.bottom-4)
			newListViewHeight=rect.bottom-4;
		MoveWindow(hROIListBox,0,0,rect.right-rect.left,newListViewHeight,true);

        
        // remove the ROI from the set
        regionsSet->remove_region(name);
        
        // update the display
        imageWindow.Repaint();
    }
}


void ROIWindow::updateButtons (ROIWindow* win) {
    bool stat;
    
    if (regionsSet->editing()) stat = false;
    else stat = true;
    
    // turn off ROI add/remove buttons
    EnableWindow(win->hNewButton, stat);
    EnableWindow(win->hDeleteButton, stat);
    
    // turn off entity creation buttons
    EnableWindow(win->hPolyButton, stat);
    EnableWindow(win->hRectButton, stat);
    EnableWindow(win->hSingleButton, stat);
}


void ROIWindow::updateROIList (ROIWindow* win) {
    vector<ROI*> rlist = regionsSet->get_regions();
    
    // remove all the original ROI checkboxes from the display
    for (int i=0; i<roiColourButtonList.size(); i++) {
        HWND cur = roiColourButtonList.at(i);
        DestroyWindow(cur);
    }
    
    // remove all the original ROI checkboxes from the list
    roiColourButtonList.clear();
    ListView_DeleteAllItems(hROIListBox);
    
    // add the checkboxes based on the new list
    for (int i=0; i<rlist.size(); i++) {
        ROI* roi = rlist.at(i);
        
        // create list entry & colour box for the ROI
		addNewRoiToList(roi,i);
    }
    
    // re-paint windows to draw ROIs
    imageWindow.Repaint();
    roiWindow.Repaint();
}
