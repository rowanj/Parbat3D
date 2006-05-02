#include <Windows.h"

#include "ImageFile"

class ImageTileSet
{
public:
	ImageTileSet(int LOD, ImageFile* file, int tile_size);
	virtual ~ImageTileSet(void);
	void set_region(PRECT region);
	char* get_tile(int x, int y);
private:
	int LOD;
	RECT current_region;
}
