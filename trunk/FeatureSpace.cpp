#include <Windows.h>
#include <math.h>
#include "main.h"
#include "Window.h"
#include "FeatureSpace.h"
#include "GLContainer.h"
#include "Config.h"
#include "Console.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include <commctrl.h>

#define POLYDATA_DEBUG 0
#define ENABLE_ROI_POINTS 1

int FeatureSpace::numFeatureSpaces=0;

const int FeatureSpace::FEATURE_WINDOW_WIDTH=650;       // inital width of feature space window
const int FeatureSpace::FEATURE_WINDOW_HEIGHT=550;      // inital height of feature space window
const int FeatureSpace::TOOLBAR_HEIGHT=30;              // height of toolbar area at bottom of window

extern ROISet *regionsSet;

// timer used for testing
#include <time.h>   
void sleep(unsigned int mseconds) {
    clock_t goal = mseconds + clock();
    while (goal > clock());
}



// create & display new feature space window
FeatureSpace::FeatureSpace(int LOD, bool only_ROIs, int b1, int b2, int b3) {
    theLOD = LOD;
    onlyROIs = only_ROIs;
    band1 = b1;
    band2 = b2;
    band3 = b3;
    tileSize = 512;
    numberPoints = 0;
    Console::write("FeatureSpace -- Our LOD is %d\n", theLOD);
    Console::write("FeatureSpace -- Band 1 = %d, Band 2 = %d, Band 3 = %d\n", band1, band2, band3);

	progressWindow.start(100,true); 	// show progress window & disable other windows
	init();						// setup feature space
	progressWindow.end();   // hide progress window & enable other windows

    Console::write("FeatureSpace -- done\n");  
}


// setup feature space
void FeatureSpace::init()
{
	int createSuccess;
    
    fsgl=NULL;
    createSuccess=Create();
    assert(createSuccess);
       
    numFeatureSpaces++;
    
    Console::write("FeatureSpace -- seting up opengl stuff...\n");
	fsgl = new FeatureSpaceGL(glContainer->GetHandle(), theLOD, band1, band2, band3);

    Console::write("FeatureSpace -- getting pixel data...\n");
    getPixelData();

	Console::write("FeatureSpace -- calling OnResize\n");    
    OnResize();
	
    Show();
}

// draw contents of GLContainer with opengl
void FeatureSpace::PaintGLContainer() {
	fsgl->draw();
}

//getPixelData gets all point coordinates from within ROIs, and
//then gets all data values for those pixel coords, and stores
//them, ready for display list generation.
void FeatureSpace::getPixelData(void)
{
	vector<ROI*> theROIs = regionsSet->get_regions();
	Console::write("FS::GpixD\tCount of regions is %d\n", regionsSet->get_regions_count());
	assert(regionsSet != NULL);
	Console::write("FS::GpixD\tGot vector of ROIs\n");
	
    ROI* currentROI;
	
	fsTileset = new ImageTileSet(theLOD, image_handler->get_image_file(), tileSize, 128);
	LODfactor = fsTileset->get_LOD_factor();
	LODwidth = fsTileset->get_LOD_width();
	LODheight = fsTileset->get_LOD_height();
	
	getImageData(); // Populate fsAllPoints, as
	
	Console::write("FS::GpixD\tLOD factor is %d\n", LODfactor);

	#if ENABLE_ROI_POINTS
	if (!theROIs.empty()) //if there are some ROIs in the set
	{
		Console::write("FS::GpixD\tGot past theROIs empty check\n");
		for (int cr = 0; cr < theROIs.size(); cr++)
		{
			fsROIPoints.clear(); // Clear the ROI hash
			Console::write("FS::GpixD\tGot into the ROIs loop\n");
			currentROI = theROIs.at(cr);
			vector<ROIEntity*> theEntities = currentROI->get_entities();
			Console::write("FS::GpixD\tGot vector of entities\n");
			if (!theEntities.empty())
			{
				Console::write("FS::GpixD\tSet our current ROI\n");
				ROIEntity* currentEntity;
				if (currentROI->get_active())
				{
					Console::write("FS::GpixD\tCurrent ROI is active -- checking types\n");
					for(int ce = 0; ce < theEntities.size(); ce++)
					{
						currentEntity = theEntities.at(ce);
						Console::write("FS::GpixD\tSet our current entity\n");
						
						char* theType = (char*)currentEntity->get_type();
						
						if(theType == ROI_POINT) //ROI == point
						{
							Console::write("FS::GpixD\tCurrent type is POINT\n");
							//add data at point to data lists
							getPointData(currentEntity, currentROI);
						}
						else if(theType == ROI_RECT) //ROI == rectangle
						{
							Console::write("FS::GpixD\tCurrent type is RECT\n");
							//add data at all points in rectangle to data lists
							getRectData(currentEntity, currentROI);
						}
					}
				}
			}
			else
			{
				Console::write("FS::GpixD\tthe entities set is empty\n");	
			}
			int red, green, blue;
			currentROI->get_colour(&red, &green, &blue);
			fsgl->add_points(fsROIPoints, red, green, blue);
		}
		fsROIPoints.clear(); // Clear the ROI hash
	}
	else
	{
		Console::write("FS::GpixD\tthe ROIs set is empty\n");
	}
	#endif // ENABLE_ROI_POINTS
	
	fsgl->add_points(fsImagePoints, 255, 255, 255);
	fsImagePoints.clear();

	delete fsTileset;
	
	Console::write("FS::GpixD\tTileset destroyed.\n");
}

