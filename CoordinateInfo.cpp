#include "config.h"

#if TMP_USE_IMAGE_FILE
#include "CoordinateInfo.h"

CoordinateInfo::CoordinateInfo(GDALDataset* dataset)
{
	GDALGetGeoTransform(dataset, geoTransform);	
}
		
CoordinateInfo::~CoordinateInfo(void)
{
	return;
}
#endif
