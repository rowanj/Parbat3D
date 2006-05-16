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
	
		// Initialize image file (could be threaded)
	image_file = new ImageFile(filename);
#if DEBUG_IMAGE_PROPERTIES
	image_file->printInfo();
#endif	
}

ImageHandler::~ImageHandler(void)
{
	delete gl_overview;
	delete gl_image;
	delete image_file;
}

void ImageHandler::redraw(void)
{
	// Overview window
	gl_overview->make_current();
	glClearColor(0.1f, 0.3f, 0.1f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
//	gluLookAt(0.0, -2.0, -2.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0);

//	glPushMatrix();
	glBegin(GL_QUADS);
		glColor3f(0.5, 0.5, 0.5);
		glVertex3i(-1, 1, 0);
		glVertex3i( 1, 1, 0);
		glVertex3i( 1,-1, 0);
		glVertex3i(-1,-1, 0);
	glEnd();
	
#if DEBUG_IMAGE_REDRAW
	glRotatef(redraw_rotz, 0.0,0.0,1.0);
	redraw_rotz-=1.0;
	if(redraw_rotz < -360.0) redraw_rotz+=360.0;
	glBegin(GL_LINES);
		glColor3f(1.0,1,0);
		glVertex3i(0,0,0);
		glVertex3i(0,1,0);
	glEnd();
#endif
//	glPopMatrix();
	gl_overview->GLswap();
	// Main image window
	gl_image->make_current();
	glClearColor(0.3f, 0.1f, 0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_image->GLswap();
}

ImageProperties* ImageHandler::get_image_properties(void)
{
	return image_file->getImageProperties();
}

BandInfo* ImageHandler::get_band_info(int band_number)
{
	return image_file->getBandInfo(band_number);
}

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
