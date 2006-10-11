#include "ProgressWindow.h"

#include "main.h"

// static variables
CRITICAL_SECTION ProgressWindow::updating_progress_bar;

// setup variables
ProgressWindow::ProgressWindow()
{
	start_count=0;
	autoIncrement=false;
	hProgressBar=NULL;
	InitializeCriticalSection(&updating_progress_bar);	
}

// free any resources used
ProgressWindow::~ProgressWindow()
{
	DeleteCriticalSection(&updating_progress_bar);
}

// create the progress window
int ProgressWindow::Create () 
{
	// create a new thread that will create the window & handle its messages
	DWORD threadId;
	HANDLE hBackgroundThread=CreateThread(NULL,0,&ThreadMain,(LPVOID)this,0,&threadId);    
}

// main execution function for progress window's thread
DWORD WINAPI ProgressWindow::ThreadMain(LPVOID lpParameter)
{
	ProgressWindow* thiswin=(ProgressWindow*)lpParameter;
    MSG messages;
 
 	thiswin->init();
 
 	// message loop   
    while(GetMessage(&messages, thiswin->GetHandle(), 0, 0))
    {
        // Translate keyboard events 
        TranslateMessage(&messages);
        // Send message to the associated window procedure 
  	    DispatchMessage(&messages);  	    
    }
}

// setup progress window
// called from progress window's own thread
void ProgressWindow::init()
{
    RECT rect;
    int mx,my;
    const int PROGRESS_WINDOW_WIDTH=175;
    const int PROGRESS_WINDOW_HEIGHT=75;
    
    
    // Get Overview Window Location for Progress window alignment
    GetWindowRect(overviewWindow.GetHandle(),&rect);
    
    // create prefs window
    int result;
    result=CreateWin(0, "Parbat3D Progress Bar Window", "Loading",
        WS_POPUP+WS_CAPTION,
	    10, 10, //rect.left+50, rect.bottom-150,
        PROGRESS_WINDOW_WIDTH, PROGRESS_WINDOW_HEIGHT,
        mainWindow.GetHandle(), NULL);
    assert(result==true);
	   
    
   	// set the background colour
	HBRUSH backBrush=CreateSolidBrush(GetSysColor(COLOR_3DFACE));
    SetBackgroundBrush(backBrush);
    
	// prevent main thread & progress window's thread doing this at same time
	EnterCriticalSection(&updating_progress_bar);
    {    
    
	    // create the progress bar
	    hProgressBar = CreateWindowEx(0, PROGRESS_CLASS, (LPSTR) NULL, WS_CHILD | WS_VISIBLE,
	                                  10, 10, 150, 25,
	                                  GetHandle(), (HMENU) 0, Window::GetAppInstance(), NULL);
	    
	    // set initial values for the progress bar
	    PostMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100)); 
	    PostMessage(hProgressBar, PBM_SETSTEP, (WPARAM) 1, 0);
	}	
	// allow other thread to update progress bar
    LeaveCriticalSection(&updating_progress_bar);
    
    
    // create a cancel button to stop loading
//    cancelButton = CreateWindowEx(0, "BUTTON", "Cancel", WS_CHILD | WS_VISIBLE, 120,
//            85, 55, 25, GetHandle(), (HMENU) 1, Window::GetAppInstance(), NULL);		
    
    
    prevProc = SetWindowProcedure(&WindowProcedure);	
    
    // progress bar will stick to screen edges even without being added to the manager
    //stickyWindowManager.AddStickyWindow(this);  // make the window stick to others
    
    // create timer for auto incrementing
    SetTimer(GetHandle(),1,100,NULL);
    
    // turn off looping once the end is reached
    loop_at_end = false;
    
    return;	
}


LRESULT CALLBACK ProgressWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {   
    static POINT moveMouseOffset;     /* mouse offset relative to window, used for snapping */
    static RECT rect;                 /* for general use */
    
    ProgressWindow* win = (ProgressWindow*) Window::GetWindowObject(hwnd);
    
    
    switch (message) {  // handle the messages
    	case WM_TIMER:
			EnterCriticalSection(&updating_progress_bar);
		    {
				if (win->autoIncrement)
				{
					win->increment();
				}
			}
			// allow other thread to update progress bar
			LeaveCriticalSection(&updating_progress_bar);				
			break;
    }
    
    
    // call the next procedure in the chain
    return CallWindowProc(win->prevProc, hwnd, message, wParam, lParam);    
}


