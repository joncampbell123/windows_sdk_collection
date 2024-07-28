/*--------------------------------------------------------------------
|
| MovPlay.c - Sample Win app to play AVI movies using mciSendCommand
|
| Movie Functions supported:
|	Play/Pause
|	Home/End
|	Step/ReverseStep
| 
+--------------------------------------------------------------------*/
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
#include <commdlg.h>
#include <string.h>
#include <stdlib.h>
#include <direct.h>
#include <mmsystem.h>		
#include <digitalv.h>		
#include "movplay.h"


/* function declarations */
long FAR PASCAL _export WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void fileCloseMovie(HWND hWnd);
void fileOpenMovie(HWND hWnd);
void positionMovie(HWND hWnd);
void playMovie(HWND hWnd, WORD wDirection);
void seekMovie(HWND hWnd, WORD wAction);
void stepMovie(HWND hWnd, WORD wDirection);
void menubarUpdate(HWND hWnd);
void titlebarUpdate(HWND hWnd, LPSTR lpstrMovie);


/**************************************************************
************************ GLOBALS ******************************
**************************************************************/
/* AVI stuff to keep around */
WORD wMCIDeviceID = 0;	/* MCI Device ID for the AVI file */
HWND hwndMovie;		/* window handle of the movie */
RECT rcMovie;		/* the rect where the movie is positioned      */
			/* for QT/W this is the movie rect, for AVI    */
			/* this is the location of the playback window */
BOOL fPlaying = FALSE;	/* Play flag: TRUE == playing, FALSE == paused */
BOOL fMovieOpen = FALSE;/* Open flag: TRUE == movie open, FALSE = none */
HANDLE hAccel = NULL;	/* accelerator table */
HMENU hMenuBar = NULL;	/* menu bar handle */
char szAppName [] = "MovPlay";

/********************************************************************
************************** FUNCTIONS ********************************
********************************************************************/
/*--------------------------------------------------------------+
| initAVI - initialize avi libraries				|
|								|
+--------------------------------------------------------------*/
BOOL initAVI(void)
{
    MCI_DGV_OPEN_PARMS	mciOpen;
		 
    /* set up the open parameters */
    mciOpen.dwCallback = NULL;
    mciOpen.wDeviceID = mciOpen.wReserved0 =
			 mciOpen.wReserved1 = 0;
    mciOpen.lpstrDeviceType = "avivideo";
    mciOpen.lpstrElementName = NULL;
    mciOpen.lpstrAlias = NULL;
    mciOpen.dwStyle = 0;
    mciOpen.hWndParent = NULL;
		 
   /* try to open the driver */
   return (mciSendCommand(0, MCI_OPEN, (DWORD)(MCI_OPEN_TYPE), 
                          (DWORD)(LPMCI_DGV_OPEN_PARMS)&mciOpen) == 0);
}

/*--------------------------------------------------------------+
| termAVI - Closes the opened AVI file and the opened device    |
|           type.                                               |
|                                                               |
+--------------------------------------------------------------*/
void termAVI(void)
{
    WORD               wID;
    MCI_GENERIC_PARMS  mciClose;

    //
    // Get the device ID for the opened device type and then close
    // the device type.
    //
    wID = mciGetDeviceID("avivideo");
    mciSendCommand(wID, MCI_CLOSE, 0L, 
                   (DWORD)(LPMCI_GENERIC_PARMS)&mciClose);
}


