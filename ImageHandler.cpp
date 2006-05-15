#include "config.h"
#include "ImageHandler.h"
#include <gl/gl.h>
#include <gl/glu.h>

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
	status = -1; // negative should be treated as non-fatal
	error_text = "Not using ImageFile.";
#endif	
	
	/* Initialize OpenGL*/
	gl_overview = new ImageGLView(hOverview);
	if (!gl_overview) {
		status = 4;
		error_text = "Could not create ImageGLView for overview window.";
	}
	#if DEBUG_IMAGE_HANDLER
	else {
		MessageBox (NULL, "Successfully created ImageGLView for overview window.", "Parbat3D :: ImageHandler", 0);
	}
	#endif
	
	gl_image = new ImageGLView(hImage);
	if (!gl_image) {
		status = 4;
		error_text = "Could not create ImageGLView for image window.";
	}
	#if DEBUG_IMAGE_HANDLER
	else {
		MessageBox (NULL, "Successfully created ImageGLView for image window.", "Parbat3D :: ImageHandler", 0);
	}
	#endif
}

ImageHandler::~ImageHandler(void)
{
	delete gl_overview;
	delete gl_image;
#if TMP_USE_IMAGE_FILE
	delete image_file;
#endif
}

void ImageHandler::redraw(void)
{
	gl_overview->make_current();
	glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_overview->GLswap();
	gl_image->make_current();
	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_image->GLswap();
}

#if TMP_USE_IMAGE_FILE
ImageProperties* ImageHandler::get_image_properties(void)
{
	;
}
#endif

void ImageHandler::resize_window(void)
{
	gl_overview->GLresize();
	gl_image->GLresize();
	this->redraw();
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
