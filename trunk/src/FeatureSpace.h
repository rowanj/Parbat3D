#ifndef _PARBAT_FeatureSpace_H
#define _PARBAT_FeatureSpace_H

#include "console.h"
#include "Window.h"
#include "GLContainer.h"
#include "ROI.h"
#include "ROISet.h"
#include "ROIFile.h"
#include "ROIWindow.h"
#include "ImageTileSet.h"
#include "FeatureSpaceGL.h"
#include "PointsHash.h"

#define PIX_COORDS 1
#define BOUNDS_COORDS 2

typedef struct
{
	int x;
	int y;
} myPoint;

class FeatureSpace:public Window, public GLContainerHandler
{
    private:
        WNDPROC prevProc;
        static int numFeatureSpaces;
        static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
		void init();
		
        static const int FEATURE_WINDOW_WIDTH;
        static const int FEATURE_WINDOW_HEIGHT;
        static const int TOOLBAR_HEIGHT;
        
        std::vector<std::list<int>*> boundsCoords;
        std::vector<std::list<int>*> pixCoords;
		int maxy; 
		int miny;
		int maxx;
		int minx;
		int yoffset;
		int vectorsize;
		std::vector<myPoint> polyPoints;
		ImageTileSet* fsTileset;
		points_hash_t fsImagePoints;
		points_hash_t fsROIPoints;
		points_hash_t::iterator hashiter;
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
		void addToImageFSTable(unsigned char b1, unsigned char b2, unsigned char b3);
		void addToROIFSTable(unsigned char b1, unsigned char b2, unsigned char b3);
    	
    protected:
		/* GUI stuff */
        GLContainer *glContainer;
        HWND hRebarControl;
		POINT initalMousePosition;								// mouse position at the time the mouse button is pushed down

        int Create();
        void OnResize();    
        virtual void PaintGLContainer();            			// draw contents of GLContainer with opengl
		void OnKeyPress(int virtualKey);						// handle key presses
		void OnGLContainerLeftMouseDown(int x,int y);			// handle left mouse button down event
		void OnGLContainerMouseMove(int vKeys,int x,int y);		// handle mouse move event
		void PanX(float amount);
		void PanY(float amount);
		void Rotate(float yaw_amount, float pitch_amount);
		
    public:
        FeatureSpace(int LOD, bool only_ROIs, int b1, int b2, int b3);	// create & display new feature space window

        /* OpenGL stuff */
        FeatureSpaceGL* fsgl;
};

#endif
