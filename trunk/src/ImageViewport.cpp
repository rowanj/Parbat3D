#include "PchApp.h"

#include "ImageViewport.h"

#include "console.h"

using namespace std;

ImageViewport::ImageViewport(ImageProperties* image_properties)
{
	image_x = 0;
	image_y = 0;
	zoom_x = 0;
	zoom_y = 0;
	viewport_width = 0;
	viewport_height = 0;
	window_height = 0;
	window_width = 0;
	zoom_level = 0.0;
	zoom_minimum = 0.01;
	
	assert(image_properties != NULL);
	num_bands = image_properties->getNumBands();
	image_height = image_properties->getHeight();
	image_width = image_properties->getWidth();
	
	zoom_image_width = int(round(image_width * zoom_level));
	zoom_image_height = int(round(image_height * zoom_level));
	
	band_red = 1;
	band_green = min(2,num_bands);
	band_blue = min(3,num_bands);
}

ImageViewport::~ImageViewport(void)
{
	while(!listeners.empty()) {
		listeners.pop_back();	
	}
}
	
float ImageViewport::set_zoom_level(float zoom_value)
{
	int old_viewport_width = viewport_width;
	int old_viewport_height = viewport_height;
	
//	zoom_value = round(zoom_value*100.0)/100.0;
	zoom_level = max(zoom_minimum, zoom_value);
	zoom_image_width = int(round(image_width * zoom_level));
	zoom_image_height = int(round(image_height * zoom_level));
	viewport_width = int(round((float)window_width / zoom_level));
	viewport_height = int(round((float)window_height / zoom_level));
	
	image_x = image_x + (old_viewport_width - viewport_width)/2;
	image_y = image_y + (old_viewport_height - viewport_height)/2;
	
	
	image_x = min(image_width - viewport_width, image_x);
	image_y = min(image_height - viewport_height, image_y);	
	
	image_x = max(image_x, 0);
	image_y = max(image_y, 0);
	
	zoom_x = int(round(image_x * zoom_level));
	zoom_y = int(round(image_y * zoom_level));
	
	notify_viewport_listeners();
	return zoom_level;
}

float ImageViewport::get_zoom_level(void) {return zoom_level;}
float ImageViewport::get_zoom_minimum(void)
{
	// find fit-to-screen
	float horiz_min, vert_min, new_min;
	horiz_min = float(window_width) /  float(image_width);
	vert_min = float(window_height) / float(image_height);
	zoom_minimum = min(horiz_min, vert_min);
	
	// Show a bit of border at minimum zoom (to ensure edges shown)
	zoom_minimum = zoom_minimum * 0.9;
	
	// allow at least zooming out to 50% if small image
	zoom_minimum = min(zoom_minimum, (float)0.5);
	
	return zoom_minimum;
}

int ImageViewport::get_zoom_image_width(void) {return zoom_image_width;}
int ImageViewport::get_zoom_image_height(void) {return zoom_image_height;}

void ImageViewport::set_window_size(int new_window_width, int new_window_height)
{
	window_width = new_window_width;
	window_height = new_window_height;
	viewport_width = int(round((float)window_width / zoom_level));
	viewport_height = int(round((float)window_height / zoom_level));
	get_zoom_minimum();
	if (zoom_level < zoom_minimum) set_zoom_level(zoom_minimum);
	notify_viewport_listeners();
}

int ImageViewport::get_window_width(void) {return window_height;}
int ImageViewport::get_window_height(void) {return window_height;}
int ImageViewport::get_viewport_width(void) {return viewport_width;}
int ImageViewport::get_viewport_height(void) {return viewport_height;}
	
void ImageViewport::set_zoom_x(int new_x)
{
	zoom_x = new_x;
	image_x = int(round(zoom_x / zoom_level));
	notify_viewport_listeners();
}

void ImageViewport::set_zoom_y(int new_y)
{
	zoom_y = new_y;
	image_y = int(round(zoom_y / zoom_level));
	notify_viewport_listeners();
}

int ImageViewport::get_zoom_x(void) {return zoom_x;}
int ImageViewport::get_zoom_y(void) {return zoom_y;}

void ImageViewport::set_image_x(int new_x)
{
	image_x = new_x;
	zoom_x = int(round(image_x * zoom_level));
	notify_viewport_listeners();
}

void ImageViewport::set_image_y(int new_y)
{
	image_y = new_y;
	zoom_y = int(round(image_y * zoom_level));
	notify_viewport_listeners();
}

int ImageViewport::get_image_x(void) {return image_x;}
int ImageViewport::get_image_y(void) {return image_y;}
	
void ImageViewport::set_display_bands(int band_R, int band_G, int band_B)
{
	band_red = min(band_R,num_bands);
	band_green = min(band_G,num_bands);
	band_blue = min(band_B,num_bands);
	notify_band_listeners();
}
void ImageViewport::get_display_bands(int* red_return, int* green_return, int* blue_return)
{
	if (red_return != NULL) *red_return = band_red;
	if (green_return != NULL) *green_return = band_green;
	if (blue_return != NULL) *blue_return = band_blue;
}
	
	/* Returns -1 on out-of-bounds */
void ImageViewport::translate_window_to_image(int window_x, int window_y,
	int* image_x_return, int* image_y_return)
{
	if (image_x_return != NULL) {
		*image_x_return = int(round((zoom_x + window_x) / zoom_level));
	}
	if (image_y_return != NULL) {
		*image_y_return = int(round((zoom_y + window_y) / zoom_level));
	}
	//Console::write("ImageViewport::translate_window_to_image\n window_x=%d image_x=%d \n window_y=%d image_y=%d\n", window_x, *image_x_return, window_y, *image_y_return);
}
	
void ImageViewport::register_listener(ViewportListener* target)
{
	listeners.push_back(target);
}

void ImageViewport::notify_viewport_listeners(void)
{
	int x = 0;
	while (x < listeners.size()) {
		listeners[x]->notify_viewport();
		x++;
	}
}

void ImageViewport::notify_band_listeners(void)
{
	int x = 0;
	while (x < listeners.size()) {
		listeners[x]->notify_bands();
		x++;
	}
}
