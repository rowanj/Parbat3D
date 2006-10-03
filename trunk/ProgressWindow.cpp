#include "ProgressWindow.h"

#include "main.h"


int ProgressWindow::Create (HWND parent) {
    RECT rect;
    int mx,my;
    const int PROGRESS_WINDOW_WIDTH=175;
    const int PROGRESS_WINDOW_HEIGHT=75;
    
    
    // Get Overview Window Location for Progress window alignment
    GetWindowRect(overviewWindow.GetHandle(),&rect);
    
    // create prefs window
    if (!CreateWin(0, "Parbat3D Progress Bar Window", "Loading",
        WS_POPUP+WS_SYSMENU+WS_CAPTION,
	    10, 10, //rect.left+50, rect.bottom-150,
        PROGRESS_WINDOW_WIDTH, PROGRESS_WINDOW_HEIGHT,
        parent, NULL))
	    return false;
    
   	// set the background colour
	HBRUSH backBrush=CreateSolidBrush(GetSysColor(COLOR_3DFACE));
    SetBackgroundBrush(backBrush);
    
    
    // create the progress bar
    hProgressBar = CreateWindowEx(0, PROGRESS_CLASS, (LPSTR) NULL, WS_CHILD | WS_VISIBLE,
                                  10, 10, 150, 25,
                                  GetHandle(), (HMENU) 0, Window::GetAppInstance(), NULL);
    
    // set initial values for the progress bar
    SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100)); 
    SendMessage(hProgressBar, PBM_SETSTEP, (WPARAM) 1, 0);
    
    
    // create a cancel button to stop loading
//    cancelButton = CreateWindowEx(0, "BUTTON", "Cancel", WS_CHILD | WS_VISIBLE, 120,
//            85, 55, 25, GetHandle(), (HMENU) 1, Window::GetAppInstance(), NULL);		
    
    
    prevProc = SetWindowProcedure(&WindowProcedure);	
    
    // progress bar will stick to screen edges even without being added to the manager
    //stickyWindowManager.AddStickyWindow(this);  // make the window stick to others
    
    
    // turn off looping once the end is reached
    loop_at_end = false;
    
    return true;
}


LRESULT CALLBACK ProgressWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {   
    static POINT moveMouseOffset;     /* mouse offset relative to window, used for snapping */
    static RECT rect;                 /* for general use */
    
    ProgressWindow* win = (ProgressWindow*) Window::GetWindowObject(hwnd);
    
    
//    switch (message) {  // handle the messages
//        case WM_COMMAND:
//            if (LOWORD(wParam) == 1 && HIWORD(wParam) == BN_CLICKED) {
//                // handle the user cancelling the loading
//            }
//            return 0;
//        
//        case WM_SHOWWINDOW:
//            // update window menu item depending on whether window is shown or hidden
//            if (wParam) {  // show
//            } else {       // hide
//            }
//            return 0;
//        
//        // WM_CLOSE: system or user has requested to close the window/application
//        case WM_CLOSE:
//            // don't destroy this window, but make it invisible
//            ShowWindow(hwnd,SW_HIDE);
//            return 0;
//        
//        // WM_DESTORY: system is destroying our window
//        case WM_DESTROY:
//            break;
//    }
    
    
    // call the next procedure in the chain
    return CallWindowProc(win->prevProc, hwnd, message, wParam, lParam);    
}


void ProgressWindow::start (int steps) {
    total_steps = steps;  // set the total number of steps in the bar
    current_steps = 0;    // initialise the current amount the bar has loaded
    
    // sends a message to the progress bar to set the size and reset the position
    SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, steps));  // resize
    SendMessage(hProgressBar, PBM_SETPOS, (WPARAM) 0, 0);              // reset position
    
    Show();               // opens the window
}


void ProgressWindow::end () {
    Hide();               // hides the window
}


void ProgressWindow::reset () {
    current_steps = 0;
    SendMessage(hProgressBar, PBM_SETPOS, (WPARAM) 0, 0);  // update the progress bar with the reset position
}

void ProgressWindow::reset (int steps) {
    total_steps = steps;
    current_steps = 0;
    
    // sends a message to the progress bar to set the size and reset the position
    SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, steps));  // resize
    SendMessage(hProgressBar, PBM_SETPOS, (WPARAM) 0, 0);              // reset position
}


void ProgressWindow::increment () {
    if (current_steps < total_steps)
        current_steps++;
    else {
        if (loop_at_end)
            current_steps = 0;
    }
    
    // update the progress bar with the new value
    SendMessage(hProgressBar, PBM_SETPOS, (WPARAM) current_steps, 0);
}

void ProgressWindow::increment (int steps) {
    if (current_steps <= (total_steps-steps))
        current_steps += steps;
    else {
        if (loop_at_end && steps<total_steps) {
            current_steps += steps;        // moves past the end of the progress bar
            current_steps -= total_steps;  // reduces back to somewhere inside the bar
        }
    }
    
    // update the progress bar with the new value
    SendMessage(hProgressBar, PBM_SETPOS, (WPARAM) current_steps, 0);
}
