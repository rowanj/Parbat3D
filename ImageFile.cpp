#if TMP_USE_IMG_FILE
#include "ImageFile.h"


ImageFile::ImageFile(char* filename)
{
	GDALAllRegister();
	
	ifDataset = (GDALDataset *) GDALOpen(filename , GA_ReadOnly);
	if(ifDataset == NULL)
	{
		MessageBox (NULL, "Dataset did not load!", "Parbat3D :: ImageFile", 0);
	}
	else
	{
		MessageBox (NULL, "Dataset loaded", "Parbat3D :: ImageFile", 0);
	}
}

ImageFile::~ImageFile(void)
{
	MessageBox (NULL, "Closing image file.", "Parbat3D :: ImageFile", 0);
	GDALClose(ifDataset);	
}

void ImageFile::printInfo(void)
{
	int i;
	char* description;
	char* longname;
	int rasterxsize;
	int rasterysize;
	int rastercount;
	
	description = (char *) ifDataset->GetDriver()->GetDescription();
	longname = (char *) ifDataset->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME);
	rasterxsize = ifDataset->GetRasterXSize();
	rasterysize = ifDataset->GetRasterYSize();
	rastercount = ifDataset->GetRasterCount();
	
	MessageBox (NULL, description, "Parbat3D :: ImageFile", 0);
	MessageBox (NULL, longname, "Parbat3D :: ImageFile", 0);
}
#endif