/*--------------------------------------------------------------+
| initApp - initialize the app overall.				|
|								|
| Returns the Window handle for the app on success, NULL if	|
| there is a failure.						|
|								|
+--------------------------------------------------------------*/
HWND initApp(HINSTANCE hInstance, HINSTANCE hPrevInstance, int nCmdShow)
{
   HWND		hWnd;	/* window handle to return */
   
   if (!hPrevInstance){
      WNDCLASS    wndclass; 
      
      wndclass.style         = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
      wndclass.lpfnWndProc   = WndProc;
      wndclass.cbClsExtra    = 0;
      wndclass.cbWndExtra    = 0;
      wndclass.hInstance     = hInstance;
      wndclass.hIcon         = LoadIcon (hInstance, "AppIcon");
      wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
      wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
      wndclass.lpszMenuName  = szAppName;
      wndclass.lpszClassName = szAppName;

      if (!RegisterClass(&wndclass)){
         MessageBox(NULL, "RegisterClass failure", szAppName, MB_OK);
         return NULL;
      }
   }

   /* create the main window for the app */
   hWnd = CreateWindow(szAppName, szAppName, WS_OVERLAPPEDWINDOW |
      WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
      CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

   if (hWnd == NULL){
      MessageBox(NULL, "CreateWindow failure", szAppName, MB_OK);
      return NULL;
   }

   hMenuBar = GetMenu(hWnd);	/* get the menu bar handle */
   menubarUpdate(hWnd);		/* update menu bar to disable Movie menu */
   
   /* Show the main window */
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   /* load the accelerator table */
   hAccel = LoadAccelerators(hInstance, szAppName);
   
   return hWnd;
}


/*--------------------------------------------------------------+
| WinMain - main routine.					|
|								|
+--------------------------------------------------------------*/
int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
				LPSTR lpszCmdParam, int nCmdShow)
{
   HWND        hWnd;
   MSG         msg;

   if (!initAVI())
	   return 0;
       
   if ((hWnd = initApp(hInstance, hPrevInstance,nCmdShow)) == NULL)
	   return 0;	/* died initializing, bail out */

   /* main message loop, be sure to handle accelerators */
   while (GetMessage(&msg, NULL, 0, 0)){
	  if (!TranslateAccelerator(hWnd, hAccel, &msg)) {
		  TranslateMessage(&msg);
		  DispatchMessage(&msg);
	  }
   }

   return msg.wParam;
}


/*--------------------------------------------------------------+
| WndProc - window proc for the app				|
|								|
+--------------------------------------------------------------*/
long FAR PASCAL _export WndProc (HWND hWnd, UINT message, WPARAM wParam,
						LPARAM lParam)
{
   PAINTSTRUCT ps;
   WORD w;
   WORD	wMenu;

   switch (message){
        case WM_CREATE:
		return 0;

	case WM_INITMENUPOPUP:
		/* be sure this isn't the system menu */
		if (HIWORD(lParam))
			return DefWindowProc(hWnd, WM_INITMENUPOPUP, 
						wParam, lParam);
  
		wMenu = LOWORD(lParam);
		switch (wMenu){
			case 0:   /* file menu */
				/* turn on/off CLOSE & PLAY */
				if (fMovieOpen) w = MF_ENABLED|MF_BYCOMMAND;
				else		w = MF_GRAYED|MF_BYCOMMAND;
				EnableMenuItem((HMENU)wParam, IDM_CLOSE, w);
				break;
				
			case 1:	/* Movie menu */
				/* check or uncheck the PLAY menu item */
				if (fPlaying)  w = MF_CHECKED|MF_BYCOMMAND;
				else	       w = MF_UNCHECKED|MF_BYCOMMAND;
				CheckMenuItem((HMENU)wParam, IDM_PLAY, w);
				break;
		} /* switch */
		break;
		
	case WM_COMMAND:
		/* handle the menu commands */
		switch (wParam) {
			/* File Menu */
			case IDM_OPEN:
				fileOpenMovie(hWnd);
				break;
			case IDM_CLOSE:
				fileCloseMovie(hWnd);
				break;
			case IDM_EXIT:
				PostMessage(hWnd, WM_CLOSE, 0, 0L);
				break;
				
			/* Movie Menu - note some of these are by */
			/* keyboard only, especially the REVERSE  */
			/* commands.				  */
			case IDM_PLAY:
			case IDM_RPLAY:
				playMovie(hWnd, wParam);
				break;
			case IDM_HOME:
			case IDM_END:
				seekMovie(hWnd, wParam);
				break;
			case IDM_STEP:
			case IDM_RSTEP:
				stepMovie(hWnd,wParam);
				break;
		}
		return 0;

	case WM_SIZE:
		positionMovie(hWnd);	/* re-center the movie */
		return 0;
		
        case WM_PAINT:
		if (!BeginPaint(hWnd, &ps))
			return 0;
		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		if (fMovieOpen)
		    fileCloseMovie(hWnd);
		termAVI();
		PostQuitMessage(0);
		return 0;
		
	case MM_MCINOTIFY:
		/* This is where we check the status of an AVI	*/
		/* movie that might have been playing.  We do	*/
		/* the play with MCI_NOTIFY on so we should get	*/
		/* a MCI_NOTIFY_SUCCESSFUL if the play		*/
		/* completes on it's own.			*/
		switch(wParam){
			case MCI_NOTIFY_SUCCESSFUL:
				/* the play finished, let's rewind */
				/* and clear our flag.		   */
				fPlaying = FALSE;
				mciSendCommand(wMCIDeviceID, MCI_SEEK, 
				               MCI_SEEK_TO_START, 
                           (DWORD)(LPVOID)NULL);
				return 0;
		}
	 
   } /* switch */
   return DefWindowProc(hWnd, message, wParam, lParam);
}

