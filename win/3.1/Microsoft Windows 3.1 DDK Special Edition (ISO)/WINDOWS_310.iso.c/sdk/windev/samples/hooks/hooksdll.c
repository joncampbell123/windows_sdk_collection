//---------------------------------------------------------------------------
//  Windows hooks Sample Application - The DLL
//
//  This sample application demonstrates how to use Windows hooks.
//
//  This File contains the source code for the hooksdll
//
//  Author:	Kyle Marsh
//              Windows Developer Technology Group
//              Microsoft Corp.
//
//---------------------------------------------------------------------------


#include "windows.h"
#include "string.h"
#include "hooks.h"

//---------------------------------------------------------------------------
// Function declarations
//---------------------------------------------------------------------------

int   FAR PASCAL LibMain(HANDLE hModule, WORD wDataSeg, WORD cbHeapSize, LPSTR lpszCmdLine);
int   FAR PASCAL WEP (int bSystemExit);
int   FAR PASCAL InitHooksDll(HWND hwndMainWindow, int nWinLineHeight);
int   FAR PASCAL PaintHooksDll(HDC hDC );
int   FAR PASCAL InstallFilter (int nHookIndex, int nCode );
void  FAR PASCAL CallWndProcFunc (int nCode, WORD wParam, DWORD lParam );
int   FAR PASCAL CbtFunc (int nCode, WORD wParam, DWORD lParam );
void  FAR PASCAL GetMessageFunc (int nCode, WORD wParam, DWORD lParam );
DWORD FAR PASCAL JournalPlaybackFunc (int nCode, WORD wParam, DWORD lParam );
void  FAR PASCAL JournalRecordFunc (int nCode, WORD wParam, DWORD lParam );
int   FAR PASCAL KeyboardFunc (int nCode, WORD wParam, DWORD lParam );
int   FAR PASCAL MouseFunc (int nCode, WORD wParam, DWORD lParam );
int   FAR PASCAL SysMsgFilterFunc (int nCode, WORD wParam, DWORD lParam );
char  FAR *szMessageString(WORD wID);

//---------------------------------------------------------------------------
// Global Variables...
//---------------------------------------------------------------------------

HANDLE	hInstance;		// Global instance handle for  DLL
HWND	hwndMain;		// Main hwnd. We will get this from the App
int	InitCalled = 0; 	// Has the Init function been called ?
int	nLineHeight;		// Heigth of lines in window
char	szType[64];		// A Place to write temporary strings
int	nRecordedEvents = -1;	// Number or Events Recorded
DWORD	dwStartRecordTime;	// Time JournalRecord Started

typedef struct TAGEventNode {
	   EVENTMSG	     Event;
    struct TAGEventNode FAR *lpNextEvent;
} EventNode;

EventNode FAR *lpEventChain  = NULL;  // Head of recorded Event List
EventNode FAR *lpLastEvent   = NULL;  // Tail of recorded Event List
EventNode FAR *lpPlayEvent   = NULL;  // Current Event being played

//
//  My Hook States
//
int HookStates[NUMOFHOOKS] = { 0,0,0,0,0,0,0,0,0 } ; // State Table of my hooks

//
//  Hook Codes
//
int HookCodes[NUMOFHOOKS] = {
			       WH_CALLWNDPROC,
			       WH_CBT,
			       WH_GETMESSAGE,
			       WH_JOURNALPLAYBACK,
			       WH_JOURNALRECORD,
			       WH_KEYBOARD,
			       WH_MOUSE,
			       WH_MSGFILTER,
			       WH_SYSMSGFILTER
			    };

//
// Filter Function Addresses
//
FARPROC lpfnHookProcs[NUMOFHOOKS] = {
				      (FARPROC) CallWndProcFunc,
				      (FARPROC) CbtFunc,
				      (FARPROC) GetMessageFunc,
				      (FARPROC) JournalPlaybackFunc,
				      (FARPROC) JournalRecordFunc,
				      (FARPROC) KeyboardFunc,
				      (FARPROC) MouseFunc,
				      NULL,
				      (FARPROC) SysMsgFilterFunc,
				    };

