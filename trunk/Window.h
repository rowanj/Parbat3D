#include <Windows.h>

class Window
{
    protected:
    HWND hwindow;
    WNDPROC prevWindowProcedure;
    static WNDPROC stPrevWindowProcedure;
    int CreateWin(DWORD dwExStyle,LPCTSTR lpClassName,LPCTSTR lpWindowName,DWORD dwStyle,int x,int y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance);
    static void Window::SetWindowObject(HWND hwnd,Window* obj);
    
    public:
    char *testString;
    virtual int Create(HINSTANCE hInstance);
    virtual int CreateC(HINSTANCE hInstance,HWND hwnd);
        
    Window::Window();
    HWND GetHandle() {return hwindow;};
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
    static Window* GetWindowObject(HWND);
    static int RegisterWinClass(WNDCLASSEX*);
    WNDPROC SetWindowProcedure(WNDPROC);
    void Destroy() {DestroyWindow(hwindow);};
};
