/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1993 - 1997  Microsoft Corporation.  All Rights Reserved.
 *
 **************************************************************************/

#include <streams.h>
#include <mmreg.h>
#include <msacm.h>
#include "fcntl.h"
#include "io.h"
#include "stdio.h"
#include "amcap.h"
#include "status.h"

// you can never have too many parentheses!
#define ABS(x) (((x) > 0) ? (x) : -(x))

HINSTANCE ghInstApp;
HACCEL ghAccel;
HFONT  ghfontApp;
TEXTMETRIC gtm;
TCHAR gszAppName[]=TEXT("AMCAP");
HWND ghwndApp, ghwndStatus;

// forgive me for using global variables
struct _capstuff {
    char szCaptureFile[_MAX_PATH];
    WORD wCapFileSize;  // size in Meg
    ICaptureGraphBuilder *pBuilder;
    IVideoWindow *pVW;
    IMediaEventEx *pME;
    IAMDroppedFrames *pDF;
    IAMVideoCompression *pVC;
    IAMVfwCaptureDialogs *pDlg;
    IAMStreamConfig *pASC;      // for audio cap
    IAMStreamConfig *pVSC;      // for video cap
    IBaseFilter *pRender;
    IBaseFilter *pVCap, *pACap;
    IGraphBuilder *pFg;
    IFileSinkFilter *pSink;
    IConfigAviMux *pConfigAviMux;
    int  iMasterStream;
    BOOL fCaptureGraphBuilt;
    BOOL fPreviewGraphBuilt;
    BOOL fCapturing;
    BOOL fPreviewing;
    BOOL fCapAudio;
    BOOL fCapAudioIsRelevant;
    int  iVideoDevice;
    int  iAudioDevice;
    double FrameRate;
    BOOL fWantPreview;
    long lCapStartTime;
    long lCapStopTime;
    char achFriendlyName[120];
    BOOL fUseTimeLimit;
    BOOL fUseFrameRate;
    DWORD dwTimeLimit;
    int iFormatDialogPos;
    int iSourceDialogPos;
    int iDisplayDialogPos;
    int iVCapDialogPos;
    int iVCrossbarDialogPos;
    int iTVTunerDialogPos;
    int iACapDialogPos;
    int iACrossbarDialogPos;
    int iTVAudioDialogPos;
    int iVCapCapturePinDialogPos;
    int iVCapPreviewPinDialogPos;
    int iACapCapturePinDialogPos;
    long lDroppedBase;
    long lNotBase;
    BOOL fPreviewFaked;
} gcap;

typedef LONG (PASCAL *LPWNDPROC)(HWND, UINT, WPARAM, LPARAM); // pointer to a window procedure

