#ifndef _PARBAT_WINDOW_H
#define _PARBAT_WINDOW_H


#include "config.h"
#include <Windows.h>


class Window
{
    private:
    WNDPROC prevWindowProcedure;        // pointer to previous window proecudure
    static WNDPROC stPrevWindowProcedure; // pointer to previous window proecudure, used during window creation
        
    HWND hwindow;                       // window handle
    static HINSTANCE hInstance;         // process's instance handle
    static HICON    hIcon;              // default icon
    HFONT hNormalFont,hBoldFont, hHeadingFont;        // fonts used for drawing static controls
    HBRUSH hBackgroundBrush;

    void drawStatic(DRAWITEMSTRUCT *dis); // draw static text control
    protected:

    int CreateWin(DWORD dwExStyle,LPCTSTR lpClassName,LPCTSTR lpWindowName,DWORD dwStyle,int x,int y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu);
            // create window, register window class (if required) & associated it with this object
    
    int OnWindowPosChanging(WINDOWPOS *wp);

    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
	    // general window event/message handler    

    void OnKeyDown(int virtualKey);	// handle global shortcut key events
    
    public:
    Window::Window();

    inline HWND GetHandle() {return hwindow;};      /* get object's window handle */
    virtual int Create();        /* create this object's window - overwritten by subclasses */
   
    static void Init(HINSTANCE hInst);            /* called at startup to setup static variables */    
    static HINSTANCE GetAppInstance() {return hInstance;};
           
    WNDPROC SetWindowProcedure(WNDPROC);            /* return current window event/messeger handler & set new one */
    WNDPROC static SetWindowProcedure(HWND,WNDPROC); /* return current window event/messeger handler & set new one for a control */ 

    static Window* GetWindowObject(HWND);           /* obtain "this" object from window handle */
    static void SetWindowObject(HWND,Window*);      /* associcate "this" object with window handle */
    
    static void SetStaticFont(HWND,int);      /* associcate a font with a static text control 
                                                    note: SetStaticFont & SetWindowObject store data in the same location */
    static const int STATIC_FONT_NORMAL=0;
    static const int STATIC_FONT_BOLD=32453;   
    static const int STATIC_FONT_HEADING=32454;       

    static void CreateTooltip (HWND hwnd,char *text);   // create a tooltip message when the user moves the mouse over a control

    /* window api wrappers */
    void Destroy() {DestroyWindow(hwindow);};
    void Show() {ShowWindow(hwindow,SW_SHOW);};
    void Hide() {ShowWindow(hwindow,SW_HIDE);};    
    void SetDefaultCursor(HCURSOR hcur) {SetClassLong(hwindow,GCL_HCURSOR,(long)hcur);};
    void SetBackgroundBrush(HBRUSH hbrush) {SetClassLong(hwindow,GCL_HBRBACKGROUND,(long)hbrush); hBackgroundBrush=hbrush;};
    static void SetBackgroundBrush(HWND hwin,HBRUSH hbrush) {SetClassLong(hwin,GCL_HBRBACKGROUND,(long)hbrush);};    
    void Repaint() {InvalidateRect(hwindow,0,false); UpdateWindow(hwindow);};
    static HWND GetParentHandle(HWND hwnd) {return (HWND)GetWindowLong(hwnd,GWL_HWNDPARENT);};	// Get Handle To Parent Window From a Handle To a Child Window 
};


#endif
