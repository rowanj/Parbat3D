
class ToolWindow
{
      private:
      static HWND hRed, hBlue, hGreen;
      static HWND *redRadiobuttons;                // band radio buttons
      static HWND *greenRadiobuttons;
      static HWND *blueRadiobuttons;
      static HWND hupdate;
      static HPEN hTabPen;
      static HBRUSH hTabBrush;
      static WNDPROC oldTabControlProc,oldDisplayTabContainerProc,oldQueryTabContainerProc,oldImageTabContainerProc,oldScrollBarContainerProc;
      static char szToolWindowClassName[];
      static void drawStatic(DRAWITEMSTRUCT *dis, HFONT hfont);
      static void drawTab(DRAWITEMSTRUCT *dis);
      static void measureTab(MEASUREITEMSTRUCT *mis);
      static void setupDrawingObjects(HWND hwnd);
      static void scrollToolWindowToTop();
      static void updateToolWindowScrollbar();
      static void scrollToolWindow(int msg);
      static HWND hToolWindowTabControl;
      static HWND hToolWindowDisplayTabHeading;
      static HWND ToolWindow::hToolWindowImageTabHeading;
      static HWND ToolWindow::hToolWindowQueryTabHeading;
      static HWND ToolWindow::hToolWindowScrollBar;
      static void freeDrawingObjects();
      
      public:
      static HWND hToolWindow;
      static HFONT hBoldFont,hNormalFont,hHeadingFont;

      static HWND hToolWindowCurrentTabContainer;
      static HWND hToolWindowDisplayTabContainer;
      static HWND hToolWindowQueryTabContainer;
      static HWND hToolWindowImageTabContainer;

      static HWND cursorXPos, cursorYPos;
      static int bands;
      static HWND *imageBandValues;

      static LRESULT CALLBACK ToolWindowProcedure(HWND, UINT, WPARAM, LPARAM);
      static LRESULT CALLBACK ToolWindowTabControlProcedure(HWND, UINT, WPARAM, LPARAM);
      static LRESULT CALLBACK ToolWindowDisplayTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);
      static LRESULT CALLBACK ToolWindowQueryTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);
      static LRESULT CALLBACK ToolWindowImageTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);
      static LRESULT CALLBACK ToolWindowScrollBarProcedure(HWND, UINT, WPARAM, LPARAM);
      static int registerToolWindow();
      static int setupToolWindow();
      static void showToolWindowTabContainer(int);      
};




