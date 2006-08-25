#ifndef _PARBAT_STICKYWINDOWMANAGER_H
#define _PARBAT_STICKYWINDOWMANAGER_H

#include <vector>
#include "Window.h"
#include <Windows.h>
using namespace std;

class StickyWindowManager
{
    private:
    HWND hDesktop;
    POINT prevMousePosition;    
    Window *controller;
    vector<Window*> stickyWindows;
    vector<Window*> snappedWindows;
    vector<Window*> nonSnappedWindows;

    int moveToInsideOfWindow(HWND snapToWin,RECT *rect);
    int moveToOutsideOfWindow(HWND snapToWin,RECT *rect);
    int GetStickyWindowId(Window *win);
    char isConnected(Window *win1,Window *win2);
    int isWindowInNormalState(HWND hwnd);
    void moveWindowByOffset(HWND hwnd,int xOffset,int yOffset);
    
    public:
    StickyWindowManager();                      // constructor
    
    void SetController(Window *controller);     // set the window that forces other connected windows to move around with it
    void AddStickyWindow(Window *sticky);       // register a window so that it will snap to other windows 
    void RemoveStickyWindow(Window *sticky);    // unregister a window so that it will no longer snap to other windows
    //void BeginSizing(Window* win);            // not implemented
    void BeginMoving(Window* win);              // called when a window is about to be dragged by the user
    void OnMoving(Window* win, RECT *rect);     // called when a window is being dragged by the user
    //void OnSizing(Window* win);               // not implemented

};

#endif
