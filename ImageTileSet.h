#ifndef _IMAGE_TILE_SET_H
#define _IMAGE_TILE_SET_H

#include "ImageFile.h"

class ImageTileSet
{
public:
	ImageTileSet(int LOD, ImageFile* file, int tex_size);
	virtual ~ImageTileSet(void);
	void set_region(int x, int y, int width, int width);
	char* get_tile(int x, int y);
	char* get_value(
	void free_tiles();
	
private:
	int LOD;
	RECT current_region;
}

#endif
