#ifndef _PARBAT_QUERYTAB_H
#define _PARBAT_QUERYTAB_H

#include "ToolTab.h"

class QueryTab:public ToolTab
{
    private:
        WNDPROC prevProc;       
    public:
       HWND *imageBandValues;
       HWND cursorXPos, cursorYPos;
        
        char* GetTabName() {return "Query";};
        char* GetTabHeading() {return "Band Values";};
        int GetContainerHeight();   
        int Create(HWND parent,RECT *parentRect);
        static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);  
        
        void SetCursorX(char *text);  // set text on cursorX control
        void SetCursorY(char *text);  // set text on cursorY control
        void SetImageBandValue(int i,char *text);   // set text on imageBandValue[i] control
    
};

#endif
