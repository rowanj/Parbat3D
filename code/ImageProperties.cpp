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
		shortFileName = NULL;
	}
	else
	{
		#if DEBUG_IMAGE_PROPERTIES
		MessageBox (NULL, "Dataset loaded", "Parbat3D :: ImageProperties", 0);
		#endif
		driverName = (char *) GDALGetDescription(GDALGetDatasetDriver(dataset));
		driverLongName = (char *) GDALGetMetadataItem(GDALGetDatasetDriver(dataset),
														GDAL_DMD_LONGNAME,0);
		Console::write("ImageProperties::Constructor - Filename passed is %s\n", filename);
		imageFileName = new char[512];
		shortFileName = new char[512];
		strcpy(imageFileName, filename);
		Console::write("ImageProperties::Constructor - got past the strcpy\n");
		Console::write("ImageProperties::Constructor - copied name is %s\n", imageFileName);
		
		width = GDALGetRasterXSize(dataset);
		height = GDALGetRasterYSize(dataset);
		numBands = GDALGetRasterCount(dataset);
		
		string::size_type position;
		string name;
		string truncName;
		
		name = imageFileName;
		position = name.find_last_of("\\");
		assert(position < name.length());
		truncName = name.substr(position + 1, name.length() - position);
		strcpy(shortFileName, (char *)truncName.c_str());
		Console::write("ImageProperties::Constructor - short name is %s\n", shortFileName);
	}
}

ImageProperties::~ImageProperties(void)
{
	if (shortFileName != NULL)
		delete[] shortFileName;
	if (imageFileName != NULL)
		delete[] imageFileName;
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

const char* ImageProperties::getFileName(void)
{	
	Console::write("ImageProperties::getFileName - Short filename is %s\n", shortFileName);
	return (const char *)shortFileName;
}    


