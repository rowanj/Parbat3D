#ifndef _IMAGE_PROPERTIES_H
#define _IMAGE_PROPERTIES_H



class ImageProperties
{
	public:
		ImageProperties(GDALDataset* dataset, const std::string& filename);
		int getWidth(void);
		int getHeight(void);
		int getNumBands(void);
		const char* getDriverName(void);
		const char* getDriverLongName(void);
		string getFileName(void);
		
	private:
		int width;
		int height;
		int numBands;
		const char* driverName;
		const char* driverLongName;
		std::string shortFileName;
		std::string imageFileName;
};
#endif
