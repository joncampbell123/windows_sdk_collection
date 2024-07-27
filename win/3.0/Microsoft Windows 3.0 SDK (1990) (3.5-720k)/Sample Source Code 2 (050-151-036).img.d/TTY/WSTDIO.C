//

//	FILE:    WSTDIO.c

//	PURPOSE: Contains functions for managing a standard I/O window.
//          Provides a means for easily sending text to a window 
//          for debugging, etc.

//	FUNCTIONS:
//          SetupStdioDC() - Initializes DC.
//          ResetStdioDC() - Selects former font into DC.
//          GetStdioLine() - Returns pointer to specified line in buffer.
//          StdioUpdate() - Scroll and update displayed text.
//          putStdio() - Process I/O window messages.
//          StdioPaint() - Paint procedure for I/O window.
//          InitStdio() - Initialize values used by I/O window.
//          stdioInit() - Define/register I/O window class.
//          wopen() - Create a default I/O window.
//          CreateStdioWindow() - Create a customized I/O window.
//          wputs() - Puts a string in the I/O window.
//          StdioWndProc() - Processes messages for the I/O window.
//                          


#include <windows.h>
#include "wstdio.h"

//========================================================================\\

// Declarations

//========================================================================\\

#define MaxLines   25
#define MaxLine    MaxLines - 1
char sScrBuff[MaxLines][81];	// Array of characters on TTY
                                 // could make this heap object so can realloc
short nFirstLine;		// Index of first line on TTY in the array
short nLastLine;		// Index of last line on TTY in the array

short nCurrPos; 		// Current TTY line output position
short nStdioCharWidth,
      nStdioCharHeight; 	// width and height of Stdio font chars


DWORD StdiobkRGB;                      // background color
DWORD StdiotextRGB;                    // text color
#define Stdio_FONT SYSTEM_FIXED_FONT   // font used for display
HFONT hOldFont;
HWND hWndStdio = NULL;                 // Handle to standard I/O window 
HANDLE hStdioInst;
BOOL bStdioQuit;
BOOL bInited = FALSE;


//========================================================================\\

// FUNCTION: SetupStdioDC(HWND, HDC)

// PURPOSE:  Sets up the I/O window DC. Called at GetDC/BeginPaint time.

//========================================================================\\

void SetupStdioDC(HWND hWnd, HDC hDC)
{
    RECT rClRect;

    GetClientRect(hWnd,&rClRect);

    // set origin to 25(+1 extra) lines from the bottom of the window
    SetViewportOrg(hDC,0,rClRect.bottom - ((MaxLines+1) * nStdioCharHeight));

    SetMapMode(hDC, MM_ANISOTROPIC);

    // Set the extents such that one unit horizontally or 
    // vertically is one character width or height.
    SetWindowExt(hDC,1,1);

    // Set the viewport such that the last line in the buffer is 
    // displayed at the bottom of the window. 
    SetViewportExt(hDC,nStdioCharWidth,nStdioCharHeight);

    // Set the background mode to opaque, and select the font.
    SetBkMode(hDC,OPAQUE);
    hOldFont = SelectObject(hDC,GetStockObject(Stdio_FONT));

}


//========================================================================\\

// FUNCTION: ResetStdioDC(HDC)

// PURPOSE:  Prepare to release the DC by selecting the system font.

//========================================================================\\

void ResetStdioDC(HDC hDC)
{
    SelectObject(hDC,hOldFont);
}



//========================================================================\\

// FUNCTION: GetStdioLine(short)

// PURPOSE:  Return a pointer to the specified line of the display.

//========================================================================\\

char *GetStdioLine(short ndx)
{
    short pos;

    // find the first line (one past the last line since we have a 
    // circular buffer). index to the desired line from there.
    pos = nLastLine + 1;
    if(pos == MaxLines) pos = 0;

    pos = pos + ndx;
    if(pos > MaxLine) pos = pos - MaxLines;

    return(sScrBuff[pos]);
}



//========================================================================\\

// FUNCTION: StdioUpdate(HWND, HDC, int)

// PURPOSE:  Scroll the window by the number of lines we have received,
//           and display the text in the invalidated areas.

//========================================================================\\

