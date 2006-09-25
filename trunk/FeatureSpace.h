#ifndef _PARBAT_FeatureSpace_H
#define _PARBAT_FeatureSpace_H

#include <list>
#include <vector>
#include <hash_map>
#include "console.h"
#include "Window.h"
#include "GLView.h"
#include "GLContainer.h"
#include "ROI.h"
#include "ROISet.h"
#include "ROIFile.h"
#include "ROIWindow.h"
#include "ImageTileSet.h"

#define PIX_COORDS 1
#define BOUNDS_COORDS 2

typedef struct myPoint
{
	int x;
	int y;
};

typedef struct pixDataStruct
{
	int count;
	ROI* pixROI;	
};

typedef struct zListStruct
{
	int z;
	list<pixDataStruct*> pixData;
};

typedef struct yListStruct
{
	int y;
	list<zListStruct*> zList;
};

typedef struct xListStruct
{
	int x;
	list<yListStruct*> yList;
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
        
        vector<list<int>*> boundsCoords;
        vector<list<int>*> pixCoords;
		int maxy; 
		int miny;
		int yoffset;
		int vectorsize;
		vector<myPoint> polyPoints;
		ImageTileSet* fsTileset;
		int theLOD;
		int LODfactor;
		int LODwidth;
		int LODheight;
		int tileSize;
		bool onlyROIs;
		int band1;
		int band2;
		int band3;
		list<xListStruct*> pixelDataList;
        
        void getPixelData();
        void getPointData(ROIEntity* theEntity, ROI* theROI);
		void getRectData(ROIEntity* theEntity, ROI* theROI);
		void getPolygonData(ROIEntity* theEntity, ROI* theROI);
		void getPolygonVertices(ROIEntity* theEntity);
		void isTurningPoint(int first, int middle, int last);
		void generateBoundaryLine(int x1, int y1, int x2, int y2);
		void pushXPixelBounds(int rx, int y);
		void pushXPixel(int rx, int y);
		void fsListAdd(int x, int y, int z, ROI* theROI);
    	xListStruct* createNewXList(int x, int y, int z, ROI* theROI);
    	yListStruct* createNewYList(int y, int z, ROI* theROI);
    	zListStruct* createNewZList(int z, ROI* theROI);
    	pixDataStruct* createNewPixDataStruct(ROI* theROI);
    	
    protected:
        GLView *glview;
        GLContainer *glContainer;
        HWND hToolbar;
        HWND hRebarControl;
        int Create();
        void OnResize();    
        
    public:
        FeatureSpace(int LOD, bool only_ROIs, int b1, int b2, int b3);      // create & display new feature space window
        virtual void PaintGLContainer();            // draw contents of GLContainer with opengl
};

extern ROISet *regionsSet;

#endif
