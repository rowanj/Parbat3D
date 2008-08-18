#ifndef _IMAGE_FILE_H
#define _IMAGE_FILE_H

#include "ImageProperties.h"
#include "CoordinateInfo.h"
#include "BandInfo.h"

using std::vector;

class ImageFile
{
	public:
		ImageFile(const std::string& theFilename);
		int getifErr(void);
		virtual ~ImageFile(void);
		ImageProperties* getImageProperties(void);
		BandInfo* getBandInfo(int bandNumber);
		std::string getInfoString(void);
		void getRasterData(int width, int height, int xpos, int ypos, char* buffer, int outWidth, int outHeight);
		
	private:
		int ifErr;
		GDALDataset* ifDataset;
		std::string filename;
		std::string infoString;
		ImageProperties* properties;
		CoordinateInfo* coordInfo;
		vector<BandInfo*> theBands;
};

#endif
