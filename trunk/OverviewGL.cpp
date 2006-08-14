#include "OverviewGL.h"
#include <stdlib.h>

OverviewGL::OverviewGL(HWND window_hwnd, ImageFile* image_file)
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
	gl_overview = new GLView(window_hwnd);

	/* Initialize OpenGL machine */
    gl_overview->make_current();
    glShadeModel(GL_FLAT);
    glDisable(GL_DEPTH_TEST);
    
    /* Use larger texture if appropriate */
    GLint max_texture_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*) &max_texture_size);
	texture_size = (((max_texture_size)<(512))?(max_texture_size):(512));

	tileset = new ImageTileSet(-1, image_file, texture_size, 0);
	LOD_height = tileset->get_LOD_height();
	LOD_width = tileset->get_LOD_width();
	
	gl_overview->GLresize();

	/* compile display list for textured tile */
	list_tile = glGenLists(1);
    glNewList(list_tile,GL_COMPILE);
    {
		glBegin(GL_QUADS);
		{
			glTexCoord2i(0, 0);
			glVertex2i(0, 0);
			glTexCoord2i(1, 0);
			glVertex2i(1, 0);
			glTexCoord2i(1, 1);
			glVertex2i(1, 1);
			glTexCoord2i(0, 1);
			glVertex2i(0, 1);
		}
		glEnd();
	}
	glEndList();


	/* Set up view transform */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	/* glOrtho(Left, Right, Bottom, Top, Near-clip, Far-clip) */
#if TRUE
	/* Orthagonal projection, clamped to 1 unit, top-left origin,
		all visible co-ordinates positive */
	glOrtho(0.0, 1.0, 1.0, 0.0, 1.0, -1.0);
#else
	/* Show one more unit on all sides for debugging */
	glOrtho(-1.0, 2.0, 2.0, -1.0, 1.0, -1.0);
#endif

	/* Set up initial model transform */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	/* Scale such that image fills overview window in largest dimension */
	scalefactor_tile = (GLfloat)texture_size / (GLfloat) max(LOD_width,LOD_height);
	glScalef(scalefactor_tile, scalefactor_tile, scalefactor_tile);
	
	/* Find scale factor for lines */
	scalefactor_lines = 1.0 / (GLfloat) max(image_width, image_height);
	set_bands(1, 2, 3);
}


OverviewGL::~OverviewGL()
{
	delete gl_overview;
	delete tileset;
}


/* Re-draw our overview window */
void OverviewGL::redraw(void)
{
	gl_overview->make_current();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	/* Set up texture for overview image */
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, (GLuint) tex_overview_id);

	/* draw textured quad for overview image */
	glCallList(list_tile);

	/* No more texturing in overview window */
	glDisable(GL_TEXTURE_2D);


	/* Draw window box in overview window */
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix(); /* Don't destroy tile transform */
		/* Scale to work in image pixels */
		glScalef(scalefactor_lines, scalefactor_lines, scalefactor_lines);

		GLfloat box_top = viewport_y;
		GLfloat box_bottom = viewport_y + viewport_height;
		GLfloat box_left = viewport_x;
		GLfloat box_right = viewport_x + viewport_width;
		
		if (box_top < 0.0) box_top = 0.0;
		if (box_bottom > image_height) box_bottom = image_height;
		if (box_left < 0.0) box_left = 0.0;
		if (box_right > box_horiz_max) box_right = image_width;
		
		/* We'll be using translucent lines */
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
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
			GLfloat halfway_point_x = (GLfloat)(viewport_x + viewport_width/2);
			GLfloat halfway_point_y = (GLfloat)(viewport_y + viewport_height/2);
			glColor4f(1.0, 0.0, 0.0, 0.2);
			/* Left line */
			glVertex3f(0.0, halfway_point_y, 0.0);
			glVertex3f((GLfloat)viewport_x, halfway_point_y, 0.0);
			/* Right line */
			glVertex3f((GLfloat)image_width, halfway_point_y, 0.0);
			glVertex3f((GLfloat)(viewport_x + viewport_width), halfway_point_y, 0.0);
			/* Top line */
			glVertex3f(halfway_point_x, 0.0, 0.0);
			glVertex3f(halfway_point_x, (GLfloat)viewport_y, 0.0);
			/* Bottom line */
			glVertex3f(halfway_point_x, box_vert_max, 0.0);
			glVertex3f(halfway_point_x, (GLfloat)(viewport_y + viewport_height), 0.0);
		}
		glEnd();
		
		glDisable(GL_BLEND);
		
		glPopMatrix(); /* Restore tile transform */
	}
	
	gl_overview->GLswap();
}

void OverviewGL::update_viewport(int x, int y, int width, int height)
{
	viewport_x = x;
	viewport_y = y;
	viewport_width = width;
	viewport_height = height;
	redraw();
}

void OverviewGL::set_bands(int band_R, int band_G, int band_B)
{
	band_red = band_R;
	band_green = band_G;
	band_blue = band_B;
	make_texture();
	redraw();
}

void OverviewGL::make_texture(void)
{
	// Get texture data	
	tex_overview = tileset->get_tile_RGB(0, 0, band_red, band_green, band_blue);

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
	/* remember to free the RGB memory */
	delete(tex_overview);
}

