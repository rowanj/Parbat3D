#ifndef GL_TEXT_H
#define GL_TEXT_H

#include "GLView.h"

#include <stdarg.h>

class GLText {
public:
	GLText(GLView* gl_view_arg, const char* font_arg, int size_arg);
	virtual ~GLText(void);
	
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
