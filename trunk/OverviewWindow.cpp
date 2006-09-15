#include <Windows.h>
#include "main.h"
#include "OverviewWindow.h"
#include "ImageWindow.h"
#include "ToolWindow.h"
#include "DisplayWindow.h"
#include "MainWindow.h"
#include "console.h"
#include "ROIWindow.h"
#include "PrefsWindow.h"
#include "ContrastWindow.h"


// Used for accessing the help folder
const char *helpPath;
bool advanced;
DisplayWindow overviewWindowDisplay;

/* setup overview window */
int OverviewWindow::Create(HWND parent)
{

    RECT rect;
    
    /* get co-ords of image window */
    GetWindowRect(imageWindow.GetHandle(),&rect);
        
    /* Create overview window */
    if (!CreateWin(WS_EX_APPWINDOW, "Parbat3D Overview Window", "Parbat3D",
		WS_OVERLAPPED+WS_CAPTION+WS_SYSMENU+WS_MINIMIZEBOX, rect.left-OverviewWindow::OVERVIEW_WINDOW_WIDTH, rect.top, OVERVIEW_WINDOW_WIDTH, 296,
		parent, NULL))
        return false;
    prevProc=SetWindowProcedure(&OverviewWindow::WindowProcedure);

    /* set menu to main windo */
	hMainMenu = LoadMenu(hThisInstance, MAKEINTRESOURCE(ID_MENU));
    SetMenu(GetHandle(), hMainMenu);
    
    /* Disable Window menu items. note: true appears to disable & false enables */
    EnableMenuItem(hMainMenu,IDM_IMAGEWINDOW,true);
    EnableMenuItem(hMainMenu,IDM_TOOLSWINDOW,true);
    EnableMenuItem(hMainMenu,IDM_ROIWINDOW,true);
    EnableMenuItem(hMainMenu,IDM_FILECLOSE,true);

    /* create a child window that will be used by OpenGL */
    overviewWindowDisplay.Create(GetHandle());
   
    /* Make the window visible on the screen */
    Show();
    
    return true;
}

