#ifndef FEATURE_SPACE_GL_H
#define FEATURE_SPACE_GL_H

#include <math.h>
#include "GLText.h"
#include "GLView.h"
#include "PointsHash.h"

class FeatureSpaceGL
{
public:
	FeatureSpaceGL(HWND hwnd_arg, int LOD_arg, int band1, int band2, int band3);
	void draw(void);
	void resize(void);
	void cam_translate(float x, float y);
		
    GLfloat cam_yaw;
    GLfloat cam_pitch;
    GLfloat cam_dolly;
    
    int granularity;
    int num_points;
        
   	static const float degs_to_rad = 180.0 / M_PI;
	static const float rads_to_deg = 1.0 / (180 / M_PI);

	void make_points_lists(points_hash_t points_hash, int maxvalue);

private:
	GLText* gl_text;
	GLView* gl_view;
		
	void make_box_list(void);

	int LOD;
    int	num_points_lists;
    int vertices;
    GLuint list_box;
    GLuint list_points_base;
    
    int band1, band2, band3;
};

#endif