//
// Hook Handles
//
HHOOK hhookHooks[NUMOFHOOKS];

//
// Output Lines
//
char szFilterLine[NUMOFHOOKS][128];

//---------------------------------------------------------------------------
// LibMain
//---------------------------------------------------------------------------
int FAR PASCAL LibMain(HANDLE hModule, WORD wDataSeg, WORD cbHeapSize, LPSTR lpszCmdLine)
{
    hInstance = hModule;
    return 1;
}

#pragma alloc_text(FIXEDSEG, WEP)

//---------------------------------------------------------------------------
// WEP
//---------------------------------------------------------------------------
int FAR PASCAL WEP (int bSystemExit)
{
    return(1);
}



//---------------------------------------------------------------------------
// InitHooksDll
//---------------------------------------------------------------------------
int FAR PASCAL InitHooksDll(HWND hwndMainWindow, int nWinLineHeight)
{
    hwndMain = hwndMainWindow;
    nLineHeight = nWinLineHeight;


    InitCalled = 1;
    return (0);
}


//---------------------------------------------------------------------------
// PaintHooksDll
//---------------------------------------------------------------------------
int FAR PASCAL PaintHooksDll(HDC hDC )
{
   int i;

   for (i = 0; i < NUMOFHOOKS; i++ ) {
       if ( HookCodes[i] != WH_MSGFILTER && HookStates[i] )
	   TabbedTextOut(hDC, 1, nLineHeight * i,
		(LPSTR)szFilterLine[i], strlen(szFilterLine[i]), 0, NULL, 1);
   }

   return (0);
}

//---------------------------------------------------------------------------
// InstallSysMsgFilter
//
// Install / Remove Filter function for the WH_SYSMSGFILTER
//
//---------------------------------------------------------------------------
int FAR PASCAL InstallFilter (int nHookIndex, int nCode )
{

     if ( ! InitCalled ) {
	return (-1);
     }
     if ( nCode ) {
	hhookHooks[nHookIndex] =
          SetWindowsHookEx(HookCodes[nHookIndex], (HOOKPROC) lpfnHookProcs[nHookIndex], hInstance,NULL);
	HookStates[nHookIndex] = TRUE;
     }
     else {
	UnhookWindowsHookEx(hhookHooks[nHookIndex]);
	HookStates[nHookIndex] = FALSE;
	InvalidateRect(hwndMain, NULL, TRUE);
	UpdateWindow(hwndMain);
     }
}

//---------------------------------------------------------------------------
// CallWndProcFunc
//
// Filter function for the WH_CALLWNDPROC
//
//---------------------------------------------------------------------------
void FAR PASCAL CallWndProcFunc (int nCode, WORD wParam, DWORD lParam )
{
   HDC	       hDC;
   // structure used for CallWpnProc
   struct tempStruct
     {
       LONG	   lParam;
       WORD	   wParam;
       WORD	   message;
       HWND	   hwnd;
     } FAR * lptsParamStruct;


   if ( nCode >= 0 ) {

      lptsParamStruct = (struct tempStruct FAR *) lParam;

      if ( wParam )
	 _fstrcpy(szType,"From Current Task");
      else
	 _fstrcpy(szType," ");

      wsprintf((LPSTR)szFilterLine[CALLWNDPROCINDEX],
	       "CALLWNDPROC\tWnd:%d\t%s %s                    ",
	       lptsParamStruct->hwnd,
	       szMessageString(lptsParamStruct->message), (LPSTR)szType);

      hDC = GetDC(hwndMain);
      TabbedTextOut(hDC, 1, nLineHeight * CALLWNDPROCINDEX,
	    (LPSTR)szFilterLine[CALLWNDPROCINDEX],
	    strlen(szFilterLine[CALLWNDPROCINDEX]), 0, NULL, 1);
      ReleaseDC(hwndMain, hDC);

   }

   //
   // We looked at the message ... sort of processed it but since we are
   // looking we will pass all messages on to CallNextHookEx.
   //
   CallNextHookEx(hhookHooks[CALLWNDPROCINDEX], nCode, (WORD)wParam,(WORD) lParam);
   return;
}

