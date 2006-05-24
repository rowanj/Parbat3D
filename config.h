// Guard against multiple inclusion
#ifndef _CONFIG_H
#define _CONFIG_H

// Enable global debug/trace mode
#define DEBUG 1
#define DEBUG_IMAGE_FILE 0
#define DEBUG_IMAGE_HANDLER 0
#define DEBUG_IMAGE_PROPERTIES 0
#define DEBUG_IMAGE_REDRAW 1
#define DEBUG_BANDS 0
#define DEBUG_IMAGE_DATA 1

// Declare some packages as optional during initial implementation.
//  This is to allow other packages to build for testing.
#define TMP_USE_SETTINGS 0
#define TMP_USE_TEXTURES 1
#define TMP_IGNOR_BUGS 1 // added by shane in order to run without crashing
                         // set to 1 (and TMP_USE_TEXTURES) to enable image displaying stuff
                         // set to 0 (and TMP_USE_TEXTURES) to disable image displaying stuff

// Platform-specific portions of code should test for the relevant PLATFORM_ define
#ifdef WIN32 // !! may not be defined by all compilers - Rowan
#define PLATFORM_W32 1
#else
// 32-bit Windows is currently the only platform we support.
#error Unsupported platform detected.  See config.h
#endif

#endif
