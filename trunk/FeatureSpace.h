#ifndef _PARBAT_FeatureSpace_H
#define _PARBAT_FeatureSpace_H

#include "Window.h"
#include "GLView.h"

class FeatureSpace:public Window
{
    private:
    WNDPROC prevProc;
    HWND hglcontainer;
    static int numFeatureSpaces;
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
    
    protected:
    GLView *glview;
    int Create(HWND parent);
        
    public:
    FeatureSpace(int LOD, bool only_ROIs);
    

};

#endif
