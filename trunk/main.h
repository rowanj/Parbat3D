#ifndef _PARBAT_MAIN_H
#define _PARBAT_MAIN_H

#include "resources.h"
#include "Settings.h"
#include "ImageHandler.h"
#include "MainWindow.h"

extern ImageHandler::ImageHandler* image_handler;
extern char szStaticControl[];
extern HINSTANCE hThisInstance;
extern HWND hDesktop;
extern settings settingsFile;
extern char *filename;
extern char *modulePath;
extern MainWindow mainWindow;
enum {DISPLAY_TAB_ID,QUERY_TAB_ID,IMAGE_TAB_ID};



/* function declarations */

void loadFile();
void closeFile();
void orderWindows();



#endif
