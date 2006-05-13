// Guard against multiple inclusion
#ifndef _CONFIG_H
#define _CONFIG_H

// Enable global debug/trace mode
#define DEBUG 1
//#undef DEBUG

// Declare some packages as optional during initial implementation.
//  This is to allow other packages to build for testing.
#define TMP_USE_IMAGE_FILE 1
#define TMP_USE_IMAGE_MANIPULATION 1
#undef TMP_USE_SETTINGS

// Platform-specific portions of code should test for the relevant PLATFORM_ define
#ifdef WIN32
#define PLATFORM_W32 1
#else
// 32-bit Windows is currently the only platform we support.
#error Unsupported platform detected.  See config.h
#endif

#endif
