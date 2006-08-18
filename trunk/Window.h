#ifndef _PARBAT_WINDOW_H
#define _PARBAT_WINDOW_H

#include <Windows.h>

class Window
{
    private:
    WNDPROC prevWindowProcedure;        // pointer to previous window proecudure
    HWND hwindow;                       // window handle
    
    protected:
    static WNDPROC stPrevWindowProcedure; // pointer to previous window proecudure, used during window creation

    int CreateWin(DWORD dwExStyle,LPCTSTR lpClassName,LPCTSTR lpWindowName,DWORD dwStyle,int x,int y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance);
            // create window & associated it with this object
    
    public:
    Window::Window();        

    inline HWND GetHandle() {return hwindow;};      /* get object's window handle */
    virtual int Create(HINSTANCE hInstance);        /* create this object's window - overwritten by subclasses */
    
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
            // general window event/message handler
            
    WNDPROC SetWindowProcedure(WNDPROC);            /* return current window event/messeger handler & set new one */

    static Window* GetWindowObject(HWND);           /* obtain "this" object from window handle */
    static void SetWindowObject(HWND,Window*);      /* associcate "this" object with window handle */

    /* simple window api wrappers */
    inline void Destroy() {DestroyWindow(hwindow);};
    inline void Show() {ShowWindow(hwindow,SW_SHOW);};
    inline void Hide() {ShowWindow(hwindow,SW_HIDE);};    
    inline void SetDefaultCursor(HCURSOR hcur) {SetClassLong(hwindow,GCL_HCURSOR,(long)hcur);};
    inline void Repaint() {InvalidateRect(hwindow,0,true); UpdateWindow(hwindow);};

};


#endif
