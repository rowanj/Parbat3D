#include "PchApp.h"

#include "FeatureTab.h"

#include "config.h"

#include "Window.h"
#include "main.h"
#include "ScrollBox.h"
#include "ToolTab.h"
#include "ToolWindow.h"
#include "console.h"
#include "ImageProperties.h"

ScrollBox hFSScrollBox;


int FeatureTab::GetContainerHeight()
{
    return 0;
}

int FeatureTab::Create(HWND parent,RECT *parentRect)
{
    ToolTab::Create(parent,parentRect);
    prevProc=SetWindowProcedure(&WindowProcedure);
    
    RECT rect;
   	rect.top=5;
   	rect.left=25;
   	rect.right=218;
   	rect.bottom=138;                 	
	hFSScrollBox.Create(GetHandle(),&rect);

	hX = CreateWindowEx(0, "BUTTON", "X", WS_CHILD | BS_GROUPBOX | WS_VISIBLE, 118, 5,
		26, 20 + (20 * toolWindow.bands), hFSScrollBox.GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetFont(hX,FONT_BOLD);			
	hY = CreateWindowEx(0, "BUTTON", "Y", WS_CHILD | BS_GROUPBOX | WS_VISIBLE, 144, 5,
		26, 20 + (20 * toolWindow.bands), hFSScrollBox.GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetFont(hY,FONT_BOLD);					
    hZ = CreateWindowEx(0, "BUTTON", "Z", WS_CHILD | BS_GROUPBOX | WS_VISIBLE, 170, 5,
		26, 20 + (20 * toolWindow.bands), hFSScrollBox.GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetFont(hZ,FONT_BOLD);					

	/* Dynamically add Radio buttons  */
	xRadiobuttons=new HWND[toolWindow.bands];
	yRadiobuttons=new HWND[toolWindow.bands];
	zRadiobuttons=new HWND[toolWindow.bands];	

    for (int i=0; i<toolWindow.bands; i++)  
    {
		xRadiobuttons[i] = CreateWindowEx(WS_EX_TRANSPARENT, "BUTTON", NULL,
			WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 7, 15 + (20 * i), 16, 16,
			hX, NULL, hThisInstance, NULL);
		
		yRadiobuttons[i] = CreateWindowEx(WS_EX_TRANSPARENT, "BUTTON", NULL,
			WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 7, 15 + (20 * i), 16, 16,
			hY, NULL, hThisInstance, NULL);
			
		zRadiobuttons[i] = CreateWindowEx(WS_EX_TRANSPARENT, "BUTTON", NULL,
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 7, 15 + (20 * i), 16, 16,
		 hZ, NULL, hThisInstance, NULL);
		
		const char* name;
		if (i>0) { 
    		// add band names to radio buttons
    		name = "";
            // Add band number to band name
            name = catcstrings( (char*) inttocstring(i), (char*) name);
            name = catcstrings( (char*) "Band ", (char*) name);
        } else
               name = "NONE";

        // Display band name in tool window 
		HWND hstatic=CreateWindowEx(0, szStaticControl, name,
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 10, 20 + (20 * i), 100, 18,
			hFSScrollBox.GetHandle(), NULL, Window::GetAppInstance(), NULL);
		SetStaticFont(hstatic,STATIC_FONT_NORMAL);
	}    
	
	
	// Create Trackbar (slider)
	hTrackbar = CreateWindowEx( 
    0,                            // no extended styles 
    "msctls_trackbar32",          // class name TRACKBAR_CLASS
    "Granularity",                // title (caption) 
    WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,  // style  | TBS_AUTOTICKS | TBS_ENABLESELRANGE
    5, 190,                       // position 
    215, 35,                      // size 
    GetHandle(),                  // parent window 
    NULL, //(HMENU) ID_TRACKBAR,  // control identifier  ID_TRACKBAR
    Window::GetAppInstance(),     // instance 
    NULL                          // no WM_CREATE parameter 
    ); 
    
    // Trackbar range - number of positions
    SendMessage(hTrackbar, TBM_SETRANGE, 
        (WPARAM) TRUE,                       // redraw flag 
        (LPARAM) MAKELONG(1, 7));            // min. & max. positions
        
    // Trackbar steps taken when clicking to the side of the pointer or scroll wheel
    SendMessage(hTrackbar, TBM_SETPAGESIZE, 
        0, (LPARAM) 1);                      // new page size 
        
	//Trackbar - sets initial position
    SendMessage(hTrackbar, TBM_SETPOS, 
        (WPARAM) TRUE,                       // redraw flag 
        (LPARAM) 1); 
	
	//Trackbar ratios
	HWND hratios=CreateWindowEx(0, szStaticControl, "1        4        16      64      256   1024   4096",
	WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 16, 225, 210, 18,
	GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetFont(hratios,FONT_NORMAL);
	
	// number of points temp variable
	const long points = guessPoints(SendMessageA(hTrackbar, TBM_GETPOS, 0, 0));
    // Add points to title
    const char* titleAndPoints;
    titleAndPoints = catcstrings( (char*) "Granularity        No. Points: ", (char*) inttocstring(points));
	
    //Trackbar titles and number of points
	htitle=CreateWindowEx(0, szStaticControl, titleAndPoints,
	WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 16, 176, 210, 18,
	GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetStaticFont(htitle,STATIC_FONT_NORMAL);
	
	// Insert ROI only checkbox
	hROIOnly = CreateWindowEx( 0, "BUTTON",
    "ROIs Only", BS_AUTOCHECKBOX | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
	18, 247, 90, 25, GetHandle(),NULL, Window::GetAppInstance(), NULL);
	SetFont(hROIOnly,FONT_NORMAL);

	// Insert 'Generate' button under radio buttons. Location based on band number 
	hgenerate =  CreateWindowEx(0, "BUTTON", "Generate", WS_CHILD | WS_VISIBLE | BS_CHECKBOX  | BS_PUSHLIKE,
	130, 247, 80, 25, GetHandle(), (HMENU) 1, Window::GetAppInstance(), NULL);     
	CreateTooltip(hgenerate,"Generate a feature space");

	SetFont(hgenerate, FONT_BOLD);
	    
	if (toolWindow.bands == 1) {
    	SendMessage(xRadiobuttons[1],BM_SETCHECK,BST_CHECKED,0);
    	SendMessage(yRadiobuttons[1],BM_SETCHECK,BST_CHECKED,0);
    	SendMessage(zRadiobuttons[1],BM_SETCHECK,BST_CHECKED,0);
    } else if (toolWindow.bands == 2) {
        SendMessage(xRadiobuttons[1],BM_SETCHECK,BST_CHECKED,0);
    	SendMessage(yRadiobuttons[2],BM_SETCHECK,BST_CHECKED,0);
    	SendMessage(zRadiobuttons[2],BM_SETCHECK,BST_CHECKED,0);
    } else {
        SendMessage(xRadiobuttons[1],BM_SETCHECK,BST_CHECKED,0);
    	SendMessage(yRadiobuttons[2],BM_SETCHECK,BST_CHECKED,0);
    	SendMessage(zRadiobuttons[3],BM_SETCHECK,BST_CHECKED,0);
    }
    
    hFSScrollBox.UpdateScrollBar();
    //return true;
}

void FeatureTab::OnGenerateClicked(int lod, bool rois_only, int x, int y, int z)
{
    
    Console::write("FeatureTab::OnGenerateClicked\n");
    SendMessage(hgenerate,BM_SETCHECK,BST_CHECKED,0);       // make button appear pushed in
    FeatureSpace *fspace=new FeatureSpace(lod - 1,rois_only, x, y, z);
    SendMessage(hgenerate,BM_SETCHECK,BST_UNCHECKED,0);     // make button appear normal

    // remove all mouse messages from the queue (prevent user from clicking on generate while one was being generated)
    MSG msg;
    while (PeekMessage(&msg,NULL,WM_MOUSEFIRST,WM_MOUSELAST,PM_REMOVE)!=0) {}

    // make sure new feature space window appears in front  
    SetForegroundWindow(fspace->GetHandle());

    //featureSpaceWindows.push_back(fspace);
}

// re-enable generate button
void FeatureTab::OnUserMessage()
{
    EnableWindow(hgenerate,true);
}

LRESULT CALLBACK FeatureTab::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    FeatureTab* win=(FeatureTab*)Window::GetWindowObject(hwnd);
    
    switch(message)
    {
        case WM_HSCROLL:
        {
			// trackbar moved
			if (LOWORD(wParam) == TB_THUMBTRACK)
			{
                	const long points = guessPoints(SendMessageA(win->hTrackbar, TBM_GETPOS, 0, 0));
                    // Add points to title
                    const char* titleAndPoints;
                    titleAndPoints = catcstrings( (char*) "Granularity        No. Points: ", (char*) inttocstring(points));
                    SetWindowText(win->htitle,titleAndPoints);
            }
        }

        case WM_COMMAND:
        {
			
			if (LOWORD(wParam) == 1 && HIWORD(wParam) == BN_CLICKED)
			{
                // find out which bands are selected
				int x, y, z;
				for (int i=0; i<toolWindow.bands; i++)
   				{
					LRESULT state = SendMessageA(win->xRadiobuttons[i], BM_GETCHECK, 0, 0);
					if(state==BST_CHECKED)
						x = i;
					
					state = SendMessageA(win->yRadiobuttons[i], BM_GETCHECK, 0, 0);
					if(state==BST_CHECKED)
						y = i;
					
					state = SendMessageA(win->zRadiobuttons[i], BM_GETCHECK, 0, 0);
					if(state==BST_CHECKED)
						z = i;
				}
				LRESULT rois_only_state = SendMessageA(win->hROIOnly, BM_GETCHECK, 0, 0);
				
            DWORD GranState = SendMessageA(win->hTrackbar, TBM_GETPOS, 0, 0);
            win->OnGenerateClicked((int)GranState, (rois_only_state==BST_CHECKED), x, y, z);
            
            
			//MessageBox( hwnd, (LPSTR) makeMessage("GranState:",(int)GranState), (LPSTR) "Title", MB_ICONINFORMATION | MB_OK );
            }
		}
		case WM_USER:
            win->OnUserMessage();
    }
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}

int FeatureTab::guessPoints(int slider_pos)
{
	ImageProperties* im_prop = image_handler->get_image_properties();
	int my_guess, granularity = 1;
	
	Console::write("guessPoints - slider_pos = %d, ",slider_pos);
	slider_pos--;
	while (slider_pos) {
		granularity *= 4;
		slider_pos--;
	}

	Console::write("granularity = %d, width = %d, height = %d\n", granularity, im_prop->getWidth(), im_prop->getHeight());
	my_guess = im_prop->getWidth() * im_prop->getHeight() / granularity;
	return my_guess;
}
