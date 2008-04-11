#ifndef _PARBAT_SCROLLABLEWINDOW_H
#define _PARBAT_SCROLLABLEWINDOW_H

#include "Window.h"

class ScrollBox:public Window
{
    private:
    WNDPROC prevProc;

    protected:
    int maxScrollHeight;
    int pixelPosition;

    static BOOL CALLBACK GetMaxScrollHeight(HWND hwnd, LPARAM lparam);
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);    
    void Scroll(int msg);

    public:
    int Create(HWND parentHandle,RECT *rect);    // create scroll box: rect=co-cordinates of new box on parent window
    void UpdateScrollBar();                      // re-calculate amount of content that needs to be scrolled & update scrollbars
};


#endif

