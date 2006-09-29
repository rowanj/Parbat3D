#ifndef _PARBAT_WINDOW_H
#define _PARBAT_WINDOW_H


#include "config.h"
#include "console.h"
#include <Windows.h>
#include <queue>
using namespace std;

//int compareZOrder( const Window *p1,const Window *p2 ) {return (p1->zValue < p2->zValue)};


class ZWindow
{
	public:
		HWND hwnd;
		int zValue;

		ZWindow(HWND hwin,int z) {hwnd=hwin; zValue=z;}
		bool operator < (ZWindow w) const
		{
			//Console::write("ZWindow < operator\n");
			return zValue < w.zValue;
		}
};




class Window
{
    private:
	    WNDPROC prevWindowProcedure;        // pointer to previous window proecudure
	    static WNDPROC stPrevWindowProcedure; // pointer to previous window proecudure, used during window creation
	        
	    HWND hwindow;                       // window handle
	    static HINSTANCE hInstance;         // process's instance handle
	    static HICON    hIcon;              // default icon
	    HFONT hNormalFont,hBoldFont, hHeadingFont;        // fonts used for drawing static controls
	    HBRUSH hBackgroundBrush;			// window's background brush used for painting background
	    int zValue;							// window's Z-order value (for setting which windows appear in front or behind)
	    HDWP hdwp_reorder;					// handle used for re-ordering windows

        priority_queue<ZWindow> zWindows;		// ordered list of windows, used for re-ordering windows   
    	unsigned int static GetZValue(HWND hwnd);									// Get window's zValue, if set
    	static BOOL CALLBACK AddZWindowCallback(HWND hwnd,LPARAM lParam);	// callback function to create zWindow queue
		HWND GetInsertAfterByZ(HWND hwnd);
	
	    void drawStatic(DRAWITEMSTRUCT *dis); // draw static text control

    protected:
	
	    int CreateWin(DWORD dwExStyle,LPCTSTR lpClassName,LPCTSTR lpWindowName,DWORD dwStyle,int x,int y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu);   
			// create window, register window class (if required) & associated it with "this" object	    
	
	    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM); // general window event/message handler    
	
	    void OnKeyDown(int virtualKey);			// handle global shortcut key events
	    
    
    public:
	    Window::Window();
	
		bool operator<(Window *w2)  const
		{
			Console::write("Window < operator-----------------");
			return zValue < w2->zValue;
		}

	
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

		void SetZValue(unsigned int zLevel);				// Set the window's Z-value (for positing windows in front of others)
															// zLevel must be between 1 & 2^16-1. higher values=futher in front, lower values=further behind.	    
	
	    /* window api wrappers */
	    void Destroy() {DestroyWindow(hwindow);};
	    void Show() {ShowWindow(hwindow,SW_SHOW);};
	    void Hide() {ShowWindow(hwindow,SW_HIDE);};    
	    void SetDefaultCursor(HCURSOR hcur) {SetClassLong(hwindow,GCL_HCURSOR,(long)hcur);};
	    void SetBackgroundBrush(HBRUSH hbrush) {SetClassLong(hwindow,GCL_HBRBACKGROUND,(long)hbrush); hBackgroundBrush=hbrush;};
	    static void SetBackgroundBrush(HWND hwin,HBRUSH hbrush) {SetClassLong(hwin,GCL_HBRBACKGROUND,(long)hbrush);};    
	    void Repaint() {InvalidateRect(hwindow,0,false); UpdateWindow(hwindow);};
	    static HWND GetParentHandle(HWND hwnd) {return (HWND)GetWindowLong(hwnd,GWL_HWNDPARENT);};	// Get Handle To Parent Window From a Handle To a Child Window 
        
        
        static const int NUMBER_OF_SHORTCUTS = 9;
        static int keyboardShortcutKeys[NUMBER_OF_SHORTCUTS];
        
        
        /**
            Sets all the keyboard shortcuts to the defaults that they start
            with when the program is first run.
        */
        static void setDefaultKeyboardShortcuts ();
        
        
        /**
            Returns the key that corresponds to a particular keyboard shortcut.
            The first parameter should be one of the constant KEY_* values to
            indicate which shortcut to change.
        */
        static int getKeyboardShortcut (int key);
        
        
        /**
            Sets a particular keyboard shortcut to a given value. The first
            parameter should be one of the constant KEY_* values to indicate
            which shortcut to change. The second parameter should be the new key
            which will trigger the shortcut.
        */
        static void setKeyboardShortcut (int key, int value);
        
        
        static const int KEY_IMAGE_WINDOW = 0;
        static const int KEY_TOOLS_WINDOW = 1;
        static const int KEY_ROI_WINDOW = 2;
        static const int KEY_PREFERENCES_WINDOW = 3;
        static const int KEY_CONTRAST_WINDOW = 4;
        
        static const int KEY_HELP = 5;
        
        static const int KEY_ROI_POINT = 6;
        static const int KEY_ROI_RECT = 7;
        static const int KEY_ROI_POLY = 8;
};


/*bool operator<(Window w1,Window w2)  
{
	Console::write("Window < operator\n");
	return w1.zValue < w2.zValue;
}*/


#endif
