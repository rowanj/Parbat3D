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
	
void GLText::draw_string(int x, int y, const char* format, ...)
{
	char* text = new char[512];
	va_list	ap;
	
	assert(format != NULL);
	
	va_start(ap, format);
    vsprintf(text, format, ap);
	va_end(ap);
	
	gl_view->make_current();
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(0.2, 0.2);
	glPushAttrib(GL_LIST_BIT);
	glListBase(list_base - 32);
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();
	
	delete[] text;
}
