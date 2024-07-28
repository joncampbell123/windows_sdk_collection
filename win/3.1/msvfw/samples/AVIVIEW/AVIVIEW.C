
/****************************************************************************
 *
 *  AVIVIEW.C
 *
 *  Sample program using the AVIFile read/write routines
 *
 ***************************************************************************/
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/


#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <vfw.h>

#include "aviview.h"
#include "audplay.h"
#include "muldivav.h"

#define GlobalSizePtr(lp)   GlobalSize(GlobalPtrHandle(lp))

/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/
typedef LONG (FAR PASCAL *LPWNDPROC)(HWND, UINT, WPARAM, LPARAM); // pointer to a window procedure

/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/
static  char        gszAppName[]="AVIView";

static  char	    gachFilter[512];	// for AVIBuildFilter

static  HINSTANCE   ghInstApp;
static  HWND        ghwndApp;
static  HACCEL	    ghAccel;

#define SCROLLRANGE  10000

#define MAXNUMSTREAMS	25
PAVIFILE	    gpfile;			// the current file
PAVISTREAM          gapavi[MAXNUMSTREAMS];	// the current streams
AVICOMPRESSOPTIONS  gaAVIOptions[MAXNUMSTREAMS];// compression options
LPAVICOMPRESSOPTIONS  galpAVIOptions[MAXNUMSTREAMS];
PGETFRAME	    gapgf[MAXNUMSTREAMS];	// data for decompressing video
HDRAWDIB	    ghdd[MAXNUMSTREAMS];	// drawdib handles
int		    gcpavi;			// # of streams

PAVISTREAM          gpaviAudio;                 // 1st audio stream found
PAVISTREAM          gpaviVideo;                 // 1st video stream found

#define             gfVideoFound (gpaviVideo != NULL)
#define             gfAudioFound (gpaviAudio != NULL)

BOOL		    gfPlaying;		// are we currently playing?
LONG		    glPlayStartTime;	// When did we start playing?
LONG		    glPlayStartPos;	// Where were we on the scrollbar?

LONG                timeStart;		// cached start, end, length
LONG                timeEnd;
LONG                timeLength;
LONG		    timehscroll;	// how much arrows scroll HORZ bar
LONG		    vertSBLen;		// vertical scroll bar
LONG		    vertHeight;


DWORD		    gdwMicroSecPerPixel = 1000L;	// scale for video

char                gachFileName[128] = "";
char                gachSaveFileName[128] = "";
WORD		    gwZoom = 2;		// one-half zoom (divide by 4)

HFONT               hfontApp;
TEXTMETRIC          tm;
BYTE		    abFormat[1024];

// buffer for wave data
LPVOID lpAudio;


				// constants for painting
            #define VSPACE  8	// some vertical spacing
            #define HSPACE  4	// space between frames for video stream
            #define TSPACE  20	// space for text area about each stream
            #define AUDIOVSPACE  64	// height of an audio stream at X1 zoom

/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

// Macros to get and set the scroll bar to a given millisecond value in the
// movie.  Movie lengths can be DWORDS but we only have 16 bits of resolution.

#define GetScrollTime(hwnd) \
    (timeStart + muldiv32(GetScrollPos(hwnd, SB_HORZ), timeLength, SCROLLRANGE))

#define SetScrollTime(hwnd, time) SetScrollPos(hwnd, SB_HORZ, \
    (int)muldiv32((time) - timeStart, SCROLLRANGE, timeLength), TRUE)

/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

LONG FAR PASCAL _export AppWndProc (HWND hwnd, unsigned uiMessage, WORD wParam, LONG lParam);
int  ErrMsg (LPSTR sz,...);

LONG NEAR PASCAL AppCommand(HWND hwnd, unsigned msg, WORD wParam, LONG lParam);
long PaintStuff(HDC hdc, HWND hwnd, BOOL fDrawEverything);

/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

int     fWait = 0;

/*----------------------------------------------------------------------------*\
|    StartWait()								|
|										|
|    Start a wait operation... put up the hourglass if it's the first call.	|
\*----------------------------------------------------------------------------*/
void StartWait()
{
    if (fWait++ == 0) {
        SetCursor(LoadCursor(NULL,IDC_WAIT));
    }
}

/*----------------------------------------------------------------------------*\
|    EndWait()									|
|										|
|    Once every one who started a wait is finished, go back to regular cursor.	|
\*----------------------------------------------------------------------------*/
void EndWait()
{
    if (--fWait == 0) {
        SetCursor(LoadCursor(NULL,IDC_ARROW));
        InvalidateRect(ghwndApp, NULL, TRUE);
    }
}

/*----------------------------------------------------------------------------*\
|    WinYield()									|
|										|
|    Code to yield while we're not calling GetMessage.				|
|    Dispatch all messages.  Pressing ESC or closing aborts.			|
\*----------------------------------------------------------------------------*/
BOOL WinYield()
{
    MSG msg;
    BOOL fAbort=FALSE;

    while(fWait > 0 && PeekMessage(&msg,NULL,0,0,PM_REMOVE))
    {
	if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE)
            fAbort = TRUE;
	if (msg.message == WM_SYSCOMMAND && (msg.wParam & 0xFFF0) == SC_CLOSE)
	    fAbort = TRUE;
	TranslateMessage(&msg);
	DispatchMessage(&msg);
    }
    return fAbort;
}


/*----------------------------------------------------------------------------*\
|    FixScrollbars()								|
|										|
|    When we load a file or zoom changes, we re-set the scrollbars.		|
\*----------------------------------------------------------------------------*/
void FixScrollbars(HWND hwnd)
{
    LONG		lHeight = 0;
    RECT		rc;
    HDC			hdc;

    //
    // Determine how tall our window needs to be to display everything.
    //
    hdc = GetDC(NULL);
    ExcludeClipRect(hdc, 0, 0, 32767, 32767);	// don't actually draw
    lHeight = PaintStuff(hdc, hwnd, TRUE);
    ReleaseDC(NULL, hdc);

    //
    // Set vertical scrollbar for scrolling the visible area
    //
    GetClientRect(hwnd, &rc);
    vertHeight = lHeight;	// total height in pixels of entire display

    //
    // We won't fit in the window... need scrollbars
    //
    if (lHeight > rc.bottom) {
	vertSBLen = lHeight - rc.bottom;
	SetScrollRange(hwnd, SB_VERT, 0, (int)vertSBLen, TRUE);
	SetScrollPos(hwnd, SB_VERT, 0, TRUE);

    //
    // We will fit in the window!  No scrollbars necessary
    //
    } else {
	vertSBLen = 0;
	SetScrollRange(hwnd, SB_VERT, 0, 0, TRUE);
    }
}


