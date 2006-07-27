
class DisplayWindow
{
   
    public:
    static int registerWindow();
    static char szDisplayClassName[];
    static CALLBACK LRESULT WindowProcedure(HWND, UINT, WPARAM, LPARAM);
};


