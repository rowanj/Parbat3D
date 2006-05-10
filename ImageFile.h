#ifndef _IMAGE_FILE_H
#define _IMAGE_FILE_H

#include <iosfwd>
#include <cstdio>
#include <cstring>
#include <math.h>

#include <gdal_priv.h>

#include "config.h"
#include <string>
#if PLATFORM_W32
#include <windows.h>
#endif


class ImageFile
{
public:
	ImageFile(char* filename);
	virtual ~ImageFile(void);
	void printInfo(void);
	
private:
	GDALDataset *ifDataset;
	double adfGeoTransform[6];
};

#endif
