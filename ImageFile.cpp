#include "config.h"
#if TMP_USE_IMAGE_FILE
#include "ImageFile.h"


ImageFile::ImageFile(char* filename)
{
    MessageBox(NULL,"Attemping to load image file...","Parbat3d :: ImageFile",0);
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


char* copyString(const char *source)
{
    char *desc=new char[255];
    int p=0;
    for (p=0;p<254;p++)
    {
        if (source[p]==0)
        {
            break;
        }
        desc[p]=source[p];
    }
    desc[p]=0;    
    return desc;
}

void ImageFile::printInfo(void)
{
	int i;
	const char* description;
	const char* longname;
	int rasterxsize;
	int rasterysize;
	int rastercount;

    //can't link this
/*
	description = (char *) ifDataset->GetDriver()->GetDescription();
	longname = (char *) ifDataset->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME);
	rasterxsize = ifDataset->GetRasterXSize();
	rasterysize = ifDataset->GetRasterYSize();
	rastercount = ifDataset->GetRasterCount();
*/

// this doesn't return 0, but i thought I would check anyway
if (GDALGetDatasetDriver(ifDataset)==0)
{
    MessageBox(NULL,"NULL","Parbat3d :: ImageFile",0);
    return;
}

//description=GDALGetDescription(GDALGetDatasetDriver(ifDataset));

    char desc[255];
	description = GDALGetDescription(GDALGetDatasetDriver(ifDataset));
    int p=0;
    for (p=0;p<254;p++)
    {
        if (description[p]==0)
        {
            break;
        }
        desc[p]=description[p];
    }
    desc[p]=0;    	
	MessageBox (NULL, desc, "Parbat3D :: ImageFile", 0);
        
	longname = GDALGetMetadataItem(GDALGetDatasetDriver(ifDataset),GDAL_DMD_LONGNAME,0);
	char longn[1024];
	for (p=0;p<254;p++)
    {
        if (longname[p]==0)
        {
            break;
        }
        longn[p]=longname[p];
    }
    longn[p]=0;
	MessageBox (NULL, longn, "Parbat3D :: ImageFile", 0);
	
	rasterxsize =  GDALGetRasterXSize(ifDataset);
	rasterysize = GDALGetRasterYSize(ifDataset);
	rastercount = GDALGetRasterCount(ifDataset);

/*
	description = (char *) ifDataset->GetDriver()->GetDescription();
	longname = (char *) ifDataset->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME);
	rasterxsize = ifDataset->GetRasterXSize();
	rasterysize = ifDataset->GetRasterYSize();
	rastercount = ifDataset->GetRasterCount();
*/
	

//	MessageBox (NULL, longname, "Parbat3D :: ImageFile", 0);
}
#endif
