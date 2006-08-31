
#include <Windows.h>
#include "main.h"
#include "Window.h"
#include "FeatureSpace.h"
#include "GLContainer.h"
#include "Config.h"
#include "Console.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include <commctrl.h>

int FeatureSpace::numFeatureSpaces=0;

const int FeatureSpace::FEATURE_WINDOW_WIDTH=650;       // inital width of feature space window
const int FeatureSpace::FEATURE_WINDOW_HEIGHT=550;      // inital height of feature space window
const int FeatureSpace::TOOLBAR_HEIGHT=30;              // height of toolbar area at bottom of window


// timer used for testing
#include <time.h>   
void sleep(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

// create & display new feature space window
FeatureSpace::FeatureSpace(int LOD, bool only_ROIs)
{
    int createSuccess;
    
    createSuccess=Create();
    assert(createSuccess);
    
    glview=new GLView(glContainer->GetHandle());
    numFeatureSpaces++;

    // todo: setup feature space's data    
    Console::write("FeatureSpace::FeatureSpace Sleeping...\n");
    sleep(5000);       
    Show();
}

// draw contents of GLContainer with opengl
void FeatureSpace::PaintGLContainer()
{
	glview->make_current();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glview->GLswap();
} 
 
// create feature space window 
int FeatureSpace::Create()
{
    RECT rect;

    // get position of overview window for alignment   
    GetWindowRect(overviewWindow.GetHandle(),&rect);

    // create feature space window
    const char *title=makeMessage("Feature Space ",numFeatureSpaces+1);
    if (!CreateWin(0, "Parbat3D Feature Window", title,
	     WS_OVERLAPPEDWINDOW|WS_SYSMENU|WS_CAPTION|WS_SIZEBOX|WS_MAXIMIZEBOX|WS_MINIMIZEBOX|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
	     rect.right, rect.top, FEATURE_WINDOW_WIDTH, FEATURE_WINDOW_HEIGHT, NULL, NULL))
        return false;
    delete(title);
       
    // create child windows
    glContainer=new GLContainer(GetHandle(),this,0,0,FEATURE_WINDOW_WIDTH,FEATURE_WINDOW_HEIGHT-TOOLBAR_HEIGHT);       

    // create toolbar
    RECT rc;
    GetClientRect(GetHandle(),&rc);
    hToolbar=CreateWindowEx(0, TOOLBARCLASSNAME, (LPSTR) NULL, 
        WS_CHILD | CCS_ADJUSTABLE | WS_VISIBLE | CCS_NOPARENTALIGN | CCS_NORESIZE , 0, rc.bottom-TOOLBAR_HEIGHT,rc.right,TOOLBAR_HEIGHT, GetHandle(), 
        (HMENU) 1, GetAppInstance(), NULL);    
    
    SendMessage(hToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0);         

    TBBUTTON tbb[7];       
    const int NUM_BUTTONS=7;
    int i;
    
    char *buttonText[7]={"RL","RR","RU","RD",NULL,"ZI","ZO"};
    for (i=0;i<NUM_BUTTONS;i++)
    {
        if (buttonText[i]!=NULL)
        {
           tbb[i].iString= SendMessage(hToolbar, TB_ADDSTRING, 0, (LPARAM) (LPSTR) "A");     //buttonText[i]
           tbb[i].iBitmap = 0; 
           tbb[i].idCommand = i; 
           tbb[i].fsState = TBSTATE_ENABLED; 
           tbb[i].fsStyle = TBSTYLE_TOOLTIPS; //BTNS_BUTTON 
           tbb[i].dwData = 0; 
        }
        else
        {
            tbb[i].iString= 0;
            tbb[i].iBitmap = 0; 
            tbb[i].idCommand = -1; 
            tbb[i].fsState = TBSTATE_ENABLED; 
            tbb[i].fsStyle = BTNS_SEP;
            tbb[i].dwData = 0; 
        }
    }

    SendMessage(hToolbar, TB_ADDBUTTONS, (WPARAM) NUM_BUTTONS, (LPARAM) (LPTBBUTTON) &tbb);    
    SendMessage(hToolbar, TB_SETBUTTONSIZE, (WPARAM) 100, (LPARAM)30);        
    
//   SendMessage(hToolbar, TB_AUTOSIZE, 0, 0); 

   ShowWindow(hToolbar, SW_SHOW);    
   
   /*
   REBARINFO     rbi;
   REBARBANDINFO rbBand;
   RECT          rc;
   HWND   hwndCB, hwndTB;
   DWORD  dwBtnSize;

   hRebarControl = CreateWindowEx(WS_EX_CLIENTEDGE, //WS_EX_TOOLWINDOW
                           REBARCLASSNAME, //REBARCLASSNAME
                           "rebar",
                           WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|
                           WS_CLIPCHILDREN|RBS_VARHEIGHT|
                           CCS_NODIVIDER,
                           0,0,0,0,
                           GetHandle(),
                           NULL,
                           GetAppInstance(),
                           NULL);

   assert(hRebarControl!=NULL);

   // Initialize and send the REBARINFO structure.
   rbi.cbSize = sizeof(REBARINFO);  // Required when using this
                                    // structure.
   rbi.fMask  = 0;
   rbi.himl   = (HIMAGELIST)NULL;
   SendMessage(hRebarControl, RB_SETBARINFO, 0, (LPARAM)&rbi);

   // Initialize structure members that both bands will share.
   rbBand.cbSize = sizeof(REBARBANDINFO);  // Required
   rbBand.fMask  = RBBIM_COLORS | RBBIM_TEXT |  //| RBBIM_BACKGROUND
                   RBBIM_STYLE | RBBIM_CHILD  | RBBIM_CHILDSIZE | 
                   RBBIM_SIZE;
   rbBand.fStyle = RBBS_CHILDEDGE | RBBS_FIXEDBMP;
   rbBand.hbmBack=NULL;

   // Create the combo box control to be added.
   hwndCB = CreateWindowEx( 0, szStaticControl, "test", 
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 0,
		0, 100, 20, hRebarControl, NULL,
		GetAppInstance(), NULL);
   // Set values unique to the band with the combo box.
   GetWindowRect(hwndCB, &rc);
   rbBand.lpText     = "Combo Box";
   rbBand.hwndChild  = hwndCB;
   rbBand.cxMinChild = 0;
   rbBand.cyMinChild = rc.bottom - rc.top;
   rbBand.cx         = 200;
   
   // Add the band that has the combo box.
   SendMessage(hRebarControl, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);
*/   


    // make this window snap to others
    stickyWindowManager.AddStickyWindow(this);

    // handle events for the window
    prevProc=SetWindowProcedure(&WindowProcedure);	
    
    
    OnResize();    
    return true;    
}
 
// resize GLContainer to fit the feature space window
void FeatureSpace::OnResize()
{
    RECT rect;
    GetClientRect(GetHandle(),&rect);
    MoveWindow(glContainer->GetHandle(),rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top-TOOLBAR_HEIGHT, true);
    MoveWindow(hToolbar,rect.left,rect.bottom-TOOLBAR_HEIGHT,rect.right,TOOLBAR_HEIGHT, true);    

}
 
/* handle events related to the feature space window */
LRESULT CALLBACK FeatureSpace::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{   

    FeatureSpace* win=(FeatureSpace*)Window::GetWindowObject(hwnd);
    
    
    switch (message)                  /* handle the messages */
    {			
        /* WM_DESTORY: system is destroying our window */
        case WM_SIZE:
            win->OnResize();
            break;
        case WM_DESTROY:
            stickyWindowManager.RemoveStickyWindow(win);
            break;
    }
    /* call the next procedure in the chain */       
    return CallWindowProc(win->prevProc,hwnd,message,wParam,lParam);    
}
