#include "ImageHandler.h"
//#include "TestUtils.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include <math.h>

#define PI 3.14159265

#include "config.h"
#include "console.h"

#if DEBUG_IMAGE_REDRAW
float redraw_rotz;
float dirty_rotz;
#endif

ImageHandler::ImageHandler(HWND overview_hwnd, HWND image_hwnd, char* filename)
{
	ImageProperties* image_properties;
	status = 0; // No error
	error_text = "No error.";
	image_tileset = NULL;

	Console::write("(II) ImageHandler instantiating.\n");
	
	/* Set up defaults */
	band_red = 1;
	band_green = 2;
	band_blue = 3;
	texture_size = 256; /* !! Could load from Settings */
	texture_size_overview = 256; /* !! Could load from Settings */
	viewport_x = 0;
	viewport_y = 0;
	
	// Check for lazily unspecified (NULL argument) parameters
	if (!overview_hwnd) {
		status = 1;
		error_text = "No handle for overview window.";
	}
	if (!image_hwnd) {
		status = 2;
		error_text = "No handle for main window.";
	}
	if (!filename) {
		status = 3;
		error_text = "No filename given.";
	}
	
	/* Initialize OpenGL*/
	gl_overview = new ImageGLView(overview_hwnd);
	if (!gl_overview) {
		status = 4;
		error_text = "Could not create ImageGLView for overview window.";
	} else {
		Console::write("(II) ImageHandler::Created ImageGLView for overview window.\n");
	}
	
	gl_image = new ImageGLView(image_hwnd);
	if (!gl_image) {
		status = 4;
		error_text = "Could not create ImageGLView for image window.";
	} else {
		Console::write("(II) ImageHandler::Created ImageGLView for image window.\n");
		viewport_width = gl_image->width();
		viewport_height = gl_image->height();
	}
		
	// Initialize image file (could be threaded)
	image_file = new ImageFile(filename);
	if (!image_file) {
		status = 5;
		error_text = "Could not create ImageFile object.";
	} else {
		image_properties = image_file->getImageProperties();
		image_height = image_properties->getHeight();
		image_width = image_properties->getWidth();
	}
	
	
	/* Find maximum supported texture size */
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*) &max_texture_size);
    Console::write("(II) ImageHandler::max_texture_size = ");
    Console::write(max_texture_size);
    Console::write("\n");
    
    
	this->GLinit();
   
    #if DEBUG_IMAGE_HANDLER
    Console::write("(II) ImageHandler::Creating tileset for overview\n");
    #endif
   	overview_tileset = new ImageTileSet(-1, image_file, texture_size_overview, 0);
	this->make_overview_texture();
    this->set_LOD(1);
	/* Initialize viewports */
	this->resize_window();
}

ImageHandler::~ImageHandler(void)
{
	Console::write("(II) ImageHandler shutting  down...\n");
	delete overview_tileset;
	delete image_tileset;
	delete[] tex_base;
	delete gl_overview;
	delete gl_image;
	delete image_file;
}

void ImageHandler::GLinit(void)
{
	#if DEBUG_OPENGL
	Console::write("(II) ImageHandler::GLinit - OpenGL Extensions found: ");
	Console::write((char*)glGetString(GL_EXTENSIONS));
	Console::write("\n");
	#endif
	/* Initialize OpenGL machine */
    gl_overview->make_current();
    glShadeModel(GL_FLAT);
    glDisable(GL_DEPTH_TEST);
    gl_image->make_current();
    glShadeModel(GL_FLAT);
    glDisable(GL_DEPTH_TEST);
    list_tile = glGenLists(1);
    glNewList(list_tile,GL_COMPILE);
    {
		glBegin(GL_QUADS);
		{
			glTexCoord2f(0.0,0.0);
			glVertex3f(0.0, 0.0,0.0);
			glTexCoord2f(1.0,0.0);
			glVertex3f(texture_size, 0.0,0.0);
			glTexCoord2f(1.0,1.0);
			glVertex3f(texture_size,-texture_size,0.0);
			glTexCoord2f(0.0,1.0);
			glVertex3f(0.0,-texture_size,0.0);
		}
		glEnd();
	}
	glEndList();
}

