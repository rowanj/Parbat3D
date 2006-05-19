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
	
	properties = new ImageProperties(ifDataset, filename);
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
const char* ImageFile::getInfoString(void)
{
	const char* message1;
	const char* message2;
	string leader;
	
	if (ifDataset != NULL)
	{
		/*leader = "Driver: ";
		message = makeMessage(leader, (char*) properties->getDriverName());
		MessageBox (NULL, message, "Parbat3D :: ImageFile", 0);*/
		
		leader = "Type: ";
		message1 = makeMessage(leader, (char*) properties->getDriverLongName());
		
		leader = ", X: ";
		message2 = makeMessage(leader, properties->getWidth());
		
		message1 = catcstrings((char*) message1, (char*) message2);
		
		leader = ", Y: ";
		message2 = makeMessage(leader, properties->getHeight());
		
		message1 = catcstrings((char*) message1, (char*) message2);
		
		leader = ", Bands: ";
		message2 = makeMessage(leader, properties->getNumBands());
		
		message1 = catcstrings((char*) message1, (char*) message2);
		
		return message1;
	}
	else
	{
		MessageBox (NULL, "Cannot get info; dataset was not loaded!", "Parbat3D :: ImageFile", 0);
		return NULL;
	}
}

void ImageFile::getRasterData(int width, int height, int xpos, int ypos, char* buffer, int outWidth, int outHeight)
{
    CPLErr myErr;
    
    myErr = GDALDatasetRasterIO(ifDataset, GF_Read, xpos, ypos, width, height, buffer, outWidth, outHeight, GDT_Byte,
    					3, NULL, 3, 3*outWidth, 1);

	if (myErr == CE_Failure)
	{
	    MessageBox (NULL, "RasterIO failed!!", "Parbat3D :: ImageFile", 0);
	}
}