/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/
LONG WINAPI AppWndProc(HWND hwnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
LONG PASCAL AppCommand(HWND hwnd, unsigned msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int ErrMsg (LPTSTR sz,...);
BOOL SetCaptureFile(HWND hWnd);
BOOL SaveCaptureFile(HWND hWnd);
BOOL AllocCaptureFile(HWND hWnd);
int DoDialog(HWND hwndParent, int DialogID, DLGPROC fnDialog, long lParam);
int FAR PASCAL AllocCapFileProc(HWND hDlg, UINT Message, UINT wParam, LONG lParam);
int FAR PASCAL FrameRateProc(HWND hDlg, UINT Message, UINT wParam, LONG lParam);
int FAR PASCAL TimeLimitProc(HWND hDlg, UINT Message, UINT wParam, LONG lParam);
int FAR PASCAL PressAKeyProc(HWND hDlg, UINT Message, UINT wParam, LONG lParam);
void TearDownGraph(void);
BOOL BuildCaptureGraph();
BOOL BuildPreviewGraph();
void UpdateStatus(BOOL fAllStats);
void AddDevicesToMenu();
void ChooseDevices(int idV, int idA);
void ChooseFrameRate();
BOOL InitCapFilters();
void FreeCapFilters();
BOOL StopPreview();
BOOL StartPreview();
BOOL StopCapture();
DWORDLONG GetSize(LPCSTR ach);
void MakeMenuOptions();
/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

// set our app's caption to be our app name followed by the capture file name
//
void SetAppCaption()
{
    char ach[_MAX_PATH + 80];
    lstrcpy(ach, gszAppName);
    if (gcap.szCaptureFile[0] != 0) {
	lstrcat(ach, " - ");
	lstrcat(ach, gcap.szCaptureFile);
    }
    SetWindowText(ghwndApp, ach);
}


/*----------------------------------------------------------------------------*\
|   AppInit( hInst, hPrev)                                                     |
|                                                                              |
|   Description:                                                               |
|       This is called when the application is first loaded into               |
|       memory.  It performs all initialization that doesn't need to be done   |
|       once per instance.                                                     |
|                                                                              |
|   Arguments:                                                                 |
|       hInstance       instance handle of current instance                    |
|       hPrev           instance handle of previous instance                   |
|                                                                              |
|   Returns:                                                                   |
|       TRUE if successful, FALSE if not                                       |
|                                                                              |
\*----------------------------------------------------------------------------*/
BOOL AppInit(HINSTANCE hInst, HINSTANCE hPrev, int sw,LPSTR szCmdLine)
{
    WNDCLASS    cls;
    HDC         hdc;

    const DWORD  dwExStyle = 0;

    CoInitialize(NULL);
    DbgInitialise(hInst);

    /* Save instance handle for DialogBoxs */
    ghInstApp = hInst;

    ghAccel = LoadAccelerators(hInst, MAKEINTATOM(ID_APP));

    if (!hPrev) {
	/*
	 *  Register a class for the main application window
	 */
	cls.hCursor        = LoadCursor(NULL,IDC_ARROW);
	cls.hIcon          = LoadIcon(hInst, TEXT("AMCapIcon"));
	cls.lpszMenuName   = MAKEINTATOM(ID_APP);
	cls.lpszClassName  = MAKEINTATOM(ID_APP);
	cls.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
	cls.hInstance      = hInst;
	cls.style          = CS_BYTEALIGNCLIENT | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	cls.lpfnWndProc    = (LPWNDPROC)AppWndProc;
	cls.cbWndExtra     = 0;
	cls.cbClsExtra     = 0;

	if (!RegisterClass(&cls))
	    return FALSE;
    }

    // Is this necessary?
    ghfontApp = (HFONT)GetStockObject(ANSI_VAR_FONT);
    hdc = GetDC(NULL);
    SelectObject(hdc, ghfontApp);
    GetTextMetrics(hdc, &gtm);
    ReleaseDC(NULL, hdc);

    ghwndApp=CreateWindowEx(dwExStyle,
			    MAKEINTATOM(ID_APP),    // Class name
			    gszAppName,             // Caption
						    // Style bits
			    WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
			    CW_USEDEFAULT, 0,       // Position
			    320,300,                // Size
			    (HWND)NULL,             // Parent window (no parent)
			    (HMENU)NULL,            // use class menu
			    hInst,                  // handle to window instance
			    (LPSTR)NULL             // no params to pass on
			   );

    // create the status bar
    statusInit(hInst, hPrev);
    ghwndStatus = CreateWindowEx(
		    0,
		    szStatusClass,
		    NULL,
		    WS_CHILD|WS_BORDER|WS_VISIBLE|WS_CLIPSIBLINGS,
		    0, 0,
		    0, 0,
		    ghwndApp,
		    NULL,
		    hInst,
		    NULL);
    if (ghwndStatus == NULL) {
	return(FALSE);
    }
    ShowWindow(ghwndApp,sw);

    // get the capture file name from win.ini
    GetProfileString("annie", "CaptureFile", "", gcap.szCaptureFile,
					sizeof(gcap.szCaptureFile));

    // get which devices to use from win.ini
    gcap.iVideoDevice = -1;     // force update
    gcap.iAudioDevice = -1;     // force update
    int idV = GetProfileInt("annie", "VideoDevice", 0);
    int idA = GetProfileInt("annie", "AudioDevice", 0);
    AddDevicesToMenu(); // list all capture devices in the system as menu items

    // do we want audio?
    gcap.fCapAudio = GetProfileInt("annie", "CaptureAudio", TRUE);

    // do we want preview?
    gcap.fWantPreview = GetProfileInt("annie", "WantPreview", FALSE);
    // which stream should be the master? NONE(-1) means nothing special happens
    // AUDIO(1) means the video frame rate is changed before written out to keep
    // the movie in sync when the audio and video capture crystals are not the
    // same (and therefore drift out of sync after a few minutes).  VIDEO(0)
    // means the audio sample rate is changed before written out
    gcap.iMasterStream = GetProfileInt("annie", "MasterStream", -1);


    // get the frame rate from win.ini before making the graph
    gcap.fUseFrameRate = GetProfileInt("annie", "UseFrameRate", 1);
    int units_per_frame = GetProfileInt("annie", "FrameRate", 666667);  // 15fps
    gcap.FrameRate = 10000000. / units_per_frame;
    gcap.FrameRate = (int)(gcap.FrameRate * 100) / 100.;
    // reasonable default
    if (gcap.FrameRate <= 0.)
	gcap.FrameRate = 15.0;
	 
    // instantiate the capture filters we need to do the menu items
    // this will start previewing, if wanted
    ChooseDevices(idV, idA);    // make these the official devices we're using
				// and builds a partial filtergraph.

    SetAppCaption();
    return TRUE;
}

/*----------------------------------------------------------------------------*\
|   WinMain( hInst, hPrev, lpszCmdLine, cmdShow )                              |
|                                                                              |
|   Description:                                                               |
|       The main procedure for the App.  After initializing, it just goes      |
|       into a message-processing loop until it gets a WM_QUIT message         |
|       (meaning the app was closed).                                          |
|                                                                              |
|   Arguments:                                                                 |
|       hInst           instance handle of this instance of the app            |
|       hPrev           instance handle of previous instance, NULL if first    |
|       szCmdLine       ->null-terminated command line                         |
|       cmdShow         specifies how the window is initially displayed        |
|                                                                              |
|   Returns:                                                                   |
|       The exit code as specified in the WM_QUIT message.                     |
|                                                                              |
\*----------------------------------------------------------------------------*/
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
{
    MSG     msg;

    /* Call initialization procedure */
    if (!AppInit(hInst,hPrev,sw,szCmdLine))
	return FALSE;

    /*
     * Polling messages from event queue
     */
    for (;;)
    {
	while (PeekMessage(&msg, NULL, 0, 0,PM_REMOVE))
	{
	    if (msg.message == WM_QUIT)
		return msg.wParam;

	    if (TranslateAccelerator(ghwndApp, ghAccel, &msg))
		continue;
	
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	}

	WaitMessage();
    }

    // not reached
    return msg.wParam;
}


/*----------------------------------------------------------------------------*\
|   AppWndProc( hwnd, uiMessage, wParam, lParam )                              |
|                                                                              |
|   Description:                                                               |
|       The window proc for the app's main (tiled) window.  This processes all |
|       of the parent window's messages.                                       |
|                                                                              |
|   Arguments:                                                                 |
|       hwnd            window handle for the window                           |
|       msg             message number                                         |
|       wParam          message-dependent                                      |
|       lParam          message-dependent                                      |
|                                                                              |
|   Returns:                                                                   |
|       0 if processed, nonzero if ignored                                     |
|                                                                              |
\*----------------------------------------------------------------------------*/
LONG WINAPI  AppWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rc;
    int cxBorder, cyBorder, cy;

    switch (msg) {

	//
	//
	case WM_CREATE:
	    break;

	case WM_COMMAND:
	    return AppCommand(hwnd,msg,wParam,lParam);

	case WM_INITMENU:
	    // we can start capture if not capturing already
	    EnableMenuItem((HMENU)wParam, MENU_START_CAP, 
			(!gcap.fCapturing) ? MF_ENABLED :
			MF_GRAYED);
	    // we can stop capture if it's currently capturing
	    EnableMenuItem((HMENU)wParam, MENU_STOP_CAP, 
			(gcap.fCapturing) ? MF_ENABLED : MF_GRAYED);

	    // We can bring up a dialog if the graph is stopped
	    EnableMenuItem((HMENU)wParam, MENU_DIALOG0, !gcap.fCapturing ?
		MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_DIALOG1, !gcap.fCapturing ?
		MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_DIALOG2, !gcap.fCapturing ?
		MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_DIALOG3, !gcap.fCapturing ?
		MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_DIALOG4, !gcap.fCapturing ?
		MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_DIALOG5, !gcap.fCapturing ?
		MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_DIALOG6, !gcap.fCapturing ?
		MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_DIALOG7, !gcap.fCapturing ?
		MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_DIALOG8, !gcap.fCapturing ?
		MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_DIALOG9, !gcap.fCapturing ?
		MF_ENABLED : MF_GRAYED);

	    // toggles capturing audio or not - can't be capturing right now
	    // and we must have an audio capture device created
	    EnableMenuItem((HMENU)wParam, MENU_CAP_AUDIO, 
		    (!gcap.fCapturing && gcap.pACap) ? MF_ENABLED : MF_GRAYED);
	    // are we capturing audio?
	    CheckMenuItem((HMENU)wParam, MENU_CAP_AUDIO, 
		    (gcap.fCapAudio) ? MF_CHECKED : MF_UNCHECKED);
	    // change audio formats when not capturing
	    EnableMenuItem((HMENU)wParam, MENU_AUDIOFORMAT, (gcap.fCapAudio &&
			!gcap.fCapturing) ? MF_ENABLED : MF_GRAYED);
	    // change frame rate when not capturing, and only if the video
	    // filter captures a VIDEOINFO type format
	    EnableMenuItem((HMENU)wParam, MENU_FRAMERATE,
			(!gcap.fCapturing && gcap.fCapAudioIsRelevant) ?
			 MF_ENABLED : MF_GRAYED);
	    // change time limit when not capturing
	    EnableMenuItem((HMENU)wParam, MENU_TIMELIMIT,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    // change capture file name when not capturing
	    EnableMenuItem((HMENU)wParam, MENU_SET_CAP_FILE,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    // pre-allocate capture file when not capturing
	    EnableMenuItem((HMENU)wParam, MENU_ALLOC_CAP_FILE,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    // can save capture file when not capturing
	    EnableMenuItem((HMENU)wParam, MENU_SAVE_CAP_FILE,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    // do we want preview?
	    CheckMenuItem((HMENU)wParam, MENU_PREVIEW, 
			(gcap.fWantPreview) ? MF_CHECKED : MF_UNCHECKED);
	    // can toggle preview if not capturing
	    EnableMenuItem((HMENU)wParam, MENU_PREVIEW,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);

	    // which is the master stream? Not applicable unless we're also
	    // capturing audio
	    EnableMenuItem((HMENU)wParam, MENU_NOMASTER,
		(!gcap.fCapturing && gcap.fCapAudio) ? MF_ENABLED : MF_GRAYED);
	    CheckMenuItem((HMENU)wParam, MENU_NOMASTER, 
		    (gcap.iMasterStream == -1) ? MF_CHECKED : MF_UNCHECKED);
	    EnableMenuItem((HMENU)wParam, MENU_AUDIOMASTER,
		(!gcap.fCapturing && gcap.fCapAudio) ? MF_ENABLED : MF_GRAYED);
	    CheckMenuItem((HMENU)wParam, MENU_AUDIOMASTER, 
		    (gcap.iMasterStream == 1) ? MF_CHECKED : MF_UNCHECKED);
	    EnableMenuItem((HMENU)wParam, MENU_VIDEOMASTER,
		(!gcap.fCapturing && gcap.fCapAudio) ? MF_ENABLED : MF_GRAYED);
	    CheckMenuItem((HMENU)wParam, MENU_VIDEOMASTER, 
		    (gcap.iMasterStream == 0) ? MF_CHECKED : MF_UNCHECKED);

	    // can't select a new capture device when capturing
	    EnableMenuItem((HMENU)wParam, MENU_VDEVICE0,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_VDEVICE1,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_VDEVICE2,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_VDEVICE3,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_VDEVICE4,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_VDEVICE5,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_VDEVICE6,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_VDEVICE7,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_VDEVICE8,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_VDEVICE9,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_ADEVICE0,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_ADEVICE1,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_ADEVICE2,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_ADEVICE3,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_ADEVICE4,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_ADEVICE5,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_ADEVICE6,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_ADEVICE7,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_ADEVICE8,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    EnableMenuItem((HMENU)wParam, MENU_ADEVICE9,
			!gcap.fCapturing ? MF_ENABLED : MF_GRAYED);
	    
	    break;

	//
	// We're out of here!
	//
	case WM_DESTROY:
	    DbgTerminate();
	    CoUninitialize();
	    PostQuitMessage(0);
	    break;

	//
	//
	case WM_CLOSE:
	    StopPreview();
	    StopCapture();
	    TearDownGraph();
	    FreeCapFilters();
	    // store current settings in win.ini for next time
	    WriteProfileString("annie", "CaptureFile", gcap.szCaptureFile);
	    char ach[10];
	    wsprintf(ach, "%d", gcap.iVideoDevice);
	    WriteProfileString("annie", "VideoDevice", ach);
	    wsprintf(ach, "%d", gcap.iAudioDevice);
	    WriteProfileString("annie", "AudioDevice", ach);
	    wsprintf(ach, "%d", (int)(10000000 / gcap.FrameRate));
	    WriteProfileString("annie", "FrameRate", ach);
	    wsprintf(ach, "%d", gcap.fUseFrameRate);
	    WriteProfileString("annie", "UseFrameRate", ach);
	    wsprintf(ach, "%d", gcap.fCapAudio);
	    WriteProfileString("annie", "CaptureAudio", ach);
	    wsprintf(ach, "%d", gcap.fWantPreview);
	    WriteProfileString("annie", "WantPreview", ach);
	    wsprintf(ach, "%d", gcap.iMasterStream);
	    WriteProfileString("annie", "MasterStream", ach);
	    break;

	case WM_ERASEBKGND:
	    break;

	// ESC will stop capture
	case WM_KEYDOWN:
	    if ((GetAsyncKeyState(VK_ESCAPE) & 0x01) && gcap.fCapturing) {
		StopCapture();
		if (gcap.fWantPreview) {
		    BuildPreviewGraph();
		    StartPreview();
		}
	    }
	    break;
		
	case WM_PAINT:
	    hdc = BeginPaint(hwnd,&ps);

	    // nothing to do

	    EndPaint(hwnd,&ps);
	    break;

	case WM_TIMER:
	    // update our status bar with #captured, #dropped
	    // if we've stopped capturing, don't do it anymore.  Some WM_TIMER
	    // messages may come late, after we've destroyed the graph and
	    // we'll get bogus numbers.
	    if (gcap.fCapturing)
	        UpdateStatus(FALSE);

	    // is our time limit up?
	    if (gcap.fUseTimeLimit) {
		if ((timeGetTime() - gcap.lCapStartTime) / 1000 >=
							gcap.dwTimeLimit) {
		    StopCapture();
		    if (gcap.fWantPreview) {
			BuildPreviewGraph();
			StartPreview();
		    }
		}
	    }
	    break;

	case WM_SIZE:
	    // make the preview window fit inside our window, taking up
	    // all of our client area except for the status window at the
	    // bottom
	    GetClientRect(ghwndApp, &rc);
	    cxBorder = GetSystemMetrics(SM_CXBORDER);
	    cyBorder = GetSystemMetrics(SM_CYBORDER);
	    cy = statusGetHeight() + cyBorder;
	    MoveWindow(ghwndStatus, -cxBorder, rc.bottom - cy,
			rc.right + (2 * cxBorder), cy + cyBorder, TRUE);
	    rc.bottom -= cy;
	    // this is the video renderer window showing the preview
	    if (gcap.pVW)
		gcap.pVW->SetWindowPosition(0, 0, rc.right, rc.bottom);
	    break;

	case WM_FGNOTIFY:
	    // uh-oh, something went wrong while capturing - the filtergraph
	    // will send us events like EC_COMPLETE, EC_USERABORT and the one
	    // we care about, EC_ERRORABORT.
	    if (gcap.pME && gcap.fCapturing) {
		    LONG event, l1, l2;
		    while (gcap.pME->GetEvent(&event, &l1, &l2, 0) == S_OK) {
			if (event == EC_ERRORABORT) {
			    // pME will go away in BuildPreviewGraph
			    gcap.pME->FreeEventParams(event, l1, l2);
			    StopCapture();
			    if (gcap.fWantPreview) {
				BuildPreviewGraph();
				StartPreview();
			    }
			    ErrMsg("ERROR during capture - disk possibly full");
			    break;
			}
			gcap.pME->FreeEventParams(event, l1, l2);
		    }
	    }
	    break;

    }
    return DefWindowProc(hwnd,msg,wParam,lParam);
}


// Make a graph builder object we can use for capture graph building
//
BOOL MakeBuilder()
{
    // we have one already
    if (gcap.pBuilder)
	return TRUE;

    HRESULT hr = CoCreateInstance((REFCLSID)CLSID_CaptureGraphBuilder,
			NULL, CLSCTX_INPROC, (REFIID)IID_ICaptureGraphBuilder,
			(void **)&gcap.pBuilder);
    return (hr == NOERROR) ? TRUE : FALSE;
}


// Make a graph object we can use for capture graph building
//
BOOL MakeGraph()
{
    // we have one already
    if (gcap.pFg)
	return TRUE;

    HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
			       IID_IGraphBuilder, (LPVOID *)&gcap.pFg);
    return (hr == NOERROR) ? TRUE : FALSE;
}


// make sure the preview window inside our window is as big as the
// dimensions of captured video, or some capture cards won't show a preview.
// (Also, it helps people tell what size video they're capturing)
// We will resize our app's window big enough so that once the status bar
// is positioned at the bottom there will be enough room for the preview
// window to be w x h
//
void ResizeWindow(int w, int h)
{
    RECT rcW, rcC;
    int xExtra, yExtra;
    int cyBorder = GetSystemMetrics(SM_CYBORDER);

    GetWindowRect(ghwndApp, &rcW);
    GetClientRect(ghwndApp, &rcC);
    xExtra = rcW.right - rcW.left - rcC.right;
    yExtra = rcW.bottom - rcW.top - rcC.bottom + cyBorder + statusGetHeight();
    
    rcC.right = w;
    rcC.bottom = h;
    SetWindowPos(ghwndApp, NULL, 0, 0, rcC.right + xExtra,
				rcC.bottom + yExtra, SWP_NOZORDER | SWP_NOMOVE);
}


// Tear down everything downstream of a given filter
void NukeDownstream(IBaseFilter *pf)
{
    //DbgLog((LOG_TRACE,1,TEXT("Nuking...")));

    IPin *pP, *pTo;
    ULONG u;
    IEnumPins *pins = NULL;
    PIN_INFO pininfo;
    HRESULT hr = pf->EnumPins(&pins);
    pins->Reset();
    while (hr == NOERROR) {
        hr = pins->Next(1, &pP, &u);
	if (hr == S_OK && pP) {
	    pP->ConnectedTo(&pTo);
	    if (pTo) {
	        hr = pTo->QueryPinInfo(&pininfo);
	        if (hr == NOERROR) {
		    if (pininfo.dir == PINDIR_INPUT) {
		        NukeDownstream(pininfo.pFilter);
		        gcap.pFg->Disconnect(pTo);
		        gcap.pFg->Disconnect(pP);
	                gcap.pFg->RemoveFilter(pininfo.pFilter);
		    }
	            pininfo.pFilter->Release();
		}
		pTo->Release();
	    }
	    pP->Release();
	}
    }
    if (pins)
        pins->Release();
}


// Tear down everything downstream of the capture filters, so we can build
// a different capture graph.  Notice that we never destroy the capture filters
// and WDM filters upstream of them, because then all the capture settings
// we've set would be lost.
//
void TearDownGraph()
{
    if (gcap.pSink)
	gcap.pSink->Release();
    gcap.pSink = NULL;
    if (gcap.pConfigAviMux)
	gcap.pConfigAviMux->Release();
    gcap.pConfigAviMux = NULL;
    if (gcap.pRender)
	gcap.pRender->Release();
    gcap.pRender = NULL;
    if (gcap.pVW) {
	// stop drawing in our window, or we may get wierd repaint effects
	gcap.pVW->put_Owner(NULL);
	gcap.pVW->put_Visible(OAFALSE);
	gcap.pVW->Release();
    }
    gcap.pVW = NULL;
    if (gcap.pME)
	gcap.pME->Release();
    gcap.pME = NULL;
    if (gcap.pDF)
	gcap.pDF->Release();
    gcap.pDF = NULL;

    // destroy the graph downstream of our capture filters
    if (gcap.pVCap)
	NukeDownstream(gcap.pVCap);
    if (gcap.pACap)
	NukeDownstream(gcap.pACap);

    // potential debug output - what the graph looks like
    // if (gcap.pFg) DumpGraph(gcap.pFg, 1);

    gcap.fCaptureGraphBuilt = FALSE;
    gcap.fPreviewGraphBuilt = FALSE;
    gcap.fPreviewFaked = FALSE;
}


// create the capture filters of the graph.  We need to keep them loaded from
// the beginning, so we can set parameters on them and have them remembered
//
BOOL InitCapFilters()
{
    HRESULT hr;
    BOOL f;
    UINT uIndex = 0;

    f = MakeBuilder();
    if (!f) {
	ErrMsg("Cannot instantiate graph builder");
	return FALSE;
    }

//
// First, we need a Video Capture filter, and some interfaces
//

    // !!! There's got to be a way to cache these from building the menu
    // Enumerate all the video devices.  We want #gcap.iVideoDevice
    ICreateDevEnum *pCreateDevEnum;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
			  IID_ICreateDevEnum, (void**)&pCreateDevEnum);
    if (hr != NOERROR)
	return FALSE;
    IEnumMoniker *pEm;
    hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
								&pEm, 0);
    pCreateDevEnum->Release();
    if (hr != NOERROR)
	return FALSE;
    pEm->Reset();
    ULONG cFetched;
    IMoniker *pM;
    gcap.pVCap = NULL;
    while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK)
    {
	// this is the one we want.  Get its name, and instantiate it.
	if ((int)uIndex == gcap.iVideoDevice) {
	    IPropertyBag *pBag;
	    gcap.achFriendlyName[0] = 0;
	    hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
	    if(SUCCEEDED(hr)) {
		VARIANT var;
		var.vt = VT_BSTR;
		hr = pBag->Read(L"FriendlyName", &var, NULL);
		if (hr == NOERROR) {
		    WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1,
					gcap.achFriendlyName, 80, NULL, NULL);
		    SysFreeString(var.bstrVal);
		}
		pBag->Release();
	    }
	    hr = pM->BindToObject(0, 0, IID_IBaseFilter, (void**)&gcap.pVCap);
	    pM->Release();
	    break;
	}
	pM->Release();
	uIndex++;
    }
    pEm->Release();
    if (gcap.pVCap == NULL) {
	ErrMsg("Error %x: Cannot create video capture filter", hr);
	goto InitCapFiltersFail;
    }

    //
    // make a filtergraph, give it to the graph builder and put the video
    // capture filter in the graph
    //

    f = MakeGraph();
    if (!f) {
	ErrMsg("Cannot instantiate filtergraph");
	goto InitCapFiltersFail;
    }
    hr = gcap.pBuilder->SetFiltergraph(gcap.pFg);
    if (hr != NOERROR) {
	ErrMsg("Cannot give graph to builder");
	goto InitCapFiltersFail;
    }

    hr = gcap.pFg->AddFilter(gcap.pVCap, NULL);
    if (hr != NOERROR) {
	ErrMsg("Error %x: Cannot add vidcap to filtergraph", hr);
	goto InitCapFiltersFail;
    }

    // Calling FindInterface below will result in building the upstream
    // section of the capture graph (any WDM TVTuners or Crossbars we might
    // need).

    // we use this interface to get the name of the driver
    // Don't worry if it doesn't work:  This interface may not be available
    // until the pin is connected, or it may not be available at all.
    hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, gcap.pVCap,
				IID_IAMVideoCompression, (void **)&gcap.pVC);

    // !!! What if this interface isn't supported?
    // we use this interface to set the frame rate and get the capture size
    hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, gcap.pVCap,
				IID_IAMStreamConfig, (void **)&gcap.pVSC);
    if (hr != NOERROR) {
	// this means we can't set frame rate (non-DV only)
	ErrMsg("Error %x: Cannot find VCapture:IAMStreamConfig", hr);
    }

    gcap.fCapAudioIsRelevant = TRUE;

    AM_MEDIA_TYPE *pmt;
    // default capture format
    if (gcap.pVSC && gcap.pVSC->GetFormat(&pmt) == S_OK) {
        // DV capture does not use a VIDEOINFOHEADER
	if (pmt->formattype == FORMAT_VideoInfo) {
            // resize our window to the default capture size
            ResizeWindow(HEADER(pmt->pbFormat)->biWidth,
				ABS(HEADER(pmt->pbFormat)->biHeight));
	}
	if (pmt->majortype != MEDIATYPE_Video) {
	    // This capture filter captures something other that pure video.
	    // Maybe it's DV or something?  Anyway, chances are we shouldn't
	    // allow capturing audio separately, since our video capture    
	    // filter may have audio combined in it already!
    	    gcap.fCapAudioIsRelevant = FALSE;
    	    gcap.fCapAudio = FALSE;
	}
        DeleteMediaType(pmt);
    }

    // we use this interface to bring up the 3 dialogs
    // NOTE:  Only the VfW capture filter supports this.  This app only brings
    // up dialogs for legacy VfW capture drivers, since only those have dialogs
    hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, gcap.pVCap,
				IID_IAMVfwCaptureDialogs, (void **)&gcap.pDlg);


    // there's no point making an audio capture filter
    if (gcap.fCapAudioIsRelevant == FALSE)
	goto SkipAudio;

