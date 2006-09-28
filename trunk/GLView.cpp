#include "config.h"
#include "GLView.h"

#include <cassert>

// pfd structure aproximated from public domain code at nehe.gamedev.net
static	PIXELFORMATDESCRIPTOR gl_pfd={	// pfd Tells Windows How We Want Things To Be
		sizeof(PIXELFORMATDESCRIPTOR),	// Size Of This Pixel Format Descriptor
		1,								// Version Number
		PFD_DRAW_TO_WINDOW |			// Format Must Support Window
		PFD_SUPPORT_OPENGL |			// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,				// Must Support Double Buffering
		PFD_TYPE_RGBA,					// Request An RGBA Format
		16,								// Select Our Color Depth
		0, 0, 0, 0, 0, 0,				// Color Bits Ignored
		0,								// No Alpha Buffer
		0,								// Shift Bit Ignored
		0,								// No Accumulation Buffer
		0, 0, 0, 0,						// Accumulation Bits Ignored
		16,								// 16Bit Z-Buffer (Depth Buffer)
		0,								// No Stencil Buffer
		0,								// No Auxiliary Buffer
		PFD_MAIN_PLANE,					// Main Drawing Layer
		0,								// Reserved
		0, 0, 0							// Layer Masks Ignored
};

GLView::GLView(HWND hWindow)
{
	status = 0; // No error
	error_text = "No error.";
	GLuint pixel_format_index;
	
	device_context = NULL;
	rendering_context = NULL;
	
	if (hWindow != NULL) {
		window_handle = hWindow;
	} else {
		status = 1;
		error_text = "No window handle given.";
	}

	// Get hardware device context and error check
	device_context = GetDC(window_handle);
    assert(device_context != NULL);
	
	pixel_format_index = ChoosePixelFormat(device_context, &gl_pfd);
	assert(pixel_format_index != 0);
	
	SetPixelFormat(device_context, pixel_format_index, &gl_pfd);
	
    rendering_context = wglCreateContext(device_context);
    assert(rendering_context != NULL);

	
	if (!wglMakeCurrent(device_context, rendering_context)) {
		error_text = "Could not make rendering context current.";
	}
	
	this->resize();
}

GLView::~GLView()
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(rendering_context);
	ReleaseDC(window_handle, device_context);
}

/* Call on window re-size to adjust OpenGL context to fit */
void GLView::resize(void)
{
	GLuint width, height;
	LPRECT window_rect;
	window_rect = new RECT;
	GetWindowRect(window_handle, window_rect);
	width = window_rect->right - window_rect->left;
	height = window_rect->bottom - window_rect->top;
	delete window_rect; 
	
	if (height == 0) height = 1; // prevent div 0
	window_height = height;
	window_width = width;
	
	this->make_current();
	glViewport(0,0,width,height);
}

void GLView::make_current()
{
	assert(device_context != NULL);
	assert(rendering_context != NULL);
	wglMakeCurrent(device_context, rendering_context);
}

void GLView::swap()
{
	this->make_current();
	SwapBuffers(device_context);
}

float GLView::aspect()
{
	return (float)window_width / (float)window_height;
}

int GLView::height()
{
	return window_height;
}

int GLView::width()
{
	return window_width;
}
