#ifndef _IMAGE_OVERVIEW_H
#define _IMAGE_OVERVIEW_H

#include "GLView.h"
#include "ImageFile.h"
#include "ImageTileSet.h"

class OverviewGL
{
  public:
	OverviewGL(HWND window_hwnd, ImageFile* image_file);
	virtual ~OverviewGL(void);
	
	void set_bands(int band_R, int band_G, int band_B);
	void update_viewport(int x, int y, int width, int height);
	void redraw(void);

  private:
	void make_texture(void);
	
	GLView* gl_overview;
	int band_red, band_green, band_blue;
	
	int image_width, image_height;
	int viewport_width, viewport_height;
	int viewport_x, viewport_y;
	
	/* General OpenGL stuff */
	GLfloat scalefactor_tile, scalefactor_lines;
	unsigned int list_tile; // display list for textured tile
		
   	/* Overview window texture */
	ImageTileSet* tileset;
	GLint texture_size;
	int LOD_height, LOD_width;
	GLuint tex_overview_id;
};

#endif
