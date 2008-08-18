#ifndef _COORDINATE_INFO_H
#define _COORDINATE_INFO_H

class CoordinateInfo
{
	public:
		CoordinateInfo(GDALDataset* dataset);
		virtual ~CoordinateInfo(void);
		
	private:
		double geoTransform[6];
};
#endif