void ImageHandler::redraw(void)
{
	GLenum errorcode;
	int tile_id;
	
	// Overview window
	gl_overview->make_current();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, (GLuint) tex_overview_id);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/* Set up view transform */
	gluLookAt(0.0,0.0,(0.5/tan(PI/6.0)),0.0,0.0,0.0,0.0,1.0,0.0);

	/* Set up aspect transform */
	glPushMatrix(); // Use matrix copy so we only squash the overview polygon
	if (image_width >= image_height) {
		glScalef(1.0, (GLfloat)image_height/(GLfloat)image_width, 1.0);
	} else {
		glScalef((GLfloat)image_width/(GLfloat)image_height, 1.0, 1.0);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-0.5, 0.5, 0.0);
		glTexCoord2f(1.0, 0.0);
		glVertex3f( 0.5, 0.5, 0.0);
		glTexCoord2f(1.0, 1.0);
		glVertex3f( 0.5,-0.5, 0.0);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-0.5,-0.5, 0.0);
	glEnd();
	glPopMatrix(); // Restore model transform
	glDisable(GL_TEXTURE_2D);
	
#if DEBUG_IMAGE_REDRAW
	/* draw rotating line to visualize redraw frequency */
	glRotatef(redraw_rotz, 0.0,0.0,1.0);
	redraw_rotz-=1.0;
	if(redraw_rotz < -360.0) redraw_rotz+=360.0;
	glBegin(GL_LINES);
		glColor3f(1.0,1,0);
		glVertex3f(0.0,0.0,0.0);
		glVertex3f(0.0,0.25,0.0);
	glEnd();
#endif
	gl_overview->GLswap();
	
	/* On to the main window */
	gl_image->make_current();
	glClearColor(0.3f, 0.1f, 0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/* Set up view transform */
	gluLookAt(0.0,0.0,(1.0/tan(PI/6.0)),0.0,0.0,0.0,0.0,1.0,0.0);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, tex_base[0]);
	glCallList(list_tile);
	glTranslatef((GLfloat)texture_size, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, tex_base[1]);
	glCallList(list_tile);
	glDisable(GL_TEXTURE_2D);
    #if FALSE
    glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	tile_id = 0;
	glTranslatef((GLfloat)-tex_columns/2.0,(GLfloat)tex_rows/2.0, 0.0);
	while (tile_id < tex_count) {
		glBindTexture(GL_TEXTURE_2D, (GLuint) tex_base[tile_id]);
		glCallList(list_tile);
		if (!(tile_id % tex_columns)) {
			glTranslatef((GLfloat)-tex_columns, -1.0, 0.0);
		} else {
			glTranslatef(1.0,0.0,0.0);
		}
		tile_id++;
	}
	glDisable(GL_TEXTURE_2D);
	#endif
	gl_image->GLswap();
}

ImageProperties* ImageHandler::get_image_properties(void)
{
	return image_file->getImageProperties();
}

BandInfo* ImageHandler::get_band_info(int band_number)
{
	return image_file->getBandInfo(band_number);
}

void ImageHandler::resize_window(void)
{
	#if DEBUG_IMAGE_HANDLER
    Console::write("(II) ImageHandler::Window resize triggered.\n");
    #endif
	gl_overview->GLresize();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(60.0, (GLfloat) gl_overview->width()/(GLfloat) gl_overview->height(), 0.1, 2.0);
	
	gl_image->GLresize();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(60.0, (GLfloat) gl_image->width()/(GLfloat) gl_image->height(), 0.1, 20.0);
	viewport_width = gl_image->width();
	viewport_height = gl_image->height();
	glOrtho(0.0, viewport_width, -viewport_height, 0.0, 0.1, 10.0);
	this->redraw();
}

int* ImageHandler::get_pixel_values(int x, int y)
{
	if (image_tileset) {
		return image_tileset->get_pixel_values(x,y);
	} else {
		return NULL;
	}
}

int* ImageHandler::get_pixel_values_viewport(int viewport_x_pos, int viewport_y_pos)
{
	if (image_tileset) {
		return image_tileset->get_pixel_values_LOD(viewport_x_pos + viewport_x, viewport_y_pos + viewport_x);
	} else {
		return NULL;
	}
}

void ImageHandler::get_geo_pos(geo_coords_ptr pos)
{
	Console::write("(WW) ImageHandler::get_geo_pos() - not implemented.\n");
}

