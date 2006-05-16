#include "config.h"
#include "ImageFile.h"


/*Constructor for ImageFile. Takes a filename, and instantiates a new
GDALDataset. Also registers all GDAL drivers, and gets info about
the new dataset*/
ImageFile::ImageFile(char* theFilename)
{
	int i;
	string leader;
	const char* message;
	
	filename = theFilename;
	
	GDALAllRegister();
	
	ifDataset = (GDALDataset *) GDALOpen(filename , GA_ReadOnly);
	
	properties = new ImageProperties(ifDataset);
	coordInfo = new CoordinateInfo(ifDataset);
	
	for (i=0;i<properties->getNumBands();i++)
	{
		theBands.push_back(new BandInfo( (GDALRasterBand*) GDALGetRasterBand(ifDataset, i+1)));
	}
}

/*Closes the handles to our dataset.*/
ImageFile::~ImageFile(void)
{
	int i;
	if (ifDataset != NULL)
	{
		#if DEBUG_IMAGE_FILE
		MessageBox (NULL, "Closing image file.", "Parbat3D :: ImageFile", 0);
		#endif
		GDALClose(ifDataset);
	}
	
	delete properties;
	delete coordInfo;
	for (i=0;i<properties->getNumBands();i++)
	{
		delete theBands[i];
	}
}

ImageProperties* ImageFile::getImageProperties(void)
{
	return properties;
}

BandInfo* ImageFile::getBandInfo(int bandNumber)
{
	if ((bandNumber <= properties->getNumBands()) && (bandNumber > 0))
	{
		return theBands[bandNumber-1];
	}
	else
	{
		MessageBox (NULL, "Band could not be retrieved: index out of range!", "Parbat3D :: ImageFile", 0);
		return NULL;
	}
}

/*Prints some basic info about the dataset: driver, driver long name, x size, y size
and the number of raster bands.*/
void ImageFile::printInfo(void)
{
	const char* message;
	string leader;
	
	if (ifDataset != NULL)
	{
		leader = "Driver: ";
		message = makeMessage(leader, (char*) properties->getDriverName());
		MessageBox (NULL, message, "Parbat3D :: ImageFile", 0);
		
		leader = "Driver (long name): ";
		message = makeMessage(leader, (char*) properties->getDriverLongName());
		MessageBox (NULL, message, "Parbat3D :: ImageFile", 0);
		
		leader = "X size: ";
		message = makeMessage(leader, properties->getWidth());
		MessageBox (NULL, message, "Parbat3D :: ImageFile", 0);
		
		leader = "Y size: ";
		message = makeMessage(leader, properties->getHeight());
		MessageBox (NULL, message, "Parbat3D :: ImageFile", 0);
		
		leader = "Raster bands count: ";
		message = makeMessage(leader, properties->getNumBands());
		MessageBox (NULL, message, "Parbat3D :: ImageFile", 0);
	}
	else
	{
		MessageBox (NULL, "Cannot get info; dataset was not loaded!", "Parbat3D :: ImageFile", 0);
	}
}
