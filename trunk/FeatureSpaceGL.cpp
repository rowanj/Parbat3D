#include "FeatureSpaceGL.h"
#include <cassert>
#include "console.h"

FeatureSpaceGL::FeatureSpaceGL(HWND hwnd, int LOD_arg, int band1_arg, int band2_arg, int band3_arg)
{
	// Set initial camera position
	cam_yaw = 270.0 / degs_to_rad;
	cam_pitch = 0.0;
	cam_dolly = 2.5;
	
	band1 = band1_arg;
	band2 = band2_arg;
	band3 = band3_arg;
	
	num_points = 0;
	vertices = 0;
	LOD = LOD_arg;
	granularity = 1;
	while (LOD_arg) {
		granularity = granularity * 4;
		LOD_arg--;
	}
		
    gl_view = new GLView(hwnd);
	gl_text = new GLText(gl_view, "Arial", 12);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	gl_view->make_current();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	
	make_box_list();
}

void FeatureSpaceGL::draw()
{
    gl_view->make_current();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	// setup projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(45.0f,gl_view->aspect(),0.1f,6.0f);
    gluLookAt(cam_dolly * (cos(cam_yaw)*cos(cam_pitch)), cam_dolly * (sin(cam_yaw)*cos(cam_pitch)), cam_dolly * sin(cam_pitch),
				0.0, 0.0, 0.0,
				0.0, 0.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
       
    // Make old 0,0,0 the centre of the box
    glTranslatef(-0.5, -0.5, -0.5);
    
    gl_text->draw_string(5, 20, "Granularity: %d:1", granularity);
    gl_text->draw_string(5, 40, "Image Points: %d", num_points);
    gl_text->draw_string(5, 60, "Unique Points: %d", vertices);
    
    // draw the bounding box
    glCallList(list_box);
//    glTranslatef(0.5, 0.5, 0.5);
    for (int list = list_points_base; list < list_points_base + num_points_lists; list++) {
		glCallList(list);
	}
        
    gl_view->swap();
}

void FeatureSpaceGL::make_box_list()
{
	gl_view->make_current();
	const float edge_opacity = 0.5;
	const float origin_opacity = 0.8;

	list_box = glGenLists(1);

	// Get ready to make bounding box list
	glNewList(list_box, GL_COMPILE);
	glColor3f(1.0, 0.0, 0.0);
	glRasterPos3f(1.05, -0.05, -0.05);
	gl_text->draw_string("Band %d", band1);

	glColor3f(0.0, 1.0, 0.0);
	glRasterPos3f(-0.05, 1.05, -0.05);
	gl_text->draw_string("Band %d", band2);

	glColor3f(0.0, 0.0, 1.0);
	glRasterPos3f(-0.05, -0.05, 1.05);
	gl_text->draw_string("Band %d", band3);
	
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos3f(-0.05, -0.05, -0.05);
	gl_text->draw_string("0");

	glBegin(GL_LINES);
	// Lines from origin
	// x
	glColor4f(1.0, 0.0, 0.0, origin_opacity);
	glVertex3f(0.0, 0.0, 0.0);
	glColor4f(1.0, 1.0, 1.0, edge_opacity);
	glVertex3f(1.0, 0.0, 0.0);
	// y
	glColor4f(0.0, 1.0, 0.0, origin_opacity);
	glVertex3f(0.0, 0.0, 0.0);
	glColor4f(1.0, 1.0, 1.0, edge_opacity);
	glVertex3f(0.0, 1.0, 0.0);
	// z
	glColor4f(0.0, 0.0, 1.0, origin_opacity);
	glVertex3f(0.0, 0.0, 0.0);
	glColor4f(1.0, 1.0, 1.0, edge_opacity);
	glVertex3f(0.0, 0.0, 1.0);

	// All other lines in translucent white
	glColor4f(1.0, 1.0, 1.0, edge_opacity);
	// Lines from x1
	// y
	glVertex3f(1.0, 0.0, 0.0);
	glVertex3f(1.0, 1.0, 0.0);
	// z
	glVertex3f(1.0, 0.0, 0.0);
	glVertex3f(1.0, 0.0, 1.0);
	
	// Lines from y1
	// x
	glVertex3f(0.0, 1.0, 0.0);
	glVertex3f(1.0, 1.0, 0.0);
	// z
	glVertex3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 1.0, 1.0);

	// Lines from z1
	// x
	glVertex3f(0.0, 0.0, 1.0);
	glVertex3f(1.0, 0.0, 1.0);
	// y
	glVertex3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 1.0, 1.0);
	
	// Lines to 1,1,1
	// x0
	glVertex3f(0.0, 1.0, 1.0);
	glVertex3f(1.0, 1.0, 1.0);
	// y0
	glVertex3f(1.0, 0.0, 1.0);
	glVertex3f(1.0, 1.0, 1.0);
	// z0
	glVertex3f(1.0, 1.0, 0.0);
	glVertex3f(1.0, 1.0, 1.0);

	glEnd();
	glEndList(); // Done with the bounding box
	

}

void FeatureSpaceGL::make_points_lists(points_hash_t points_hash, int maxvalue)
{
	gl_view->make_current();
	// Use points hash
#if TRUE
	points_hash_t::iterator hashi;
	unsigned int point;
	unsigned char* pointx;
	unsigned char* pointy;
	unsigned char* pointz;
	pointx = (unsigned char*)&point + 0;
	pointy = (unsigned char*)&point + 1;
	pointz = (unsigned char*)&point + 2;
	unsigned int count;
	
	num_points_lists = 1;
	list_points_base = glGenLists(num_points_lists);
	glNewList(list_points_base, GL_COMPILE);
	glPushAttrib(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
	glPointSize(2);
	glEnable(GL_POINT_SMOOTH);
	glColor4f(1.0, 1.0, 1.0, 0.5);
	glBegin(GL_POINTS);
	for(hashi = points_hash.begin(); hashi != points_hash.end(); hashi++) {
		point = hashi->first;
		count = hashi->second;
		glColor4f(1.0, 1.0, 1.0, ((float)count/(float)maxvalue)*0.7 + 0.3);
		glVertex3f(*pointx/256.0, *pointy/256.0, *pointz/256.0);
		//value = hashi->second;
		vertices++;
	}
	glEnd();
	glPopAttrib();
	glEndList();
#endif

	// Make list_points to benchmark display performance
#if FALSE
	const int divisor = RAND_MAX / 256;
	const int points_per_list = 50000;
//	int count = 256 * 256 * 256 / 16;
//	int count = 256 * 256 * 10;
//	int count = 10;
//	int count = 50000;
	int count = 100000;
	num_points_lists = count / points_per_list + 1;
	list_points_base = glGenLists(num_points_lists);
	char r, g, b;
	srand(0); // seed the random number generator
	
	for (int this_list = num_points_lists - 1; this_list >= 0; this_list--) {
	glNewList(list_points_base + this_list, GL_COMPILE);
	glPushAttrib(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
	glPointSize(3);
	glEnable(GL_POINT_SMOOTH);
	glColor4f(1.0, 1.0, 1.0, 0.5);
	glBegin(GL_POINTS);
	while (count >= points_per_list * this_list) {
		count--;
		/* generate random coordinates */
		r = rand() / divisor;
		g = rand() / divisor;
		b = rand() / divisor;
		glVertex3f(r/256.0, g/256.0, b/256.0);
	}
	glEnd();
	glPopAttrib();
	glEndList();
	}
#endif

}

void FeatureSpaceGL::resize(void)
{
	gl_view->resize();
}
