#ifndef _PARBAT_GLContainer_H
#define _PARBAT_GLContainer_H

#include "Window.h"
#include "GLView.h"

class GLContainerHandler
{
    public:
    virtual void PaintGLContainer() { };
    virtual void OnGLContainerLeftMouseDown(int x,int y) { };    
    virtual void OnGLContainerMouseMove(int vkeys,int x,int y) { }; 
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
