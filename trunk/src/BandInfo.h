#ifndef _BAND_INFO_H
#define _BAND_INFO_H

#include <iosfwd>
#include <cstdio>
#include <math.h>

#define CPL_STDCALL __stdcall
#define CPL_DISABLE_STDCALL 1
#include <gdal_priv.h>

#include "config.h"
#include <string>
#include "StringUtils.h"

#include "console.h"
#include <windows.h>

class BandInfo
{
	public:
		BandInfo(GDALRasterBand* theBand);
		std::string getRasterDataType(void);
		double getDataMin(void);
		double getDataMax(void);
		int getBlockXSize(void);
		int getBlockYSize(void);
		int getOverviewCount(void);
		int getBandNumber(void);
		GDALRasterBand* getBand(void);
		
	private:
		std::string rasterDataType;
		int bandNumber;
		int overviewCount;
		int blockXSize;
		int blockYSize;
		int bGotMin;
		int bGotMax;
		double adfMinMax[2];
		GDALRasterBand* band;
};
#endif //_BAND_INFO_H

