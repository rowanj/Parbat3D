#include <Windows.h>
#include "Window.h"
#include "main.h"
#include "ScrollBox.h"
#include "ToolTab.h"
#include "QueryTab.h"
#include "Config.h"
#include "ToolWindow.h"

ScrollBox hQScrollBox;

int QueryTab::GetContainerHeight()
{
    //return 80 + (20 * (toolWindow.bands+1));
    return 0;
}

int QueryTab::Create(HWND parent,RECT *parentRect)
{
	HWND hstatic;
	
    ToolTab::Create(parent,parentRect);
    
    RECT rect;
   	rect.top=5;
   	rect.left=25;
   	rect.right=218;
   	rect.bottom=200;                 	
	hQScrollBox.Create(GetHandle(),&rect);

	/* Create container for band values */
	HWND queryValueContainer = CreateWindowEx(0, "BUTTON", "Values",
		WS_CHILD | BS_GROUPBOX | WS_VISIBLE, 118, 5, 66, 20 + (20 * (toolWindow.bands-1)),
		hQScrollBox.GetHandle(), NULL, Window::GetAppInstance(), NULL);
	
	/* Dynamically add image band values */
	imageBandValues = new HWND[toolWindow.bands];
    

    for (int i=1; i<toolWindow.bands; i++)  
    {

		const char* name;
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

        // add channel names under the query tab
        hstatic=CreateWindowEx(0, szStaticControl, name,
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 10, 0 + (20 * i), 100, 18,
			hQScrollBox.GetHandle(), NULL, Window::GetAppInstance(), NULL);
		SetFont(hstatic,Window::FONT_NORMAL);

		// add the band values to the value container under the query tab
        char tempBandValue[4] = "0"; // temporary storage for the band value
        imageBandValues[i] = CreateWindowEx(0, szStaticControl, tempBandValue, WS_CHILD | WS_VISIBLE | SS_CENTER, 5, 15 + (20 * (i-1)),
			50, 18, queryValueContainer, NULL, Window::GetAppInstance(), NULL);
		SetFont(imageBandValues[i],Window::FONT_NORMAL);                 			
    }

	
	/* display cursor position under query tab */
	hstatic=CreateWindowEx(0, szStaticControl, "X", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
                 20, 235, 100, 18, GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetFont(hstatic,Window::FONT_NORMAL);    
	             
	hstatic=CreateWindowEx(0, szStaticControl, "Y", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
                 20, 255, 100, 18, GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetFont(hstatic,Window::FONT_NORMAL); 
	                                 
    hstatic=cursorXPos = CreateWindowEx(0, szStaticControl, "-", WS_CHILD | WS_VISIBLE,
                 50, 235, 50, 18, GetHandle(), NULL, Window::GetAppInstance(), NULL);			
	SetFont(hstatic,Window::FONT_NORMAL);  
	                                
    hstatic=cursorYPos = CreateWindowEx(0, szStaticControl, "-", WS_CHILD | WS_VISIBLE,
                 50, 255, 50, 18, GetHandle(), NULL, Window::GetAppInstance(), NULL);
	SetFont(hstatic,Window::FONT_NORMAL);                                  
				 
	hQScrollBox.UpdateScrollBar();		
}

void QueryTab::SetImageBandValue(int i,char *text)
{
    SetWindowText(imageBandValues[i],text);
}

void QueryTab::SetCursorX(char *text)
{
    SetWindowText(cursorXPos,text);
}

void QueryTab::SetCursorY(char *text)
{
    SetWindowText(cursorYPos,text);
}

