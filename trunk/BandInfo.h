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
#if PLATFORM_W32
#include <windows.h>
#endif //PLATFORM_W32


class BandInfo
{
	public:
		BandInfo(GDALRasterBand* theBand);
		const char* getRasterDataType(void);
		const char* getColourInterpretationName(void);
		double getDataMin(void);
		double getDataMax(void);
		int getBlockXSize(void);
		int getBlockYSize(void);
		int getOverviewCount(void);
		int getBandNumber(void);
		GDALRasterBand* getBand(void);
		virtual ~BandInfo(void);
		
	private:
		const char* rasterDataType;
		const char* colourInterpretationName;
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

