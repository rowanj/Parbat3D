#ifndef _BAND_INFO_H
#define _BAND_INFO_H

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