//---------------------------------------------------------------------------
// CbtFunc
//
// Filter function for the WH_CBT
//
//---------------------------------------------------------------------------
int FAR PASCAL CbtFunc (int nCode, WORD wParam, DWORD lParam )
{
   HDC	       hDC;
   CBTACTIVATESTRUCT FAR *Active;
   LPMOUSEHOOKSTRUCT MouseHookParam;
   LPCBT_CREATEWND   CreateWndParam;
   LPRECT	     Rect;

   if ( nCode >= 0 ) {
      switch ( nCode ) {
	  case HCBT_ACTIVATE:
	     Active = (CBTACTIVATESTRUCT FAR *) lParam;
	     wsprintf((LPSTR)szFilterLine[CBTINDEX],
		  "CBT\t\tGoing to Activate Wnd:%d Wnd:%d is active. Mouse Flag:%d                      ",
		  wParam,Active->hWndActive,Active->fMouse);

	     break;

	  case HCBT_CLICKSKIPPED:
	     MouseHookParam = (LPMOUSEHOOKSTRUCT) lParam;
	     wsprintf((LPSTR)szFilterLine[CBTINDEX],
		  "CBT\t\tMouse Message Removed from Queue. Point:%d %d %s                              ",
		  MouseHookParam->pt.x,MouseHookParam->pt.y,
		  szMessageString(wParam));

	     break;

	  case HCBT_CREATEWND:
	     CreateWndParam = (LPCBT_CREATEWND) lParam;
	     wsprintf((LPSTR)szFilterLine[CBTINDEX],
		  "CBT\t\tGoing to Create Wnd:%d %s                                                     ",
		  wParam,CreateWndParam->lpcs->lpszName);

	     break;

	  case HCBT_DESTROYWND:
	     wsprintf((LPSTR)szFilterLine[CBTINDEX],
		  "CBT\t\tGoing to Destroy Wnd:%d                                                       ",
		  wParam);

	     break;

	  case HCBT_KEYSKIPPED:
	     wsprintf((LPSTR)szFilterLine[CBTINDEX],
		  "CBT\t\tKeyboard Message Removed from Queue. Key:%d                                   ",
		  wParam);

	     break;

	  case HCBT_MINMAX:
	     switch ( LOWORD(lParam) ) {
		 case SW_HIDE:
		    _fstrcpy(szType, "SW_HIDE");
		    break;

		 case SW_NORMAL:
		    _fstrcpy(szType, "SW_NORMAL");
		    break;

		 case SW_SHOWMINIMIZED:
		    _fstrcpy(szType, "SW_SHOWMINIMIZED");
		    break;

		 case SW_MAXIMIZE:
		    _fstrcpy(szType, "SW_MAXIMIZE");
		    break;

		 case SW_SHOWNOACTIVATE:
		    _fstrcpy(szType, "SW_SHOWNOACTIVATE");
		    break;

		 case SW_SHOW:
		    _fstrcpy(szType, "SW_SHOW");
		    break;

		 case SW_MINIMIZE:
		    _fstrcpy(szType, "SW_MINIMIZE");
		    break;

		 case SW_SHOWMINNOACTIVE:
		    _fstrcpy(szType, "SW_SHOWMINNOACTIVE");
		    break;

		 case SW_SHOWNA:
		    _fstrcpy(szType, "SW_SHOWNA");
		    break;

		 case SW_RESTORE:
		    _fstrcpy(szType, "SW_RESTORE");
		    break;

		 default:
		    _fstrcpy(szType,"Unknown Message");

	     }
	     wsprintf((LPSTR)szFilterLine[CBTINDEX],
		  "CBT\t\tGoing to MIN/MAX Wnd:%d %s                                                    ",
		  wParam,(LPSTR)szType);

	     break;

	  case HCBT_MOVESIZE:
	     Rect = (LPRECT) lParam;
	     wsprintf((LPSTR)szFilterLine[CBTINDEX],
		  "CBT\t\tGoing to MOVE/SIZE Wnd:%d to %d %d %d %d                                      ",
		  wParam,Rect->left,Rect->top,Rect->right,Rect->bottom);

	     break;

	  case HCBT_QS:
	     wsprintf((LPSTR)szFilterLine[CBTINDEX],
		  "CBT\t\tWM_QUEUESYNC Message                                                          ");

	     break;

	  case HCBT_SETFOCUS:
	     wsprintf((LPSTR)szFilterLine[CBTINDEX],
		  "CBT\t\tGiving Focus to Wnd:%d Taking Focus From Wnd:%d                               ",
		  wParam,LOWORD(lParam));

	     break;

	  case HCBT_SYSCOMMAND:
	     switch ( wParam ) {
		  case SC_SIZE:
		     _fstrcpy(szType,"SC_SIZE");
		     break;

		  case SC_MOVE:
		     _fstrcpy(szType,"SC_MOVE");
		     break;

		  case SC_MINIMIZE:
		     _fstrcpy(szType,"SC_MINIMIZE");
		     break;

		  case SC_MAXIMIZE:
		     _fstrcpy(szType,"SC_MAXIMIZE");
		     break;

		  case SC_NEXTWINDOW:
		     _fstrcpy(szType,"SC_NEXTWINDOW");
		     break;

		  case SC_PREVWINDOW:
		     _fstrcpy(szType,"SC_PREVWINDOW");
		     break;

		  case SC_CLOSE:
		     _fstrcpy(szType,"SC_CLOSE");
		     break;

		  case SC_VSCROLL:
		     _fstrcpy(szType,"SC_VSCROLL");
		     break;

		  case SC_HSCROLL:
		     _fstrcpy(szType,"SC_HSCROLL");
		     break;

		  case SC_MOUSEMENU:
		     _fstrcpy(szType,"SC_MOUSEMENU");
		     break;

		  case SC_KEYMENU:
		     _fstrcpy(szType,"SC_KEYMENU");
		     break;

		  case SC_ARRANGE:
		     _fstrcpy(szType,"SC_ARRANGE");
		     break;

		  case SC_RESTORE:
		     _fstrcpy(szType,"SC_RESTORE");
		     break;

		  case SC_TASKLIST:
		     _fstrcpy(szType,"SC_TASKLIST");
		     break;

		  case SC_SCREENSAVE:
		     _fstrcpy(szType,"SC_SCREENSAVE");
		     break;

		  case SC_HOTKEY:
		     _fstrcpy(szType,"SC_HOTKEY");
		     break;

		 default:
		    _fstrcpy(szType,"Unknown Message");

	     }
	     wsprintf((LPSTR)szFilterLine[CBTINDEX],
		  "CBT\t\tAbout to Perform System Command: %s                                           ",
		  (LPSTR)szType);

	     break;

      }

      hDC = GetDC(hwndMain);
      TabbedTextOut(hDC, 1, nLineHeight * CBTINDEX,
	    (LPSTR)szFilterLine[CBTINDEX],
	    strlen(szFilterLine[CBTINDEX]), 0, NULL, 1);
      ReleaseDC(hwndMain, hDC);
   }

   //
   // We looked at the message ... sort of processed it but since we are
   // looking we will pass all messages on to CallNextHookEx.
   //
   return( (int)CallNextHookEx(hhookHooks[CBTINDEX], nCode,(WORD)wParam,(WORD) lParam));
}