/*--------------------------------------------------------------+
| menubarUpdate - update the menu bar based on the <fMovieOpen> |
|		  flag value.  This will turn on/off the	|
|		  Movie menu.					|
|								|
+--------------------------------------------------------------*/
void menubarUpdate(HWND hWnd)
{
   WORD w;
   
   if (fMovieOpen){
	   w = MF_ENABLED|MF_BYPOSITION;
   } else {
	   w = MF_GRAYED|MF_BYPOSITION;
   }
   EnableMenuItem(hMenuBar, 1, w);	/* change the Movie menu (#1) */
   DrawMenuBar(hWnd);	/* re-draw the menu bar */
}

/*--------------------------------------------------------------+
| titlebarUpdate - update the title bar to include the name	|
|		   of the movie playing.			|
|								|
+--------------------------------------------------------------*/
void titlebarUpdate(HWND hWnd, LPSTR lpstrMovie)
{
   char achNewTitle[BUFFER_LENGTH];	// space for the title 
   
   if (lpstrMovie != NULL)
       wsprintf((LPSTR)achNewTitle,"%s - %s", (LPSTR)szAppName,lpstrMovie);
   else
       lstrcpy((LPSTR)achNewTitle, (LPSTR)szAppName);
   SetWindowText(hWnd, (LPSTR)achNewTitle);
}

/*--------------------------------------------------------------+ 
| positionMovie - sets the movie rectange <rcMovie> to be	|
|		centered within the app's window.		|
|								|
+--------------------------------------------------------------*/
VOID positionMovie(HWND hWnd)
{
   RECT	rcClient, rcMovieBounds;
   MCI_DGV_RECT_PARMS	mciRect;
   
   /* if there is no movie yet then just get out of here */
   if (!fMovieOpen)
	   return;
   
   GetClientRect(hWnd, &rcClient);	/* get the parent windows rect */
	   
   /* get the original size of the movie */
   mciSendCommand(wMCIDeviceID, MCI_WHERE, 
                  (DWORD)(MCI_DGV_WHERE_SOURCE), 
                  (DWORD)(LPMCI_DGV_RECT_PARMS)&mciRect);
   rcMovieBounds = mciRect.rc;	/* get it in the movie bounds rect */
   
   rcMovie.left = (rcClient.right/2) - (rcMovieBounds.right / 2);
   rcMovie.top = (rcClient.bottom/2) - (rcMovieBounds.bottom / 2);
   rcMovie.right = rcMovie.left + rcMovieBounds.right;
   rcMovie.bottom = rcMovie.top + rcMovieBounds.bottom;
   
   /* reposition the playback (child) window */
   MoveWindow(hwndMovie, rcMovie.left, rcMovie.top,
	   rcMovieBounds.right, rcMovieBounds.bottom, TRUE);
}

