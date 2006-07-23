#ifndef _PARBAT_MAIN_H
#define _PARBAT_MAIN_H

#include "resources.h"
#include "Settings.h"
#include "ImageHandler.h"

extern ImageHandler::ImageHandler* image_handler;
extern char szStaticControl[];
extern HINSTANCE hThisInstance;
extern HWND hDesktop;
extern settings winPos;
extern char *filename;
enum {DISPLAY_TAB_ID,QUERY_TAB_ID,IMAGE_TAB_ID};



/* function declarations */

void loadFile();
void closeFile();
void orderWindows();


int isWindowInNormalState(HWND hwnd);
int toggleMenuItemTick(HMENU hMenu,int itemId);

void updateImageScrollbar();
void updateImageWindowTitle();
void updateToolWindowScrollbar();
void scrollToolWindowToTop();

#endif
