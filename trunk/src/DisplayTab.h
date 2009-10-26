#ifndef _PARBAT_DISPLAYTAB_H
#define _PARBAT_DISPLAYTAB_H

#include "ToolTab.h"

class DisplayTab:public ToolTab
{
    private:
        WNDPROC prevProc;
        HWND hRed, hBlue, hGreen;        
        HWND hupdate;
        HWND *redRadiobuttons;                // band radio buttons
        HWND *greenRadiobuttons;
        HWND *blueRadiobuttons;
    public:
        const char* GetTabName() {return "Display";};
        const char* GetTabHeading() {return "Channel Selection";};
        int GetContainerHeight();   
        int Create(HWND parent,RECT *parentRect);
        static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);    
    
};

#endif
