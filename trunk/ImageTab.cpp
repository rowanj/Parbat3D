#include <Windows.h>
#include "Window.h"
#include "main.h"
#include "ScrollBox.h"
#include "ToolTab.h"
#include "ImageTab.h"
#include "Config.h"
#include "ToolWindow.h"


int ImageTab::GetContainerHeight()
{
    return 0;
}

int ImageTab::Create(HWND parent,RECT *parentRect)
{
    ToolTab::Create(parent,parentRect);
    prevProc=SetWindowProcedure(&WindowProcedure);

}

LRESULT CALLBACK ImageTab::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ImageTab* win=(ImageTab*)Window::GetWindowObject(hwnd);
    
    switch(message)
    {
        
    }
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}
