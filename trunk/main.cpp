#include "config.h"
#include <Windows.h>
#include <Commctrl.h>
#include <iostream>
#include <cassert>
#include "window.h"

#include "ImageProperties.h"
#include "BandInfo.h"
#include "StringUtils.h"
#include "console.h"

#include "main.h"

#include "MainWindow.h"
#include "ImageWindow.h"
#include "OverviewWindow.h"
#include "ToolWindow.h"
#include "DisplayWindow.h"
#include "ROIWindow.h"
#include "PrefsWindow.h"
#include "ContrastWindow.h"
#include "ContrastAdvWindow.h"
#include "ProgressWindow.h"

#include "ROISet.h"

using namespace std;

ImageHandler::ImageHandler* image_handler = NULL;	// Instance handle ptr

char szStaticControl[] = "static";  /* classname of static text control */

HINSTANCE hThisInstance;            /* a handle that identifies our process */

HWND hDesktop;                      /* handle to desktop window (used for snapping) */

settings* settingsFile;             /* Used for loading and saving window position and sizes */

ROISet *regionsSet;

MainWindow mainWindow;
ToolWindow toolWindow;
OverviewWindow overviewWindow;
ImageWindow imageWindow;
ROIWindow roiWindow;
PrefsWindow prefsWindow;
ContrastWindow contrastWindow;
ContrastAdvWindow contrastAdvWindow;
ProgressWindow progressWindow;

StickyWindowManager stickyWindowManager;
vector<FeatureSpace*> featureSpaceWindows;

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
    
    hThisInstance=hInstance;
    Window::Init(hInstance);
    
    //string settings_path (catcstrings(modulePath, "\\settings.ini"));  // store in build folder
    
    // create the directory in Application Data if it doesn't already exist
    char* team_dir = copyString(catcstrings(copyString(getenv("APPDATA")), "\\Imagery"));
    CreateDirectory(team_dir, NULL);
    char* prog_dir = copyString(catcstrings(team_dir, "\\Parbat3D"));
    CreateDirectory(prog_dir, NULL);
    
    string settings_path (catcstrings(prog_dir, "\\settings.ini"));  // store in App Data folder
    settingsFile = new settings(settings_path);
    
    regionsSet = new ROISet();
    
    MSG messages;
    
    INITCOMMONCONTROLSEX icc;
    icc.dwSize=sizeof(INITCOMMONCONTROLSEX);
    icc.dwICC=ICC_BAR_CLASSES|ICC_COOL_CLASSES;
    InitCommonControlsEx(&icc);
    
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

    
    if (!mainWindow.Create())
    {
        MessageBox(0,"Unable to create main window","Parbat3D Error",MB_OK);
        return 0;
    }
    
    /* Setup main & image windows */
    //  note: image window must be created before overview window
    if ((!imageWindow.Create(mainWindow.GetHandle())) ||
        (!overviewWindow.Create(imageWindow.GetHandle())) ||
        (!prefsWindow.Create(imageWindow.GetHandle())) ||
        (!contrastAdvWindow.Create(imageWindow.GetHandle())) ||
        (!roiWindow.Create(imageWindow.GetHandle())))
    {
        /* report error if windows could not be setup (note: unlikely to happen) */
        MessageBox(0,"Unable to create window","Parbat3D Error",MB_OK);
        return 0;
    }
    
    progressWindow.Create();
       
    /* make windows to stick to each other & move around with overview window */
    stickyWindowManager.SetController(&overviewWindow);    
    stickyWindowManager.AddStickyWindow(&imageWindow);
    stickyWindowManager.AddStickyWindow(&prefsWindow);
    stickyWindowManager.AddStickyWindow(&contrastWindow);
    stickyWindowManager.AddStickyWindow(&contrastAdvWindow);
    stickyWindowManager.AddStickyWindow(&roiWindow);
    
    
    // setup the keyboard shortcuts
    
    Window::setDefaultKeyboardShortcuts();  // set the default keyboard shortcuts first
    
    int keyNew;         // new key for the shortcut
    string keyNameStr;  // name of the shortcut
    for (int i=0; i<Window::NUMBER_OF_SHORTCUTS; i++) {
        keyNameStr = makeString("", i);          // create the name of this shortcut
        keyNew = settingsFile->getSettingi("keyboard shortcuts", keyNameStr, Window::getKeyboardShortcut(i));
        Window::setKeyboardShortcut(i, keyNew);  // set the shortcut (whether or not it has a new value)
    }
    
    
    /* Execute the message loop. It will run until GetMessage( ) returns 0 */
    while(GetMessage(&messages, NULL, 0, 0))
    {
        /* Translate keyboard events */
        TranslateMessage(&messages);
        /* Send message to the associated window procedure */
        DispatchMessage(&messages);
    }

	#if TMP_WAIT_FOR_KEYPRESS_ON_CLOSE
	Console::write("\nProgram is about to terminate. Press enter to close window...\n");
	Console::waitForEnter();
	#endif

    /* End program */
    return messages.wParam;
}


void emptyOutMessageQueue()
{
	MSG messages;
	
     /* Execute the message loop for every message that is waiting to be processed */
    while(PeekMessage(&messages, NULL, 0, 0, PM_REMOVE))
    {
        /* Translate keyboard events */
        //TranslateMessage(&messages);
        /* Send message to the associated window procedure */
        //DispatchMessage(&messages);
        DefWindowProc(messages.hwnd,messages.message,messages.lParam,messages.wParam);
    }
}


