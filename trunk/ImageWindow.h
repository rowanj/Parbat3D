
class ImageWindow
{
    public:
    static HWND hImageWindow;
    static DisplayWindow imageWindowDisplay;

    static LRESULT CALLBACK ImageWindowProcedure(HWND, UINT, WPARAM, LPARAM);
    static int registerImageWindow();
    static int setupImageWindow();
    static void updateImageWindowTitle();
    static void updateImageScrollbar();
    static void scrollImageX(int scrollMsg);
    static void scrollImageY(int scrollMsg);    
    static void zoomImage(int nlevels);
};


