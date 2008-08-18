#include "PchApp.h"

#include "ToolTab.h"

#include "Config.h"

#include "Window.h"
#include "main.h"
#include "ScrollBox.h"
#include "ToolWindow.h"


Window testwin;
// create tool tab container
int ToolTab::Create(HWND parent,RECT *parentRect)
{
    const int SCROLLBAR_WIDTH=13;
    int height=GetContainerHeight();
    if (height==0)
        height=parentRect->bottom;
        
	if (!CreateWin( 0, "Parbat3D ToolTab Container", "", 
		WS_CHILD | WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | SS_OWNERDRAW, parentRect->left,
		parentRect->top, parentRect->right-SCROLLBAR_WIDTH, height, parent, NULL))
		return false;
    SetBackgroundBrush(toolWindow.hTabBrush);
	prevProc=SetWindowProcedure(&WindowProcedure);

	// create static control for heading
    hHeading =CreateWindowEx( 0, szStaticControl, GetTabHeading(), 
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_OWNERDRAW, 0,
		0, parentRect->right-SCROLLBAR_WIDTH, 20, GetHandle(), NULL,
		GetAppInstance(), NULL);
	SetFont(hHeading,Window::FONT_HEADING);

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
    {
    }        
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam); 
}
