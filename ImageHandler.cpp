#include "config.h"
#include "ImageHandler.h"

ImageHandler::ImageHandler(HWND overview_hwnd, HWND image_hwnd, char* filename)
{
	status = 0; // No error
	error_text = "No error.";
	
	// Check for lazily unspecified (NULL argument) parameters
	if (!overview_hwnd) {
		status = 1;
		error_text = "No handle for overview window.";
	}
	if (!image_hwnd) {
		status = 2;
		error_text = "No handle for main window.";
	}
	if (!filename) {
		status = 3;
		error_text = "No filename given.";
	}
	
	// Validate handles to objects
	// !! ToDo
	
	hOverview = overview_hwnd;
	hImage = image_hwnd;
	
	// Initialize image file (could be threaded)
#if TMP_USE_IMAGE_FILE
	image_file = new ImageFile(filename);
	image_file->printInfo();
#else
	error_text = "Not using ImageFile.";
#endif	
	
	// Initialize OpenGL (could be threaded)
	if (gl_overview = new ImageGLView(hOverview)) { // Instantiated successfully
		if (gl_overview->status) {
			status = 4;
			error_text = gl_overview->error_text;
		}
	} else {
		status = 5;
		error_text = "Could not create ImageGLView for overview.";
	}
	
	if (gl_image = new ImageGLView(hOverview)) {
		if (gl_image->status) {
			status = 6;
			error_text = gl_overview->error_text;
		}
	} else {
		status = 7;
		error_text = "Could not create ImageGLView for image window.";
	}

}

ImageHandler::~ImageHandler(void)
{
#if DEBUG
	MessageBox (NULL, "Shutting down image handler.", "Parbat3D :: ImageHandler", 0);
#endif
	delete gl_overview;
	delete gl_image;
#if TMP_USE_IMAGE_FILE
	delete image_file;
#endif
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
	LPRECT window_rect;
	GetWindowRect(hOverview,window_rect);
	gl_overview->make_current();
	glViewport(0,0,window_rect->right - window_rect->left,
				window_rect->bottom - window_rect->top);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// ?? Do we have to free() window_rect - Rowan
	GetWindowRect(hImage,window_rect);
	gl_image->make_current();
	glViewport(0,0,window_rect->right - window_rect->left,
				window_rect->bottom - window_rect->top);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// ?? Do we have to free() window_rect - Rowan
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
