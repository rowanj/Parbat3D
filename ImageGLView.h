#ifndef _IMAGE_GL_VIEW_H
#define _IMAGE_GL_VIEW_H

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

class ImageGLView
{
public:
	ImageGLView(HWND hWindow);
	virtual ~ImageGLView(void);
	void make_current(void);
	void GLresize(void);
	void GLswap(void);
	
	int width(void);
	int height(void);
	
	int status;
	const char* error_text;

private:
	int window_height, window_width;
	HWND window_handle;
	HDC device_context;
	HGLRC rendering_context;
};

#endif
