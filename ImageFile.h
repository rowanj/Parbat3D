#ifndef _IMAGE_FILE_H
#define _IMAGE_FILE_H

//#if TMP_USE_IMAGE_FILE
#include <iosfwd>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <string>
#include <iostream>
#include <math.h>

#define CPL_STDCALL __stdcall
#define CPL_DISABLE_STDCALL 1
#include <gdal_priv.h>

#include "config.h"
#include "ImageProperties.h"
#include "StringUtils.h"
#include <string>
#if PLATFORM_W32
#include <windows.h>
#endif


class ImageFile
{
public:
	ImageFile(char* theFilename);
	virtual ~ImageFile(void);
	void printInfo(void);
	
private:
	GDALDataset* ifDataset;
	char* filename;
	ImageProperties* properties;
};

//#endif
#endif
