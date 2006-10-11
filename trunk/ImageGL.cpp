#include "ImageGL.h"
#include "Settings.h"
#include <stdlib.h>
#include <cassert>
#include "console.h"
#include "config.h"

#define DEBUG_GL_TEXTURES 0
#define DEBUG_GL 1

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
	
	gl_text = NULL;
	gl_text = new GLText(gl_image, "Ariel", 14);
	assert(gl_text != NULL);

	/* Initialize local variables
		Most of the texture and tile stuff is initialized in flush_textures() */
	LOD = -1; // Invalid value to force creation on first run
	tileset = NULL; // No reference to a tileset untill LOD is decided
	tile_count = 0;
	viewport_x = 0;
	viewport_y = 0;
	viewport_width = 0;
	viewport_height = 0;
	viewport_start_row = 0;
	viewport_start_col = 0;
	viewport_end_row = 0;
	viewport_end_col = 0;
	flush_textures();

	
	/* Initialize OpenGL machine */
    gl_image->make_current();

	// Write OpenGL extensions to console
	//Console::write((char*) glGetString(GL_EXTENSIONS));

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glShadeModel(GL_FLAT);
    glDisable(GL_DEPTH_TEST);
    

    /* Select texture size */
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
			Console::write("(II) User texture size (rounded) = %d\n", user_texture_size);
		}
	    glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*) &max_texture_size);
		texture_size = min(user_texture_size, max_texture_size);
	}

	gl_image->resize();

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

	/* Set inital mouse coords */
	mouse_x=0;
	mouse_y=0;

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
	delete gl_text;
	delete gl_image;
	delete tileset;
}


/* Re-draw our window */
void ImageGL::notify_viewport(void)
{
	#if DEBUG_GL
	Console::write("(II) ImageGL::notify_viewport()\n");
	#endif
	
	/* Update viewport locals */
	viewport_x = viewport->get_image_x(); 
	viewport_y = viewport->get_image_y();
	viewport_width = viewport->get_viewport_width();
	viewport_height = viewport->get_viewport_height();
	
//	#if DEBUG_GL_TEXTURES
	gl_image->make_current();
	glDrawBuffer(GL_FRONT);
	gl_text->draw_string(10,20, "Redrawing viewport at %d,%d.", viewport_x, viewport_y);
	gl_text->draw_string(10,40, "Loading tiles...");
	glDrawBuffer(GL_BACK);
//	#endif

	check_textures();

	gl_image->make_current();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(viewport_x, viewport_x + viewport_width,
			viewport_y + viewport_height, viewport_y,
			1.0,-1.0);
		
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	/* Statically top-left four tiles */
#if TRUE
	glMatrixMode(GL_MODELVIEW);
	GLuint tex_id;

	for (short y = viewport_start_row; y <= viewport_end_row; y++) {
	for (short x = viewport_start_col; x <= viewport_end_col; x++) {
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

	gl_image->swap();
}

/* Draw ROI outlines */
void ImageGL::draw_rois(void)
{
	gl_image->make_current();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix(); /* Don't destroy tile transform */
	/* Scale to work in image pixels */
	glLoadIdentity();
   
    // draw exiting roi's
    vector<ROI*> rois=roiset->get_regions();
    int i,j;
	int red,green,blue;
    
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
				draw_existing_roi_entity(entity);
			}
		}
	}

	// draw the ROI that is currently being defined (if there is one)
	if (roiset->editing())
	{
		ROIEntity *entity=roiset->get_current_entity();
        glColor4f(1.0, 1.0, 1.0, 1.0);		
		draw_new_roi_entity(entity);
	}

		
	glPopMatrix(); /* Restore tile transform */
	assert(glGetError() == GL_NO_ERROR);
}