// Get entire image into ROI
void FeatureSpace::getImageData(void)
{
	int ncols = fsTileset->get_columns();
	int nrows = fsTileset->get_rows();
	unsigned char* tile;
//	const unsigned char top_clip = 250;
//	const unsigned char bottom_clip = 5;

	for (int row = 0; row < nrows; row++) {
		for (int col = 0; col < ncols; col++) {
			tile = (unsigned char*) fsTileset->get_tile_RGB_LOD(col * tileSize, row * tileSize, band1, band2, band3);
			for (int pos = 0; pos < tileSize * tileSize * 3; pos = pos + 3) {
//				if (tile[pos] < top_clip && tile[pos+1] < top_clip && tile[pos+2] < top_clip &&
//				    tile[pos] > bottom_clip && tile[pos+1] > bottom_clip && tile[pos+2] > bottom_clip) {
					addToImageFSTable(tile[pos], tile[pos+1], tile[pos+2]);
					numberPoints++;
//				}
			}
			delete[] tile;
		}
	}
}

// -----------------------------------------------------------------------------------------
// getPointData
// -----------------------------------------------------------------------------------------
// Adds a point to the pix data lists
// -----------------------------------------------------------------------------------------
void FeatureSpace::getPointData(ROIEntity* theEntity, ROI* theROI)
{
	vector<coords> theCoords = theEntity->get_points();
	int x = theCoords[0].x;
	int y = theCoords[0].y;
	int fx = x / LODfactor;
	int fy = y / LODfactor;
	int fromTileXOrigin = (fx / tileSize) * tileSize;
	int fromTileYOrigin = (fy / tileSize) * tileSize;
	int tilex = fx % tileSize;
	int tiley = fy % tileSize;
	
	Console::write("tile X orig = %d, tile Y orig = %d\n", fromTileXOrigin, fromTileYOrigin);
	Console::write("fx = %d, fy = %d\n", fx, fy);
	Console::write("x pos in tile = %d, y pos in tile = %d\n", tilex, tiley);
	
	char* grabbedData = fsTileset->get_tile_RGB_LOD(fx, fy, band1, band2, band3);
	
	Console::write("got past tile load\n");
	
	int offset = (tiley * tileSize * 3) + (tilex * 3);
	unsigned char b1 = (unsigned char)grabbedData[offset];
	unsigned char b2 = (unsigned char)grabbedData[offset + 1];
	unsigned char b3 = (unsigned char)grabbedData[offset + 2];
	
	addToROIFSTable(b1, b2, b3);
	delete[] grabbedData;
}

// -----------------------------------------------------------------------------------------
// getRectData
// -----------------------------------------------------------------------------------------
// Gets data for all points inside a given rect
// -----------------------------------------------------------------------------------------

