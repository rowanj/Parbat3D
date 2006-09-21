
#include <Windows.h>
#include "main.h"
#include "Window.h"
#include "FeatureSpace.h"
#include "GLContainer.h"
#include "Config.h"
#include "Console.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include <commctrl.h>

int FeatureSpace::numFeatureSpaces=0;

const int FeatureSpace::FEATURE_WINDOW_WIDTH=650;       // inital width of feature space window
const int FeatureSpace::FEATURE_WINDOW_HEIGHT=550;      // inital height of feature space window
const int FeatureSpace::TOOLBAR_HEIGHT=30;              // height of toolbar area at bottom of window


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
    Console::write("FeatureSpace -- Our LOD is %d\n", theLOD);
    Console::write("FeatureSpace -- Band 1 = %d, Band 2 = %d, Band 3 = %d\n", band1, band2, band3);
    
	int createSuccess;
    
    createSuccess=Create();
    assert(createSuccess);
    
    glview=new GLView(glContainer->GetHandle());
    numFeatureSpaces++;

    // todo: setup feature space's data    
    getPixelData();
	   
    Show();
}

// draw contents of GLContainer with opengl
void FeatureSpace::PaintGLContainer() {
    glview->make_current();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    bbox_size = 0.9;
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0, 0, -1);
    
    glPolygonMode(GL_FRONT, GL_LINE);
    glPolygonMode(GL_BACK, GL_LINE);
    glColor3f(1, 1, 1);
    
    glBegin(GL_QUADS);
        glVertex3f( bbox_size,  bbox_size,  0);
        glVertex3f(-bbox_size,  bbox_size,  0);
        glVertex3f(-bbox_size, -bbox_size,  0);
        glVertex3f( bbox_size, -bbox_size,  0);
    glEnd();
    
    glview->GLswap();
} 

//getPixelData gets all point coordinates from within ROIs, and
//then gets all data values for those pixel coords, and stores
//them, ready for display list generation.
void FeatureSpace::getPixelData()
{
	vector<ROI*> theROIs = regionsSet->get_regions();
	Console::write("FeatureSpace -- Count of regions is %d\n", regionsSet->get_regions_count());
	assert(regionsSet != NULL);
	Console::write("FeatureSpace -- Got vector of ROIs\n");
	
    ROI* currentROI;
	
	fsTileset = new ImageTileSet(theLOD, image_handler->get_image_file(), 512, 128);
	
	Console::write("FeatureSpace -- Got past fsTileset creation\n");
	assert(theROIs[0] != NULL);
	if (!theROIs.empty()) //if there are some ROIs in the set
	{
		Console::write("FeatureSpace -- Got past theROIs empty check\n");
		for (int cr = 0; cr < theROIs.size(); cr++)
		{
			Console::write("FeatureSpace -- Got into the ROIs loop\n");
			currentROI = theROIs.at(cr);
			vector<ROIEntity*> theEntities = currentROI->get_entities();
			Console::write("FeatureSpace -- Got vector of entities\n");
			if (!theEntities.empty())
			{
				Console::write("FeatureSpace -- Set our current ROI\n");
				ROIEntity* currentEntity;
				if (currentROI->get_active())
				{
					Console::write("FeatureSpace -- Current ROI is active -- checking types\n");
					for(int ce = 0; ce < theEntities.size(); ce++)
					{
						currentEntity = theEntities.at(ce);
						Console::write("FeatureSpace -- Set our current entity\n");
						
						char* theType = (char*)currentEntity->get_type();
						
						if(theType == ROI_POINT) //ROI == point
						{
							Console::write("FeatureSpace -- Current type is POINT\n");
							//add data at point to data lists
							getPointData(currentEntity, currentROI);
						}
						else if(theType == ROI_RECT) //ROI == rectangle
						{
							Console::write("FeatureSpace -- Current type is RECT\n");
							//add data at all points in rectangle to data lists
							getRectData(currentEntity, currentROI);
						}
						else //ROI == polygon
						{
							Console::write("FeatureSpace -- Current type is POLYGON\n");
							//add data at all points in polygon to data lists
							getPolygonData(currentEntity, currentROI);
						}
					}
				}
			}
			else
			{
				Console::write("FeatureSpace -- the entities set is empty\n");	
			}
		}
	}
	else
	{
		Console::write("FeatureSpace -- the ROIs set is empty\n");
	}
}

// -----------------------------------------------------------------------------------------
// getPointData
// -----------------------------------------------------------------------------------------
// Adds a point to the pix data lists
// -----------------------------------------------------------------------------------------
void FeatureSpace::getPointData(ROIEntity* theEntity, ROI* theROI)
{
	
}

