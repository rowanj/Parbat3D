

class Window
{
    private:
    HWND hwnd;
    int CreateWin(DWORD dwExStyle,LPCTSTR lpClassName,LPCTSTR lpWindowName,DWORD dwStyle,int x,int y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam);
    
    public:
    HWND GetHandle() {return hwnd;};
    LRESULT static WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    Window* static GetWindowObject(HWND hwnd);
    static int RegisterWinClass(WNDCLASSEX*);
};