void FeatureSpace::getRectData(ROIEntity* theEntity, ROI* theROI)
{
	vector<coords> theCoords = theEntity->get_points();
	int x1, x2, y1, y2;
	int fx1, fx2, fy1, fy2;
	int startx, starty, endx, endy;
	
	//init our rectangle coords
	x1 = theCoords[0].x;
	y1 = theCoords[0].y;
	x2 = theCoords[1].x;
	y2 = theCoords[1].y;
	
	//swap the coords if p2 points greater than p1
	if (x2 < x1)
	{
		int temp = x1;
		x1 = x2;
		x2 = temp;
	}
	if (y2 < y1)
	{
		int temp = y1;
		y1 = y2;
		y2 = temp;
	}
	
	//get our x and y in terms of the LOD
	fx1 = x1 / LODfactor;
	fy1 = y1 / LODfactor;
	fx2 = x2 / LODfactor;
	fy2 = y2 / LODfactor;
	
	Console::write("FS::Rect - Rect LOD coords are x1 = %d, y1 = %d, x2 = %d, y2 = %d\n", fx1, fy1, fx2, fy2);
	
	//work out the origin points of our start and end tiles
	startx = (fx1 / tileSize) * tileSize;
	starty = (fy1 / tileSize) * tileSize;
	endx = (fx2 / tileSize) * tileSize;
	endy = (fy2 / tileSize) * tileSize;
	
	Console::write("FS::Rect - StartX = %d, StartY = %d, EndX = %d, EndY = %d\n", startx, starty, endx, endy);
	
	//loop through all tiles that have some points we're interested in
	for(int tiley = starty; tiley <= endy; tiley+=tileSize)
	{
		for(int tilex = startx; tilex <= endx; tilex+=tileSize)
		{
			Console::write("FS::Rect - In tile starting at x %d, y %d\n", tilex, tiley);
			int x, y;
			
			char* grabbedData = fsTileset->get_tile_RGB_LOD(tilex, tiley, band1, band2, band3);
			
			for(y = max(tiley, fy1); y < min((fy2 + 1), tiley + tileSize); y++)
			{
				for(x = max(tilex, fx1); x < min((fx2 + 1), tilex + tileSize); x++)
				{
					//Console::write("FS::Rect - Getting point at x %d, y %d\n", x, y);
					int offx = x % tileSize;
					int offy = y % tileSize;
					int offset = (offy * tileSize * 3) + (offx * 3);
					unsigned char b1 = (unsigned char)grabbedData[offset];
					unsigned char b2 = (unsigned char)grabbedData[offset + 1];
					unsigned char b3 = (unsigned char)grabbedData[offset + 2];
					addToROIFSTable(b1, b2, b3);
				}
			}
			delete[] grabbedData;
		}
	}
}

// -----------------------------------------------------------------------------------------
// getPolygonData
// -----------------------------------------------------------------------------------------
// This function creates a list of the coordinates of all points circumscribed by a
// polygon. It does so by recording the x coordinate for any points at a given y coordinate.
// The algorithm generates these boundary points in sorted order from lowest x to highest x,
// and there will always be an even number of these values.
// The algorithm then gets the pixel value of any point x, y, where y is the current y being
// gathered, and x is a value between the pairs of x points stored for that y.
// -----------------------------------------------------------------------------------------