// draw an ROI entity in the current colour
void ImageGL::draw_existing_roi_entity(ROIEntity *entity)
{
	int k;
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


// draw an ROI entity in the current colour
void ImageGL::draw_new_roi_entity(ROIEntity *entity)
{
	int k;
    const char *type=entity->get_type();
	vector<coords> points=entity->get_points();   

	// do nothing if no points have been defiend
	if (points.size()==0)
		return;
		             
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
 		glVertex3f(mouse_x, mouse_y, 0.0);	    
	    glEnd();
	}
	else if (type==ROI_RECT)
	{
	    // draw outline of rectangle
		glBegin(GL_LINE_LOOP);
		{
            coords topleft=points.at(0);
   			glVertex3f(topleft.x, topleft.y, 0.0);
   			glVertex3f(topleft.x, mouse_y, 0.0);
   			glVertex3f(mouse_x, mouse_y, 0.0);
   			glVertex3f(mouse_x, topleft.y, 0.0);                   			
	    }
	    glEnd();                     
	}	
}


void ImageGL::notify_bands(void)
{
	#if DEBUG_GL
	Console::write("(II) ImageGL::notify_bands()\n");
	#endif
	int r, g, b;
	viewport->get_display_bands(&band_red, &band_green, &band_blue);
	
	tile_textures.assign(tile_count, 0);
	free_textures = textures;
	notify_viewport();
}

void ImageGL::check_textures(void)
{
	check_tileset();
			
	/* Find new display rect */
	int new_start_row, new_start_col, new_end_row, new_end_col;
	new_start_col = viewport_x / tile_image_size;
	new_start_row = viewport_y / tile_image_size;

	new_end_col = new_start_col + (viewport_width / tile_image_size) + 1;
	new_end_row = new_start_row + (viewport_height / tile_image_size) + 1;

	new_end_row = min(new_end_row, tile_rows - 1);
	new_end_col = min(new_end_col, tile_cols - 1);


	/* free un-used texture IDs */
	gl_image->make_current();
	assert(tileset != NULL);
	/* Compare new exposed tiles to old */
	/* Delete old & ! new */
	// For each column within old or new set bounds
	for (int x = min (viewport_start_col, new_start_col);
		x <= max (viewport_end_col, new_end_col); x++) {
			// if this column is within the new set, load it
			if ((x >= new_start_col) && (x <= new_end_col)) {
				for (int y = new_start_row; y <= new_end_row; y++) {
					load_tile_tex(x,y);
				}
			} else { // unload it
				for (int y = viewport_start_row; y <= viewport_end_row; y++) {
					free_tile_texture(x,y);
				}
			}
		}
		
	// For each row within old or new set bounds
	for (int y = min (viewport_start_row, new_start_row);
		y <= max (viewport_end_row, new_end_row); y++) {
			// If this row is within the new set, load it
			if ((y >= new_start_row) && (y <= new_end_row)) {
				for (int x = new_start_col; x <= new_end_col; x++) {
					load_tile_tex(x,y);
				}
			} else { // unload it
				for (int x = viewport_start_col; x <= viewport_end_col; x++) {
					free_tile_texture(x,y);
				}
			}
		}

	viewport_start_row = new_start_row;
	viewport_start_col = new_start_col;
	viewport_end_row = new_end_row;
	viewport_end_col = new_end_col;
	

	/* Have we messed up? */
	assert(glGetError() == GL_NO_ERROR);
}

