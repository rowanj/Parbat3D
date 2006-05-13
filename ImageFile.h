#ifndef _IMAGE_FILE_H
#define _IMAGE_FILE_H

//#if TMP_USE_IMAGE_FILE
#include <iosfwd>
#include <cstdio>
#include <cstring>
#include <math.h>

#define CPL_STDCALL __stdcall
#define CPL_DISABLE_STDCALL 1
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

//#endif
#endif
