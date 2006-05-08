#ifndef _IMAGE_GL_VIEW_H
#define _IMAGE_GL_VIEW_H

#include <windows.h>
#include <gl/gl.h>

class ImageGLView
{
public:
	ImageGLView(HWND hWindow);
	virtual ~ImageGLView(void);
	void make_current(void);
	
	int status;
	const char* error_text;

private:
	HWND window_handle;
	HDC device_context;
	HGLRC rendering_context;
};

#endif
