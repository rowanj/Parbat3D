#include "ImageHandler.h"

#include <cassert>

// Enable this to include debugging
#define DEBUG_IMAGE_HANDLER 1

#if DEBUG_IMAGE_HANDLER
#include "console.h"
#endif

ImageHandler::ImageHandler(HWND overview_hwnd, HWND image_hwnd, char* filename, ROISet *ROI_set)
{
	#if DEBUG_IMAGE_HANDLER
	Console::write("(II) Initializing image handler(%d, %d, %s)\n", overview_hwnd, image_hwnd, filename);
	#endif
	
	brightness_value = 250;
	contrast_value = 250;
	
	// Check for lazily unspecified (NULL argument) parameters
	assert(overview_hwnd != NULL);
	assert(image_hwnd != NULL);
	assert(filename != NULL);
				
	// Initialize image file (could be threaded)
	image_file = new ImageFile(filename);
	assert(image_file != NULL);
	// !! If image_file worked...
	{
		image_properties = NULL;
		image_properties = image_file->getImageProperties();
		assert(image_properties != NULL);
		image_viewport = NULL;
		image_viewport = new ImageViewport(image_properties);
		assert(image_viewport != NULL);

		/* Initialize viewports */
		overview_gl = NULL;
		overview_gl = new OverviewGL(overview_hwnd, image_file, image_viewport);
		assert(overview_gl != NULL);
		image_gl = NULL;
		image_gl = new ImageGL(image_hwnd, image_file, image_viewport, ROI_set);
		assert(image_gl != NULL);
		
		image_viewport->set_display_bands(1,2,3);
	}
}

ImageHandler::~ImageHandler(void)
{
	#if DEBUG_IMAGE_HANDLER
	Console::write("(II) ImageHandler shutting  down...\n");
	#endif
	delete image_viewport;
	delete overview_gl;
	delete image_gl;
	delete image_file;
	#if DEBUG_IMAGE_HANDLER
	Console::write("(II) ImageHandler shutdown complete.\n");
	#endif
}

void ImageHandler::redraw(void)
{
	image_viewport->notify_viewport_listeners();
}

ImageProperties* ImageHandler::get_image_properties(void)
{
	assert(image_properties != NULL);
	return image_properties;
}

BandInfo* ImageHandler::get_band_info(int band_number)
{
	return image_file->getBandInfo(band_number);
}

void ImageHandler::resize_image_window(void)
{
    image_gl->resize_window();
}

/* This function gets pixel values in absolute image coordinates */
unsigned char* ImageHandler::get_image_pixel_values(int x, int y)
{
	return image_gl->get_pixel_values(x,y);
}
/* This function gets pixel values from absolute coordinates at zoom scaling*/
unsigned char* ImageHandler::get_zoom_pixel_values(int x, int y)
{
	float zoom_factor = image_viewport->get_zoom_level() / 100.0;
	/* Translate zoom coordinates to image coordinates */
	x = int(round(zoom_factor * float(x)));
	y = int(round(zoom_factor * float(y)));
		
	/* Resume query */
	return get_image_pixel_values(x,y);
}


/* This function gets pixel values from the current viewport */
unsigned char* ImageHandler::get_window_pixel_values(int x, int y)
{
	int ix, iy;
	image_viewport->translate_window_to_image(x, y, &ix, &iy);
	return image_gl->get_pixel_values(ix,iy);
}

const char* ImageHandler::get_info_string(void)
{
    return image_file->getInfoString();
}    

// Should be given parameters between 1 and 500 (inclusive)
void ImageHandler::set_brightness_contrast(int new_brightness, int new_contrast)
{
	Console::write("ImageHandler::set_contrast_brightness(%d, %d)\n", new_brightness, new_contrast);

	float brightness_param, contrast_param;
	brightness_value = new_brightness;
	contrast_value = new_contrast;
	
	brightness_param = (float(brightness_value) / 250.0) - 1.0;
	contrast_param = float(contrast_value) / 250.0;
	
	image_gl->set_brightness_contrast(brightness_param, contrast_param);
	overview_gl->set_brightness_contrast(brightness_param, contrast_param);
}
void ImageHandler::get_brightness_contrast(int* brightness_return, int* contrast_return)
{
	if (brightness_return != NULL) *brightness_return = brightness_value;
	if (contrast_return != NULL) *contrast_return = contrast_value;
}

void ImageHandler::reset_brightness_contrast(void)
{
	set_brightness_contrast(250,250);
}

