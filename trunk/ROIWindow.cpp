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

vector<HWND> ROIWindow::roiCheckboxList;

WNDPROC ROIWindow::prevListViewProc;

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
                                WS_CHILD | LVS_REPORT | LVS_EDITLABELS | WS_VISIBLE | LVS_NOCOLUMNHEADER | LVS_SHOWSELALWAYS, 
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
	lvc.cx = rect.right-25;     // width of column in pixels
	lvc.fmt = LVCFMT_CENTER; 
	ListView_InsertColumn(hROIListBox, 0, &lvc);

	lvc.iSubItem = 1;
	lvc.pszText = "Colour";
	lvc.cx = 20;     // width of column in pixels
	lvc.fmt = LVCFMT_CENTER; 
	ListView_InsertColumn(hROIListBox,2, &lvc);





	/* colour dialog box test
	{
		CHOOSECOLOR cc;
		COLORREF customColours[16];		// todo: give these inital values
		cc.lStructSize=sizeof(CHOOSECOLOR);
		cc.hwndOwner=toolWindow.GetHandle();
		cc.hInstance=NULL;
		cc.rgbResult=0xFF0000;				// inital colour (format: 0xBBGGRR)
		cc.lpCustColors=customColours;	// pointer to array of 16 custom colours
		cc.Flags=CC_ANYCOLOR|CC_FULLOPEN|CC_RGBINIT|CC_SOLIDCOLOR;
		cc.lCustData=0;
		cc.lpfnHook=NULL;
		cc.lpTemplateName=NULL;
		ChooseColor(&cc);				// returns nonzero if user clicked OK, otherwise zero
										// new colour is stored in cc.rgbResult
	} */

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

/* handle drawing the colour buttons within the list view control */
LRESULT CALLBACK ROIWindow::ROIListViewProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HBRUSH hbrush;
	DRAWITEMSTRUCT *di;
	RECT rect;
	ROI *roi;
	int colour;
	int i;
	
	switch (message)
	{
		case WM_DRAWITEM:
			for (i=0;i<roiCheckboxList.size();i++)
			{
				if (di->hwndItem==roiCheckboxList.at(i))
				{
					roi=regionsSet->get_regions().at(i); //900-di->itemID
					roi->get_color(&colour,&colour+1,&colour+2);
					hbrush=CreateSolidBrush(colour);
					di=(LPDRAWITEMSTRUCT) lParam;
					SelectObject(di->hDC,hbrush);
					GetClientRect(di->hwndItem,&rect);
					Rectangle(di->hDC,rect.left,rect.top,rect.right,rect.bottom);
					DeleteObject(hbrush);
					break;
				}
			}
			break;		
	}
    return CallWindowProc(prevListViewProc,hwnd,message,wParam,lParam);	
}

/* All messages/events related to the ROI window (or it's controls) are sent to this procedure */
LRESULT CALLBACK ROIWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static POINT moveMouseOffset;     /* mouse offset relative to window, used for snapping */
    //static POINT sizeMousePosition;   /* mouse position, used for sizing window */
    //static RECT  sizeWindowPosition;  /* window position, used for sizing window */
    static RECT rect;                 /* for general use */

    ROIWindow* win=(ROIWindow*)Window::GetWindowObject(hwnd);
    
    switch (message)                  /* handle the messages */
    {
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
            return 0;

        /* WM_DESTORY: system is destroying our window */
        case WM_DESTROY:
            CheckMenuItem(overviewWindow.hMainMenu,IDM_ROIWINDOW,MF_UNCHECKED|MF_BYCOMMAND);            
            break;
    }
    // call next window procedure in chain
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}



int ROIWindow::getROICheckedCount () {
    int i;
    int listSize = roiCheckboxList.size();
    int checked = 0;
    
    // find how many ROIs are selected
    for (i=0; i<listSize; i++) {
        HWND hCur = roiCheckboxList.at(i);
        if ((SendMessageA(hCur, BM_GETCHECK, 0, 0)) == BST_CHECKED)
            checked++;
    }
    
    return checked;
}


