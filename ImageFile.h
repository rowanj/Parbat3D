#ifndef _IMAGE_FILE_H
#define _IMAGE_FILE_H

#include "config.h"

#include <iosfwd>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <math.h>

#define CPL_STDCALL __stdcall
#define CPL_DISABLE_STDCALL 1

#include <gdal_priv.h>


#include "ImageProperties.h"
#include "CoordinateInfo.h"
#include "BandInfo.h"
#include "StringUtils.h"
#include <string>
#if PLATFORM_W32
#include <windows.h>
#endif

using std::vector;

class ImageFile
{
public:
	ImageFile(char* theFilename);
	virtual ~ImageFile(void);
	ImageProperties* getImageProperties(void);
	BandInfo* getBandInfo(int bandNumber);
	const char* getInfoString(void);
	
private:
	GDALDataset* ifDataset;
	char* filename;
	ImageProperties* properties;
	CoordinateInfo* coordInfo;
	vector<BandInfo*> theBands; 
};

#endif
