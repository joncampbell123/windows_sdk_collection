//--------------------------------------------------------------------------;
//
//  File: memtime.cpp
//
//  Description:
//      This code times various things about system memory and video
//      memory. It is intended to show the advantages and disadvantages
//      of different drawing methoids.
//      Please refer to the documented for a more detailed discriptions of
//      what is going on.
//
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//---------------------------------------------------------------------------
//
//  Copyright (c) 1994 - 1995 Microsoft Corporation.  All Rights Reserved.
//
//---------------------------------------------------------------------------

#include<windows.h>
#include<windowsx.h>
#include<commdlg.h>
#include<strstrea.h>
#include<iomanip.h>
#include <ddraw.h>

#include"memtime.h"

/*----------------------------------------------------------------------------*\
|                                                                                                                                                          |
|   g l o b a l   v a r i a b l e s                                                                                    |
|                                                                                                                                                          |
\*----------------------------------------------------------------------------*/
static  char    szAppName[]="Direct Draw video/system memory timing tests";

static HINSTANCE    hInstApp;
static HWND         hwndApp;
static HACCEL       hAccelApp;
static HPALETTE     hpalApp;

static DWORD ScreenHeight, ScreenWidth;
static PCHAR pSource, pDest;

static LPDIRECTDRAW pDirectDrawObj;
static LPDIRECTDRAWSURFACE pOnScreenBuf;
static LPDIRECTDRAWSURFACE pOffScreenBuf;
static LPDIRECTDRAWSURFACE pSysMemBuf;

static ULONG ModeXFrameCount;
static ULONG ModeYFrameCount;
static ULONG StretchFrameCount;

/*----------------------------------------------------------------------------

Timers

*/

extern "C" MemFunc DwordMemCopy_Pitch;
extern "C" MemFunc ByteMemCopy_Pitch;

extern "C" MemFunc DwordMemFill_Pitch;
extern "C" MemFunc ByteMemFill_Pitch;

extern "C" MemFunc VertMemSto_Pitch;


TIMING_RESULT SystemTests[]=
{
    {0, DwordMemCopy_Pitch, "System to System dword copy"},
    {0, ByteMemCopy_Pitch, "System to System byte copy"},

    {0, DwordMemFill_Pitch, "System dword Fill"},
    {0, ByteMemFill_Pitch, "System byte Fill"},

    {0, VertMemSto_Pitch, "System vertical byte fill"}
};


TIMING_RESULT VideoTests[]=
{
    {0, DwordMemCopy_Pitch, "System to Video dword Copy"},
    {0, ByteMemCopy_Pitch, "System to Video byte Copy"},

    {0, DwordMemFill_Pitch, "Video dword Fill"},
    {0, ByteMemFill_Pitch, "Video byte Fill"},

    {0, VertMemSto_Pitch, "Video vertical byte fill"}
};




/*----------------------------------------------------------------------------

defines

*/

#define NUMBER_OF(aa) (sizeof(aa)/sizeof((aa)[0]))
        

/*----------------------------------------------------------------------------*\
|
|   f u n c t i o n   d e f i n i t i o n s
|
\*----------------------------------------------------------------------------*/

