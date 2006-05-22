#include "ImageTileSet.h"
#include "ImageProperties.h"

/* NB: tex_size is size of tile in memory,
	tile_size is size of tile on image */

ImageTileSet::ImageTileSet(int level_of_detail, ImageFile* file, int tex_size_param)
{
	ImageProperties* image_properties;
	
	tiles_allocated = 0;
	
	/* !! Needs to get actual sample size */
	sample_size = 1;
	
	LOD = level_of_detail;
	image_file = file;
	tex_size = tex_size_param;
	
	image_properties = image_file->getImageProperties();
	image_width = image_properties->getWidth();
	image_height = image_properties->getHeight();
	num_bands = image_properties->getNumBands();
	
	/* special case for overview window */
	if (LOD = -1) {
		tile_size = 1;
		image_width_LOD = tex_size;
		image_height_LOD = tex_size;
		tile_columns = 1;
		tile_rows = 1;
		last_column_width = image_width;
		last_row_height = image_height;
	} else {
		/* (width in tiles is width/tiles rounded up) */
		tile_size = tex_size * (2^LOD);
		image_width_LOD = image_width / (2^LOD);
		image_height_LOD = image_height / (2^LOD);
		tile_columns = (image_width/tile_size) + ((image_width%tile_size)!=0);
		tile_rows = (image_height/tile_size) + ((image_height%tile_size)!=0);
		last_column_width = tile_size;
		last_row_height = tile_size;
		if (image_width % tile_size) last_column_width = (image_width % tile_size);
		if (image_height % tile_size) last_row_height = (image_height % tile_size);
	}
	
	/* Allocate pointer list */
	tile_pointers = (void**) malloc(tile_columns * tile_rows * sizeof(void*));
}

ImageTileSet::~ImageTileSet(void)
{
	/* De-allocate tiles */
	while (!tiles.empty()) {
		free(tile_pointers[tiles[tiles_allocated-1]]);
		tile_pointers[tiles[tiles_allocated-1]] = NULL;
		tiles.pop_back();
		tiles_allocated--;
	}
	
	/* Free list of tile pointers */
	free (tile_pointers);
}

void ImageTileSet::set_region(int x, int y, int width, int height)
{
	/* Special case for overview window */
	if (LOD = -1) {
		/* Grab entire image to one tile */
		load_tile(0,0);
		return;
	}

	/* De-allocate tiles out of region */

	
	/* Validate dimensions and set region */
	if (x < image_width) {
		region_left = x;
	} else {
		region_left = image_width - 1;
	}
	
	if (y < image_height) {
		region_top = y;
	} else {
		region_top = image_height - 1;
	}
	
	if ((region_left + width) > image_width) {
		region_width = 1;
	} else {
		region_width = width;
	}
	if ((region_top + height) > image_height) {
		region_height = 1;
	} else {
		region_height = height;
	}
	
	
	/* Get any new tiles from ImageFile */
}

char* ImageTileSet::get_tile_RGB(int x, int y, int band_R, int band_G, int band_B)
{
	int tile_index;
	char* tile;
	/* Check if tile is loaded, load if not */
	tile_index = load_tile(x,y);
	
	/* Convert color space & sample size */
	
	/* return temporary tile */
	tile = (char*) tile_pointers[tile_index];
	return tile;
}

int ImageTileSet::load_tile(int x, int y)
{
	int tile_index, tile_index_x, tile_index_y;
	int tile_size_x, tile_size_y;
	
	/* Calculate tile index */
	tile_index_x = x/tile_size;
	tile_index_y = y/tile_size;
	tile_index = (tile_columns * tile_index_y) + tile_index_x;
	
	/* Load if not already */
	if (!tile_pointers[tile_index]) {
		/* Adjust for border tiles */
		if (tile_index_x == tile_columns-1) {
			tile_size_x = last_column_width;
		} else {
			tile_size_x = tile_size;
		}
		if (tile_index_y == tile_rows-1) {
			tile_size_y = last_row_height;
		} else {
			tile_size_y = tile_size;
		}
		/* Allocate tile */
		tile_pointers[tile_index] = (void*) malloc(tex_size * tex_size * num_bands * sample_size);

		/* Get tile data */
		/* !! shouldn't be cast to char* here */
		if (LOD!=-1) {
			image_file->getRasterData(tile_size_x, tile_size_y, x, y, (char*) tile_pointers[tile_index], tile_size_x, tile_size_y);
		} else {
			image_file->getRasterData(tile_size_x, tile_size_y, x, y, (char*) tile_pointers[tile_index], tex_size, tex_size);
		}
		/* Shuffle data for edge tiles */
	
		/* add index to list of allocated tiles */
		tiles.push_back(tile_index);
	
		/* Increment count of loaded tiles */
		tiles_allocated++;
	}
	return tile_index;
}

void* ImageTileSet::get_point_values(int x, int y)
{
	void* return_values;
	return_values = (void*) malloc(sample_size * num_bands);
	
	/* Find values in block */
	
	return return_values;
}
