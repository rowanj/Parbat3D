#include <Windows.h>
#include "Window.h"
#include "main.h"
#include "ScrollBox.h"
#include "ToolTab.h"
#include "DisplayTab.h"
#include "Config.h"
#include "ToolWindow.h"

ScrollBox hDScrollBox;

int DisplayTab::GetContainerHeight()
{
    //return 90 + (20 * toolWindow.bands);
    return 0;
}

int DisplayTab::Create(HWND parent,RECT *parentRect)
{
    ToolTab::Create(parent,parentRect);
    prevProc=SetWindowProcedure(&WindowProcedure);
    
    RECT rect;
   	rect.top=5;
   	rect.left=25;
   	rect.right=218;
   	rect.bottom=217;                 	
	hDScrollBox.Create(GetHandle(),&rect);

    hRed = CreateWindowEx(0, "BUTTON", "R", WS_CHILD | BS_GROUPBOX | WS_VISIBLE , 118, 5,
		26, 20 + (20 * toolWindow.bands), hDScrollBox.GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetFont(hRed,FONT_BOLD);
	
	hGreen = CreateWindowEx(0, "BUTTON", "G", WS_CHILD | BS_GROUPBOX | WS_VISIBLE, 144, 5,
		26, 20 + (20 * toolWindow.bands), hDScrollBox.GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetFont(hGreen,FONT_BOLD);	
		
    hBlue = CreateWindowEx(0, "BUTTON", "B", WS_CHILD | BS_GROUPBOX | WS_VISIBLE, 170, 5,
		26, 20 + (20 * toolWindow.bands), hDScrollBox.GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetFont(hBlue,FONT_BOLD);		

	/* Dynamically add Radio buttons  */
	redRadiobuttons=new HWND[toolWindow.bands];
	greenRadiobuttons=new HWND[toolWindow.bands];
	blueRadiobuttons=new HWND[toolWindow.bands];	

    for (int i=0; i<toolWindow.bands; i++)  
    {
		char tooltip[100];
		redRadiobuttons[i] = CreateWindowEx(WS_EX_TRANSPARENT, "BUTTON", NULL,
			WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 7, 15 + (20 * i), 16, 16,
			hRed, NULL, hThisInstance, NULL);
		SetBackgroundBrush(redRadiobuttons[i],toolWindow.hTabBrush);
		if (i==0)
			sprintf(tooltip,"Click if you do not wish to display anything in the red channel.\n"); 		
		else
			sprintf(tooltip,"Click to display Band %d in the red channel.\n",i); 
		//CreateTooltip(redRadiobuttons[i],tooltip);	
			
		greenRadiobuttons[i] = CreateWindowEx(WS_EX_TRANSPARENT, "BUTTON", NULL,
			WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 7, 15 + (20 * i), 16, 16,
			hGreen, NULL, hThisInstance, NULL);
		if (i==0)
			sprintf(tooltip,"Click if you do not wish to display anything in the green channel.\n"); 		
		else
			sprintf(tooltip,"Click to display Band %d in the green channel.\n",i); 
		//CreateTooltip(greenRadiobuttons[i],tooltip);	
			
		blueRadiobuttons[i] = CreateWindowEx(WS_EX_TRANSPARENT, "BUTTON", NULL,
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 7, 15 + (20 * i), 16, 16, hBlue, NULL,
			hThisInstance, NULL);
		if (i==0)
			sprintf(tooltip,"Click if you do not wish to display anything in the blue channel.\n"); 		
		else
			sprintf(tooltip,"Click to display Band %d in the blue channel.\n",i); 
		//CreateTooltip(blueRadiobuttons[i],tooltip);	
		
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
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 10, 20 + (20 * i), 100, 18,
			hDScrollBox.GetHandle(), NULL, Window::GetAppInstance(), NULL);
		SetStaticFont(hstatic,STATIC_FONT_NORMAL);
           
		// Insert 'Update' button under radio buttons. Location based on band number 
		hupdate =  CreateWindowEx(0, "BUTTON", "Update", WS_CHILD | WS_VISIBLE, 80,
			247, 80, 25, GetHandle(), NULL, Window::GetAppInstance(), NULL);  
		SetFont(hupdate,FONT_BOLD);
		CreateTooltip(hupdate,"Update Image"); 

    
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
    
    hDScrollBox.UpdateScrollBar();
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
