#include "config.h"
#include "ImageGLView.h"

// pfd structure aproximated from public domain code at nehe.gamedev.net
static	PIXELFORMATDESCRIPTOR gl_pfd={	// pfd Tells Windows How We Want Things To Be
		sizeof(PIXELFORMATDESCRIPTOR),	// Size Of This Pixel Format Descriptor
		1,								// Version Number
		PFD_DRAW_TO_WINDOW |			// Format Must Support Window
		PFD_SUPPORT_OPENGL |			// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,				// Must Support Double Buffering
		PFD_TYPE_RGBA,					// Request An RGBA Format
		24,								// Select Our Color Depth
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

ImageGLView::ImageGLView(HWND hWindow)
{
	status = 0; // No error
	error_text = "No error.";
	GLuint pixel_format_index;
	
	
	if (hWindow != NULL) {
		window_handle = hWindow;
	} else {
		status = 1;
		error_text = "No window handle given.";
	}

	// Get hardware device context and error check
	if (!(device_context = GetDC(window_handle))) {
		status = 2;
		error_text = "Could not get device context.";
	}
	
	
	// Find and set the pixel format
	if(!(pixel_format_index = ChoosePixelFormat(device_context, &gl_pfd))) {
		status = 3;
		error_text = "Could not find suitable pixel format.";
	}
	if(!SetPixelFormat(device_context, pixel_format_index, &gl_pfd)) {
		status = 4;
		error_text = "Could not set pixel format.";
	}
	
	if (!(rendering_context = wglCreateContext(device_context))) {
		status = 5;
		error_text = "Could not create rendering context.";
	}
	
	if (!wglMakeCurrent(device_context, rendering_context)) {
		status = 6;
		error_text = "Could not make rendering context current.";
	}
	
	this->GLresize();
}

ImageGLView::~ImageGLView()
{
	#if DEBUG_IMAGE_HANDLER
	MessageBox (NULL, "Destroying ImageGLView.", "Parbat3D :: ImageHandler", 0);
	#endif
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(rendering_context);
	ReleaseDC(window_handle, device_context);
}

void ImageGLView::GLresize(void)
{
	GLuint width, height;
	LPRECT window_rect;
	window_rect = (tagRECT*) malloc(sizeof(RECT));
	GetWindowRect(window_handle, window_rect);
	width = window_rect->right - window_rect->left;
	height = window_rect->bottom - window_rect->top;
	free(window_rect); 
	
	if (height == 0) height = 1; // prevent div 0
	window_height = height;
	window_width = width;
	
	this->make_current();
	glViewport(0,0,width,height);
}

void ImageGLView::make_current()
{
	wglMakeCurrent(device_context, rendering_context);
}

void ImageGLView::GLswap()
{
	SwapBuffers(device_context);
}

int ImageGLView::height()
{
	return window_height;
}

int ImageGLView::width()
{
	return window_width;
}

