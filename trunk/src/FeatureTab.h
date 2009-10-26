#ifndef _PARBAT_FEATURETAB_H
#define _PARBAT_FEATURETAB_H

#include "ToolTab.h"

class FeatureTab:public ToolTab
{
    private:
        WNDPROC prevProc;
        HWND hTrackbar;
        HWND hX, hY, hZ;        
        HWND hgenerate;
        HWND *xRadiobuttons;                // band radio buttons
        HWND *yRadiobuttons;
        HWND *zRadiobuttons;
        HWND hROIOnly;
        HWND htitle;
        void OnGenerateClicked(int lod, bool rois_only, int x, int y, int z);
        void OnUserMessage();
        static int guessPoints(int slider_pos);
    public:
        const char* GetTabName() {return "Feature";};
        const char* GetTabHeading() {return "Feature Space";};
        int GetContainerHeight();   
        int Create(HWND parent,RECT *parentRect);
        static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);    
    
};

#endif
