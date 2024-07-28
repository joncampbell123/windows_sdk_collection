/* 
 * utility.h 
 *
 * Created by Microsoft Corporation.
 * (c) Copyright Microsoft Corp. 1990 - 1992  All Rights Reserved
 */

//--- PROTO TYPES ---

//- FAR  
BOOL FAR          ObjectsBusy(void);
void FAR          WaitForAllObjects(void);
void FAR          WaitForObject(APPITEMPTR);
void FAR          ErrorMessage(WORD);
void FAR          Hourglass(BOOL);
BOOL FAR          DisplayBusyMessage (APPITEMPTR);
BOOL FAR          Dirty(int);
LPSTR FAR         CreateNewUniqueName(LPSTR);
BOOL FAR          ValidateName(LPSTR);
BOOL FAR          ProcessMessage(HWND, HANDLE);
void FAR          FreeAppItem(APPITEMPTR);
LONG FAR          SizeOfLinkData (LPSTR);
void FAR          ShowDoc(LHCLIENTDOC, int);
APPITEMPTR FAR    GetTopItem(void);
void FAR          SetTopItem(APPITEMPTR);
APPITEMPTR FAR    GetNextActiveItem(void);
APPITEMPTR FAR    GetNextItem(APPITEMPTR);
BOOL FAR          ReallocLinkData(APPITEMPTR,long);
BOOL FAR          AllocLinkData(APPITEMPTR,long);
void FAR          FreeLinkData(LPSTR);
void FAR          ShowNewWindow(APPITEMPTR);
PSTR FAR          UnqualifyPath(PSTR);
BOOL FAR PASCAL   fnTimerBlockProc(HWND, WORD, int, DWORD);
void FAR          ToggleBlockTimer(BOOL, APPITEMPTR);