void StdioUpdate(HWND hWnd, HDC hDC, int iLinesOut)
{
    RECT rcRect;

    if(iLinesOut > 0){

         // scroll screen by number of lines received
	 GetClientRect(hWnd,&rcRect);
	 rcRect.bottom -= nStdioCharHeight;
    
	 ScrollWindow(hWnd,0,-(nStdioCharHeight * iLinesOut),&rcRect,NULL);
    }

    UpdateWindow(hWnd);
}


//========================================================================\\

// FUNCTION: putStdio(HWND, HDC, WORD, LPSTR)

// PURPOSE:  Process incoming text to Stdio window.

//========================================================================\\

void putStdio(HWND hWnd, HDC hDC, WORD wParam, LPSTR lParam)
{
    short i, j;
    char *sBuffer;
    RECT rClRect, rcInvalid;
    char *psLine;
    short iLinesOut = 0;    // # of lines to scroll

    sBuffer = sScrBuff[nLastLine]; // pointer to current line

    // scan the text, handle any special characters, and display the rest.

    for(i=0; i<wParam; i++){
	switch(lParam[i]) {

        case '\r': // return
            // move to the start of the line
	    nCurrPos = 0;   // reset the current position in the line
            break;

        case '\n': // new line

	    // "scroll" the window
	    ++iLinesOut;    // increment lines to scroll
	    nCurrPos = 0;   // reset the current position in the line

	    ++nLastLine;
	    if(nLastLine > MaxLine) nLastLine = 0;

            // clear the new line
	    sBuffer = sScrBuff[nLastLine];
	    for(j=0; j<80; j++) sBuffer[j] = '\0';
            break;

        case '\b': // backspace

            // move back one space
	    if(nCurrPos > 0) {

		--nCurrPos;
		sBuffer[nCurrPos] = '\0';
		rcInvalid.top = MaxLine; rcInvalid.bottom = MaxLine + 1;
		rcInvalid.left = nCurrPos;
		rcInvalid.right = nCurrPos + 1;
		LPtoDP(hDC,(POINT *) &rcInvalid, 2);

		// invalidate the area so that it gets redrawn
		InvalidateRect(hWnd,&rcInvalid, TRUE);

	    }
	    break;

	case '\t':
	    // ignore tabs for now
	    break;

        default:
            //add char to buffer
	    if(nCurrPos < 80){

		// put the character in the screen buffer
		sBuffer[nCurrPos] = lParam[i]; // add char to screen line

		// calculate area to invalidate
		rcInvalid.top = MaxLine; rcInvalid.bottom = MaxLine + 1;
		rcInvalid.left = nCurrPos;
		++nCurrPos;
		rcInvalid.right = nCurrPos;

		// only need to invalidate the area if it is on the last line
		if(iLinesOut == 0) {
		    LPtoDP(hDC,(POINT *) &rcInvalid, 2);
		    // invalidate the area so that it gets redrawn
		    InvalidateRect(hWnd,&rcInvalid, FALSE);
		}
            }
            break;
	}
	// force scroll after 2 lines. you will scroll faster if you increase
	// this, but it may not look good.

	if(iLinesOut > 2) {
	    StdioUpdate(hWnd, hDC, iLinesOut);
	    iLinesOut = 0;
	}
    }

    // force scroll and update at the end of each bunch of characters.
    StdioUpdate(hWnd, hDC, iLinesOut);
}

//========================================================================\\

// FUNCTION: StdioPaint(HWND)

// PURPOSE:  The I/O window paint procedure.  Draws necessary text in 
//           the window.

//========================================================================\\

