#ifndef _PARBAT_WINDOW_H
#define _PARBAT_WINDOW_H

#include <Windows.h>

class Window
{
    private:
    WNDPROC prevWindowProcedure;        
    HWND hwindow;    
    protected:
    static WNDPROC stPrevWindowProcedure;
    int CreateWin(DWORD dwExStyle,LPCTSTR lpClassName,LPCTSTR lpWindowName,DWORD dwStyle,int x,int y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance);
    
    public:
    virtual int Create(HINSTANCE hInstance);
        
    Window::Window();
    inline HWND GetHandle() {return hwindow;};
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
    static LRESULT DefaultProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    static Window* GetWindowObject(HWND);
    static void SetWindowObject(HWND,Window*);    
    static int RegisterWinClass(WNDCLASSEX*);
    WNDPROC SetWindowProcedure(WNDPROC);
    inline void Destroy() {DestroyWindow(hwindow);};
    inline void Show() {ShowWindow(hwindow,SW_SHOW);};
    inline void Hide() {ShowWindow(hwindow,SW_HIDE);};    
};

class TestWindow:Window
{
    public:
    int Create(HINSTANCE hInstance,HWND parent);
};

#endif
