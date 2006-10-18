#include "FeatureSpaceGL.h"
#include <cassert>
#include "console.h"
#include <mmsystem.h>

#if USE_POINT_SPRITES
#include "point_texture.c"
#define GL_POINT_SPRITE_ARB               0x8861
#define GL_COORD_REPLACE_ARB              0x8862
PFNGLPOINTPARAMETERFARBPROC  glPointParameterfARB  = NULL;
PFNGLPOINTPARAMETERFVARBPROC glPointParameterfvARB = NULL;
#endif

#define FS_USE_MIPMAPS 1
#define FS_USE_EXTRA_VIEWPORTS 0

FeatureSpaceGL::FeatureSpaceGL(HWND hwnd, int LOD_arg, int band1_arg, int band2_arg, int band3_arg)
{
	// Set initial camera position
/*	cam_yaw = 270.0 / degs_to_rad;
	cam_pitch = 0.0;
	cam_dolly = -2.5; /* */
	
	cam_xrot=0;
	cam_yrot=0;
	cam_xpan=0;
	cam_ypan=0;
	cam_zoom=0;
	
	band1 = band1_arg;
	band2 = band2_arg;
	band3 = band3_arg;
	
	smooth = false;
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
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
#if USE_POINT_SPRITES
	setup_point_sprites();
	glPointSize(point_sprite_max_size);
	glEnable( GL_BLEND );
//	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glBlendFunc( GL_SRC_ALPHA, GL_DST_ALPHA );
//	glBlendFunc( GL_SRC_ALPHA, GL_ZERO );
#else
	glPointSize(3);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
				
	make_box_list();
	
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // Make world 0,0,0 the centre of the box
    glTranslatef(-0.5, -0.5, -0.5);
    

}

#if USE_POINT_SPRITES
void FeatureSpaceGL::setup_point_sprites(void)
{
	Console::write("(II) Setting up point sprites...\n");

	gl_view->make_current();
	glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &points_texture_id );

	glBindTexture( GL_TEXTURE_2D, points_texture_id );
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	glTexImage2D( GL_TEXTURE_2D, 0, points_texture.bytes_per_pixel, points_texture.width, points_texture.height, 0,
			     GL_RGB, GL_UNSIGNED_BYTE, points_texture.pixel_data);

	char *ext = (char*)glGetString( GL_EXTENSIONS );

    if( strstr( ext, "GL_ARB_point_parameters" ) == NULL )
    {
        MessageBox(NULL,"GL_ARB_point_parameters extension was not found",
            "ERROR",MB_OK|MB_ICONEXCLAMATION);
    }
    else
    {
        glPointParameterfARB  = (PFNGLPOINTPARAMETERFARBPROC)wglGetProcAddress("glPointParameterfARB");
        glPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC)wglGetProcAddress("glPointParameterfvARB");
    }
    
    // Enable point sprite mipmapping
    #if FS_USE_MIPMAPS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D, points_texture.bytes_per_pixel, points_texture.width, points_texture.height, GL_RGB, GL_UNSIGNED_BYTE, points_texture.pixel_data);
    #endif
    

   	float quadratic[] =  {0.1f, 0.0f, 50.0f };
    glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );

    glPointParameterfARB( GL_POINT_FADE_THRESHOLD_SIZE_ARB, 2.0f );

    glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 1.0f );
    glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, point_sprite_max_size);

    glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );

    assert (glPointParameterfARB != NULL);
    assert (glPointParameterfvARB != NULL);

	assert(glGetError() == GL_NO_ERROR);
}
#endif // USE_POINT_SPRITES

