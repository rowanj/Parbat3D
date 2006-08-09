#include "ImageTileSet.h"
#include "ImageProperties.h"
#include "console.h"
#include "config.h"
#include <math.h>
#include "MathUtils.h"

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
		tile_memory_size = tex_size * tex_size * sample_size * num_bands;
	
	/* special case for overview window */
	if (LOD == -1) {
		columns = 1;
		rows = 1;
		last_column_width = image_width;
		last_row_height = image_height;
		while ( (last_column_width > tex_size) || (last_row_height > tex_size) ) {
			last_column_width /= 2;
			last_row_height /= 2;       
		}
		LOD_width = last_column_width;
		LOD_height = last_row_height;
		tile_size = tex_size; /* must be non-zero */
	} else {
		/* (width in tiles is width/tiles rounded up) */
		int LOD_temp = MathUtils::ipow(2,LOD);
		tile_size = tex_size * LOD_temp;
		LOD_width = image_width / LOD_temp;
		LOD_height = image_height / LOD_temp;
		columns = (image_width/tile_size) + ((image_width%tile_size)!=0);
		rows = (image_height/tile_size) + ((image_height%tile_size)!=0);
		last_column_width = tile_size;
		last_row_height = tile_size;
		if (image_width % tile_size) last_column_width = (image_width % tile_size);
		if (image_height % tile_size) last_row_height = (image_height % tile_size);
	}
	sprintf(buffer, "(II) LOD_width = %d, LOD_height = %d\n", LOD_width, LOD_height);
	Console::write(buffer);
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
	char message[256];
	int pix, piy, size;
	
	/* Make room to put texture in */
	size = tex_size * tex_size * 3;
	out_tile = new char[size];
	#if DEBUG_IMAGE_TILESET
	Console::write("(II) ImageTileset::get_tile_RGB\n");
	#endif
	/* Check if tile is loaded, load if not */
	
	#if DEBUG_IMAGE_TILESET
	sprintf(message, "(II) ImageTileset::get_tile_RGB - tile cache: hits=%d, misses=%d.\n", cache_hits, cache_misses);
	Console::write(message);
	#endif
	tile_index = this->load_tile(x,y);
	tile = tiles[tile_index]->data;

	

	/* Prevent bug on single channel images */
	if(band_R>num_bands) band_R = num_bands;
	if(band_G>num_bands) band_G = num_bands;
	if(band_B>num_bands) band_B = num_bands;
	
	/* Convert color space */
 
	/* Convert band packing */
	pix = 0;
	piy = 0;
	if (band_R && band_G && band_B) {
		/* Optimized loop for usual case */
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
	} else {
		/* Conditional loop for 'none' case.*/
		while (pix < size) {
			if (band_R) {
				out_tile[pix]   = tile[piy+band_R-1];
			} else {
				out_tile[pix]   = 0;
			}
			if (band_G) {
				out_tile[pix+1] = tile[piy+band_G-1];
			} else {
				out_tile[pix+1] = 0;
			}
			if (band_B) {
				out_tile[pix+2] = tile[piy+band_B-1];
			} else {
				out_tile[pix+2] = 0;
			}
			pix = pix + 3;
			piy = piy + num_bands;
		}
	}
	
	/* return temporary tile */
	return out_tile;
}

