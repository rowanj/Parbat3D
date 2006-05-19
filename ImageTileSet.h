#ifndef _IMAGE_TILE_SET_H
#define _IMAGE_TILE_SET_H

#include "ImageFile.h"
#include <vector>

typedef struct image_band_tile_t {
	int width;
	int height;
	char* values;
} image_band_tile, *image_band_tile_ptr;

typedef struct image_tile_t {
	int number_bands;
	int width;
	int height;
	image_band_tile_ptr bands[];
} image_tile, *image_tile_ptr;


class ImageTileSet
{
public:
	/* Class control */
	ImageTileSet(int LOD, ImageFile* file, int tex_size);
	virtual ~ImageTileSet(void);
	
	/* Viewport control */
	void set_region(int x, int y, int width, int width);
	
	/* Tile Data */
	char* get_tile_RGB(int x, int y, int band_R, int band_G, int band_B);
	int* get_point_values(int x, int y);
	
private:
	int LOD;
	int image_width, image_height, num_bands;
	int tex_size;
	RECT current_region;
	vector<image_tile_ptr> tiles;
};

#endif
