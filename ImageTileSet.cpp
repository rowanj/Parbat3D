#include "ImageTileSet.h"
#include "ImageProperties.h"
#include <math.h>

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
	if (LOD == -1) {
		tile_size = tex_size;
		image_width_LOD = tex_size;
		image_height_LOD = tex_size;
		tile_columns = 1;
		tile_rows = 1;
		last_column_width = image_width;
		last_row_height = image_height;
	} else {
		/* (width in tiles is width/tiles rounded up) */
		tile_size = tex_size * (int)pow(2,LOD);
		image_width_LOD = image_width / (int)pow(2,LOD);
		image_height_LOD = image_height / (int)pow(2,LOD);
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
	/* De-allocate tiles out of region */
	/* Ensure pointers set to NULL for presence test */

	
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
	char* out_tile;
	int pix, piy, size;
	
	/* Check if tile is loaded, load if not */
	tile_index = load_tile(x,y);
	tile = (char*) tile_pointers[tile_index];

  	
	/* Make room to put texture in */
	size = tex_size * tex_size * 3;
	out_tile = (char*) malloc(size);

   MessageBox(0,"Where da bug? 5.3.3","ImageHander Constructor",MB_OK);
   
	/* Prevent bug on single channel images */
	if(band_R>num_bands) band_R = num_bands;
	if(band_G>num_bands) band_G = num_bands;
	if(band_B>num_bands) band_B = num_bands;
	
	/* Convert color space */
//	 MessageBox(0,inttocstring(band_R),"band_R",MB_OK);
//	 MessageBox(0,inttocstring(band_G),"band_G",MB_OK);	 
//	 MessageBox(0,inttocstring(band_B),"band_B",MB_OK);	 
//	 MessageBox(0,inttocstring(size),"size",MB_OK);	 	 
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

//   MessageBox(0,"Where da bug? 5.3.4","ImageHander Constructor",MB_OK);		
	
	/* return temporary tile */
	return out_tile;
}

int ImageTileSet::load_tile(int x, int y)
{
	int tile_index, tile_index_x, tile_index_y;
	int tile_size_x, tile_size_y;
	int pix, piy;
	
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
		
		/* Allocate tile and get tile data */
		tile_pointers[tile_index] = (void*) malloc(tex_size * tex_size * num_bands * sample_size);
		if (!tile_pointers[tile_index]) {
			MessageBox(0,"Failed allocating tile.","ImageTileSet::load_tile",MB_OK);
		}
                                        
		/* !! shouldn't be cast to char* here */
		if (LOD!=-1) {
			image_file->getRasterData(tile_size_x, tile_size_y, x, y, (char*) tile_pointers[tile_index], tile_size_x / (int)pow(2,LOD), tile_size_y / (int)pow(2,LOD));
		} else {
			image_file->getRasterData(tile_size_x, tile_size_y, x, y, (char*) tile_pointers[tile_index], tex_size, tex_size);
		}
		
		/* !! Shuffle data for edge tiles */
	
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

int ImageTileSet::get_tile_columns(void)
{
	return tile_columns;
}
int ImageTileSet::get_tile_rows(void)
{
	return tile_rows;
}
int ImageTileSet::get_tile_size(void)
{
	return tile_size;
}
int ImageTileSet::get_LOD_width(void)
{
	return image_width_LOD;
}
int ImageTileSet::get_LOD_height(void)
{
	return image_height_LOD;
}