/*----------------------------------------------------------------------------*\
|    InitStreams()								|
|										|
|    Initialize the streams of a loaded file -- the compression options, the	|
|    DrawDIB handles, and the scroll bars.					|
\*----------------------------------------------------------------------------*/
void InitStreams(HWND hwnd)
{
    AVISTREAMINFO     avis;
    LONG	lTemp;
    int		i;
    WORD	w;

    //
    // Start with bogus times
    //
    timeStart = 0x7FFFFFFF;
    timeEnd   = 0;

    //
    // Walk through and init all streams loaded
    //
    for (i = 0; i < gcpavi; i++) {

        AVIStreamInfo(gapavi[i], &avis, sizeof(avis));

	//
	// Save and SaveOptions code takes a pointer to our compression opts
	//
	galpAVIOptions[i] = &gaAVIOptions[i];

	//
	// clear options structure to zeroes
	//
	_fmemset(galpAVIOptions[i], 0, sizeof(AVICOMPRESSOPTIONS));

	//
 	// Initialize the compression options to some default stuff
	// !!! Pick something better
	//
	galpAVIOptions[i]->fccType = avis.fccType;

	switch(avis.fccType) {

	    case streamtypeVIDEO:		
		galpAVIOptions[i]->dwFlags = AVICOMPRESSF_VALID |
			AVICOMPRESSF_KEYFRAMES | AVICOMPRESSF_DATARATE;
		galpAVIOptions[i]->fccHandler = NULL;
		galpAVIOptions[i]->dwQuality = (DWORD)ICQUALITY_DEFAULT;
		galpAVIOptions[i]->dwKeyFrameEvery = 7;	// !!! ask compressor?
		galpAVIOptions[i]->dwBytesPerSecond = 60000;
		break;

	    case streamtypeAUDIO:
		galpAVIOptions[i]->dwFlags |= AVICOMPRESSF_VALID;
		galpAVIOptions[i]->dwInterleaveEvery = 5;
		AVIStreamReadFormat(gapavi[i], AVIStreamStart(gapavi[i]),
				    NULL, &lTemp);
		galpAVIOptions[i]->cbFormat = lTemp;
		if (lTemp)
		    galpAVIOptions[i]->lpFormat = GlobalAllocPtr(GHND, lTemp);
		// Use current format as default format
		if (galpAVIOptions[i]->lpFormat)
		    AVIStreamReadFormat(gapavi[i],
					AVIStreamStart(gapavi[i]),
					galpAVIOptions[i]->lpFormat,
					&lTemp);
		break;

	    default:
		break;
	}

	//
	// We're finding the earliest and latest start and end points for
	// our scrollbar.
	//
        timeStart = min(timeStart, AVIStreamStartTime(gapavi[i]));
        timeEnd   = max(timeEnd, AVIStreamEndTime(gapavi[i]));

	//
	// Initialize video streams for getting decompressed frames to display
	//
        if (avis.fccType == streamtypeVIDEO) {

	    gapgf[i] = AVIStreamGetFrameOpen(gapavi[i], NULL);

	    if (gapgf[i] == NULL)
		continue;
	    
	    ghdd[i] = DrawDibOpen();
	    // !!! DrawDibBegin?
	    
	    if (gpaviVideo == NULL) {

		//
		// Remember the first video stream --- treat it specially
		//
                gpaviVideo = gapavi[i];

		//
		// Set the horizontal scrollbar scale to show every frame
		// of the first video stream exactly once
		//
		w = (avis.rcFrame.right - avis.rcFrame.left) * gwZoom / 4 +
								    HSPACE;
		gdwMicroSecPerPixel = muldiv32(1000000,
					       avis.dwScale,
					       w * avis.dwRate);

		// Move one frame on the top video screen for each HSCROLL
		timehscroll = muldiv32(1000, avis.dwScale, avis.dwRate);
	    }

	} else if (avis.fccType == streamtypeAUDIO) {

	    //
	    // If there are no video streams, we base everything on this
	    // audio stream.
	    //
	    if (gpaviAudio == NULL && gpaviVideo == NULL) {

		// Show one sample per pixel
		gdwMicroSecPerPixel = muldiv32(1000000,
					       avis.dwScale,
					       avis.dwRate);
		// Move one sample per HSCROLL
		// Move at least enough to show movement
		timehscroll = muldiv32(1000, avis.dwScale, avis.dwRate);
	    }

	    //
	    // Remember the first audio stream --- treat it specially
	    //
	    if (gpaviAudio == NULL)
	        gpaviAudio = gapavi[i];
	}

    }

    timeLength = timeEnd - timeStart;

    if (timeLength == 0)
	timeLength = 1;

    // Make sure HSCROLL scrolls enough to be noticeable.
    timehscroll = max(timehscroll, timeLength / SCROLLRANGE + 2);

    SetScrollRange(hwnd, SB_HORZ, 0, SCROLLRANGE, TRUE);
    SetScrollTime(hwnd, timeStart);

    FixScrollbars(hwnd);
}

/*----------------------------------------------------------------------------*\
|    FixWindowTitle()								|
|										|
|    Update the window title to reflect what's loaded.				|
\*----------------------------------------------------------------------------*/
void FixWindowTitle(HWND hwnd)
{
    char ach[80];

    wsprintf(ach, "%s %s",
            (LPSTR)gszAppName,
            (LPSTR)gachFileName);

    SetWindowText(hwnd, ach);

    InvalidateRect(hwnd, NULL, TRUE);
}

/*----------------------------------------------------------------------------*\
|    FreeDrawStuff()								|
|										|
| Free up the resources associated with DrawDIB					|
\*----------------------------------------------------------------------------*/
void FreeDrawStuff(HWND hwnd)
{
    int	i;
    
    // Make sure we're not playing!
    aviaudioStop();
    
    for (i = 0; i < gcpavi; i++) {
	if (gapgf[i]) {
	    AVIStreamGetFrameClose(gapgf[i]);
	    gapgf[i] = NULL;
	}
	if (ghdd[i]) {
	    DrawDibClose(ghdd[i]);
	    ghdd[i] = 0;
	}
    }
    SetScrollRange(hwnd, SB_HORZ, 0, 0, TRUE);
    gpaviVideo = gpaviAudio = NULL;
}


/*----------------------------------------------------------------------------*\
|    FreeAvi()									|
|										|
|    Free the resources associated with an open file.				|
\*----------------------------------------------------------------------------*/
void FreeAvi(HWND hwnd)
{
    int	i;

    FreeDrawStuff(hwnd);

    AVISaveOptionsFree(gcpavi, galpAVIOptions);
    
    for (i = 0; i < gcpavi; i++) {
	AVIStreamRelease(gapavi[i]);
    }

    if (gpfile)
	AVIFileRelease(gpfile);
    gpfile = NULL;
    
    // Good a place as any to make sure audio data gets freed
    if (lpAudio)
	GlobalFreePtr(lpAudio);
    lpAudio = NULL;

    gcpavi = 0;
}


/*----------------------------------------------------------------------------*\
|    InitBall()									|
|										|
|    Open up our fake "ball" file as an installible stream hander		|
\*----------------------------------------------------------------------------*/
void InitBall(HWND hwnd)
{
    PAVISTREAM FAR PASCAL NewBall(void);

    // close everything down
    FreeAvi(hwnd);

    //
    // The NewBall() function creates a PAVISTREAM we can use as it it was
    // an AVI file.
    //
    gapavi[0] = NewBall();

    if (gapavi[0])
	gcpavi = 1;
    
    lstrcpy(gachFileName, "BALL");
    InitStreams(hwnd);
    FixWindowTitle(hwnd);
}

/*----------------------------------------------------------------------------*\
|    InitAvi()									|
|										|
|    Open up a file through the AVIFile handlers.				|
\*----------------------------------------------------------------------------*/
void InitAvi(HWND hwnd, LPSTR szFile, WORD wMenu)
{
    HRESULT	hr;
    int		i;
    PAVIFILE	pfile;
    PAVISTREAM  pavi;

    hr = AVIFileOpen(&pfile, (LPCSTR) szFile, 0, 0L);
    
    if (hr != 0) {
        ErrMsg("Unable to open %s", (LPSTR)szFile);
	return;
    }
    
    //
    // If we're opening something new, close other open files, otherwise
    // just close the draw stuff so we'll merge streams with the new file
    //
    if (wMenu == MENU_OPEN)
	FreeAvi(hwnd);

    //
    // Get all the streams from the new file
    //
    for (i = gcpavi; i <= MAXNUMSTREAMS; i++) {
	if (AVIFileGetStream(pfile, &pavi, 0L, i - gcpavi) != AVIERR_OK)
	    break;
	if (i == MAXNUMSTREAMS) {
	    AVIStreamRelease(pavi);
	    ErrMsg("Exceeded maximum number of streams");
	    break;
	}
	gapavi[i] = pavi;
    }

    //
    // Couldn't get any streams out of the file
    //
    if (gcpavi == i && i != MAXNUMSTREAMS)
    {
        ErrMsg("Unable to open %s", (LPSTR)szFile);
	if (pfile)
	    AVIFileRelease(pfile);
	return;
    }

    gcpavi = i;

    if (gpfile) {
	AVIFileRelease(pfile);
    } else
	gpfile = pfile;
    
    FreeDrawStuff(hwnd);
    InitStreams(hwnd);
    FixWindowTitle(hwnd);
}

