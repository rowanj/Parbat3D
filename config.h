// Guard against multiple inclusion
#ifndef _CONFIG_H
#define _CONFIG_H

// Enable global debug/trace mode
#define DEBUG 1
#define DEBUG_IMAGE_FILE 0
#define DEBUG_IMAGE_HANDLER 0
#define DEBUG_IMAGE_PROPERTIES 0
//#undef DEBUG

// Declare some packages as optional during initial implementation.
//  This is to allow other packages to build for testing.
#define TMP_USE_IMAGE_FILE 1
#define TMP_USE_IMAGE_MANIPULATION 1
#define TMP_USE_SETTINGS 0
#define TMP_USE_OO_OPENGL 0

// Platform-specific portions of code should test for the relevant PLATFORM_ define
#ifdef WIN32
#define PLATFORM_W32 1
#else
// 32-bit Windows is currently the only platform we support.
#error Unsupported platform detected.  See config.h
#endif

#endif
