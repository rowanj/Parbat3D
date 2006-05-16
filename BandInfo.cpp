#include "config.h"

#if TMP_USE_IMAGE_FILE
#include "BandInfo.h"

BandInfo::BandInfo(GDALRasterBand* theBand)
{
	#if DEBUG_BANDS
	const char* message;
	string leader;
	#endif //DEBUG_BANDS
	GDALColorInterp theColourInterpretation;
	
	band = (GDALRasterBand*) theBand;
	
	bandNumber = GDALGetBandNumber(theBand);
	#if DEBUG_BANDS
	leader = "Band Number: ";
	message = makeMessage(leader, bandNumber);
	MessageBox (NULL, message, "Parbat3D :: BandInfo", 0);
	#endif //DEBUG_BANDS
	
	rasterDataType = GDALGetDataTypeName(GDALGetRasterDataType(band));
	#if DEBUG_BANDS
	if (rasterDataType != NULL)
	{
		leader = "Raster Data Type: ";
		message = makeMessage(leader, (char*) rasterDataType);
		MessageBox (NULL, message, "Parbat3D :: BandInfo", 0);
	}
	else
	{
		MessageBox (NULL, "No raster data type", "Parbat3D :: BandInfo", 0);
	}
	#endif //DEBUG_BANDS
	
	//theColourInterpretation = band->GetColorInterpretation(); //this is not working
	theColourInterpretation = GDALGetRasterColorInterpretation(band);
	

	if (theColourInterpretation > -1)
	{
		colourInterpretationName = GDALGetColorInterpretationName(theColourInterpretation);
	}
	else
	{
		colourInterpretationName = NULL;
	}
	
	#if DEBUG_BANDS
	if (colourInterpretationName != NULL)
	{
		leader = "Color Name: ";
		message = makeMessage(leader, (char*) colourInterpretationName);
		MessageBox (NULL, message, "Parbat3D :: BandInfo", 0);
	}
	else
	{
		MessageBox (NULL, "No colour name", "Parbat3D :: BandInfo", 0);
	}
	#endif //DEBUG_BANDS
		
	overviewCount = GDALGetOverviewCount(band);
	#if DEBUG_BANDS
	leader = "Overviews: ";
	message = makeMessage(leader, overviewCount);
	MessageBox (NULL, message, "Parbat3D :: BandInfo", 0);
	#endif //DEBUG_BANDS
		
	GDALGetBlockSize(band, &blockXSize, &blockYSize); 
	adfMinMax[0] = GDALGetRasterMinimum(theBand, &bGotMin);
	adfMinMax[1] = GDALGetRasterMaximum(theBand, &bGotMax);
	
	/*if( !(bGotMin && bGotMax) )
	{
		GDALComputeRasterMinMax(band, TRUE, adfMinMax);
	}*/
	#if DEBUG_BANDS
	leader = "Min value: ";
	message = makeMessage(leader, adfMinMax[0]);
	MessageBox (NULL, message, "Parbat3D :: BandInfo", 0);
	leader = "Max value: ";
	message = makeMessage(leader, adfMinMax[1]);
	MessageBox (NULL, message, "Parbat3D :: BandInfo", 0);
	#endif //DEBUG_BANDS
}

const char* BandInfo::getRasterDataType(void)
{
	return rasterDataType;
}

const char* BandInfo::getColourInterpretationName(void)
{
	return colourInterpretationName;
}

double BandInfo::getDataMin(void)
{
	return adfMinMax[0];
}

double BandInfo::getDataMax(void)
{
	return adfMinMax[1];
}

int BandInfo::getBlockXSize(void)
{
	return blockXSize;
}

int BandInfo::getBlockYSize(void)
{
	return blockYSize;
}

int BandInfo::getOverviewCount(void)
{
	return overviewCount;
}

int BandInfo::getBandNumber(void)
{
	return bandNumber;
}

GDALRasterBand* BandInfo::getBand(void)
{
	return band;	
}

BandInfo::~BandInfo(void)
{
	return;
}
		
#endif //TMP_USE_IMAGE_FILE
