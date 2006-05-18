#ifndef _IMAGE_HANDLER_H
#define _IMAGE_HANDLER_H

#include <string>
#if PLATFORM_W32
#include <windows.h>
#endif

#include "ImageFile.h"
#include "ImageProperties.h"
#include "ImageGLView.h"

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

	/* State variables */
	int image_width, image_height;
    int max_texture_size;
    bool dirty;
    
    /* Overview window texture */
	unsigned int tex_overview_id;
	char *tex_overview;
};

#endif