void orderWindows()
{
    SetWindowPos(imageWindow.GetHandle(),toolWindow.GetHandle(),0,0,0,0,SWP_NOMOVE+SWP_NOSIZE+SWP_NOACTIVATE+SWP_NOSENDCHANGING);        
    SetWindowPos(imageWindow.GetHandle(),overviewWindow.GetHandle(),0,0,0,0,SWP_NOMOVE+SWP_NOSIZE+SWP_NOACTIVATE+SWP_NOSENDCHANGING);        
    SetWindowPos(imageWindow.GetHandle(),roiWindow.GetHandle(),0,0,0,0,SWP_NOMOVE+SWP_NOSIZE+SWP_NOACTIVATE+SWP_NOSENDCHANGING);
	//SetWindowPos(imageWindow.GetHandle(),contrastWindow.GetHandle(),0,0,0,0,SWP_NOMOVE+SWP_NOSIZE+SWP_NOACTIVATE+SWP_NOSENDCHANGING);        
}    

void loadFile() {
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
    
    
    if(GetOpenFileName(&ofn)) {
        // Clean up any previous instance
        closeFile();	
        
        // starts the progress bar (shows the window)
        progressWindow.start(100,true);
        
        // load image & setup windows
        int ih_status;
        assert(image_handler == NULL);
	    image_handler = new ImageHandler::ImageHandler(overviewWindow.overviewWindowDisplay.GetHandle(), imageWindow.imageWindowDisplay.GetHandle(), ofn.lpstrFile, regionsSet);
	    assert(image_handler != NULL);
	    ih_status = image_handler->get_status();
		if (ih_status > 0) {
		   // An error occurred instantiaing the image handler class.
		   MessageBox (NULL, image_handler->get_status_text() , "[error] Parbat3D :: ImageHandler", 0);
		   /*  !! Clean-up, when this returns we should be in 'clean' state,
               identical to initial startup */
               delete image_handler;
               image_handler = NULL;
		   return;
		} else if (ih_status < 0) { // Error occurred, but was non-fatal
		   MessageBox (NULL, image_handler->get_status_text() , "[warning] Parbat3D :: ImageHandler", 0);
		}
    	
        // update image window settings
        filename=copyString(image_handler->get_image_properties()->getFileName());
                    
        imageWindow.updateImageWindowTitle();              
        imageWindow.updateImageScrollbar();      

        // re-create tool & image window
        toolWindow.Create(imageWindow.GetHandle());
        roiWindow.Create(imageWindow.GetHandle());
        contrastWindow.Create(imageWindow.GetHandle());
        
        
        // show tool & image windows
        toolWindow.Show();
        imageWindow.Show();
        if (atoi(settingsFile->getSetting("roi window", "open").c_str()) == 1)
            roiWindow.Show();
        
        // update opengl displays
        overviewWindow.Repaint();
        imageWindow.Repaint();
        
        
        // enable window menu items
        EnableMenuItem(overviewWindow.hMainMenu,IDM_IMAGEWINDOW,false);
        EnableMenuItem(overviewWindow.hMainMenu,IDM_TOOLSWINDOW,false);
        EnableMenuItem(overviewWindow.hMainMenu,IDM_ROIWINDOW,false);
        EnableMenuItem(overviewWindow.hMainMenu,IDM_FILECLOSE,false);
        EnableMenuItem(overviewWindow.hMainMenu,IDM_CONTSWINDOW,false);
        
        
        // stops the progress bar (hides the window)
        progressWindow.end();
    }
}

void closeFile() {
    // deallocate variables
    Console::write("closeFile()...\n");
    
    Console::write("deleting filename...\n");    
	if (filename!=NULL)
        delete(filename);
    filename=NULL;
    
    Console::write("deleting image handler...\n");    
	if (image_handler)
        delete image_handler;
    image_handler=NULL;
    
    // destroy tool window
    Console::write("destroying tool window... toolwindow handle=%d\n",toolWindow.GetHandle());
    if (toolWindow.GetHandle()!=NULL)
        toolWindow.Destroy();
    
    // hide image window
    Console::write("hiding image window...\n");
    if (imageWindow.GetHandle()!=NULL)
        imageWindow.Hide();
    
    // destroy the roi window
    Console::write("destroying roi window & deleting ROIs...\n");
    if (roiWindow.GetHandle()!=NULL) {
        roiWindow.Destroy();       // roiWindow.Hide(); seems to cause the window to alternate save positions
        roiWindow.deleteAllROI();  // remove all the ROI from the list
    }
    
    Console::write("disabling menu items...\n");	
    
    // disable menu items
    EnableMenuItem(overviewWindow.hMainMenu,IDM_IMAGEWINDOW,true);
    EnableMenuItem(overviewWindow.hMainMenu,IDM_TOOLSWINDOW,true);
    EnableMenuItem(overviewWindow.hMainMenu,IDM_ROIWINDOW,true);
    EnableMenuItem(overviewWindow.hMainMenu,IDM_FILECLOSE,true);    
    EnableMenuItem(overviewWindow.hMainMenu,IDM_CONTSWINDOW,true);    
    
    
    // repaint main window
    overviewWindow.overviewWindowDisplay.Repaint();
    
    Console::write("closeFile() done\n");
}
