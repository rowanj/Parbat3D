#ifndef _PARBAT_IMAGETAB_H
#define _PARBAT_IMAGETAB_H

#include "ToolTab.h"

class ImageTab:public ToolTab
{
    private:
        WNDPROC prevProc;
    public:
        char* GetTabName() {return "Image";};
        char* GetTabHeading() {return "Image Properties";};
        int GetContainerHeight();   
        int Create(HWND parent,RECT *parentRect);
        static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);    
    
};

#endif
