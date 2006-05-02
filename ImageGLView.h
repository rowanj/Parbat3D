class ImageGLView
{
public:
	ImageGLView(HWND hwnd);
	virtual ~ImageGLView(void);
	void select();
private:
	HWND window_handle;
	HDC device_context;
	HGLRC rendering_context;
}
