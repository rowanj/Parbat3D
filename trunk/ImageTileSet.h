#ifndef _IMAGE_TILE_SET_H
#define _IMAGE_TILE_SET_H

#include "ImageFile.h"
#include <vector>
#include <sys/types.h>

typedef struct tile_t {
	int tile_index;
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
	unsigned int* get_pixel_values(int x, int y);
	
	int get_tex_size(void);
	int get_columns(void);
	int get_rows(void);
	int get_tile_size(void);
	int get_LOD(void);
	int get_LOD_width(void);
	int get_LOD_height(void);
	
	void get_cache_stats(unsigned int* size_p,
						 unsigned int* fill_p,
						 unsigned int* hits_p,
						 unsigned int* misses_p);
	
private:
	/* Helper functions */
	int load_tile(int image_x, int image_y);
	void align_tile(char** tile, int tile_dimension,
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
	int tex_size, tile_memory_size;

	/* This stores the actual tile information */
	vector<tile_ptr> tiles;
};

#endif
