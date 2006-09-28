#ifndef _PARBAT_FeatureSpace_H
#define _PARBAT_FeatureSpace_H

#include <list>
#include <vector>
#include "console.h"
#include "Window.h"
#include "GLContainer.h"
#include "ROI.h"
#include "ROISet.h"
#include "ROIFile.h"
#include "ROIWindow.h"
#include "ImageTileSet.h"
#include "PointsHash.h"
#include "FeatureSpaceGL.h"

#define PIX_COORDS 1
#define BOUNDS_COORDS 2

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
        
        vector<list<int>*> boundsCoords;
        vector<list<int>*> pixCoords;
		int maxy; 
		int miny;
		int yoffset;
		int vectorsize;
		vector<myPoint> polyPoints;
		ImageTileSet* fsTileset;
		points_hash_t fsPoints;
		points_hash_t::iterator hashiter;
		unsigned int maxPixelCount;
		unsigned int numberPoints;
		int theLOD;
		int LODfactor;
		int LODwidth;
		int LODheight;
		int tileSize;
		bool onlyROIs;
		int band1;
		int band2;
		int band3;
   		
   		// Input stuff
   		int prev_mouse_x;
   		int prev_mouse_y;

        void getPixelData(void);
        void getImageData(void);
        void getPointData(ROIEntity* theEntity, ROI* theROI);
		void getRectData(ROIEntity* theEntity, ROI* theROI);
		void getPolygonData(ROIEntity* theEntity, ROI* theROI);
		void getPolygonVertices(ROIEntity* theEntity);
		void isTurningPoint(int first, int middle, int last);
		void generateBoundaryLine(int x1, int y1, int x2, int y2);
		void pushXPixelBounds(int rx, int y);
		void pushXPixel(int rx, int y);
		unsigned int catForHash(unsigned char b1, unsigned char b2, unsigned char b3);
		void addToFSTable(unsigned char b1, unsigned char b2, unsigned char b3);
		void addToFSTable(unsigned char b1, unsigned char b2, unsigned char b3, ROI* theROI);
    	
    protected:
		/* GUI stuff */
        GLContainer *glContainer;
        HWND hToolbar;
        HWND hRebarControl;
		POINT initalMousePosition;					// mouse position at the time the mouse button is pushed down

        int Create();
        void OnResize();    
        virtual void PaintGLContainer();            // draw contents of GLContainer with opengl
		void OnKeyPress(int virtualKey);			// handle key presses
		void OnGLContainerLeftMouseDown(int x,int y);			// handle left mouse button down event
		void OnGLContainerMouseMove(int vKeys,int x,int y);	// handle mouse move event
		
    public:
        FeatureSpace(int LOD, bool only_ROIs, int b1, int b2, int b3);      // create & display new feature space window

        /* OpenGL stuff */
        FeatureSpaceGL* fsgl;
};

#endif
