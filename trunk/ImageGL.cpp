#include "ImageGL.h"
#include "Settings.h"
#include <stdlib.h>
#include <cassert>
#include "console.h"
#include "config.h"

ImageGL::ImageGL(HWND window_hwnd, ImageFile* image_file_ptr, ImageViewport* image_viewport_param, ROISet *roisToOutline)
{
	ImageProperties* image_properties;

	/* Check for bad arguments */
	assert (window_hwnd != NULL);
	assert (image_file_ptr != NULL);
	assert (image_viewport_param != NULL);

	/* Copy parameters to locals */
	roiset=roisToOutline;
	viewport = image_viewport_param;
	image_file = image_file_ptr;
	/* Grab image properties from image_file */
	image_properties = image_file->getImageProperties();
	image_height = image_properties->getHeight();
	image_width = image_properties->getWidth();	
	/* Load cache size from preferences */
	cache_size = settingsFile->getSettingi("preferences","cachesize",128);
	
	/* Create our OpenGL context */
	gl_image = NULL;
	gl_image = new GLView(window_hwnd);
	assert(gl_image != NULL);

	/* Initialize local variables
		Most of the texture and tile stuff is initialized in flush_textures() */
	LOD = -1; // Invalid value to force creation on first run
	tileset = NULL; // No reference to a tileset untill LOD is decided
	tile_count = 0;
	flush_textures();

	
	/* Initialize OpenGL machine */
    gl_image->make_current();

    glShadeModel(GL_FLAT);
    glDisable(GL_DEPTH_TEST);

    /* Use larger texture if appropriate */
    {
	    GLint max_texture_size;
		int user_texture_size = settingsFile->getSettingi("preferences","texsize",512);
		{
			int order = -1;
			/* Prevent use of absurdly low values */
			user_texture_size = max(user_texture_size, 32);
			
			/* Round to next lowest power of two if not already */
			while (user_texture_size) {
				user_texture_size = user_texture_size>>1;
				order++;
			}
			user_texture_size = int(round(pow(2,order)));
			#if DEBUG_GL
			Console::write("(II) User texture size (rounded) = ");
			Console::write(user_texture_size);
			Console::write("\n");
			#endif
		}
	    glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*) &max_texture_size);
		texture_size = min(user_texture_size, max_texture_size);
	}

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
			glVertex2i(0, 1);
			glTexCoord2i(1, 1);
			glVertex2i(1, 1);
			glTexCoord2i(1, 0);
			glVertex2i(1, 0);
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
	flush_textures();
	tile_textures.clear();
	delete gl_image;
	delete tileset;
}


/* Re-draw our window */
void ImageGL::notify_viewport(void)
{
	#if DEBUG_GL
	Console::write("(II) ImageGL::notify_viewport()\n");
	#endif

	check_textures();

	gl_image->make_current();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(viewport->get_image_x(),viewport->get_image_x()+viewport->get_viewport_width(),
			viewport->get_image_y()+viewport->get_viewport_height(),viewport->get_image_y(),
			1.0,-1.0);
		
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	/* Statically top-left four tiles */
#if TRUE
	glMatrixMode(GL_MODELVIEW);
	GLuint tex_id;
	for (short y = 0; y < 2; y++) {
	for (short x = 0; x < 2; x++) {
		glLoadIdentity();
		tex_id = get_tile_texture(x,y);
		assert(glIsTexture(tex_id) == GL_TRUE);
		glBindTexture(GL_TEXTURE_2D, tex_id);
		assert(glGetError() == GL_NO_ERROR);
		glTranslatef((GLfloat)(x * tile_image_size), (GLfloat)(y * tile_image_size), 0.0);
		glPushMatrix();
		glScalef(tile_image_size, tile_image_size, tile_image_size);
		glCallList(list_tile);
		glPopMatrix();
	}
	}
#endif
	glDisable(GL_TEXTURE_2D);
	assert(glGetError() == GL_NO_ERROR);

	draw_rois();
	
	gl_image->GLswap();
}

