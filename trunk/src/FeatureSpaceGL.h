#ifndef FEATURE_SPACE_GL_H
#define FEATURE_SPACE_GL_H

#include "GLText.h"
#include "GLView.h"
#include "PointsHash.h"

#define USE_POINT_SPRITES 1
#if USE_POINT_SPRITES
#include <GL/glext.h>
#endif

using namespace std;

class FeatureSpaceGL
{
public:
	FeatureSpaceGL(HWND hwnd_arg, int LOD_arg, int band1, int band2, int band3);
	void draw(void);
	void resize(void);
	
	void translate_cam(float x, float y);
	void dolly_cam(float diff);
	void rot_cam(float x_diff, float y_diff);
	void zoom_cam(float diff);
    
    int granularity;
        
   	static const float degs_to_rad = 180.0 / M_PI;
	static const float rads_to_deg = 1.0 / (180 / M_PI);

	void add_points(points_hash_t points_hash,
			unsigned char red, unsigned char green, unsigned char blue);
			
	void toggle_smooth(void);
#if USE_POINT_SPRITES
	void setup_point_sprites(void);
#endif

    
private:
	GLText* gl_text;
	GLView* gl_view;
		
	void make_box_list(void);
	void update_viewport(void);

	int LOD;
    unsigned int vertices;
    unsigned int num_points;
    GLuint list_box;
    GLuint list_line_square;
    vector<GLuint> points_lists;
#if USE_POINT_SPRITES
	GLuint points_texture_id;
	static const GLfloat point_sprite_max_size = 30.0;
#endif
    

    GLfloat cam_dolly;
    GLfloat z_rot;
    GLfloat pitch;
    GLfloat cam_xpan;
    GLfloat cam_ypan;
    
    bool smooth;
    
    int band1, band2, band3;
};

#endif
