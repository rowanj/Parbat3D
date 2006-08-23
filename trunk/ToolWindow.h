#ifndef _PARBAT_TOOLWINDOW_H
#define _PARBAT_TOOLWINDOW_H

#include "ScrollBox.h"
#include "ToolTab.h"
#include "DisplayTab.h"
#include "QueryTab.h"
#include "ImageTab.h"
#include "FeatureTab.h"



class ToolWindow:public Window
{
    private:
       WNDPROC prevProc;
       HPEN hTabPen;
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
       HWND ToolWindow::hToolWindowImageTabHeading;
       HWND ToolWindow::hToolWindowQueryTabHeading;
       HWND ToolWindow::hToolWindowScrollBar;
       void freeDrawingObjects();       

     
      
      public:
       DisplayTab displayTab;                
       QueryTab queryTab;     
       ImageTab imageTab; 
       FeatureTab featureTab; 
       HFONT hBoldFont,hNormalFont,hHeadingFont;
       HBRUSH hTabBrush;
       HWND hToolWindowCurrentTabContainer;
       //HWND hToolWindowQueryTabContainer;
       HWND hToolWindowImageTabContainer;

       int bands;

       void drawStatic(DRAWITEMSTRUCT *dis, HFONT hfont);

       virtual int Create(HWND);
       static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
       static LRESULT CALLBACK ToolWindowTabControlProcedure(HWND, UINT, WPARAM, LPARAM);
       static LRESULT CALLBACK ToolWindowDisplayTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);
       static LRESULT CALLBACK ToolWindowQueryTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);
       static LRESULT CALLBACK ToolWindowImageTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);
       static LRESULT CALLBACK ToolWindowScrollBarProcedure(HWND, UINT, WPARAM, LPARAM);
       void showToolWindowTabContainer(int);     

       void SetCursorPosition(int x,int y); // update cursor position on query tab
       void SetImageBandValue(int band,int value); // update image band value on query tab
};


#endif

