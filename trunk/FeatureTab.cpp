#include <Windows.h>
#include "Window.h"
#include "main.h"
#include "ScrollBox.h"
#include "ToolTab.h"
#include "FeatureTab.h"
#include "Config.h"
#include "ToolWindow.h"


int FeatureTab::GetContainerHeight()
{
    return 0;
}

int FeatureTab::Create(HWND parent,RECT *parentRect)
{
    ToolTab::Create(parent,parentRect);
    prevProc=SetWindowProcedure(&WindowProcedure);

}

LRESULT CALLBACK FeatureTab::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    FeatureTab* win=(FeatureTab*)Window::GetWindowObject(hwnd);
    
    switch(message)
    {
        
    }
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}
