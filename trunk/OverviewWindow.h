
class OverviewWindow
{
    public:
       
    static HMENU hMainMenu;
    static HWND hOverviewWindow;
    static HWND hOverviewWindowDisplay;
    static const int OVERVIEW_WINDOW_WIDTH=250;    /* width of the overview window in pixels */
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
    static int toggleMenuItemTick(HMENU hMenu,int itemId);  
    static int registerWindow();
    static int setupWindow();   
};


