#ifndef _IMAGE_OVERVIEW_H
#define _IMAGE_OVERVIEW_H

#include "GLView.h"
#include "ImageFile.h"
#include "ImageTileSet.h"
#include "ImageViewport.h"

class OverviewGL : public ViewportListener
{
  public:
	OverviewGL(HWND window_hwnd, ImageFile* image_file, ImageViewport* image_viewport_param);
	virtual ~OverviewGL(void);
	
	void notify_viewport(void);
	void notify_bands(void);

  private:
	void make_texture(void);
	
	ImageViewport* viewport;
	GLView* gl_overview;
	
	/* General OpenGL stuff */
	GLfloat scalefactor_tile, scalefactor_lines;
	unsigned int list_tile; // display list for textured tile
	
	int image_width, image_height;
	int band_red, band_green, band_blue;
	
   	/* Overview window texture */
	ImageTileSet* tileset;
	GLint texture_size;
	int LOD_height, LOD_width;
	GLuint tex_overview_id;
};

#endif
