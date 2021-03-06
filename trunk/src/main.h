#ifndef _PARBAT_MAIN_H
#define _PARBAT_MAIN_H

#include "resources.h"
#include "Settings.h"
#include "ImageHandler.h"
#include "MainWindow.h"
#include "ToolWindow.h"
#include "OverviewWindow.h"
#include "ImageWindow.h"
#include "ROIWindow.h"
#include "PrefsWindow.h"
#include "ProgressWindow.h"
#include "ContrastWindow.h"
#include "ContrastAdvWindow.h"
#include "StickyWindowManager.h"
#include "FeatureSpace.h"

extern ImageHandler::ImageHandler* image_handler;
extern char szStaticControl[];
extern HINSTANCE hThisInstance;
extern HWND hDesktop;
extern settings* settingsFile;
extern char *filename;
extern char *modulePath;
extern MainWindow mainWindow;
extern ToolWindow toolWindow;
extern ImageWindow imageWindow;
extern OverviewWindow overviewWindow;
extern ROIWindow roiWindow;
extern PrefsWindow prefsWindow;
extern ContrastWindow contrastWindow;
extern ContrastAdvWindow contrastAdvWindow;
extern ProgressWindow progressWindow;

extern StickyWindowManager stickyWindowManager;
extern vector<FeatureSpace*> featureSpaceWindows;

enum {DISPLAY_TAB_ID,QUERY_TAB_ID,IMAGE_TAB_ID,FEATURE_TAB_ID};



/* function declarations */

void loadFile();
void closeFile();
void orderWindows();
void HandleWindowMessagesInBackground();
void emptyOutMessageQueue();

#endif
