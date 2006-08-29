#ifndef _PARBAT_FeatureSpace_H
#define _PARBAT_FeatureSpace_H

#include "Window.h"

class FeatureSpace:public Window
{
    private:
    WNDPROC prevProc;
        
    public:
    FeatureSpace(int LOD, bool only_ROIs);
    //DisplayWindow FeatureSpaceDisplay;

    int Create(HWND parent);
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
};

#endif
