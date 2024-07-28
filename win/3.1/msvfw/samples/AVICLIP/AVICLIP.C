/*****************************************************************************
 ***								           ***
 ***     Project:  AVICLIP - Utility for capturing AVI frames to the       ***
 ***                         ClipBoard				           ***
 ***								           ***
 ***      Module:  aviclip.c					           ***
 ***									   ***
 ***     Remarks:							   ***
 ***									   ***
 ***									   ***
 *****************************************************************************/
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
#include <mmsystem.h>
#include <commdlg.h>
#include <vfw.h>
#include <commdlg.h>
#include "aviclip.h"

/*****************************************************************************
 ***  Global Variables           
 *****************************************************************************/

static  char   	szAppName[]="AVIClip";
static	HANDLE	hInstApp;
static  HWND	hwndApp;
static	HWND	hwndMCI;
static 	RECT	rcMCIWnd;

static 	char gachFileName[128] = "";


/*****************************************************************************
 ***  Global Functions      
 *****************************************************************************/

typedef LONG (FAR PASCAL *LPWNDPROC)(); // pointer to a window procedure

LONG FAR PASCAL _export AppWndProc (HWND hwnd, WORD uiMessage, WORD wParam, LONG lParam);

LONG  NEAR PASCAL AppCommand(HWND hwnd, WORD wParam, LONG lParam);

HWND MakeMCIWnd();
void DisplayMCIError( HWND hwndMCIWnd );
void ClipFrame(LONG lCurFrame);

WORD 	DIBNumColors (LPSTR lpbi);
WORD 	PaletteSize (LPSTR lpbi);
LPSTR 	FindDIBBits (LPSTR lpbi);
HPALETTE CopyPalette(HPALETTE hpal);
HANDLE 	CopyDib(HANDLE hdib);


/*****************************************************************************
 ***   AppInit(hInst, hPrev)                                               *** 
 ***                                                                       ***
 ***   Description:                                                        ***
 ***       This is called when the application is first loaded into        ***
 ***       memory.  It performs all initialization that doesn't            ***
 ***       need to be done once per instance.                    	   ***
 ***                                                                       ***
 ***   Arguments:                                                          ***
 ***       hInstance       instance handle of current instance             ***
 ***       hPrev           instance handle of previous instance            ***
 ***                                                                       ***
 ***   Returns:                                                            ***
 ***     TRUE if successful, FALSE if not                                  ***
 ***                                                                       ***
 *****************************************************************************/

BOOL AppInit(HANDLE hInst,HANDLE hPrev,WORD sw,LPSTR szCmdLine)
{
	WNDCLASS   cls;
	int        iWinHeight;
	WORD	wVer;

	/* first let's make sure we are running on 1.1 */
	wVer = HIWORD(VideoForWindowsVersion());
	if (wVer < 0x010a){
		/* oops, we are too old, blow out of here */
		MessageBeep(MB_ICONHAND);
		MessageBox(NULL, "Video for Windows version is too old",
			  "AVIClip Error", MB_OK|MB_ICONSTOP);
		return FALSE;
	}
 

	/* Save instance handle for DialogBoxs */
	hInstApp = hInst;


	if (!hPrev) {
        	cls.hCursor        = LoadCursor(NULL,IDC_ARROW);
		cls.hIcon          = LoadIcon(hInst,MAKEINTATOM(ID_APP));
   		cls.lpszMenuName   = MAKEINTRESOURCE (MENU_AVICLIP);
		cls.lpszClassName  = MAKEINTATOM(ID_APP);
		cls.hbrBackground  = GetStockObject(LTGRAY_BRUSH) ;
		cls.hInstance      = hInst;
		cls.style          = CS_BYTEALIGNCLIENT | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
		cls.lpfnWndProc    = AppWndProc;
		cls.cbWndExtra     = 0;
		cls.cbClsExtra     = 0;

		if (!RegisterClass(&cls))
	    	return FALSE;
    	}

	// Need to initialize AVI 
	AVIFileInit();

	iWinHeight = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYMENU) + 
   			(GetSystemMetrics(SM_CYFRAME) * 2);
   			
	/* create the main window for the app */
	hwndApp = CreateWindow (MAKEINTATOM(ID_APP),    	// Class name
				 szAppName,              	// Caption
				 WS_OVERLAPPED | WS_CAPTION |   
				 WS_SYSMENU | WS_MINIMIZEBOX,	// Style
  				 CW_USEDEFAULT,CW_USEDEFAULT, 	// Pos
				 180, iWinHeight,               // Size
				 (HWND)NULL,             	// Parent window (no parent)
				 (HMENU)NULL,            	// use class menu
				 (HANDLE)hInst,          	// handle to window instance
				 (LPSTR)NULL             	// no params to pass on
				);
        
        if (hwndApp) {
		ShowWindow(hwndApp,sw);
     		return TRUE; 
     	} else 
     		return FALSE;
}




