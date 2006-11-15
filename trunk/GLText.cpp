#include "GLText.h"

#include <stdio.h>
#include <Windows.h>
#include <cassert>

GLText::GLText(GLView* gl_view_arg, const char* font_arg, int size_arg)
{
	HFONT h_font;
	HFONT h_oldfont;

	gl_view = gl_view_arg;
	font = font_arg;
	size = size_arg;
	
	assert(gl_view_arg != NULL);
	assert(font != NULL);
	assert(strlen(font) > 0);
	assert(size > 0);
	
	red = 1.0;
	green = 1.0;
	blue = 1.0;
	
	/* !! Windows platform specific font generation */
	h_font = CreateFont(-size, // +ve values indicate cell height, -ve indicates character height
						0,			// font width (0 = use default)
						0, 0,		// angles of escapement and orientation
						FW_NORMAL,	// font weight (i.e. FW_BOLD, FW_BLACK)
						FALSE,		// italic
						FALSE,		// underline
						FALSE,		// strikeout
						ANSI_CHARSET,
						OUT_TT_PRECIS,	// Use TrueType fonts by preference
						CLIP_DEFAULT_PRECIS,
						ANTIALIASED_QUALITY,	// Better eye-candy
						FF_DONTCARE|DEFAULT_PITCH,	// font family and pitch
						font);
						
	gl_view->make_current();
	list_base = glGenLists(96); // We support the 96 common printables
	h_oldfont = (HFONT)SelectObject(gl_view->get_device_context(), h_font);	// Save the old font pen
	wglUseFontBitmaps(gl_view->get_device_context(), 32, 96, list_base);	// Builds 96 Characters Starting At Character 32
	assert(glGetError() == GL_NO_ERROR);
	
	// restore the GDI pen
	SelectObject(gl_view->get_device_context(), h_oldfont);	// Select old font pen
	DeleteObject(h_font);			// Delete the new font
}

GLText::~GLText(void)
{
	gl_view->make_current();
	glDeleteLists(list_base, 96);
}

void GLText::draw_string(const char* format, ...)
{
	char* text = new char[512];
	GLboolean depth_test;
	va_list	ap;
	
	assert(format != NULL);
	
	va_start(ap, format);
    vsprintf(text, format, ap);
	va_end(ap);

	gl_view->make_current();
	// Save state of depth test
	depth_test = glIsEnabled(GL_DEPTH_TEST);
	if (depth_test == GL_TRUE) {
		glDisable(GL_DEPTH_TEST);
	}

	// Save list base state
	glPushAttrib(GL_LIST_BIT);
	glListBase(list_base - 32);
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();

	// Restore state of depth test
	if (depth_test == GL_TRUE) {
		glEnable(GL_DEPTH_TEST);
	}
	
	assert(glGetError() == GL_NO_ERROR);
	
	delete[] text;

}
	
void GLText::draw_string(int x, int y, const char* format, ...)
{
	char* text = new char[512];
	GLboolean depth_test;
	va_list	ap;
	
	assert(format != NULL);
	
	va_start(ap, format);
    vsprintf(text, format, ap);
	va_end(ap);
	
	gl_view->make_current();
	// Save state of depth test
	depth_test = glIsEnabled(GL_DEPTH_TEST);
	if (depth_test == GL_TRUE) {
		glDisable(GL_DEPTH_TEST);
	}
	
	// Save matrix mode
	glPushAttrib(GL_MATRIX_MODE);
	
	// Re-set projection matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	
	// Map to screen coordinates
	glOrtho(0, gl_view->width(),
			gl_view->height(), 0,
			1.0,-1.0);
	
	// Re-set modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	// Set drawing colour
	glColor3f(red, green, blue);
	// Set position
	glRasterPos2f(x, y);
	// Save list base state
	glPushAttrib(GL_LIST_BIT);
	glListBase(list_base - 32);
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();
	
	// Restore modelview matrix
	glPopMatrix();
	
	// Restore projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	
	// Restore matrix mode
	glPopAttrib();
	
	// Restore state of depth test
	if (depth_test == GL_TRUE) {
		glEnable(GL_DEPTH_TEST);
	}
	
	assert(glGetError() == GL_NO_ERROR);
	
	delete[] text;
}

void GLText::set_color(GLfloat red_arg, GLfloat green_arg, GLfloat blue_arg)
{
	red = red_arg;
	green = green_arg;
	blue = blue_arg;
	
	if (red < 0.0) red = 0.0;
	if (green < 0.0) green = 0.0;
	if (blue < 0.0) blue = 0.0;
	if (red > 1.0) red = 1.0;
	if (green > 1.0) green = 1.0;
	if (blue > 1.0) blue = 1.0;
}

void GLText::get_color(GLfloat* red_return, GLfloat* green_return, GLfloat* blue_return)
{
	if (red_return != NULL) *red_return = red;
	if (green_return != NULL) *green_return = green;
	if (blue_return != NULL) *blue_return = blue;
}

