#include "config.h"
#include "ImageFile.h"

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
	int i;
	string leader;
	const char* message;
	
	filename = theFilename;
	
	GDALAllRegister();
	
	ifDataset = (GDALDataset *) GDALOpen(filename , GA_ReadOnly);
	
	if (ifDataset == NULL) //if we've not got a valid file
	{
		ifErr = 1;
	}
	else
	{
		properties = new ImageProperties(ifDataset, filename);
		coordInfo = new CoordinateInfo(ifDataset);
		
		for (i=0;i<properties->getNumBands();i++)
		{
			theBands.push_back(new BandInfo( (GDALRasterBand*) GDALGetRasterBand(ifDataset, i+1)));
		}
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
		#if DEBUG_IMAGE_FILE
		Console::write("ImageFile - Cannot get info; dataset was not loaded!\n");
		#endif
		return NULL;
	}
}

/*
ImageFile::getRasterData

Fill a buffer with raster data from the file, as specified.
*/
void ImageFile::getRasterData(int width, int height, int xpos, int ypos, char* buffer, int outWidth, int outHeight)
{
	int startTime = 0;
	int endTime = 0;
	float elapsedTime = 0.0;
	
    CPLErr myErr;
    
    int bands;
	
	bands = properties->getNumBands();
	
    myErr = GDALDatasetAdviseRead(ifDataset, xpos, ypos, width, height, outWidth, outHeight, GDT_Byte, bands, NULL, NULL);
    if (myErr == CE_Failure)
	{
		Console::write("ImageFile - Failed to inform driver of upcoming read!\n");
	}
	
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
		char* xposstring;
		char* yposstring;
		char* heightstring;
		char* widthstring;
		char* outheightstring;
		char* outwidthstring;
		char* timeString;
		char* finalstring;
		
		xposstring = (char*)inttocstring(xpos);
		yposstring = (char*)inttocstring(ypos);
		heightstring = (char*)inttocstring(height);
		widthstring = (char*)inttocstring(width);
		outheightstring = (char*)inttocstring(outHeight);
		outwidthstring = (char*)inttocstring(outWidth);
		timeString = (char*)floattocstring(elapsedTime);
		
		finalstring = (char*)catcstrings("ImageFile - Got data at location X = ", xposstring);
		finalstring = (char*)catcstrings(finalstring, ", Y = ");
		finalstring = (char*)catcstrings(finalstring, yposstring);
		finalstring = (char*)catcstrings(finalstring, "\n");
		Console::write(finalstring);
		finalstring = (char*)catcstrings("ImageFile - Data width = ", widthstring);
		finalstring = (char*)catcstrings(finalstring, ", height = ");
		finalstring = (char*)catcstrings(finalstring, heightstring);
		finalstring = (char*)catcstrings(finalstring, "\n");
		Console::write(finalstring);
		finalstring = (char*)catcstrings("ImageFile - Output width = ", outwidthstring);
		finalstring = (char*)catcstrings(finalstring, ", output height = ");
		finalstring = (char*)catcstrings(finalstring, outheightstring);
		finalstring = (char*)catcstrings(finalstring, "\n");
		Console::write(finalstring);
		finalstring = (char*)catcstrings("ImageFile - Elapsed time was ", timeString);
		finalstring = (char*)catcstrings(finalstring, " seconds\n\n");
		Console::write(finalstring);
		
		delete(xposstring);
		delete(yposstring);
		delete(heightstring);
		delete(widthstring);
		delete(outheightstring);
		delete(outwidthstring);
		delete(finalstring);
	}
}