/*void FeatureSpace::getPolygonData(ROIEntity* theEntity, ROI* theROI)
{	
	int totalPoints = 0;
	int fx1, fx2, fy1, fy2;
	int startx, starty, endx, endy;
	maxy = 0;
	maxx = 0;
	miny = std::numeric_limits<int>::max();
	minx = std::numeric_limits<int>::max();
	yoffset = 0;
	vectorsize = 0;
	
	int time_start, time_end;
	
	getPolygonVertices(theEntity);
	
	if (!polyPoints.empty())	//if we've got something to rasterise...
	{
		yoffset = miny;
		vectorsize = (maxy - miny) + 1;
		boundsCoords.resize(vectorsize, new list<int>);
		pixCoords.resize(vectorsize, new list<int>);
		
		//get the point coordinates for our boundary lines first
		for (int i = 0; i < polyPoints.size() - 1; i++)
		{
			generateBoundaryLine(polyPoints[i].x, polyPoints[i].y, polyPoints[i+1].x, polyPoints[i+1].y);
		}
		generateBoundaryLine(polyPoints[polyPoints.size()-1].x, polyPoints[polyPoints.size()-1].y, polyPoints[0].x, polyPoints[0].y);
		
		//sort our resultant lists
		int sortstart = GetTickCount();
		for (int i = 0; i < vectorsize; i++)
		{
			boundsCoords[i]->sort();
		}
		int sortend = GetTickCount();
		Console::write("FS::GPD Time to sort lists was %f seconds\n\n", (float)(sortend - sortstart) / 1000.0);
		
		//get the point coordinates for all points between those lines, if size not odd
		time_start = GetTickCount();
		
		//get bounding rect
		
		//get our x and y in terms of the LOD
		fx1 = minx / LODfactor;
		fy1 = miny / LODfactor;
		fx2 = maxx / LODfactor;
		fy2 = maxy / LODfactor;
		
		Console::write("FS::GPD - Poly LOD bounds are x1 = %d, y1 = %d, x2 = %d, y2 = %d\n", fx1, fy1, fx2, fy2);
		
		//work out the origin points of our start and end tiles
		startx = (fx1 / tileSize) * tileSize;
		starty = (fy1 / tileSize) * tileSize;
		endx = (fx2 / tileSize) * tileSize;
		endy = (fy2 / tileSize) * tileSize;
		
		Console::write("FS::GPD - StartX = %d, StartY = %d, EndX = %d, EndY = %d\n", startx, starty, endx, endy);
		
		for(int tiley = starty; tiley <= endy; tiley+=tileSize)
		{
			for(int tilex = startx; tilex <= endx; tilex+=tileSize)
			{
				Console::write("FS::GPD - In tile starting at x %d, y %d\n", tilex, tiley);
				
				char* grabbedData = fsTileset->get_tile_RGB_LOD(tilex, tiley, band1, band2, band3);
				
				int LODyoffset = yoffset / LODfactor;
				
				for (int i = 0; i < vectorsize; i++)
				{
					//iterators for our "point pairs"
					if (boundsCoords[i]->size() > 1)
					{
						list<int>::iterator j1 = boundsCoords[i]->begin();
						list<int>::iterator j2 = j1;
						//set our second point to the next point in the list
						while(*j1 == *j2)
						{
							j2++;
						}
						if(j2 == boundsCoords[i]->end()) j2--;
						
						//loop until we reach the end of the point list
						if ((*j1 == *j2) && (boundsCoords[i]->size() == 2))
						{
							pushXPixel(*j1, i + yoffset);
							totalPoints++;
						}
						else
						{
							while(true)
							{
		
								for(int k = *j1; k <= *j2; k++)
								{
									pushXPixel(k, i + yoffset);
									totalPoints++;
								}
								
								//move j2 on to the next point in the list
								j2++;
								if(j2 == boundsCoords[i]->end()) //if we're at the end of the list, break
								{
									break;
								}
								//otherwise, set j1 to be the next point as well, and
								//move j2 on to the next point in the pair
								j1 = j2;
								j2++;
								if(j2 == boundsCoords[i]->end()) //if we're at the end of the list, break
								{
									break;
								}
							}
						}
					}
					else //if (boundsCoords[i]->size() == 1)
					{
						pushXPixel(*boundsCoords[i]->begin(), i + yoffset);
						totalPoints++;
					}
				}
				
				delete[] grabbedData;
			}
		}
		time_end = GetTickCount();
		Console::write("FS::GPD Time to draw points between pairs was %f seconds\n", (float)(time_end - time_start) / 1000.0);
	}
	
	//write the resultant array
	Console::write("FS::GPD -- Total points in poly = %d\n", totalPoints);
	
	//push the values for this data to our lists of values -- TO DO
	
	//we're finished -- clear our data for the next run
	time_start = GetTickCount();
	for (int i = 0; i < vectorsize; i++)
	{
		delete boundsCoords[i];
		delete pixCoords[i];
	}
	time_end = GetTickCount();
	Console::write("FS::GPD Time to clear vectors was %f seconds\n", (float)(time_end - time_start) / 1000.0);
	
	boundsCoords.clear();
	pixCoords.clear();
	maxy = 0; 
	miny = std::numeric_limits<int>::max();
	yoffset = 0;
	vectorsize = 0;
}*/

