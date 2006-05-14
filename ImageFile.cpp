#include "config.h"
#if TMP_USE_IMAGE_FILE
#include "ImageFile.h"


/*Constructor for ImageFile. Takes a filename, and instanciates a new
GDALDataset. Also registers all GDAL drivers, and gets info about
the new dataset*/
ImageFile::ImageFile(char* theFilename)
{
	filename = theFilename;
	
	GDALAllRegister();
	
	ifDataset = (GDALDataset *) GDALOpen(filename , GA_ReadOnly);
	
	properties = new ImageProperties(ifDataset);
}

/*Closes the handles to our dataset.*/
ImageFile::~ImageFile(void)
{
	if (ifDataset != NULL)
	{
		MessageBox (NULL, "Closing image file.", "Parbat3D :: ImageFile", 0);
		GDALClose(ifDataset);
	}
	
	delete properties;
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
#endif
