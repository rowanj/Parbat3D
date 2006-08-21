#ifndef _IMAGE_HANDLER_H
#define _IMAGE_HANDLER_H

#include <string>
#if PLATFORM_W32
#include <windows.h>
#endif

#include "ImageFile.h"
#include "ImageProperties.h"
#include "OverviewGL.h"
#include "ImageGL.h"
#include "ImageViewport.h"

#include "config.h"

class ImageHandler
{
public:
	/* Class Operators */
	ImageHandler(HWND overview_hwnd, HWND image_hwnd, char* filename);
	virtual ~ImageHandler(void);
	
	/* Data Operators */
	ImageProperties* get_image_properties(void);
	BandInfo* get_band_info(int band_number);
	const char* get_info_string(void);
	
	/* Window Operators */
	void redraw(void);
	void resize_image_window(void);
	ImageViewport* get_image_viewport(void);
	
	// get pixel values from window co-ordinates
	unsigned int* get_window_pixel_values(int x, int y); // remember to delete[]
	// get pixel values from absolute coordinates at this zoom level
	unsigned int* get_zoom_pixel_values(int x, int y); // remember to delete[]
    // get pixel values from absolute image coordinates (from displayed LOD)
    unsigned int* get_image_pixel_values(int x, int y); // remember to delete[]
       
    /* Status operators */
	int get_status(void);
	const char* get_status_text(void);

private:
	/* Sub-objects */
	ImageFile* image_file;
	ImageProperties* image_properties;
	ImageViewport* image_viewport;
	ImageGL* image_gl;
	OverviewGL* overview_gl;

	/* State variables */
	int status;
	const char* status_text;
};

#endif
