#ifndef _IMAGE_HANDLER_H
#define _IMAGE_HANDLER_H

#include <string>
#if PLATFORM_W32
#include <windows.h>
#endif

#include "ImageFile.h"
#include "ImageTileSet.h"
#include "ImageProperties.h"
#include "ImageGLView.h"

#include "config.h"

typedef struct pixel_values_t {
	char number_bands;
	int* values;
} pixel_values, *pixel_values_ptr;

typedef struct geo_coords_t {
	int x;
	int y;
	float lattitude;
	float longitude;
} geo_coords, *geo_coords_ptr;


class ImageHandler
{
public:
	/* Class Operators */
	ImageHandler(HWND overview_hwnd, HWND image_hwnd, char* filename);
	virtual ~ImageHandler(void);
	
	/* Data Operators */
	ImageProperties* get_image_properties(void);
	BandInfo* get_band_info(int band_number);
		/* free() pixel_values_ptr->values and pixel_values_ptr after use */
	int* get_pixel_values(int x, int y);
	void get_geo_pos(geo_coords_ptr pos);
	const char* get_info_string(void);
	
	/* Viewport Operators */
	void redraw(void);
	void resize_window(void);
	void set_viewport(int x, int y);
	void set_bands(int band_R, int band_G, int band_B);
	int get_viewport_x(void);
	int get_viewport_y(void);
	int get_viewport_width(void);
	int get_viewport_height(void);
	
	/* Control current zoom, 0 = 1:1, 1 = 2:1, 2 = 4:1, etc. */
	int get_LOD(void);
	int set_LOD(int level_of_detail);
	
	/* Image dimension (screen pixels) at current zoom */
	int get_LOD_width(void);
	int get_LOD_height(void);
	
	/* State Variables */
	int status;
	const char* error_text;

private:
	/* Sub-objects */
	ImageFile* image_file;

	ImageGLView* gl_overview;
	ImageGLView* gl_image;

	/* Internal Functions */
	void make_overview_texture(void);
	void make_textures(void);
	void GLinit(void);

	/* State variables */
	int LOD;
	int image_width, image_height;
    int max_texture_size;
    int band_red, band_green, band_blue;
	int texture_size, texture_size_overview;
	int viewport_width, viewport_height, viewport_x, viewport_y;
	int viewport_columns, viewport_rows;
	
    /* Overview window texture */
	ImageTileSet* overview_tileset;
	char *tex_overview;
	GLuint tex_overview_id;
	
	/* Image window textures */
	ImageTileSet* image_tileset;
	int tex_rows, tex_columns, tex_count;
	GLuint *tex_base;
	int tile_size;
	
	/* Display lists */
	unsigned int list_tile;
};

#endif
