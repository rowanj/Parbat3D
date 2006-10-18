#ifndef _IMAGE_FILE_H
#define _IMAGE_FILE_H

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
#include <windows.h>

using std::vector;

class ImageFile
{
	public:
		ImageFile(char* theFilename);
		int getifErr(void);
		virtual ~ImageFile(void);
		ImageProperties* getImageProperties(void);
		BandInfo* getBandInfo(int bandNumber);
		const char* getInfoString(void);
		void getRasterData(int width, int height, int xpos, int ypos, char* buffer, int outWidth, int outHeight);
		
	private:
		int ifErr;
		GDALDataset* ifDataset;
		char* filename;
		char* infoString;
		ImageProperties* properties;
		CoordinateInfo* coordInfo;
		vector<BandInfo*> theBands;
};

#endif
