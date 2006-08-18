
#include <Windows.h>
#include <Commctrl.h>
#include <iostream>
#include <cassert>
#include "config.h"
#include "window.h"

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

#include "RoI.h"

using namespace std;

ImageHandler::ImageHandler* image_handler = NULL;	// Instance handle ptr

char szStaticControl[] = "static";  /* classname of static text control */

HINSTANCE hThisInstance;            /* a handle that identifies our process */

HWND hDesktop;                      /* handle to desktop window (used for snapping) */

settings settingsFile;              /* Used for loading and saving window position and sizes */

MainWindow mainWindow;
ToolWindow toolWindow;
OverviewWindow overviewWindow;

char *filename=NULL;                    // currently open image filename

char *modulePath=NULL;                  /* used to store the path to this executable's directory */

void GetModulePath()
{
    //Get full path of exe
    int moduleSize=256;
    int lasterror=0;
    int result;
    do
    {
        modulePath=new char[moduleSize];                 
        result=GetModuleFileName(NULL, (LPTSTR)modulePath, moduleSize);
        // check if whole string was copied succesfully
        if ((result>0)&&(result<moduleSize))
            break;
        delete(modulePath);
        modulePath=NULL;
        moduleSize*=2;
    } while (result!=0);   // if no error occured, try again

    // "remove" the process filename from the string
    if (modulePath!=0)
    {
        char *p;
        for (p=modulePath+strlen(modulePath)-1;(p>=modulePath)&&(*p!='\\');p--);
        *p=0;
    }
    
    // this line was used for testing the current working path, remove it if no more testing on this is being done
	//MessageBox(0,catcstrings( (char*) "The install folder is: ", (char*) modulePath),"Parbat3D Error",MB_OK);                         
}

/* program entry point */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)
{
 	 // Set global variable for application directory string
    GetModulePath();
    
    string settings_path (catcstrings(modulePath, "\\settings.ini"));
    settingsFile.open(settings_path);
    
    MSG messages;
    
    
    InitCommonControls();           /* load window classes for common controls */    
    
    hThisInstance=hInstance;        /* record this process's instance handle */
    hDesktop=GetDesktopWindow();    /* record handle to desktop window */    

    
    Console::open();
    CONSOLE_SCREEN_BUFFER_INFO myInfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &myInfo);
    int consoleColumns = myInfo.dwSize.X;
    int consoleRows = myInfo.dwSize.Y;
    Console::write("Console columns: ");
    Console::write((char*)inttocstring(consoleColumns));
    Console::write("\n");
    Console::write("Console rows: ");
	Console::write((char*)inttocstring(consoleRows));
	Console::write("\n");
/*
    coords c1,c2,c3,c4,c;
    RoI roi;
    RoIEntity en1,en2;
   
    c1.x=1;
    c1.y=2;
    c2.x=3;
    c2.y=4;
    c3.x=5;
    c3.y=6;
    c4.x=7;
    c4.y=8;
    en1.points.push_back(c1);
    en1.points.push_back(c3);    
    en2.points.push_back(c2);
    en2.points.push_back(c4);    
    roi.add_entity(&en1);
    roi.add_entity(&en2);
    vector<RoIEntity*> vres=roi.get_entities();
    for (int i=0;i<vres.size();i++)
    {
        Console::write("entity ");
        Console::write(i);      
        Console::write(":\n");        
        RoIEntity* en=(RoIEntity*)vres.at(i);
        for (int j=0;j<en->points.size();j++)
        {
            Console::write(" x=");
            Console::write(en->points.at(j).x);
            Console::write(" y=");
            Console::write(en->points.at(j).y);        
            Console::write("\n");        
       }            
    }*/
    

    
	if (!mainWindow.Create(hInstance))
    {
        MessageBox(0,"Unable to create main window","Parbat3D Error",MB_OK);
        return 0;
    }
  
    /* Register window classes */
    if ((!ImageWindow::registerImageWindow()))
    {
        /* report error if window classes could not be registered */
        MessageBox(0,"Unable to register window class","Parbat3D Error",MB_OK);
        return 0;
    }
        
    /* Setup main & image windows */
    //  note: image window must be created before main window
    //  note: tool window is only created when an image is loaded

    if ((!ImageWindow::setupImageWindow()) || (!overviewWindow.Create(hThisInstance)))
    {
        /* report error if windows could not be setup (note: unlikely to happen) */
        MessageBox(0,"Unable to create window","Parbat3D Error",MB_OK);
        return 0;
    }
  
  	/*Window pwin;
  	TestWindow win;
  	pwin.Create(hInstance);
  	pwin.Destroy();  	
  	if (pwin.GetHandle()==NULL)
  	{
     	pwin.Create(hInstance);
    }
    win.Create(hInstance,pwin.GetHandle());
  	
//	win.CreateC(hInstance,pwin.GetHandle());*/
  
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
    SetWindowPos(ImageWindow::hImageWindow,toolWindow.GetHandle(),0,0,0,0,SWP_NOMOVE+SWP_NOSIZE+SWP_NOACTIVATE+SWP_NOSENDCHANGING);        
    SetWindowPos(ImageWindow::hImageWindow,overviewWindow.GetHandle(),0,0,0,0,SWP_NOMOVE+SWP_NOSIZE+SWP_NOACTIVATE+SWP_NOSENDCHANGING);        


}    

void loadFile()
{
    OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = overviewWindow.GetHandle();
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
	    image_handler = new ImageHandler::ImageHandler(overviewWindow.overviewWindowDisplay.GetHandle(), ImageWindow::imageWindowDisplay.GetHandle(), ofn.lpstrFile);
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
                    filename=copyString(image_handler->get_image_properties()->getFileName());
                    
                    ImageWindow::updateImageWindowTitle();              
                    ImageWindow::updateImageScrollbar();      

                    // re-create tool window
                    toolWindow.Create(hThisInstance);
                    
                    // show tool & image windows
                    ShowWindow(toolWindow.GetHandle(),SW_SHOW);
                    ShowWindow(ImageWindow::hImageWindow,SW_SHOW);    
                    orderWindows();                    

                    // update opengl displays
                    RedrawWindow(overviewWindow.overviewWindowDisplay.GetHandle(),NULL,NULL,RDW_INTERNALPAINT);
                    RedrawWindow(ImageWindow::imageWindowDisplay.GetHandle(),NULL,NULL,RDW_INTERNALPAINT);                

                    // enable window menu items
                    EnableMenuItem(overviewWindow.hMainMenu,IDM_IMAGEWINDOW,false);
                    EnableMenuItem(overviewWindow.hMainMenu,IDM_TOOLSWINDOW,false);
                    EnableMenuItem(overviewWindow.hMainMenu,IDM_FILECLOSE,false);
                            
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
    /* deallocate variables */
    if (filename!=NULL) delete(filename);
    filename=NULL;
    
	if (image_handler) delete image_handler;
    image_handler=NULL;
	
	/* destroy tool window */
    if (toolWindow.GetHandle()!=NULL)
    {
        toolWindow.Destroy();
    }    
    
    /* hide image window */
    if (ImageWindow::hImageWindow)
    {
        ShowWindow(ImageWindow::hImageWindow,SW_HIDE);
    }

    /* disable menu items */
    EnableMenuItem(overviewWindow.hMainMenu,IDM_IMAGEWINDOW,true);
    EnableMenuItem(overviewWindow.hMainMenu,IDM_TOOLSWINDOW,true);
    EnableMenuItem(overviewWindow.hMainMenu,IDM_FILECLOSE,true);    
    
    /* repaint main window */
    overviewWindow.overviewWindowDisplay.Repaint();
}
