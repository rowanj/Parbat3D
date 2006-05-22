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
	int* value;
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
	pixel_values_ptr get_pixel_values(unsigned int x, unsigned int y);
	void get_geo_pos(geo_coords_ptr pos);
	const char* get_info_string(void);   
	
	/* Viewport Operators */
	void redraw(void);
	void resize_window(void);
	PRECT get_viewport(void);
	PRECT set_viewport(void);
	void set_bands(int band_R, int band_G, int band_B);
	int get_LOD(void);
	int set_LOD(int level_of_detail);
	#if TMP_USE_TEXTURES
	int get_LOD_width(void);
	int get_LOD_height(void);
	#endif
	
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

	/* State variables */
	int LOD;
	int image_width, image_height;
    int max_texture_size;
    int band_red, band_green, band_blue;
    bool textures_loaded;
	int texture_size, texture_size_overview;
	
    /* Overview window texture */
	ImageTileSet* overview_tileset;
	char *tex_overview;
	unsigned int *tex_overview_id;
	
	/* Image window textures */
	#if TMP_USE_TEXTURES
	ImageTileSet* image_tileset;
	int tex_rows, tex_columns, tex_count;
	GLuint tex_base[];
	int tile_size;
	#endif
	
	/* Display lists */
	unsigned int list_tile;
};

#endif
