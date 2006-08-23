#include "ImageGL.h"
#include <stdlib.h>
#include <cassert>
#include "console.h"
#include "config.h"

ImageGL::ImageGL(HWND window_hwnd, ImageFile* image_file_ptr, ImageViewport* image_viewport_param)
{
	ImageProperties* image_properties;

	image_file = image_file_ptr;
	image_properties = image_file->getImageProperties();
	
	viewport = image_viewport_param;
	viewport->get_display_bands(&band_red, &band_green, &band_blue);
	
	image_height = image_properties->getHeight();
	image_width = image_properties->getWidth();
	tileset = NULL;
	LOD = -1; // local special value for un-initialized
	cache_size = 128; // 128MB cache
	textures = new GLuint[4];
	
	/* Initialize OpenGL*/
	gl_image = new GLView(window_hwnd);

	/* Initialize OpenGL machine */
    gl_image->make_current();
    assert(glGetError() == GL_NO_ERROR);
	glGenTextures(4, textures);

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
			glTexCoord2i(0, 1);
			glVertex2i(0, texture_size);
			glTexCoord2i(1, 1);
			glVertex2i(texture_size, texture_size);
			glTexCoord2i(1, 0);
			glVertex2i(texture_size, 0);
		}
		glEnd();
	}
	glEndList();


	/* Test that we haven't already messed up... */
	assert(glGetError() == GL_NO_ERROR);

	/* Initial viewport size */
	resize_window();
	/* Start listening for events */
	viewport->register_listener(this);
}


ImageGL::~ImageGL()
{
	delete[] textures;
	delete gl_image;
	delete tileset;
}


/* Re-draw our window */
void ImageGL::notify_viewport(void)
{
	#if DEBUG_GL
	Console::write("(II) ImageGL::notify_viewport()\n");
	#endif

	gl_image->make_current();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(viewport->get_zoom_x(),viewport->get_zoom_x()+viewport->get_viewport_width(),
			viewport->get_zoom_y()+viewport->get_viewport_height(),viewport->get_zoom_y(),
			1.0,-1.0);
		
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	/* Statically top-left four tiles */
#if TRUE
	glMatrixMode(GL_MODELVIEW);
	for (short x = 0; x < 4; x++) {
		glLoadIdentity();
		assert(glIsTexture(textures[x]) == GL_TRUE);
		glBindTexture(GL_TEXTURE_2D, textures[x]);
		assert(glGetError() == GL_NO_ERROR);
		glTranslatef((GLfloat)((x%2)*texture_size), (GLfloat)((x/2)*texture_size), 0.0);
		glCallList(list_tile);
	}
#endif
	glDisable(GL_TEXTURE_2D);
	gl_image->GLswap();
}


void ImageGL::notify_bands()
{
	#if DEBUG_GL
	Console::write("(II) ImageGL::notify_bands()\n");
	#endif
	viewport->get_display_bands(&band_red, &band_green, &band_blue);
	make_texture();
	notify_viewport();
}

void ImageGL::make_texture(void)
{
	#if DEBUG_GL
	Console::write("(II) ImageGL::make_texture()\n");
	#endif
	/* Find necessary tileset LOD */
	float tmp_zoom = 1.0;
	int needed_LOD = 0;
/*	while (tmp_zoom >= viewport->get_zoom_level()) {
		tmp_zoom = tmp_zoom/2.0;
		needed_LOD++;
	} */
	
	/* Find needed grid of textures */
	
	/* Use current if available and correct */
	if (needed_LOD == LOD) {
		/* free any un-used textures */
		return;
	} else {
		#if DEBUG_GL
		Console::write("(II) ImageGL changing to LOD ");
		Console::write(needed_LOD);
		Console::write("\n");
		#endif
		/* Else (re-)create tileset and free all textures */
		if(tileset != NULL) delete tileset;
		LOD = needed_LOD;
		tileset = new ImageTileSet(LOD, image_file, texture_size, cache_size);
	}
	
	assert(tileset != NULL);
	char* tex_data;
	gl_image->make_current();
	int tile_x, tile_y;
	for (short x = 0; x < 4; x++) {
		if (x == 0) {
			tile_x = 0;
			tile_y = 0;
		}
		if (x == 1) {
			tile_x = texture_size;
			tile_y = 0;
		}
		if (x == 2) {
			tile_x = 0;
			tile_y = texture_size;
		}
		if (x == 3) {
			tile_x = texture_size;
			tile_y = texture_size;
		}
		tex_data = tileset->get_tile_RGB(tile_x,tile_y,1,2,3);
		glBindTexture(GL_TEXTURE_2D, textures[x]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_size, texture_size, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);
		delete[] tex_data;
		assert(glIsTexture(textures[x]) == GL_TRUE);
	}

	/* free un-used textures */
	/* for each needed texture */
		/* check for video mem space */
		/* load tile data */
		/* create texture */
		/* free tile data */
		/* store info */
		
		
	/* Have we messed up? */
	assert(glGetError() == GL_NO_ERROR);
}

void ImageGL::resize_window(void)
{
     gl_image->GLresize();
     viewport->set_window_size(gl_image->width(), gl_image->height());
}

unsigned int* ImageGL::get_pixel_values(int image_x, int image_y)
{
	if (tileset != NULL)
		return tileset->get_pixel_values(image_x, image_y);
		
	return new unsigned int[image_file->getImageProperties()->getNumBands()];
}
