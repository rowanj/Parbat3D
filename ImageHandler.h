#ifndef _IMAGE_HANDLER_H
#define _IMAGE_HANDLER_H

#include "config.h"
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
	ImageHandler(HWND overview_hwnd, HWND image_hwnd, char* filename);
	void post_init(void); // We may need to do some initialization after object creation
	virtual ~ImageHandler(void);
	void redraw(void);
	ImageProperties* get_image_properties(void);
	BandInfo* get_band_info(int band_number);
	void resize_window(void);
	PRECT get_viewport(void);
	PRECT set_viewport(void);
	pixel_values_ptr get_pixel_values(unsigned int x, unsigned int y);
	void get_geo_pos(geo_coords_ptr pos);
       
	int status;
	const char* error_text;

private:
	ImageGLView* gl_overview;
	ImageGLView* gl_image;
	HWND hOverview, hImage;
	ImageFile* image_file;
#if DEBUG_IMAGE_REDRAW
	float redraw_rotz;
#endif
};

#if TMP_USE_TEXTURES
static GLubyte checkImage[64][64][4];
static GLuint texName;
void makeCheckImage(void);
#endif


#endif