/*--------------------------------------------------------------+ 
| fileCloseMovie - close the movie and anything associated	|
|		   with it.					|
|								|
| This function clears the <fPlaying> and <fMovieOpen> flags	|
|								|
+--------------------------------------------------------------*/
void fileCloseMovie(HWND hWnd)
{
  MCI_GENERIC_PARMS  mciGeneric;
    
  mciSendCommand(wMCIDeviceID, MCI_CLOSE, 0L, 
                 (DWORD)(LPMCI_GENERIC_PARMS)&mciGeneric);

  fPlaying = FALSE;	// can't be playing any longer
  fMovieOpen = FALSE;	// no more movies open
	   
  titlebarUpdate(hWnd, NULL);	// title bar back to plain
  menubarUpdate(hWnd);		// update menu bar
	   
  /* cause a total repaint to occur */
  InvalidateRect(hWnd, NULL, TRUE);
  UpdateWindow(hWnd);
}


/*--------------------------------------------------------------+ 
| fileOpenMovie - open an AVI movie. Use CommDlg open box to	|
|	        open and then handle the initialization to	|
|		show the movie and position it properly.  Keep	|
|		the movie paused when opened.			|
|								|
|		Sets <fMovieOpened> on success.			|
+--------------------------------------------------------------*/
void fileOpenMovie(HWND hWnd)
{
   OPENFILENAME ofn;
   
   static char szFile [BUFFER_LENGTH];
   static char szFileTitle [BUFFER_LENGTH];

   /* use the OpenFile dialog to get the filename */
   memset(&ofn, 0, sizeof(ofn));
   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hWnd;
   ofn.lpstrFilter = "Video for Windows\0*.avi\0\0";
   ofn.lpstrFile = szFile;
   ofn.nMaxFile = sizeof(szFile);
   ofn.lpstrFileTitle = szFileTitle;
   ofn.nMaxFileTitle = sizeof(szFileTitle);
   ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

   /* use CommDlg to get our filename */
   if (GetOpenFileName(&ofn)){
	 MCI_DGV_OPEN_PARMS	mciOpen;
	 MCI_DGV_WINDOW_PARMS	mciWindow;
	 MCI_DGV_STATUS_PARMS	mciStatus;

	 /* we got a filename, now close any old movie and open */
	 /* the new one.					*/
	 if (fMovieOpen)
		 fileCloseMovie(hWnd);	
	 
	 /* we have a .AVI movie to open, use MCI */
	 /* set up the open parameters */
	 mciOpen.dwCallback = NULL;
	 mciOpen.wDeviceID = mciOpen.wReserved0 =
		 mciOpen.wReserved1 = 0;
	 mciOpen.lpstrDeviceType = NULL;
	 mciOpen.lpstrElementName = ofn.lpstrFile;
	 mciOpen.lpstrAlias = NULL;
	 mciOpen.dwStyle = WS_CHILD;
	 mciOpen.hWndParent = hWnd;

	 /* try to open the file */
	 if (mciSendCommand(0, MCI_OPEN, 
		 (DWORD)(MCI_OPEN_ELEMENT|MCI_DGV_OPEN_PARENT|MCI_DGV_OPEN_WS), 
       (DWORD)(LPMCI_DGV_OPEN_PARMS)&mciOpen) == 0){

		 /* we opened the file o.k., now set up to */
		 /* play it.				   */
		 wMCIDeviceID = mciOpen.wDeviceID;	/* save ID */
		 fMovieOpen = TRUE;	/* a movie was opened */

		 /* show the playback window */
		 mciWindow.dwCallback = NULL;
		 mciWindow.hWnd = NULL;
		 mciWindow.wReserved1 = mciWindow.wReserved2 = 0;
		 mciWindow.nCmdShow = SW_SHOW;
		 mciWindow.lpstrText = (LPSTR)NULL;
		 mciSendCommand(wMCIDeviceID, MCI_WINDOW, 
			 MCI_DGV_WINDOW_STATE, 
			 (DWORD)(LPMCI_DGV_WINDOW_PARMS)&mciWindow);

		 /* get the window handle */
		 mciStatus.dwItem = MCI_DGV_STATUS_HWND;
		 mciSendCommand(wMCIDeviceID, 
			 MCI_STATUS, MCI_STATUS_ITEM,
			 (DWORD)(LPMCI_STATUS_PARMS)&mciStatus);
		 hwndMovie = (HWND)mciStatus.dwReturn;

		 /* now get the movie centered */
		 positionMovie(hWnd);
	 } else {
		/* generic error for open */
		MessageBox(hWnd, "Unable to open Movie", NULL, 
			      MB_ICONEXCLAMATION|MB_OK);
		fMovieOpen = FALSE;
	 }
   }
   /* update menu and title bars */
   if (fMovieOpen)
	   titlebarUpdate(hWnd, (LPSTR)ofn.lpstrFileTitle);
   else
	   titlebarUpdate(hWnd, NULL);
   menubarUpdate(hWnd);
   
   
   /* cause an update to occur */
   InvalidateRect(hWnd, NULL, FALSE);
   UpdateWindow(hWnd);
}

