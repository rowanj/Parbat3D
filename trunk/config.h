// Guard against multiple inclusion
#ifndef _CONFIG_H
#define _CONFIG_H

// Enable global debug/trace mode
#define DEBUG 0
#define DEBUG_IMAGE_FILE 0
#define DEBUG_IMAGE_PROPERTIES 0
#define DEBUG_BANDS 0
#define DEBUG_IMAGE_DATA 0
#define DEBUG_TOOLWINDOW 0
#define DEBUG_WINDOW 0
#define DEBUG_SCROLLING 0
#define DEBUG_STICKY_WINDOW_MANAGER 0

// Declare some packages as optional during initial implementation.
//  This is to allow other packages to build for testing.
#define TMP_USE_SETTINGS 0
#define TMP_WAIT_FOR_KEYPRESS_ON_CLOSE 0 //wait for user to press a key in console window before closing on/off
#define TMP_DISABLE_FEATURES_FOR_DEMO_DAY	//Disable some features for demo day release. Comment out line to re-enable features.

#define _WIN32_IE 0x0600    // project compatible with comctrl32.dll up to v6.0
                            // required in order to use certain common controls functions & constants

// Platform-specific portions of code should test for the relevant PLATFORM_ define
#ifdef WIN32 // !! may not be defined by all compilers - Rowan
#define PLATFORM_W32 1
#else
// 32-bit Windows is currently the only platform we support.
#error Unsupported platform detected.  See config.h
#endif

#endif
