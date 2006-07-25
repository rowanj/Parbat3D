
class MainWindow
{
    private:
    static char szMainWindowClassName[];

    public:
    static HWND hMainWindow;
    static LRESULT CALLBACK MainWindowProcedure(HWND, UINT, WPARAM, LPARAM);
    static int registerMainWindow();
    static int setupMainWindow();
};


