#include <Windows.h>
#include "Settings.h"
#include "DisplayWindow.h"
#include "ImageHandler.h"
#include "ToolWindow.h"
#include "ImageWindow.h"
#include "main.h"

char DisplayWindow::szDisplayClassName[] = "Parbat3D Display Window";

/* register a class that can be used to create a display window */
int DisplayWindow::registerWindow()
{
    WNDCLASSEX wincl;
    
    /* Create window class for the display control windows */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szDisplayClassName;
    wincl.lpfnWndProc = DisplayWindow::WindowProcedure;  /* This function is called by windows */
    wincl.style = CS_DBLCLKS;  /* Ctach double-clicks */
    wincl.cbSize = sizeof(WNDCLASSEX);
    /* Use default icon and mousepointer */
    wincl.hIcon = NULL;
    wincl.hIconSm = NULL;
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL; /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    wincl.hbrBackground = NULL;
    /* Register the window class, if fails return false */
    return RegisterClassEx(&wincl);
}    

/* Handle messages/events related to the display window */
LRESULT CALLBACK DisplayWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static PAINTSTRUCT ps;
    static HDC hdc;
    static RECT rect;
    static HBRUSH hbrush;
    static SIZE textSize;
    static POINT textPos;
    static char text[]="No Image Loaded";
    const int textLen=15;
    
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            /* init variables used for painting "No Image Loaded" message */
            hbrush=CreateSolidBrush(0);
            hdc=GetDC(hwnd);                                                                      /* get device context (drawing) object */
            SelectObject(hdc,ToolWindow::hNormalFont);                                                   /* set font that will be used for drawing text */    
            GetTextExtentPoint32(hdc,text,textLen,&textSize); /* get width & height of string in pixels */   
            ReleaseDC(hwnd,hdc);
            break;
            
        case WM_SIZE:           
            if (image_handler) {
                /* Re-size OpenGL stuff */
                image_handler->resize_window();
            } else {
                /* re-position message */
                GetClientRect(hwnd,&rect);
                textPos.x=(rect.right-rect.left)/2 - textSize.cx/2;
                textPos.y=(rect.bottom-rect.top)/2 - textSize.cy/2;            
            }    
            break;

        case WM_MOUSEMOVE:
            {
                /* check if image is open, mouse has moved on image window & the query tab is displayed in the tool window */
                if ((image_handler)&&(hwnd==ImageWindow::hImageWindowDisplay)&&(ToolWindow::hToolWindowCurrentTabContainer==ToolWindow::hToolWindowQueryTabContainer)) {
                    /* Get mouse screen position */
                    int mx = (short)LOWORD(lParam);
                    int my = (short)HIWORD(lParam);
                    
                    /* Convert screen position to image position */
                    int vx = image_handler->get_viewport_x(); /* get top right corner of screen position in image */
                    int vy = image_handler->get_viewport_y();
                    int lod = (int)pow((double)2,(double)image_handler->get_LOD());
                    int ix = (mx + vx) * lod;
                    int iy = (my + vy) * lod;
                    
                    /* if cursor is outside of image bounds then display 0,0 as coordinates */
                    ImageProperties* ip = image_handler->get_image_properties();
                    if (ix<0 || iy<0 || ix>(ip->getWidth()) || iy>=(ip->getHeight())) {
                        ix = 0;
                        iy = 0;
                    }
                    
                    /* Get band values */
                    unsigned int* bv = image_handler->get_pixel_values_viewport(mx, my);
                    
                    string leader = "";
                    
                    /* Update display of cursor position */
                    SetWindowText(ToolWindow::cursorXPos, (char *) makeMessage(leader, ix));
                    SetWindowText(ToolWindow::cursorYPos, (char *) makeMessage(leader, iy));

                    /* Update display of pixel values under query tab */                    
                    if (bv) { /* make sure the band values were returned */
                        for (int i=1; i<=ToolWindow::bands; i++)
                            SetWindowText(ToolWindow::imageBandValues[i], (char *) makeMessage(leader, bv[i-1]));
                    }
                    
                    delete[] bv;
                }
            }
            break;
            
		case WM_PAINT:
            hdc=BeginPaint(hwnd,&ps);
			if (image_handler)
    			/* Re-draw OpenGL stuff */
                image_handler->redraw();
            else
            {
                /* Display "No Image Loaded" on black background */
                SelectObject(hdc,hbrush);
                Rectangle(hdc,0,0,rect.right,rect.bottom);
                SelectObject(hdc,ToolWindow::hNormalFont);
                SetTextColor(hdc,RGB(255,255,255));
                SetBkColor(hdc,0);
                TextOut(hdc,textPos.x,textPos.y,text,textLen);
            }
			EndPaint(hwnd,&ps);
			break;

    default:                   /* for messages that we don't deal with */
            /* let windows peform the default operation based on the message */
            return DefWindowProc(hwnd, message, wParam, lParam);
    }			
    /* return true to indicate that we have processed the message */    
    return 0; 
}
