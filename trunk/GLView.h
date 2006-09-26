#ifndef _GL_VIEW_H
#define _GL_VIEW_H

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

class GLView
{
public:
	GLView(HWND hWindow);		/* Creates OpenGL rendering context */
	virtual ~GLView(void);		/* Deletes OpenGL rendering context
										and frees up resources */
	void make_current(void);		/* Selects this rendering context for
										use.  Only necessary if multiple
										instances of this class are used. */
	void GLresize(void);			/* Re-sizes OpenGL viewport to fit
										current size of the window */
	void GLswap(void);				/* Calls platform-dependant function
										to swap OpenGL buffers */
	
	GLfloat aspect(void);				/* Gets aspect of current viewport */
	int width(void);				/* Gets current width of viewport */
	int height(void);				/* Gets current height of viewport */
	
	int status;						/* Contains failure code if an error
										occurred on instantiation. If no
										error, value 0 is used */
	const char* error_text;			/* Pointer to textual description of
										any error, or "No error." */

	HDC get_device_context(void) {return device_context;};

private:
	/* State variables */
	int window_height, window_width;
	
	/* OpenGL handles */
	HWND window_handle;
	HDC device_context;
	HGLRC rendering_context;
};

#endif
