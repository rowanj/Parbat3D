#ifndef _IMAGE_TILE_SET_H
#define _IMAGE_TILE_SET_H

#include "ImageFile.h"
#include <vector>


class ImageTileSet
{
public:
	/* Class control */
	ImageTileSet(int level_of_detail, ImageFile* file, int tex_size_param);
	virtual ~ImageTileSet(void);
	
	/* Viewport control */
	void set_region(int x, int y, int width, int height);
	
	/* Tile Data */
	char* get_tile_RGB(int x, int y, int band_R, int band_G, int band_B);
	void* get_point_values(int x, int y);
	
	int get_tile_columns(void);
	int get_tile_rows(void);
	int get_tile_size(void);
	int get_LOD_width(void);
	int get_LOD_height(void);
	
private:
	/* Tileset properties */
	int LOD;
	ImageFile* image_file;
	
	/* Data properties */
	int image_width, image_height, num_bands;
	int image_width_LOD, image_height_LOD;
	int tile_columns, tile_rows, tile_size;
	int last_column_width, last_row_height;
	int tex_size;
	int region_left, region_top, region_width, region_height;
	int sample_size;

	/* Helper functions */
	int load_tile(int x, int y);

	/* This stores the actual tile information */
	void **tile_pointers; /* Chunk of pointers to tile data */
	vector<int> tiles; /* list of indexes of pointers actually allocated */
	int tiles_allocated; /* Number of tiles still held in memmory */
};

#endif