void ImageGL::draw_rois(void)
{
	gl_image->make_current();
	/* Draw ROI outlines over the top of the image */
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix(); /* Don't destroy tile transform */
	/* Scale to work in image pixels */
	glLoadIdentity();

	/* We'll be using translucent lines */
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    vector<ROI*> rois=roiset->get_regions();
    int i,j,k,red,green,blue;
    for (i=0;i<rois.size();i++)
    {
        ROI* roi=rois.at(i);            
        if (roi->get_active())
        {
            // set ROI colour
            roi->get_colour(&red,&green,&blue);
            glColor4f(1.0*red/255, 1.0*green/255, 1.0*blue/255, 1.0);
                
            // draw ROI entities
            vector<ROIEntity*> entities=roi->get_entities();
            for (j=0;j<entities.size();j++)
            {
                ROIEntity *entity=entities.at(j);
                const char *type=entity->get_type();
                vector<coords> points=entity->get_points();                
            if (type==ROI_POLY)
            {
                // draw outline of polygon
           		glBegin(GL_LINE_LOOP);
           		{
                    for (k=0;k<points.size();k++)
                    {
                        coords point=points.at(k);
               			glVertex3f(point.x, point.y, 0.0);
                    }
                }
                    glEnd();
            }
            else if (type==ROI_POINT)
            {
                // draw point
           		glBegin(GL_POINTS);
          		{
                    if (points.size()>=1)
                    {
               			glVertex3f(points.at(0).x, points.at(0).y, 0.0);
                    }
                }
                glEnd();                    
            }
            else if (type==ROI_RECT)
            {
                // draw outline of rectangle
           		glBegin(GL_LINE_LOOP);
           		{
                    if (points.size()>=2)
                    {
                        coords topleft=points.at(0);
                        coords bottomright=points.at(1);
                            
               			glVertex3f(topleft.x, topleft.y, 0.0);
               			glVertex3f(topleft.x, bottomright.y, 0.0);
               			glVertex3f(bottomright.x, bottomright.y, 0.0);
               			glVertex3f(bottomright.x, topleft.y, 0.0);                   			
                    }
                }
                glEnd();                    
            }
           }
       }
    }

	//glDisable(GL_BLEND);
		
	glPopMatrix(); /* Restore tile transform */
	assert(glGetError() == GL_NO_ERROR);
}

void ImageGL::notify_bands(void)
{
	#if DEBUG_GL
	Console::write("(II) ImageGL::notify_bands()\n");
	#endif
	int r, g, b;
	viewport->get_display_bands(&band_red, &band_green, &band_blue);
	
	tile_textures.clear();
	tile_textures.assign(tile_count, 0);
	free_textures = textures;
}

void ImageGL::check_textures(void)
{
	check_tileset();
			
	/* Find new display rect */
	int new_row, new_col, new_num_rows, new_num_cols;
	new_col = viewport->get_image_x() / tile_image_size;
	new_row = viewport->get_image_y() / tile_image_size;

	new_num_cols = (viewport->get_viewport_width() / tile_image_size) + 1;
	new_num_rows = (viewport->get_viewport_height() / tile_image_size) + 1;

	/* free un-used texture IDs */
	gl_image->make_current();
/*	// Left of new
	for (int x = viewport_start_col; x < new_col; x++) {
		for (int y = viewport_start_row; y < (viewport_start_row + viewport_rows); y++) {
			int tile_texture = get_tile_texture(x,y);
			if (glIsTexture(tile_texture) == GL_TRUE) {
				free_textures.push_back(tile_tex);
				set_tile_texture(x,y,0);
			}
		}
		viewport_start_col++;
		viewport_cols--;
	}
	// Right of new
	for (int x = start_col + num_cols; x >= (new_col + new_num_rows); x--) {
		for (int y = start_row; y < (start_row + num_rows); y++) {
			int tile_index = get_tile_id(x, y);
			int tile_tex = tile_texture[tile_index];
			if (glIsTexture(tile_tex) == GL_TRUE) {
				free_textures.push_back(tile_tex);
			}
			textures[tile_tex] = 0;
			tile_texture[tile_index] = -1;
		}
		num_cols--;
	}
	// Above new
	for (int y = start_row; y < new_row; y++) {
		for (int x = start_col; x < (start_col + num_cols); x++) {
			int tile_index = get_tile_id(x, y);
			int tile_tex = tile_texture[tile_index];
			if (glIsTexture(tile_tex) == GL_TRUE) {
				free_textures.push_back(tile_tex);
			}
			textures[tile_tex] = 0;
			tile_texture[tile_index] = -1;
		}
		start_row++;
		num_rows--;
	}
	// Below new
	for (int y = start_row + num_rows; y >= (new_row + new_num_rows); y--) {
		for (int x = start_col; x < (start_col + num_cols); x++) {
			int tile_index = get_tile_id(x, y);
			int tile_tex = tile_texture[tile_index];
			if (glIsTexture(tile_tex) == GL_TRUE) {
				free_textures.push_back(tile_tex);
			}
			textures[tile_tex] = 0;
			tile_texture[tile_index] = -1;
		}
		num_rows--;
	} */
	
	assert(tileset != NULL);
	
	/* Find which new tiles are needed */
	
	/* for each needed texture */
	load_tile_tex(0,0);
	load_tile_tex(1,0);
	load_tile_tex(0,1);
	load_tile_tex(1,1);

	/* Have we messed up? */
	assert(glGetError() == GL_NO_ERROR);
}

void ImageGL::load_tile_tex(int x_index, int y_index) {
	char* tex_data;
	int tile_x = x_index * tile_image_size;
	int tile_y = y_index * tile_image_size;
	GLuint tex_id;
	
	// !! Temporary stop-leak
	if (get_tile_texture(x_index, y_index) != 0) return;
	
	int num_free = free_textures.size();
	assert(free_textures.size() > 0);
	// Load the tile data
	tex_data = tileset->get_tile_RGB(tile_x,tile_y,band_red,band_green,band_blue);
	
	// Grab a free texture ID
	tex_id = free_textures.back();
	free_textures.pop_back();
	
	// Load the data into the texture
	gl_image->make_current();
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_size, texture_size, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);
	delete[] tex_data;
	assert(glGetError() == GL_NO_ERROR);
	assert(glIsTexture(tex_id) == GL_TRUE);

	// Update the tile involved to show the new texture ID
	set_tile_texture(x_index, y_index, tex_id);
}