LRESULT CALLBACK AppWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
int  ErrMsg (LPSTR sz,...);
LONG AppCommand (HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
void PrintTimingResults( void );

void AppExit(void);
BOOL AppIdle(void);
BOOL DirectDrawInit();
VOID DoSystemTest( PTIMING_RESULT pTimeObject );
VOID DoVideoTests( PTIMING_RESULT pTimeObject );
ULONG GetFPS( int width, int height );
ULONG GetStretchFPS( int width, int height );

/*----------------------------------------------------------------------------*\
|   AppAbout( hDlg, uiMessage, wParam, lParam )                             
|
|   Description:                                                            
|       This function handles messages belonging to the "About" dialog box.
|       The only message that it looks for is WM_COMMAND, indicating the use
|       has pressed the "OK" button.  When this happens, it takes down
|       the dialog box.                                               
|                                                                             
|   Arguments:                                                            
|       hDlg        window handle of about dialog window  
|       uiMessage   message number                        
|       wParam      message-dependent                     
|       lParam      message-dependent                     
|                                                                             
|   Returns:                                                              
|       TRUE if message has been processed, else FALSE                
|                                                                             
\*----------------------------------------------------------------------------*/
BOOL CALLBACK
AppAbout( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch (msg) {
        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK) {
                EndDialog(hwnd,TRUE);
            }
            break;
        case WM_INITDIALOG:
            return TRUE;
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*\
|   AppInit( hInst, hPrev)                                                  
|
|   Description:                                                            
|       This is called when the application is first loaded into        
|       memory.  It performs all initialization that doesn't need to be done
|       once per instance.                                              
|                                                                               
|   Arguments:                                                              
|       hInstance               instance handle of current instance     
|       hPrev                   instance handle of previous instance    
|                                                                               
|   Returns:                                                                
|       TRUE if successful, FALSE if not                                
|                                                                               
\*----------------------------------------------------------------------------*/
BOOL
AppInit(HINSTANCE hInst, HINSTANCE hPrev, int sw, LPSTR szCmdLine )
{
    WNDCLASS cls;
    HRESULT ddrval;
    DDSURFACEDESC ddsd;
    BOOL returnValue = FALSE;
    
    /* Save instance handle for DialogBoxs */
    hInstApp = hInst;
    
    hAccelApp = LoadAccelerators(hInst, "AppAccel");
    
    if (!hPrev) {
        /*
         *      Register a class for the main application window
         */
        cls.hCursor        = LoadCursor(NULL,IDC_ARROW);
        cls.hIcon                  = LoadIcon(hInst,"AppIcon");
        cls.lpszMenuName   = "AppMenu";
        cls.lpszClassName  = szAppName;
        cls.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
        cls.hInstance      = hInst;
        cls.style                  = CS_BYTEALIGNCLIENT | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
        cls.lpfnWndProc    = (WNDPROC)AppWndProc;
        cls.cbWndExtra     = 0;
        cls.cbClsExtra     = 0;
        
        if (RegisterClass(&cls)) {
            hwndApp = CreateWindow (
                        szAppName,      // Class name
                        szAppName,      // Caption
                        WS_OVERLAPPEDWINDOW,        // Style bits
                        50, 50,         // Position
                        400+20,400+75,  // Size
                        (HWND)NULL,     // Parent window (no parent)
                        (HMENU)NULL,    // use class menu
                        hInst,          // handle to window instance
                        (LPSTR)NULL);   // no params to pass on
            ShowWindow(hwndApp,sw);

            // init our direct draw stuff
            ddrval = DirectDrawCreate( NULL, &pDirectDrawObj, NULL );
            if( DD_OK == ddrval) {
                // clear out the surface desctiptions
                ZeroMemory( &ddsd, sizeof( ddsd ) );
                // set the size
                ddsd.dwSize = sizeof( ddsd );

                // figure out what display mode we are in now
                ddrval = pDirectDrawObj->GetDisplayMode( &ddsd );
                if( DD_OK == ddrval ) {
                    // make sure we're in  a 8-bit mode
                    if( ddsd.ddpfPixelFormat.dwRGBBitCount >= 8) {
                        ScreenHeight = ddsd.dwHeight;
                        ScreenWidth = ddsd.dwWidth;
        
                        // if we are biger than 800x600 it's to much memory
                        // to move around. Clip it
                        if(ScreenHeight > 600) ScreenHeight = 600;
                        if(ScreenWidth > 800) ScreenWidth = 800;

                        if( (ScreenWidth < 800) || (ScreenHeight < 600) ) {
                            ErrMsg("Resolutions smaller than 800x600 may yeild different numbers because of L2 cache effects");
                        }
                        
                        // allocate some memory we are going to play with
                        pSource = (PCHAR)GlobalAllocPtr(GHND, ScreenHeight * ScreenWidth);
                        pDest = (PCHAR)GlobalAllocPtr(GHND, ScreenHeight * ScreenWidth);

                        // make sure the memory worked and it's dword alligned.
                        // (we're in Win32, when is not going to be dword alligned?)
                        if( pSource && pDest && !((DWORD)pSource & 0x3) && !((DWORD)pDest & 0x3)) {
                            returnValue = TRUE;
                        } else {
                            ErrMsg("Memory allocation failure");
                        }
                    } else {
                        ErrMsg("You must be in a 8-bit (or better) color mode to run this app");
                    }
                } else {
                    ErrMsg("DirectDraw failure getting the display mode");
                }
            } else {
                ErrMsg("DirectDraw create failure");
            }
        } else {
            ErrMsg("Register Class failure");
        }
    }
    return returnValue;
}

/*----------------------------------------------------------------------------*\
|   BandwidthTestsSetup
|
|   Description:
|       Setup the surfaces for the bandwidth tests
|
|   Returns:                                                                
|       TRUE if successful, FALSE if not                                
\*----------------------------------------------------------------------------*/
BOOL
BandwidthTestsSetup( VOID )
{
    DDSURFACEDESC ddsd;
    BOOL returnValue = FALSE;
    HRESULT ddrval;

    if( pOnScreenBuf ) pOnScreenBuf->Release();
    if( pOffScreenBuf ) pOffScreenBuf->Release();
    if( pSysMemBuf ) pSysMemBuf->Release();
    
    // Tell the world we love it and want to share
    ddrval = pDirectDrawObj->SetCooperativeLevel( hwndApp, DDSCL_NORMAL );
    if( DD_OK == ddrval ) {
        // clear out out stuff
        ZeroMemory( &ddsd, sizeof( ddsd ) );
        ddsd.dwSize = sizeof( ddsd );

        ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
        ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

        ddsd.dwHeight = ScreenHeight;
        ddsd.dwWidth = ScreenWidth;
        // create our off screen video memory.
        ddrval = pDirectDrawObj->CreateSurface( &ddsd, &pOffScreenBuf, NULL );
        if( DD_OK == ddrval ) {
            // yay!
            returnValue = TRUE;
        } else {
            ErrMsg("Failure creating video surface");
        }
    } else {
        ErrMsg("Failure setting Co-op mode");
    }
    return returnValue;
}
/*----------------------------------------------------------------------------*\
|   AppExit()
|
|   Description:
|       app is just about to exit, cleanup
|
\*----------------------------------------------------------------------------*/
void AppExit()
{
    if( pSource ) GlobalFreePtr(pSource);
    if( pDest ) GlobalFreePtr(pDest);
    if( pOffScreenBuf ) pOffScreenBuf->Release();
    if( pOnScreenBuf ) pOnScreenBuf->Release();
    if( pSysMemBuf ) pSysMemBuf->Release();
    if( pDirectDrawObj ) pDirectDrawObj->Release();

}

/*----------------------------------------------------------------------------*\
|   WinMain( hInst, hPrev, lpszCmdLine, cmdShow )                                                      |
|
|   Description:
|       The main procedure for the App.  After initializing, it just goes
|       into a message-processing loop until it gets a WM_QUIT message
|       (meaning the app was closed).                                                                              |
|
|   Arguments:
|       hInst       instance handle of this instance of the app
|       hPrev       instance handle of previous instance, NULL if first
|       szCmdLine   ->null-terminated command line
|       cmdShow     specifies how the window is initially displayed
|
|       Returns:
|               The exit code as specified in the WM_QUIT message.
|
\*----------------------------------------------------------------------------*/
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
{
    MSG msg;

    /* Call initialization procedure */
    if (!AppInit(hInst,hPrev,sw,szCmdLine))
    return FALSE;

    /*
     * Polling messages from event queue
     */
    for (;;) {
        if(!GetMessage(&msg, NULL, 0, 0)) {
            return msg.wParam;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}


/*----------------------------------------------------------------------------*\
|   AppPaint(hwnd, hdc)
|
|   Description:
|       The paint function.  Right now this does nothing.
|
|   Arguments:
|       hwnd    window painting into
|       hdc     display context to paint to
|
|   Returns:
|       nothing
|
\*----------------------------------------------------------------------------*/
AppPaint (HWND hwnd, HDC hdc)
{
    TEXTMETRIC tm;
    int     x,y;
    RECT    rc;
    
#define PRINT_STUFF(sz) (TextOut(hdc, x, y, sz, lstrlen(sz)), y += tm.tmHeight+1)

    SelectObject(hdc, GetStockObject(ANSI_FIXED_FONT));
    GetTextMetrics(hdc, &tm);

    GetClientRect(hwnd,&rc);
    
    SetTextColor(hdc,GetSysColor(COLOR_WINDOWTEXT));
    SetBkColor(hdc,GetSysColor(COLOR_WINDOW));

    // setup start pos
    x=4;
    y=4;

    PRINT_STUFF("Select 'Time All' to run tests");
    y += (tm.tmHeight+1) * 2;
    PRINT_STUFF("The screen will go blank during some tests");

    return TRUE;
}

/*----------------------------------------------------------------------------*\
|   AppWndProc( hwnd, uiMessage, wParam, lParam )
|
|   Description:
|       The window proc for the app's main (tiled) window.
|       This processes all of the parent window's messages.
|
\*----------------------------------------------------------------------------*/
LRESULT CALLBACK AppWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (msg) {
        case WM_COMMAND:
            return AppCommand(hwnd,msg,wParam,lParam);
            break;

        case WM_PAINT:
            hdc = BeginPaint(hwnd,&ps);
            AppPaint( hwnd, hdc );
            break;

        case WM_DESTROY:
            hAccelApp = NULL;
            PostQuitMessage(0);
            break;

    }
    return DefWindowProc(hwnd,msg,wParam,lParam);
}

/*----------------------------------------------------------------------------*\
|   AppCommand(hwnd, msg, wParam, lParam )
|
|   Description:
|       handles WM_COMMAND messages for the main window (hwndApp)
|       of the parent window's messages.
|
\*----------------------------------------------------------------------------*/
LONG AppCommand (HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    char achFileName[128] = "";
    
    switch(wParam) {
        case MENU_ABOUT:
            DialogBox(hInstApp,"AppAbout",hwnd,(DLGPROC)AppAbout);
            break;
        

        // do all the timing tests
        case MENU_TIMEALL:
            {
            HCURSOR Arror = SetCursor(LoadCursor(0,IDC_WAIT));

            DWORD PriorityClass = GetPriorityClass(GetCurrentProcess());
            int ThreadPriority = GetThreadPriority(GetCurrentThread());

            if( NUMBER_OF(SystemTests) != NUMBER_OF(VideoTests))
            {
                ErrMsg("test number mismatch");
                break;
            }

            // Jack this thread up to realtime.
            // NOTE: This is very bad from for the real world. The rule is
            // "The Higher your ptiority, the less work your thread should do".
            // We are doing a lot of work. But this is only a test app, so who cares.
            SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
            SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);

            // setup the surfaces for the bandwidth tests
            BandwidthTestsSetup();
            // Walk through the list of timers, and call them all

            for(int Counter = 0;Counter < NUMBER_OF(SystemTests) ;Counter++) {

                SetWindowText(hwndApp,SystemTests[Counter].pDescription);

                DoSystemTest( &SystemTests[Counter] );

                SetWindowText(hwndApp,VideoTests[Counter].pDescription);

                DoVideoTests( &VideoTests[Counter] );
            
            }

            // do 320x240 frame rate tests
            ModeXFrameCount = GetFPS(320,240);
            // do 640x480 frame rate tests
            ModeYFrameCount = GetFPS(640,480);
            // do 320x240 stretch to 640x480 frame rate tests
            StretchFrameCount = GetStretchFPS(640,480);

            // return ourselves to normal
            SetPriorityClass(GetCurrentProcess(),PriorityClass);
            SetThreadPriority(GetCurrentThread(),ThreadPriority);

            SetCursor(Arror);
            SetWindowText(hwndApp,szAppName);
            
            PrintTimingResults();
            }
            break;

        case MENU_EXIT:
            PostMessage(hwnd,WM_CLOSE,0,0L);
            break;
    }
    return 0L;
}

/*----------------------------------------------------------------------------*\
|   ErrMsg - Opens a Message box with a error message in it.  The user can
|       select the OK button to continue
\*----------------------------------------------------------------------------*/
int ErrMsg (LPSTR sz,...)
{
        char ach[128];

        wvsprintf (ach,sz,(LPSTR)(&sz+1));       /* Format the string */
        MessageBox(hwndApp,ach,szAppName,MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
        return FALSE;
}




/*----------------------------------------------------------------------------*\
|   PrintTimingResults( void )
|
|   Description:
|       Print the results of the timting tests
|
\*----------------------------------------------------------------------------*/
VOID
PrintTimingResults( void )
{
    double result;
    LARGE_INTEGER perfFreq;
    char OutputBuffer[2048];

    ostrstream Out(OutputBuffer,sizeof(OutputBuffer));


    // get the frequemcy out of timer
    QueryPerformanceFrequency( &perfFreq );

    // Turn all the time resutls into MB per second
    for(int Counter = 0; Counter < NUMBER_OF(SystemTests) ; Counter++) {
        if(SystemTests[Counter].Time) {
            result = (double)SystemTests[Counter].Iterations;
            result /= (double)SystemTests[Counter].Time / (double)perfFreq.LowPart;
            result /= (double)1000000.0;
            Out<<result;
        } else {
            Out<<"N/A";
        }
        Out<<" MB/sec \t";
        Out<<SystemTests[Counter].pDescription<<endl;

        if(VideoTests[Counter].Time) {
            result = (double)VideoTests[Counter].Iterations;
            result /= (double)VideoTests[Counter].Time / (double)perfFreq.LowPart;
            result /= (double)1000000.0;
            Out<<result;            
        } else {
            Out<<"N/A";
        }
        Out<<" MB/sec \t";
        Out<<VideoTests[Counter].pDescription<<endl;


    }

    if( ModeXFrameCount ) {
        Out<<((double)ModeXFrameCount/5.0);
    } else {
        Out<<"N/A";
    }
    Out<<" FPS \t\t";
    Out<<"320x240 Frame Rate"<<endl;

    if( ModeYFrameCount ) {
        Out<<((double)ModeYFrameCount/5.0);
    } else {
        Out<<"N/A";
    }
    Out<<" FPS \t\t";
    Out<<"640x480 Frame Rate"<<endl;

    if( StretchFrameCount ) {
        Out<<((double)StretchFrameCount/5.0);
    } else {
        Out<<"N/A";
    }
    Out<<" FPS \t\t";
    Out<<"320x240 stretched to 640x480"<<endl;

    Out<<ends;
#ifndef LOR_TESTS
    // display the results
    MessageBox(0, OutputBuffer ,"Timing Results",MB_OK);
#endif

}

VOID
DoSystemTest( PTIMING_RESULT pTimeObject )
{
    LARGE_INTEGER startTime, endTime;

    QueryPerformanceCounter( &startTime );

    // do the memory copy
    pTimeObject->Iterations = (*pTimeObject->pTimer)(pSource, pDest, ScreenHeight, ScreenWidth, ScreenWidth, 100 );
    
    QueryPerformanceCounter( &endTime );

    // if it takes more than 32bits of clock ticks
    // it's not worth profiling.
    pTimeObject->Time = endTime.LowPart - startTime.LowPart;
}

VOID
DoVideoTests( PTIMING_RESULT pTimeObject )
{
    DDSURFACEDESC ddsd;
    ULONG returnValue;
    LARGE_INTEGER startTime, endTime;

    ZeroMemory( &ddsd, sizeof( ddsd ) );
    ddsd.dwSize = sizeof( ddsd );


    // lock the video memory surfce
    // NOTE: We are about to do a lock for a REALLY long time.
    // this is really bad from in the real world.
    returnValue = pOffScreenBuf->Lock( NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR, NULL);
    if( returnValue == DD_OK ) {
        if( ddsd.lPitch && ddsd.dwWidth && ddsd.dwHeight && ddsd.lpSurface) {

            // get the time, and do the copy
            QueryPerformanceCounter( &startTime );

            pTimeObject->Iterations = (*pTimeObject->pTimer)( pSource, ddsd.lpSurface, ddsd.dwHeight, ddsd.dwWidth, ddsd.lPitch, 100 );

            QueryPerformanceCounter( &endTime );

            pOffScreenBuf->Unlock( ddsd.lpSurface );
            // stor the delta time
            pTimeObject->Time = endTime.LowPart - startTime.LowPart;
        } else {
            pOffScreenBuf->Unlock( ddsd.lpSurface );
            ErrMsg("Lock returns bad stuff");
        }
    } else {
        ErrMsg("Can't lock surface");
    }
    return;

}

BOOL
InitFullScreen(
    DWORD Height,
    DWORD Width
    )
{
    DDSURFACEDESC ddsd;
    DDSCAPS ddscaps;
    HRESULT ddrval;
    ULONG returnValue = FALSE;

    // Kill our current offscreen surface.
    if( pOffScreenBuf ) {
        ddrval = pOffScreenBuf->Release();
        if( ddrval != DD_OK ) {
            ErrMsg("Can't release offscreen buf");
        }
    }


    // Get exclusive mode
    ddrval = pDirectDrawObj->SetCooperativeLevel( hwndApp, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX | DDSCL_NOWINDOWCHANGES );

    if( ddrval == DD_OK ){
        // set the display mode to the requested mode
        ddrval = pDirectDrawObj->SetDisplayMode( Height, Width, 8);
        if(ddrval == DD_OK){
            ddsd.dwSize = sizeof( ddsd );
            ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
            ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
                          DDSCAPS_FLIP |
                          DDSCAPS_COMPLEX;
            ddsd.dwBackBufferCount = 1;
            // get the primary buffer
            ddrval = pDirectDrawObj->CreateSurface( &ddsd, &pOnScreenBuf, NULL );

            if( ddrval == DD_OK ) {
                ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
                // the back buffer might be in system memory depending
                // on the card and the mode.
                ddrval = pOnScreenBuf->GetAttachedSurface(&ddscaps, &pOffScreenBuf);

                if( ddrval == DD_OK ) {
                    returnValue = TRUE;
                } 
            }
        } else {
            ErrMsg("can't set display mode %d x %d",Width,Height);
        }
    }
    return returnValue;
}


BOOL
CreateSysMemSurface( VOID )
{
    DDSURFACEDESC ddsd;
    ULONG returnValue;

    // clear out out stuff
    ZeroMemory( &ddsd, sizeof( ddsd ) );
    ddsd.dwSize = sizeof( ddsd );

    // build a surface in system memory
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY;
    
    ddsd.dwHeight = 240;
    ddsd.dwWidth = 320;
    returnValue = pDirectDrawObj->CreateSurface( &ddsd, &pSysMemBuf, NULL );
    if( DD_OK == returnValue ) {
        return TRUE;
    }

    return FALSE;
}

ULONG
GetStretchFPS( int width, int height )
{
    ULONG time;
    HRESULT ddrval;
    DDSURFACEDESC ddsd;
    DDBLTFX     ddbltfx;
    ULONG FrameCount;

    // set the mode (and create the surfaces)
    if( InitFullScreen( width, height ) ) {
        // get the system memory surface
        if( CreateSysMemSurface() ) {

            // build the ROP for the blt
            ZeroMemory( &ddbltfx, sizeof(ddbltfx) );
            ddbltfx.dwSize = sizeof( ddbltfx );
            ddbltfx.dwROP = SRCCOPY;

            time = timeGetTime();
            // wait for a seconds for the mode to settle out
            // (I don't need to do this.. I'm just paranoid)
            while( (timeGetTime() - time) < 1000);
            FrameCount = 0;
            time = timeGetTime();
            ddrval = 0;
            // do this for 5 seconds
            while( ((timeGetTime() - time) < 5000) && (DD_OK == ddrval) ) {
                // simluate rendering and off screen frame (with a copy)
                ddrval |= pSysMemBuf->Lock( NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

                DwordMemCopy_Pitch( pSource, ddsd.lpSurface, ddsd.dwHeight, ddsd.dwWidth, ddsd.lPitch, 1);

                ddrval |= pSysMemBuf->Unlock( NULL );

                // blt the system buffer into the back buffer
                ddrval |= pOffScreenBuf->Blt( NULL, pSysMemBuf, NULL, DDBLT_ROP, &ddbltfx);

                // flip the back buffer on screen
                ddrval |= pOnScreenBuf->Flip( NULL, DDFLIP_WAIT );

                FrameCount++;
            }
        }
        pDirectDrawObj->RestoreDisplayMode();
        if( DD_OK != ddrval ) {
            ErrMsg("blt failure %X",ddrval);
        }

        pOffScreenBuf->Release();
        pOffScreenBuf = NULL;
        pOnScreenBuf->Release();
        pOnScreenBuf = NULL;
        pSysMemBuf->Release();
        pSysMemBuf = NULL;
    }
    return FrameCount;
}

ULONG
GetFPS( int width, int height )
{
    ULONG time;
    DDSURFACEDESC ddsd;
    HRESULT ddrval;
    ULONG FrameCount;

    ZeroMemory( &ddsd, sizeof( ddsd ) );
    ddsd.dwSize = sizeof( ddsd );

    // set the mode (and setup the surfaces)    
    if( InitFullScreen( width, height ) ) {
        time = timeGetTime();
        // wait for a seconds for the mode to settle out
        // (I don't need to do this, I'm just paranoid)
        while( (timeGetTime() - time) < 1000);
        ddrval = DD_OK;
        FrameCount = 0;
        time = timeGetTime();
        while( ((timeGetTime() - time) < 5000) && (ddrval == DD_OK) ) {
            // simluate a render into the back buffer with a copy
            ddrval |= pOffScreenBuf->Lock( NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
            DwordMemCopy_Pitch( pSource, ddsd.lpSurface, ddsd.dwHeight, ddsd.dwWidth, ddsd.lPitch, 1 );
            ddrval |= pOffScreenBuf->Unlock( NULL );

            // flip the off screen surface on screen
            ddrval |= pOnScreenBuf->Flip( NULL, DDFLIP_WAIT );
            FrameCount++;
        }
        pDirectDrawObj->RestoreDisplayMode();
        if( DD_OK != ddrval ) {
            ErrMsg("blt failure %X",ddrval);
        }
        pOffScreenBuf->Release();
        pOffScreenBuf = NULL;
        pOnScreenBuf->Release();
        pOnScreenBuf = NULL;
    }

    return FrameCount;
}

/*****************************************************************************
 *
 * dprintf() is called by the DPF macro if DEBUG is defined at compile time.
 *
 * The messages will be send to COM1: like any debug message. To
 * enable debug output, add the following to WIN.INI :
 *
 * [debug]
 * QA=1
 *
 ****************************************************************************/

#ifdef DEBUG

#define MODNAME "QA"


#define _WINDLL
#include <stdarg.h>

void FAR CDECL dprintf(LPSTR szFormat, ...)
{
    char ach[128];
    va_list va;
    
    static BOOL fDebug = -1;
    
    if (fDebug == -1) {
        fDebug = GetProfileIntA("Debug", MODNAME, TRUE);
    }
    
    if (!fDebug) return;

    
    
    lstrcpyA(ach, MODNAME ": ");
    va_start(va, szFormat);
    wvsprintfA(ach+lstrlenA(ach),szFormat,(LPSTR)va);
    va_end(va);
    lstrcatA(ach, "\r\n");
    
    OutputDebugStringA(ach);
}

#endif