//---------------------------------------------------------------------------
// GetMessageFunc
//
// Filter function for the WH_GETMESSAGE
//
//---------------------------------------------------------------------------
void FAR PASCAL GetMessageFunc (int nCode, WORD wParam, DWORD lParam )
{
   MSG FAR *lpMsg;
   HDC	   hDC;

   if ( nCode >= 0 ) {
      lpMsg = (MSG FAR *) lParam;
      wsprintf((LPSTR)szFilterLine[GETMESSAGEINDEX],
	       "GETMESSAGE\tWnd:%d Time:%ld  Point:%d %d %s                                  ",
	       lpMsg->hwnd, lpMsg->time,
	       lpMsg->pt.x, lpMsg->pt.y, szMessageString(lpMsg->message));


      hDC = GetDC(hwndMain);
      TabbedTextOut(hDC, 1, nLineHeight * GETMESSAGEINDEX,
	    (LPSTR)szFilterLine[GETMESSAGEINDEX],
	    strlen(szFilterLine[GETMESSAGEINDEX]), 0, NULL, 1);
      ReleaseDC(hwndMain, hDC);
   }

   //
   // We looked at the message ... sort of processed it but since we are
   // looking we will pass all messages on to CallNextHookEx.
   //
   CallNextHookEx(hhookHooks[GETMESSAGEINDEX], nCode, wParam, lParam);
   return;
}

