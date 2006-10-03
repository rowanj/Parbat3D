#ifndef _PARBAT_PROGRESS_WINDOW_H
#define _PARBAT_PROGRESS_WINDOW_H

#include <Windows.h>

#include "Window.h"


class ProgressWindow:public Window {
    private:
        WNDPROC prevProc;
        
        HWND hProgressBar;
//        HWND cancelButton;
        
        int current_steps;
        int total_steps;
        
        bool loop_at_end;
        
        
    public:
        //DisplayWindow ProgressWindowDisplay;
        
        /**
            Empty constructor.
        */
        ProgressWindow () {}
        
        
        /**
            Create the progress bar window and sets up all the components that
            are inside it.
        */
        int Create (HWND parent);
        
        
        /**
            Handles any events that are related to the progress window.
        */
        static LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
        
        
        /**
            Opens the progress bar window with a set number of steps to
            complete.
        */
        void start (int steps);
        
        
        /**
            Closes the progress bar window to show that progress is complete.
            This can be called even if the required number of steps have not
            been completed yet.
        */
        void end ();
        
        
        /**
            Resets the counter to zero.
        */
        void reset ();
        
        /**
            Resets the counter to zero and sets a new amount of steps to
            complete. Basically does the same thing as start() but will not show
            the progress bar if it is hidden.
        */
        void reset (int steps);
        
        
        /**
            Increase the number of steps that have been completed so far by one.
        */
        void increment ();
        
        /**
            Increase the number of steps that have been completed so far by the
            given amount.
        */
        void increment (int steps);        
        
        
        /**
            Returns true if the total number of steps have been completed.
        */
        bool isComplete () { return (current_steps>=total_steps); }
        
        
        /**
            If set to true then once the progress bar reaches the end, it will
            reset and start loading again. This allows the progress bar to show
            time passing rather than an amount of the total.
            Looping if turned off by default.
        */
        void setLooping (bool loop) { loop_at_end = loop; }
};

#endif
