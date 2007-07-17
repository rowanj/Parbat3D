#include "config.h"
#include "ImageFile.h"
#include "main.h"

/*
ImageFile

=========

ImageFile is the main store and interface to the file we're using.
It is also the wrapper for the GDAL dataset that is returned by the
GDAL library.


*/

/*
ImageFile::ImageFile

Constructor for ImageFile. Takes a filename, and instantiates a new
GDALDataset. Also registers all GDAL drivers, and gets info about
the new dataset*/
ImageFile::ImageFile(char* theFilename)
{
	filename = new char[512];
	infoString = new char[512];
	
	strcpy(filename, theFilename);
	Console::write("ImageFile::Constructor - Filename passed was %s\n", theFilename);
	Console::write("ImageFile::Constructor - Filename copy result is %s\n", filename);
	
	GDALAllRegister();
	
	ifErr = 0;
	ifDataset = (GDALDataset *) GDALOpen(filename , GA_ReadOnly);
	
	if (ifDataset == NULL) //if we've not got a valid file
	{
		ifErr = 1;
	}
	else
	{
        ifErr = 0;  /**** bug fix by shane ****/
		properties = new ImageProperties(ifDataset, filename);
		coordInfo = new CoordinateInfo(ifDataset);
		
		for (int i=0;i<properties->getNumBands();i++)
		{
			theBands.push_back(new BandInfo( (GDALRasterBand*) GDALGetRasterBand(ifDataset, i+1)));
		}

		sprintf(infoString, "Type: %s, X: %d, Y: %d, Bands: %d\n",
				(char*)properties->getDriverLongName(),
				properties->getWidth(),
				properties->getHeight(),
				properties->getNumBands());
		
		Console::write("ImageFile::Constructor - infoString is %s\n", infoString);
	}
}

int ImageFile::getifErr(void)
{
	return ifErr;
}

/*
ImageFile::~ImageFile

Deconstructor for ImageFile, which closes the handles to our dataset.
*/
ImageFile::~ImageFile(void)
{
	int i;
	if (ifDataset != NULL)
	{
		#if DEBUG_IMAGE_FILE
		Console::write("ImageFile - Closing image file.\n");
		#endif
		GDALClose(ifDataset);
	}
	
	delete properties;
	delete coordInfo;
	for (i=0;i<properties->getNumBands();i++)
	{
		delete theBands[i];
	}
	delete[] filename;
	delete[] infoString;
}

/*
ImageFile::getImageProperties

Returns the properties object of the ImageFile.
*/
ImageProperties* ImageFile::getImageProperties(void)
{
	return properties;
}

/*
ImageFile::getBandInfo

Gets the BandInfo object for a given band ID number.
*/
BandInfo* ImageFile::getBandInfo(int bandNumber)
{
	if ((bandNumber <= properties->getNumBands()) && (bandNumber > 0))
	{
		return theBands[bandNumber-1];
	}
	else
	{
	#if DEBUG_IMAGE_FILE
		Console::write("ImageFile - Band could not be retrieved: index out of range!\n");
	#endif
		return NULL;
	}
}

/*
ImageFile::getInfoString

Prints some basic info about the dataset: driver, driver long name, x size, y size
and the number of raster bands.
*/
const char* ImageFile::getInfoString(void)
{
	return (const char *)infoString;
}

/*
ImageFile::getRasterData

Fill a buffer with raster data from the file, as specified.
*/
void ImageFile::getRasterData(int width, int height, int xpos, int ypos, char* buffer, int outWidth, int outHeight)
{
	emptyOutMessageQueue();	// prevent program from entering "not-responding" state

	int startTime = 0;
	int endTime = 0;
	float elapsedTime = 0.0;
	
    CPLErr myErr;
    
    int bands;
	
	bands = properties->getNumBands();
	
    /*myErr = GDALDatasetAdviseRead(ifDataset, xpos, ypos, width, height, outWidth, outHeight, GDT_Byte, bands, NULL, NULL);
    if (myErr == CE_Failure)
	{
		Console::write("ImageFile - Failed to inform driver of upcoming read!\n");
	}*/
	
    startTime = GetTickCount();
    myErr = GDALDatasetRasterIO(ifDataset, GF_Read, xpos, ypos, width, height, buffer, outWidth, outHeight, GDT_Byte,
    					bands, NULL, bands, bands*outWidth, 1);
	endTime = GetTickCount();
	elapsedTime = (float)(endTime - startTime) / 1000.0;
	
	if (myErr == CE_Failure)
	{
	    MessageBox (NULL, "RasterIO failed!!", "Parbat3D :: ImageFile", 0);
		#if DEBUG_IMAGE_FILE
		Console::write("ImageFile - RasterIO failed!!\n");
		#endif
	}
	else
	{	
		Console::write("ImageFile - Got data at location X = %d, Y = %d\n", xpos, ypos);
		Console::write("ImageFile - Data width = %d, height = %d\n", width, height);
		Console::write("ImageFile - Output width = %d, height = %d\n", outWidth, outHeight);
		Console::write("ImageFile - Elapsed time was %f seconds\n\n", elapsedTime);
	}
}
