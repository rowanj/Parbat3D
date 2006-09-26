#include "config.h"
#include "ImageProperties.h"


ImageProperties::ImageProperties(GDALDataset* dataset, char* filename)
{
	if(dataset == NULL)
	{
		#if DEBUG_IMAGE_PROPERTIES
		MessageBox (NULL, "Dataset did not load!", "Parbat3D :: ImageProperties", 0);
		#endif
		driverName = NULL;
		driverLongName = NULL;
		imageFileName = NULL;
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
		imageFileName = filename;
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
}

/* Returns a string containing the file name, without path */

const char* ImageProperties::getFileName(void)
{
	string::size_type position;
	string name;
	string truncName;
	const char * finalName;
	
	name = imageFileName;
	position = name.find_last_of("\\");
	assert(position < name.length());
	truncName = name.substr(position + 1, name.length() - position);
	finalName = truncName.c_str();
	
	// !! This is returning a dynamically-allocated array cast to const (causes access violation) - Rowan
	//    return finalName;
	
	return imageFileName;
}    


