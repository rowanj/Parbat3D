#ifndef _PARBAT_FeatureSpace_H
#define _PARBAT_FeatureSpace_H

#include "Window.h"
#include "GLView.h"
#include "GLContainer.h"

class FeatureSpace:public Window, public GLContainerHandler
{
    private:
    WNDPROC prevProc;
    static int numFeatureSpaces;
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
    
    protected:
    GLView *glview;
    GLContainer *glContainer;
    int Create();
        
    public:
    FeatureSpace(int LOD, bool only_ROIs);      // create & display new feature space window
    virtual void PaintGLContainer();    
    

};

#endif
