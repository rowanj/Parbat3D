#ifndef _PARBAT_GLContainer_H
#define _PARBAT_GLContainer_H

#include "Window.h"
#include "GLView.h"

class GLContainerHandler
{
    public:
    virtual void PaintGLContainer() { };
    virtual void OnGLContainerKeyPress(int virtualKey) { };    
};


class GLContainer:public Window
{
    private:
    WNDPROC prevProc;
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
    
    protected:
    GLContainerHandler *handler;
        
    public:
    GLContainer(HWND parent,GLContainerHandler *eventHandler,int x,int y,int width,int height);    

};

#endif
