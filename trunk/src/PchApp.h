#ifndef PCH_APP_H
#define PCH_APP_H

/*
	This header provides program-wide declarations and imports.  It should only
	include system/third-party headers, as changes will require re-building all
	modules.
	
	Each .cpp file in the project should #include this file before any other
	includes, declarations, or code.
*/ 

#include <gtkmm.h>

#ifdef __WINE__
#define __stdcall __attribute__((ms_abi)) 
#define __cdecl __attribute__((ms_abi)) 
#define _stdcall __attribute__((ms_abi)) 
#define _cdecl __attribute__((ms_abi)) 
#define __fastcall __attribute__((ms_abi)) 
#define _fastcall __attribute__((ms_abi)) 
#define __declspec(x) __declspec_##x 
#define __declspec_align(x) __attribute__((aligned(x))) 
#define __declspec_allocate(x) __attribute__((section(x))) 
#define __declspec_deprecated __attribute__((deprecated)) 
#define __declspec_dllimport __attribute__((dllimport)) 
#define __declspec_dllexport __attribute__((dllexport)) 
#define __declspec_naked __attribute__((naked)) 
#define __declspec_noinline __attribute__((noinline)) 
#define __declspec_noreturn __attribute__((noreturn)) 
#define __declspec_nothrow __attribute__((nothrow)) 
#define __declspec_novtable __attribute__(()) 
#define __declspec_selectany __attribute__((weak)) 
#define __declspec_thread __thread 
#define __int8 char 
#define __int16 short
#define __int32 int 
#define __int64 long 

#endif

#define WIN64 
#define _WIN64 
#define __WIN64
#define __WIN64__ 
#define WIN32 
#define _WIN32 
#define __WIN32 
#define __WIN32__ 
#define __WINNT 
#define __WINNT__

//#endif



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

