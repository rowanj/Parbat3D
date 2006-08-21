#include "ImageGL.h"
#include <stdlib.h>

ImageGL::ImageGL(HWND window_hwnd, ImageFile* image_file_ptr, ImageViewport* image_viewport_param)
{
	ImageProperties* image_properties;

	image_file = image_file_ptr;
	image_properties = image_file->getImageProperties();
	
	viewport = image_viewport_param;
	viewport->register_listener(this);
	viewport->get_display_bands(&band_red, &band_green, &band_blue);
	
	image_height = image_properties->getHeight();
	image_width = image_properties->getWidth();
	viewport_x = 0;
	viewport_y = 0;
	viewport_width = 100;
	viewport_height = 100;
		
	/* Initialize OpenGL*/
	gl_image = new GLView(window_hwnd);

	/* Initialize OpenGL machine */
    gl_image->make_current();
    glShadeModel(GL_FLAT);
    glDisable(GL_DEPTH_TEST);
    
    /* Use larger texture if appropriate */
    GLint max_texture_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*) &max_texture_size);
	texture_size = (((max_texture_size)<(512))?(max_texture_size):(512));


	gl_image->GLresize();

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
#if FALSE
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

	/* Initial viewport size */
	resize_window();
}


ImageGL::~ImageGL()
{
	delete gl_image;
	delete tileset;
}


/* Re-draw our window */
void ImageGL::notify_viewport(void)
{
#if FALSE
	gl_image->make_current();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
 
    glLoadIdentity();
   	glOrtho(viewport_x, viewport_x + viewport_width,
	   		-(viewport_y + viewport_height),-viewport_y,
			0.1, 10.0);
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/* Set up view transform */
	gluLookAt(0.0,0.0,(1.0/tan(PI/6.0)),0.0,0.0,0.0,0.0,1.0,0.0);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	

	/* Draw all valid textures */
	glTranslatef((GLfloat)texture_size * (GLfloat)start_column,
				 -(GLfloat)texture_size * (GLfloat)start_row, 0.0);
	for (tile_y = 0; tile_y < viewport_rows; tile_y++) {
		glPushMatrix();
		for (tile_x = 0; tile_x < viewport_columns; tile_x++) {
			tile_id = tile_x + (tile_y * viewport_columns);
			if ((tile_id < tex_base_size) &&
					((tile_x + start_column) < image_columns) &&
					((tile_y + start_row) < image_rows)) {
				glBindTexture(GL_TEXTURE_2D, tex_base[tile_id]);
				glCallList(list_tile);
				glTranslatef((GLfloat)texture_size, 0.0, 0.0);
			}
		}
		glPopMatrix();
		glTranslatef(0.0, -(GLfloat)texture_size, 0.0);
	}

	/* Statically draw first four tiles */
	if (tex_base_size) {
	glBindTexture(GL_TEXTURE_2D, tex_base[0]);
	glCallList(list_tile);
	glTranslatef((GLfloat)texture_size, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, tex_base[1]);
	glCallList(list_tile);
	glTranslatef(-(GLfloat)texture_size, -(GLfloat)texture_size, 0.0);
	glBindTexture(GL_TEXTURE_2D, tex_base[2]);
	glCallList(list_tile);
	glTranslatef((GLfloat)texture_size, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, tex_base[3]);
	glCallList(list_tile);
	}
	glDisable(GL_TEXTURE_2D);
#endif
	
	gl_image->GLswap();
}


void ImageGL::notify_bands()
{
	viewport->get_display_bands(&band_red, &band_green, &band_blue);
	make_texture();
	notify_viewport();
}

void ImageGL::make_texture(void)
{
#if FALSE
//	tileset = new ImageTileSet(0, image_file, texture_size, 0);
//	LOD_height = tileset->get_LOD_height();
//	LOD_width = tileset->get_LOD_width();	unsigned int tmp_id;
	int tx, ty;
	char *tmp_tex;
	
	Console::write("(II) ImageHandler::Making image textures - LOD=");
	Console::write(LOD);
	Console::write("\n");
	
	/* !! remove duplicates of this */
	if (!image_tileset) {
		image_tileset = new ImageTileSet(LOD, image_file, texture_size, 96 * 1024 * 1024);
		image_columns = image_tileset->get_columns();
		image_rows = image_tileset->get_rows();
	}
	
	if (image_tileset->get_LOD() != LOD) {
		delete image_tileset;
		image_tileset = new ImageTileSet(LOD, image_file, texture_size, 96 * 1024 * 1024);
		image_columns = image_tileset->get_columns();
		image_rows = image_tileset->get_rows();
	}
		
	tile_size = image_tileset->get_tile_size();
	tex_count = viewport_columns * viewport_rows;
	gl_image->make_current();

	/* If textures were allocated, delete them */
	if (tex_base_size) {
		glDeleteTextures(tex_base_size, tex_base);
		delete[] tex_base;
	}

	tex_base = new GLuint[tex_count];
	glGenTextures(tex_count, tex_base);
	tex_base_size = tex_count;

	for (ty = 0; ty < viewport_rows; ty++){
		for (tx = 0; tx < viewport_columns; tx++) {
			tmp_id = (ty * viewport_columns) + tx;
			if (((tx+start_column) < image_columns) && ((ty+start_row) < image_rows)) {
				tmp_tex = image_tileset->get_tile_RGB(tile_size * (tx + start_column), tile_size * (ty+start_row), band_red, band_green, band_blue);
				glBindTexture(GL_TEXTURE_2D, tex_base[tmp_id]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_size, texture_size, 0, GL_RGB, GL_UNSIGNED_BYTE, tmp_tex);
				delete[] tmp_tex;
			}
		}
	}
#endif
}

void ImageGL::resize_window(void)
{
     gl_image->GLresize();
     viewport->set_window_size(gl_image->width(), gl_image->height());
}

unsigned int* ImageGL::get_pixel_values(int image_x, int image_y)
{
	unsigned int* blerg = new unsigned int[8];
	blerg[0] = 10;
	blerg[1] = 20;
	blerg[2] = 30;
	blerg[3] = 40;
	blerg[4] = 50;
	blerg[5] = 60;
	blerg[6] = 70;
	blerg[7] = 80;
	return blerg;
}