// create the audio capture filter, even if we are not capturing audio right
// now, so we have all the filters around all the time.

    //
    // We want an audio capture filter and some interfaces
    //

    // !!! There's got to be a way to cache these from building the menu
    // Enumerate all the audio devices.  We want #gcap.iAudioDevice
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
			  IID_ICreateDevEnum, (void**)&pCreateDevEnum);
    if (hr != NOERROR)
	goto InitCapFiltersFail;
    uIndex = 0;
    hr = pCreateDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory,
								&pEm, 0);
    pCreateDevEnum->Release();
    if (hr != NOERROR) {
	// there are no audio capture devices. We'll only allow video capture
	gcap.fCapAudio = FALSE;
	goto SkipAudio;
    }
    pEm->Reset();
    gcap.pACap = NULL;
    while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK)
    {
	// this is the one we want!
	if ((int)uIndex == gcap.iAudioDevice) {
	    hr = pM->BindToObject(0, 0, IID_IBaseFilter, (void**)&gcap.pACap);
	    pM->Release();
	    break;
	}
	pM->Release();
	uIndex++;
    }
    pEm->Release();
    if (gcap.pACap == NULL) {
	// there are no audio capture devices. We'll only allow video capture
	gcap.fCapAudio = FALSE;
	ErrMsg("Cannot create audio capture filter");
	goto SkipAudio;
    }

    //
    // put the audio capture filter in the graph
    //

    // We'll need this in the graph to get audio property pages
    hr = gcap.pFg->AddFilter(gcap.pACap, NULL);
    if (hr != NOERROR) {
        ErrMsg("Error %x: Cannot add audcap to filtergraph", hr);
        goto InitCapFiltersFail;
    }

    // Calling FindInterface below will result in building the upstream
    // section of the capture graph (any WDM TVAudio's or Crossbars we might
    // need).

    // !!! What if this interface isn't supported?
    // we use this interface to set the captured wave format
    hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, gcap.pACap,
				IID_IAMStreamConfig, (void **)&gcap.pASC);
    if (hr != NOERROR) {
	    ErrMsg("Cannot find ACapture:IAMStreamConfig");
    }

SkipAudio:

    // potential debug output - what the graph looks like
    // DumpGraph(gcap.pFg, 1);

    return TRUE;

InitCapFiltersFail:
    FreeCapFilters();
    return FALSE;
}


// all done with the capture filters and the graph builder
//
void FreeCapFilters()
{
    if (gcap.pFg)
	gcap.pFg->Release();
    gcap.pFg = NULL;
    if (gcap.pBuilder)
	gcap.pBuilder->Release();
    gcap.pBuilder = NULL;
    if (gcap.pVCap)
	gcap.pVCap->Release();
    gcap.pVCap = NULL;
    if (gcap.pACap)
	gcap.pACap->Release();
    gcap.pACap = NULL;
    if (gcap.pASC)
	gcap.pASC->Release();
    gcap.pASC = NULL;
    if (gcap.pVSC)
	gcap.pVSC->Release();
    gcap.pVSC = NULL;
    if (gcap.pVC)
	gcap.pVC->Release();
    gcap.pVC = NULL;
    if (gcap.pDlg)
	gcap.pDlg->Release();
    gcap.pDlg = NULL;
}


// build the capture graph!
//
BOOL BuildCaptureGraph()
{
    int cy, cyBorder;
    HRESULT hr;
    BOOL f;
    AM_MEDIA_TYPE *pmt;

    // we have one already
    if (gcap.fCaptureGraphBuilt)
	return TRUE;

    // No rebuilding while we're running
    if (gcap.fCapturing || gcap.fPreviewing)
	return FALSE;

    // We don't have the necessary capture filters
    if (gcap.pVCap == NULL)
	return FALSE;
    if (gcap.pACap == NULL && gcap.fCapAudio)
	return FALSE;

    // no capture file name yet... we need one first
    if (gcap.szCaptureFile[0] == 0) {
	f = SetCaptureFile(ghwndApp);
	if (!f)
	    return f;
    }

    // we already have another graph built... tear down the old one
    if (gcap.fPreviewGraphBuilt)
	TearDownGraph();

//
// We need a rendering section that will write the capture file out in AVI
// file format
//

    WCHAR wach[_MAX_PATH];
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, gcap.szCaptureFile, -1, wach,
								_MAX_PATH);
    GUID guid = MEDIASUBTYPE_Avi;
    hr = gcap.pBuilder->SetOutputFileName(&guid, wach, &gcap.pRender,
								&gcap.pSink);
    if (hr != NOERROR) {
	ErrMsg("Cannot set output file");
	goto SetupCaptureFail;
    }

// Now tell the AVIMUX to write out AVI files that old apps can read properly.
// If we don't, most apps won't be able to tell where the keyframes are,
// slowing down editing considerably
// Doing this will cause one seek (over the area the index will go) when
// you capture past 1 Gig, but that's no big deal.
// NOTE: This is on by default, so it's not necessary to turn it on

// Also, set the proper MASTER STREAM

    hr = gcap.pRender->QueryInterface(IID_IConfigAviMux,
						(void **)&gcap.pConfigAviMux);
    if (hr == NOERROR && gcap.pConfigAviMux) {
	gcap.pConfigAviMux->SetOutputCompatibilityIndex(TRUE);
	if (gcap.fCapAudio) {
	    hr = gcap.pConfigAviMux->SetMasterStream(gcap.iMasterStream);
	    if (hr != NOERROR)
		ErrMsg("SetMasterStream failed!");
	}
    }

//
// Render the video capture and preview pins - even if the capture filter only
// has a capture pin (and no preview pin) this should work... because the
// capture graph builder will use a smart tee filter to provide both capture
// and preview.  We don't have to worry.  It will just work.
//

    hr = gcap.pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, gcap.pVCap,
						NULL, gcap.pRender);
    if (hr != NOERROR) {
	ErrMsg("Cannot render video capture stream");
	goto SetupCaptureFail;
    }

    if (gcap.fWantPreview) {
        hr = gcap.pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, gcap.pVCap,
						NULL, NULL);
        if (hr == VFW_S_NOPREVIEWPIN) {
	    // preview was faked up for us using the (only) capture pin
	    gcap.fPreviewFaked = TRUE;
	} else if (hr != S_OK) {
	    ErrMsg("Cannot render video preview stream");
	    goto SetupCaptureFail;
        }
    }

//
// Render the audio capture pin?
//

    if (gcap.fCapAudio) {
	hr = gcap.pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE,
					gcap.pACap, NULL, gcap.pRender);
	if (hr != NOERROR) {
	    ErrMsg("Cannot render audio capture stream");
	    goto SetupCaptureFail;
	}
    }

//
// Get the preview window to be a child of our app's window
//

    // This will go through a possible decoder, find the video renderer it's
    // connected to, and get the IVideoWindow interface on it
    // If the capture filter doesn't have a preview pin, and preview is being
    // faked up with a smart tee filter, the interface will actually be on
    // the capture pin, not the preview pin

    // NOTE: We do this even if we didn't ask for a preview, because rendering
    // the capture pin may have rendered the preview pin too (WDM overlay 
    // devices) because they must have a preview going.  So we better always
    // put the preview window in our app, or we may get a top level window
    // appearing out of nowhere!

	if (gcap.fPreviewFaked) {
            hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, gcap.pVCap,
				IID_IVideoWindow, (void **)&gcap.pVW);
        } else {
            hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_PREVIEW, gcap.pVCap,
				IID_IVideoWindow, (void **)&gcap.pVW);
        }
        if (hr != NOERROR && gcap.fWantPreview) {
	    ErrMsg("This graph cannot preview");
        } else if (hr == NOERROR) {
	    RECT rc;
	    gcap.pVW->put_Owner((long)ghwndApp);    // We own the window now
	    gcap.pVW->put_WindowStyle(WS_CHILD);    // you are now a child
	    // give the preview window all our space but where the status bar is
	    GetClientRect(ghwndApp, &rc);
	    cyBorder = GetSystemMetrics(SM_CYBORDER);
	    cy = statusGetHeight() + cyBorder;
	    rc.bottom -= cy;
	    gcap.pVW->SetWindowPosition(0, 0, rc.right, rc.bottom); // be this big
	    gcap.pVW->put_Visible(OATRUE);
        }

    // now tell it what frame rate to capture at.  Just find the format it
    // is capturing with, and leave everything alone but change the frame rate
    hr = gcap.fUseFrameRate ? E_FAIL : NOERROR;
    if (gcap.pVSC && gcap.fUseFrameRate) {
	hr = gcap.pVSC->GetFormat(&pmt);
	// DV capture does not use a VIDEOINFOHEADER
        if (hr == NOERROR) {
	    if (pmt->formattype == FORMAT_VideoInfo) {
	        VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pmt->pbFormat;
	        pvi->AvgTimePerFrame = (LONGLONG)(10000000 / gcap.FrameRate);
	        hr = gcap.pVSC->SetFormat(pmt);
	    }
	    DeleteMediaType(pmt);
        }
    }
    if (hr != NOERROR)
	ErrMsg("Cannot set frame rate for capture");

    // now ask the filtergraph to tell us when something is completed or aborted
    // (EC_COMPLETE, EC_USERABORT, EC_ERRORABORT).  This is how we will find out
    // if the disk gets full while capturing
    hr = gcap.pFg->QueryInterface(IID_IMediaEventEx, (void **)&gcap.pME);
    if (hr == NOERROR) {
	gcap.pME->SetNotifyWindow((LONG)ghwndApp, WM_FGNOTIFY, 0);
    }

// All done.

    // potential debug output - what the graph looks like
    // DumpGraph(gcap.pFg, 1);

    gcap.fCaptureGraphBuilt = TRUE;
    return TRUE;

SetupCaptureFail:
    TearDownGraph();
    return FALSE;
}



// build the preview graph!
//
// !!! PLEASE NOTE !!!  Some new WDM devices have totally separate capture
// and preview settings.  An application that wishes to preview and then 
// capture may have to set the preview pin format using IAMStreamConfig on the
// preview pin, and then again on the capture pin to capture with that format.
// In this sample app, there is a separate page to set the settings on the 
// capture pin and one for the preview pin.  To avoid the user
// having to enter the same settings in 2 dialog boxes, an app can have its own
// UI for choosing a format (the possible formats can be enumerated using
// IAMStreamConfig) and then the app can programmatically call IAMStreamConfig
// to set the format on both pins.
//
BOOL BuildPreviewGraph()
{
    int cy, cyBorder;
    HRESULT hr;
    AM_MEDIA_TYPE *pmt;
    BOOL fPreviewUsingCapturePin = FALSE;

    // we have one already
    if (gcap.fPreviewGraphBuilt)
	return TRUE;

    // No rebuilding while we're running
    if (gcap.fCapturing || gcap.fPreviewing)
	return FALSE;

    // We don't have the necessary capture filters
    if (gcap.pVCap == NULL)
	return FALSE;
    if (gcap.pACap == NULL && gcap.fCapAudio)
	return FALSE;

    // we already have another graph built... tear down the old one
    if (gcap.fCaptureGraphBuilt)
	TearDownGraph();

//
// Render the preview pin - even if there is not preview pin, the capture
// graph builder will use a smart tee filter and provide a preview.
//
// !!! what about latency/buffer issues?

    hr = gcap.pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, gcap.pVCap,
							NULL, NULL);
    if (hr == VFW_S_NOPREVIEWPIN) {
	// preview was faked up for us using the (only) capture pin
	gcap.fPreviewFaked = TRUE;
    } else if (hr != S_OK) {
	ErrMsg("This graph cannot preview!");
    }