/* Return index of tile containing *IMAGE* pixel x, y */
int ImageTileSet::load_tile(int x, int y)
{
	#if DEBUG_IMAGE_TILESET
	char buffer[256];
	#endif
	tile_ptr new_tile;
	int tile_index, tile_index_x, tile_index_y;
	int tile_size_x, tile_size_y;
	int data_size_x, data_size_y;
	int p_in, p_out, p_length;
	int tile_check;
	int tile_memory_size;

	tile_memory_size = tex_size * tex_size * num_bands * sample_size;
	
	/* Calculate tile index */
	tile_index_x = x/tile_size;
	tile_index_y = y/tile_size;
	tile_index = (columns * tile_index_y) + tile_index_x;
	#if DEBUG_IMAGE_TILESET
	sprintf(buffer, "(II) ImageTileSet::load_tile(%d,%d) INDEX:%d (x_index %d, y_index %d)\n", x, y, tile_index, tile_index_x, tile_index_y);
	Console::write(buffer);
	#endif
	
	/* Quick hack to wrap out-of-bounds requests */
	while (tile_index_x >= columns) tile_index_x--;
	while (tile_index_y >= rows) tile_index_y--;
	tile_index = (columns * tile_index_y) + tile_index_x;
	
	/* Load if not already */
	tile_check = 0;
	while (tile_check < tiles.size()) {
		if (tiles[tile_check]->tile_index == tile_index) {
			cache_hits++;
			return tile_check;
		}
		tile_check++;
	}
	
	{ // Tile wasn't in cache
		cache_misses++;
		/* Allocate tile */
		new_tile = new tile;
		new_tile->tile_index = tile_index;
		new_tile->data = new char[tile_memory_size];
		/* Check cache space, deallocate head if oversize */
		cache_fill = cache_fill + tile_memory_size;
		if (cache_fill > cache_size) {
			if (tiles.size() > 0) {
				delete[] tiles[0]->data; /* Delete data */
				delete tiles[0]; /* Delete structure */
				tiles.erase(tiles.begin());
				cache_fill = cache_fill - tile_memory_size;
			}
		}
		
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
			data_size_x = tile_size_x / MathUtils::ipow(2,LOD);
			data_size_y = tile_size_y / MathUtils::ipow(2,LOD);
			/* X & Y are now tile top-left position */
			x = (x/tile_size) * tile_size;
			y = (y/tile_size) * tile_size;
			image_file->getRasterData(tile_size_x, tile_size_y, x, y, new_tile->data, data_size_x, data_size_y);
			/* Shuffle data for edge tiles */
			if (!((tile_size_y == tile_size) && (tile_size_x == tile_size))) {
				align_tile(&(new_tile->data), tex_size, data_size_x, data_size_y);
			}
		} else {
			data_size_x = tex_size;
			data_size_y = tex_size;
			image_file->getRasterData(image_width, image_height, 0, 0, new_tile->data, LOD_width, LOD_height);
			if (!((LOD_height == tex_size) && (LOD_width == tex_size))) {
				align_tile(&(new_tile->data), tex_size, LOD_width, LOD_height);
			}
		}
		
		/* add index to list of allocated tiles */
		tiles.push_back(new_tile);
		
		/* !! update cache */
		return tiles.size() - 1;
	}
}

/* Coords are in image pixels */
unsigned int* ImageTileSet::get_pixel_values(int x, int y)
{
	unsigned int* return_values = new unsigned int[num_bands];
	char *tmp_vals = new char[num_bands];
	int tmp, tile_index, pixel_start;
	char* tile_data;
	
	if ((x >= image_width) || (y >= image_height) || (x < 0) || (y < 0)) {
		for (tmp = 0; tmp < num_bands; tmp++) {
			return_values[tmp] = 0;
		}
		return return_values;
	}
	
	tile_index = load_tile(x,y);
	tile_data = tiles[tile_index]->data;

	/* Find index in tile */
	x = (x / MathUtils::ipow(2,LOD)) % tex_size;
	y = (y / MathUtils::ipow(2,LOD)) % tex_size;
	
	return_values[0] = x;
	return_values[1] = y;
	
	pixel_start = (x + (y*tex_size)) * (num_bands * sample_size);
	return_values[2] = pixel_start;
	for (tmp = 0; tmp < num_bands; tmp++) {
		return_values[tmp] = 0;
		return_values[tmp] = (unsigned int)tile_data[pixel_start + tmp];
		return_values[tmp] = return_values[tmp] & 0x000000FF;
	}
	
	/* This finds values by image pixels */
	return return_values;
}