void ImageGL::load_tile_tex(int x_index, int y_index) {
	#if DEBUG_GL_TEXTURES
	Console::write("(II) load_tile_tex(%d, %d)\n", x_index, y_index);
	#endif
	char* tex_data;
	int tile_x = x_index * tile_image_size;
	int tile_y = y_index * tile_image_size;
	GLuint tex_id;
	
	// Only load if not already
	if (get_tile_texture(x_index, y_index) != 0) return;
	
	if (free_textures.size() == 0) add_new_texture();
	assert(free_textures.size() > 0);
	// Load the tile data
	tex_data = tileset->get_tile_RGB(tile_x,tile_y,band_red,band_green,band_blue);
	
	// Grab a free texture ID
	tex_id = free_textures.back();
	free_textures.pop_back();
	
	// Load the data into the texture
	gl_image->make_current();
	assert(glIsTexture(tex_id) == GL_TRUE);
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
		tile_rows = tileset->get_rows();
		tile_cols = tileset->get_columns();
		tile_count = tile_rows * tile_cols;
		#if DEBUG_GL
		Console::write("Allocating  ");
		Console::write(tile_count);
		Console::write(" tiles.\n");
		#endif
		tile_textures.assign(tile_count, 0);
			
		viewport_start_col = 400;
		viewport_start_row = 400;
		viewport_end_col = -1;
		viewport_end_row = -1;
			
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

	/* Re-size the OpenGL context */
	gl_image->resize();

	// We use this as an invalid test case later, so we don't want it to work...
	assert(glIsTexture(0) == GL_FALSE);

	viewport->set_window_size(gl_image->width(), gl_image->height());
}

void ImageGL::add_new_texture(void)
{
	gl_image->make_current();
	/* Get another texture ID and set it up */
	GLuint new_tex;
	GLint proxy_width; // used for checking available video memory
	glGenTextures(1, &new_tex);
	glBindTexture(GL_TEXTURE_2D, new_tex);
	/* Set up wrapping and filtering parameters for this texture */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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

unsigned char* ImageGL::get_pixel_values(int image_x, int image_y)
{
	if (tileset != NULL)
		return tileset->get_pixel_values(image_x, image_y);
		
	/* If we don't currently have a tileset, the values will be wrong anyway,
		so we should just avoid delete[]'ing memory we haven't new[]'ed.
		This might occur for a few instants between the window being shown and
		the tileset being loaded... or something */
	return new unsigned char[image_file->getImageProperties()->getNumBands()];
}

// Return the texture id of a tile at column/row index:
GLuint ImageGL::get_tile_texture(int x_index, int y_index) {
	int tile_index = y_index*tile_cols + x_index;
	assert(tile_textures.size() > tile_index);
	#if DEBUG_GL_TEXTURES
	Console::write("(II) get_tile_texture(%d, %d) - tile_textures[%d]-> %d\n",
			x_index, y_index, tile_index, tile_textures.at(tile_index) );
	#endif
	return tile_textures.at(tile_index);
}
// Set the texture id of a tile at column/row index:
void ImageGL::set_tile_texture(int x_index, int y_index, GLuint new_id) {
	int tile_index = y_index*tile_cols + x_index;
	assert(tile_textures.size() > tile_index);
	#if DEBUG_GL_TEXTURES
	Console::write("(II) set_tile_texture(%d, %d) - tile_textures[%d] = %d\n",
			x_index, y_index, tile_index, new_id);
	#endif
	tile_textures[tile_index] = new_id;
}

void ImageGL::free_tile_texture(int x_index, int y_index)
{
	#if DEBUG_GL_TEXTURES
	Console::write("(II) free_tile_texture(%d, %d)\n", x_index, y_index);
	#endif
	GLuint tex_id;
	int tile_index = y_index*tile_cols + x_index;
	assert(tile_textures.size() > tile_index);
	tex_id = tile_textures[tile_index];
	if (tex_id != 0) {
		free_textures.push_back(tex_id);
		tile_textures[tile_index] = 0;
	}
}

void ImageGL::set_brightness_contrast(float brightness_arg, float contrast_arg)
{
	#if DEBUG_GL
	Console::write("ImageGL::set_brightness_contrast(%1.4f,%1.4f)\n", brightness_arg, contrast_arg);
	#endif
	
	glPushAttrib(GL_MATRIX_MODE);
	glMatrixMode(GL_COLOR);
	glLoadIdentity();

	// Set brightness
	glTranslatef(brightness_arg,brightness_arg,brightness_arg);
	// Set contrast
	glScalef(contrast_arg,contrast_arg,contrast_arg);

	glPopAttrib();
	notify_bands();
}
