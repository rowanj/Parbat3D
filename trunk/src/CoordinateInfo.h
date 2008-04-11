#ifndef _COORDINATE_INFO_H
#define _COORDINATE_INFO_H

#include <iosfwd>
#include <cstdio>
#include <math.h>

#define CPL_STDCALL __stdcall
#define CPL_DISABLE_STDCALL 1
#include <gdal_priv.h>

#include "config.h"
#include <string>
#include <windows.h>

class CoordinateInfo
{
	public:
		CoordinateInfo(GDALDataset* dataset);
		virtual ~CoordinateInfo(void);
		
	private:
		double geoTransform[6];
};
#endif

