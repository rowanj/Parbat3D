#include "ImageGLView.h"

ImageGLView::ImageGLView(HWND hWindow)
{
	// !! ToDo - Add error checking at various stages
	status = 0; // No error
	error_text = "No error.";
	GLuint pixel_format_index;
	
	// pfd structure aproximated from public domain code at nehe.gamedev.net
	static	PIXELFORMATDESCRIPTOR pfd={	// pfd Tells Windows How We Want Things To Be
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
	if(!(pixel_format_index = ChoosePixelFormat(device_context, &pfd))) {
		status = 3;
		error_text = "Could not find suitable pixel format.";
	}
	if(!SetPixelFormat(device_context, pixel_format_index, &pfd)) {
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

}

ImageGLView::~ImageGLView()
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(rendering_context);
	ReleaseDC(window_handle, device_context);
}

void ImageGLView::GLinit(void)
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
}

void ImageGLView::GLresize(void)
{
	GLuint width, height;
	LPRECT window_rect;
	window_rect = (tagRECT*) malloc(sizeof(RECT));
	GetWindowRect(window_handle, window_rect);
	width = window_rect->right - window_rect->left;
	height = window_rect->bottom - window_rect->top;
	if (height == 0) height = 1; // prevent div 0
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	free(window_rect);
}

void ImageGLView::make_current()
{
	wglMakeCurrent(device_context, rendering_context);
}

