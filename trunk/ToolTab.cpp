#include <Windows.h>
#include "Window.h"
#include "main.h"
#include "ScrollBox.h"
#include "ToolTab.h"
#include "Config.h"
#include "ToolWindow.h"


Window testwin;
// create tool tab container
int ToolTab::Create(HWND parent,RECT *parentRect)
{
    const int SCROLLBAR_WIDTH=13;
	if (!CreateWin( 0, "Parbat3D ToolTab Container", "", 
		WS_CHILD | WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | SS_OWNERDRAW, parentRect->left,
		parentRect->top, parentRect->right-SCROLLBAR_WIDTH, GetContainerHeight(), parent, NULL))
		return false;
    SetBackgroundBrush(toolWindow.hTabBrush);
	prevProc=SetWindowProcedure(&WindowProcedure);

	// create static control for heading
    hHeading =CreateWindowEx( 0, szStaticControl, GetTabHeading(), 
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_OWNERDRAW, 0,
		0, parentRect->right-SCROLLBAR_WIDTH, 20, GetHandle(), NULL,
		GetAppInstance(), NULL);
	SetStaticFont(hHeading,STATIC_FONT_HEADING);

    /* start of scroll box / edit control testing code */
    testwin.Create();
    RECT rect;		
    GetClientRect(testwin.GetHandle(),&rect);
    rect.top+=10;
    rect.left+=10;
    rect.right-=20;
    rect.bottom-=20;
    scrollBox.Create(testwin.GetHandle(),&rect);

	// create static control for heading
    HWND testStatic =CreateWindowEx( 0, szStaticControl, "some text", 
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_OWNERDRAW, 10,
		rect.bottom-10, 100, 50, scrollBox.GetHandle(), NULL,
		GetAppInstance(), NULL);    
	SetStaticFont(testStatic,STATIC_FONT_BOLD);
  HWND    hwndEdit = CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT",      // predefined class 
                                    0,        // no window title 
                                    WS_CHILD | WS_VISIBLE | 
                                    ES_LEFT | ES_AUTOHSCROLL, 
                                    10, 10, 300, 20,  // set size in WM_SIZE message 
                                    scrollBox.GetHandle(),        // parent window 
                                    (HMENU) 8,   // edit control ID 
                                    (HINSTANCE) hThisInstance, 
                                    NULL);       // pointer not needed 
 
            // Add text to the window. 
            SendMessage(hwndEdit, WM_SETTEXT, 0, (LPARAM) "hello?"); 
    EnableWindow(hwndEdit,true);
    SetFocus(hwndEdit);
    SendMessage(hwndEdit,WM_CHAR,'s',0);
    scrollBox.Hide();
    scrollBox.Show();
    /* end of scroll box / edit control testing code */    
	return true;
}

LRESULT CALLBACK ToolTab::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ToolTab* win=(ToolTab*)Window::GetWindowObject(hwnd);

// See config.h to toggle these debug statements
#if DEBUG_TOOLWINDOW
	Console::write("ToolTab::WindowProcedure win=");
	Console::write((int)win->prevProc);
	Console::write("\n");
#endif
    switch (message) 
    {/*
        case WM_DRAWITEM:
			#if DEBUG_TOOLWINDOW
            Console::write("ToolTab::WindowProcedure WM_DRAWITEM: ");
            Console::write((int)win->hHeading);
            Console::write(" compared with ");
			Console::write((int)((DRAWITEMSTRUCT*)lParam)->hwndItem);            
            Console::write("\n");
            #endif
            // draw window's owner-drawn static text controls using our custom fonts
            if (((DRAWITEMSTRUCT*)lParam)->CtlType==ODT_STATIC)
                if (((DRAWITEMSTRUCT*)lParam)->hwndItem==win->hHeading)
                    toolWindow.drawStatic((DRAWITEMSTRUCT*)lParam,toolWindow.hBoldFont);                
                else
                    toolWindow.drawStatic((DRAWITEMSTRUCT*)lParam,toolWindow.hNormalFont);
            return 0;*/
    }        
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam); 
}