const char* ImageHandler::get_info_string(void)
{
    return image_file->getInfoString();
}    

void ImageHandler::make_overview_texture(void)
{
	#if DEBUG_IMAGE_HANDLER
	Console::write("(II) ImageHandler::make_overview_texture()\n");
	#endif
	// Get texture data	
	tex_overview = overview_tileset->get_tile_RGB(0, 0, band_red, band_green, band_blue);
	#if DEBUG_IMAGE_HANDLER
	Console::write("(II) ImageHandler::Successfully got overview texture from tileset.\n");
	#endif

	/* Make texture from data */
	gl_overview->make_current();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if (!tex_overview_id) glGenTextures(1, &tex_overview_id);
	glBindTexture(GL_TEXTURE_2D, (GLuint) tex_overview_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_size_overview, texture_size_overview, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_overview);
	/* We don't need the RGB data here anymore */
	delete(tex_overview);
	#if DEBUG_IMAGE_HANDLER
	Console::write("(II) ImageHandler::Successfully created overview texture.\n");
	#endif
}

void ImageHandler::make_textures(void)
{
	unsigned int tmp_id;
	int tx, ty;
	char *tmp_tex;
	
	Console::write("(II) ImageHandler::Making image textures - LOD=");
	Console::write(LOD);
	Console::write("\n");
	
	if (!image_tileset) {
		Console::write("(II) ImageHandler:: Creating prototype image tileset.\n");
		image_tileset = new ImageTileSet(LOD, image_file, texture_size, 32 * 1024 * 1024);
	}
	tex_columns = 2;
	tex_rows = 1;
	tile_size = image_tileset->get_tile_size();
	tex_count = tex_columns * tex_rows;
	gl_image->make_current();
	tex_base = new GLuint[tex_count];
	glGenTextures(tex_count, tex_base);
	for (ty = 0; ty < tex_rows; ty++){
		for (tx = 0; tx < tex_columns; tx++) {
			tmp_id = (ty * tex_columns) + tx;
			tmp_tex = image_tileset->get_tile_RGB(tile_size * tx + 1, tile_size * ty + 1, band_red, band_green, band_blue);
			glBindTexture(GL_TEXTURE_2D, tex_base[tmp_id]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_size, texture_size, 0, GL_RGB, GL_UNSIGNED_BYTE, tmp_tex);
			delete[] tmp_tex;
		}
	}
}

void ImageHandler::set_bands(int band_R, int band_G, int band_B)
{
	#if DEBUG_IMAGE_HANDLER
	char buffer[256];
	sprintf(buffer, "(II) ImageHandler::set_bands(%d,%d,%d)\n",band_R,band_G,band_B);
	Console::write(buffer);
	#endif
	band_red = band_R;
	band_green = band_G;
	band_blue = band_B;
	make_overview_texture();
	make_textures();
	redraw();
}

int ImageHandler::get_LOD(void) {return LOD;}
int ImageHandler::set_LOD(int level_of_detail)
{
	#if DEBUG_IMAGE_HANDLER
	char  buffer[256];
	sprintf(buffer, "(II) ImageHandler::set_LOD(%d) - Current: %d\n", level_of_detail, LOD);
	Console::write(buffer);
	#endif
	
	if (!image_tileset) {
		Console::write("(II) Making initial tileset.\n");
		LOD = level_of_detail;
		make_textures();
		redraw();
	}
	
	if (LOD!=level_of_detail) {
		if (image_tileset) {
			#if DEBUG_IMAGE_HANDLER
			Console::write("(II) ImageHandler::Deleting image_tileset;");
			#endif
			delete(image_tileset);
			image_tileset = NULL;
		}
		LOD = level_of_detail;
		make_textures();
		redraw();
	}
}

int ImageHandler::get_LOD_width(void) {return image_tileset->get_LOD_width();}
int ImageHandler::get_LOD_height(void) {return image_tileset->get_LOD_height();}
int ImageHandler::get_viewport_width(void) {return viewport_width;}
int ImageHandler::get_viewport_height(void) {return viewport_height;}
int ImageHandler::get_viewport_x(void) {return viewport_x;}
int ImageHandler::get_viewport_y(void) {return viewport_y;}