void ImageGL::check_tileset(void)
{
	/* Find needed LOD */
	float tmp_zoom = 0.5;
	int needed_LOD = 0;
	while (tmp_zoom >= viewport->get_zoom_level()) {
		tmp_zoom = tmp_zoom/2.0;
		needed_LOD++;
	}

	/* Compare with current.
		If not the same, we need to do a few things... */
	if (needed_LOD != LOD) {
		#if DEBUG_GL
		Console::write("Changing LOD to ");
		Console::write(needed_LOD);
		Console::write("\n");
		#endif
		// Free old tileset and load new
		if (tileset != NULL) delete tileset;
		LOD = needed_LOD;
		tileset = new ImageTileSet(LOD, image_file, texture_size, cache_size);

		// [re-]initialize the texture ID array
//		if (tile_textures != NULL) delete[] tile_textures;
		tile_rows = tileset->get_rows();
		tile_cols = tileset->get_columns();
		tile_count = tile_rows * tile_cols;
//		tile_textures = new GLuint[tile_count];
//		for (int x = 0; x < tile_count; x++) tile_textures[x] = 0;
		tile_textures.clear();
		tile_textures.assign(tile_count, 0);
			
		/* All textures can be re-used */
		free_textures = textures;
		
		/* find size of tile in image space */
		tile_image_size = texture_size;
		for (int x = 0; x < LOD; x++) {
			tile_image_size = tile_image_size * 2;
		}
	}
	#if DEBUG_GL
	else {
		Console::write("Leaving LOD at ");
		Console::write(LOD);
		Console::write("\n");
	}
	#endif
}

void ImageGL::flush_textures(void)
{
	/* De-allocate all our textures */
	gl_image->make_current();
	while (!textures.empty()) {
		GLuint my_tex;
		my_tex = textures.back();
		textures.pop_back();
		glDeleteTextures(1, &my_tex);
	}
	/* Clean out free texture vector */
	free_textures.clear();
}

void ImageGL::resize_window(void)
{
	#if DEBUG_GL
	Console::write("(II) Resize window triggered.\n");
	#endif
	int new_tex_rows, new_tex_cols;
	int new_tex_count;
	
	/* Re-size the OpenGL context */
	gl_image->GLresize();

	/* Number how many textures at near-next LOD cover the screen? */
	new_tex_cols = (gl_image->width() / (texture_size / 2)) + 1;
	new_tex_rows = (gl_image->height() / (texture_size / 2)) + 1;
	new_tex_count = new_tex_rows * new_tex_cols;
	
	/* Do we need more texture IDs?  Allocate them */
	gl_image->make_current();
	while (textures.size() < new_tex_count) {
		/* Get another texture ID and set it up */
		GLuint new_tex;
		GLint proxy_width; // used for checking available video memory
		glGenTextures(1, &new_tex);
		glBindTexture(GL_TEXTURE_2D, new_tex);
		/* Set up wrapping and filtering parameters for this texture */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		/* Load proxy texture to check for enough space */
		glTexImage2D(GL_PROXY_TEXTURE_2D, 0, GL_RGB, texture_size, texture_size, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &proxy_width);
		/* And abort if not */
		assert(proxy_width == texture_size);
		
		/* Now, we know the new texture is valid, and we can start using it */
		textures.push_back(new_tex);
		free_textures.push_back(new_tex);
	}

	// We use this as an invalid test case later, so we don't want it to work...
	assert(glIsTexture(0) == GL_FALSE);

	viewport->set_window_size(gl_image->width(), gl_image->height());
}

unsigned int* ImageGL::get_pixel_values(int image_x, int image_y)
{
	if (tileset != NULL)
		return tileset->get_pixel_values(image_x, image_y);
		
	/* If we don't currently have a tileset, the values will be wrong anyway,
		so we should just avoid delete[]'ing memory we haven't new[]'ed.
		This might occur for a few instants between the window being shown and
		the tileset being loaded... or something */
	return new unsigned int[image_file->getImageProperties()->getNumBands()];
}

// Return the texture id of a tile at column/row index:
GLuint ImageGL::get_tile_texture(int x_index, int y_index) {
	int tile_index = y_index*tile_cols + x_index;
	assert(tile_textures.size() > tile_index);
	return tile_textures.at(tile_index);
}
// Set the texture id of a tile at column/row index:
void ImageGL::set_tile_texture(int x_index, int y_index, GLuint new_id) {
	int tile_index = y_index*tile_cols + x_index;
	assert(tile_textures.size() > tile_index);
	tile_textures[tile_index] = new_id;
}
