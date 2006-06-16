#include "config.h"
#include "CoordinateInfo.h"

CoordinateInfo::CoordinateInfo(GDALDataset* dataset)
{
	GDALGetGeoTransform(dataset, geoTransform);	
}
		
CoordinateInfo::~CoordinateInfo(void)
{
	return;
}