//
// Get the preview window to be a child of our app's window
//

    // this will go through a possible decoder, find the video renderer it's
    // connected to, and get the IVideoWindow interface on it
    // If preview is being done through a smart tee filter, the IVideoWindow
    // interface will be found off the capture pin
    if (gcap.fPreviewFaked) {
        hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
			gcap.pVCap, IID_IVideoWindow, (void **)&gcap.pVW);
    } else {
        hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_PREVIEW,
			gcap.pVCap, IID_IVideoWindow, (void **)&gcap.pVW);
    }
    if (hr != NOERROR) {
	ErrMsg("This graph cannot preview properly");
    } else {
	RECT rc;
	gcap.pVW->put_Owner((long)ghwndApp);    // We own the window now
	gcap.pVW->put_WindowStyle(WS_CHILD);    // you are now a child
	// give the preview window all our space but where the status bar is
	GetClientRect(ghwndApp, &rc);
	cyBorder = GetSystemMetrics(SM_CYBORDER);
	cy = statusGetHeight() + cyBorder;
	rc.bottom -= cy;
	gcap.pVW->SetWindowPosition(0, 0, rc.right, rc.bottom); // be this big
	gcap.pVW->put_Visible(OATRUE);
    }

    // now tell it what frame rate to capture at.  Just find the format it
    // is capturing with, and leave everything alone but change the frame rate
    // No big deal if it fails.  It's just for preview
    // !!! Should we then talk to the preview pin?
    if (gcap.pVSC && gcap.fUseFrameRate) {
	hr = gcap.pVSC->GetFormat(&pmt);
	// DV capture does not use a VIDEOINFOHEADER
        if (hr == NOERROR) {
	    if (pmt->formattype == FORMAT_VideoInfo) {
	        VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pmt->pbFormat;
	        pvi->AvgTimePerFrame = (LONGLONG)(10000000 / gcap.FrameRate);
	        hr = gcap.pVSC->SetFormat(pmt);
		if (hr != NOERROR)
		    ErrMsg("%x: Cannot set frame rate for preview", hr);
	    }
	    DeleteMediaType(pmt);
	}
    }

// All done.

    // potential debug output - what the graph looks like
    // DumpGraph(gcap.pFg, 1);

    gcap.fPreviewGraphBuilt = TRUE;
    return TRUE;
}


// Start previewing
//
BOOL StartPreview()
{
    BOOL f = TRUE;

    // way ahead of you
    if (gcap.fPreviewing)
	return TRUE;

    if (!gcap.fPreviewGraphBuilt)
	return FALSE;

    // run the graph
    IMediaControl *pMC = NULL;
    HRESULT hr = gcap.pFg->QueryInterface(IID_IMediaControl, (void **)&pMC);
    if (SUCCEEDED(hr)) {
	hr = pMC->Run();
	if (FAILED(hr)) {
	    // stop parts that ran
	    pMC->Stop();
	}
	pMC->Release();
    }
    if (FAILED(hr)) {
	ErrMsg("Error %x: Cannot run preview graph", hr);
	return FALSE;
    }

    gcap.fPreviewing = TRUE;
    return TRUE;
}


// stop the preview graph
//
BOOL StopPreview()
{
    // way ahead of you
    if (!gcap.fPreviewing) {
	return FALSE;
    }

    // stop the graph
    IMediaControl *pMC = NULL;
    HRESULT hr = gcap.pFg->QueryInterface(IID_IMediaControl, (void **)&pMC);
    if (SUCCEEDED(hr)) {
	hr = pMC->Stop();
	pMC->Release();
    }
    if (FAILED(hr)) {
	ErrMsg("Error %x: Cannot stop preview graph", hr);
	return FALSE;
    }

    gcap.fPreviewing = FALSE;

    // !!! get rid of menu garbage
    InvalidateRect(ghwndApp, NULL, TRUE);

    return TRUE;
}


// start the capture graph
//
BOOL StartCapture()
{
    BOOL f, fHasStreamControl;
    HRESULT hr;

    // way ahead of you
    if (gcap.fCapturing)
	return TRUE;

    // or we'll get confused
    if (gcap.fPreviewing)
	StopPreview();

    // or we'll crash
    if (!gcap.fCaptureGraphBuilt)
	return FALSE;

    // This amount will be subtracted from the number of dropped and not 
    // dropped frames reported by the filter.  Since we might be having the
    // filter running while the pin is turned off, we don't want any of the
    // frame statistics from the time the pin is off interfering with the
    // statistics we gather while the pin is on
    gcap.lDroppedBase = 0;
    gcap.lNotBase = 0;

    REFERENCE_TIME start = MAX_TIME, stop = MAX_TIME;
    // show us a preview first? but don't capture quite yet...
    hr = gcap.pBuilder->ControlStream(&PIN_CATEGORY_PREVIEW, NULL,
				gcap.fWantPreview ? NULL : &start,
				gcap.fWantPreview ? &stop : NULL, 0, 0);
    if (SUCCEEDED(hr))
	hr = gcap.pBuilder->ControlStream(&PIN_CATEGORY_CAPTURE, NULL, &start,
								NULL, 0, 0);

    // Do we have the ability to control capture and preview separately?
    fHasStreamControl = SUCCEEDED(hr);

    // prepare to run the graph
    IMediaControl *pMC = NULL;
    hr = gcap.pFg->QueryInterface(IID_IMediaControl, (void **)&pMC);
    if (FAILED(hr)) {
	ErrMsg("Error %x: Cannot get IMediaControl", hr);
	return FALSE;
    }

    // If we were able to turn preview on yet keep capture off, then we can
    // run the graph now for frame accurate start later yet still showing a
    // preview.   Otherwise, we can't run the graph yet without capture
    // starting too, so we'll pause it so the latency between when they
    // press a key and when capture begins is still small (but they won't have
    // a preview while they wait to press a key)
    //
    // If there is no stream control, you're going to get a preview while
    // capturing, even if preview isn't on.  I'm not bothering to disconnect
    // the preview pin to prevent this, but I could.

    if (fHasStreamControl)
	hr = pMC->Run();
    else
	hr = pMC->Pause();
    if (FAILED(hr)) {
	// stop parts that started
	pMC->Stop();
	pMC->Release();
	ErrMsg("Error %x: Cannot start graph", hr);
	return FALSE;
    }

    // press a key to start capture
    f = DoDialog(ghwndApp, IDD_PressAKeyDialog, (DLGPROC)PressAKeyProc, 0);
    if (!f) {
	pMC->Stop();
	pMC->Release();
	if (gcap.fWantPreview) {
	    BuildPreviewGraph();
	    StartPreview();
	}
	return f;
    }

    // Start capture NOW!
    if (fHasStreamControl) {
	// we may not have this yet
        if (!gcap.pDF) {
	    hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, gcap.pVCap,
				   IID_IAMDroppedFrames, (void **)&gcap.pDF);
	}

	// turn the capture pin on now!
	gcap.pBuilder->ControlStream(&PIN_CATEGORY_CAPTURE, NULL, NULL, &stop,
									0, 0);
	// make note of the current dropped frame counts
	if (gcap.pDF) {
	    gcap.pDF->GetNumDropped(&gcap.lDroppedBase);
	    gcap.pDF->GetNumNotDropped(&gcap.lNotBase);
    	    //DbgLog((LOG_TRACE,0,TEXT("Dropped counts are %ld and %ld"),
	    //		gcap.lDroppedBase, gcap.lNotBase));
        } 
    } else {
	hr = pMC->Run();
	if (FAILED(hr)) {
	    // stop parts that started
	    pMC->Stop();
	    pMC->Release();
	    ErrMsg("Error %x: Cannot run graph", hr);
	    return FALSE;
	}
    }

    pMC->Release();

    // when did we start capture?
    gcap.lCapStartTime = timeGetTime();

    // 30 times a second I want to update my status bar - #captured, #dropped
    SetTimer(ghwndApp, 1, 33, NULL);

    gcap.fCapturing = TRUE;
    return TRUE;
}


// stop the capture graph
//
BOOL StopCapture()
{
    // way ahead of you
    if (!gcap.fCapturing) {
	return FALSE;
    }

    // stop the graph
    IMediaControl *pMC = NULL;
    HRESULT hr = gcap.pFg->QueryInterface(IID_IMediaControl, (void **)&pMC);
    if (SUCCEEDED(hr)) {
	hr = pMC->Stop();
	pMC->Release();
    }
    if (FAILED(hr)) {
	ErrMsg("Error %x: Cannot stop graph", hr);
	return FALSE;
    }

    // when the graph was stopped
    gcap.lCapStopTime = timeGetTime();

    // no more status bar updates
    KillTimer(ghwndApp, 1);

    // one last time for the final count and all the stats
    UpdateStatus(TRUE);

    gcap.fCapturing = FALSE;

    // !!! get rid of menu garbage
    InvalidateRect(ghwndApp, NULL, TRUE);

    return TRUE;
}


// Let's talk about UI for a minute.  There are many programmatic interfaces
// you can use to program a capture filter or related filter to capture the
// way you want it to.... eg:  IAMStreamConfig, IAMVideoCompression,
// IAMCrossbar, IAMTVTuner, IAMTVAudio, IAMAnalogVideoDecoder, IAMCameraControl,
// IAMVideoProcAmp, etc.
//
// But you probably want some UI to let the user play with all these settings.
// For new WDM-style capture devices, we offer some default UI you can use.
// The code below shows how to bring up all of the dialog boxes supported 
// by any capture filters.
//
// The following code shows you how you can bring up all of the
// dialogs supported by a particular object at once on a big page with lots
// of thumb tabs.  You do this by starting with an interface on the object that
// you want, and using ISpecifyPropertyPages to get the whole list, and
// OleCreatePropertyFrame to bring them all up.  This way you will get custom
// property pages a filter has, too, that are not one of the standard pages that
// you know about.  There are at least 9 objects that may have property pages.
// Your app already has 2 of the object pointers, the video capture filter and
// the audio capture filter (let's call them pVCap and pACap)
// 1.  The video capture filter - pVCap
// 2.  The video capture filter's capture pin - get this by calling
//     FindInterface(&PIN_CATEGORY_CAPTURE, pVCap, IID_IPin, &pX);
// 3.  The video capture filter's preview pin - get this by calling
//     FindInterface(&PIN_CATEGORY_PREVIEW, pVCap, IID_IPin, &pX);
// 4.  The audio capture filter - pACap
// 5.  The audio capture filter's capture pin - get this by calling
//     FindInterface(&PIN_CATEGORY_CAPTURE, pACap, IID_IPin, &pX);
// 6.  The crossbar connected to the video capture filter - get this by calling
//     FindInterface(NULL, pVCap, IID_IAMCrossbar, &pX);
// 7.  There is a possible second crossbar to control audio - get this by 
//     looking upstream of the first crossbar like this:
//     FindInterface(&LOOK_UPSTREAM_ONLY, pX, IID_IAMCrossbar, &pX2);
// 8.  The TV Tuner connected to the video capture filter - get this by calling
//     FindInterface(NULL, pVCap, IID_IAMTVTuner, &pX);
// 9.  The TV Audio connected to the audio capture filter - get this by calling
//     FindInterface(NULL, pACap, IID_IAMTVAudio, &pX);
//
// Your last choice for UI is to make your own pages, and use the results of 
// your custom page to call the interfaces programmatically.


