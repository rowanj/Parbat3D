#include "ImageHandler.h"
//#include "TestUtils.h"
#include <gl/gl.h>
#include <gl/glu.h>
//#include <math.h>
#include "MathUtils.h"

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
	tex_base_size = 0;
	tex_base = NULL;
	LOD = 1;
	
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

	/* Initialize viewports */
	this->resize_window();
	this->set_viewport(0,0);
//    this->set_LOD(1);
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
	int tile_x, tile_y;
	
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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
   	glOrtho(viewport_x, viewport_x + viewport_width,
	   		-(viewport_y + viewport_height),-viewport_y,
			0.1, 10.0);
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/* Set up view transform */
	gluLookAt(0.0,0.0,(1.0/tan(PI/6.0)),0.0,0.0,0.0,0.0,1.0,0.0);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
	#if TRUE
	/* Draw all valid textures */
	glTranslatef((GLfloat)texture_size * (GLfloat)start_column,
				 -(GLfloat)texture_size * (GLfloat)start_row, 0.0);
	for (tile_y = 0; tile_y < viewport_rows; tile_y++) {
		glPushMatrix();
		for (tile_x = 0; tile_x < viewport_columns; tile_x++) {
			tile_id = tile_x + (tile_y * viewport_columns);
			if ((tile_id < tex_base_size) &&
					((tile_x + start_column) < image_columns) &&
					((tile_y + start_row) < image_rows)) {
				glBindTexture(GL_TEXTURE_2D, tex_base[tile_id]);
				glCallList(list_tile);
				glTranslatef((GLfloat)texture_size, 0.0, 0.0);
			}
		}
		glPopMatrix();
		glTranslatef(0.0, -(GLfloat)texture_size, 0.0);
	}
	#else
	/* Statically draw first four tiles */
	if (tex_base_size) {
	glBindTexture(GL_TEXTURE_2D, tex_base[0]);
	glCallList(list_tile);
	glTranslatef((GLfloat)texture_size, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, tex_base[1]);
	glCallList(list_tile);
	glTranslatef(-(GLfloat)texture_size, -(GLfloat)texture_size, 0.0);
	glBindTexture(GL_TEXTURE_2D, tex_base[2]);
	glCallList(list_tile);
	glTranslatef((GLfloat)texture_size, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, tex_base[3]);
	glCallList(list_tile);
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
	viewport_width = gl_image->width();
	viewport_height = gl_image->height();
	viewport_columns = (viewport_width / texture_size) + ((viewport_width % texture_size)!=0) + 1;
	viewport_rows = (viewport_height / texture_size) + ((viewport_height % texture_size)!=0) + 1;
	this->make_textures();
	this->redraw();
}

unsigned int* ImageHandler::get_pixel_values(int x, int y)
{
	if (image_tileset) {
		return image_tileset->get_pixel_values(x,y);
	} else {
		return NULL;
	}
}

unsigned int* ImageHandler::get_pixel_values_viewport(int viewport_x_pos, int viewport_y_pos)
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
	
	/* !! remove duplicates of this */
	if (!image_tileset) {
		image_tileset = new ImageTileSet(LOD, image_file, texture_size, 96 * 1024 * 1024);
		image_columns = image_tileset->get_columns();
		image_rows = image_tileset->get_rows();
	}
	
	if (image_tileset->get_LOD() != LOD) {
		delete image_tileset;
		image_tileset = new ImageTileSet(LOD, image_file, texture_size, 96 * 1024 * 1024);
		image_columns = image_tileset->get_columns();
		image_rows = image_tileset->get_rows();
	}
		
	tile_size = image_tileset->get_tile_size();
	tex_count = viewport_columns * viewport_rows;
	gl_image->make_current();

	/* If textures were allocated, delete them */
	if (tex_base_size) {
		glDeleteTextures(tex_base_size, tex_base);
		delete[] tex_base;
	}

	tex_base = new GLuint[tex_count];
	glGenTextures(tex_count, tex_base);
	tex_base_size = tex_count;

	for (ty = 0; ty < viewport_rows; ty++){
		for (tx = 0; tx < viewport_columns; tx++) {
			tmp_id = (ty * viewport_columns) + tx;
			if (((tx+start_column) < image_columns) && ((ty+start_row) < image_rows)) {
				tmp_tex = image_tileset->get_tile_RGB(tile_size * (tx + start_column), tile_size * (ty+start_row), band_red, band_green, band_blue);
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
	
	int previous_center_x, previous_center_y;
	
	/* In old-LOD coords*/
	previous_center_x = viewport_x + (viewport_width/2);
	previous_center_y = viewport_y + (viewport_height/2);
	
	if (level_of_detail < LOD) {
		viewport_x = ((previous_center_x * MathUtils::ipow(2,LOD - level_of_detail))-(viewport_width/2));
		viewport_y = ((previous_center_y * MathUtils::ipow(2,LOD - level_of_detail))-(viewport_height/2));
	} else {
		viewport_x = ((previous_center_x / MathUtils::ipow(2,level_of_detail-LOD))-(viewport_width/2));
		viewport_y = ((previous_center_y / MathUtils::ipow(2,level_of_detail-LOD))-(viewport_height/2));
	}
	/* Set up as in set_viewport */
	start_column = viewport_x / texture_size;
	start_row = viewport_y / texture_size;
	if (start_column < 0) start_column = 0;
	if (start_row < 0) start_row = 0;
	LOD = level_of_detail;
	make_textures();
	redraw();
}

void ImageHandler::set_viewport(int x, int y)
{
	viewport_x = x;
	viewport_y = y;
	start_column = x / texture_size;
	start_row = y / texture_size;
	if (start_column < 0) start_column = 0;
	if (start_row < 0) start_row = 0;
	make_textures();
	redraw();
}
void ImageHandler::set_viewport_x(int x) {set_viewport(x, viewport_y);}
void ImageHandler::set_viewport_y(int y) {set_viewport(viewport_x, y);}
int ImageHandler::get_LOD_width(void) {return image_tileset->get_LOD_width();}
int ImageHandler::get_LOD_height(void) {return image_tileset->get_LOD_height();}
int ImageHandler::get_viewport_width(void) {return viewport_width;}
int ImageHandler::get_viewport_height(void) {return viewport_height;}
int ImageHandler::get_viewport_x(void) {return viewport_x;}
int ImageHandler::get_viewport_y(void) {return viewport_y;}
