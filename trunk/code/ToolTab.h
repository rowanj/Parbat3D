#ifndef _PARBAT_TOOLTAB_H
#define _PARBAT_TOOLTAB_H


class ToolTab:public Window
{
    protected:
        WNDPROC prevProc;
        ScrollBox scrollBox;
    public:
        HWND hHeading;        
        virtual int Create(HWND parent,RECT *parentRect);
        virtual char* GetTabName() {return NULL;};
        virtual char* GetTabHeading() {return NULL;};
        virtual int GetContainerHeight() {return 0;};
        static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);    
};

#endif
