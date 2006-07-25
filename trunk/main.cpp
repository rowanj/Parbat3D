
#include <Windows.h>
#include <Commctrl.h>
#include <iostream>
#include "config.h"

#include "ImageProperties.h"
#include "BandInfo.h"
#include "StringUtils.h"
#include "SnappingWindow.h"
#include "console.h"

#include "main.h"

#include "MainWindow.h"
#include "ImageWindow.h"
#include "OverviewWindow.h"
#include "ToolWindow.h"
#include "DisplayWindow.h"

using namespace std;

ImageHandler::ImageHandler* image_handler = NULL;	// Instance handle ptr

/* Unique class names for our main windows */

/* pre-defined class names for controls */
char szStaticControl[] = "static";  /* static text control */

/* a handle that identifies our process */
HINSTANCE hThisInstance;

/* global variables to store handles to our windows */
HWND hDesktop;

/* Used for loading and saving window position and sizes */
settings winPos ("settings.ini");


char *filename=NULL;                    // currently open image filename


/* program entry point */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)
{
 	MSG messages;     /* Here messages to the application is saved */
  

    InitCommonControls();           /* load window classes for common controls */    
    
    hThisInstance=hInstance;        /* record this process's instance handle */
    hDesktop=GetDesktopWindow();    /* record handle to desktop window */    
    
    Console::open();
    Console::write("testing1!\n");
    Console::write(new string("testing2!\n"));

    
    /* Register window classes */
    if ((!MainWindow::registerMainWindow()) || (!ToolWindow::registerToolWindow()) || (!ImageWindow::registerImageWindow()) || (!OverviewWindow::registerWindow()) || (!DisplayWindow::registerWindow()))
    {
        /* report error if window classes could not be registered */
        MessageBox(0,"Unable to register window class","Parbat3D Error",MB_OK);
        return 0;
    }
        
    /* Setup main & image windows */
    //  note: image window must be created before main window
    //  note: tool window is only setup when an image is loaded
    if ((!MainWindow::setupMainWindow()) || (!ImageWindow::setupImageWindow()) || (!OverviewWindow::setupWindow()))
    {
        /* report error if windows could not be setup (note: unlikely to happen) */
        MessageBox(0,"Unable to create window","Parbat3D Error",MB_OK);
        return 0;
    }
  
    /* Execute the message loop. It will run until GetMessage( ) returns 0 */
    while(GetMessage(&messages, NULL, 0, 0))
    {
        /* Send message to the associated window procedure */
        DispatchMessage(&messages);
    }

    /* End program */
    return messages.wParam;
}

void orderWindows()
{
    //SetWindowPos(hOverviewWindow,hToolWindow,0,0,0,0,SWP_NOMOVE+SWP_NOSIZE);        
    SetWindowPos(ImageWindow::hImageWindow,ToolWindow::hToolWindow,0,0,0,0,SWP_NOMOVE+SWP_NOSIZE+SWP_NOACTIVATE+SWP_NOSENDCHANGING);        
    SetWindowPos(ImageWindow::hImageWindow,OverviewWindow::hOverviewWindow,0,0,0,0,SWP_NOMOVE+SWP_NOSIZE+SWP_NOACTIVATE+SWP_NOSENDCHANGING);        


}    

void loadFile()
{
    OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW - which note?
    ofn.hwndOwner = OverviewWindow::hOverviewWindow;
    ofn.lpstrFilter =  "All Supported Images\0*.ecw;*.jpg;*.tif;*.j2k;*.jp2\0ERMapper Compressed Wavelets (*.ecw)\0*.ecw\0JPEG (*.jpg)\0*.jpg\0JPEG 2000 (*.j2k,*.jp2)\0*.j2k;*.jp2\0TIFF / GeoTIFF (*.tif)\0*.tif\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "txt";

    if(GetOpenFileName(&ofn))
    {
		// Clean up any previous instance
		closeFile();

        // load image & setup windows
	    image_handler = new ImageHandler::ImageHandler(OverviewWindow::hOverviewWindowDisplay, ImageWindow::hImageWindowDisplay, ofn.lpstrFile);
	    if (image_handler) {
			if (image_handler->status > 0) {
				// An error occurred instantiaing the image handler class.
				MessageBox (NULL, image_handler->error_text , "[error] Parbat3D :: ImageHandler", 0);
			} else {
				if (image_handler->status < 0) { // Error occurred, but was non-fatal
					MessageBox (NULL, image_handler->error_text , "[warning] Parbat3D :: ImageHandler", 0);
				}
    			else
    			{
                    // update image window settings
                    const char *newfilename=image_handler->get_image_properties()->getFileName();
                    int newfilename_length=strlen(newfilename);
                    if (filename!=NULL)
                       delete(filename);
                    filename=new char[newfilename_length+1];
                    strcpy(filename,newfilename);
                    
                    ImageWindow::updateImageWindowTitle();              
                    ImageWindow::updateImageScrollbar();      

                    // re-create tool window
                    ToolWindow::setupToolWindow();
                    
                    // show tool & image windows
                    ShowWindow(ToolWindow::hToolWindow,SW_SHOW);
                    ShowWindow(ImageWindow::hImageWindow,SW_SHOW);    
                    orderWindows();                    

                    // update opengl displays
                    RedrawWindow(OverviewWindow::hOverviewWindowDisplay,NULL,NULL,RDW_INTERNALPAINT);
                    RedrawWindow(ImageWindow::hImageWindowDisplay,NULL,NULL,RDW_INTERNALPAINT);                

                    // enable window menu items
                    EnableMenuItem(OverviewWindow::hMainMenu,IDM_IMAGEWINDOW,false);
                    EnableMenuItem(OverviewWindow::hMainMenu,IDM_TOOLSWINDOW,false);
                    EnableMenuItem(OverviewWindow::hMainMenu,IDM_FILECLOSE,false);
                            
                }				
			}

		} else { // Object wasn't created - this is probably terminal
			MessageBox (NULL, "[error] Could not instantiate ImageHandler." , "Parbat3D :: ImageHandler", 0);
			// !! Should probably die gracefully at this point - Rowan
		}
    }
}

void closeFile()
{
    
	if (image_handler) delete image_handler;
    image_handler=NULL;
	
    if (ToolWindow::hToolWindow)
    {
        DestroyWindow(ToolWindow::hToolWindow);
        ToolWindow::hToolWindow=NULL;
    }    
    
    if (ImageWindow::hImageWindow)
    {
        ShowWindow(ImageWindow::hImageWindow,SW_HIDE);
    }

    /* disable menu items */
    EnableMenuItem(OverviewWindow::hMainMenu,IDM_IMAGEWINDOW,true);
    EnableMenuItem(OverviewWindow::hMainMenu,IDM_TOOLSWINDOW,true);
    EnableMenuItem(OverviewWindow::hMainMenu,IDM_FILECLOSE,true);    
    
    InvalidateRect(OverviewWindow::hOverviewWindowDisplay,0,true);  /* repaint main window */		
    UpdateWindow(OverviewWindow::hOverviewWindowDisplay);
}
