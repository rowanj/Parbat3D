#include "PchApp.h"

#include "CoordinateInfo.h"

#include "config.h"


/*
CoordinateInfo will return the correct geographic transform for a set of
data, if applicable.

It currently does nothing, but does it harmlessly. :)
*/

CoordinateInfo::CoordinateInfo(GDALDataset* dataset)
{
	GDALGetGeoTransform(dataset, geoTransform);	
}
		
CoordinateInfo::~CoordinateInfo(void)
{
	return;
}