void MakeMenuOptions()
{
    HRESULT hr;
    HMENU hMenuSub = GetSubMenu(GetMenu(ghwndApp), 2); // Options menu

    // remove any old choices from the last device
    RemoveMenu(hMenuSub, 4, MF_BYPOSITION);
    RemoveMenu(hMenuSub, 4, MF_BYPOSITION);
    RemoveMenu(hMenuSub, 4, MF_BYPOSITION);
    RemoveMenu(hMenuSub, 4, MF_BYPOSITION);
    RemoveMenu(hMenuSub, 4, MF_BYPOSITION);
    RemoveMenu(hMenuSub, 4, MF_BYPOSITION);
    RemoveMenu(hMenuSub, 4, MF_BYPOSITION);
    RemoveMenu(hMenuSub, 4, MF_BYPOSITION);
    RemoveMenu(hMenuSub, 4, MF_BYPOSITION);
    RemoveMenu(hMenuSub, 4, MF_BYPOSITION);

    int zz = 0;
    gcap.iFormatDialogPos = -1;
    gcap.iSourceDialogPos = -1;
    gcap.iDisplayDialogPos = -1;
    gcap.iVCapDialogPos = -1;
    gcap.iVCrossbarDialogPos = -1;
    gcap.iTVTunerDialogPos = -1;
    gcap.iACapDialogPos = -1;
    gcap.iACrossbarDialogPos = -1;
    gcap.iTVAudioDialogPos = -1;
    gcap.iVCapCapturePinDialogPos = -1;
    gcap.iVCapPreviewPinDialogPos = -1;
    gcap.iACapCapturePinDialogPos = -1;

    // If this device supports the old legacy UI dialogs, offer them

    if (gcap.pDlg && !gcap.pDlg->HasDialog(VfwCaptureDialog_Format)) {
	AppendMenuA(hMenuSub, MF_STRING, MENU_DIALOG0 + zz, "Video Format...");
	gcap.iFormatDialogPos = zz++;
    }
    if (gcap.pDlg && !gcap.pDlg->HasDialog(VfwCaptureDialog_Source)) {
	AppendMenuA(hMenuSub, MF_STRING, MENU_DIALOG0 + zz, "Video Source...");
	gcap.iSourceDialogPos = zz++;
    }
    if (gcap.pDlg && !gcap.pDlg->HasDialog(VfwCaptureDialog_Display)) {
	AppendMenuA(hMenuSub, MF_STRING, MENU_DIALOG0 + zz, "Video Display...");
	gcap.iDisplayDialogPos = zz++;
    }

    // don't bother looking for new property pages if the old ones are supported
    // or if we don't have a capture filter
    if (gcap.pVCap == NULL || gcap.iFormatDialogPos != -1)
	return;

    // New WDM devices support new UI and new interfaces.
    // Your app can use some default property
    // pages for UI if you'd like (like we do here) or if you don't like our
    // dialog boxes, feel free to make your own and programmatically set 
    // the capture options through interfaces like IAMCrossbar, IAMCameraControl
    // etc.

    // There are 9 objects that might support property pages.  Let's go through
    // them.

    ISpecifyPropertyPages *pSpec;
    CAUUID cauuid;

    // 1. the video capture filter itself

    hr = gcap.pVCap->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);
    if (hr == S_OK) {
        hr = pSpec->GetPages(&cauuid);
        if (hr == S_OK && cauuid.cElems > 0) {
	    AppendMenuA(hMenuSub,MF_STRING,MENU_DIALOG0+zz,"Video Capture Filter...");
	    gcap.iVCapDialogPos = zz++;
	    CoTaskMemFree(cauuid.pElems);
	}
	pSpec->Release();
    }

    // 2.  The video capture capture pin

    IAMStreamConfig *pSC;
    hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, gcap.pVCap,
				IID_IAMStreamConfig, (void **)&pSC);
    if (hr == S_OK) {
        hr = pSC->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);
        if (hr == S_OK) {
            hr = pSpec->GetPages(&cauuid);
            if (hr == S_OK && cauuid.cElems > 0) {
	        AppendMenuA(hMenuSub,MF_STRING,MENU_DIALOG0+zz,"Video Capture Pin...");
	        gcap.iVCapCapturePinDialogPos = zz++;
	        CoTaskMemFree(cauuid.pElems);
	    }
	    pSpec->Release();
        }
	pSC->Release();
    }

    // 3.  The video capture preview pin.
    // This basically sets the format being previewed.  Typically, you
    // want to capture and preview using the SAME format, instead of having to
    // enter the same value in 2 dialog boxes.  For a discussion on this, see
    // the comment above the MakePreviewGraph function.

    hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_PREVIEW, gcap.pVCap,
				IID_IAMStreamConfig, (void **)&pSC);
    if (hr == S_OK) {
        hr = pSC->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);
        if (hr == S_OK) {
            hr = pSpec->GetPages(&cauuid);
            if (hr == S_OK && cauuid.cElems > 0) {
	        AppendMenuA(hMenuSub,MF_STRING,MENU_DIALOG0+zz,"Video Preview Pin...");
	        gcap.iVCapPreviewPinDialogPos = zz++;
		CoTaskMemFree(cauuid.pElems);
	    }
	    pSpec->Release();
        }
	pSC->Release();
    }

    // 4 & 5.  The video crossbar, and a possible second crossbar

    IAMCrossbar *pX, *pX2;
    IBaseFilter *pXF;
    hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, gcap.pVCap,
				IID_IAMCrossbar, (void **)&pX);
    if (hr == S_OK) {
        hr = pX->QueryInterface(IID_IBaseFilter, (void **)&pXF);
        if (hr == S_OK) {
            hr = pX->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);
            if (hr == S_OK) {
                hr = pSpec->GetPages(&cauuid);
                if (hr == S_OK && cauuid.cElems > 0) {
	            AppendMenuA(hMenuSub,MF_STRING,MENU_DIALOG0+zz,
							"Video Crossbar...");
	            gcap.iVCrossbarDialogPos = zz++;
		    CoTaskMemFree(cauuid.pElems);
	        }
	        pSpec->Release();
            }
            hr = gcap.pBuilder->FindInterface(&LOOK_UPSTREAM_ONLY, pXF,
				IID_IAMCrossbar, (void **)&pX2);
            if (hr == S_OK) {
                hr = pX2->QueryInterface(IID_ISpecifyPropertyPages,
							(void **)&pSpec);
                if (hr == S_OK) {
                    hr = pSpec->GetPages(&cauuid);
                    if (hr == S_OK && cauuid.cElems > 0) {
	                AppendMenuA(hMenuSub,MF_STRING,MENU_DIALOG0+zz,
							"Second Crossbar...");
	                gcap.iACrossbarDialogPos = zz++;
		        CoTaskMemFree(cauuid.pElems);
	            }
	            pSpec->Release();
                }
	        pX2->Release();
	    }
 	    pXF->Release();
        }
	pX->Release();
    }

    // 6.  The TVTuner

    IAMTVTuner *pTV;
    hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, gcap.pVCap,
				IID_IAMTVTuner, (void **)&pTV);
    if (hr == S_OK) {
        hr = pTV->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);
        if (hr == S_OK) {
            hr = pSpec->GetPages(&cauuid);
            if (hr == S_OK && cauuid.cElems > 0) {
	        AppendMenuA(hMenuSub,MF_STRING,MENU_DIALOG0+zz,"TV Tuner...");
	        gcap.iTVTunerDialogPos = zz++;
		CoTaskMemFree(cauuid.pElems);
	    }
	    pSpec->Release();
        }
 	pTV->Release();
    }

    // no audio capture, we're done
    if (gcap.pACap == NULL)
	return;

    // 7.  The Audio capture filter itself

    hr = gcap.pACap->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);
    if (hr == S_OK) {
        hr = pSpec->GetPages(&cauuid);
        if (hr == S_OK && cauuid.cElems > 0) {
            AppendMenuA(hMenuSub,MF_STRING,MENU_DIALOG0+zz,"Audio Capture Filter...");
            gcap.iACapDialogPos = zz++;
	    CoTaskMemFree(cauuid.pElems);
        }
        pSpec->Release();
    }

    // 8.  The Audio capture pin

    hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, gcap.pACap,
				IID_IAMStreamConfig, (void **)&pSC);
    if (hr == S_OK) {
        hr = pSC->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);
        if (hr == S_OK) {
            hr = pSpec->GetPages(&cauuid);
            if (hr == S_OK && cauuid.cElems > 0) {
	        AppendMenuA(hMenuSub,MF_STRING,MENU_DIALOG0+zz,"Audio Capture Pin...");
	        gcap.iACapCapturePinDialogPos = zz++;
		CoTaskMemFree(cauuid.pElems);
	    }
	    pSpec->Release();
        }
 	pSC->Release();
    }

    // 9.  The TV Audio filter

    IAMTVAudio *pTVA;
    hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, gcap.pACap,
				IID_IAMTVAudio, (void **)&pTVA);
    if (hr == S_OK) {
        hr = pTVA->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);
        if (hr == S_OK) {
            hr = pSpec->GetPages(&cauuid);
            if (hr == S_OK && cauuid.cElems > 0) {
	        AppendMenuA(hMenuSub,MF_STRING,MENU_DIALOG0+zz,"TV Audio...");
	        gcap.iTVAudioDialogPos = zz++;
		CoTaskMemFree(cauuid.pElems);
	    }
	    pSpec->Release();
        }
 	pTVA->Release();
    }
}


// how many captured/dropped so far
//
void UpdateStatus(BOOL fAllStats)
{
    HRESULT hr;
    LONG lDropped, lNot, lAvgFrameSize;
    char ach[160];

    // we use this interface to get the number of captured and dropped frames
    // NOTE:  We cannot query for this interface earlier, as it may not be
    // available until the pin is connected
    if (!gcap.pDF) {
	hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, gcap.pVCap,
				IID_IAMDroppedFrames, (void **)&gcap.pDF);
    }

    // this filter can't tell us dropped frame info.
    if (!gcap.pDF) {
	statusUpdateStatus(ghwndStatus,
				"Filter cannot report capture information");
	return;
    }

    hr = gcap.pDF->GetNumDropped(&lDropped);
    if (hr == S_OK)
	hr = gcap.pDF->GetNumNotDropped(&lNot);
    if (hr != S_OK)
	return;

    lDropped -= gcap.lDroppedBase;
    lNot -= gcap.lNotBase;

    if (!fAllStats) {
	LONG lTime = timeGetTime() - gcap.lCapStartTime;
	wsprintf(ach, "Captured %d frames (%d dropped) %d.%dsec", lNot,
				lDropped, lTime / 1000, 
				lTime / 100 - lTime / 1000 * 10);
	statusUpdateStatus(ghwndStatus, ach);
	return;
    }

    // we want all possible stats, including capture time and actual acheived
    // frame rate and data rate (as opposed to what we tried to get).  These
    // numbers are an indication that though we dropped frames just now, if we
    // chose a data rate and frame rate equal to the numbers I'm about to
    // print, we probably wouldn't drop any frames.
    
    // average size of frame captured
    hr = gcap.pDF->GetAverageFrameSize(&lAvgFrameSize);
    if (hr != S_OK)
	return;

    // how long capture lasted
    LONG lDurMS = gcap.lCapStopTime - gcap.lCapStartTime;
    double flFrame;     // acheived frame rate
    LONG lData;         // acheived data rate

    if (lDurMS > 0) {
	flFrame = (double)(LONGLONG)lNot * 1000. /
						(double)(LONGLONG)lDurMS;
	lData = (LONG)(LONGLONG)(lNot / (double)(LONGLONG)lDurMS *
				1000. * (double)(LONGLONG)lAvgFrameSize);
    } else {
	flFrame = 0.;
	lData = 0;
    }

    wsprintf(ach, "Captured %d frames in %d.%d sec (%d dropped): %d.%d fps %d.%d Meg/sec",
		lNot, lDurMS / 1000, lDurMS / 100 - lDurMS / 1000 * 10,
		lDropped, (int)flFrame,
		(int)(flFrame * 10.) - (int)flFrame * 10,
		lData / 1000000,
		lData / 1000 - (lData / 1000000 * 1000));
    statusUpdateStatus(ghwndStatus, ach);
}


// Check the devices we're currently using and make filters for them
//
void ChooseDevices(int idV, int idA)
{
    #define VERSIZE 40
    #define DESCSIZE 80
    int versize = VERSIZE;
    int descsize = DESCSIZE;
    WCHAR wachVer[VERSIZE], wachDesc[DESCSIZE];
    char achStatus[VERSIZE + DESCSIZE + 5], achDesc[DESCSIZE], achVer[VERSIZE];

    if (idV != gcap.iVideoDevice) {
	// uncheck the old, check the new
	if (gcap.iVideoDevice >= 0)     // might be uninitialized
	    CheckMenuItem(GetMenu(ghwndApp), MENU_VDEVICE0 + gcap.iVideoDevice,
								MF_UNCHECKED); 
	CheckMenuItem(GetMenu(ghwndApp), MENU_VDEVICE0 + idV, MF_CHECKED); 
    }

    if (idA != gcap.iAudioDevice) {
	// uncheck the old, check the new
	if (gcap.iAudioDevice >= 0)     // might be uninitialized
	    CheckMenuItem(GetMenu(ghwndApp), MENU_ADEVICE0 + gcap.iAudioDevice,
								MF_UNCHECKED); 
	CheckMenuItem(GetMenu(ghwndApp), MENU_ADEVICE0 + idA, MF_CHECKED); 
    }

    // they chose a new device. rebuild the graphs
    if (gcap.iVideoDevice != idV || gcap.iAudioDevice != idA) {
	gcap.iVideoDevice = idV;
	gcap.iAudioDevice = idA;
	if (gcap.fPreviewing)
	    StopPreview();
	if (gcap.fCaptureGraphBuilt || gcap.fPreviewGraphBuilt)
	    TearDownGraph();
	FreeCapFilters();
	InitCapFilters();
	if (gcap.fWantPreview) { // were we previewing?
	    BuildPreviewGraph();
	    StartPreview();
	}
	MakeMenuOptions();	// the UI choices change per device
    }

    gcap.iVideoDevice = idV;
    gcap.iAudioDevice = idA;

    // Put the video driver name in the status bar - if the filter supports
    // IAMVideoCompression::GetInfo, that's the best way to get the name and
    // the version.  Otherwise use the name we got from device enumeration
    // as a fallback.
    if (gcap.pVC) {
	HRESULT hr = gcap.pVC->GetInfo(wachVer, &versize, wachDesc, &descsize,
							NULL, NULL, NULL, NULL);
	if (hr == S_OK) {
	    WideCharToMultiByte(CP_ACP, 0, wachVer, -1, achVer, VERSIZE, NULL,
								NULL);
	    WideCharToMultiByte(CP_ACP, 0, wachDesc, -1, achDesc, DESCSIZE,
								NULL, NULL);
	    wsprintf(achStatus, "%s - %s", achDesc, achVer);
	    statusUpdateStatus(ghwndStatus, achStatus);
	    return;
	}
    }
    statusUpdateStatus(ghwndStatus, gcap.achFriendlyName);
}


