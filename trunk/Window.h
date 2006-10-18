#ifndef _PARBAT_WINDOW_H
#define _PARBAT_WINDOW_H


#include "config.h"
#include "console.h"
#include <Windows.h>
#include <queue>
using namespace std;

/* Window class used for constructing & using object-based windows 
   All of the main windows subclass this class
*/
class Window
{
    private:
	    WNDPROC prevWindowProcedure;        // pointer to previous window proecudure
	    static WNDPROC stPrevWindowProcedure; // pointer to previous window proecudure, used during window creation
	        
	    HWND hwindow;                       // window handle
	    static HINSTANCE hInstance;         // process's instance handle
	    static HICON    hIcon;              // default icon
	    HBRUSH hBackgroundBrush;			// window's background brush used for painting background
	    int zValue;							// window's Z-order value (for setting which windows appear in front or behind)
	    HDWP hdwp_reorder;					// handle used for re-ordering windows
		DWORD threadId;						// id of thread that created the window
	
	    void drawStatic(DRAWITEMSTRUCT *dis); // draw static text control

		/* General window event/message handler. 
		   This procedure receives messages for all windows created by CreateWin(..) (by default)
		   Any message handling that needs to occur for all windows is handled here (eg. global keyboard shortcuts)
		*/
	    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM); 
	
		/* handle global shortcut key events */
	    void OnKeyDown(int virtualKey);			

    protected:
	
		/* Create a window, register window class (if required) & associated it with "this" window object 
		   All subclasses that subclass the Window class call this function to create their windows.
		*/
	    int CreateWin(DWORD dwExStyle,LPCTSTR lpClassName,LPCTSTR lpWindowName,DWORD dwStyle,int x,int y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu); 
			    
    
    public:
		/* Constructor */
	    Window::Window();
	    
		/* Return the id of the thread that created this object's window */
	    DWORD GetThreadId() {return threadId;}

		/* Get object's window handle */
	    inline HWND GetHandle() {return hwindow;};
	    
	    /* create this object's window - overwritten by subclasses */
	    virtual int Create();        
	   
	    /* Setup static variables (called at started) */
	    static void Init(HINSTANCE hInst);
		
		/* Return the application instance variable */         
	    static HINSTANCE GetAppInstance() {return hInstance;};

		/* Set the function that will a handle a window's messages/events.
		   The address of the previous function is returned.
		   The new function must pass certain messages on to the previous function so that the default 
		   	 operation can be performed by the Windows API and/or Window::WindowProcedure
		*/
	    WNDPROC SetWindowProcedure(WNDPROC);            
	    WNDPROC static SetWindowProcedure(HWND,WNDPROC);
	
		/* Obtain the Window object that is associated with a window */
	    static Window* GetWindowObject(HWND);     
		
       /* Associcate a Window object with a window 
	   	  This should usually only be called by CreateWin(..) */
	    static void SetWindowObject(HWND,Window*);

		/* Set the default font used when painting a window/control */
	    static void SetFont(HWND,int);					
	    static const int FONT_NORMAL=0;			// for normal plain text
	    static const int FONT_BOLD=32453;   	// same size as NORMAL, but bold
	    static const int FONT_HEADING=32454;    // a bit bigger than NORMAL, but also bold

   		/* now same as SetFont, left here to prevent breaking code */	    
	    static void SetStaticFont(HWND,int); 
	    static const int STATIC_FONT_NORMAL=0;
	    static const int STATIC_FONT_BOLD=32453;   
	    static const int STATIC_FONT_HEADING=32454;       

		/* create a tooltip message when the user moves the mouse over a control */
	    static void CreateTooltip (HWND hwnd,char *text); 
	
	    /* destroy this window */
	    void Destroy() {DestroyWindow(hwindow);};
	    
	    /* make this window visible */
	    void Show() {ShowWindow(hwindow,SW_SHOW);};
	    
		/* make this window invisible */
	    void Hide() {ShowWindow(hwindow,SW_HIDE);};    
	    
	    /* set the default cursor that is shown as mouse is moved over a window */
	    void SetDefaultCursor(HCURSOR hcur) {SetClassLong(hwindow,GCL_HCURSOR,(long)hcur);};
	    
	    /* set the brush used to paint the background of a window */
	    void SetBackgroundBrush(HBRUSH hbrush) {SetClassLong(hwindow,GCL_HBRBACKGROUND,(long)hbrush); hBackgroundBrush=hbrush;};
	    static void SetBackgroundBrush(HWND hwin,HBRUSH hbrush) {SetClassLong(hwin,GCL_HBRBACKGROUND,(long)hbrush);};    
	    
	    /* cause a window to be re-drawn */
	    void Repaint() {InvalidateRect(hwindow,0,false); UpdateWindow(hwindow);};
	    
	    /* return the handle of a particular window's parent window */
	    static HWND GetParentHandle(HWND hwnd) {return (HWND)GetWindowLong(hwnd,GWL_HWNDPARENT);};	// Get Handle To Parent Window From a Handle To a Child Window 
        
        /* array used to store keyboard shortcuts */
        static const int NUMBER_OF_SHORTCUTS = 10;
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
            This has not yet been implemented for all shortcut keys.
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
        
        static const int KEY_SET_ZOOM_LEVEL = 9;
};

#endif
