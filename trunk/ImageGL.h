#ifndef _IMAGE_GL_H
#define _IMAGE_GL_H

#include "GLView.h"
#include "ImageFile.h"
#include "ImageTileSet.h"
#include "ImageViewport.h"
#include "ROISet.h"

class ImageGL : public ViewportListener
{
  public:
	ImageGL(HWND window_hwnd, ImageFile* image_file, ImageViewport* image_viewport_param, ROISet *roisToOutline);
	virtual ~ImageGL(void);
	
	void resize_window(void);
	
	unsigned int* get_pixel_values(int image_x, int image_y); // remember to delete[]
	
	void notify_viewport(void);
	void notify_bands(void);

  private:
	/* Helper functions */
	void make_texture(void);
	
	/* Sub-objects */
	GLView* gl_image;
	ImageFile* image_file;
	ImageViewport* viewport;
	ROISet* roiset;        // set of ROIs to be displayed
	
	/* State variables */
	int LOD;
	int LOD_width, LOD_height;
	int image_width, image_height;
	int window_width, window_height;
	int viewport_width, viewport_height;
	int viewport_x, viewport_y;
	
	/* General OpenGL stuff */
	unsigned int list_tile; // display list for textured tile

	/* Image window textures */
	ImageTileSet* tileset;
	GLuint* textures;
	int band_red, band_green, band_blue;
	int texture_rows, texture_columns, tex_count;
	int texture_size;
	int cache_size;
	int start_column, start_row;
	
    GLfloat scalefactor_lines;  // scale factor used when drawing ROI outlines
};

#endif
