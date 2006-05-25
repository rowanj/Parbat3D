#ifndef _IMAGE_TILE_SET_H
#define _IMAGE_TILE_SET_H

#include "ImageFile.h"
#include <vector>
#include <sys/types.h>

typedef struct tile_t {
	int index;
	int image_x;
	int image_y;
	char* data;
} tile, *tile_ptr;

class ImageTileSet
{
public:
	/* Class control */
	ImageTileSet(int level_of_detail, ImageFile* file, int tex_size_param, int cache_size);
	virtual ~ImageTileSet(void);
	
	/* Tile Data */
	char* get_tile_RGB(int x, int y, int band_R, int band_G, int band_B);
	int* get_pixel_values(int x, int y);
	
	int get_tex_size(void)		{return tex_size;}
	int get_columns(void)		{return columns;}
	int get_rows(void)			{return rows;}
	int get_tile_size(void)		{return tile_size;}
	int get_LOD_width(void)		{return LOD_width;}
	int get_LOD_height(void)	{return LOD_height;}
	
	void get_cache_stats(unsigned int* size_p,
						 unsigned int* fill_p,
						 unsigned int* hits_p,
						 unsigned int* misses_p);
	
private:
	/* Helper functions */
	int load_tile(int image_x, int image_y);
	void align_tile(void* tile, int tile_dimension,
					int data_width, int data_height);
	
	/* Tileset properties */
	int LOD;
	ImageFile* image_file;
	
	/* Cache */
	int cache_size;
	int cache_fill;
	int cache_hits;
	int cache_misses;
	
	/* Data properties */
	int image_width, image_height, num_bands, sample_size;
	int LOD_width, LOD_height;
	int columns, rows, tile_size;
	int last_column_width, last_row_height;
	int tex_size;

	/* This stores the actual tile information */
	vector<tile_ptr> tiles;
};

#endif
