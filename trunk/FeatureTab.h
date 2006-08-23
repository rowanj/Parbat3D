#ifndef _PARBAT_FEATURETAB_H
#define _PARBAT_FEATURETAB_H

#include "ToolTab.h"

class FeatureTab:public ToolTab
{
    private:
        WNDPROC prevProc;
        HWND hTrackbar;
        HWND hX, hY, hZ;        
        HWND hgenerate;
        HWND *xRadiobuttons;                // band radio buttons
        HWND *yRadiobuttons;
        HWND *zRadiobuttons;
    public:
        char* GetTabName() {return "Feature Space";};
        char* GetTabHeading() {return "Feature Space";};
        int GetContainerHeight();   
        int Create(HWND parent,RECT *parentRect);
        static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);    
    
};

#endif