// -----------------------------------------------------------------------------------------
// getRectData
// -----------------------------------------------------------------------------------------
// Gets data for all points inside a given rect
// -----------------------------------------------------------------------------------------

void FeatureSpace::getRectData(ROIEntity* theEntity, ROI* theROI)
{
	
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

void FeatureSpace::getPolygonData(ROIEntity* theEntity, ROI* theROI)
{	
	int totalPoints = 0;
	maxy = 0;
	miny = std::numeric_limits<int>::max();
	yoffset = 0;
	vectorsize = 0;
	
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
		
		int firstPoint = 0;
		int lastPoint = polyPoints.size() - 1;
		int secondLastPoint = polyPoints.size() - 2;
		
		//remove turning points
		//calculate first point
		isTurningPoint(lastPoint, firstPoint, firstPoint + 1);
		//calculate middle points
		for (int i = firstPoint; i < lastPoint - 1; i++)
		{
			isTurningPoint(i, i+1, i+2);
		}
		//calculate last point
		isTurningPoint(secondLastPoint, lastPoint, firstPoint);
		
		//get the point coordinates for all points between those lines, if size not odd
		for (int i = 0; i < vectorsize; i++)
		{
			//iterators for our "point pairs"
			if ((boundsCoords[i]->size()%2) == 0 && boundsCoords[i]->size() != 0)
			{
				//Console::write("Drawing lines at Y %d:\n", i + yoffset);
				list<int>::iterator j1 = boundsCoords[i]->begin();
				list<int>::iterator j2 = j1;
				//set our second point to the next point in the list
				j2++;
				
				//loop until we reach the end of the point list
				while(true)
				{
					//if our points are identical, just get the one copy of it
					//Console::write("\tFilling line from %d to %d\n", *j1, *j2);
					if (*j1 == *j2)
					{
						pushXPixel(*j1, i + yoffset);
						totalPoints++;
						//Console::write("Fill point at X %d, Y %d\n", *j1, i + yoffset);
					}
					else //get all points between them (inclusive)
					{
						for(int k = *j1; k <= *j2; k++)
						{
							pushXPixel(k, i + yoffset);
							totalPoints++;
							//plotPixel((short)k, (short)(i + yoffset));
							//Console::write("Fill point at X %d, Y %d\n", Y, i + yoffset);
						}
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
			else if (boundsCoords[i]->size() == 1)
			{
				//Console::write("Single point at %d -- drawing\n", i + yoffset);
				pushXPixel(*boundsCoords[i]->begin(), i + yoffset);
				totalPoints++;
				//plotPixel((short)*pixCoords[i]->begin(), (short)(i + yoffset));
			}
			else
			{
				Console::write("Odd number of points at %d\n", i + yoffset);
			}
		}
	}
	
	//write the resultant array
	Console::write("FeatureSpace -- Total points in poly = %d\n", totalPoints);
	
	//push the values for this data to our lists of values -- TO DO
	
	//we're finished -- clear our data for the next run
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
		Console::write("Poly point at X %d, Y %d\n", thePoint->x, thePoint->y);
		
		//determine if this point is a max or min y value
		if (thePoint->y > maxy) maxy = thePoint->y;
		if (thePoint->y < miny) miny = thePoint->y;
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
		//Console::write("\tpoint is turning point -- attempting to find pixel values matching\n");
		for(list<int>::iterator j = boundsCoords[polyPoints[middle].y - yoffset]->begin(); j != boundsCoords[polyPoints[middle].y - yoffset]->end(); j++)
		{
			if(*j == polyPoints[middle].x && boundsCoords[polyPoints[middle].y - yoffset]->size() > 1)
			{
				boundsCoords[polyPoints[middle].y - yoffset]->erase(j);
				Console::write("\t\tfound point record to erase where X = %d\n", *j);
				Console::write("\t\tPoint at Y %d:\n", middle + yoffset);
				for(list<int>::iterator iter = boundsCoords[polyPoints[middle].y - yoffset]->begin(); iter != boundsCoords[polyPoints[middle].y - yoffset]->end(); iter++)
				{
					Console::write("\t\t\tX %d\n", *iter);
				}
				break;
			}
			else
			{
				Console::write("Single point - ignoring\n");
			}
		}
	}
	else
	{
		Console::write("\tpoint was not turning point\n");
	}
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
// pushXPixel
// -----------------------------------------------------------------------------------------
// Push our given X coord into the list of X coords for Y.
// -----------------------------------------------------------------------------------------

void FeatureSpace::pushXPixelBounds(int rx, int y)
{
	list<int>::iterator i, j;
	
	//Console::write("y - yoffset = %d\n", y - yoffset);
	if (boundsCoords[y - yoffset]->empty())
	{
		//Console::write("\t\tX list for Y = %d is empty\n", y);
		list<int>* tempxlist = new list<int>;
		boundsCoords[y - yoffset] = tempxlist;
		boundsCoords[y - yoffset]->push_front(rx);
		//Console::write("\t\t\tPushed X = %d to front of list for Y = %d\n", rx, y);
	}
	else
	{
		//Console::write("\t\tX list for Y = %d is not empty\n", y);
		//Console::write("\t\t\tPerforming sorted insert.\n");
		i = boundsCoords[y - yoffset]->begin();
		while (i != boundsCoords[y - yoffset]->end())
		{
			if (*i < rx)
			{
				j = i;
				j++;
				if (j == boundsCoords[y - yoffset]->end())
				{
					//Console::write("\t\t\t\tX %d is largest yet for %d -- pushing %d to back\n", rx, y, rx);
					boundsCoords[y - yoffset]->push_back(rx);
					break;
				}
				else
				{
					i++;
				}
			}
			else if (*i > rx)
			{
				//Console::write("\t\t\t\t%d  is greater than or equal to X = %d -- inserting %d\n", *i, rx, rx);
				boundsCoords[y - yoffset]->insert(i, rx);
				break;
			}
			else if (*i == rx)
			{
				Console::write("\t**Y = %d has duplicate X value for X = %d\n", y, rx);
				Console::write("\t**List of X at Y is:\n");
				for(list<int>::iterator iter = boundsCoords[y - yoffset]->begin(); iter != boundsCoords[y - yoffset]->end(); iter++)
				{
					Console::write("\t\tX %d\n", *iter);
				}
				break;
			}
		}
	}
}


void FeatureSpace::pushXPixel(int rx, int y)
{
	list<int>::iterator i, j;
	
	//Console::write("y - yoffset = %d\n", y - yoffset);
	if (pixCoords[y - yoffset]->empty())
	{
		//Console::write("\t\tX list for Y = %d is empty\n", y);
		list<int>* tempxlist = new list<int>;
		pixCoords[y - yoffset] = tempxlist;
		pixCoords[y - yoffset]->push_front(rx);
		//Console::write("\t\t\tPushed X = %d to front of list for Y = %d\n", rx, y);
	}
	else
	{
		//Console::write("\t\tX list for Y = %d is not empty\n", y);
		//Console::write("\t\t\tPerforming sorted insert.\n");
		i = pixCoords[y - yoffset]->begin();
		while (i != pixCoords[y - yoffset]->end())
		{
			if (*i < rx)
			{
				j = i;
				j++;
				if (j == pixCoords[y - yoffset]->end())
				{
					//Console::write("\t\t\t\tX %d is largest yet for %d -- pushing %d to back\n", rx, y, rx);
					pixCoords[y - yoffset]->push_back(rx);
					break;
				}
				else
				{
					i++;
				}
			}
			else if (*i > rx)
			{
				//Console::write("\t\t\t\t%d  is greater than or equal to X = %d -- inserting %d\n", *i, rx, rx);
				pixCoords[y - yoffset]->insert(i, rx);
				break;
			}
			else if (*i == rx)
			{
				Console::write("\t**Y = %d has duplicate X value for X = %d\n", y, rx);
				Console::write("\t**List of X at Y is:\n");
				for(list<int>::iterator iter = pixCoords[y - yoffset]->begin(); iter != pixCoords[y - yoffset]->end(); iter++)
				{
					Console::write("\t\tX %d\n", *iter);
				}
				break;
			}
		}
	}
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

    // create toolbar
    RECT rc;
    GetClientRect(GetHandle(),&rc);
    hToolbar=CreateWindowEx(0, TOOLBARCLASSNAME, (LPSTR) NULL, 
        WS_CHILD | CCS_ADJUSTABLE | WS_VISIBLE | CCS_NOPARENTALIGN | CCS_NORESIZE , 0, rc.bottom-TOOLBAR_HEIGHT,rc.right,TOOLBAR_HEIGHT, GetHandle(), 
        (HMENU) 1, GetAppInstance(), NULL);    
    
    SendMessage(hToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0);         

    TBBUTTON tbb[7];       
    const int NUM_BUTTONS=7;
    int i;
    
    char *buttonText[7]={"RL","RR","RU","RD",NULL,"ZI","ZO"};
    for (i=0;i<NUM_BUTTONS;i++)
    {
        if (buttonText[i]!=NULL)
        {
           tbb[i].iString= SendMessage(hToolbar, TB_ADDSTRING, 0, (LPARAM) (LPSTR) "A");     //buttonText[i]
           tbb[i].iBitmap = 0; 
           tbb[i].idCommand = i; 
           tbb[i].fsState = TBSTATE_ENABLED; 
           tbb[i].fsStyle = TBSTYLE_TOOLTIPS; //BTNS_BUTTON 
           tbb[i].dwData = 0; 
        }
        else
        {
            tbb[i].iString= 0;
            tbb[i].iBitmap = 0; 
            tbb[i].idCommand = -1; 
            tbb[i].fsState = TBSTATE_ENABLED; 
            tbb[i].fsStyle = BTNS_SEP;
            tbb[i].dwData = 0; 
        }
    }

    SendMessage(hToolbar, TB_ADDBUTTONS, (WPARAM) NUM_BUTTONS, (LPARAM) (LPTBBUTTON) &tbb);    
    SendMessage(hToolbar, TB_SETBUTTONSIZE, (WPARAM) 100, (LPARAM)30);        
    
//   SendMessage(hToolbar, TB_AUTOSIZE, 0, 0); 

   ShowWindow(hToolbar, SW_SHOW);    
   
   /*
   REBARINFO     rbi;
   REBARBANDINFO rbBand;
   RECT          rc;
   HWND   hwndCB, hwndTB;
   DWORD  dwBtnSize;

   hRebarControl = CreateWindowEx(WS_EX_CLIENTEDGE, //WS_EX_TOOLWINDOW
                           REBARCLASSNAME, //REBARCLASSNAME
                           "rebar",
                           WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|
                           WS_CLIPCHILDREN|RBS_VARHEIGHT|
                           CCS_NODIVIDER,
                           0,0,0,0,
                           GetHandle(),
                           NULL,
                           GetAppInstance(),
                           NULL);

   assert(hRebarControl!=NULL);

   // Initialize and send the REBARINFO structure.
   rbi.cbSize = sizeof(REBARINFO);  // Required when using this
                                    // structure.
   rbi.fMask  = 0;
   rbi.himl   = (HIMAGELIST)NULL;
   SendMessage(hRebarControl, RB_SETBARINFO, 0, (LPARAM)&rbi);

   // Initialize structure members that both bands will share.
   rbBand.cbSize = sizeof(REBARBANDINFO);  // Required
   rbBand.fMask  = RBBIM_COLORS | RBBIM_TEXT |  //| RBBIM_BACKGROUND
                   RBBIM_STYLE | RBBIM_CHILD  | RBBIM_CHILDSIZE | 
                   RBBIM_SIZE;
   rbBand.fStyle = RBBS_CHILDEDGE | RBBS_FIXEDBMP;
   rbBand.hbmBack=NULL;

   // Create the combo box control to be added.
   hwndCB = CreateWindowEx( 0, szStaticControl, "test", 
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 0,
		0, 100, 20, hRebarControl, NULL,
		GetAppInstance(), NULL);
   // Set values unique to the band with the combo box.
   GetWindowRect(hwndCB, &rc);
   rbBand.lpText     = "Combo Box";
   rbBand.hwndChild  = hwndCB;
   rbBand.cxMinChild = 0;
   rbBand.cyMinChild = rc.bottom - rc.top;
   rbBand.cx         = 200;
   
   // Add the band that has the combo box.
   SendMessage(hRebarControl, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);
*/   


    // make this window snap to others
    stickyWindowManager.AddStickyWindow(this);

    // handle events for the window
    prevProc=SetWindowProcedure(&WindowProcedure);	
    
    
    OnResize();    
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
    height = rect.bottom - rect.top - TOOLBAR_HEIGHT;
    
    MoveWindow(glContainer->GetHandle(), x, y, width, height, true);
    MoveWindow(hToolbar, x, rect.bottom-TOOLBAR_HEIGHT, rect.right, TOOLBAR_HEIGHT, true);    
    
    /*
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    */
}
 
/* handle events related to the feature space window */
LRESULT CALLBACK FeatureSpace::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

    FeatureSpace* win=(FeatureSpace*)Window::GetWindowObject(hwnd);
    
    
    switch (message)                  /* handle the messages */
    {			
        /* WM_DESTORY: system is destroying our window */
        case WM_SIZE:
            win->OnResize();
            break;
        case WM_DESTROY:
            stickyWindowManager.RemoveStickyWindow(win);
            break;
    }
    /* call the next procedure in the chain */       
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}
