#ifndef _COORDINATE_INFO_H
#define _COORDINATE_INFO_H

#if TMP_USE_IMAGE_FILE
#include <iosfwd>
#include <cstdio>
#include <math.h>

#define CPL_STDCALL __stdcall
#define CPL_DISABLE_STDCALL 1
#include <gdal_priv.h>

#include "config.h"
#include <string>
#if PLATFORM_W32
#include <windows.h>
#endif

class CoordinateInfo
{
	public:
		CoordinateInfo(GDALDataset* dataset);
		virtual ~CoordinateInfo(void);
		
	private:
		double geoTransform[6];
};
#endif

#endif

