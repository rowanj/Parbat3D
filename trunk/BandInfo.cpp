#include "config.h"

#include "BandInfo.h"

/*
BandInfo

========

A BandInfo object is instantiated as part of the ImageFile class instantiation. It
contains information about the spectral bands present in the image file. It calls
GDAL where appropriate, and is passed a GDALRasterBand from which it obtains its
data.

Many of the GDAL calls do not produce any output for many file types. Things like
Colour Interpretation Name, for example, do not seem to produce results.
*/

/*
BandInfo::BandInfo

Constructor for the BandInfo class. Stores the band number, the raster data type,
the colour interpretation name (if available), the X and Y block sizes and the
minimum and maximum values in this band.
*/
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

/*
BandInfo::getRasterDataType

Returns the raster data type of the band.
*/
const char* BandInfo::getRasterDataType(void)
{
	return rasterDataType;
}

/*
BandInfo::getColourInterpretationName

Returns the colour interpretation name of the band.
*/
const char* BandInfo::getColourInterpretationName(void)
{
	return colourInterpretationName;
}

/*
BandInfo::getDataMin

Returns the minimum value of the band. Used for histograms and transforms.
*/
double BandInfo::getDataMin(void)
{
	return adfMinMax[0];
}

/*
BandInfo::getDataMax

Returns the maximum value of the band. Used for histograms and transforms.
*/
double BandInfo::getDataMax(void)
{
	return adfMinMax[1];
}

/*
BandInfo::getBlockXSize

Returns the block size of the band on the X axis.
*/
int BandInfo::getBlockXSize(void)
{
	return blockXSize;
}

/*
BandInfo::getBlockYSize

Returns the block size of the band on the Y axis.
*/
int BandInfo::getBlockYSize(void)
{
	return blockYSize;
}

/*
BandInfo::getOverviewCount

Returns the number of overviews for this band.
*/
int BandInfo::getOverviewCount(void)
{
	return overviewCount;
}

/*
BandInfo::getBandNumber

Returns the band number for this band.
*/
int BandInfo::getBandNumber(void)
{
	return bandNumber;
}

/*
BandInfo::getBand

Returns a pointer to the GDAL band structure itself.
*/
GDALRasterBand* BandInfo::getBand(void)
{
	return band;	
}

/*
BandInfo::~BandInfo

Deconstructor; currently does nothing.
*/
BandInfo::~BandInfo(void)
{
	return;
}