void FeatureSpace::getPolygonData(ROIEntity* theEntity, ROI* theROI)
{	
	int totalPoints = 0;
	maxy = 0;
	miny = std::numeric_limits<int>::max();
	yoffset = 0;
	vectorsize = 0;
	
	int time_start, time_end;
	
	getPolygonVertices(theEntity);
	
	if (!polyPoints.empty())	//if we've got something to rasterise...
	{
		yoffset = miny;
		vectorsize = (maxy - miny) + 1;
		boundsCoords.resize(vectorsize, new list<int>);
		pixCoords.resize(vectorsize, new list<int>);
		
		//get the point coordinates for our boundary lines first
		for (int i = 0; i < polyPoints.size() - 1; i++)
		{
			generateBoundaryLine(polyPoints[i].x, polyPoints[i].y, polyPoints[i+1].x, polyPoints[i+1].y);
		}
		generateBoundaryLine(polyPoints[polyPoints.size()-1].x, polyPoints[polyPoints.size()-1].y, polyPoints[0].x, polyPoints[0].y);
		
		//sort our resultant lists
		int sortstart = GetTickCount();
		for (int i = 0; i < vectorsize; i++)
		{
			boundsCoords[i]->sort();
		}
		int sortend = GetTickCount();
		Console::write("FS::GPD Time to sort lists was %f seconds\n\n", (float)(sortend - sortstart) / 1000.0);
		
		//get the point coordinates for all points between those lines, if size not odd
		time_start = GetTickCount();
		for (int i = 0; i < vectorsize; i++)
		{
			//iterators for our "point pairs"
			if (boundsCoords[i]->size() > 1)
			{
				list<int>::iterator j1 = boundsCoords[i]->begin();
				list<int>::iterator j2 = j1;
				//set our second point to the next point in the list
				while(*j1 == *j2)
				{
					j2++;
				}
				if(j2 == boundsCoords[i]->end()) j2--;
				
				//loop until we reach the end of the point list
				if ((*j1 == *j2) && (boundsCoords[i]->size() == 2))
				{
					pushXPixel(*j1, i + yoffset);
					totalPoints++;
				}
				else
				{
					while(true)
					{

						for(int k = *j1; k <= *j2; k++)
						{
							pushXPixel(k, i + yoffset);
							totalPoints++;
						}
						
						//move j2 on to the next point in the list
						j2++;
						if(j2 == boundsCoords[i]->end()) //if we're at the end of the list, break
						{
							break;
						}
						//otherwise, set j1 to be the next point as well, and
						//move j2 on to the next point in the pair
						j1 = j2;
						j2++;
						if(j2 == boundsCoords[i]->end()) //if we're at the end of the list, break
						{
							break;
						}
					}
				}
			}
			else //if (boundsCoords[i]->size() == 1)
			{
				pushXPixel(*boundsCoords[i]->begin(), i + yoffset);
				totalPoints++;
			}
		}
		time_end = GetTickCount();
		Console::write("FS::GPD Time to draw points between pairs was %f seconds\n", (float)(time_end - time_start) / 1000.0);
	}
	
	//write the resultant array
	Console::write("FS::GPD -- Total points in poly = %d\n", totalPoints);
	
	//push the values for this data to our lists of values -- TO DO
	
	//we're finished -- clear our data for the next run
	time_start = GetTickCount();
	for (int i = 0; i < vectorsize; i++)
	{
		delete boundsCoords[i];
		delete pixCoords[i];
	}
	time_end = GetTickCount();
	Console::write("FS::GPD Time to clear vectors was %f seconds\n", (float)(time_end - time_start) / 1000.0);
	
	boundsCoords.clear();
	pixCoords.clear();
	maxy = 0; 
	miny = std::numeric_limits<int>::max();
	yoffset = 0;
	vectorsize = 0;
}

// -----------------------------------------------------------------------------------------
// getPolygonVertices
// -----------------------------------------------------------------------------------------
// convert the polygon vertices for the current ROI to vector<myPoint> format
// -----------------------------------------------------------------------------------------

void FeatureSpace::getPolygonVertices(ROIEntity* theEntity)
{
	vector<coords> theCoords = theEntity->get_points();
	coords currentCoords;
	
	//clear our vector of polygon points
	polyPoints.clear();
	
	//make a new point structure, get the data,
	//and push it to the back of the vector
	for(int i = 0; i < theCoords.size(); i++)
	{
		currentCoords = theCoords.at(i);
		myPoint* thePoint = new myPoint;
		thePoint->x = currentCoords.x;
		thePoint->y = currentCoords.y;
		polyPoints.push_back(*thePoint);
		//Console::write("FS:GPV Poly point at X %d, Y %d\n", thePoint->x, thePoint->y);
		
		//determine if this point is a max or min y/x value
		if (thePoint->y > maxy) maxy = thePoint->y;
		if (thePoint->y < miny) miny = thePoint->y;
		if (thePoint->x > maxx) maxx = thePoint->x;
		if (thePoint->x < minx) minx = thePoint->x;
	}
}

// -----------------------------------------------------------------------------------------
// isTurningPoint
// -----------------------------------------------------------------------------------------
// Determine whether a point is a turning point in the y-axis.
// Algorithm by Rowan James, execution by Dafydd Williams
// -----------------------------------------------------------------------------------------

