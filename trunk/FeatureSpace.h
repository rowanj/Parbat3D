#ifndef _PARBAT_FeatureSpace_H
#define _PARBAT_FeatureSpace_H

#include <list>
#include <vector>
#include "console.h"
#include "Window.h"
#include "GLView.h"
#include "GLContainer.h"
#include "ROI.h"
#include "ROISet.h"

typedef struct myPoint
{
	int x;
	int y;
};

class FeatureSpace:public Window, public GLContainerHandler
{
    private:
        WNDPROC prevProc;
        static int numFeatureSpaces;
        static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
        static const int FEATURE_WINDOW_WIDTH;
        static const int FEATURE_WINDOW_HEIGHT;
        static const int TOOLBAR_HEIGHT;
        float bbox_size;
        
        vector<list<int>*> pixCoords;
		int maxy; 
		int miny;
		int yoffset;
		int vectorsize;
		vector<myPoint> polyPoints;
        
        void getPixelData();
		void getRectData(ROIEntity* theEntity);
		void getPolygonData(ROIEntity* theEntity);
		void getPolygonVertices(ROIEntity* theEntity);
		void isTurningPoint(int first, int middle, int last);
		void generateBoundaryLine(int x1, int y1, int x2, int y2);
		void pushXPixel(int rx, int y);
		void addPointToFSLists(int x, int y, ROI* theROI);
    
    protected:
        GLView *glview;
        GLContainer *glContainer;
        HWND hToolbar;
        HWND hRebarControl;
        int Create();
        void OnResize();    
        
    public:
        FeatureSpace(int LOD, bool only_ROIs);      // create & display new feature space window
        virtual void PaintGLContainer();            // draw contents of GLContainer with opengl
};

#endif
