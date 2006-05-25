#include "ImageTileSet.h"
#include "ImageProperties.h"
#include "console.h"
#include "config.h"
#include <math.h>

/* NB: tex_size is size of tile in memory,
	tile_size is size of tile on image */

ImageTileSet::ImageTileSet(int level_of_detail, ImageFile* file, int tex_size_param, int cache_size_param)
{
	char buffer[512];
	sprintf(buffer, "(II) ImageTileSet(%d, <imagefile>, %d, %d) Instantiating.\n", level_of_detail, tex_size_param, cache_size_param);
	Console::write(buffer);
	/* Local Variables */
	ImageProperties* image_properties;
	
	/* set instance variables */
	LOD = level_of_detail;
	image_file = file;
	tex_size = tex_size_param;
	cache_size = cache_size_param;
	
	/* initialize state */
	cache_fill = 0;
	cache_hits = 0;
	cache_misses = 0;
	
	/* !! Needs to get actual sample size from ImageFile */
	sample_size = 1;
	
	/* Grab a handle to the image properties object */
	image_properties = image_file->getImageProperties();
	/* Fill image properties */
	image_width = image_properties->getWidth();
	image_height = image_properties->getHeight();
	num_bands = image_properties->getNumBands();
	
	/* special case for overview window */
	if (LOD == -1) {
		tile_size = 1;
		LOD_width = tex_size;
		LOD_height = tex_size;
		columns = 1;
		rows = 1;
		last_column_width = image_width;
		last_row_height = image_height;
	} else {
		/* (width in tiles is width/tiles rounded up) */
		tile_size = tex_size * (int)pow(2,LOD);
		LOD_width = image_width / (int)pow(2,LOD);
		LOD_height = image_height / (int)pow(2,LOD);
		columns = (image_width/tile_size) + ((image_width%tile_size)!=0);
		rows = (image_height/tile_size) + ((image_height%tile_size)!=0);
		last_column_width = tile_size;
		last_row_height = tile_size;
		if (image_width % tile_size) last_column_width = (image_width % tile_size);
		if (image_height % tile_size) last_row_height = (image_height % tile_size);
	}
	
	/* Allocate pointer list */
}

ImageTileSet::~ImageTileSet(void)
{
	char buffer[256];
	sprintf(buffer, "(II) ImageTileset (LOD:%d) de-allocating.\n", LOD);
	Console::write(buffer);
	/* De-allocate tiles */
	while (!tiles.empty()) {
		delete[] tiles.back()->data;
		delete tiles.back();
		tiles.pop_back();
	}
}

char* ImageTileSet::get_tile_RGB(int x, int y, int band_R, int band_G, int band_B)
{
	int tile_index;
	char* tile;
	char* out_tile;
	int pix, piy, size;
	
	#if DEBUG_IMAGE_TILESET
	Console::write("(II) ImageTileset::get_tile_RGB\n");
	#endif
	/* Check if tile is loaded, load if not */
	tile_index = load_tile(x,y);
	#if DEBUG_IMAGE_TILESET
	Console::write("(II) ImageTileset::get_tile_RGB - Tile loaded.\n");
	#endif
	tile = tiles[tile_index]->data;

	/* Make room to put texture in */
	size = tex_size * tex_size * 3;
	out_tile = (char*) malloc(size);

	/* Prevent bug on single channel images */
	if(band_R>num_bands) band_R = num_bands;
	if(band_G>num_bands) band_G = num_bands;
	if(band_B>num_bands) band_B = num_bands;
	
	/* Convert color space */
 
	/* Convert band packing */
	pix = 0;
	piy = 0;
	band_R--;
	band_G--;
	band_B--;
	while (pix < size) {
		out_tile[pix]   = tile[piy+band_R];
		out_tile[pix+1] = tile[piy+band_G];
		out_tile[pix+2] = tile[piy+band_B];
		pix = pix + 3;
		piy = piy + num_bands;
	}

	/* return temporary tile */
	return out_tile;
}

int ImageTileSet::load_tile(int x, int y)
{
	#if DEBUG_IMAGE_TILESET
	char buffer[256];
	sprintf(buffer, "(II) ImageTileSet::load_tile(%d,%d)\n", x, y);
	Console::write(buffer);
	#endif
	tile_ptr new_tile;
	int tile_index, tile_index_x, tile_index_y;
	int tile_size_x, tile_size_y;
	int p_in, p_out, p_length;
	
	/* Calculate tile index */
	tile_index_x = x/tile_size;
	tile_index_y = y/tile_size;
	tile_index = (columns * tile_index_y) + tile_index_x;
	#if DEBUG_IMAGE_TILESET
	sprintf(buffer, "(II) ImageTileSet::load_tile INDEX:%d (x_index %d, y_index %d)\n", tile_index, tile_index_x, tile_index_y);
	Console::write(buffer);
	#endif
	
	/* Load if not already */
	if(false) {
		return 0; /* !! index of tile in vector */
	} else {
		/* Check cache space, deallocate head if not */
		/* Allocate tile */
		new_tile = new tile;
		new_tile->index = tile_index;
		new_tile->image_x = x;
		new_tile->image_y = y;
		new_tile->data = new char[tex_size * tex_size * sample_size * num_bands];
		
		/* Adjust for border tiles */
		if (tile_index_x == columns-1) {
			tile_size_x = last_column_width;
		} else {
			tile_size_x = tile_size;
		}
		if (tile_index_y == rows-1) {
			tile_size_y = last_row_height;
		} else {
			tile_size_y = tile_size;
		}
                                        
		if (LOD!=-1) {
			image_file->getRasterData(tile_size_x, tile_size_y, x, y, new_tile->data, tile_size_x / (int)pow(2,LOD), tile_size_y / (int)pow(2,LOD));
		} else {
			image_file->getRasterData(tile_size_x, tile_size_y, x, y, new_tile->data, tex_size, tex_size);
		}
		
		/* !! Shuffle data for edge tiles */
		
		
		/* add index to list of allocated tiles */
		tiles.push_back(new_tile);
		
		/* !! update cache */
		return tiles.size() - 1;
	}
}

int* ImageTileSet::get_pixel_values(int x, int y)
{
	int* return_values = new int[num_bands];

	int tmp;
	for (tmp = 0; tmp < num_bands; tmp++) {
		return_values[tmp] = tmp * 2;
	}
	/* !! Find values in block */
	
	
	return return_values;
}

void ImageTileSet::align_tile(void* tile, int tile_dimension,
					int data_width, int data_height)
{
	/* !! Insert Dave's code here */
}

