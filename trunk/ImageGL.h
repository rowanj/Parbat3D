#ifndef _IMAGE_GL_H
#define _IMAGE_GL_H

#include "GLView.h"
#include "GLText.h"
#include "ImageFile.h"
#include "ImageTileSet.h"
#include "ImageViewport.h"
#include "ROISet.h"

#include <queue>
#include <vector>

class ImageGL : public ViewportListener
{
  public:
	ImageGL(HWND window_hwnd, ImageFile* image_file, ImageViewport* image_viewport_param, ROISet *roisToOutline);
	virtual ~ImageGL(void);
	
	void resize_window(void);
	
	unsigned int* get_pixel_values(int image_x, int image_y); // remember to delete[]
	
	void notify_viewport(void);
	void notify_bands(void);
	
	// set mouse position in image co-ords (for drawing new ROI)	
	void set_mouse_position(int image_x, int image_y) {mouse_x=image_x; mouse_y=image_y;};
	
	void set_brightness_contrast(float brightnes_arg, float contrast_arg);

  private:
	/* Helper functions */
	void check_textures(void);
	void check_tileset(void);
	void flush_textures(void);
	void load_tile_tex(int x_index, int y_index);
	GLuint	get_tile_texture(int x_index, int y_index);
	void	free_tile_texture(int x_index, int y_index);
	void	set_tile_texture(int tile_x, int tile_y, GLuint new_id);
	void	add_new_texture(void);
	void draw_rois(void);
	void draw_existing_roi_entity(ROIEntity *entity);
	void draw_new_roi_entity(ROIEntity *entity);	
	
	/* Sub-objects */
	GLView* gl_image;
	GLText* gl_text;
	ImageFile* image_file;
	ImageViewport* viewport;
	ROISet* roiset;        // set of ROIs to be displayed
	
	/* State variables */
	int cache_size; // MB maximum decoded tiles kept in cache (static)
	int image_width, image_height; // Image dimensions (static)
	
	/* Viewport positon locals
		These will change on zoom/scroll */
	int viewport_width, viewport_height; // Image pixels displayed in window
	int viewport_x, viewport_y; // Top-left corner (image pixels)
	int mouse_x, mouse_y;		// Current mouse position in image co-ords (for drawing new ROI)

	/* Image window textures */
	/* Set these up on window resize */
		ImageTileSet* tileset; // Tileset object we populate textures from
		int window_width, window_height; // Screen pixels, GL context dimensions
		vector<GLuint> free_textures; // Which IDs are currently un-used
		vector<GLuint> textures; // Which IDs are available (used or not)
		int viewport_start_row, viewport_start_col;
		int viewport_end_row, viewport_end_col;
	
	/* Set these up on LOD change */
		int LOD; // Sample density factor (0 = 1:1, 1 = 1:2, 2 = 1:4, ...)
		vector<GLuint> tile_textures; // Array of texture IDs for correct tile
		int  tile_image_size; // Size of tile in image pixels
		int  tile_rows, tile_cols, tile_count; // Total number of rows/cols 
		int LOD_width, LOD_height; // Scaled dimensions at this LOD (*not* zoom level)

	/* General OpenGL stuff */
		unsigned int list_tile; // display list for textured tile
		int texture_size; // Dimension of each texture
	
	/* Band information */
		int band_red, band_green, band_blue;
};

#endif
