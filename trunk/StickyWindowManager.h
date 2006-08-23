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
    POINT mouseOffset;
    Window *controller;
    vector<Window*> stickyWindows;

    int moveToInsideOfWindow(HWND snapToWin,RECT *rect);
    int moveToOutsideOfWindow(HWND snapToWin,RECT *rect);
    int GetStickyWindowId(Window *win);
            
    public:
    StickyWindowManager();
    void SetController(Window *controller);
    void AddStickyWindow(Window *sticky);
    //void RemoveStickyWindow(Window *sticky);
    //void beginSizing(Window* win);
    void beginMoving(Window* win);
    void onMoving(Window* win, RECT *rect);
    //void onSizing(Window* win);

};

#endif
