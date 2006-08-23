
#include <Windows.h>
#include "Window.h"
#include "main.h"
#include "ScrollBox.h"
#include "ToolTab.h"
#include "DisplayTab.h"
#include "Config.h"
#include "ToolWindow.h"


int DisplayTab::GetContainerHeight()
{
    return 90 + (20 * toolWindow.bands);
}

int DisplayTab::Create(HWND parent,RECT *parentRect)
{
    ToolTab::Create(parent,parentRect);
    prevProc=SetWindowProcedure(&WindowProcedure);

    hRed = CreateWindowEx(0, "BUTTON", "R", WS_CHILD | BS_GROUPBOX | WS_VISIBLE, 138, 25,
		26, 20 + (20 * toolWindow.bands), GetHandle(), NULL, Window::GetAppInstance(), NULL);
	hGreen = CreateWindowEx(0, "BUTTON", "G", WS_CHILD | BS_GROUPBOX | WS_VISIBLE, 164, 25,
		26, 20 + (20 * toolWindow.bands), GetHandle(), NULL, Window::GetAppInstance(), NULL);
    hBlue = CreateWindowEx(0, "BUTTON", "B", WS_CHILD | BS_GROUPBOX | WS_VISIBLE, 190, 25,
		26, 20 + (20 * toolWindow.bands), GetHandle(), NULL, Window::GetAppInstance(), NULL);

	/* Dynamically add Radio buttons  */
	redRadiobuttons=new HWND[toolWindow.bands];
	greenRadiobuttons=new HWND[toolWindow.bands];
	blueRadiobuttons=new HWND[toolWindow.bands];	

    for (int i=0; i<toolWindow.bands; i++)  
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
        } else
               name = "NONE";

        // Display band name in tool window 
		HWND hstatic=CreateWindowEx(0, szStaticControl, name,
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 60, 40 + (20 * i), 100, 18,
			GetHandle(), NULL, Window::GetAppInstance(), NULL);
		SetStaticFont(hstatic,STATIC_FONT_NORMAL);
           
		// Insert 'Update' button under radio buttons. Location based on band number 
		hupdate =  CreateWindowEx(0, "BUTTON", "Update", WS_CHILD | WS_VISIBLE, 136,
			50 + (20 * toolWindow.bands), 80, 25, GetHandle(), NULL, Window::GetAppInstance(), NULL);     
    
	}    
	
	if (toolWindow.bands == 1) {
    	SendMessage(redRadiobuttons[1],BM_SETCHECK,BST_CHECKED,0);
    	SendMessage(greenRadiobuttons[1],BM_SETCHECK,BST_CHECKED,0);
    	SendMessage(blueRadiobuttons[1],BM_SETCHECK,BST_CHECKED,0);
    } else if (toolWindow.bands == 2) {
        SendMessage(redRadiobuttons[1],BM_SETCHECK,BST_CHECKED,0);
    	SendMessage(greenRadiobuttons[2],BM_SETCHECK,BST_CHECKED,0);
    	SendMessage(blueRadiobuttons[2],BM_SETCHECK,BST_CHECKED,0);
    } else {
        SendMessage(redRadiobuttons[1],BM_SETCHECK,BST_CHECKED,0);
    	SendMessage(greenRadiobuttons[2],BM_SETCHECK,BST_CHECKED,0);
    	SendMessage(blueRadiobuttons[3],BM_SETCHECK,BST_CHECKED,0);
    }	
}

LRESULT CALLBACK DisplayTab::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    DisplayTab* win=(DisplayTab*)Window::GetWindowObject(hwnd);
    
    switch(message)
    {
            case WM_COMMAND:
            //if(hupdate==(HWND)lParam)
            {
                // find out which bands are selected
				int r, g, b;
				for (int i=0; i<toolWindow.bands; i++)
   				{
					LRESULT state = SendMessageA(win->redRadiobuttons[i], BM_GETCHECK, 0, 0);
					if(state==BST_CHECKED)
						r = i;
					
					state = SendMessageA(win->greenRadiobuttons[i], BM_GETCHECK, 0, 0);
					if(state==BST_CHECKED)
						g = i;
					
					state = SendMessageA(win->blueRadiobuttons[i], BM_GETCHECK, 0, 0);
					if(state==BST_CHECKED)
						b = i;
				}

	    		// Temporary message
	    		//const char *butNum = "selected bands are: "; 
				//butNum = catcstrings( (char*) butNum, (char*) inttocstring(r) );
				//butNum = catcstrings( (char*) butNum, (char*) ", " );
				//butNum = catcstrings( (char*) butNum, (char*) inttocstring(g) );
				//butNum = catcstrings( (char*) butNum, (char*) ", " );
				//butNum = catcstrings( (char*) butNum, (char*) inttocstring(b) );
				//MessageBox( hwnd, (LPSTR) butNum,
                  //  (LPSTR) szOverviewWindowClassName,
        		//	MB_ICONINFORMATION | MB_OK );
        		// #define DEBUG_IMAGE_HANDLER to see these values printed in console window.
				
				// !! Insert band numbers (bands start at 1, not 0) here. - Rowan
				// 0 now equals none - Damian
//            	if (image_handler) image_handler->set_bands(r,g,b);
                assert(image_handler != NULL);
                image_handler->get_image_viewport()->set_display_bands(r,g,b);
            }                
            return 0;
    }
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}