void StdioPaint(HWND hWnd )   
{
    char *psLine;
    register int i;
    PAINTSTRUCT ps;
    HDC hDC;
    RECT rcUpdate, rcClient;
    int nVPaintBeg, nVPaintEnd, nHPaintBeg, nHPaintEnd;

    hDC = BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
    SetupStdioDC(hWnd,hDC);

    rcUpdate = ps.rcPaint;
    DPtoLP(hDC,(POINT *) &rcUpdate, 2);

    // calculate first and last lines to update
    nVPaintBeg = max (0, rcUpdate.top);
    nVPaintEnd = min (MaxLines, rcUpdate.bottom);

    // calculate the first and last columns to update
    nHPaintBeg = max (0, rcUpdate.left);
    nHPaintEnd = min (80, rcUpdate.right);

    // display the lines that need to be drawn
    for(i=nVPaintBeg; i<nVPaintEnd; i++){
	psLine = GetStdioLine(i) + nHPaintBeg;
        TextOut(hDC,
		nHPaintBeg,
                i,
		psLine,
		strlen(psLine));
    }
    ResetStdioDC(hDC);
    EndPaint( hWnd, (LPPAINTSTRUCT)&ps );

}


//========================================================================\\

// FUNCTION: InitStdio(HWND)

// PURPOSE:  Initialize variables used by I/O window.

//========================================================================\\

void InitStdio(HWND hWnd)
{
    int i,j;
    HDC hDC;
    TEXTMETRIC Metrics;

    // initialize screen buffer to nulls 
    for(i=0; i<MaxLines; i++)
	for(j=0; j<81; j++)
	    sScrBuff[i][j] = '\0';

    nFirstLine = 0;
    nLastLine = MaxLine;
    nCurrPos = 0;

    // get the text metrics for the font we are using
    hDC = GetDC(hWnd);
    hOldFont = SelectObject(hDC,GetStockObject(Stdio_FONT));
    GetTextMetrics(hDC,&Metrics);
    SelectObject(hDC,hOldFont);
    ReleaseDC(hWnd,hDC);

    // calculate the height and width of the font 
    nStdioCharWidth = Metrics.tmMaxCharWidth;
    nStdioCharHeight = Metrics.tmHeight + Metrics.tmExternalLeading;

    // get the background and forground colors we are going to use
    StdiobkRGB = GetSysColor(COLOR_WINDOW); // background color
    StdiotextRGB = GetSysColor(COLOR_WINDOWTEXT); // text color

    bInited = TRUE;

}


//========================================================================\\

// FUNCTION: stdioInit(HANDLE)

// PURPOSE:  Initialize the stdio module. Registers the window class.

// RETURNS:  Status of RegisterClass().

//========================================================================\\

BOOL stdioInit(HANDLE hInstance)
{

    PWNDCLASS pStdioClass;
    HWND hDummyWnd;

    if(hInstance == NULL) return FALSE;

    // create the stdio window
    pStdioClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    pStdioClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
    pStdioClass->lpszClassName  = (LPSTR)"Stdio";
    pStdioClass->hbrBackground  = COLOR_WINDOW + 1;
    pStdioClass->hInstance      = hInstance;
    pStdioClass->style		= CS_HREDRAW | CS_VREDRAW;
    pStdioClass->lpfnWndProc    = StdioWndProc;

    if (!RegisterClass( (LPWNDCLASS)pStdioClass ) )
        // Initialization failed.
        // Windows will automatically deallocate all allocated memory.
        return FALSE;

    LocalFree( (HANDLE)pStdioClass );

    hStdioInst = hInstance;
    return TRUE;
}


//========================================================================\\

// FUNCTION: wopen(HWND, BOOL)

// PURPOSE:  Create a default style stdio window. If bQuit is TRUE, 
//           PostQuitMessage will be called when the window is closed. 
//           Therefore, the stdio window can be used for the main 
//           application window.

// RETURNS:  Handle to window created. 

//========================================================================\\

HWND wopen(HWND hWndParent, BOOL bQuit)
{

    // if window already created, return handle
    if(hWndStdio != NULL) return hWndStdio;

    hWndStdio = CreateWindow((LPSTR)"Stdio",
                    (LPSTR)"STDIO",
		    WS_OVERLAPPEDWINDOW,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    (HWND)hWndParent,
                    (HMENU)NULL,       
                    (HANDLE)hStdioInst, 
                    (LPSTR)NULL        
                );
    if(hWndStdio == NULL) return FALSE;
    ShowWindow(hWndStdio, SW_SHOW);
    UpdateWindow(hWndStdio);

    bStdioQuit = bQuit;
    return hWndStdio;

}


//========================================================================\\

