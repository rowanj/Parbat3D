#include "ImageGL.h"
#include "Settings.h"
#include <stdlib.h>
#include <cassert>
#include "console.h"
#include "config.h"

ImageGL::ImageGL(HWND window_hwnd, ImageFile* image_file_ptr, ImageViewport* image_viewport_param, ROISet *roisToOutline)
{
	ImageProperties* image_properties;

	assert (window_hwnd != NULL);
	assert (image_file_ptr != NULL);
	assert (image_viewport_param != NULL);

	LOD = -1; // Invalid value to force creation
	tex_count = 0;
	tileset = NULL;
	textures = NULL;
	
	roiset=roisToOutline;
	viewport = image_viewport_param;
	image_file = image_file_ptr;
	image_properties = image_file->getImageProperties();
	image_height = image_properties->getHeight();
	image_width = image_properties->getWidth();	

	cache_size = settingsFile->getSettingi("preferences","cachesize",128);
	
	/* Initialize OpenGL*/
	gl_image = new GLView(window_hwnd);

	/* Initialize OpenGL machine */
    gl_image->make_current();

    glShadeModel(GL_FLAT);
    glDisable(GL_DEPTH_TEST);

    /* Use larger texture if appropriate */
    {
	    GLint max_texture_size;
		int user_texture_size = settingsFile->getSettingi("preferences","texsize",512);
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

	/* Find scale factor for lines */
	scalefactor_lines = 1.0 / (GLfloat) max(image_width, image_height);

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
	glOrtho(viewport->get_image_x(),viewport->get_image_x()+viewport->get_viewport_width(),
			viewport->get_image_y()+viewport->get_viewport_height(),viewport->get_image_y(),
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

	/* Draw ROI outlines over the top of the image */
    {
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix(); /* Don't destroy tile transform */
		/* Scale to work in image pixels */
		glLoadIdentity();
       // glScalef(scalefactor_lines, scalefactor_lines, scalefactor_lines);
	
		/* We'll be using translucent lines */
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        vector<ROI*> rois=roiset->get_regions();
        int i,j,k,red,green,blue;
        for (i=0;i<rois.size();i++)
        {
            ROI* roi=rois.at(i);            
            
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

        // draw test line
		glBegin(GL_LINES);
		{
            
			glColor4f(1.0, 0.0, 0.0, 0.8);
			/* top left */
			glVertex3f(0.0, 0.0, 0.0);
			/* bottom right */
			glVertex3f(100.0, 100.0, 0.0);
		}
		glEnd();
	
		//glDisable(GL_BLEND);
		
		glPopMatrix(); /* Restore tile transform */
	}	
	
	gl_image->GLswap();
}


void ImageGL::notify_bands()
{
	#if DEBUG_GL
	Console::write("(II) ImageGL::notify_bands()\n");
	#endif
	make_texture();
	notify_viewport();
}

void ImageGL::make_texture(void)
{
	#if DEBUG_GL
	Console::write("(II) ImageGL::make_texture()\n");
	#endif
	/* Find necessary tileset LOD */
	int r, g, b;
	viewport->get_display_bands(&r, &g, &b);

	float tmp_zoom = 0.5;
	int needed_LOD = 0;
	while (tmp_zoom >= viewport->get_zoom_level()) {
		tmp_zoom = tmp_zoom/2.0;
		needed_LOD++;
	}
	Console::write("needed_LOD = ");
	Console::write(needed_LOD);
	Console::write("\n");
	
	/* Find needed grid of textures */
	
	/* Do we have the right tileset and color loaded? */
	if (!(needed_LOD == LOD && band_red == r && band_green == g && band_blue == b)) {
		/* If not, we need to free all our textures and start again */
	}
	band_red = r; band_green = g; band_blue = b;
	
	/* If we don't have the right tileset we need to get it */
	if (needed_LOD != LOD) {
		if(tileset != NULL) delete tileset;
		LOD = needed_LOD;
		tileset = new ImageTileSet(LOD, image_file, texture_size, cache_size);
	}
	
	/* Make sure we have enough texture IDs*/
	if (textures == NULL) {
		textures = new GLuint[4];
		glGenTextures(4, textures);
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
		tex_data = tileset->get_tile_RGB(tile_x,tile_y,band_red,band_green,band_blue);
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
	int new_height, new_width;
	int new_tex_rows, new_tex_cols, new_tex_count;
    gl_image->GLresize();
    viewport->set_window_size(gl_image->width(), gl_image->height());
    
}

unsigned int* ImageGL::get_pixel_values(int image_x, int image_y)
{
	if (tileset != NULL)
		return tileset->get_pixel_values(image_x, image_y);
		
	return new unsigned int[image_file->getImageProperties()->getNumBands()];
}