/*----------------------------------------------------------------------------*\
|   AppInit( hInst, hPrev)						       |
|									       |
|   Description:							       |
|	This is called when the application is first loaded into	       |
|	memory.  It performs all initialization that doesn't need to be done   |
|	once per instance.						       |
|									       |
|   Arguments:								       |
|	hInstance	instance handle of current instance		       |
|	hPrev		instance handle of previous instance		       |
|									       |
|   Returns:								       |
|	TRUE if successful, FALSE if not				       |
|									       |
\*----------------------------------------------------------------------------*/
BOOL AppInit(HINSTANCE hInst, HINSTANCE hPrev, WORD sw, LPSTR szCmdLine)
{
    WNDCLASS	cls;
    HDC		hdc;
    WORD	wVer;

    /* first let's make sure we are running on 1.1 */
    wVer = HIWORD(VideoForWindowsVersion());
    if (wVer < 0x010a){
	    /* oops, we are too old, blow out of here */
	    MessageBeep(MB_ICONHAND);
	    MessageBox(NULL, "Video for Windows version is too old",
		       "AVIView Error", MB_OK|MB_ICONSTOP);
	    return FALSE;
    }

    //
    // Save instance handle for DialogBoxs
    //
    ghInstApp = hInst;

    ghAccel = LoadAccelerators(hInst, MAKEINTATOM(ID_APP));

    //
    // Did we get passed a filename on the command line? We'll open it at create
    // time.
    //
    if (szCmdLine && szCmdLine[0])
        lstrcpy(gachFileName, szCmdLine);

    if (!hPrev) {
	/*
	 *  Register a class for the main application window
	 */
        cls.hCursor        = LoadCursor(NULL,IDC_ARROW);
        cls.hIcon          = LoadIcon(hInst,MAKEINTATOM(ID_APP));
        cls.lpszMenuName   = MAKEINTATOM(ID_APP);
        cls.lpszClassName  = MAKEINTATOM(ID_APP);
        cls.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
        cls.hInstance      = hInst;
        cls.style          = CS_BYTEALIGNCLIENT | CS_VREDRAW | CS_HREDRAW |
				CS_DBLCLKS;
        cls.lpfnWndProc    = (LPWNDPROC)AppWndProc;
        cls.cbWndExtra     = 0;
        cls.cbClsExtra     = 0;

        if (!RegisterClass(&cls))
	    return FALSE;
    }

    //
    // Must be called before using any of the AVIFile routines
    //
    AVIFileInit();

    //
    // Create our main application window
    //
    ghwndApp = CreateWindow (
			    MAKEINTATOM(ID_APP),    // Class name
                            gszAppName,             // Caption
                            WS_OVERLAPPEDWINDOW,    // Style bits
                            CW_USEDEFAULT, 0,       // Position
                            320,300,                // Size
                            (HWND)NULL,             // Parent window (no parent)
                            (HMENU)NULL,            // use class menu
                            hInst,          	    // handle to window instance
                            (LPSTR)NULL             // no params to pass on
                           );
    ShowWindow(ghwndApp,sw);

    hfontApp = GetStockObject(ANSI_VAR_FONT);
    hdc = GetDC(NULL);
    SelectObject(hdc, hfontApp);
    GetTextMetrics(hdc, &tm);
    ReleaseDC(NULL, hdc);

    return TRUE;
}

/*----------------------------------------------------------------------------*\
|   WinMain( hInst, hPrev, lpszCmdLine, cmdShow )			       |
|                                                                              |
|   Description:                                                               |
|       The main procedure for the App.  After initializing, it just goes      |
|       into a message-processing loop until it gets a WM_QUIT message         |
|       (meaning the app was closed).                                          |
|                                                                              |
|   Arguments:                                                                 |
|	hInst		instance handle of this instance of the app	       |
|	hPrev		instance handle of previous instance, NULL if first    |
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

    //
    // Call our initialization procedure
    //
    if (!AppInit(hInst, hPrev, sw, szCmdLine))
        return FALSE;

    /*
     * Polling messages from event queue
     */
    for (;;)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                return msg.wParam;

	    if (TranslateAccelerator(ghwndApp, ghAccel, &msg))
		continue;
	    
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

	//
	// If we have no messages to dispatch, we do our background task...
	// If we're playing a file, we set the scroll bar to show the video
	// frames corresponding with the current playing audio sample
	//
	if (gfPlaying) {
	    LONG    l;

	    //
	    // Use the audio clock to tell how long we've been playing.  To
	    // maintain sync, it's important we use this clock.
	    //
	    l = aviaudioTime(); 	// returns -1 if no audio playing

	    //
	    // If we can't use the audio clock to tell us how long we've been
	    // playing, calculate it ourself
	    //
	    if (l == -1)
		l = timeGetTime() - glPlayStartTime + glPlayStartPos;

	    if (l != GetScrollTime(ghwndApp)) {
	        if (l < timeStart)	// make sure number isn't out of bounds
		    l = timeStart;
	        if (l > timeEnd)	// looks like we're all done!
		    gfPlaying = FALSE;
		SetScrollTime(ghwndApp, l);
		InvalidateRect(ghwndApp, NULL, FALSE);
		UpdateWindow(ghwndApp);

		continue;
	    }
	}
	
	WaitMessage();
    }

    /* NOT REACHED */
}

typedef BYTE _huge * HPBYTE;
typedef int _huge *  HPINT;


