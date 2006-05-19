#include "config.h"
#include "ImageProperties.h"


ImageProperties::ImageProperties(GDALDataset* dataset)
{
	if(dataset == NULL)
	{
		#if DEBUG_IMAGE_PROPERTIES
		MessageBox (NULL, "Dataset did not load!", "Parbat3D :: ImageProperties", 0);
		#endif
		driverName = NULL;
		driverLongName = NULL;
		width = 0;
		height = 0;
		numBands = 0;
	}
	else
	{
		#if DEBUG_IMAGE_PROPERTIES
		MessageBox (NULL, "Dataset loaded", "Parbat3D :: ImageProperties", 0);
		#endif
		driverName = (char *) GDALGetDescription(GDALGetDatasetDriver(dataset));
		driverLongName = (char *) GDALGetMetadataItem(GDALGetDatasetDriver(dataset),
														GDAL_DMD_LONGNAME,0);
		width = GDALGetRasterXSize(dataset);
		height = GDALGetRasterYSize(dataset);
		numBands = GDALGetRasterCount(dataset);
	}
}

int ImageProperties::getWidth(void)
{
	return width;
}

int ImageProperties::getHeight(void)
{
	return height;
}

int ImageProperties::getNumBands(void)
{
	return numBands;
}

const char* ImageProperties::getDriverName(void)
{
	return driverName;
}

const char* ImageProperties::getDriverLongName(void)
{
	return driverLongName;
}

ImageProperties::~ImageProperties(void)
{
	return;	
}

const char* ImageProperties::getFileName(void)
{
    return imageFileName;
}    

void ImageProperties::setFileName(char* n)
{
    imageFileName = n;
}    