void FeatureSpace::isTurningPoint(int first, int middle, int last)
{
	//Console::write("examining point X %d, Y %d\n", polyPoints[middle].x, polyPoints[middle].y);
	if((polyPoints[first].y > polyPoints[middle].y && polyPoints[last].y > polyPoints[middle].y) || //if both neighbour y greater than middle y
	   (polyPoints[first].y < polyPoints[middle].y && polyPoints[last].y < polyPoints[middle].y)) //or if both neighbour y less than middle y
	{
		//make a new list of int to copy known good points to
		list<int>* tempList = new list<int>;
		
		//Console::write("FS::TP\tpoint is turning point -- attempting to find pixel values matching\n");
		if (boundsCoords[polyPoints[middle].y - yoffset]->size() == 1)
		{
			//Console::write("FS::TP\t\tSingle point - copying to new list\n");
			tempList->push_back(*boundsCoords[polyPoints[middle].y - yoffset]->begin());
		}
		else
		{
			for(list<int>::iterator j = boundsCoords[polyPoints[middle].y - yoffset]->begin(); j != boundsCoords[polyPoints[middle].y - yoffset]->end(); j++)
			{
				if(*j != polyPoints[middle].x)
				{
					//Console::write("FS::TP\t\tCopying to new list where X = %d\n", *j);
					tempList->push_back(*j);
				}
			}
		}
		
		//write our new list to replace our old list
		delete boundsCoords[polyPoints[middle].y - yoffset];
		boundsCoords[polyPoints[middle].y - yoffset] = tempList;
	}
	/*else
	{
		Console::write("FS::TP\tpoint was not turning point\n");
	}*/
}

// -----------------------------------------------------------------------------------------
// generateBoundaryLine
// -----------------------------------------------------------------------------------------
// Generate a line from a to b using the DDA Algorithm.
// Stub code by Tony Gray @ UTas
// -----------------------------------------------------------------------------------------

void FeatureSpace::generateBoundaryLine (int x1, int y1, int x2, int y2)
{
	//Console::write("Line from X1 %d, Y1 %d to X2 %d, Y2 %d\n", x1, y1, x2, y2);
	
	float increment;
	int dx, dy, steps;
	
	//if our second point is less than our first on the x axis, swap the point pairs
	if (x2<x1)
	{
		int temp;
		
		temp = x1;
		x1 = x2;
		x2 = temp;
		temp = y1;
		y1 = y2;
		y2 = temp;
	}
	
	//get our differences between the x coord pair and the y coord pair
	dx = x2 - x1;
	dy = y2 - y1;
	
	//if our x points are identical and our first y is greater than our second y
	//(in other words, if we have a vertical line segment going straight down)
	//swap our y coords only.
	if (x1 == x2 && y1 > y2)
	{
		int temp;
		
		temp = y1;
		y1 = y2;
		y2 = temp;
		//dy = abs(dy);
	}
	
	//We're always stepping by Y in this modified algorithm, so set steps
	steps = abs(dy);
	increment = (float) dx / (float) steps;

	if (dy < 0)
	{
		int temp;
		temp = y1;
		y1 = y2;
		y2 = temp;
		temp = x1;
		x1 = x2;
		x2 = temp;
		increment = increment - (increment * 2);
	}
	//Console::write("We'll start from X %d, Y %d, and go to X %d, Y %d\n", x1, y1, x2, y2);
	float x = (float) x1;
	cout.precision(6);
	//Console::write("After conversion to float, our x starting point is %d\n", x); 
	//Console::write("Our increment is %f\n", increment); 
	if (y1 == y2)
	{
		pushXPixelBounds(x1, y1);
		//plotPixel((short)x1, (short)y1);
		//Console::write("drew a point at X %d, Y %d\n", x1, y1);
		pushXPixelBounds(x2, y1);
		//plotPixel((short)x2, (short)y1);
		//Console::write("drew a point at X %d, Y %d\n", x2, y1);
	}
	else if (x1 == x2)
	{
		if (y1 > y2)
		{
			int temp = y1;
			y1 = y2;
			y2 = temp;
		}
		
		//Console::write("this is a vertical line\n");
		//Console::write("i starts at %d and ends at %d\n", y1, y2); 
		for(int i = y1; i <= y2; i++)
		{
			pushXPixelBounds(x1, i);
			//Console::write("drew a point at X %d, Y %d\n", x1, i);
			//plotPixel((short)x1, (short)i);
		}
	}
	else
	{
		for (int y = y1; y <= y2; y++)
		{
			int rx = int(round(x));
			//draw our pixel, for reference
			//Console::write("drew a point at X %d, Y %d\n", rx, y);
			//plotPixel((short)rx, (short)y);
			
			pushXPixelBounds(rx, y);
			
			x += increment;
		}
	}
}


// -----------------------------------------------------------------------------------------
// pushXPixelBounds
// -----------------------------------------------------------------------------------------
// Push our given X coord into the list of X coords for Y -- tailored for the
// boundary points vector.
// -----------------------------------------------------------------------------------------