/*----------------------------------------------------------------------------*\
|    PaintVideo()								|
|										|
|    Draw a video frame in the specified rect.					|
\*----------------------------------------------------------------------------*/
void PaintVideo(HDC hdc, RECT rcFrame, int iStream, LPBITMAPINFOHEADER lpbi, LONG lCurFrame, LONG lPos)
{
    int		iLen;
    char 	ach[200];
    RECT	rc;

    //
    // If we have a picture, draw it
    //
    if (lpbi)
    {
        //
        // use the palette of the first video stream
        //
        DrawDibDraw(ghdd[iStream], hdc,
	    rcFrame.left, rcFrame.top,
	    rcFrame.right - rcFrame.left,
	    rcFrame.bottom - rcFrame.top,
	    lpbi, NULL,
	    0, 0, -1, -1,
	    gapavi[iStream] == gpaviVideo ? 0 :DDF_BACKGROUNDPAL);
    
        iLen = wsprintf(ach, "%ld %ld.%03lds",
	    lCurFrame, lPos / 1000, lPos % 1000);
    }
    
    //
    // Before or after the movie (or read error) draw GRAY
    //
    else {
        SelectObject(hdc,GetStockObject(DKGRAY_BRUSH));
    
        PatBlt(hdc,
	    rcFrame.left, rcFrame.top,
	    rcFrame.right - rcFrame.left,
	    rcFrame.bottom - rcFrame.top,
	    PATCOPY);
        iLen = 0;
        ach[0] = '\0';
    }
    
    //
    // print something meaningful under the frame
    //
    rc.left = rcFrame.left;
    rc.right = rcFrame.right + HSPACE;
    rc.top = rcFrame.bottom + VSPACE;
    rc.bottom = rc.top + TSPACE;
    ExtTextOut(hdc, rc.left, rc.top, ETO_OPAQUE,
	       &rc, ach, iLen, NULL);
}
    
    
/*----------------------------------------------------------------------------*\
|    PaintAudio()								|
|										|
|    Draw some samples of audio inside the given rectangle.			|
\*----------------------------------------------------------------------------*/
void PaintAudio(HDC hdc, PRECT prc, PAVISTREAM pavi, LONG lStart, LONG lLen)
{
    PCMWAVEFORMAT wf;
    int i;
    int x,y;
    int w,h;
    BYTE b;
    HBRUSH hbr;
    RECT rc = *prc;
    LONG    lBytes;
    LONG    l, lLenOrig = lLen;
    LONG    lWaveBeginTime = AVIStreamStartTime(pavi);
    LONG    lWaveEndTime   = AVIStreamEndTime(pavi);

    //
    // We can't draw before the beginning of the stream - adjust
    //
    if (lStart < lWaveBeginTime) {
	lLen -= lWaveBeginTime - lStart;
	lStart = lWaveBeginTime;
	// right justify the legal samples in the rectangle - don't stretch
	rc.left = rc.right - (int)muldiv32(rc.right - rc.left, lLen, lLenOrig);
    }

    //
    // We can't draw past the end of the stream
    //
    if (lStart + lLen > lWaveEndTime) {
	lLenOrig = lLen;
	lLen = max(0, lWaveEndTime - lStart);	// maybe nothing to draw!
	// left justify the legal samples in the rectangle - don't stretch
	rc.right = rc.left + (int)muldiv32(rc.right - rc.left, lLen, lLenOrig);
    }

    // Now start working with samples, not time
    l = lStart;
    lStart = AVIStreamTimeToSample(pavi, lStart);
    lLen = AVIStreamTimeToSample(pavi, l + lLen) - lStart;

    //
    // Get the format of the wave data
    //
    l = sizeof(wf);
    AVIStreamReadFormat(pavi, lStart, &wf, &l);
    if (!l)
        return;

    w = rc.right - rc.left;
    h = rc.bottom - rc.top;

    //
    // We were starting before the beginning or continuing past the end.
    // We're not painting in the whole original rect --- use a dark background
    //
    if (rc.left > prc->left) {
        SelectObject(hdc, GetStockObject(DKGRAY_BRUSH));
	PatBlt(hdc, prc->left, rc.top, rc.left - prc->left,
						rc.bottom - rc.top, PATCOPY);
    }
    if (rc.right < prc->right) {
        SelectObject(hdc, GetStockObject(DKGRAY_BRUSH));
	PatBlt(hdc, rc.right, rc.top, prc->right - rc.right,
						rc.bottom - rc.top, PATCOPY);
    }

#define BACKBRUSH  (GetSysColor(COLOR_BTNFACE))		// background
#define MONOBRUSH  (GetSysColor(COLOR_BTNSHADOW))	// for mono audio
#define LEFTBRUSH  (RGB(0,0,255))			// left channel
#define RIGHTBRUSH (RGB(0,255,0))			// right channel
#define HPOSBRUSH  (RGB(255,0,0))			// current position
    
    //
    // Paint the background
    //
    hbr = SelectObject(hdc, CreateSolidBrush(BACKBRUSH));
    PatBlt(hdc, rc.left, rc.top, w, h, PATCOPY);
    DeleteObject(SelectObject(hdc, hbr));

    //
    // !!! we can only paint PCM data right now.  Sorry!
    //
    if (wf.wf.wFormatTag != WAVE_FORMAT_PCM)
        return;

    //
    // How many bytes are we painting? Alloc some space for them
    //
    lBytes = lLen * wf.wf.nChannels * wf.wBitsPerSample / 8;
    if (!lpAudio)
        lpAudio = GlobalAllocPtr (GHND, lBytes);
    else if ((LONG)GlobalSizePtr(lpAudio) < lBytes)
        lpAudio = GlobalReAllocPtr(lpAudio, lBytes, GMEM_MOVEABLE);
    if (!lpAudio)
        return;

    //
    // Read in the wave data
    //
    AVIStreamRead(pavi, lStart, lLen, lpAudio, lBytes, NULL, &l);
    if (l != lLen)
        return;
    
#define MulDiv(a,b,c) (UINT)((DWORD)(UINT)(a) * (DWORD)(UINT)(b) / (UINT)(c))

    //
    // !!! Flickers less painting it NOW or LATER?
    // First show the current position as a bar
    //
    hbr = SelectObject(hdc, CreateSolidBrush(HPOSBRUSH));
    PatBlt(hdc, prc->right / 2, prc->top, 1, prc->bottom - prc->top, PATCOPY);
    DeleteObject(SelectObject(hdc, hbr));

    //
    // Paint monochrome wave data
    //
    if (wf.wf.nChannels == 1) {

	//
	// Draw the x-axis
	//
        hbr = SelectObject(hdc, CreateSolidBrush(MONOBRUSH));
        y = rc.top + h/2;
        PatBlt(hdc, rc.left, y, w, 1, PATCOPY);
    
	//
	// 8 bit data is centred around 0x80
	//
        if (wf.wBitsPerSample == 8) {
            for (x=0; x<w; x++) {

		// which byte of audio data belongs at this pixel?
                b = *((HPBYTE)lpAudio + muldiv32(x, lLen, w));

                if (b > 0x80) {
                    i = y - MulDiv(b - 0x80, (h / 2), 128);
                    PatBlt(hdc, rc.left+x, i, 1, y-i, PATCOPY);
                }
                else {
                    i = y + MulDiv(0x80 - b, (h / 2), 128);
                    PatBlt(hdc, rc.left + x, y, 1, i - y, PATCOPY);
                }
            }
        }

	//
	// 16 bit data is centred around 0x00
	//
        else if (wf.wBitsPerSample == 16) {
            for (x=0; x<w; x++) {

		// which byte of audio data belongs at this pixel?
                i = *((HPINT)lpAudio + muldiv32(x,lLen,w));

                if (i > 0) {
                   i = y - (int) ((LONG)i * (h/2) / 32768);
                   PatBlt(hdc, rc.left+x, i, 1, y-i, PATCOPY);
                }
                else {
                   i = (int) ((LONG)i * (h/2) / 32768);
                   PatBlt(hdc, rc.left+x, y, 1, -i, PATCOPY);
                }
            }
        }
        DeleteObject(SelectObject(hdc, hbr));
    } // endif mono

    //
    // Draw stereo waveform data
    //
    else if (wf.wf.nChannels == 2) {

	//
	// 8 bit data is centred around 0x80
	//
        if (wf.wBitsPerSample == 8) {

            // Left channel
            hbr = SelectObject(hdc, CreateSolidBrush(LEFTBRUSH));
            y = rc.top + h/4;
            PatBlt(hdc, rc.left, y, w, 1, PATCOPY);

            for (x=0; x<w; x++) {
                b = *((HPBYTE)lpAudio + muldiv32(x,lLen,w) * 2);

                if (b > 0x80) {
                    i = y - MulDiv(b-0x80,(h/4),128);
                    PatBlt(hdc, rc.left+x, i, 1, y-i, PATCOPY);
                }
                else {
                    i = y + MulDiv(0x80-b,(h/4),128);
                    PatBlt(hdc, rc.left+x, y, 1, i-y, PATCOPY);
                }
            }
            DeleteObject(SelectObject(hdc, hbr));
                
            // Right channel
            hbr = SelectObject(hdc, CreateSolidBrush(RIGHTBRUSH));
            y = rc.top + h * 3 / 4;
            PatBlt(hdc, rc.left, y, w, 1, PATCOPY);

            for (x=0; x<w; x++) {
                b = *((HPBYTE)lpAudio + muldiv32(x,lLen,w) * 2 + 1);

                if (b > 0x80) {
                    i = y - MulDiv(b-0x80,(h/4),128);
                    PatBlt(hdc, rc.left+x, i, 1, y-i, PATCOPY);
                }
                else {
                    i = y + MulDiv(0x80-b,(h/4),128);
                    PatBlt(hdc, rc.left+x, y, 1, i-y, PATCOPY);
                }
            }
            DeleteObject(SelectObject(hdc, hbr));
        }

	//
	// 16 bit data is centred around 0x00
	//
        else if (wf.wBitsPerSample == 16) {

            // Left channel
            hbr = SelectObject(hdc, CreateSolidBrush(LEFTBRUSH));
            y = rc.top + h/4;
            PatBlt(hdc, rc.left, y, w, 1, PATCOPY);

            for (x=0; x<w; x++) {
                i = *((HPINT)lpAudio + muldiv32(x,lLen,w) * 2);
                if (i > 0) {
                    i = y - (int) ((LONG)i * (h/4) / 32768);
                    PatBlt(hdc, rc.left+x, i, 1, y-i, PATCOPY);
                }
                else {
                    i = (int) ((LONG)i * (h/4) / 32768);
                    PatBlt(hdc, rc.left+x, y, 1, -i, PATCOPY);
                }
            }
            DeleteObject(SelectObject(hdc, hbr));

            // Right channel
            hbr = SelectObject(hdc, CreateSolidBrush(RIGHTBRUSH));
            y = rc.top + h * 3 / 4;
            PatBlt(hdc, rc.left, y, w, 1, PATCOPY);

            for (x=0; x<w; x++) {
                i = *((HPINT)lpAudio + muldiv32(x,lLen,w) * 2 + 1);
                if (i > 0) {
                   i = y - (int) ((LONG)i * (h/4) / 32768);
                   PatBlt(hdc, rc.left+x, i, 1, y-i, PATCOPY);
                }
                else {
                   i = (int) ((LONG)i * (h/4) / 32768);
                   PatBlt(hdc, rc.left+x, y, 1, -i, PATCOPY);
                }
            }
            DeleteObject(SelectObject(hdc, hbr));
        }
    } // endif stereo
}

