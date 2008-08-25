#include "PchApp.h"

#include "DisplayWindow.h"

#include "console.h"
#include "ImageHandler.h"
#include "ImageWindow.h"
#include "main.h"
#include "ROISet.h"
#include "ROIWindow.h"
#include "Settings.h"
#include "ToolWindow.h"
#include "Window.h"


DisplayWindow::DisplayWindow() {
    text="No Image Loaded";
    textLen=15;
    
    moving_viewport = false;  // not currently scrolling using the mouse
}


int DisplayWindow::Create(HWND hparent) {
    RECT rect;
    
    GetClientRect(hparent,&rect);    
    CreateWin(0, "Parbat3D Display Window", NULL, WS_CHILD+WS_VISIBLE,
        rect.left, rect.top, rect.right, rect.bottom, hparent, NULL);
    
	SetDefaultCursor(LoadCursor(NULL, IDC_CROSS));
	SetBackgroundBrush(NULL);
	prevProc=SetWindowProcedure(&WindowProcedure);
    
    // init variables used for painting "No Image Loaded" message
    hbrush=CreateSolidBrush(0);
    HDC hdc=GetDC(GetHandle());                                                                 /* get device context (drawing) object */
    hNormalFont=CreateFont(-MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72),0,0,0,400,false,false,false,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,"Tahoma"); //"MS Sans Serif" //Tahoma                
    SelectObject(hdc,hNormalFont);                                                   /* set font that will be used for drawing text */    
    GetTextExtentPoint32(hdc,text,textLen,&textSize);                                /* get width & height of string in pixels */          
    ReleaseDC(GetHandle(),hdc);
    
    GetClientRect(GetHandle(),&rect);
    textPos.x=(rect.right-rect.left)/2 - textSize.cx/2;
    textPos.y=(rect.bottom-rect.top)/2 - textSize.cy/2;

    return 0; // ?! is this right/necessary
}


// Handle messages/events related to the display window
LRESULT CALLBACK DisplayWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static PAINTSTRUCT ps;
    static HDC hdc;
    static RECT rect;
    int mx, my;             // mouse position
    int ix, iy;             // image position (based on the mouse position)
    
    DisplayWindow* win=(DisplayWindow*)Window::GetWindowObject(hwnd);
    
    
    switch (message) {
        case WM_SIZE:           
            if (image_handler) {
                /* Re-size OpenGL stuff */
                image_handler->resize_image_window();
       			/* update title bar text incase zoom level changes */
 	           imageWindow.updateImageWindowTitle();
            } else {
                /* re-position message */
                GetClientRect(hwnd,&rect);
                win->textPos.x=(rect.right-rect.left)/2 - win->textSize.cx/2;
                win->textPos.y=(rect.bottom-rect.top)/2 - win->textSize.cy/2;            
            }    
            break;
        
        
        case WM_MOUSEMOVE:
            // get the current mouse position
            mx = (short) LOWORD(lParam);
            my = (short) HIWORD(lParam);
            
            // check if an image is open and that mouse movement is in the image window (not the overview one)
            if (image_handler && win==&imageWindow.imageWindowDisplay) {
                // check if the query tab is displayed in the tool window
                if (toolWindow.hToolWindowCurrentTabContainer==toolWindow.queryTab.GetHandle()) {
                    unsigned char* bv;
                    
                    //Console::write("DisplayWindow WM_MOUSEMOVE\n mx=%d my=%d\n",mx,my);
                    
                    bv = image_handler->get_window_pixel_values(mx, my);
                    image_handler->get_image_viewport()->translate_window_to_image(mx, my, &ix, &iy);
                    
                    // if cursor is outside of image bounds then display 0,0 as coordinates
                    ImageProperties* ip = image_handler->get_image_properties();
                    if (ix>=0 && iy>=0 && ix<(ip->getWidth()) && iy<(ip->getHeight())) {
                        // Update display of cursor position
                        toolWindow.SetCursorPosition(ix,iy);
                        //SetWindowText(toolWindow.cursorXPos, );
                        //SetWindowText(toolWindow.cursorYPos, );
                        
                        // Update display of pixel values under query tab
                        if (bv && (ix!=0 || iy!=0)) { /* make sure the band values were returned */
                            for (int i=1; i<=toolWindow.bands; i++)
                                toolWindow.SetImageBandValue(i,bv[i-1]);
                                //SetWindowText(toolWindow.imageBandValues[i], (char *) makeMessage(leader, bv[i-1]));
                        }
                    }
                    delete[] bv;
                }
                
                // check if an ROI is currently being created
                if (regionsSet->editing()) {
    	            // update current mouse position (for drawing new ROIs)
                    image_handler->get_image_viewport()->translate_window_to_image(mx, my, &ix, &iy);
                    image_handler->set_mouse_position(ix,iy);
                    imageWindow.Repaint();
                }
                
                if (win->moving_viewport) {
                    // calculate the distance from the last mouse position to the current mouse position
                    int xd = win->prev_viewport_x - mx;
                    int yd = win->prev_viewport_y - my;
                    
                    // scroll the appropriate amount vertically and horizontally
                    if (xd != 0) imageWindow.scrollImage(false, xd);  // horizontal scroll
                    if (yd != 0) imageWindow.scrollImage(true, yd);   // vertical scroll
                    
                    // store the current viewport position as the previous (for next scroll message)
                    win->prev_viewport_x = mx;
                    win->prev_viewport_y = my;
                }
            }
            break;
        
        
        case WM_LBUTTONDOWN:
            // get position of mouse on screen
            mx = (short)LOWORD(lParam);
            my = (short)HIWORD(lParam);
            
            if ((image_handler) && (win==&imageWindow.imageWindowDisplay)) {
                // check if an ROI is currently being created
                if (regionsSet->editing()) {
                    // get position of mouse in image
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
                    
                // if nothing else is using mouse clicks then we can scroll
                } else {
                    // start scrolling
                    win->moving_viewport = true;
                    
                    // store the current mouse position to use for calculating scroll amount
                    win->prev_viewport_x = (short) LOWORD(lParam);
                    win->prev_viewport_y = (short) HIWORD(lParam);
                }
            }
            break;
        
        
        case WM_LBUTTONUP:
            // stop scrolling
            win->moving_viewport = false;
            break;
        
        // mouse left client area of the window - this doesn't seem to work for disabling the scrolling
//        case WM_MOUSELEAVE:
//            win->moving_viewport = false;
//            break;
        
        
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