void FeatureSpace::pushXPixelBounds(int rx, int y)
{
	//list<int>::iterator i, j;
	
	//Console::write("y - yoffset = %d\n", y - yoffset);
	if (boundsCoords[y - yoffset]->empty())
	{
		//Console::write("\t\tX list for Y = %d is empty\n", y);
		list<int>* tempxlist = new list<int>;
		boundsCoords[y - yoffset] = tempxlist;
		//Console::write("\t\t\tPushed X = %d to front of list for Y = %d\n", rx, y);
	}
	
	boundsCoords[y - yoffset]->push_front(rx);
}


// -----------------------------------------------------------------------------------------
// pushXPixel
// -----------------------------------------------------------------------------------------
// Push our given X coord into the list of X coords for Y.
// -----------------------------------------------------------------------------------------

void FeatureSpace::pushXPixel(int rx, int y)
{
	//list<int>::iterator i, j;
	
	//Console::write("y - yoffset = %d\n", y - yoffset);
	if (pixCoords[y - yoffset]->empty())
	{
		//Console::write("\t\tX list for Y = %d is empty\n", y);
		list<int>* tempxlist = new list<int>;
		pixCoords[y - yoffset] = tempxlist;
		//Console::write("\t\t\tPushed X = %d to front of list for Y = %d\n", rx, y);
	}
	
	pixCoords[y - yoffset]->push_front(rx);
}


// -----------------------------------------------------------------------------------------
// catForHash
// -----------------------------------------------------------------------------------------
// Concatenates three unsigned chars together in preparation for
// passing as a hash key.
// -----------------------------------------------------------------------------------------

unsigned int FeatureSpace::catForHash(unsigned char b1, unsigned char b2, unsigned char b3)
{
	return (unsigned int)b1 + ((unsigned int)b2 << 8) + ((unsigned int)b3 << 16);
}


// -----------------------------------------------------------------------------------------
//addToImageFSTable
// -----------------------------------------------------------------------------------------
//Adds a point to our Image FS hash table.
// -----------------------------------------------------------------------------------------
void FeatureSpace::addToImageFSTable(unsigned char b1, unsigned char b2, unsigned char b3)
{
	unsigned int hash = catForHash(b1, b2, b3);
	fsImagePoints[hash]++;
}


// -----------------------------------------------------------------------------------------
//addToROIFSTable
// -----------------------------------------------------------------------------------------
//Adds a point to current ROI FS hash table
// -----------------------------------------------------------------------------------------
void FeatureSpace::addToROIFSTable(unsigned char b1, unsigned char b2, unsigned char b3)
{
	unsigned int hash = catForHash(b1, b2, b3);
	fsROIPoints[hash]++;
	fsImagePoints.erase(hash);
}


// create feature space window 
int FeatureSpace::Create() {
    RECT rect;

    // get position of overview window for alignment   
    GetWindowRect(overviewWindow.GetHandle(),&rect);

    // create feature space window
    const char *title=makeMessage("Feature Space ",numFeatureSpaces+1);
    if (!CreateWin(0, "Parbat3D Feature Window", title,
	     WS_OVERLAPPEDWINDOW|WS_SYSMENU|WS_CAPTION|WS_SIZEBOX|WS_MAXIMIZEBOX|WS_MINIMIZEBOX|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
	     rect.right, rect.top, FEATURE_WINDOW_WIDTH, FEATURE_WINDOW_HEIGHT, NULL, NULL))
        return false;
    delete(title);

      
    // create child windows
    glContainer=new GLContainer(GetHandle(),this,0,0,FEATURE_WINDOW_WIDTH,FEATURE_WINDOW_HEIGHT-TOOLBAR_HEIGHT);       

    // make this window snap to others
    stickyWindowManager.AddStickyWindow(this);

    // handle events for the window
    prevProc=SetWindowProcedure(&WindowProcedure);	
 
    
    return true;    
}
 
// resize GLContainer to fit the feature space window
void FeatureSpace::OnResize() {
    RECT rect;
    int x, y, width, height;
    
    GetClientRect(GetHandle(), &rect);
    x = rect.left;
    y = rect.top;
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;

    MoveWindow(glContainer->GetHandle(), x, y, width, height, true);

    if (fsgl!=NULL)
		fsgl->resize();
		
    glContainer->Repaint();
}


// handle left mouse button down event
void FeatureSpace::OnGLContainerLeftMouseDown(int x,int y)
{
	// record inital mouse position for rotating/zooming
	prev_mouse_x=x;
	prev_mouse_y=y;
	Console::write("FeatureSpace::OnMouseDown\n");
}