//---------------------------------------------------------------------------
// JournalPlaybackFunc
//
// Filter function for the WH_JOURNALPLAYBACK
//
//---------------------------------------------------------------------------
DWORD FAR PASCAL JournalPlaybackFunc (int nCode, WORD wParam, DWORD lParam )
{
   HDC		 hDC;
   static int	 nRepeatRequests;
   HANDLE	 hEvent;
   LPEVENTMSG lpEvent;
   static DWORD  dwTimeAdjust;
   long 	 lReturnValue;
   static DWORD  dwLastEventTime;
   HMENU	 hMenu;

   if ( nCode >= 0 ) {
      // No Playback if we haven't recorded an Event
      //
      // No Playback While recording.
      // This is not a limitation of the hooks.
      // This is only because of the simple event storage used in this example
      //
      // We should never get here since the enable / disable menu stuff should
      // make getting here impossible
      //
      if ( lpEventChain == NULL || HookStates[JOURNALRECORDINDEX]) {
	  InstallFilter(JOURNALPLAYBACKINDEX, FALSE);
	  hMenu = GetMenu(hwndMain);
	  CheckMenuItem(hMenu, IDM_JOURNALPLAYBACK, MF_UNCHECKED | MF_BYCOMMAND);
	  EnableMenuItem(hMenu, IDM_JOURNALPLAYBACK, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
	  wsprintf((LPSTR)szFilterLine[JOURNALPLAYBACKINDEX],
		  "WH_JOURNALPLAYBACK -- No recorded Events to Playback  or JournalRecord in Progress                  ");
	  hDC = GetDC(hwndMain);
	  TabbedTextOut(hDC, 1, nLineHeight * JOURNALPLAYBACKINDEX,
		(LPSTR)szFilterLine[JOURNALPLAYBACKINDEX],
		strlen(szFilterLine[JOURNALPLAYBACKINDEX]), 0, NULL, 1);
	  ReleaseDC(hwndMain, hDC);
          return( (int)CallNextHookEx(hhookHooks[JOURNALPLAYBACKINDEX], nCode, wParam, lParam ));
      }

      if ( lpPlayEvent == NULL ) {
	 lpPlayEvent = lpEventChain;
	 lpLastEvent = NULL;	   // For the next time we start the recorder
	 dwTimeAdjust = GetTickCount() - dwStartRecordTime;
	 dwLastEventTime = (DWORD) GetTickCount();
	 nRepeatRequests = 1;
      }

      if (nCode == HC_SKIP ) {
	  nRepeatRequests = 1;

	  if ( lpPlayEvent->lpNextEvent == NULL ) {
	      wsprintf((LPSTR)szFilterLine[JOURNALPLAYBACKINDEX],
		      "WH_JOURNALPLAYBACK -- Done Recorded Events");
	      hDC = GetDC(hwndMain);
	      TabbedTextOut(hDC, 1, nLineHeight * JOURNALPLAYBACKINDEX,
		    (LPSTR)szFilterLine[JOURNALPLAYBACKINDEX],
		    strlen(szFilterLine[JOURNALPLAYBACKINDEX]), 0, NULL, 1);
	      ReleaseDC(hwndMain, hDC);

	      hEvent = (HANDLE)LOWORD(GlobalHandle((HIWORD(lpEventChain))));
	      GlobalUnlock(hEvent);
	      GlobalFree(hEvent);
	      lpEventChain = lpPlayEvent = NULL ;

	      InstallFilter(JOURNALPLAYBACKINDEX, FALSE);
	      hMenu = GetMenu(hwndMain);
	      CheckMenuItem(hMenu, IDM_JOURNALPLAYBACK, MF_UNCHECKED | MF_BYCOMMAND);
	      EnableMenuItem(hMenu, IDM_JOURNALPLAYBACK, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
	  }
	  else {
	      dwLastEventTime = lpPlayEvent->Event.time;
	      lpPlayEvent = lpPlayEvent->lpNextEvent;

	      hEvent = (HANDLE)LOWORD(GlobalHandle((HIWORD(lpEventChain))));
	      GlobalUnlock(hEvent);
	      GlobalFree(hEvent);
	      lpEventChain = lpPlayEvent;
	  }
      }
      else if ( nCode == HC_GETNEXT) {
	  lpEvent = (LPEVENTMSG) lParam;
	  lpEvent->message = lpPlayEvent->Event.message;
	  lpEvent->paramL  = lpPlayEvent->Event.paramL;
	  lpEvent->paramH  = lpPlayEvent->Event.paramH;
	  lpEvent->time    = lpPlayEvent->Event.time + dwTimeAdjust;

	  wsprintf((LPSTR)szFilterLine[JOURNALPLAYBACKINDEX],
		   "WH_JOURNALPLAYBACK -- Playing Event %d times                    ",
		   nRepeatRequests++);
	  hDC = GetDC(hwndMain);
	  TabbedTextOut(hDC, 1, nLineHeight * JOURNALPLAYBACKINDEX,
		(LPSTR)szFilterLine[JOURNALPLAYBACKINDEX],
		strlen(szFilterLine[JOURNALPLAYBACKINDEX]), 0, NULL, 1);
	  ReleaseDC(hwndMain, hDC);

	  lReturnValue = lpEvent->time - GetTickCount();
	  //
	  // No Long ( negative ) waits
	  //
	  if ( lReturnValue < 0L ) {
	     lReturnValue  = 0L;
	     lpEvent->time = GetTickCount();
	  }
	  return ( (DWORD) lReturnValue );
      }

      hDC = GetDC(hwndMain);
      TabbedTextOut(hDC, 1, nLineHeight * JOURNALPLAYBACKINDEX,
	    (LPSTR)szFilterLine[JOURNALPLAYBACKINDEX],
	    strlen(szFilterLine[JOURNALPLAYBACKINDEX]), 0, NULL, 1);
      ReleaseDC(hwndMain, hDC);
   }

   return( (int)CallNextHookEx(hhookHooks[JOURNALPLAYBACKINDEX], nCode, wParam, lParam));
}

//---------------------------------------------------------------------------
// JournalRecordFunc
//
// Filter function for the WH_JOURNALRECORD
//
//---------------------------------------------------------------------------
void FAR PASCAL JournalRecordFunc (int nCode, WORD wParam, DWORD lParam )
{
   HDC	   hDC;
   HANDLE	  hEvent;
   EventNode FAR *lpEventNode;
   LPEVENTMSG  lpEvent;
   HMENU	  hMenu;

   if ( nCode >= 0 ) {

      lpEvent = (LPEVENTMSG) lParam;
      //
      // Skip recording while playing back
      // This is not a limitation of the hooks.
      // This is only because of the simple event storage used in this example
      //
      if ( HookStates[JOURNALPLAYBACKINDEX] ) {
	 wsprintf((LPSTR)szFilterLine[JOURNALRECORDINDEX],
	       "WH_JOURNALRECORD\tSkipping Recording during Playback                        ");
	 hDC = GetDC(hwndMain);
	 TabbedTextOut(hDC, 1, nLineHeight * JOURNALRECORDINDEX,
	       (LPSTR)szFilterLine[JOURNALRECORDINDEX],
	       strlen(szFilterLine[JOURNALRECORDINDEX]), 0, NULL, 1);
	 ReleaseDC(hwndMain, hDC);
	 return;
      }

      //
      // Stop recording ?
      //
      if ( lpEvent->message == WM_KEYDOWN &&
	   LOBYTE(lpEvent->paramL)  == VK_F2 ) {
	 wsprintf((LPSTR)szFilterLine[JOURNALRECORDINDEX],
	       "WH_JOURNALRECORD\tRecording Stopped with F2                                                                 ");
	 InstallFilter(JOURNALRECORDINDEX, FALSE);
	 hMenu = GetMenu(hwndMain);
	 CheckMenuItem(hMenu, IDM_JOURNALRECORD, MF_UNCHECKED | MF_BYCOMMAND);
	 EnableMenuItem(hMenu, IDM_JOURNALPLAYBACK, MF_ENABLED | MF_BYCOMMAND);
	 return ;
      }

      if ((hEvent=
	  GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE,sizeof(EventNode))) == NULL ) {
	 wsprintf((LPSTR)szFilterLine[JOURNALRECORDINDEX],
	       "WH_JOURNALRECORD\tNo Memory Available");
	 InstallFilter(JOURNALRECORDINDEX, FALSE);
	 hMenu = GetMenu(hwndMain);
	 CheckMenuItem(hMenu, IDM_JOURNALRECORD, MF_UNCHECKED | MF_BYCOMMAND);
	 EnableMenuItem(hMenu, IDM_JOURNALPLAYBACK, MF_ENABLED | MF_BYCOMMAND);
	 return;
      }
      if ( (lpEventNode = (EventNode FAR *)GlobalLock(hEvent)) == NULL ) {
	 GlobalFree(hEvent);
	 wsprintf((LPSTR)szFilterLine[JOURNALRECORDINDEX],
	       "WH_JOURNALRECORD\tNo Memory Available");
	 InstallFilter(JOURNALRECORDINDEX, FALSE);
	 hMenu = GetMenu(hwndMain);
	 CheckMenuItem(hMenu, IDM_JOURNALRECORD, MF_UNCHECKED | MF_BYCOMMAND);
	 EnableMenuItem(hMenu, IDM_JOURNALPLAYBACK, MF_ENABLED | MF_BYCOMMAND);
      }


      if ( lpLastEvent == NULL ) {
	 dwStartRecordTime = (DWORD) GetTickCount();
	 lpEventChain = lpEventNode;
      }
      else {
	 lpLastEvent->lpNextEvent = lpEventNode;
      }

      lpLastEvent = lpEventNode;
      lpLastEvent->lpNextEvent = NULL;

      lpLastEvent->Event.message = lpEvent->message;
      lpLastEvent->Event.paramL  = lpEvent->paramL;
      lpLastEvent->Event.paramH  = lpEvent->paramH;
      lpLastEvent->Event.time	 = lpEvent->time;


      wsprintf((LPSTR)szFilterLine[JOURNALRECORDINDEX],
	    "WH_JOURNALRECORD\tRecording\tTime:%ld\tPRESS F2 To Stop Recording\t%s                        ",
	    lpEvent->time,szMessageString(lpEvent->message));
      hDC = GetDC(hwndMain);
      TabbedTextOut(hDC, 1, nLineHeight * JOURNALRECORDINDEX,
	    (LPSTR)szFilterLine[JOURNALRECORDINDEX],
	    strlen(szFilterLine[JOURNALRECORDINDEX]), 0, NULL, 1);
      ReleaseDC(hwndMain, hDC);
      return;
   }

   CallNextHookEx(hhookHooks[JOURNALRECORDINDEX], nCode, wParam, lParam);
   return;
}

//---------------------------------------------------------------------------
// KeyboardFunc
//
// Filter function for the WH_KEYBOARD
//
//---------------------------------------------------------------------------
int FAR PASCAL KeyboardFunc (int nCode, WORD wParam, DWORD lParam )
{
   HDC	       hDC;

   if ( nCode >= 0 ) {

      if ( nCode == HC_NOREMOVE )
	 _fstrcpy(szType, "NOT Removed from Queue");
      else
	 _fstrcpy(szType, "REMOVED from Queue                                             ");

      wsprintf((LPSTR)szFilterLine[KEYBOARDINDEX],
	       "KEYBOARD\tKey:%d\t%s",wParam,(LPSTR)szType);

      hDC = GetDC(hwndMain);
      TabbedTextOut(hDC, 1, nLineHeight * KEYBOARDINDEX,
	    (LPSTR)szFilterLine[KEYBOARDINDEX],
	    strlen(szFilterLine[KEYBOARDINDEX]), 0, NULL, 1);
      ReleaseDC(hwndMain, hDC);
   }

   //
   // We looked at the message ... sort of processed it but since we are
   // looking we will pass all messages on to CallNextHookEx.
   //
   return( (int)CallNextHookEx(hhookHooks[KEYBOARDINDEX], nCode, wParam, lParam));
}

//---------------------------------------------------------------------------
// MouseFunc
//
// Filter function for the WH_MOUSE
//
//---------------------------------------------------------------------------
int FAR PASCAL MouseFunc (int nCode, WORD wParam, DWORD lParam )
{
   HDC	       hDC;
   LPMOUSEHOOKSTRUCT MouseHookParam;

   if ( nCode >= 0 ) {
      if ( nCode == HC_NOREMOVE )
	 _fstrcpy(szType, "NOT Removed from Queue");
      else
	 _fstrcpy(szType, "REMOVED from Queue                                             ");

      MouseHookParam = (MOUSEHOOKSTRUCT FAR *) lParam;

      wsprintf((LPSTR)szFilterLine[MOUSEINDEX],
	      "MOUSE\t\tWnd:%d Point:%d %d\t%s %s",MouseHookParam->hwnd,
	      MouseHookParam->pt.x,MouseHookParam->pt.y,
	      szMessageString(wParam),(LPSTR)szType);

      hDC = GetDC(hwndMain);
      TabbedTextOut(hDC, 1, nLineHeight * MOUSEINDEX,
	    (LPSTR)szFilterLine[MOUSEINDEX], strlen(szFilterLine[MOUSEINDEX]),
	    0, NULL, 1 );
      ReleaseDC(hwndMain, hDC);

   }

   //
   // We looked at the message ... sort of processed it but since we are
   // looking we will pass all messages on to CallNextHookEx.
   //
   return( (int)CallNextHookEx(hhookHooks[MOUSEINDEX], nCode, wParam, lParam));
}

//---------------------------------------------------------------------------
// SysMsgFilterFunc
//
// Filter function for the WH_SYSMSGFILTER
//
//---------------------------------------------------------------------------
int FAR PASCAL SysMsgFilterFunc (int nCode, WORD wParam, DWORD lParam )
{
   MSG FAR *lpMsg;
   HDC	       hDC;

   if ( nCode >= 0 ) {
      if ( nCode == MSGF_DIALOGBOX )
	 _fstrcpy(szType,"Dialog");
      else
	 _fstrcpy(szType,"Menu");

      lpMsg = (MSG FAR *) lParam;
      wsprintf((LPSTR)szFilterLine[SYSMSGFILTERINDEX],
	       "SYSMSGFILTER\t%s\tWnd:%d Time:%ld  Point:%d %d %s                                ",
	       (LPSTR)szType, lpMsg->hwnd, lpMsg->time,
	       lpMsg->pt.x, lpMsg->pt.y, szMessageString(lpMsg->message));

      hDC = GetDC(hwndMain);
      TabbedTextOut(hDC, 1, nLineHeight * SYSMSGFILTERINDEX,
	    (LPSTR)szFilterLine[SYSMSGFILTERINDEX],
	    strlen(szFilterLine[SYSMSGFILTERINDEX]), 0, NULL, 1 );
      ReleaseDC(hwndMain, hDC);
   }

   //
   // We looked at the message ... sort of processed it but since we are
   // looking we will pass all messages on to CallNextHookEx.
   //
   return( (int)CallNextHookEx(hhookHooks[SYSMSGFILTERINDEX], nCode, wParam, lParam));
}


//---------------------------------------------------------------------------
// MessageString
//
// Function to load string from the STRINGTABLE
//
//---------------------------------------------------------------------------
char FAR * szMessageString(WORD wID)
{
   static char szBuffer[256];

   if ( LoadString(hInstance, wID, szBuffer, 255) == 0)
      _fstrcpy(szBuffer,"Unknown Message");

   return (szBuffer);

}
