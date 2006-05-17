#ifndef _IMAGE_DATA_H
#define _IMAGE_DATA_H

#include <iosfwd>
#include <cstdio>
#include <math.h>

#define CPL_STDCALL __stdcall
#define CPL_DISABLE_STDCALL 1
#include <gdal_priv.h>

#include "config.h"
#include <string>
#include "StringUtils.h"
#if PLATFORM_W32
#include <windows.h>
#endif //PLATFORM_W32

class ImageData
{
	public:
		ImageData(void);
		virtual ~ImageData(void);
};

#endif

