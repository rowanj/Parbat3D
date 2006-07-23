
extern HWND hToolWindow;
extern HFONT hBoldFont,hNormalFont,hHeadingFont;

extern HWND hToolWindowCurrentTabContainer;
extern HWND hToolWindowDisplayTabContainer;
extern HWND hToolWindowQueryTabContainer;
extern HWND hToolWindowImageTabContainer;

extern HWND cursorXPos, cursorYPos;
extern int bands;
extern HWND *imageBandValues;

LRESULT CALLBACK ToolWindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolWindowTabControlProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolWindowDisplayTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolWindowQueryTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolWindowImageTabContainerProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolWindowScrollBarProcedure(HWND, UINT, WPARAM, LPARAM);
int registerToolWindow();
int setupToolWindow();
void showToolWindowTabContainer(int);