/*----------------------------------------------------------------------------*\
|    PaintStuff()								|
|										|
|    Paint the screen with what we plan to show them.				|
\*----------------------------------------------------------------------------*/
long PaintStuff(HDC hdc, HWND hwnd, BOOL fDrawEverything)
{
    int		yStreamTop;
    char        ach[400];
    int         iFrameWidth, iLen;
    LONG        lSamp, lCurSamp;
    int         n;
    int         nFrames;
    LPBITMAPINFOHEADER lpbi;
    LONG        l;
    LONG	lTime;
    LONG        lSize = 0;
    LONG	lAudioStart;
    LONG	lAudioLen;
    RECT        rcFrame, rcC;
    int         i;
    LONG	lFrame;
    LONG	lCurFrame;
    HBRUSH      hbr, hbrOld;
    RECT	rc;

#define PRINT(sz) \
    (TextOut(hdc, TSPACE, yStreamTop, sz, lstrlen(sz)), \
    yStreamTop += tm.tmHeight+1)

#define PF1(sz,a)                 (wsprintf(ach, sz, a), PRINT(ach))
#define PF2(sz,a,b)               (wsprintf(ach, sz, a, b), PRINT(ach))
#define PF3(sz,a,b,c)             (wsprintf(ach, sz, a, b, c), PRINT(ach))
#define PF4(sz,a,b,c,d)           (wsprintf(ach, sz, a, b, c, d), PRINT(ach))
#define PF5(sz,a,b,c,d,e)         (wsprintf(ach, sz, a, b, c, d, e), PRINT(ach))
#define PF6(sz,a,b,c,d,e,f)       (wsprintf(ach, sz, a, b, c, d, e, f), PRINT(ach))
#define PF7(sz,a,b,c,d,e,f,g)     (wsprintf(ach, sz, a, b, c, d, e, f, g), PRINT(ach))
#define PF8(sz,a,b,c,d,e,f,g,h)   (wsprintf(ach, sz, a, b, c, d, e, f, g, h), PRINT(ach))
#define PF9(sz,a,b,c,d,e,f,g,h,i) (wsprintf(ach, sz, a, b, c, d, e, f, g, h, i), PRINT(ach))

#define FIXCC(fcc)  if (fcc == 0)       fcc = mmioFOURCC('N', 'o', 'n', 'e'); \
                    if (fcc == BI_RLE8) fcc = mmioFOURCC('R', 'l', 'e', '8');

    SelectObject(hdc, hfontApp);

    GetClientRect(hwnd, &rcC);

    //
    // Look at scrollbars to find current position
    //
    lTime = GetScrollTime(hwnd);
    yStreamTop = -GetScrollPos(hwnd, SB_VERT);

    //
    // Walk through all streams and draw something
    //
    for (i=0; i<gcpavi; i++) {
	AVISTREAMINFO		avis;
	LONG  lEnd, lEndTime, lNextFmt, lPrevFmt, l;
	LONG  lPos, lNextKey, lPrevKey, lNextAny, lPrevAny;

	//
	// Get some info and print something about this stream
	//
	AVIStreamInfo(gapavi[i], &avis, sizeof(avis));
        FIXCC(avis.fccHandler);
        FIXCC(avis.fccType);
	l = sizeof(abFormat);
        AVIStreamReadFormat(gapavi[i], 0, &abFormat, &l);

        PF7("Stream%d [%4.4ls/%4.4ls] Start: %ld Length: %ld (%ld.%03ld sec)                                                                              ",
		    i,
                    (LPSTR)&avis.fccType,
                    (LPSTR)&avis.fccHandler,
		    AVIStreamStart(gapavi[i]),
                    AVIStreamLength(gapavi[i]),
                    AVIStreamLengthTime(gapavi[i]) / 1000,
		    AVIStreamLengthTime(gapavi[i]) % 1000);

        lPos = AVIStreamTimeToSample(gapavi[i], lTime);
        AVIStreamSampleSize(gapavi[i], lPos, &lSize);

        lPrevKey = AVIStreamFindSample(gapavi[i], lPos, FIND_PREV|FIND_KEY);
        lPrevAny = AVIStreamFindSample(gapavi[i], lPos, FIND_PREV|FIND_ANY);
        lPrevFmt = AVIStreamFindSample(gapavi[i], lPos, FIND_PREV|FIND_FORMAT);

        lNextKey = AVIStreamFindSample(gapavi[i], lPos, FIND_NEXT|FIND_KEY);
        lNextAny = AVIStreamFindSample(gapavi[i], lPos, FIND_NEXT|FIND_ANY);
        lNextFmt = AVIStreamFindSample(gapavi[i], lPos, FIND_NEXT|FIND_FORMAT);

        PF5("Pos:%ld Time:%ld.%03ld sec Size:%ld bytes %s                                                                                               ",
                        lPos, lTime/1000, lTime%1000, lSize,
                        (LPSTR)(lPos == lPrevKey ? "Key" : ""));

        PF6("PrevKey=%ld NextKey=%ld, PrevAny=%ld NextAny=%ld, PrevFmt=%ld NextFmt=%ld                                                                       ",
                    lPrevKey, lNextKey,
                    lPrevAny, lNextAny,
                    lPrevFmt, lNextFmt);

	//
	// Draw a VIDEO stream
	//
	if (avis.fccType == streamtypeVIDEO) {
	    if (gapgf[i] == NULL)
		continue;
	    
            lpbi = (LPBITMAPINFOHEADER)abFormat;
            FIXCC(lpbi->biCompression);

            //
            // display video format
            //
            //  Video: 160x120x8 (cram)
            //
            PF4("Format: %dx%dx%d (%4.4s)                    ",
		(int)lpbi->biWidth, (int)lpbi->biHeight,
		(int)lpbi->biBitCount, (LPVOID)&lpbi->biCompression);
                        
	    //
	    // Which frame belongs at this time?
	    //
	    lEndTime = AVIStreamEndTime(gapavi[i]);
	    if (lTime <= lEndTime)
		lFrame = AVIStreamTimeToSample(gapavi[i], lTime);
	    else {	// we've scrolled past the end of this stream
		lEnd = AVIStreamEnd(gapavi[i]);
		lFrame = lEnd + AVIStreamTimeToSample(
			gapavi[i], lTime - lEndTime);
	    }

	    //
	    // how wide is each frame to paint?
	    //
	    iFrameWidth = (avis.rcFrame.right - avis.rcFrame.left) *
		gwZoom / 4 + HSPACE;

	    //
	    // how many frames can we fit on each half of the screen?
	    //
	    nFrames = (rcC.right - iFrameWidth) / (2 * iFrameWidth);
	    if (nFrames < 0)
		nFrames = 0;    // at least draw *something*

	    //
	    // Step through all the frames we'll draw
	    //
	    for (n = -nFrames; n <= nFrames; n++)
	    {

		//
		// Each video stream is drawn as a horizontal line of
		// frames, very close together.
		// The first video stream shows a different frame in
		// each square. Thus the scale of time is determined
		// by the first video stream.
		// Every other video stream shows whatever
		// frame belongs at the time corresponding to the mid-
		// point of each square.
		//
		if (gapavi[i] == gpaviVideo) {

		    //
		    // by definition, we know what frame we're drawing..
		    // (lFrame-n), (lFrame-(n-1)), ..., (lFrame), ...,
		    // (lFrame+(n-1)), (lFrame+n)
		    //
		    lCurFrame = lFrame + n;

		    //
		    // what time is it at that frame? This number will
		    // be printed underneath the frame
		    //
		    l = AVIStreamSampleToTime(gapavi[i], lCurFrame);

		} else {	// NOT the first video stream

		    //
		    // What time is it at the midpoint of the square
		    // we'll draw?  That's what frame we use.
		    //
		    l = lTime + muldiv32(n * iFrameWidth,
					gdwMicroSecPerPixel, 1000);

		    //
		    // What frame belongs to that time?
		    //
		    lCurFrame =AVIStreamTimeToSample(gapavi[i], l);

		    //
		    // what time is it at that frame? This number will
		    // be printed underneath the frame
		    //
		    l = AVIStreamSampleToTime(gapavi[i], lCurFrame);
		}

		// !!!
		// Could actually return an LPBI for invalid frames
		// so we better force it to NULL.
		//
		if (gapgf[i] && lCurFrame >= AVIStreamStart(gapavi[i]))
		    lpbi = AVIStreamGetFrame(gapgf[i], lCurFrame);
		else
		    lpbi = NULL;

		//
		// Figure out where to draw this frame
		//
		rcFrame.left   = rcC.right / 2 -
			((avis.rcFrame.right - avis.rcFrame.left) * gwZoom / 4)
			/ 2 + (n * iFrameWidth);
		rcFrame.top    = yStreamTop;
		rcFrame.right  = rcFrame.left +
			(avis.rcFrame.right - avis.rcFrame.left) * gwZoom / 4;
		rcFrame.bottom = rcFrame.top +
			(avis.rcFrame.bottom - avis.rcFrame.top) * gwZoom / 4;

		//
		// draw a border around the current frame.  Make the
		// one around the centre frame a special colour.
		//
		if (n == 0)
		    hbr = CreateSolidBrush(RGB(255,0,0));
		else
		    hbr = CreateSolidBrush(RGB(255,255,255));
		InflateRect(&rcFrame, 1, 1);
		FrameRect(hdc, &rcFrame, hbr);
		InflateRect(&rcFrame, -1, -1);
		DeleteObject (hbr);
	    
		//
		// Now draw the video frame in the computed rectangle
		//
		PaintVideo(hdc, rcFrame, i, lpbi, lCurFrame, l);

	    }

	    //
	    // Move down to where we can draw the next stream
	    //
	    yStreamTop += (rcFrame.bottom - rcFrame.top) +
			  TSPACE;
	}

	//
	// Draw an AUDIO stream
	//
	else if (avis.fccType == streamtypeAUDIO) {
	    
            LPWAVEFORMAT pwf = (LPWAVEFORMAT)abFormat;
            char *szFmt;

            if (pwf->wFormatTag == 1) {  // PCM
                if (pwf->nChannels == 1)
                    szFmt = "Format: Mono %dHz %dbit";
                else
                    szFmt = "Format: Stereo %dHz %dbit";
            } else if (pwf->wFormatTag == 2) {  // ADPCM
                if (pwf->nChannels == 1)
                    szFmt = "Format: ADPCM Mono %dHz %dbit";
                else
                    szFmt = "Format: ADPCM Stereo %dHz %dbit";
            } else {
                if (pwf->nChannels == 1)
                    szFmt = "Format: Compressed Mono %dHz %dbit";
                else
                    szFmt = "Format: Compressed Stereo %dHz %dbit";
            }

            PF2(szFmt,(int)pwf->nSamplesPerSec,
                (int)(pwf->nAvgBytesPerSec * 8 / pwf->nSamplesPerSec));

	    //
	    // Figure out which samples are visible
	    //
	    lAudioStart = lTime - muldiv32(rcC.right / 2,
					gdwMicroSecPerPixel, 1000);
	    lAudioLen = 2 * (lTime - lAudioStart);

	    //
	    // Make the rectangle to draw audio into
	    //
	    rc.left = rcC.left;
	    rc.right = rcC.right;
	    rc.top = yStreamTop;
	    rc.bottom = rc.top + AUDIOVSPACE * gwZoom / 4;

	    //
	    // Actually paint the audio
	    //
	    PaintAudio(hdc, &rc, gapavi[i], lAudioStart, lAudioLen);

	    //
	    // Move down to where we can draw the next stream
	    //
	    yStreamTop += AUDIOVSPACE * gwZoom / 4;
	    
	}

	yStreamTop += VSPACE;

	//
	// Give up once we're painting below the bottom of the window
	//
	if (!fDrawEverything && yStreamTop >= rcC.bottom)
	    break;
    }

    //
    // How many lines did we draw?
    //
    return yStreamTop + GetScrollPos(hwnd, SB_VERT);
}


