#include "config.h"
#include "ImageHandler.h"
#include "TestUtils.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include <math.h>
#define PI 3.14159265

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
		
	if (!image_file) {
		status = 5;
		error_text = "Could not create ImageFile object.";
	} else {
		image_properties = image_file->getImageProperties();
		image_height = image_properties->getHeight();
		image_width = image_properties->getWidth();
	}
#if DEBUG_IMAGE_PROPERTIES
	image_file->printInfo();
#endif
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*) &max_texture_size);
    tex_overview = (char*) malloc(256*256*3);
	this->make_textures();

	this->resize_window();
}

ImageHandler::~ImageHandler(void)
{
	free(tex_overview);
	delete gl_overview;
	delete gl_image;
	delete image_file;
}

void ImageHandler::redraw(void)
{
	GLenum errorcode;
	
	// Overview window
	gl_overview->make_current();
	glClearColor(0.1f, 0.3f, 0.1f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
//	glBindTexture(GL_TEXTURE_2D, (GLuint) texName);
	glBindTexture(GL_TEXTURE_2D, (GLuint) tex_overview_id);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// !! Vertical eye offset will need to be a great deal more complex to deal with
	//	portrait-oriented imagery
	gluLookAt(0.0,0.0,(0.5/tan(PI/6.0)),0.0,0.0,0.0,0.0,1.0,0.0);

	// Set up aspect transform
	if (image_width >= image_height) {
		glScalef(1.0, (GLfloat)image_height/(GLfloat)image_width, 1.0);
	} else {
		glScalef((GLfloat)image_width/(GLfloat)image_height, 1.0, 1.0);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-0.5, 0.5, 0.0);
		glTexCoord2f(1.0, 0.0);
		glVertex3f( 0.5, 0.5, 0.0);
		glTexCoord2f(1.0, 1.0);
		glVertex3f( 0.5,-0.5, 0.0);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-0.5,-0.5, 0.0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	
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
	gl_overview->GLswap();
	
	// Main image window
	gl_image->make_current();
	glClearColor(0.3f, 0.1f, 0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_image->GLswap();
}

ImageProperties* ImageHandler::get_image_properties(void)
{
	return image_properties;
}

BandInfo* ImageHandler::get_band_info(int band_number)
{
	return image_file->getBandInfo(band_number);
}

void ImageHandler::resize_window(void)
{
	gl_overview->GLresize();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// !! Does this deal with files smaller than 10px wide?
	// !! Will need to compensate for portrait-oriented images
	gluPerspective(60.0, (GLfloat) gl_overview->width()/(GLfloat) gl_overview->height(), 0.1, 2.0);
	
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

const char* ImageHandler::get_info_string(void)
{
    return image_file->getInfoString();
}    

void ImageHandler::make_textures(void)
{
	// Make textures
		int i, j, c;
	
	// 64*64 Checkerboard for debug
	for (i = 0; i < 64; i++) {
		for (j = 0; j < 64; j++) {
			c = ((((i&0x8)==0) ^ ((j&0x8))==0))*255;
			checkImage[i][j][0] = (GLubyte) 255;
			checkImage[i][j][1] = (GLubyte) c;
			checkImage[i][j][2] = (GLubyte) c;
		}
	}
	
	// Make texture for overview
	image_file->getRasterData(image_width, image_height, 0, 0, (char*)tex_overview, 256, 256);
	
	gl_overview->make_current();
	glShadeModel(GL_FLAT);
	glDisable(GL_DEPTH_TEST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, (GLuint*) &texName);	
	glBindTexture(GL_TEXTURE_2D, (GLuint) texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,  64,  64, 0, GL_RGB, GL_UNSIGNED_BYTE, checkImage);
	
	
	glGenTextures(1, (GLuint*) &tex_overview_id);
	glBindTexture(GL_TEXTURE_2D, (GLuint) tex_overview_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_overview);
	/* */
}