void ROIWindow::newROI (ROIWindow* win, const char* roiType) {
    int listSize = regionsSet->get_regions_count();
    
    // create ROI with the default name
    ROI *rCur = regionsSet->new_region();
    if (roiType != NULL)
        regionsSet->new_entity(roiType);
    
    // create checkbox for the ROI
    HWND hROITick = CreateWindowEx(
                        WS_EX_CLIENTEDGE, "BUTTON",
                        NULL,
                		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_OWNERDRAW,
                        2+170, 0 + (20 * listSize),
                        20, 20, hROIListBox, (HMENU)900+listSize,
                		Window::GetAppInstance(), NULL);
	SetBackgroundBrush(hROITick,CreateSolidBrush(0));
    roiCheckboxList.push_back(hROITick);    // add the ROI checkbox to the list
    
    // add row to listview control
    LVITEM item;
    item.mask=LVIF_TEXT;
    item.iItem=listSize;
    item.iSubItem=0;
    item.pszText=copyString((rCur->get_name()).c_str());
    ListView_InsertItem(hROIListBox,&item);    
	// set ROI name
    item.mask=LVIF_TEXT;
    item.iItem=listSize;
    item.iSubItem=0;
    item.pszText=copyString((rCur->get_name()).c_str());
//    ListView_SetItem(hROIListBox,&item);
}


void ROIWindow::loadROI (ROIWindow* win) {
    OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";
    
    ZeroMemory(&ofn, sizeof(ofn));
    
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
		
		Console::write("ROI file opened: ");
		Console::write(&fn_str);
		Console::write("\n");
		
		// load the set from the file and combine it with the current set
        ROIFile *rf = new ROIFile();
        ROISet* rs = rf->loadSetFromFile(fn_str);
        regionsSet->combine(rs, true);
        
        // update the list of checkboxes with the new regions that were loaded
        win->updateROIList(win);
    }
}


void ROIWindow::saveROI (ROIWindow* win) {
    int checked = win->getROICheckedCount();
    
    OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";
    
    ZeroMemory(&ofn, sizeof(ofn));
    
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
		
		Console::write("ROI file to save: ");
		Console::write(&fn_str);
		Console::write("\n");
		
        // save only the selected ROIs
        if (checked>0) {
            
        // save all the ROIs
        } else {
            ROIFile *rf = new ROIFile();
            rf->saveSetToFile(fn_str, regionsSet);
        }
    }
}


void ROIWindow::newEntity (ROIWindow* win, const char* roiType) {
    int items_in_list = SendMessage(hROIListBox, LVM_GETITEMCOUNT, 0, 0);
    int selected = -1;
    char buffer[256];
    string *name;
    
    for (int i=0; i<items_in_list; i++) {
        if (SendMessage(hROIListBox, LVM_GETITEMSTATE, i, LVIS_SELECTED) == LVIS_SELECTED) {
            selected = i;
            
            LVITEM lvitem;
            lvitem.mask = LVIF_TEXT;
            lvitem.iItem = selected;
            lvitem.pszText = buffer;
            lvitem.cchTextMax = 256;
            lvitem.iSubItem = 0;
            
            ListView_GetItem(hROIListBox, &lvitem);
            name = new string(buffer);
        }
    }
    
    // if no ROIs are selected then create a new one
    if (selected == -1) {
        win->newROI(win, roiType);
        
    // if an ROI is selected then create a new entity for it
    } else {
        regionsSet->set_current(*name);
        regionsSet->new_entity(roiType);
    }
}


void ROIWindow::deleteROI (ROIWindow* win) {
    int checked = win->getROICheckedCount();
    
    // if no ROIs are selected then create a new one
    if (checked >= 1) {
        //MessageBox(NULL, (LPSTR) "Delete it", (LPSTR) "Action", MB_ICONINFORMATION | MB_OK );
        
        // ** delete procedure
        // loop through roiCheckboxList
        //  check each to see if checked
        //  use name to remove it from regionsSet
        //  remove it from roiCheckboxList
        //  reduce the y position of all HWNDs after it
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
    for (int i=0; i<roiCheckboxList.size(); i++) {
        HWND cur = roiCheckboxList.at(i);
        DestroyWindow(cur);
    }
    
    // remove all the original ROI checkboxes from the list
    roiCheckboxList.clear();
    
    // add the checkboxes based on the new list
    for (int i=0; i<rlist.size(); i++) {
        ROI* roi = rlist.at(i);
        
        // create checkbox for the ROI
        HWND hROITick = CreateWindowEx(
                            0, "BUTTON",
                            copyString((roi->get_name()).c_str()),
                    		BS_AUTOCHECKBOX | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
                            10, 10 + (20 * i),
                            100, 16, ROIscrollBox.GetHandle(), NULL,
                    		Window::GetAppInstance(), NULL);
        roiCheckboxList.push_back(hROITick);    // add the ROI checkbox to the list
    }
}
