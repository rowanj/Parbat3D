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
//#else
//	error_text = "Not using ImageFile.";
#endif	
	
	/* Initialize OpenGL*/
	/* OO version */
#if TMP_USE_OO_OPENGL
#else // Use OO_OPENGL
#endif // Use OO_OPENGL

}

ImageHandler::~ImageHandler(void)
{
#if DEBUG_IMAGE_HANDLER
	MessageBox (NULL, "Shutting down image handler.", "Parbat3D :: ImageHandler", 0);
#endif
#if TMP_USE_OO_OPENGL
	delete gl_overview;
	delete gl_image;
#endif
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
#if TMP_USE_OO_OPENGL
	gl_overview->GLresize();
	gl_image->GLresize();
#endif
}

void ImageHandler::post_init( void )
{
	this->init_GL();
}

void ImageHandler::init_GL( void )
{
#if TMP_USE_OO_OPENGL
#if DEBUG_IMAGE_HANDLER
	MessageBox (NULL, "post_init() - OO version.", "Parbat3D :: ImageHandler", 0);
#endif
	if (gl_overview = new ImageGLView(hOverview)) { // Instantiated successfully
		if (gl_overview->status) {
			status = 4;
			error_text = gl_overview->error_text;
		} else {
			gl_overview->GLresize();
			gl_overview->GLinit();
		}
	} else {
		status = 5;
		error_text = "Could not create ImageGLView for overview.";
	}
	
	if (gl_image = new ImageGLView(hOverview)) {
		if (gl_image->status) {
			status = 6;
			error_text = gl_overview->error_text;
		} else {
			gl_overview->GLinit();
		}
	} else {
		status = 7;
		error_text = "Could not create ImageGLView for image window.";
	}
#else
#if DEBUG_IMAGE_HANDLER
	MessageBox (NULL, "post_init() - non-OO version.", "Parbat3D :: ImageHandler", 0);
#endif
#endif	
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
