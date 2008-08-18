#include "PchApp.h"

#include "ImageTab.h"

#include "Config.h"

#include "Window.h"
#include "main.h"
#include "ScrollBox.h"
#include "ToolTab.h"
#include "ToolWindow.h"


int ImageTab::GetContainerHeight()
{
    return 0;
}

int ImageTab::Create(HWND parent,RECT *parentRect)
{
    ToolTab::Create(parent,parentRect);
    
	/* add the image property information under the image tab */
	ImageProperties* ip=image_handler->get_image_properties();
	string leader;
	int ipItems=5;
	string n[ipItems];
	string v[ipItems];
	
	/* If the filename is too long to be displayed, truncate it.
	Later on, a roll-over tooltip should be implemented to bring
	up the full name.*/
	string fullname = ip->getFileName();
	string fname, bname, finalname;
	if (fullname.length() > 25) {
		fname = fullname.substr(0, 12);
		bname = fullname.substr(fullname.length()-12, fullname.length()-1);
		finalname = fname + "…" + bname;
	} else
	    finalname = fullname;
	    
	string drivername = ip->getDriverLongName();
	string finaldrivname;
	if (drivername.length() > 25) {
		fname = drivername.substr(0, 12);
		bname = drivername.substr(drivername.length()-12, drivername.length()-1);
		finaldrivname = fname + "…" + bname;
	} else
	    finaldrivname = drivername;
	
	    
	n[0]="File Name"; v[0]=makeMessage(leader, (char*) finalname.c_str());
	n[1]="File Type"; v[1]=makeMessage(leader, (char*) finaldrivname.c_str());
	n[2]="Width"; v[2]=makeMessage(leader, ip->getWidth());
	n[3]="Height"; v[3]=makeMessage(leader, ip->getHeight());
	n[4]="Bands"; v[4]=makeMessage(leader, ip->getNumBands());
	
	HWND hstatic;
	for (int i=0; i<ipItems; i++) {
		hstatic=CreateWindowEx(0, szStaticControl, (char*) n[i].c_str(),
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 20, 40+(i*20), 50, 18,
			GetHandle(), NULL, Window::GetAppInstance(), NULL);
		SetFont(hstatic,Window::FONT_NORMAL);
		
		hstatic=CreateWindowEx(0, szStaticControl, (char*) v[i].c_str(),
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 78, 40+(i*20), 160, 18,
			GetHandle(), NULL, Window::GetAppInstance(), NULL);
		SetFont(hstatic,Window::FONT_NORMAL);
    }
    

}


