#include <Windows.h>
#include <vector>

#include "console.h"
#include "main.h"
#include "OverviewWindow.h"
#include "MainWindow.h"
#include "DisplayWindow.h"
#include "ToolWindow.h"
#include "ImageHandler.h"
#include "ImageWindow.h"
#include "RoI.h"
#include "RoIFile.h"
#include "ROIWindow.h"
#include "Settings.h"
#include "ScrollBox.h"
#include "StringUtils.h"


ScrollBox ROIscrollBox;

vector<HWND> ROIWindow::roiCheckboxList;

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
	     WS_POPUP+WS_SYSMENU+WS_CAPTION,
	     rect.left, rect.bottom+30, 250, 300, parent, NULL))
	    return false;
	    
    prevProc=SetWindowProcedure(&WindowProcedure);
	
	RECT rect2;
   	rect2.top=5;
   	rect2.left=5;
   	rect2.right=235;
   	rect2.bottom=227;                 	
	ROIscrollBox.Create(GetHandle(),&rect2);

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
	
	
    //ROIscrollBox.UpdateScrollBar();
	
	return true;
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
                win->newROI(win, "");
            
            /* Delete ROI Button */
			if (LOWORD(wParam) == 4 && HIWORD(wParam) == BN_CLICKED)
                win->deleteROI(win);
            
            /* Create Polygon ROI Button */
			if (LOWORD(wParam) == 5 && HIWORD(wParam) == BN_CLICKED)
                win->newEntity(win, "POLYGON");
            
            /* Create Rectangle ROI Button */
			if (LOWORD(wParam) == 6 && HIWORD(wParam) == BN_CLICKED)
                win->newEntity(win, "RECT");
            
            /* Create Single Point ROI Button */
			if (LOWORD(wParam) == 7 && HIWORD(wParam) == BN_CLICKED) {
                win->newEntity(win, "POINT");
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


void ROIWindow::newROI (ROIWindow* win, string roiType) {
    // get unique name
    int listSize = roiCheckboxList.size();
    int i = listSize;
    
    string *name = new string(makeMessage("ROI-", listSize));
    while (regionsSet->name_exists(*name)) {
        i++;
        name = new string(makeMessage("ROI-", i));
    }
    
    // create ROI with the name
    RoI *rCur = regionsSet->new_region(*name);
    if (roiType != "")
        regionsSet->new_entity(roiType);
    
    // create checkbox for the ROI
    HWND hROITick = CreateWindowEx(
                        0, "BUTTON",
                        copyString(name->c_str()),
                		BS_AUTOCHECKBOX | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
                        10, 10 + (20 * listSize),
                        100, 16, ROIscrollBox.GetHandle(), NULL,
                		Window::GetAppInstance(), NULL);
    roiCheckboxList.push_back(hROITick);    // add the ROI checkbox to the list
}


void ROIWindow::loadROI (ROIWindow* win) {
    MessageBox(NULL, (LPSTR) "Open ROI", (LPSTR) "Action", MB_ICONINFORMATION | MB_OK );
}


void ROIWindow::saveROI (ROIWindow* win) {
    int checked = win->getROICheckedCount();
    
    if (checked>0) {
        MessageBox(NULL, (LPSTR) "Save Some", (LPSTR) "Action", MB_ICONINFORMATION | MB_OK );
    } else {
        MessageBox(NULL, (LPSTR) "Save All", (LPSTR) "Action", MB_ICONINFORMATION | MB_OK );
        RoIFile *rf = new RoIFile();
        rf->saveSetToFile("temp.roi", regionsSet);
    }
}


void ROIWindow::newEntity (ROIWindow* win, string roiType) {
    int i;
    int listSize = roiCheckboxList.size();
    int checked = win->getROICheckedCount();
    
    // if no ROIs are selected then create a new one
    if (checked == 0) {
        win->newROI(win, roiType);
        
    // if an ROI is selected then create a new entity for it
    } else if (checked == 1) {
        MessageBox(NULL, (LPSTR) "Create new entity", (LPSTR) "Action", MB_ICONINFORMATION | MB_OK );
    }
}


void ROIWindow::deleteROI (ROIWindow* win) {
    int checked = win->getROICheckedCount();
    
    // if no ROIs are selected then create a new one
    if (checked >= 1) {
        MessageBox(NULL, (LPSTR) "Delete it", (LPSTR) "Action", MB_ICONINFORMATION | MB_OK );
        
        // ** delete procedure
        // loop through roiCheckboxList
        //  check each to see if checked
        //  use name to remove it from regionsSet
        //  remove it from roiCheckboxList
        //  reduce the y position of all HWNDs after it
    }
}


void ROIWindow::updateButtons (ROIWindow* win) {
    // check if entity is being created
    if (regionsSet->editing()) {
        // turn off ROI add/remove buttons
        EnableWindow(win->hNewButton, false);
        EnableWindow(win->hDeleteButton, false);
        
        // turn off entity creation buttons
        EnableWindow(win->hPolyButton, false);
        EnableWindow(win->hRectButton, false);
        EnableWindow(win->hSingleButton, false);
    }
}