// handle mouse move event
void FeatureSpace::OnGLContainerMouseMove(int virtualKeys,int x,int y)
{
//	Console::write("FeatureSpace::OnMouseMove\n");
	int x_diff = x - prev_mouse_x;
	int y_diff = y - prev_mouse_y;
//	Console::write("x_diff=%d y_diff=%d \n",x_diff,y_diff);
	// checked if left mouse button is down
	if ((virtualKeys&MK_LBUTTON) && !(virtualKeys&MK_RBUTTON))
	{
		Rotate(x_diff * fsgl->rads_to_deg / 2.0, y_diff * fsgl->rads_to_deg / 2.0);
	}
/*	if ((virtualKeys&MK_RBUTTON) && !(virtualKeys&MK_LBUTTON))
	{
		float nx = (float)x_diff/100.0;
		float ny = (float)y_diff/100.0;
		fsgl->cam_translate(nx, ny);
	} */
	// check if left & right mouse button is down
	if ((virtualKeys&MK_RBUTTON) && !(virtualKeys&MK_LBUTTON))
	{
		ChangeCameraZoom(y_diff * 0.01);
	}
	
	// Operations are cumulative, so re-set distance
	prev_mouse_x=x;
	prev_mouse_y=y;	
}

/* rotate the feature space */
void FeatureSpace::Rotate(float yaw_amount, float pitch_amount)
{
		float cam_yaw = fsgl->cam_yaw;
		float cam_pitch = fsgl->cam_pitch;
		float rads_to_deg = fsgl->rads_to_deg;
		
		cam_yaw-=yaw_amount;
		cam_pitch+=pitch_amount;
		if (cam_pitch > (M_PI/2.0) - rads_to_deg) cam_pitch = M_PI/2.0 - rads_to_deg;
		if (cam_pitch < -(M_PI/2.0) + rads_to_deg) cam_pitch = -M_PI/2.0 + rads_to_deg;
		
		fsgl->cam_yaw = cam_yaw;
		fsgl->cam_pitch = cam_pitch;
		
		glContainer->Repaint();	
}

/* change the camera zoom level by a +/- amount */
void FeatureSpace::ChangeCameraZoom(float amount)
{
		float cam_dolly = fsgl->cam_dolly;
		
		cam_dolly+=amount;
		if (cam_dolly < 0.1) cam_dolly = 0.1;
		if (cam_dolly > 5.0) cam_dolly = 5.0;
		
		fsgl->cam_dolly = cam_dolly;
		
		glContainer->Repaint();
}



// handle key presses on the feature space window
void FeatureSpace::OnKeyPress(int virtualKey)
{
	// check whether control is currently down
	bool control_pressed=(bool)(GetKeyState(VK_CONTROL)&128);

	// check whether shift is currently down
	bool shift_pressed=(bool)(GetKeyState(VK_SHIFT)&128);	
	
	switch (virtualKey)
	{
		
		case VK_UP:
			if (control_pressed)
			{
				Rotate(0, 5 * fsgl->rads_to_deg / 2.0);
			}
			else if (shift_pressed)
			{
				ChangeCameraZoom(-0.1);			
			}
			//else
			//	PanY(+0.1);
			break;
			
		case VK_DOWN:
			if (control_pressed)
			{
				Rotate(0, -5 * fsgl->rads_to_deg / 2.0);
			}
			else if (shift_pressed)
			{
				ChangeCameraZoom(+0.1);
			}
			//else
			//	PanY(-0.1);
			break;
			
		case VK_LEFT:
			if (control_pressed)
			{
				Rotate(5 * fsgl->rads_to_deg / 2.0, 0);
			}
			// else			
			//	PanX(+0.1);
			break;
			
		case VK_RIGHT:
			if (control_pressed)
			{
				Rotate(-5 * fsgl->rads_to_deg / 2.0, 0);
			}			
			//else
			//	PanX(-0.1);			
			break;
			
		case VK_PRIOR:	// page-up key
			ChangeCameraZoom(-0.1);
			break;
			
		case VK_NEXT:	// page-down key
			ChangeCameraZoom(+0.1);
			break;

		case VK_SPACE:
			fsgl->toggle_smooth();
			break;
						
	}
}
 
 
/* handle events related to the feature space window */
LRESULT CALLBACK FeatureSpace::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

    FeatureSpace* win=(FeatureSpace*)Window::GetWindowObject(hwnd);
    
    switch (message)                  /* handle the messages */
    {			
        /* WM_DESTORY: system is destroying our window */
        case WM_KEYDOWN:
			win->OnKeyPress(wParam);
			break;
        case WM_SIZE:
            win->OnResize();
            win->fsgl->resize();
            break;
        case WM_DESTROY:
            stickyWindowManager.RemoveStickyWindow(win);
            break;
    }
    /* call the next procedure in the chain */       
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}
