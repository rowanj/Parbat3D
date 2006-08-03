#include "ImageOverview.h"
#include <stdlib.h>

ImageOverview::ImageOverview(HWND window_hwnd, ImageFile* image_file)
{
	ImageProperties* image_properties;
	image_properties = image_file->getImageProperties();
	image_height = image_properties->getHeight();
	image_width = image_properties->getWidth();
	viewport_x = 0;
	viewport_y = 0;
	viewport_width = 100;
	viewport_height = 100;
	
	/* Initialize OpenGL*/
	gl_overview = new ImageGLView(window_hwnd);

	/* Initialize OpenGL machine */
    gl_overview->make_current();
    glShadeModel(GL_FLAT);
    glDisable(GL_DEPTH_TEST);
    
    /* Use large textures if appropriate */
    GLint max_texture_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*) &max_texture_size);
	texture_size = (((max_texture_size)<(512))?(max_texture_size):(512));

	tileset = new ImageTileSet(-1, image_file, texture_size, 0);
	
	gl_overview->GLresize();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(60.0, (GLfloat) gl_overview->width()/(GLfloat) gl_overview->height(), 0.1, 2.0);
	
	set_bands(1, 2, 3);
}


ImageOverview::~ImageOverview()
{
	delete gl_overview;
	delete tileset;
}

void ImageOverview::redraw(void)
{
		// Overview window
	gl_overview->make_current();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	/* Set up texture for overview image */
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, (GLuint) tex_overview_id);

	/* Set up view transform */
	/* We look 'down' at the origin, from high enough above it that one unit fills
		both horizontal and vertical dimensions */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	#define PI 3.14159265
	gluLookAt(0.0,0.0,(0.5/tan(PI/6.0)),0.0,0.0,0.0,0.0,1.0,0.0);

	/* Set up aspect transform */
	glPushMatrix(); // Use matrix copy so we only squash the overview polygon
	/* squash square we draw to the correct aspect ratio */
/*	if (image_width >= image_height) {
		glScalef(1.0, (GLfloat)image_height/(GLfloat)image_width, 1.0);
	} else {
		glScalef((GLfloat)image_width/(GLfloat)image_height, 1.0, 1.0);
	} */

	/* draw textured quad for overview image */
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-0.5, 0.5, 0.0);
		glTexCoord2f(1.0, 0.0);
		glVertex3f( 0.5, 0.5, 0.0);
		glTexCoord2f(1.0, 1.0);
		glVertex3f( 0.5,-0.5, 0.0);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-0.5,-0.5, 0.0);
	glEnd();
	/* No more texturing in overview window */
	glDisable(GL_TEXTURE_2D);

	/* Draw window box in overview window */
	{
		GLfloat box_top, box_bottom, box_left, box_right;
		GLfloat box_vert_max, box_horiz_max;
		
		box_vert_max = image_height;
		box_horiz_max = image_width;

		/* Move drawing such that origin is top-left pixel of overview image */
		glTranslatef(-0.5, 0.5, 0.0);
		/* Scale to work in LOD pixels, x increases to right, y increases to bottom */
		glScalef(1.0/box_horiz_max, -1.0/box_vert_max, 1.0);

		/* We'll be using translucent lines */
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		/* !! These could be global, recalc triggered on resize/zoom... */
		box_top = viewport_y;
		box_bottom = viewport_y + viewport_height;
		box_left = viewport_x;
		box_right = viewport_x + viewport_width;
		if (box_top < 0.0) box_top = 0.0;
		if (box_bottom > box_vert_max) box_bottom = box_vert_max;
		if (box_left < 0.0) box_left = 0.0;
		if (box_right > box_horiz_max) box_right = box_horiz_max;
		
		glBegin(GL_LINE_LOOP);
		{
			glColor4f(1.0, 0.0, 0.0, 0.8);
			/* top left */
			glVertex3f(box_left, box_top, 0.0);
			/* top right */
			glVertex3f(box_right, box_top, 0.0);
			/* bottom right */
			glVertex3f(box_right, box_bottom, 0.0);
			/* bottom left */
			glVertex3f(box_left, box_bottom, 0.0);
		}
		glEnd();
	
		/* Draw cross-hairs, for locating box when small */
		glBegin(GL_LINES);
		{
			glColor4f(1.0, 0.0, 0.0, 0.2);
			/* Left line */
			glVertex3f(0.0, (GLfloat)(viewport_y + (viewport_height/2)), 0.0);
			glVertex3f((GLfloat)viewport_x, (GLfloat)(viewport_y + (viewport_height/2)), 0.0);
			/* Right line */
			glVertex3f(box_horiz_max, (GLfloat)(viewport_y + (viewport_height/2)), 0.0);
			glVertex3f((GLfloat)(viewport_x + viewport_width), (GLfloat)(viewport_y + (viewport_height/2)), 0.0);
			/* Top line */
			glVertex3f((GLfloat)(viewport_x + (viewport_width/2)), 0.0, 0.0);
			glVertex3f((GLfloat)(viewport_x + (viewport_width/2)), (GLfloat)viewport_y, 0.0);
			/* Bottom line */
			glVertex3f((GLfloat)(viewport_x + (viewport_width/2)), box_vert_max, 0.0);
			glVertex3f((GLfloat)(viewport_x + (viewport_width/2)), (GLfloat)(viewport_y + viewport_height), 0.0);
		}
		glEnd();
		glDisable(GL_BLEND);
	}
	
	glPopMatrix(); // Restore model transform
	
#if DEBUG_IMAGE_REDRAW
	/* draw rotating line to visualize redraw frequency */
	glRotatef(redraw_rotz, 0.0,0.0,1.0);
	redraw_rotz-=1.0;
	if(redraw_rotz < -360.0) redraw_rotz+=360.0;
	glBegin(GL_LINES);
		glColor3f(1.0,1,0);
		glVertex3f(0.0,0.0,0.0);
		glVertex3f(0.0,0.25,0.0);
	glEnd();
#endif
	gl_overview->GLswap();
}


void ImageOverview::set_bands(int band_R, int band_G, int band_B)
{
	band_red = band_R;
	band_green = band_G;
	band_blue = band_B;
	make_texture();
	redraw();
}

void ImageOverview::make_texture(void)
{
	#if DEBUG_IMAGE_HANDLER
	Console::write("(II) ImageHandler::make_overview_texture()\n");
	#endif
	// Get texture data	
	tex_overview = tileset->get_tile_RGB(0, 0, band_red, band_green, band_blue);
	#if DEBUG_IMAGE_HANDLER
	Console::write("(II) ImageHandler::Successfully got overview texture from tileset.\n");
	#endif

	/* Make texture from data */
	gl_overview->make_current();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if (!tex_overview_id) glGenTextures(1, &tex_overview_id);
	glBindTexture(GL_TEXTURE_2D, (GLuint) tex_overview_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_size, texture_size, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_overview);
	/* We don't need the RGB data here anymore */
	delete(tex_overview);
	#if DEBUG_IMAGE_HANDLER
	Console::write("(II) ImageHandler::Successfully created overview texture.\n");
	#endif
}

void ImageOverview::resize_viewport(int new_width, int new_height)
{
	viewport_width = new_width;
	viewport_height = new_height;
	redraw();
}
void ImageOverview::move_viewport(int new_x, int new_y)
{
	viewport_x = new_x;
	viewport_y = new_y;
	redraw();
}
