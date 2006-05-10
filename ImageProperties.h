#ifndef _IMAGE_PROPERTIES_H
#define _IMAGE_PROPERTIES_H
#if TMP_USE_IMAGE_FILE
#include <iosfwd>
#include <cstdio>
#include <math.h>
#include <gdal_priv.h>

#include "config.h"
#include <string>
#if PLATFORM_W32
#include <windows.h>
#endif


class ImageProperties
{
	public:
		ImageProperties(void);
		virtual ~ImageProperties(void);
};
#endif


#endif