void ProgressWindow::start (int steps, bool auto_increment) {

	// prevent main thread & progress window's thread doing this at same time
	EnterCriticalSection(&updating_progress_bar);
    {
		start_count++;
		
		// don't do anything unless progress bar has been setup
		if (hProgressBar==NULL)
			return;
		
		if (start_count==1)
		{
			autoIncrement=auto_increment;
			if (autoIncrement)
				loop_at_end=true;
			else
				loop_at_end=false;
	
		    total_steps = steps;  // set the total number of steps in the bar
		    current_steps = 0;    // initialise the current amount the bar has loaded
	
	
		    // sends a message to the progress bar to set the size and reset the position
		    PostMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, steps));  // resize
		    PostMessage(hProgressBar, PBM_SETPOS, (WPARAM) 0, 0);              // reset position
			    
		    Show();               		// shows the window
		 	mainWindow.DisableAll();	// disable all of the windows owned by the main thread		
			EnableWindow(GetHandle(),false);	// disable this window
		}

	}
	// allow other thread to update progress bar
	LeaveCriticalSection(&updating_progress_bar);
	

}


void ProgressWindow::end () {
	// prevent main thread & progress window's thread doing this at same time
	EnterCriticalSection(&updating_progress_bar);
    {
		start_count--;
	
		// don't do anything unless progress bar has been setup
		if (hProgressBar==NULL)
			return;
	
		if (start_count==0)
		{
		 	mainWindow.EnableAll();	// re-enable all of the windows owned by the main thread		
		 	progressWindow.Hide();	 				// hides the window
		}	
	}
	LeaveCriticalSection(&updating_progress_bar);
}


void ProgressWindow::reset () {
	// prevent main thread & progress window's thread doing this at same time
	EnterCriticalSection(&updating_progress_bar);
	{

		// don't do anything unless progress bar has been setup
		if (hProgressBar==NULL)
			return;
	
		    current_steps = 0;
		    PostMessage(hProgressBar, PBM_SETPOS, (WPARAM) 0, 0);  // update the progress bar with the reset position
	}
	// allow other thread to update progress bar
    LeaveCriticalSection(&updating_progress_bar);
}

void ProgressWindow::reset (int steps) {
	// prevent main thread & progress window's thread doing this at same time
	EnterCriticalSection(&updating_progress_bar);
    {
		// don't do anything unless progress bar has been setup
		if (hProgressBar==NULL)
			return;
	
	    total_steps = steps;
	    current_steps = 0;
	
	    // sends a message to the progress bar to set the size and reset the position
	    PostMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, steps));  // resize
	    PostMessage(hProgressBar, PBM_SETPOS, (WPARAM) 0, 0);              // reset position
	}
	// allow other thread update progress bar
    LeaveCriticalSection(&updating_progress_bar);
}


void ProgressWindow::increment () {
	// prevent main thread & progress window's thread doing this at same time
	EnterCriticalSection(&updating_progress_bar);
	{	

		// don't do anything unless progress bar has been setup
		if (hProgressBar==NULL)
			return;
		
		if (current_steps < total_steps)
		    current_steps++;
		else {
		    if (loop_at_end)
		        current_steps = 0;
		}
		
		// update the progress bar with the new value
		PostMessage(hProgressBar, PBM_SETPOS, (WPARAM) current_steps, 0);
	}	
	// allow other thread update progress bar
    LeaveCriticalSection(&updating_progress_bar);
}

void ProgressWindow::increment (int steps) {
	// prevent main thread & progress window's thread doing this at same time
	EnterCriticalSection(&updating_progress_bar);
	{

		// don't do anything unless progress bar has been setup
		if (hProgressBar==NULL)
			return;
	
	    if (current_steps <= (total_steps-steps))
	        current_steps += steps;
	    else {
	        if (loop_at_end && steps<total_steps) {
	            current_steps += steps;        // moves past the end of the progress bar
	            current_steps -= total_steps;  // reduces back to somewhere inside the bar
	        }
	    }
    
	    // update the progress bar with the new value
	    PostMessage(hProgressBar, PBM_SETPOS, (WPARAM) current_steps, 0);
	}
	// allow other thread update progress bar
    LeaveCriticalSection(&updating_progress_bar);
}
