#include "ImageHandler.h"

#include <cassert>

#include "config.h"
#include "console.h"

ImageHandler::ImageHandler(HWND overview_hwnd, HWND image_hwnd, char* filename, ROISet *roisToOutline)
{
	status = 0; // No error
	status_text = "No error.";
	reset_contrast_brightness();
	
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
		image_gl = new ImageGL(image_hwnd, image_file, image_viewport, roisToOutline);
		assert(image_gl != NULL);
		
		image_viewport->set_display_bands(1,2,3);
	}
}

ImageHandler::~ImageHandler(void)
{
	Console::write("(II) ImageHandler shutting  down...\n");
	delete image_viewport;
	delete overview_gl;
	delete image_gl;
	delete image_file;
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
unsigned int* ImageHandler::get_image_pixel_values(int x, int y)
{
	return image_gl->get_pixel_values(x,y);
}
/* This function gets pixel values from absolute coordinates at zoom scaling*/
unsigned int* ImageHandler::get_zoom_pixel_values(int x, int y)
{
	float zoom_factor = image_viewport->get_zoom_level() / 100.0;
	/* Translate zoom coordinates to image coordinates */
	x = int(round(zoom_factor * float(x)));
	y = int(round(zoom_factor * float(y)));
		
	/* Resume query */
	return get_image_pixel_values(x,y);
}

/* This function gets pixel values from the current viewport */
unsigned int* ImageHandler::get_window_pixel_values(int x, int y)
{
	int ix, iy;
	image_viewport->translate_window_to_image(x, y, &ix, &iy);
	return image_gl->get_pixel_values(x,y);
}

const char* ImageHandler::get_info_string(void)
{
    return image_file->getInfoString();
}    

int	ImageHandler::get_status(void) {return status;}
const char* ImageHandler::get_status_text(void) {return status_text;}
ImageViewport* ImageHandler::get_image_viewport(void) {return image_viewport;}

void ImageHandler::set_contrast_brightness(int new_contrast, int new_brightness)
{
	contrast_value = new_contrast;
	brightness_value = new_brightness;
	use_ctst_brt = true;
	// !! call function to update display routines
}
void ImageHandler::get_contrast_brightness(int* contrast_return, int* brightness_return)
{
	if (contrast_return != NULL) *contrast_return = contrast_value;
	if (brightness_return != NULL) *brightness_return = brightness_value;
}

void ImageHandler::reset_contrast_brightness(void)
{
	contrast_value = 255;
	brightness_value = 255;
	use_ctst_brt = false;
	// !! call function to update display routines
}
bool ImageHandler::using_contrast_brightness(void) {return use_ctst_brt;}
