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
    StickyWindowManager();
    void SetController(Window *controller);
    void AddStickyWindow(Window *sticky);
    void RemoveStickyWindow(Window *sticky);
    //void BeginSizing(Window* win);
    void BeginMoving(Window* win);
    void OnMoving(Window* win, RECT *rect);
    //void OnSizing(Window* win);

};

#endif