/*----------------------------------------------------------------------------*\
|   AppWndProc( hwnd, uiMessage, wParam, lParam )			       |
|                                                                              |
|   Description:                                                               |
|       The window proc for the app's main (tiled) window.  This processes all |
|       of the parent window's messages.                                       |
|                                                                              |
|   Arguments:                                                                 |
|	hwnd		window handle for the window			       |
|       uiMessage       message number                                         |
|       wParam          message-dependent                                      |
|       lParam          message-dependent                                      |
|                                                                              |
|   Returns:                                                                   |
|       0 if processed, nonzero if ignored                                     |
|                                                                              |
\*----------------------------------------------------------------------------*/
LONG FAR PASCAL _export AppWndProc(hwnd, msg, wParam, lParam)
    HWND     hwnd;
    unsigned msg;
    WORD     wParam;
    long     lParam;
{
    PAINTSTRUCT ps;
    BOOL        f;
    HDC         hdc;
    int		i, iFrameWidth, nFrames, n;
    RECT	rc, rcC;
    LONG	l, lTime, lSamp, lCurSamp;

    switch (msg) {

	//
	// If we passed a command line filename, open it
	//
        case WM_CREATE:
            if (gachFileName[0])
                InitAvi(hwnd, gachFileName, MENU_OPEN);
	    break;

        case WM_COMMAND:
            return AppCommand(hwnd,msg,wParam,lParam);

        case WM_INITMENU:
	    f = gfVideoFound || gfAudioFound;
            EnableMenuItem((HMENU)wParam, MENU_SAVEAS, f ? MF_ENABLED :
			MF_GRAYED);
            EnableMenuItem((HMENU)wParam, MENU_OPTIONS,f ? MF_ENABLED :
			MF_GRAYED);

            f = gcpavi > 0;
            EnableMenuItem((HMENU)wParam, MENU_CLOSE,  f ? MF_ENABLED :
			MF_GRAYED);
            EnableMenuItem((HMENU)wParam, MENU_MERGE,  f ? MF_ENABLED :
			MF_GRAYED);
	    
	    f = gfAudioFound | gfVideoFound;
            EnableMenuItem((HMENU)wParam, MENU_PLAY,
			(f & !gfPlaying) ? MF_ENABLED : MF_GRAYED);
            EnableMenuItem((HMENU)wParam, MENU_STOP,
			(f & gfPlaying) ? MF_ENABLED : MF_GRAYED);

	    CheckMenuItem((HMENU)wParam, MENU_ZOOMQUARTER, 
		    (gwZoom == 1) ? MF_CHECKED : MF_UNCHECKED);
	    CheckMenuItem((HMENU)wParam, MENU_ZOOMHALF, 
		    (gwZoom == 2) ? MF_CHECKED : MF_UNCHECKED);
	    CheckMenuItem((HMENU)wParam, MENU_ZOOM1, 
		    (gwZoom == 4) ? MF_CHECKED : MF_UNCHECKED);
	    CheckMenuItem((HMENU)wParam, MENU_ZOOM2, 
		    (gwZoom == 8) ? MF_CHECKED : MF_UNCHECKED);
	    CheckMenuItem((HMENU)wParam, MENU_ZOOM4, 
		    (gwZoom == 16) ? MF_CHECKED : MF_UNCHECKED);
	    	    
            break;

	//
	// During a wait state (eg saving) don't let us choose any menus
	//
	case WM_NCHITTEST:
	    if (fWait) {

		// Let windows tell us where the cursor is
		lParam = DefWindowProc(hwnd,msg,wParam,lParam);

		// If it's over a menu, pretend it's in the client (force 
		// hourglass)
		if (lParam == HTMENU)
		    lParam = HTCLIENT;

		return lParam;
	    }
	    break;

	//
	// Set vertical scrollbar for scrolling streams
	//
	case WM_SIZE:
	    GetClientRect(hwnd, &rc);

	    //
	    // There is not enough vertical room to show all streams. Scrollbars
	    // are required.
	    //
	    if (vertHeight > rc.bottom) {
	        vertSBLen = vertHeight - rc.bottom;
	        SetScrollRange(hwnd, SB_VERT, 0, (int)vertSBLen, TRUE);

	    //
	    // Everything fits vertically. No scrollbar necessary.
	    //
	    } else {
	        vertSBLen = 0;
	        SetScrollRange(hwnd, SB_VERT, 0, 0, TRUE);
	    }
	    break;

	//
	// During a wait state, show an hourglass over our client area
	// !!! Is this necessary?
	//
        case WM_SETCURSOR:
            if (fWait && LOWORD(lParam) == HTCLIENT) {
                SetCursor(LoadCursor(NULL, IDC_WAIT));
                return TRUE;
            }
            break;

	//
	// We're out of here!
	//
        case WM_DESTROY:
            FreeAvi(hwnd);	// close all open streams
	    AVIFileExit();	// shuts down the AVIFile system
	    PostQuitMessage(0);
	    break;

	//
	// Don't let us close ourselves in a wait state (eg saving)
	//
        case WM_CLOSE:
	    if (fWait)
		return 0;
            break;

	//
	// Block keyboard access to menus if waiting
	//
	case WM_SYSCOMMAND:
	    switch (wParam & 0xFFF0) {
		case SC_KEYMENU:
		    if (fWait)
			return 0;
		    break;
	    }
	    break;

        case WM_PALETTECHANGED:

	    // It came from us.  Ignore it
            if (wParam == (WORD)hwnd)
                break;

	case WM_QUERYNEWPALETTE:

            hdc = GetDC(hwnd);

	    //
	    // Realize the palette of the first video stream
	    // !!! If first stream isn't video, we're DEAD!
	    //
            if (f = DrawDibRealize(ghdd[0], hdc, FALSE))
                InvalidateRect(hwnd,NULL,TRUE);

            ReleaseDC(hwnd,hdc);

            return f;

        case WM_ERASEBKGND:
            break;

        case WM_PAINT:
            hdc = BeginPaint(hwnd,&ps);

	    PaintStuff(hdc, hwnd, FALSE);

            EndPaint(hwnd,&ps);
            break;

	//
	// handle the keyboard interface
	//
	case WM_KEYDOWN:
            switch (wParam)
            {
                case VK_UP:    PostMessage(hwnd, WM_VSCROLL, SB_LINEUP,0L);
		    break;
                case VK_DOWN:  PostMessage(hwnd, WM_VSCROLL, SB_LINEDOWN,0L);
		    break;
                case VK_PRIOR: PostMessage(hwnd, WM_HSCROLL, SB_PAGEUP,0L);
		    break;
                case VK_NEXT:  PostMessage(hwnd, WM_HSCROLL, SB_PAGEDOWN,0L);
		    break;
                case VK_HOME:  PostMessage(hwnd, WM_HSCROLL, SB_THUMBPOSITION,
								0L);
		     break;
                case VK_END:   PostMessage(hwnd, WM_HSCROLL, SB_THUMBPOSITION,
								0x7FFF);
		    break;
                case VK_LEFT:  PostMessage(hwnd, WM_HSCROLL, SB_LINEUP, 0L);
		    break;
                case VK_RIGHT: PostMessage(hwnd, WM_HSCROLL, SB_LINEDOWN, 0L);
		    break;
	    }
	    break;

        case WM_HSCROLL:
            l = GetScrollTime(hwnd);

            switch (wParam) {
                case SB_LINEDOWN:      l += timehscroll;  break;
                case SB_LINEUP:        l -= timehscroll;  break;
                case SB_PAGEDOWN:      l += timeLength/10; break;
                case SB_PAGEUP:        l -= timeLength/10; break;
                case SB_THUMBTRACK:
                case SB_THUMBPOSITION:
			l = LOWORD(lParam);
			l = timeStart + muldiv32(l, timeLength, SCROLLRANGE);
			break;
            }

	    if (l < timeStart)
		l = timeStart;

	    if (l > timeEnd)
		l = timeEnd;

	    if (l == GetScrollTime(hwnd))
		break;
	    
	    SetScrollTime(hwnd, l);
            InvalidateRect(hwnd, NULL, FALSE);
            UpdateWindow(hwnd);
            break;

        case WM_VSCROLL:
            l = GetScrollPos(hwnd, SB_VERT);
	    GetClientRect(hwnd, &rc);

            switch (wParam) {
                case SB_LINEDOWN:      l += 10;  break;
                case SB_LINEUP:        l -= 10;  break;
                case SB_PAGEDOWN:      l += rc.bottom; break;
                case SB_PAGEUP:        l -= rc.bottom; break;
                case SB_THUMBTRACK:
                case SB_THUMBPOSITION: l = LOWORD(lParam); break;
            }

	    if (l < 0)
		l = 0;

	    if (l > vertSBLen)
		l = vertSBLen;

	    if (l == GetScrollPos(hwnd, SB_VERT))
		break;
	    
	    SetScrollPos(hwnd, SB_VERT, (int)l, TRUE);
            InvalidateRect(hwnd, NULL, TRUE);
            UpdateWindow(hwnd);
            break;

	//
	// Wave driver wants to tell us something.  Pass it on.
	//
	case MM_WOM_OPEN:
	case MM_WOM_DONE:
	case MM_WOM_CLOSE:
	    aviaudioMessage(hwnd, msg, wParam, lParam);
	    break;
    }
    return DefWindowProc(hwnd,msg,wParam,lParam);
}