/*****************************************************************************
 ***   WinMain(hInst, hPrev, lpszCmdLine, cmdShow)                         ***
 ***                                                                       ***
 ***   Description:                                                        ***
 ***     The main procedure for the App.  After initializing, it just goes *** 
 ***     into a message-processing loop until it gets a WM_QUIT message    ***
 ***     (meaning the app was closed).                                     ***
 ***                                                                       ***
 ***   Arguments:                                                          ***
 ***     hInst         instance handle of this instance of the app         ***
 ***     hPrev         instance handle of previous instance, NULL if first ***
 ***     szCmdLine     ->null-terminated command line                      ***
 ***     cmdShow       specifies how the window is initially displayed     ***
 ***                                                                       ***
 ***   Returns:                                                            ***
 ***     The exit code as specified in the WM_QUIT message.                ***
 ***                                                                       *** 
 *****************************************************************************/
int PASCAL WinMain( HANDLE hInst,HANDLE hPrev,LPSTR szCmdLine, WORD sw)
{
	MSG     msg;

	// Call initialization procedure 
	if (!AppInit(hInst,hPrev,sw,szCmdLine))
		return FALSE;

  
	// Polling messages from event queue
	while (GetMessage(&msg,NULL,0,0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
    	}

	return msg.wParam;
}



/*****************************************************************************
 ***   AppWndProc(hwnd, uiMessage, wParam, lParam)                         ***  
 ***                                                                       ***
 ***   Description:                                                        ***
 ***       The window proc for the app's main (tiled) window.  This	   ***
 ***	  processes all of the parent window's messages.                   ***
 ***                                                                       ***
 ***   Arguments:                                                          ***
 ***       hwnd            window handle for the window                    ***
 ***       uiMessage       message number                                  ***
 ***       wParam          message-dependent                               ***
 ***       lParam          message-dependent                               ***
 ***                                                                       ***
 ***   Returns:                                                            ***
 ***       0 if processed, nonzero if ignored                              ***
 ***                                                                       ***   
\******************************************************************************/
LONG FAR PASCAL _export AppWndProc( HWND hwnd,WORD msg, WORD wParam, LONG lParam)
{
    	RECT 	rc;
	PAINTSTRUCT ps; 
	   
    	switch (msg) 
    	{
		case WM_COMMAND:
	   		return AppCommand(hwnd,wParam,lParam);
	    
		case WM_DESTROY:
			if( hwndMCI )
				MCIWndDestroy(hwndMCI);
			
			// Deinitalize AVI stuff
			AVIFileExit();

			PostQuitMessage(0);
	    		break; 
	    		
		case WM_PAINT:
			BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			return 0; 
		
		case WM_SIZE:
			if (hwndMCI)
				MoveWindow(hwndMCI,0,0,LOWORD(lParam),HIWORD(lParam),TRUE);
        		break;   
        		
    		case MCIWNDM_NOTIFYSIZE:
    			if (hwndMCI){  
	    			/* adjust to size of the movie window */
				GetWindowRect(hwndMCI, &rc);
				AdjustWindowRect(&rc, GetWindowLong(hwnd, GWL_STYLE), TRUE);
				SetWindowPos(hwnd, NULL, 0, 0, rc.right - rc.left,
                    			rc.bottom - rc.top,
                    			SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE); 
                	} else {  
	                	/* movie closed, adjust to the default size */
	                	int iWinHeight;  
	                	
				iWinHeight = GetSystemMetrics(SM_CYCAPTION) + 
						GetSystemMetrics(SM_CYMENU) + 
	   					(GetSystemMetrics(SM_CYFRAME) * 2); 
	   			SetWindowPos(hwnd, NULL, 0, 0, 180, iWinHeight, 
	   				SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
	                }
	   		break; 
	   }
        		

    return DefWindowProc( hwnd, msg, wParam, lParam);

}


/*****************************************************************************
 ***   AppCommand( hwnd, wParam, lParam)                                   ***  
 ***                                                                       ***
 ***   Description:                                                        ***
 ***       The procedure to handle all the WM_COMMAND messages.   	   ***
 ***                                                                       ***
 ***   Arguments:                                                          ***
 ***       hwnd            window handle for the window                    ***
 ***       wParam          message-dependent                               ***
 ***       lParam          message-dependent                               ***
 ***                                                                       ***
 ***   Returns:                                                            ***
 ***       0L                                                              ***
 ***                                                                       ***   
\******************************************************************************/
LONG NEAR PASCAL AppCommand ( HWND hwnd, WORD wParam, LONG lParam)

{

	OPENFILENAME		ofn;
	LONG			lResult;
	LONG			lCurFrame;

	switch( wParam ){
 	 	case IDM_EXIT:
			PostMessage(hwndApp,WM_CLOSE,0,0L);
		    	break;

		case IDM_OPEN:
			if( hwndMCI )
				MCIWndDestroy(hwndMCI);	// MCIWnd is already open, kill it

			hwndMCI = MakeMCIWnd();
			
			if ( hwndMCI ) {
            			gachFileName[0] = 0;
				ofn.lStructSize = sizeof(OPENFILENAME);
            			ofn.hwndOwner = hwnd;
            			ofn.hInstance = NULL;
				ofn.lpstrTitle = "Open AVI";
				ofn.lpstrFilter = "AVI Files\0*.avi\0";
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

            			if (GetOpenFileNamePreview(&ofn))
					lResult = MCIWndOpen( hwndMCI, (LPSTR)gachFileName, 0);

				if( lResult )
					DisplayMCIError( hwndMCI );
				else {		
			 		GetClientRect(hwndMCI, &rcMCIWnd);
			   		ShowWindow( hwndMCI, SW_SHOWNORMAL );
					
					// Enable the snip button, we have AVI
					EnableWindow( GetDlgItem( hwnd, IDM_SNIP ), TRUE);

					// Set AVI sequence to first frame
					lResult = MCIWndHome( hwndMCI );
					if( lResult )
						DisplayMCIError( hwndMCI );
				}
			}
			break;

		case IDM_SNIP:
			if (hwndMCI){
				lCurFrame = MCIWndGetPosition( hwndMCI );
				ClipFrame( lCurFrame );
			}
			break;
    	}
	return 0L;
}


/**************************************************************
 ***  DisplayMCIError, display an MCI error in a messagebox
 **************************************************************/

void DisplayMCIError( HWND hwndMCIWnd )
{
	char szBuf[128];
	
	MCIWndGetError( hwndMCIWnd, (LPSTR)szBuf, sizeof( szBuf ) );
	MessageBox( hwndApp, (LPSTR)szBuf, "MCI Error", MB_OK);
}


/***********************************************************
 ***  MakeMCIWnd, make an MCI window
 ***********************************************************/

HWND MakeMCIWnd()
{
	HWND	hwndMCIWnd;

	hwndMCIWnd = MCIWndCreate(hwndApp, hInstApp,
					MCIWNDF_NOMENU              |
					MCIWNDF_NOTIFYMODE	    |
           				MCIWNDF_NOTIFYSIZE	    |
					MCIWNDF_SHOWNAME            |
           				MCIWNDF_SHOWPOS             |
          				MCIWNDF_NOERRORDLG          |
  					WS_CHILD		    |
  					WS_DLGFRAME,

  					NULL);
	if (hwndMCIWnd == NULL)
		return 0; 
		
	MCIWndUseFrames( hwndMCIWnd );	// Set time format to frames
	return hwndMCIWnd;
}


/***********************************************************
 ***  ClipFrame, copy single AVI frame to clipboard
 ***********************************************************/

void ClipFrame(LONG lCurFrame)
{
	PAVIFILE	pfile;
	PAVISTREAM 	pstream;
	PGETFRAME	pget;

	HPALETTE	hPal, hPalNew = NULL;	// remember to init these
	HRESULT		hResult;
	LPBITMAPINFOHEADER  lpbi;
	LPSTR		lpDIBBits;

	hResult = AVIFileOpen(&pfile, (LPCSTR)gachFileName, 0, 0L);
	hResult = AVIFileGetStream(pfile, &pstream, streamtypeVIDEO, 0);
	
	pget = AVIStreamGetFrameOpen(pstream, NULL);
	
	if(pget) { // Great, decompressor open

		// Get our own personal copy of the DIB
		lpbi = AVIStreamGetFrame(pget, lCurFrame);
		if (lpbi)
			lpbi = (LPBITMAPINFOHEADER)MAKELONG(0,
				CopyDib((HANDLE)HIWORD((DWORD)(lpbi))));

		// Find the beginning of the bits
		if (lpbi)
		    lpDIBBits = FindDIBBits ((LPSTR)lpbi);

		// Get our own copy of the current palette
		hPal = MCIWndGetPalette(hwndMCI);
		if (hPal)
			hPalNew = CopyPalette(hPal);

		// We have valid stuff to put on the clipboard
		OpenClipboard(hwndMCI);
		EmptyClipboard();

		if (hPalNew)
		    SetClipboardData(CF_PALETTE, hPalNew);
		if (lpbi)
		    SetClipboardData(CF_DIB, (HANDLE)HIWORD((DWORD)(lpbi)));

		CloseClipboard();

		// Now release our stuff
		AVIStreamGetFrameClose(pget);
	} 
	
	// Close the stream and file
	AVIStreamRelease( pstream );
	AVIFileRelease( pfile );
}


/***********************************************************
 ***  DIBNumColors, return the number of colors in a DIB
 ***********************************************************/

WORD DIBNumColors (LPSTR lpbi)
{

	//  For a Windows style DIB, the number of colors in the
	//  color table can be less than the number of bits per pixel
	//  allows for (i.e. lpbi->biClrUsed can be set to some value).
	//  If this is the case, return the appropriate value.
 	if (((LPBITMAPINFOHEADER)lpbi)->biClrUsed != 0)
		return (WORD) ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;

	// Calculate the number of colors in the color table based on
	//  the number of bits per pixel for the DIB.
 	switch (((LPBITMAPINFOHEADER) lpbi)->biBitCount) {
		case 1:
			return 2;
		case 4:
			return 16;
		case 8:
			return 256;
		default:
			return 0;
	}
}


/***********************************************************
 ***  PaletteSize, makes a copy of a GDI logical palette
 ***********************************************************/

WORD PaletteSize (LPSTR lpbi)
{
      return (DIBNumColors (lpbi) * sizeof (RGBQUAD));
}


/**********************************************************
 ***  FindDIBBits, return the entire DIB
 **********************************************************/

LPSTR FindDIBBits (LPSTR lpbi)
{
   return (lpbi + *(LPDWORD)lpbi + PaletteSize (lpbi));
}


/*********************************************************
 ***  CopyDib, makes a copy of a DIB and locks it down
 *********************************************************/

HANDLE CopyDib (HANDLE hdib)
{
    BYTE huge *ps;
    BYTE huge *pd;
    HANDLE h;
    DWORD cnt;

    if (h = GlobalAlloc(GMEM_MOVEABLE, cnt = GlobalSize(hdib)))
    {
        ps = GlobalLock(hdib);
        pd = GlobalLock(h);

        while (cnt-- > 0)
           *pd++ = *ps++;

        GlobalUnlock(hdib);
        GlobalUnlock(h);
    }
    return (HANDLE)(HIWORD((DWORD)GlobalLock(h)));
}


/**********************************************************
 ***  CopyPalette, makes a copy of a GDI logical palette
 **********************************************************/

HPALETTE CopyPalette(HPALETTE hpal)
{
    PLOGPALETTE ppal;
    int         nNumEntries=0;

    if (!hpal)
        return NULL;

    GetObject(hpal,sizeof(int),(LPSTR)&nNumEntries);

    if (nNumEntries == 0)
        return NULL;

    ppal = (PLOGPALETTE)LocalAlloc(LPTR,sizeof(LOGPALETTE) +
                nNumEntries * sizeof(PALETTEENTRY));

    if (!ppal)
        return NULL;

    ppal->palVersion    = PALVERSION;
    ppal->palNumEntries = nNumEntries;

    GetPaletteEntries(hpal,0,nNumEntries,ppal->palPalEntry);

    hpal = CreatePalette(ppal);

    LocalFree((HANDLE)ppal);
    return hpal;
}

/************************************************************************
 *** End of File: AVICLIP.C
 ************************************************************************/

