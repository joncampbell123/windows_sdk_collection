/*--------------------------------------------------------------------
|
| MovPlay.c - Sample Win app to play AVI movies using mciSendString
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
#include <stdio.h>
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
    return mciSendString("open avivideo", NULL, 0, NULL) == 0;
}

void termAVI(void)
{
    mciSendString("close avivideo", NULL, 0, NULL);
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
				mciSendString("seek mov to start", NULL, 0,
						NULL);
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
   RECT	rcClient;
   RECT rcMovieBounds;
   char	achRect[128];
   char *p;
   
   /* if there is no movie yet then just get out of here */
   if (!fMovieOpen)
	   return;
   
   GetClientRect(hWnd, &rcClient);	/* get the parent windows rect */
	   
   /* get the original size of the movie */
   mciSendString("where mov source", (LPSTR)achRect, sizeof(achRect), NULL);
   
   SetRectEmpty(&rcMovieBounds);	// zero out movie rect
   p = achRect;	// point to rectangle string returned by where command 
   while (*p == ' ') p++;	// skip over starting spaces
   while (*p != ' ') p++;	// skip over the x (which is 0) 
   while (*p == ' ') p++;	// skip over spaces between x and y
   while (*p != ' ') p++;	// skip over the y (which is 0)
   while (*p == ' ') p++;	// skip over the spaces between y and width
       
   /* now find the width */
   for (; *p >= '0' && *p <= '9'; p++)
       rcMovieBounds.right = (10 * rcMovieBounds.right) + (*p - '0');
   while (*p == ' ') p++;	// skip spaces between width and height
   
   /* now find the height */
   for (; *p >= '0' && *p <= '9'; p++)
       rcMovieBounds.bottom = (10 * rcMovieBounds.bottom) + (*p - '0');

   /* figure out where to position the window at */
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
  mciSendString("close mov", NULL, 0, NULL);

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
   static int  nLastFilter = 1;	  /* keep last file-type opened */

   /* use the OpenFile dialog to get the filename */
   memset(&ofn, 0, sizeof(ofn));
   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hWnd;
   ofn.lpstrFilter = "Video for Windows\0*.avi\0\0";
   ofn.nFilterIndex = nLastFilter;
   ofn.lpstrFile = szFile;
   ofn.nMaxFile = sizeof(szFile);
   ofn.lpstrFileTitle = szFileTitle;
   ofn.nMaxFileTitle = sizeof(szFileTitle);
   ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

   /* use CommDlg to get our filename */
   if (GetOpenFileName(&ofn)){
	 char	achCommand[BUFFER_LENGTH];
	 DWORD  dw;

	 /* we got a filename, now close any old movie and open */
	 /* the new one.					*/
	 if (fMovieOpen)
		 fileCloseMovie(hWnd);	
	 
	 /* we have a .AVI movie to open, use MCI */
	 /* 
	 | build up the string -
	 |      open alias mov parent HWND FILENAME
	 | to pass to mciSendString 
	 */
    wsprintf((LPSTR)achCommand,"open %s alias mov style child parent %d",
             ofn.lpstrFile,hWnd);

	 /* try to open the file */
	 if (mciSendString((LPSTR)achCommand, NULL, 0, NULL) == 0){

		 fMovieOpen = TRUE;

		 /* we opened the file o.k., now set up to */
		 /* play it.				   */
		 mciSendString("window mov state show", NULL, 0, NULL);
		 /* get the window handle */
		 if ((dw = mciSendString("status mov window handle", 
			(LPSTR)achCommand, sizeof(achCommand), 
			NULL)) == 0L)
		    hwndMovie = (HWND)atoi(achCommand);
		 else {
		     mciGetErrorString(dw, achCommand, 
				     sizeof(achCommand));
		     MessageBox(hWnd, achCommand, NULL,
				 MB_ICONEXCLAMATION|MB_OK);
		 }
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
	   if (wDirection == IDM_RPLAY){
	        mciSendString("play mov reverse notify", NULL, 0, hWnd);
	   } else {
	       mciSendString("play mov notify", NULL, 0, hWnd);
	   }
   } else {
	   /* tell it to pause */
	   mciSendString("pause mov", NULL, 0, NULL);
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
           mciSendString("seek mov to start", NULL, 0, NULL);
			
   } else if (wAction == IDM_END){
	   /* go to the end of the movie */
           mciSendString("seek mov to end", NULL, 0, NULL);
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
   if (fPlaying)
	   playMovie(hWnd, NULL);  /* turn off the movie */
       
   if (wDirection == IDM_STEP)
	   mciSendString("step mov by 1", NULL, 0, NULL);
   else
	   mciSendString("step mov reverse by 1", NULL,0, NULL);
}

/*--------------------------- end of file ----------------------*/
