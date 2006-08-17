#ifndef _PARBAT_TOOLWINDOW_H
#define _PARBAT_TOOLWINDOW_H

class ToolWindow:public Window
{
    private:
       WNDPROC prevProc;
       HWND hRed, hBlue, hGreen;
       HWND *redRadiobuttons;                // band radio buttons
       HWND *greenRadiobuttons;
       HWND *blueRadiobuttons;
       HWND hupdate;
       HPEN hTabPen;
       HBRUSH hTabBrush;
       WNDPROC oldTabControlProc,oldDisplayTabContainerProc,oldQueryTabContainerProc,oldImageTabContainerProc,oldScrollBarContainerProc;
       char szToolWindowClassName[];
       void draw(DRAWITEMSTRUCT *dis, HFONT hfont);
       void drawTab(DRAWITEMSTRUCT *dis);
       void measureTab(MEASUREITEMSTRUCT *mis);
       void setupDrawingObjects(HWND hwnd);
       void scrollToolWindowToTop();
       void updateToolWindowScrollbar();
       void scrollToolWindow(int msg);
       HWND hToolWindowTabControl;
       HWND hToolWindowDisplayTabHeading;
       HWND ToolWindow::hToolWindowImageTabHeading;
       HWND ToolWindow::hToolWindowQueryTabHeading;
       HWND ToolWindow::hToolWindowScrollBar;
       void freeDrawingObjects();
       
       void drawStatic(DRAWITEMSTRUCT *dis, HFONT hfont);
       
      
      public:
       HFONT hBoldFont,hNormalFont,hHeadingFont;

       HWND hToolWindowCurrentTabContainer;
       HWND hToolWindowDisplayTabContainer;
       HWND hToolWindowQueryTabContainer;
       HWND hToolWindowImageTabContainer;

       HWND cursorXPos, cursorYPos;
       int bands;
       HWND *imageBandValues;

       virtual int Create(HINSTANCE hInstance);
       static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
       static LRESULT CALLBACK ToolWindowTabControlProcedure(HWND, UINT, WPARAM, LPARAM);
       static LRESULT CALLBACK ToolWindowDisplayTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);
       static LRESULT CALLBACK ToolWindowQueryTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);
       static LRESULT CALLBACK ToolWindowImageTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);
       static LRESULT CALLBACK ToolWindowScrollBarProcedure(HWND, UINT, WPARAM, LPARAM);
       void showToolWindowTabContainer(int);      
};


#endif