/* toggles a menu item between a checked & unchecked state */
/* returns true if new state is checked, else false if unchecked */
int OverviewWindow::toggleMenuItemTick(HMENU hMenu,int itemId)
{
    int menuState=GetMenuState(hMenu,itemId,MF_BYCOMMAND);
    if (menuState&MF_CHECKED)
    {
        CheckMenuItem(hMenu,itemId,MF_UNCHECKED|MF_BYCOMMAND);
        return false;
    }
    else
    {
        CheckMenuItem(hMenu,itemId,MF_CHECKED|MF_BYCOMMAND);
        return true;
    }
}
/*
void OverviewWindow::advancedYes()
{
		advanced = true;
}

void OverviewWindow::advancedNo()
{
		advanced = false;
}
*/
/* This function is called by the Windows function DispatchMessage( ) */
/* All messages/events related to the main window (or it's controls) are sent to this procedure */
LRESULT CALLBACK OverviewWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static RECT rect;                       /* for general use */
    
    OverviewWindow* win=(OverviewWindow*)Window::GetWindowObject(hwnd);
        
    switch (message)                  /* handle the messages */
    {
			
        /* WM_COMMAND: some command has been preformed by user, eg. menu item clicked */            
        case WM_COMMAND:
            switch( wParam )
            {
                case IDM_FILEOPEN:
                    loadFile();
                    return 0;
                  
                case IDM_FILESAVE:
                case IDM_FILESAVEAS:
					MessageBox( hwnd, (LPSTR) "This will be greyed out if no file open, else will say \"Where do you want to store such a big file?\"",
                              (LPSTR) "Overview Window",
                              MB_ICONINFORMATION | MB_OK );
                    return 0;
                
                case IDM_FILECLOSE:
                    closeFile();
                    return 0;                    
                          
             	case IDM_IMAGEWINDOW:
                    if (win->toggleMenuItemTick(win->hMainMenu,IDM_IMAGEWINDOW))
                        ShowWindow(imageWindow.GetHandle(),SW_SHOW);
                    else
                        ShowWindow(imageWindow.GetHandle(),SW_HIDE);
                    return 0;
             
                case IDM_TOOLSWINDOW:
                    if (win->toggleMenuItemTick(win->hMainMenu,IDM_TOOLSWINDOW))
                        toolWindow.Show();
                    else
                        toolWindow.Hide();
                    return 0;

                case IDM_ROIWINDOW:
                    if (win->toggleMenuItemTick(win->hMainMenu,IDM_ROIWINDOW))
                        roiWindow.Show();
                    else
                        roiWindow.Hide();
                    return 0;
                    
                case IDM_PREFSWINDOW:
                    if (win->toggleMenuItemTick(win->hMainMenu,IDM_PREFSWINDOW))
                        prefsWindow.Show();
                    else
                        prefsWindow.Hide();
                    return 0;

                case IDM_CONTSWINDOW:
                    if (win->toggleMenuItemTick(win->hMainMenu,IDM_CONTSWINDOW))
                        contrastWindow.Show();
                    else
                        contrastWindow.Hide();
                        contrastAdvWindow.Hide();
                    return 0;

                case IDM_HELPCONTENTS:
					 // get path to help folder
                     helpPath = catcstrings( (char*) modulePath, (char*) "\\help\\index.htm");

				     // launch default browser
                     ShellExecute(NULL, "open", helpPath, NULL, "help", SW_SHOWNORMAL);
                     return 0;

                case IDM_FILEEXIT:
                    SendMessage( hwnd, WM_CLOSE, 0, 0L );
                    return 0;

                case IDM_HELPABOUT:
                    MessageBox (NULL, "Created by Team Imagery, 2006" , "Parbat3D", 0);
                    return 0;
           }
           break;

        // WM_SIZE: the window has been re-sized, minimized, maximised or restored
        case WM_SIZE:
                
            /* resize display/opengl window to fit new size */
            GetClientRect(win->GetHandle(),&rect);
            MoveWindow(win->overviewWindowDisplay.GetHandle(),rect.left,rect.top,rect.right,rect.bottom,true);

            /* handle minimizing and restoring the child windows */            
		    switch(wParam)
		    {
                
                case SIZE_RESTORED:
                    // restore windows to their previous state 
                    mainWindow.RestoreAll();
                    return 0;

                 case SIZE_MINIMIZED:
                    // record whether windows are currently visible & then hide them all
                    mainWindow.MinimizeAll(); 
					return 0;
		    }    
  	        return 0;

                   
        /* WM_SYSCOMMAND: a system-related command associated with window needs to be executed */    
        case WM_SYSCOMMAND:
            /* check if user has tried to minimize the overview window */
            if (wParam==SC_MINIMIZE)
            {
                /* cause the main window to minimised instead */
                //ShowWindow(mainWindow.GetHandle(),SW_MINIMIZE);
                //return 0;
            }            
            break;
                     
        /* WM_CLOSE: system or user has requested to close the window/application */
        case WM_CLOSE:
            // Shut down the image file and OpenGL
			if (image_handler && settingsFile->getSettingi("preferences", "displayconfirmwindow", 1) == 1) { // Was instantiated
                if (MessageBox(hwnd,"Are you sure you wish to quit?\nAn image is currently open.","Parbat3D",MB_YESNO|MB_ICONQUESTION)!=IDYES)
                    return 0;
                closeFile();
			}
			
			/* destroy all the windows that have been created */
			/*
            if (imageWindow.GetHandle()!=NULL)        // image
                DestroyWindow(imageWindow.GetHandle());
            if (roiWindow.GetHandle()!=NULL)          // ROI
                DestroyWindow(roiWindow.GetHandle());
			if (prefsWindow.GetHandle()!=NULL)        // preferences
                DestroyWindow(prefsWindow.GetHandle());
            if (contrastWindow.GetHandle()!=NULL)     // contrast (normal)
                DestroyWindow(contrastWindow.GetHandle());
            if (contrastAdvWindow.GetHandle()!=NULL)  // contrast (advanced)
                DestroyWindow(contrastAdvWindow.GetHandle());
			*/
			
            /* destroy this window */
            DestroyWindow( hwnd );
            return 0;
            
        /* WM_DESTROY: window is being destroyed */
        case WM_DESTROY:
            /* Save the window location */
            RECT r;
            GetWindowRect(hwnd,&r);
            settingsFile->setSetting("overview window","x", r.left);
            settingsFile->setSetting("overview window","y", r.top);
            
            /* post a message that will cause WinMain to exit from the message loop */
            PostQuitMessage (0);
            break;
    }
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}

