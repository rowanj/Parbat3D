#include "PchApp.h"

#include "ProgressWindow.h"

#include "main.h"


const int WM_USER_START=WM_USER;
const int WM_USER_STOP=WM_USER+1;
const int WM_USER_INCREMENT=WM_USER+2;
const int WM_USER_RESET=WM_USER+3;


// setup variables
ProgressWindow::ProgressWindow()
{
	start_count=0;
	autoIncrement=false;
	hProgressBar=NULL;
}

// free any resources used
ProgressWindow::~ProgressWindow()
{
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
    while(GetMessage(&messages, NULL, 0, 0))
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
    GetWindowRect(GetDesktopWindow(),&rect);
    
    // create prefs window
    int result;
    result=CreateWin(0, "Parbat3D Progress Bar Window", "Parbat3D Loading...",
        WS_POPUP+WS_CAPTION,
	    (rect.right-PROGRESS_WINDOW_WIDTH)/2, (rect.bottom-PROGRESS_WINDOW_HEIGHT)/2, //rect.left+50, rect.bottom-150,
        PROGRESS_WINDOW_WIDTH, PROGRESS_WINDOW_HEIGHT,
        mainWindow.GetHandle(), NULL);
    assert(result==true);
	   
    
   	// set the background colour
	HBRUSH backBrush=CreateSolidBrush(GetSysColor(COLOR_3DFACE));
    SetBackgroundBrush(backBrush);
    
    
    // create the progress bar
    hProgressBar = CreateWindowEx(0, PROGRESS_CLASS, (LPSTR) NULL, WS_CHILD | WS_VISIBLE,
                                  10, 10, 150, 25,
                                  GetHandle(), (HMENU) 0, Window::GetAppInstance(), NULL);
    
    // set initial values for the progress bar
    PostMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100)); 
    PostMessage(hProgressBar, PBM_SETSTEP, (WPARAM) 1, 0);
    
    
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


// handle messages related to the progress window
LRESULT CALLBACK ProgressWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {   
    static POINT moveMouseOffset;     /* mouse offset relative to window, used for snapping */
    static RECT rect;                 /* for general use */
    
    ProgressWindow* win = (ProgressWindow*) Window::GetWindowObject(hwnd);
    
    
    switch (message) {  // handle the messages
    	case WM_TIMER:
			if (win->autoIncrement)
			{
				win->increment();
			}
			break;
			
		case WM_USER_START:
			win->message_start((int)wParam,(bool)lParam);
			break;
			
		case WM_USER_STOP:
			win->message_end();
			break;
			
		case WM_USER_RESET:
			win->message_reset();
			break;
			
		case WM_USER_INCREMENT:
			
			if (wParam==0)
				win->message_increment();
			else
				win->message_increment(wParam);
			break;
    }
    
    
    // call the next procedure in the chain
    return CallWindowProc(win->prevProc, hwnd, message, wParam, lParam);    
}



void ProgressWindow::start (int steps, bool auto_increment) {
 	mainWindow.DisableAll();	// disable all of the windows owned by the main thread			

	SendMessage(GetHandle(),WM_USER_START,steps,auto_increment); // send message to progress window's own thread
}

void ProgressWindow::message_start(int steps, bool auto_increment)
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
		EnableWindow(GetHandle(),false);	// disable this window
	}

}


void ProgressWindow::end()
{
	SendMessage(GetHandle(),WM_USER_STOP,0,0); 	// send message to progress window's own thread
 	mainWindow.EnableAll();	// re-enable all of the windows owned by the main thread
}

void ProgressWindow::message_end()
{
	start_count--;

	// don't do anything unless progress bar has been setup
	if (hProgressBar==NULL)
		return;

	if (start_count==0)
	{
	 	progressWindow.Hide();	 				// hides the window
	}	
}

void ProgressWindow::reset()
{
	SendMessage(GetHandle(),WM_USER_RESET,0,0); 	// send message to progress window's own thread
}

void ProgressWindow::message_reset () {

	// don't do anything unless progress bar has been setup
	if (hProgressBar==NULL)
		return;

	    current_steps = 0;
	    PostMessage(hProgressBar, PBM_SETPOS, (WPARAM) 0, 0);  // update the progress bar with the reset position
}

void ProgressWindow::reset (int steps) {
	// don't do anything unless progress bar has been setup
	if (hProgressBar==NULL)
		return;

    total_steps = steps;
    current_steps = 0;

    // sends a message to the progress bar to set the size and reset the position
    PostMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, steps));  // resize
    PostMessage(hProgressBar, PBM_SETPOS, (WPARAM) 0, 0);              // reset position
}


void ProgressWindow::increment()
{
	SendMessage(GetHandle(),WM_USER_INCREMENT,0,0); 	// send message to progress window's own thread
}

void ProgressWindow::message_increment () {

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

void ProgressWindow::increment(int steps)
{
	SendMessage(GetHandle(),WM_USER_INCREMENT,steps,0); 	// send message to progress window's own thread
}

void ProgressWindow::message_increment (int steps) {

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
