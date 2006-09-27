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
	ImageHandler(HWND overview_hwnd, HWND image_hwnd, char* filename, ROISet *roisToOutline);
	virtual ~ImageHandler(void);
	
	/* Data Operators */
	ImageProperties* get_image_properties(void);
	BandInfo* get_band_info(int band_number);
	const char* get_info_string(void);
	
	/* Window Operators */
	void redraw(void);
	void resize_image_window(void);
	ImageViewport* get_image_viewport(void);
	ImageFile* get_image_file(void);
	
	// get pixel values from window co-ordinates
	unsigned char* get_window_pixel_values(int x, int y); // remember to delete[]
	// get pixel values from absolute coordinates at this zoom level
	unsigned char* get_zoom_pixel_values(int x, int y); // remember to delete[]
    // get pixel values from absolute image coordinates (from displayed LOD)
    unsigned char* get_image_pixel_values(int x, int y); // remember to delete[]
       
    /* Status operators */
	int get_status(void);
	const char* get_status_text(void);

	/* Contrast/brigtness */
	void set_brightness_contrast(int new_brightness, int new_contrast);
	void get_brightness_contrast(int* brightness_return, int* contrast_return);
	void reset_brightness_contrast(void);
	
	/* Set Current Mouse Position In Image Co-Ords (For ROI Drawing) */
	void set_mouse_position(int ix,int iy) {image_gl->set_mouse_position(ix,iy);};

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
	
	int contrast_value;
	int brightness_value;
	
};

#endif