// put all installed video and audio devices in the menus
//
void AddDevicesToMenu()
{
    UINT    uIndex = 0;
    HMENU   hMenuSub;
    HRESULT hr;

    hMenuSub = GetSubMenu(GetMenu(ghwndApp), 1);        // Devices menu

    // remove the bogus separator
    RemoveMenu(hMenuSub, 0, 0);

    // enumerate all video capture devices
    ICreateDevEnum *pCreateDevEnum;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
			  IID_ICreateDevEnum, (void**)&pCreateDevEnum);
    if (hr != NOERROR)
	goto EnumAudio;
    IEnumMoniker *pEm;
    hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
								&pEm, 0);
    pCreateDevEnum->Release();
    if (hr != NOERROR) {
	ErrMsg("Sorry, you have no video capture hardware");
	goto EnumAudio;
    }
    pEm->Reset();
    ULONG cFetched;
    IMoniker *pM;
    while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK)
    {
	IPropertyBag *pBag;
	hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
	if(SUCCEEDED(hr)) {
	    VARIANT var;
	    var.vt = VT_BSTR;
	    hr = pBag->Read(L"FriendlyName", &var, NULL);
	    if (hr == NOERROR) {
		char achName[80];
		WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, achName, 80,
								NULL, NULL);
		AppendMenuA(hMenuSub, MF_STRING, MENU_VDEVICE0 + uIndex,
								achName);
		SysFreeString(var.bstrVal);
	    }
	    pBag->Release();
	}
	pM->Release();
	uIndex++;
    }
    pEm->Release();

    // separate the video and audio devices
    AppendMenuA(hMenuSub, MF_SEPARATOR, 0, NULL);

EnumAudio:

    // enumerate all audio capture devices
    uIndex = 0;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
			  IID_ICreateDevEnum, (void**)&pCreateDevEnum);
    if (hr != NOERROR)
	return;
    hr = pCreateDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory,
								&pEm, 0);
    pCreateDevEnum->Release();
    if (hr != NOERROR)
	return;
    pEm->Reset();
    while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK)
    {
	IPropertyBag *pBag;
	hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
	if(SUCCEEDED(hr)) {
	    VARIANT var;
	    var.vt = VT_BSTR;
	    hr = pBag->Read(L"FriendlyName", &var, NULL);
	    if (hr == NOERROR) {
		char achName[80];
		WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, achName, 80,
								NULL, NULL);
		AppendMenuA(hMenuSub, MF_STRING, MENU_ADEVICE0 + uIndex,
								achName);
		SysFreeString(var.bstrVal);
	    }
	    pBag->Release();
	}
	pM->Release();
	uIndex++;
    }
    pEm->Release();
}



// let them pick a frame rate
//
void ChooseFrameRate()
{
    double rate = gcap.FrameRate;

    DoDialog(ghwndApp, IDD_FrameRateDialog, (DLGPROC)FrameRateProc, 0);

    HRESULT hr = E_FAIL;

    // If somebody unchecks "use frame rate" it means we will no longer
    // tell the filter what frame rate to use... it will either continue
    // using the last one, or use some default, or if you bring up a dialog
    // box that has frame rate choices, it will obey them.

    // new frame rate?
    if (gcap.fUseFrameRate && gcap.FrameRate != rate) {
	if (gcap.fPreviewing)
	    StopPreview();
	// now tell it what frame rate to capture at.  Just find the format it
	// is capturing with, and leave everything else alone
	if (gcap.pVSC) {
	    AM_MEDIA_TYPE *pmt;
	    hr = gcap.pVSC->GetFormat(&pmt);
	    // DV capture does not use a VIDEOINFOHEADER
            if (hr == NOERROR) {
		if (pmt->formattype == FORMAT_VideoInfo) {
		    VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pmt->pbFormat;
		    pvi->AvgTimePerFrame =(LONGLONG)(10000000 / gcap.FrameRate);
                    hr = gcap.pVSC->SetFormat(pmt);
                    if (hr != S_OK)
		        ErrMsg("%x: Cannot set new frame rate", hr);
		}
		DeleteMediaType(pmt);
	    }
	}
	if (hr != NOERROR)
	    ErrMsg("Cannot set frame rate for capture");
	if (gcap.fWantPreview)  // we were previewing
	    StartPreview();
    }
}


// let them set a capture time limit
//
void ChooseTimeLimit()
{
    DoDialog(ghwndApp, IDD_TimeLimitDialog, (DLGPROC)TimeLimitProc, 0);
}


// choose an audio capture format using ACM
//
void ChooseAudioFormat()
{
    ACMFORMATCHOOSE cfmt;
    DWORD dwSize;
    LPWAVEFORMATEX lpwfx;
    AM_MEDIA_TYPE *pmt;

    // there's no point if we can't set a new format
    if (gcap.pASC == NULL)
	return;

    // What's the largest format size around?
    acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, &dwSize);
    HRESULT hr = gcap.pASC->GetFormat(&pmt);
    if (hr != NOERROR)
	return;
    lpwfx = (LPWAVEFORMATEX)pmt->pbFormat;
    dwSize = max(dwSize, lpwfx->cbSize + sizeof(WAVEFORMATEX));

    // !!! This doesn't really map to the supported formats of the filter.
    // We should be using a property page based on IAMStreamConfig

    // Put up a dialog box initialized with the current format
    if (lpwfx = (LPWAVEFORMATEX)GlobalAllocPtr(GHND, dwSize)) {
	CopyMemory(lpwfx, pmt->pbFormat, pmt->cbFormat);
	_fmemset(&cfmt, 0, sizeof(ACMFORMATCHOOSE));
	cfmt.cbStruct = sizeof(ACMFORMATCHOOSE);
	cfmt.fdwStyle = ACMFORMATCHOOSE_STYLEF_INITTOWFXSTRUCT;
	// show only formats we can capture
	cfmt.fdwEnum = ACM_FORMATENUMF_HARDWARE | ACM_FORMATENUMF_INPUT;
	cfmt.hwndOwner = ghwndApp;
	cfmt.pwfx = lpwfx;
	cfmt.cbwfx = dwSize;

	// we chose a new format... so give it to the capture filter
	if (!acmFormatChoose(&cfmt)) {
	    if (gcap.fPreviewing)
		StopPreview();  // can't call IAMStreamConfig::SetFormat
				// while streaming
	    ((CMediaType *)pmt)->SetFormat((LPBYTE)lpwfx,
					lpwfx->cbSize + sizeof(WAVEFORMATEX));
	    gcap.pASC->SetFormat(pmt);  // filter will reconnect
	    if (gcap.fWantPreview)
		StartPreview();
	}
	GlobalFreePtr(lpwfx) ;
    }
    DeleteMediaType(pmt);
}


