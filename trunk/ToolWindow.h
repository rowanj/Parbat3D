#ifndef _PARBAT_TOOLWINDOW_H
#define _PARBAT_TOOLWINDOW_H


class ToolTab:public Window
{
    protected:
        WNDPROC prevProc;
    public:
        HWND hHeading;        
        virtual int Create(HWND parent,RECT *parentRect);
        virtual char* GetTabName() {return NULL;};
        virtual char* GetTabHeading() {return NULL;};
        virtual int GetContainerHeight() {return 0;};
        static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);    
};


class DisplayTab:public ToolTab
{
    private:
        WNDPROC prevProc;
        HWND hRed, hBlue, hGreen;        
        HWND hupdate;
        HWND *redRadiobuttons;                // band radio buttons
        HWND *greenRadiobuttons;
        HWND *blueRadiobuttons;
    public:
        char* GetTabName() {return "Display";};
        char* GetTabHeading() {return "Channel Selection";};
        int GetContainerHeight();   
        int Create(HWND parent,RECT *parentRect);
        static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);    
    
};



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
       HFONT hBoldFont,hNormalFont,hHeadingFont;
       HBRUSH hTabBrush;
       HWND hToolWindowCurrentTabContainer;
       HWND hToolWindowQueryTabContainer;
       HWND hToolWindowImageTabContainer;

       HWND cursorXPos, cursorYPos;
       int bands;
       HWND *imageBandValues;

       void drawStatic(DRAWITEMSTRUCT *dis, HFONT hfont);

       virtual int Create(HWND);
       static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
       static LRESULT CALLBACK ToolWindowTabControlProcedure(HWND, UINT, WPARAM, LPARAM);
       static LRESULT CALLBACK ToolWindowDisplayTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);
       static LRESULT CALLBACK ToolWindowQueryTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);
       static LRESULT CALLBACK ToolWindowImageTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);
       static LRESULT CALLBACK ToolWindowScrollBarProcedure(HWND, UINT, WPARAM, LPARAM);
       void showToolWindowTabContainer(int);      
};


#endif