/*----------------------------------------------------------------------------*\
|    SaveCallback()								|
|										|
|    Our save callback that prints our progress in our window title bar.	|
\*----------------------------------------------------------------------------*/
BOOL FAR PASCAL _export SaveCallback(int iProgress)
{
    char    ach[128];

    wsprintf(ach, "%s - Saving %s: %d%%",
        (LPSTR) gszAppName, (LPSTR) gachSaveFileName, iProgress);

    SetWindowText(ghwndApp, ach);

    //
    // Give ourselves a chance to abort
    //
    return WinYield();
}

/*----------------------------------------------------------------------------*\
|    AppCommand()								|
|										|
|    Process all of our WM_COMMAND messages.					|
\*----------------------------------------------------------------------------*/
LONG NEAR PASCAL AppCommand (HWND hwnd, unsigned msg, WORD wParam, long lParam)
{
    OPENFILENAME ofn;

    switch(wParam)
    {
	//
	// We want out of here!
	//
	case MENU_EXIT:
	    PostMessage(hwnd,WM_CLOSE,0,0L);
            break;

	//
	// Set the compression options for each stream - pass an array of 
	// streams and an array of compression options structures
	//
        case MENU_OPTIONS:
            AVISaveOptions(hwnd,
		ICMF_CHOOSE_KEYFRAME | ICMF_CHOOSE_DATARATE |
			ICMF_CHOOSE_PREVIEW,
		gcpavi, gapavi, galpAVIOptions);
	    break;
	    
	//
	// Save all the open streams into a file
	//
        case MENU_SAVEAS:

            gachSaveFileName[0] = 0;

	    //
            // prompt user for file to save
	    //
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = hwnd;
            ofn.hInstance = NULL;
	    AVIBuildFilter(gachFilter, sizeof(gachFilter), TRUE);
            ofn.lpstrFilter = gachFilter;
            ofn.lpstrCustomFilter = NULL;
            ofn.nMaxCustFilter = 0;
            ofn.nFilterIndex = 0;
            ofn.lpstrFile = gachSaveFileName;
            ofn.nMaxFile = sizeof(gachSaveFileName);
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.lpstrTitle = "Save AVI File";
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY |
			    OFN_OVERWRITEPROMPT;
            ofn.nFileOffset = 0;
            ofn.nFileExtension = 0;
            ofn.lpstrDefExt = "avi";
            ofn.lCustData = 0;
            ofn.lpfnHook = NULL;
            ofn.lpTemplateName = NULL;

	    //
	    // If we get a filename, save it
	    //
            if (GetSaveFileName(&ofn))
            {
                FARPROC lpfn = MakeProcInstance((FARPROC)SaveCallback,
				ghInstApp);

		if (lpfn)
		{
		    DWORD	fccHandler[MAXNUMSTREAMS];
		    int		i;
		    HRESULT	hr;
		  
		    StartWait();

		    for (i = 0; i < gcpavi; i++)
		        fccHandler[i] = galpAVIOptions[i]->fccHandler;

		    hr = AVISaveV(gachSaveFileName,
			     NULL,
			     (AVISAVECALLBACK) lpfn,
			     gcpavi,
			     gapavi,
			     galpAVIOptions);
		    if (hr != AVIERR_OK)
			ErrMsg("Error saving AVI file");

		    // Now put the video compressors back that we stole
		    for (i = 0; i < gcpavi; i++)
		        galpAVIOptions[i]->fccHandler = fccHandler[i];
		    
		    EndWait();
		    FreeProcInstance(lpfn);
		    FixWindowTitle(hwnd);
		}
            }
	    break;

	//
	// Close everything
	//
	case MENU_CLOSE:
	    FreeAvi(hwnd);
	    gachFileName[0] = '\0';
	    FixWindowTitle(hwnd);
	    break;
	    
	//
	// Open a new file, or merge streams with a new file
	//
        case MENU_OPEN:
	case MENU_MERGE:
            gachFileName[0] = 0;

	    //
            // prompt user for file to open
	    //
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = hwnd;
            ofn.hInstance = NULL;
	    if (wParam == MENU_MERGE)
		ofn.lpstrTitle = "Merge With";
	    else
		ofn.lpstrTitle = "Open AVI";
	    AVIBuildFilter(gachFilter, sizeof(gachFilter), FALSE);
	    ofn.lpstrFilter = gachFilter;
            ofn.lpstrCustomFilter = NULL;
            ofn.nMaxCustFilter = 0;
            ofn.nFilterIndex = 0;
            ofn.lpstrFile = gachFileName;
            ofn.nMaxFile = sizeof(gachFileName);
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST |OFN_HIDEREADONLY;
            ofn.nFileOffset = 0;
            ofn.nFileExtension = 0;
            ofn.lpstrDefExt = NULL;
            ofn.lCustData = 0;
            ofn.lpfnHook = NULL;
            ofn.lpTemplateName = NULL;

	    //
	    // If we've got a filename, go open it
	    //
            if (GetOpenFileNamePreview(&ofn))
		InitAvi(hwnd, gachFileName, wParam);

	    break;

	//
	// Open the "fake" ball file as our current file
	//
	case MENU_BALL:
	    InitBall(hwnd);
	    break;
	    
	case MENU_ZOOMQUARTER:
	    gwZoom = 1;
	    FixScrollbars(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
	    break;
	    
	case MENU_ZOOMHALF:
	    gwZoom = 2;
	    FixScrollbars(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
	    break;
	    
	case MENU_ZOOM1:
	    gwZoom = 4;
	    FixScrollbars(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
	    break;
	    
	case MENU_ZOOM2:
	    gwZoom = 8;
	    FixScrollbars(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
	    break;
	    
	case MENU_ZOOM4:
	    gwZoom = 16;
	    FixScrollbars(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
	    break;

	//
	// Simulate playing the file.  We just play the 1st audio stream and let
	// our main message loop scroll the video by whenever it's bored.
	//
	case MENU_PLAY:
	    if (gfAudioFound)
	        aviaudioPlay(hwnd,
			 gpaviAudio,
			 AVIStreamTimeToSample(gpaviAudio, GetScrollTime(hwnd)),
			 AVIStreamEnd(gpaviAudio),
			 FALSE);
	    gfPlaying = TRUE;
	    glPlayStartTime = timeGetTime();
	    glPlayStartPos = GetScrollTime(hwnd);
	    break;

	//
	// Stop the play preview
	//
	case MENU_STOP:
	    if (gfAudioFound)
	        aviaudioStop();
	    gfPlaying = FALSE;
	    break;

    }
    return 0L;
}

/*----------------------------------------------------------------------------*\
|   ErrMsg - Opens a Message box with a error message in it.  The user can     |
|            select the OK button to continue                                  |
\*----------------------------------------------------------------------------*/
int ErrMsg (LPSTR sz,...)
{
    static char ach[2000];

    wvsprintf (ach,sz,(LPSTR)(&sz+1));	 /* Format the string */
    MessageBox(NULL,ach,NULL, MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
    return FALSE;
}
