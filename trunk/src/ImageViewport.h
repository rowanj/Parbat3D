#ifndef IMAGE_VIEWPORT_H
#define IMAGE_VIEWPORT_H

#include "ImageProperties.h"
#include "ViewportListener.h"

class ImageViewport
{
public:
	ImageViewport(ImageProperties* image_properties);
	virtual ~ImageViewport(void);
	
	float set_zoom_level(float zoom_value);
	float get_zoom_level(void);
	float get_zoom_minimum(void);
	
	int get_zoom_image_width(void);
	int get_zoom_image_height(void);
	void set_window_size(int new_window_width, int new_window_height);
	int get_window_width(void);
	int get_window_height(void);
	int get_viewport_width(void); // Image pixels represented
	int get_viewport_height(void); // Image pixels represented
	
	void set_zoom_x(int new_x);
	void set_zoom_y(int new_y);
	int get_zoom_x(void);
	int get_zoom_y(void);
	void set_image_x(int new_x);
	void set_image_y(int new_y);
	int get_image_x(void);
	int get_image_y(void);
	
	void set_display_bands(int band_R, int band_G, int band_B);
	void get_display_bands(int* red_return, int* green_return, int* blue_return);
	
	/* Returns -1 on out-of-bounds */
	void translate_window_to_image(int window_x, int window_y,
		int* image_x_return, int* image_y_return);
	
	/* Add a function to the notification list */
	void register_listener(ViewportListener* target);
	void notify_viewport_listeners(void);
	void notify_band_listeners(void);
	
private:
	/* Listeners */
	std::vector<ViewportListener*> listeners;
	
	/* Local variables */
    float zoom_level;
    float zoom_minimum;
	int zoom_x, zoom_y;
	
	int num_bands;
	int image_x, image_y;
	int image_width, image_height;

	int window_width, window_height;

   	int zoom_image_width, zoom_image_height;
   	int viewport_width, viewport_height;
    int band_red, band_green, band_blue;    
};

#endif
