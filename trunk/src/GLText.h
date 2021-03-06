/*
GLText
Author: Rowan James <rowanjames@users.sourceforge.net>

Simple C++ class for nicely drawing text into an OpenGL viewport.

Known issues:
* Coordinates not accurate in case of viewports that don't occupy either
	the full viewport, or aren't flush against bottom edge.  Unsure why.
* Currently supports Windows only.  Font-loading is platform specific.

*/

#ifndef GL_TEXT_H
#define GL_TEXT_H

#include "GLView.h"

class GLText {
public:
	GLText(GLView* gl_view_arg, const char* font_arg, int size_arg);
	virtual ~GLText(void);
	
	void draw_string(const char* format, ...);
	void draw_string(int x, int y, const char* format, ...);
	
	void set_color(GLfloat red_arg, GLfloat green_arg, GLfloat blue_arg);
	void get_color(GLfloat* red_return, GLfloat* green_return, GLfloat* blue_return);
	
private:
	GLView* gl_view;
	const char* font;
	int size;
	GLuint list_base;
	GLfloat red, green, blue;
};

#endif
