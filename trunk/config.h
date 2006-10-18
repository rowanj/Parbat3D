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


#define TMP_WAIT_FOR_KEYPRESS_ON_CLOSE 0 //wait for user to press a key in console window before closing on/off

#define _WIN32_IE 0x0600    // project compatible with comctrl32.dll up to v6.0
                            // required in order to use certain common controls functions & constants

#endif