void FeatureSpaceGL::draw()
{
    gl_view->make_current();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	// setup projection
/*	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0,0,gl_view->width(),gl_view->height());
    gluPerspective(45.0f,gl_view->aspect(),0.1f,6.0f);
    gluLookAt(cam_dolly * (cos(cam_yaw)*cos(cam_pitch)), cam_dolly * (sin(cam_yaw)*cos(cam_pitch)), cam_dolly * sin(cam_pitch),
				0.0, 0.0, 0.0,
				0.0, 0.0, 1.0); /* */
    
    gl_text->draw_string(5, 20, "Granularity: %d:1", granularity);
    gl_text->draw_string(5, 40, "Image Points: %d", num_points);
    gl_text->draw_string(5, 60, "Unique Points: %d", vertices);
    
    // draw the bounding box
    glDisable(GL_TEXTURE_2D);
    glCallList(list_box);
    // Draw the points
//    Console::write("Drawing %d lists\n", points_lists.size());
	if (smooth) {
		glEnable(GL_POINT_SMOOTH);
	} else {
		glDisable(GL_POINT_SMOOTH);
	}
	
#if USE_POINT_SPRITES
	glEnable(GL_TEXTURE_2D);
    glEnable( GL_POINT_SPRITE_ARB );
    glBindTexture(GL_TEXTURE_2D, points_texture_id);

#endif
	for (int pos = 0; pos < points_lists.size(); pos++) {
		glCallList(points_lists.at(pos));
	}
#if USE_POINT_SPRITES
    glDisable( GL_POINT_SPRITE_ARB );
#endif
	
#if FS_USE_EXTRA_VIEWPORTS // DRAW EXTRA VIEWPORTS
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
	vertices += points_hash.size();
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


void FeatureSpaceGL::translate_cam(float x, float y)
{
	cam_xpan+=x;
	cam_ypan+=y;
	
	Console::write("translate_cam(%f,%f);\n", x, y);
	gl_view->make_current();
	glPushAttrib(GL_MATRIX_MODE);
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(0.5,0.5,0.5);
	glRotatef(-cam_xrot, 0.0, 0.0, 1.0);
	glTranslatef(x, 0, 0);
	glRotatef(cam_xrot, 0.0, 0.0, 1.0);
	glTranslatef(-0.5,-0.5,-0.5);
	
	glMatrixMode(GL_PROJECTION);
	glRotatef(-cam_yrot, 1.0, 0.0, 0.0);	
	glTranslatef(0.0, 0.0, -y);
	glRotatef(cam_yrot, 1.0, 0.0, 0.0);	
	glPopAttrib();
	draw();
}

void FeatureSpaceGL::zoom_cam(float diff)
{
	cam_zoom+=diff;
	
	gl_view->make_current();
	glPushAttrib(GL_MATRIX_MODE);
	glMatrixMode(GL_MODELVIEW);

	glMatrixMode(GL_PROJECTION);
	glRotatef(-cam_yrot, 1.0, 0.0, 0.0);	
	glTranslatef(0,diff,0);
	glRotatef(cam_yrot, 1.0, 0.0, 0.0);		
	glPopAttrib();
	draw();
}	

void FeatureSpaceGL::rot_cam(int x_diff, int y_diff)
{
		/* float cam_yaw = fsgl->cam_yaw;
		float cam_pitch = fsgl->cam_pitch;
		float rads_to_deg = fsgl->rads_to_deg;
		
		cam_yaw-=x_diff * rads_to_deg / 2.0;
		cam_pitch+=y_diff * rads_to_deg / 2.0;
		if (cam_pitch > (M_PI/2.0) - rads_to_deg) cam_pitch = M_PI/2.0 - rads_to_deg;
		if (cam_pitch < -(M_PI/2.0) + rads_to_deg) cam_pitch = -M_PI/2.0 + rads_to_deg;
		
		fsgl->cam_yaw = cam_yaw;
		fsgl->cam_pitch = cam_pitch; */
	cam_xrot+=x_diff;
	cam_yrot+=y_diff;
	
	gl_view->make_current();
	glPushAttrib(GL_MATRIX_MODE);
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(0.5,0.5,0.5);
	glRotatef(x_diff, 0.0, 0.0, 1.0);
	glTranslatef(-0.5,-0.5,-0.5);
	glMatrixMode(GL_PROJECTION);
	glRotatef(y_diff, 1.0, 0.0, 0.0);
	glPopAttrib();
	draw();
}		


void FeatureSpaceGL::resize(void)
{
	if (gl_view!=NULL)
		gl_view->resize();
	glViewport(0,0,gl_view->width(),gl_view->height());
	update_viewport();
}

void FeatureSpaceGL::update_viewport(void)
{
	gl_view->make_current();
	glPushAttrib(GL_MATRIX_MODE);
   	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// !! restore prev. settings
    gluPerspective(50.0f,gl_view->aspect(),0.01f,6.0f);
//    glTranslatef(0.0, -2.0, 0.0);
/*    gluLookAt(cam_dolly * (cos(cam_yaw)*cos(cam_pitch)), cam_dolly * (sin(cam_yaw)*cos(cam_pitch)), cam_dolly * sin(cam_pitch),
				128.0, 128.0, 128.0,
				0.0, 0.0, 1.0); /* */
/*	gluLookAt(0.0, cam_dolly, 0.0,
			0.0, 0.0, 0.0,
			0.0, 0.0, 1.0); /* */
	
	gluLookAt(0.0, -3.0, 0.0,
			 0.0, 0.0, 0.0,
			 0.0, 0.0, 1.0);
	glPopAttrib();
}

void FeatureSpaceGL::toggle_smooth(void)
{
	Console::write("Toggling FS Smoothing\n");
	smooth = !smooth;
	draw();
}
