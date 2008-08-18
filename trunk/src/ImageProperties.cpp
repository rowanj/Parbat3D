#include "config.h"
#include "ImageProperties.h"


ImageProperties::ImageProperties(GDALDataset* dataset, const std::string& filename) :
	width(0),
	height(0),
	numBands(0),
	driverName(NULL),
	driverLongName(NULL),
	imageFileName(filename)
{
	assert(dataset != NULL);

	driverName = (char *) GDALGetDescription(GDALGetDatasetDriver(dataset));
	driverLongName = (char *) GDALGetMetadataItem(GDALGetDatasetDriver(dataset),
												  GDAL_DMD_LONGNAME,0);
												  
	width = GDALGetRasterXSize(dataset);
	height = GDALGetRasterYSize(dataset);
	numBands = GDALGetRasterCount(dataset);
		
	string::size_type position = imageFileName.find_last_of("\\");
	if (position != string::npos) {
		shortFileName = imageFileName.substr(position + 1);
	} else {
		shortFileName = imageFileName;
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

/* Returns a string containing the file name, without path */

string ImageProperties::getFileName(void)
{	
	return shortFileName;
}    