/* !! tile_dimension will always be tex_size */
void ImageTileSet::align_tile(char** tile, int tile_dimension, int data_width, int data_height)
{
    char* temp_tile;        //our working space
    char* height_pad;       //our block of zeros to pad (height-wise)
    char* width_pad;        //our end-of-row padding block
    int height_pad_size, width_pad_size;    //the dimensions of our pads
    
    int i;
    
    int row_length = tile_dimension * num_bands * sample_size;   //how long is a row?
    int pad_row_length;     //the part of a row that's pad
    int data_row_length;    //the part of a row that's data
    char* copy_from_loc;    //our copy-from location pointer for memcpy
    char* copy_to_loc;      //our copy-to location pointer for memcpy
    
    // Get the sizes of our pads (in pixels)
    height_pad_size = tile_dimension - data_height;
    width_pad_size = tile_dimension - data_width;
    
    //if we need a height pad, make one of the appropriate size
    if (height_pad_size != 0)
    {
//        height_pad = (char*) malloc(row_length);    //height padding rows will always be row_length
		height_pad = new char[row_length];
        for (i = 0; i < row_length; i++)
        {
            height_pad[i] = 0;
        }
    }
    
    //if we need a width pad, make one of appropriate size
    if (width_pad_size != 0)
    {
        pad_row_length = width_pad_size * num_bands * sample_size;   //our padin bytes
//        width_pad = (char*) malloc(pad_row_length);
		width_pad = new char[pad_row_length];
        for (i = 0; i < pad_row_length; i++)
        {
            width_pad[i] = 0;
        }
    }
    
    data_row_length = data_width * num_bands * sample_size;  //our row data chunk, in bytes
    
    
    //allocate our working area to the same size as the actual tile
//    temp_tile = (char*) malloc(tile_dimension * tile_dimension * num_bands * sample_size);
	temp_tile = new char[tile_dimension * tile_dimension * num_bands * sample_size];
    
    //set our locator pointers
    copy_from_loc = *tile;
    copy_to_loc = temp_tile;
    
    if (data_row_length == row_length && height_pad_size > 0) //if we have no need for row padding
    {
        for (i = 0; i < data_height; i++)   //copy data_height rows of data
        {
            memcpy(copy_to_loc, copy_from_loc, data_row_length);
            copy_from_loc = copy_from_loc + data_row_length;
            copy_to_loc = copy_to_loc + data_row_length;
        }
        for (i = 0; i < height_pad_size; i++)   //and add height_pad_size rows of zeros
        {
            memcpy(copy_to_loc, height_pad, row_length);
            copy_to_loc = copy_to_loc + row_length;
        }
    }
    else if (data_row_length < row_length && height_pad_size > 0)   //else if we need row padding and height padding
    {
        for (i = 0; i < data_height; i++)   //copy data_height rows of data + padding
        {
            memcpy(copy_to_loc, copy_from_loc, data_row_length);
            copy_from_loc = copy_from_loc + data_row_length;
            copy_to_loc = copy_to_loc + data_row_length;
            
            memcpy(copy_to_loc, width_pad, pad_row_length);
            copy_to_loc = copy_to_loc + pad_row_length;
        }
        for (i = 0; i < height_pad_size; i++)   //and add height_pad_size rows of zeros
        {
            memcpy(copy_to_loc, height_pad, row_length);
            copy_to_loc = copy_to_loc + row_length;
        }
    }
    else if (data_row_length < row_length && height_pad_size == 0)  //if we just need row padding
    {
        for (i = 0; i < data_height; i++)   //copy data_height rows of data + padding
        {
            memcpy(copy_to_loc, copy_from_loc, data_row_length);
            copy_from_loc = copy_from_loc + data_row_length;
            copy_to_loc = copy_to_loc + data_row_length;
            
            memcpy(copy_to_loc, width_pad, pad_row_length);
            copy_to_loc = copy_to_loc + pad_row_length;
        }
    }
    else    //if we were accidentaly passed a block that didn't need padding, we'd better copy it just in case.
    {
        for (i = 0; i < data_height; i++)   //copy data_height rows of data
        {
            memcpy(copy_to_loc, copy_from_loc, data_row_length);
            copy_from_loc = copy_from_loc + data_row_length;
            copy_to_loc = copy_to_loc + data_row_length;
        }
    }
    
    //free the old tile block (could be bad otherwise)
//    free(*tile);
	delete[] *tile;
    
    //...and free our pads
    if (height_pad_size != 0)
    {
//        free(height_pad);
		delete[] height_pad;
    }
    
    if (width_pad_size != 0)
    {
//        free(width_pad);
		delete[] width_pad;
    }
    
    //replace the old tile with the new one
    *tile = temp_tile;
}

void ImageTileSet::get_cache_stats(unsigned int* size_p, unsigned int* fill_p, unsigned int* hits_p, unsigned int* misses_p)
{
	if (size_p) *size_p = cache_size;
	if (fill_p) *fill_p = cache_fill;
	if (hits_p) *hits_p = cache_hits;
	if (misses_p) *misses_p = cache_misses;
}

int ImageTileSet::get_texture_size(void) {return tex_size;}
int ImageTileSet::get_columns(void) {return columns;}
int ImageTileSet::get_rows(void) {return rows;}
int ImageTileSet::get_tile_size(void) {return tile_size;}
int ImageTileSet::get_LOD_width(void) {return LOD_width;}
int ImageTileSet::get_LOD_height(void) {return LOD_height;}
int ImageTileSet::get_LOD(void) {return LOD;}
