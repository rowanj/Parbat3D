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
/*	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); */
	glPointSize(3);
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
	glViewport(0,0,gl_view->width(),gl_view->height());
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
    // Draw the points
    Console::write("Drawing %d lists\n", points_lists.size());
	for (int pos = 0; pos < points_lists.size(); pos++) {
		glCallList(points_lists.at(pos));
	}
	
#if FALSE // DRAW EXTRA VIEWPORTS
	assert(glGetError() == GL_NO_ERROR);
	
	const int viewport_size = 130;
	const float viewport_bound = 0.55;
	// Top viewport
	glViewport(0,0,viewport_size,viewport_size);
	gl_text->draw_string(5, 0, "Top");
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-viewport_bound,viewport_bound,-viewport_bound,viewport_bound,1.0,-1.0);
	glCallList(list_line_square);
//	glCallList(list_box);
    for (int list = list_points_base; list < list_points_base + num_points_lists; list++) {
		glCallList(list);
	}
	
	assert(glGetError() == GL_NO_ERROR);

	// Right viewport
	glViewport(gl_view->width()-viewport_size,0,viewport_size,viewport_size);
	gl_text->draw_string(5, 20, "Right");
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-viewport_bound,viewport_bound,-viewport_bound,viewport_bound,1.0,-1.0);
	glCallList(list_line_square);
	glRotatef(-90.0,0.0,1.0,0.0);
	glRotatef(-90.0,1.0,0.0,0.0);
//	glCallList(list_box);
    for (int list = list_points_base; list < list_points_base + num_points_lists; list++) {
		glCallList(list);
	}

	assert(glGetError() == GL_NO_ERROR);

	// Front viewport
	glViewport(gl_view->width()-viewport_size,gl_view->height()-viewport_size,viewport_size,viewport_size);
	gl_text->draw_string(5, 40, "Front");
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-viewport_bound,viewport_bound,-viewport_bound,viewport_bound,1.0,-1.0);
	glRotatef(-90.0,1.0,0.0,0.0);
//	glCallList(list_box);
    for (int list = list_points_base; list < list_points_base + num_points_lists; list++) {
		glCallList(list);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-0.5,-0.5,-0.5);
	glRotatef(90.0,1.0,0.0,0.0);
	glCallList(list_line_square);
#endif

	assert(glGetError() == GL_NO_ERROR);
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
	if (band1) gl_text->draw_string("X: Band %d", band1);

	glColor3f(0.0, 1.0, 0.0);
	glRasterPos3f(-0.05, 1.05, -0.05);
	if (band2) gl_text->draw_string("Y: Band %d", band2);

	glColor3f(0.0, 0.0, 1.0);
	glRasterPos3f(-0.05, -0.05, 1.05);
	if (band3) gl_text->draw_string("Z: Band %d", band3);
	
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
	
	list_line_square = glGenLists(1);
	glNewList(list_line_square, GL_COMPILE);
	glColor4f(1.0, 1.0, 1.0, edge_opacity);
	glBegin(GL_LINE_LOOP);
	glVertex2i(0,0);
	glVertex2i(1,0);
	glVertex2i(1,1);
	glVertex2i(0,1);
	glEnd();
	glEndList();
	assert(glGetError() == GL_NO_ERROR);
}

void FeatureSpaceGL::add_points(points_hash_t points_hash, unsigned char red, unsigned char green, unsigned char blue)
{
	Console::write("FeatureSpaceGL::add_points(size = %d, r = %d, g = %d, b = %d\n",
	        points_hash.size(), red, green, blue);
	const unsigned int points_per_list = 5000;
	// How many new display lists do we need?
	unsigned int new_lists_count = (points_hash.size()/points_per_list) + 1;
	unsigned int average_count = 0;
	points_hash_t::iterator hashi;

	// Variables for the point coordinates
	unsigned int point; // The value of it
	unsigned char* pointx = (unsigned char*)&point; // pointers to the useful bytes within it
	unsigned char* pointy = (unsigned char*)&point + 1;
	unsigned char* pointz = (unsigned char*)&point + 2;
	unsigned int count;
	GLfloat r = (float)red/255.0;
	GLfloat g = (float)green/255.0;
	GLfloat b = (float)blue/255.0;
	
	assert(points_hash.size() > 0);
	
	// Find average point density
	unsigned int total_count = 0;
	hashi = points_hash.begin();
	while (hashi != points_hash.end()) {
		total_count = total_count + hashi->second;
		hashi++;
	}
	vertices += total_count;
	average_count = total_count / points_hash.size();

	Console::write("total = %d, average_count = %d\n", total_count, average_count);
	
	// Make them
	gl_view->make_current();
	GLuint new_list_base = glGenLists(new_lists_count);

	// For each list
	hashi = points_hash.begin();
	for(GLuint list = new_list_base; list < new_list_base + new_lists_count; list++) {
		unsigned int this_list_count = 0;

		glNewList(list, GL_COMPILE);

		glBegin(GL_POINTS);
		// While space in the list and points left to add
		while ((this_list_count <= points_per_list) && (hashi != points_hash.end())) {
			point = hashi->first;
			count = hashi->second;
			if (count != 0) {
				glColor4f(r, g, b, (((float)count/(float)average_count) * 0.45) + 0.1);
				glVertex3f(*pointx/255.0, *pointy/255.0, *pointz/255.0);
				num_points = num_points + count;
				this_list_count++;
			}
			hashi++;
		}
		glEnd();
		glEndList();
		
		points_lists.push_back(list);
	}
}


#if FALSE
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
	glPointSize(3);
	glEnable(GL_POINT_SMOOTH);
	glColor4f(1.0, 1.0, 1.0, 0.5);
	glBegin(GL_POINTS);
	for(hashi = points_hash.begin(); hashi != points_hash.end(); hashi++) {
		point = hashi->first;
		count = hashi->second;
		glColor4f(1.0, 1.0, 1.0, 0.9 * (float)count/(float)maxvalue + 0.1);
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
#endif

void FeatureSpaceGL::cam_translate(float x, float y)
{
	gl_view->make_current();
	glPushAttrib(GL_MATRIX_MODE);
	glMatrixMode(GL_PROJECTION);
	glTranslatef(x, y, 0.0);
	glPopAttrib();
}

void FeatureSpaceGL::resize(void)
{
	if (gl_view!=NULL)
		gl_view->resize();
}
