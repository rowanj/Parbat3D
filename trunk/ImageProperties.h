#ifndef _IMAGE_PROPERTIES_H
#define _IMAGE_PROPERTIES_H
#include <iosfwd>
#include <cstdio>
#include <math.h>

#define CPL_STDCALL __stdcall
#define CPL_DISABLE_STDCALL 1
#include <gdal_priv.h>

#include "config.h"
#include "StringUtils.h"
#include "console.h"
#include <string>
#include <iostream>

#include <windows.h>


class ImageProperties
{
	public:
		ImageProperties(GDALDataset* dataset, char* filename);
		int getWidth(void);
		int getHeight(void);
		int getNumBands(void);
		const char* getDriverName(void);
		const char* getDriverLongName(void);
		virtual ~ImageProperties(void);
		const char* getFileName(void);
		
	private:
		int width;
		int height;
		int numBands;
		char* shortFileName;
		const char* driverName;
		const char* driverLongName;
		char* imageFileName;
};
#endif
