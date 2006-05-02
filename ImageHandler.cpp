
#include "ImageHandler.h"

ImageHandler::ImageHandler(HWND overview_hwnd, HWND main_hwnd, char* filename, int* status_enum)
{
	int tmp_status;
	
	// Check for lazily unspecified (NULL argument) parameters
	if (!overview_hwnd) tmp_status += 1;
	if (!main_hwnd) tmp_status += 2;
	if (!filename) tmp_status += 4;
	
	// If &status is NULL then parent doesn't want notification
	if (status_enum != NULL) {
		*status_enum = tmp_status;
	}
}

ImageHandler::~ImageHandler(void)
{
	;
}

void ImageHandler::redraw(void)
{
	;
}

#if TMP_USE_IMAGE_FILE
ImageProperties* ImageHandler::get_image_properties(void)
{
	;
}
#endif

void ImageHandler::resize_window(void)
{
	;
}

PRECT ImageHandler::get_viewport(void)
{
	return NULL;
}

PRECT ImageHandler::set_viewport(void)
{
	return NULL;
}

pixel_values_ptr ImageHandler::get_pixel_values(unsigned int x, unsigned int y)
{
	return NULL;
}

void ImageHandler::get_geo_pos(geo_coords_ptr pos)
{
	;
}