// FUNCTION: CreateStdioWindow(LPSTR, DWORD, int, int, int, int, HWND, 
//                            HANDLE, BOOL)

// PURPOSE:  Create an I/O window with definable name, style, size, etc.

// RETURNS:  Handle to window created. 

//========================================================================\\

HWND CreateStdioWindow(LPSTR lpWindowName, DWORD dwStyle,
                       int X, int Y, int nWidth, int nHeight,
                       HWND hWndParent, HANDLE hInstance, BOOL bQuit)
{

    // if window already created, return handle
    if(hWndStdio != NULL) return hWndStdio;

    hWndStdio = CreateWindow((LPSTR)"Stdio",
                    (LPSTR)lpWindowName,
                    dwStyle,
                    X,
                    Y,
                    nWidth,
                    nHeight,
                    (HWND)hWndParent,
                    (HMENU)NULL,       
                    (HANDLE)hInstance, 
                    (LPSTR)NULL);

    if(hWndStdio == NULL) return FALSE;

    bStdioQuit = bQuit;
    return hWndStdio;
}


//========================================================================\\

// FUNCTION: wputs(LPSTR)

// PURPOSE:  Equivalent to puts() stdio function. Currently, '\n' is
//           not recognized as in '\r\n', as with normal puts(). Must 
//           send '\r\n' explicitly.

// RETURNS:  Status of wopen(), if called, otherwise TRUE.

//========================================================================\\


BOOL wputs(LPSTR lpStr)
{
    HDC hDC;
    int nStrLen;

    // if being used for quick and dirty text output, a stdio window
    // will be opened if it hasn't been already.
    if(hWndStdio == NULL) if(wopen(NULL, FALSE) == NULL) return FALSE;

    hDC = GetDC(hWndStdio);
    SetupStdioDC(hWndStdio,hDC);
    nStrLen = lstrlen(lpStr);

    putStdio(hWndStdio,hDC,nStrLen,(LPSTR)lpStr);

    ResetStdioDC(hDC);
    ReleaseDC(hWndStdio,hDC);

    return TRUE;
}


//========================================================================\\

// FUNCTION: StdioWndProc(HWND, unsigned, WORD, LONG)

// PURPOSE:  Process messages for the I/O window. This function should 
//           be exported in the application's .DEF file. 

//========================================================================\\

long FAR PASCAL StdioWndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    PAINTSTRUCT ps;
    HDC hDC;
    LPPOINT ptMinMaxInfo;


    switch (message)
    {

    case WM_CREATE:

	// initialize stdio variables
	InitStdio(hWnd);
	break;


    case WM_SYSCOLORCHANGE:

	// if the colors have been changed in the control panel,
	// we need to change also.

	StdiobkRGB = GetSysColor(COLOR_WINDOW); // background color
	StdiotextRGB = GetSysColor(COLOR_WINDOWTEXT); // text color
        return DefWindowProc( hWnd, message, wParam, lParam );
	break;

    case WM_GETMINMAXINFO:
	if(!bInited) InitStdio(hWnd);

	// constrain the sizing of the window to 80 by 25 characters.

	ptMinMaxInfo = (LPPOINT) lParam;

	ptMinMaxInfo[1].x = nStdioCharWidth * 80
			     + 2 * GetSystemMetrics(SM_CXFRAME);
	ptMinMaxInfo[1].y = nStdioCharHeight * 26
			     + 2 * GetSystemMetrics(SM_CYFRAME);

	ptMinMaxInfo[4].x = nStdioCharWidth * 80
			     + 2 * GetSystemMetrics(SM_CXFRAME);
	ptMinMaxInfo[4].y = nStdioCharHeight * 26
			     + 2 * GetSystemMetrics(SM_CYFRAME);
	break;

    case WM_PAINT:

	// repaint the Stdio window
	StdioPaint(hWnd);
	break;

    case WM_DESTROY:

	// if specified when created, PostQuitMessage should be called
	// when the window is destroyed.

        if(bStdioQuit)
            PostQuitMessage(0);
        break;

    case WM_CLOSE:
        // destroy stdio data
        hWndStdio = NULL;

        // go ahead and close down
        // -- fall through to default --
    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
        break;
    }
    return(0L);
}