/*----------------------------------------------------------------------------*\
|    AppCommand()
|
|    Process all of our WM_COMMAND messages.
\*----------------------------------------------------------------------------*/
LONG PASCAL AppCommand (HWND hwnd, unsigned msg, WPARAM wParam, LPARAM lParam)
{
    HRESULT hr;
    int id = GET_WM_COMMAND_ID(wParam, lParam);
    switch(id)
    {
	//
	// Our about box
	//
	case MENU_ABOUT:
	    DialogBox(ghInstApp, MAKEINTRESOURCE(IDD_ABOUT), hwnd, 
							(DLGPROC)AboutDlgProc);
	    break;

	//
	// We want out of here!
	//
	case MENU_EXIT:
	    PostMessage(hwnd,WM_CLOSE,0,0L);
	    break;

	// choose a capture file
	//
	case MENU_SET_CAP_FILE:
	    SetCaptureFile(hwnd);
	    break;

	// pre-allocate the capture file
	//
	case MENU_ALLOC_CAP_FILE:
	    AllocCaptureFile(hwnd);
	    break;

	// save the capture file
	//
	case MENU_SAVE_CAP_FILE:
	    SaveCaptureFile(hwnd);
	    break;

	// start capturing
	//
	case MENU_START_CAP:
	    if (gcap.fPreviewing)
		StopPreview();
	    if (gcap.fPreviewGraphBuilt)
		TearDownGraph();
	    BuildCaptureGraph();
	    StartCapture();
	    break;

	// toggle preview
	// 
	case MENU_PREVIEW:
	    gcap.fWantPreview = !gcap.fWantPreview;
	    if (gcap.fWantPreview) {
		BuildPreviewGraph();
		StartPreview();
	    } else
		StopPreview();
	    break;

	// stop capture
	//
	case MENU_STOP_CAP:
	    StopCapture();
	    if (gcap.fWantPreview) {
		BuildPreviewGraph();
		StartPreview();
	    }
	    break;

	// select the master stream
	//
	case MENU_NOMASTER:
	    gcap.iMasterStream = -1;
	    if (gcap.pConfigAviMux) {
		hr = gcap.pConfigAviMux->SetMasterStream(gcap.iMasterStream);
		if (hr != NOERROR)
		    ErrMsg("SetMasterStream failed!");
	    }
	    break;
	case MENU_AUDIOMASTER:
	    gcap.iMasterStream = 1;
	    if (gcap.pConfigAviMux) {
		hr = gcap.pConfigAviMux->SetMasterStream(gcap.iMasterStream);
		if (hr != NOERROR)
		    ErrMsg("SetMasterStream failed!");
	    }
	    break;
	case MENU_VIDEOMASTER:
	    gcap.iMasterStream = 0;
	    if (gcap.pConfigAviMux) {
		hr = gcap.pConfigAviMux->SetMasterStream(gcap.iMasterStream);
		if (hr != NOERROR)
		    ErrMsg("SetMasterStream failed!");
	    }
	    break;

	// toggle capturing audio
	case MENU_CAP_AUDIO:
	    if (gcap.fPreviewing)
		StopPreview();
	    gcap.fCapAudio = !gcap.fCapAudio;
	    // when we capture we'll need a different graph now
	    if (gcap.fCaptureGraphBuilt || gcap.fPreviewGraphBuilt)
		TearDownGraph();
	    if (gcap.fWantPreview) {
		BuildPreviewGraph();
		StartPreview();
	    }
	    break;

	// choose the audio capture format
	//
	case MENU_AUDIOFORMAT:
	    ChooseAudioFormat();
	    break;

	// pick a frame rate
	//
	case MENU_FRAMERATE:
	    ChooseFrameRate();
	    break;

	// pick a time limit
	//
	case MENU_TIMELIMIT:
	    ChooseTimeLimit();
	    break;

	// pick which video capture device to use
	// pick which video capture device to use
	//
	case MENU_VDEVICE0:
	case MENU_VDEVICE1:
	case MENU_VDEVICE2:
	case MENU_VDEVICE3:
	case MENU_VDEVICE4:
	case MENU_VDEVICE5:
	case MENU_VDEVICE6:
	case MENU_VDEVICE7:
	case MENU_VDEVICE8:
	case MENU_VDEVICE9:
	    ChooseDevices(id - MENU_VDEVICE0, gcap.iAudioDevice);
	    break;

	// pick which audio capture device to use
	//
	case MENU_ADEVICE0:
	case MENU_ADEVICE1:
	case MENU_ADEVICE2:
	case MENU_ADEVICE3:
	case MENU_ADEVICE4:
	case MENU_ADEVICE5:
	case MENU_ADEVICE6:
	case MENU_ADEVICE7:
	case MENU_ADEVICE8:
	case MENU_ADEVICE9:
	    ChooseDevices(gcap.iVideoDevice, id - MENU_ADEVICE0);
	    break;

	// video format dialog
	//
	case MENU_DIALOG0:
	case MENU_DIALOG1:
	case MENU_DIALOG2:
	case MENU_DIALOG3:
	case MENU_DIALOG4:
	case MENU_DIALOG5:
	case MENU_DIALOG6:
	case MENU_DIALOG7:
	case MENU_DIALOG8:
	case MENU_DIALOG9:

 	    // they want the VfW format dialog
	    if (id - MENU_DIALOG0 == gcap.iFormatDialogPos) {
		// this dialog will not work while previewing
		if (gcap.fWantPreview)
		    StopPreview();
		HRESULT hrD;
	        hrD = gcap.pDlg->ShowDialog(VfwCaptureDialog_Format, ghwndApp);
		// Oh uh!  Sometimes bringing up the FORMAT dialog can result
		// in changing to a capture format that the current graph 
		// can't handle.  It looks like that has happened and we'll
		// have to rebuild the graph.
		if (hrD == VFW_E_CANNOT_CONNECT) {
    		    DbgLog((LOG_TRACE,1,TEXT("DIALOG CORRUPTED GRAPH!")));
		    TearDownGraph();	// now we need to rebuild
		    // !!! This won't work if we've left a stranded h/w codec
		}

		// Resize our window to be the same size that we're capturing
	        if (gcap.pVSC) {
		    AM_MEDIA_TYPE *pmt;
		    // get format being used NOW
		    hr = gcap.pVSC->GetFormat(&pmt);
	    	    // DV capture does not use a VIDEOINFOHEADER
            	    if (hr == NOERROR) {
	 		if (pmt->formattype == FORMAT_VideoInfo) {
		            // resize our window to the new capture size
		            ResizeWindow(HEADER(pmt->pbFormat)->biWidth,
					ABS(HEADER(pmt->pbFormat)->biHeight));
			}
		        DeleteMediaType(pmt);
		    }
	        }

	        if (gcap.fWantPreview) {
		    BuildPreviewGraph();
		    StartPreview();
		}
	    } else if (id - MENU_DIALOG0 == gcap.iSourceDialogPos) {
		// this dialog will not work while previewing
		if (gcap.fWantPreview)
		    StopPreview();
	        gcap.pDlg->ShowDialog(VfwCaptureDialog_Source, ghwndApp);
	        if (gcap.fWantPreview)
		    StartPreview();
	    } else if (id - MENU_DIALOG0 == gcap.iDisplayDialogPos) {
		// this dialog will not work while previewing
		if (gcap.fWantPreview)
		    StopPreview();
	        gcap.pDlg->ShowDialog(VfwCaptureDialog_Display, ghwndApp);
	        if (gcap.fWantPreview)
		    StartPreview();

	    // now the code for the new dialogs

	    } else if (id - MENU_DIALOG0 == gcap.iVCapDialogPos) {
		ISpecifyPropertyPages *pSpec;
		CAUUID cauuid;
    	        hr = gcap.pVCap->QueryInterface(IID_ISpecifyPropertyPages,
							(void **)&pSpec);
    		if (hr == S_OK) {
        	    hr = pSpec->GetPages(&cauuid);
                    hr = OleCreatePropertyFrame(ghwndApp, 30, 30, NULL, 1,
                    (IUnknown **)&gcap.pVCap, cauuid.cElems,
		    (GUID *)cauuid.pElems, 0, 0, NULL);
		    CoTaskMemFree(cauuid.pElems);
		    pSpec->Release();
		}

	    } else if (id - MENU_DIALOG0 == gcap.iVCapCapturePinDialogPos) {
		// You can change this pin's output format in these dialogs.
		// If the capture pin is already connected to somebody who's 
		// fussy about the connection type, that may prevent using 
		// this dialog(!) because the filter it's connected to might not
		// allow reconnecting to a new format.  This might happen if
		// we are dealing with a filter with only one output pin,
		// and we are previewing by splitting its output.  In such
		// a case, I need to tear down the graph downstream of the
		// capture filter before bringing up these dialogs.
		// Don't worry about a normal capture graph where the capture
		// pin is connected to the AVI MUX; it will accept any fmt
		// change.
		// In any case, the graph must be STOPPED when calling them.
		if (gcap.fWantPreview)
		    StopPreview();	// make sure graph is stopped
		if (gcap.fPreviewFaked) {
    		    DbgLog((LOG_TRACE,1,TEXT("Tear down graph for dialog")));
		    TearDownGraph();	// graph could prevent dialog working
		}
    		IAMStreamConfig *pSC;
    		hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
				gcap.pVCap, IID_IAMStreamConfig, (void **)&pSC);
		ISpecifyPropertyPages *pSpec;
		CAUUID cauuid;
    	        hr = pSC->QueryInterface(IID_ISpecifyPropertyPages,
							(void **)&pSpec);
    		if (hr == S_OK) {
        	    hr = pSpec->GetPages(&cauuid);
                    hr = OleCreatePropertyFrame(ghwndApp, 30, 30, NULL, 1,
                    (IUnknown **)&pSC, cauuid.cElems,
		    (GUID *)cauuid.pElems, 0, 0, NULL);

		    // !!! What if changing output formats couldn't reconnect
		    // and the graph is broken?  Shouldn't be possible...
		
	            if (gcap.pVSC) {
		        AM_MEDIA_TYPE *pmt;
		        // get format being used NOW
		        hr = gcap.pVSC->GetFormat(&pmt);
	    	        // DV capture does not use a VIDEOINFOHEADER
            	        if (hr == NOERROR) {
	 		    if (pmt->formattype == FORMAT_VideoInfo) {
		                // resize our window to the new capture size
		                ResizeWindow(HEADER(pmt->pbFormat)->biWidth,
					  ABS(HEADER(pmt->pbFormat)->biHeight));
			    }
		            DeleteMediaType(pmt);
		        }
	            }

		    CoTaskMemFree(cauuid.pElems);
		    pSpec->Release();
		}
		pSC->Release();
	        if (gcap.fWantPreview) {
		    BuildPreviewGraph();
		    StartPreview();
		}

	    } else if (id - MENU_DIALOG0 == gcap.iVCapPreviewPinDialogPos) {
		// this dialog may not work if the preview pin is connected
		// already, because the downstream filter may reject a format
		// change, so we better kill the graph.
		if (gcap.fWantPreview) {
		    StopPreview();
		    TearDownGraph();
		}
    		IAMStreamConfig *pSC;
		// This dialog changes the preview format, so it might affect
		// the format being drawn.  Our app's window size is taken
		// from the size of the capture pin's video, not the preview
		// pin, so changing that here won't have any effect. All in all,
		// this probably won't be a terribly useful dialog in this app.
    		hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_PREVIEW,
				gcap.pVCap, IID_IAMStreamConfig, (void **)&pSC);
		ISpecifyPropertyPages *pSpec;
		CAUUID cauuid;
    	        hr = pSC->QueryInterface(IID_ISpecifyPropertyPages,
							(void **)&pSpec);
    		if (hr == S_OK) {
        	    hr = pSpec->GetPages(&cauuid);
                    hr = OleCreatePropertyFrame(ghwndApp, 30, 30, NULL, 1,
                    (IUnknown **)&pSC, cauuid.cElems,
		    (GUID *)cauuid.pElems, 0, 0, NULL);
		    CoTaskMemFree(cauuid.pElems);
		    pSpec->Release();
		}
		pSC->Release();
		if (gcap.fWantPreview) {
		    BuildPreviewGraph();
		    StartPreview();
		}

	    } else if (id - MENU_DIALOG0 == gcap.iVCrossbarDialogPos) {
    		IAMCrossbar *pX;
    		hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
				gcap.pVCap, IID_IAMCrossbar, (void **)&pX);
		ISpecifyPropertyPages *pSpec;
		CAUUID cauuid;
    	        hr = pX->QueryInterface(IID_ISpecifyPropertyPages,
							(void **)&pSpec);
    		if (hr == S_OK) {
        	    hr = pSpec->GetPages(&cauuid);
                    hr = OleCreatePropertyFrame(ghwndApp, 30, 30, NULL, 1,
                    (IUnknown **)&pX, cauuid.cElems,
		    (GUID *)cauuid.pElems, 0, 0, NULL);
		    CoTaskMemFree(cauuid.pElems);
		    pSpec->Release();
		}
		pX->Release();

	    } else if (id - MENU_DIALOG0 == gcap.iTVTunerDialogPos) {
    		IAMTVTuner *pTV;
    		hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
				gcap.pVCap, IID_IAMTVTuner, (void **)&pTV);
		ISpecifyPropertyPages *pSpec;
		CAUUID cauuid;
    	        hr = pTV->QueryInterface(IID_ISpecifyPropertyPages,
							(void **)&pSpec);
    		if (hr == S_OK) {
        	    hr = pSpec->GetPages(&cauuid);
                    hr = OleCreatePropertyFrame(ghwndApp, 30, 30, NULL, 1,
                    (IUnknown **)&pTV, cauuid.cElems,
		    (GUID *)cauuid.pElems, 0, 0, NULL);
		    CoTaskMemFree(cauuid.pElems);
		    pSpec->Release();
		}
		pTV->Release();

	    } else if (id - MENU_DIALOG0 == gcap.iACapDialogPos) {
		ISpecifyPropertyPages *pSpec;
		CAUUID cauuid;
    	        hr = gcap.pACap->QueryInterface(IID_ISpecifyPropertyPages,
							(void **)&pSpec);
    		if (hr == S_OK) {
        	    hr = pSpec->GetPages(&cauuid);
                    hr = OleCreatePropertyFrame(ghwndApp, 30, 30, NULL, 1,
                    (IUnknown **)&gcap.pACap, cauuid.cElems,
		    (GUID *)cauuid.pElems, 0, 0, NULL);
		    CoTaskMemFree(cauuid.pElems);
		    pSpec->Release();
		}

	    } else if (id - MENU_DIALOG0 == gcap.iACapCapturePinDialogPos) {
		// this dialog will not work while previewing - it might change
		// the output format!
		if (gcap.fWantPreview)
		    StopPreview();
    		IAMStreamConfig *pSC;
    		hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
				gcap.pACap, IID_IAMStreamConfig, (void **)&pSC);
		ISpecifyPropertyPages *pSpec;
		CAUUID cauuid;
    	        hr = pSC->QueryInterface(IID_ISpecifyPropertyPages,
							(void **)&pSpec);
    		if (hr == S_OK) {
        	    hr = pSpec->GetPages(&cauuid);
                    hr = OleCreatePropertyFrame(ghwndApp, 30, 30, NULL, 1,
                    (IUnknown **)&pSC, cauuid.cElems,
		    (GUID *)cauuid.pElems, 0, 0, NULL);
		    CoTaskMemFree(cauuid.pElems);
		    pSpec->Release();
		}
		pSC->Release();
	        if (gcap.fWantPreview)
		    StartPreview();

	    } else if (id - MENU_DIALOG0 == gcap.iACrossbarDialogPos) {
    		IAMCrossbar *pX, *pX2;
		IBaseFilter *pXF;
		// we could use better error checking here... I'm assuming
		// this won't fail
    		hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
				gcap.pVCap, IID_IAMCrossbar, (void **)&pX);
		hr = pX->QueryInterface(IID_IBaseFilter, (void **)&pXF);
    		hr = gcap.pBuilder->FindInterface(&LOOK_UPSTREAM_ONLY,
				pXF, IID_IAMCrossbar, (void **)&pX2);
		ISpecifyPropertyPages *pSpec;
		CAUUID cauuid;
    	        hr = pX2->QueryInterface(IID_ISpecifyPropertyPages,
							(void **)&pSpec);
    		if (hr == S_OK) {
        	    hr = pSpec->GetPages(&cauuid);
                    hr = OleCreatePropertyFrame(ghwndApp, 30, 30, NULL, 1,
                    (IUnknown **)&pX2, cauuid.cElems,
		    (GUID *)cauuid.pElems, 0, 0, NULL);
		    CoTaskMemFree(cauuid.pElems);
		    pSpec->Release();
		}
		pX2->Release();
		pXF->Release();
		pX->Release();

	    } else if (id - MENU_DIALOG0 == gcap.iTVAudioDialogPos) {
    		IAMTVAudio *pTVA;
    		hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
				gcap.pACap, IID_IAMTVAudio, (void **)&pTVA);
		ISpecifyPropertyPages *pSpec;
		CAUUID cauuid;
    	        hr = pTVA->QueryInterface(IID_ISpecifyPropertyPages,
							(void **)&pSpec);
    		if (hr == S_OK) {
        	    hr = pSpec->GetPages(&cauuid);
                    hr = OleCreatePropertyFrame(ghwndApp, 30, 30, NULL, 1,
                    (IUnknown **)&pTVA, cauuid.cElems,
		    (GUID *)cauuid.pElems, 0, 0, NULL);
		    CoTaskMemFree(cauuid.pElems);
		    pSpec->Release();
		}
		pTVA->Release();
	    }

	    break;

    }
    return 0L;
}


/*----------------------------------------------------------------------------*\
|   ErrMsg - Opens a Message box with a error message in it.  The user can     |
|            select the OK button to continue                                  |
\*----------------------------------------------------------------------------*/
int ErrMsg (LPTSTR sz,...)
{
    static TCHAR ach[2000];
    va_list va;

    va_start(va, sz);
    wvsprintf (ach,sz, va);
    va_end(va);
    MessageBox(ghwndApp,ach,NULL, MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
    return FALSE;
}


/* AboutDlgProc()
 *
 * Dialog Procedure for the "about" dialog box.
 *
 */

BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_COMMAND:
		EndDialog(hwnd, TRUE);
		return TRUE;
	case WM_INITDIALOG:
		return TRUE;
	}
	return FALSE;
}


// pre-allocate the capture file
//
BOOL AllocCaptureFile(HWND hWnd)
{
    // we'll get into an infinite loop in the dlg proc setting a value
    if (gcap.szCaptureFile[0] == 0)
	return FALSE;

    /*
     * show the allocate file space dialog to encourage
     * the user to pre-allocate space
     */
    if (DoDialog(hWnd, IDD_AllocCapFileSpace, (DLGPROC)AllocCapFileProc, 0)) {

	// ensure repaint after dismissing dialog before
	// possibly lengthy operation
	UpdateWindow(ghwndApp);

	// User has hit OK. Alloc requested capture file space
	BOOL f = MakeBuilder();
	if (!f)
	    return FALSE;
	WCHAR wach[_MAX_PATH];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, gcap.szCaptureFile, -1,
							wach, _MAX_PATH);
	if (gcap.pBuilder->AllocCapFile(wach,
		(DWORDLONG)gcap.wCapFileSize * 1024L * 1024L) != NOERROR) {
	    MessageBoxA(ghwndApp, "Error",
				"Failed to pre-allocate capture file space",
				MB_OK | MB_ICONEXCLAMATION);
	    return FALSE;
	}
	return TRUE;
    } else {
	return FALSE;
    }
}


/*
 * Put up the open file dialog
 */
BOOL OpenFileDialog(HWND hWnd, LPSTR lpName, int cb)
{
    OPENFILENAMEA ofn;
    LPSTR p;
    char         achFileName[_MAX_PATH];
    char         achBuffer[_MAX_PATH] ;

    if (lpName == NULL || cb <= 0)
	return FALSE;

    // start with capture file as current file name
    achFileName[0] = 0;
    lstrcpy(achFileName, gcap.szCaptureFile);

    // Get just the path info
    // Terminate the full path at the last backslash
    lstrcpy(achBuffer, achFileName);
    for (p = achBuffer + lstrlen(achBuffer); p > achBuffer; p--) {
	if (*p == '\\') {
	    *(p+1) = '\0';
	    break;
	}
    }

    _fmemset(&ofn, 0, sizeof(OPENFILENAME)) ;
    ofn.lStructSize = sizeof(OPENFILENAME) ;
    ofn.hwndOwner = hWnd ;
    ofn.lpstrFilter = "Microsoft AVI\0*.avi\0\0";
    ofn.nFilterIndex = 0 ;
    ofn.lpstrFile = achFileName;
    ofn.nMaxFile = sizeof(achFileName) ;
    ofn.lpstrFileTitle = NULL;
    ofn.lpstrTitle = "Set Capture File";
    ofn.nMaxFileTitle = 0 ;
    ofn.lpstrInitialDir = achBuffer;
    ofn.Flags = OFN_HIDEREADONLY | OFN_NOREADONLYRETURN | OFN_PATHMUSTEXIST ;

    if (GetOpenFileNameA(&ofn)) {
	// We have a capture file name
	lstrcpyn(lpName, achFileName, cb);
	return TRUE;
    } else {
	return FALSE;
    }
}


