#ifndef _IMAGE_OVERVIEW_H
#define _IMAGE_OVERVIEW_H

#include "ImageGLView.h"
#include "ImageFile.h"
#include "ImageTileSet.h"

class ImageOverview
{
  public:
	ImageOverview(HWND window_hwnd, ImageFile* image_file);
	virtual ~ImageOverview(void);
	
	void set_bands(int band_R, int band_G, int band_B);
	void redraw(void);

	void resize_viewport(int new_width, int new_height);
	void move_viewport(int new_x, int new_y);

  private:
	void make_texture(void);
	
	ImageGLView* gl_overview;
	int band_red, band_green, band_blue;
	
	int image_width, image_height;
	int viewport_width, viewport_height;
	int viewport_x, viewport_y;
	
   	/* Overview window texture */
	ImageTileSet* tileset;
	GLint texture_size;
	char *tex_overview;
	GLuint tex_overview_id;
};

#endif
