#include <Windows.h>
#include "Window.h"
#include "main.h"
#include "ScrollBox.h"
#include "ToolTab.h"
#include "QueryTab.h"
#include "Config.h"
#include "ToolWindow.h"


int QueryTab::GetContainerHeight()
{
    return 80 + (20 * (toolWindow.bands+1));
}

int QueryTab::Create(HWND parent,RECT *parentRect)
{
    ToolTab::Create(parent,parentRect);

	/* Create container for band values */
	HWND queryValueContainer = CreateWindowEx(0, "BUTTON", "Values",
		WS_CHILD | BS_GROUPBOX | WS_VISIBLE, 138, 45, 66, 20 + (20 * (toolWindow.bands-1)),
		GetHandle(), NULL, Window::GetAppInstance(), NULL);
	
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
        CreateWindowEx(0, szStaticControl, name,
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW, 20, 40 + (20 * i), 100, 18,
			GetHandle(), NULL, Window::GetAppInstance(), NULL);

		// add the band values to the value container under the query tab
        char tempBandValue[4] = "0"; // temporary storage for the band value
        imageBandValues[i] = CreateWindowEx(0, szStaticControl, tempBandValue, WS_CHILD | WS_VISIBLE, 5, 15 + (20 * (i-1)),
			50, 18, queryValueContainer, NULL, Window::GetAppInstance(), NULL);
    }

	
	/* display cursor position under query tab */
	CreateWindowEx(0, szStaticControl, "X", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW,
                 20, 60 + (20 * toolWindow.bands), 100, 18, GetHandle(), NULL, Window::GetAppInstance(), NULL);
	CreateWindowEx(0, szStaticControl, "Y", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE  | SS_OWNERDRAW,
                 20, 60 + (20 * (toolWindow.bands+1)), 100, 18, GetHandle(), NULL, Window::GetAppInstance(), NULL);
    cursorXPos = CreateWindowEx(0, szStaticControl, "-", WS_CHILD | WS_VISIBLE,
                 143, 60 + (20 * toolWindow.bands), 50, 18, GetHandle(), NULL, Window::GetAppInstance(), NULL);			
    cursorYPos = CreateWindowEx(0, szStaticControl, "-", WS_CHILD | WS_VISIBLE,
                 143, 60 + (20 * (toolWindow.bands+1)), 50, 18, GetHandle(), NULL, Window::GetAppInstance(), NULL);			


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

