#ifndef PCH_APP_H
#define PCH_APP_H

/*
	This header provides program-wide declarations and imports.  It should only
	include system/third-party headers, as changes will require re-building all
	modules.
	
	Each .cpp file in the project should #include this file before any other
	includes, declarations, or code.
*/ 

// We use std::string all over the place, but other std:: namespaces should be
// explicit
#include <string>
using std::string;

// We use a fair bit of stuff from the C/C++ standard libraries
#include <cassert>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstdarg>
#include <cstdlib>
#include <cstring>

#include <sstream>
#include <iostream>   
#include <fstream>
#include <sys/types.h>

#include <limits>
#include <algorithm>


// C++ standard containers
#include <vector>
#include <list>
#include <queue>
#include <deque>

// OpenGL
#include <GL/gl.h>
#include <GL/glu.h>

// And Win32-API
#define WINVER 0x0500
#define _WIN32_IE 0x600
#define _WIN32_WINNT 0x501
//#define NOMINMAX
#include <windows.h>
#include <commctrl.h>

// And GDAL
//#define CPL_STDCALL __stdcall
//#define CPL_DISABLE_STDCALL 1
#include <gdal/gdal_priv.h>


#endif // include guard

