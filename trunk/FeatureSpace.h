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
    static const int FEATURE_WINDOW_WIDTH;
    static const int FEATURE_WINDOW_HEIGHT;
    static const int TOOLBAR_HEIGHT;        
    
    protected:
    GLView *glview;
    GLContainer *glContainer;
    int Create();
    void OnResize();    
        
    public:
    FeatureSpace(int LOD, bool only_ROIs);      // create & display new feature space window
    virtual void PaintGLContainer();            // draw contents of GLContainer with opengl


};

#endif
