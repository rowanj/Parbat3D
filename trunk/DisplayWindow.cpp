#include <Windows.h>

#include "console.h"
#include "DisplayWindow.h"
#include "ImageHandler.h"
#include "ImageWindow.h"
#include "main.h"
#include "ROISet.h"
#include "ROIWindow.h"
#include "Settings.h"
#include "ToolWindow.h"
#include "Window.h"

DisplayWindow::DisplayWindow()
{
    text="No Image Loaded";
    textLen=15;
}

int DisplayWindow::Create(HWND hparent)
{
    RECT rect;
    
    GetClientRect(hparent,&rect);    
    CreateWin(0, "Parbat3D Display Window", NULL, WS_CHILD+WS_VISIBLE,
		rect.left, rect.top, rect.right, rect.bottom, hparent, NULL);    

	SetDefaultCursor(LoadCursor(NULL, IDC_CROSS));
	SetBackgroundBrush(NULL);
	prevProc=SetWindowProcedure(&WindowProcedure);
	
    /* init variables used for painting "No Image Loaded" message */
    hbrush=CreateSolidBrush(0);
    HDC hdc=GetDC(GetHandle());                                                                 /* get device context (drawing) object */
    hNormalFont=CreateFont(-MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72),0,0,0,400,false,false,false,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,"Tahoma"); //"MS Sans Serif" //Tahoma                
    SelectObject(hdc,hNormalFont);                                                   /* set font that will be used for drawing text */    
    GetTextExtentPoint32(hdc,text,textLen,&textSize);                                /* get width & height of string in pixels */          
    ReleaseDC(GetHandle(),hdc);

    GetClientRect(GetHandle(),&rect);
    textPos.x=(rect.right-rect.left)/2 - textSize.cx/2;
    textPos.y=(rect.bottom-rect.top)/2 - textSize.cy/2;            
    
	
}


/* Handle messages/events related to the display window */
LRESULT CALLBACK DisplayWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static PAINTSTRUCT ps;
    static HDC hdc;
    static RECT rect;

    DisplayWindow* win=(DisplayWindow*)Window::GetWindowObject(hwnd);
    
    switch (message) {
        case WM_SIZE:           
            if (image_handler) {
                /* Re-size OpenGL stuff */
                image_handler->resize_image_window();
            } else {
                /* re-position message */
                GetClientRect(hwnd,&rect);
                win->textPos.x=(rect.right-rect.left)/2 - win->textSize.cx/2;
                win->textPos.y=(rect.bottom-rect.top)/2 - win->textSize.cy/2;            
            }    
            break;
        
        
        case WM_MOUSEMOVE:
            /* check if image is open, mouse has moved on image window & the query tab is displayed in the tool window */
            if ((image_handler)&&(win==&imageWindow.imageWindowDisplay)&&(toolWindow.hToolWindowCurrentTabContainer==toolWindow.queryTab.GetHandle())) {
                /* Get mouse screen position */
                int mx = (short)LOWORD(lParam);
                int my = (short)HIWORD(lParam);
                int ix, iy;
                unsigned int* bv;
                
                bv = image_handler->get_window_pixel_values(mx, my);
                image_handler->get_image_viewport()->translate_window_to_image(mx, my, &ix, &iy);
                
                /* if cursor is outside of image bounds then display 0,0 as coordinates */
                ImageProperties* ip = image_handler->get_image_properties();
                if (ix>=0 && iy>=0 && ix<(ip->getWidth()) && iy<(ip->getHeight())) {

                    /* Update display of cursor position */
                    toolWindow.SetCursorPosition(ix,iy);
                    //SetWindowText(toolWindow.cursorXPos, );
                    //SetWindowText(toolWindow.cursorYPos, );
                    
                    /* Update display of pixel values under query tab */                    
                    if (bv && (ix!=0 || iy!=0)) { /* make sure the band values were returned */
                        for (int i=1; i<=toolWindow.bands; i++)
                            toolWindow.SetImageBandValue(i,bv[i-1]);
                            //SetWindowText(toolWindow.imageBandValues[i], (char *) makeMessage(leader, bv[i-1]));
                    }
                }
                delete[] bv;
            }
            if ((image_handler) && (win==&imageWindow.imageWindowDisplay) && regionsSet->editing()) {
                imageWindow.Repaint();           
            }
            break;
        
        
        case WM_LBUTTONDOWN:
            if ((image_handler) && (win==&imageWindow.imageWindowDisplay) && regionsSet->editing()) {
                // get position of mouse on screen
                int mx = (short)LOWORD(lParam);
                int my = (short)HIWORD(lParam);
                
                // get position of mouse in image
                int ix, iy;
                image_handler->get_image_viewport()->translate_window_to_image(mx, my, &ix, &iy);
                
                // check if cursor is outside of image bounds
                ImageProperties* ip = image_handler->get_image_properties();
                if (ix>=0 && iy>=0 && ix<(ip->getWidth()) && iy<(ip->getHeight())) {
                    regionsSet->add_point(ix, iy);
                    //regionsSet->add_entity_to_current();
                    ROIWindow* roiwin = (ROIWindow*)Window::GetWindowObject(roiWindow.GetHandle());
                    roiwin->updateButtons(roiwin);
                    imageWindow.Repaint();                    
                }
            }
            break;
        
        
        case WM_LBUTTONDBLCLK:
            if ((image_handler) && (win==&imageWindow.imageWindowDisplay) && (regionsSet->editingType()==ROI_POLY)) {
                regionsSet->finish_entity(true);
                ROIWindow* roiwin = (ROIWindow*)Window::GetWindowObject(roiWindow.GetHandle());
                roiwin->updateButtons(roiwin);
                imageWindow.Repaint();
            }
            break;
        
        
        case WM_RBUTTONDOWN:
            if ((image_handler) && (win==&imageWindow.imageWindowDisplay) && regionsSet->editing()) {
                regionsSet->backtrack();
                ROIWindow* roiwin = (ROIWindow*)Window::GetWindowObject(roiWindow.GetHandle());
                roiwin->updateButtons(roiwin);
                imageWindow.Repaint();                
            }
            break;
        
        
        case WM_DESTROY:
            DeleteObject(win->hNormalFont);
            DeleteObject(win->hbrush); 
            return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
        
        case WM_ERASEBKGND:
            
            return 0;
                
		case WM_PAINT:
            static int p;
            hdc=BeginPaint(hwnd,&ps);
			if (image_handler!=NULL)
    			/* Re-draw OpenGL stuff */
                image_handler->redraw();
            else
            {
                /* Display "No Image Loaded" on black background */
                GetClientRect(hwnd,&rect);                
                SelectObject(hdc,win->hbrush);
                Rectangle(hdc,0,0,rect.right,rect.bottom);
                SelectObject(hdc,win->hNormalFont);
                SetTextColor(hdc,RGB(255,255,255));
                SetBkColor(hdc,0);
                TextOut(hdc,win->textPos.x,win->textPos.y,win->text,win->textLen);
            }
			EndPaint(hwnd,&ps);
			break;

    default:                   /* for messages that we don't deal with */
            /* let windows peform the default operation based on the message */
            return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
    }			
    /* return true to indicate that we have processed the message */    
    return 0; 
}