/*
 * Put up a dialog to allow the user to select a capture file.
 */
BOOL SetCaptureFile(HWND hWnd)
{
    if (OpenFileDialog(hWnd, gcap.szCaptureFile, _MAX_PATH)) {
	OFSTRUCT os;

	// We have a capture file name

	/*
	 * if this is a new file, then invite the user to
	 * allocate some space
	 */
	if (OpenFile(gcap.szCaptureFile, &os, OF_EXIST) == HFILE_ERROR) {

	    // bring up dialog, and set new file size
	    BOOL f = AllocCaptureFile(hWnd);
	    if (!f)
		return FALSE;
	}
    } else {
	return FALSE;
    }

    SetAppCaption();    // new a new app caption

    // tell the file writer to use the new filename
    if (gcap.pSink) {
	WCHAR wach[_MAX_PATH];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, gcap.szCaptureFile, -1,
							wach, _MAX_PATH);
	gcap.pSink->SetFileName(wach, NULL);
    }

    return TRUE;
}


/*
 * Put up a dialog to allow the user to save the contents of the capture file
 * elsewhere
 */
BOOL SaveCaptureFile(HWND hWnd)
{
    HRESULT hr;
    char achDstFile[_MAX_PATH];
    WCHAR wachDstFile[_MAX_PATH];
    WCHAR wachSrcFile[_MAX_PATH];

    if (gcap.pBuilder == NULL)
	return FALSE;

    if (OpenFileDialog(hWnd, achDstFile, _MAX_PATH)) {

	// We have a capture file name
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, gcap.szCaptureFile, -1,
						wachSrcFile, _MAX_PATH);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, achDstFile, -1,
						wachDstFile, _MAX_PATH);
	statusUpdateStatus(ghwndStatus, "Saving capture file - please wait...");

	// we need our own graph builder because the main one might not exist
	ICaptureGraphBuilder *pBuilder;
	hr = CoCreateInstance((REFCLSID)CLSID_CaptureGraphBuilder,
			NULL, CLSCTX_INPROC, (REFIID)IID_ICaptureGraphBuilder,
			(void **)&pBuilder);
	if (hr == NOERROR) {
	    // allow the user to press ESC to abort... don't ask for progress
	    hr = pBuilder->CopyCaptureFile(wachSrcFile, wachDstFile,TRUE,NULL);
	    pBuilder->Release();
	}
	if (hr == S_OK)
	    statusUpdateStatus(ghwndStatus, "Capture file saved");
	else if (hr == S_FALSE)
	    statusUpdateStatus(ghwndStatus, "Capture file save aborted");
	else
	    statusUpdateStatus(ghwndStatus, "Capture file save ERROR");
	return (hr == NOERROR ? TRUE : FALSE); 

    } else {
	return TRUE;    // they cancelled or something
    }
}

// brings up a dialog box
//
int DoDialog(HWND hwndParent, int DialogID, DLGPROC fnDialog, long lParam)
{
    DLGPROC fn;
    int result;

    fn = (DLGPROC)MakeProcInstance(fnDialog, ghInstApp);
    result = DialogBoxParam(
		ghInstApp,
		MAKEINTRESOURCE(DialogID),
		hwndParent,
		fn,
		lParam);
    FreeProcInstance(fn);

    return result;
}


//
// GetFreeDiskSpace: Function to Measure Available Disk Space
//
static long GetFreeDiskSpaceInKB(LPSTR pFile)
{
    DWORD dwFreeClusters, dwBytesPerSector, dwSectorsPerCluster, dwClusters;
    char RootName[MAX_PATH];
    LPSTR ptmp;    //required arg
    ULARGE_INTEGER ulA, ulB, ulFreeBytes;

    // need to find path for root directory on drive containing
    // this file.

    GetFullPathName(pFile, sizeof(RootName), RootName, &ptmp);

    // truncate this to the name of the root directory (god how tedious)
    if (RootName[0] == '\\' && RootName[1] == '\\') {

	// path begins with  \\server\share\path so skip the first
	// three backslashes
	ptmp = &RootName[2];
	while (*ptmp && (*ptmp != '\\')) {
	    ptmp++;
	}
	if (*ptmp) {
	    // advance past the third backslash
	    ptmp++;
	}
    } else {
	// path must be drv:\path
	ptmp = RootName;
    }

    // find next backslash and put a null after it
    while (*ptmp && (*ptmp != '\\')) {
	ptmp++;
    }
    // found a backslash ?
    if (*ptmp) {
	// skip it and insert null
	ptmp++;
	*ptmp = '\0';
    }

    // the only real way of finding out free disk space is calling
    // GetDiskFreeSpaceExA, but it doesn't exist on Win95

    HINSTANCE h = LoadLibraryA("kernel32.dll");
    if (h) {
	typedef BOOL (WINAPI *MyFunc)(LPCSTR RootName, PULARGE_INTEGER pulA, PULARGE_INTEGER pulB, PULARGE_INTEGER pulFreeBytes);
	MyFunc pfnGetDiskFreeSpaceEx = (MyFunc)GetProcAddress(h,
						"GetDiskFreeSpaceExA");
	FreeLibrary(h);
	if (pfnGetDiskFreeSpaceEx) {
	    if (!pfnGetDiskFreeSpaceEx(RootName, &ulA, &ulB, &ulFreeBytes))
		return -1;
	    return (long)(ulFreeBytes.QuadPart / 1024);
	}
    }

    if (!GetDiskFreeSpace(RootName, &dwSectorsPerCluster, &dwBytesPerSector,
					&dwFreeClusters, &dwClusters))
	return (-1);
    return(MulDiv(dwSectorsPerCluster * dwBytesPerSector,
		   dwFreeClusters,
		   1024));
}



// AllocCapFileProc: Capture file Space Allocation Dialog Box Procedure
//
int FAR PASCAL AllocCapFileProc(HWND hDlg, UINT Message, UINT wParam, LONG lParam)
{
    static int      nFreeMBs = 0 ;

    switch (Message) {
	case WM_INITDIALOG:
	{
	    DWORDLONG        dwlFileSize = 0;
	    long             lFreeSpaceInKB;

	    // Get current capture file name and measure its size
	    dwlFileSize = GetSize(gcap.szCaptureFile);

	    // Get free disk space and add current capture file size to that.
	    // Convert the available space to MBs.
	    if ((lFreeSpaceInKB =
			GetFreeDiskSpaceInKB(gcap.szCaptureFile)) != -1L) {
		lFreeSpaceInKB += (long)(dwlFileSize / 1024);
		nFreeMBs = lFreeSpaceInKB / 1024 ;
		SetDlgItemInt(hDlg, IDD_SetCapFileFree, nFreeMBs, TRUE) ;
	    } else {
		EnableWindow(GetDlgItem(hDlg, IDD_SetCapFileFree), FALSE);
	    }

	    gcap.wCapFileSize = (WORD) (dwlFileSize / (1024L * 1024L));

	    SetDlgItemInt(hDlg, IDD_SetCapFileSize, gcap.wCapFileSize, TRUE) ;
	    return TRUE ;
	}

	case WM_COMMAND :
	    switch (GET_WM_COMMAND_ID(wParam, lParam)) {
		case IDOK :
		{
		    int         iCapFileSize ;

		    iCapFileSize = (int) GetDlgItemInt(hDlg, IDD_SetCapFileSize, NULL, TRUE) ;
		    if (iCapFileSize <= 0 || iCapFileSize > nFreeMBs) {
			// You are asking for more than we have !! Sorry, ...
			SetDlgItemInt(hDlg, IDD_SetCapFileSize, iCapFileSize, TRUE) ;
			SetFocus(GetDlgItem(hDlg, IDD_SetCapFileSize)) ;
			MessageBeep(MB_ICONEXCLAMATION) ;
			return FALSE ;
		    }
		    gcap.wCapFileSize = iCapFileSize ;

		    EndDialog(hDlg, TRUE) ;
		    return TRUE ;
		}

		case IDCANCEL :
		    EndDialog(hDlg, FALSE) ;
		    return TRUE ;

		case IDD_SetCapFileSize:
		{
		    long l;
		    BOOL bchanged;
		    char achBuffer[21];

		    // check that entered size is a valid number
		    GetDlgItemText(hDlg, IDD_SetCapFileSize, achBuffer, sizeof(achBuffer));
		    l = atol(achBuffer);
		    bchanged = FALSE;
		    if (l < 1) {
			l = 1;
			bchanged = TRUE;
		    // don't infinite loop if there's < 1 Meg free
		    } else if (l > nFreeMBs && nFreeMBs > 0) {
			l = nFreeMBs;
			bchanged = TRUE;
		    } else {
			// make sure there are no non-digit chars
			// atol() will ignore trailing non-digit characters
			int c = 0;
			while (achBuffer[c]) {
			    if (IsCharAlpha(achBuffer[c]) ||
				!IsCharAlphaNumeric(achBuffer[c])) {

				// string contains non-digit chars - reset
				l = 1;
				bchanged = TRUE;
				break;
			    }
			    c++;
			}
		    }
		    if (bchanged) {
			wsprintf(achBuffer, "%ld", l);
			SetDlgItemText(hDlg, IDD_SetCapFileSize, achBuffer);
		    }
		    break;
		}
	    }
	    break ;
    }

    return FALSE ;
}


//
// FrameRateProc: Choose a frame rate
//
int FAR PASCAL FrameRateProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam)
{
  char  ach[32];
  
  switch (msg) {
    case WM_INITDIALOG:
	/* put the current frame rate in the box */
	sprintf(ach, "%f", gcap.FrameRate, ach);
	SetDlgItemText(hwnd, IDC_FRAMERATE, ach);
	CheckDlgButton(hwnd, IDC_USEFRAMERATE, gcap.fUseFrameRate);
	break;
	
    case WM_COMMAND:
	switch(wParam){
	    case IDCANCEL:
		EndDialog(hwnd, FALSE);
		break;
		
	    case IDOK:
		/* get the new frame rate */
		GetDlgItemText(hwnd, IDC_FRAMERATE, ach, sizeof(ach));
		if (atof(ach) <= 0.) {
		    ErrMsg("Invalid frame rate.");
		    break;
		}
		gcap.FrameRate = atof(ach);
		gcap.fUseFrameRate = IsDlgButtonChecked(hwnd, IDC_USEFRAMERATE);
		EndDialog(hwnd, TRUE);
		break;
	}
	break;
	
    default:
	return FALSE;
  }
  return TRUE;
}


//
// TimeLimitProc: Choose a capture time limit
//
int FAR PASCAL TimeLimitProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam)
{
  char  ach[32];
  DWORD   dwTimeLimit;
  
  switch (msg) {
    case WM_INITDIALOG:
	/* put the current time limit info in the boxes */
	sprintf(ach, "%d", gcap.dwTimeLimit, ach);
	SetDlgItemText(hwnd, IDC_TIMELIMIT, ach);
	CheckDlgButton(hwnd, IDC_USETIMELIMIT, gcap.fUseTimeLimit);
	break;
	
    case WM_COMMAND:
	switch(wParam){
	    case IDCANCEL:
		EndDialog(hwnd, FALSE);
		break;
		
	    case IDOK:
		/* get the new time limit */
		dwTimeLimit = GetDlgItemInt(hwnd, IDC_TIMELIMIT, NULL, FALSE);
		gcap.dwTimeLimit = dwTimeLimit;
		gcap.fUseTimeLimit = IsDlgButtonChecked(hwnd, IDC_USETIMELIMIT);
		EndDialog(hwnd, TRUE);
		break;
	}
	break;
	
    default:
	return FALSE;
  }
  return TRUE;
}


//
// PressAKeyProc: Press OK to capture
//
int FAR PASCAL PressAKeyProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam)
{
  char ach[_MAX_PATH];

  switch (msg) {
    case WM_INITDIALOG:
	/* set the current file name in the box */
	wsprintf(ach, "%s", gcap.szCaptureFile);
	SetDlgItemText(hwnd, IDC_CAPFILENAME, ach);
	break;
	
    case WM_COMMAND:
	switch(wParam){
	    case IDCANCEL:
		EndDialog(hwnd, FALSE);
		break;
		
	    case IDOK:
		EndDialog(hwnd, TRUE);
		break;
	}
	break;
	
    default:
	return FALSE;
  }
  return TRUE;
}

DWORDLONG GetSize(LPCSTR ach)
{
    HANDLE hFile = CreateFileA(ach, GENERIC_READ, FILE_SHARE_READ, 0,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile == INVALID_HANDLE_VALUE) {
	return 0;
    }

    DWORD dwSizeHigh;
    DWORD dwSizeLow = GetFileSize(hFile, &dwSizeHigh);

    DWORDLONG dwlSize = dwSizeLow + ((DWORDLONG)dwSizeHigh << 32);

    if (!CloseHandle(hFile)) {
	dwlSize = 0;
    }

    return dwlSize;
}