/*--------------------------------------------------------------+
| playMovie - play/pause the movie depending on the state	|
|		of the <fPlaying> flag.				|
|								|
| This function sets the <fPlaying> flag appropriately when done|
|								|
+--------------------------------------------------------------*/
void playMovie(HWND hWnd, WORD wDirection)
{
   fPlaying = !fPlaying;	/* swap the play flag */
   if (wDirection == NULL)
	   fPlaying = FALSE;	/* wDirection == NULL means PAUSE */

   /* play/pause the AVI movie */
   if (fPlaying){
	   DWORD		            dwFlags;
	   MCI_DGV_PLAY_PARMS	mciPlay;
		   
	   /* init to play all */
 	   mciPlay.dwCallback = MAKELONG(hWnd,0);
	   mciPlay.dwFrom = mciPlay.dwTo = 0;
	   dwFlags = MCI_NOTIFY;
	   if (wDirection == IDM_RPLAY)
		   dwFlags |= MCI_DGV_PLAY_REVERSE;
		   
	   mciSendCommand(wMCIDeviceID, MCI_PLAY, dwFlags,
		               (DWORD)(LPMCI_DGV_PLAY_PARMS)&mciPlay);
   } else {
	   MCI_DGV_PAUSE_PARMS	mciPause;
	   
	   /* tell it to pause */
	   mciSendCommand(wMCIDeviceID,MCI_PAUSE,0L,
                     (DWORD)(LPMCI_DGV_PAUSE_PARMS)&mciPause);
   }
}

/*--------------------------------------------------------------+
| seekMovie - seek in the movie depending on the wAction.	|
|	      Possible actions are IDM_HOME (start of movie) or	|
|	      IDM_END (end of movie)				|
|								|
|	      Always stop the play before seeking.		|
|								|
+--------------------------------------------------------------*/
void seekMovie(HWND hWnd, WORD wAction)
{
   /* first stop the movie from playing if it is playing */
   if (fPlaying){
	   playMovie(hWnd, NULL);	
   }
   if (wAction == IDM_HOME){
	   /* home the movie */
	   mciSendCommand(wMCIDeviceID, MCI_SEEK, MCI_SEEK_TO_START, 
			(DWORD)(LPVOID)NULL);
			
   } else if (wAction == IDM_END){
	   /* go to the end of the movie */
	   mciSendCommand(wMCIDeviceID, MCI_SEEK, MCI_SEEK_TO_END, 
			(DWORD)(LPVOID)NULL);
   } 
}

/*--------------------------------------------------------------+
| stepMovie - step forward or reverse in the movie.  wDirection	|
|		can be IDM_STEP (forward) or IDM_RSTEP (reverse)|
|								|
|		Again, stop the play if one is in progress.	|
|								|
+--------------------------------------------------------------*/
void stepMovie(HWND hWnd, WORD wDirection)
{
   MCI_DGV_STEP_PARMS	mciStep;
   
   if (fPlaying)
	   playMovie(hWnd, NULL);  /* turn off the movie */
   
	   
   mciStep.dwFrames = 1L;
   if (wDirection == IDM_STEP)
	   mciSendCommand(wMCIDeviceID, MCI_STEP, MCI_DGV_STEP_FRAMES,
		   (DWORD)(LPMCI_DGV_STEP_PARMS)&mciStep);
   else
	   mciSendCommand(wMCIDeviceID, MCI_STEP, 
		   MCI_DGV_STEP_FRAMES|MCI_DGV_STEP_REVERSE,
		   (DWORD)(LPMCI_DGV_STEP_PARMS)&mciStep);
}

/*--------------------------- end of file ----------------------*/
