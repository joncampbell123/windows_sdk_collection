/*
 * dialog.h 
 *
 * Created by Microsoft Corporation.
 * (c) Copyright Microsoft Corp. 1990 - 1992  All Rights Reserved
 */

//--- INCLUDES ---

#include <commdlg.h>

//--- PROTOTYPES ---

//--- FAR 
BOOL FAR          FullyQualify(LPSTR, LPSTR);
BOOL FAR          OfnGetName(HWND, LPSTR, WORD);
LPSTR FAR         OfnGetNewLinkName(HWND, LPSTR);
void FAR          OfnInit(HANDLE);
BOOL FAR PASCAL   fnInsertNew(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL   fnProperties(HWND, unsigned, WORD, LONG);
void FAR          LinkProperties(void);
void FAR          AboutBox(void);
BOOL FAR PASCAL   fnAbout( HWND, unsigned, WORD, LONG);
void FAR          RetryMessage (APPITEMPTR,LONG);
BOOL FAR PASCAL   fnRetry(HWND, WORD, WORD, DWORD);
void FAR          InvalidLink(void);
BOOL FAR PASCAL   fnInvalidLink(HWND, unsigned, WORD, LONG);

//--- Local
static void       AddExtension(LPOPENFILENAME);
static void       Normalize(LPSTR);
static BOOL       InitLinkDlg (HWND, int *, HWND, APPITEMPTR **);
static void       UpdateLinkButtons(HWND, int, HWND, APPITEMPTR *);
static BOOL       ChangeLinks(HWND, int, HWND, APPITEMPTR *);
static void       CancelLinks(HWND, int, HWND, APPITEMPTR *);
static void       DisplayUpdate(int, HWND, APPITEMPTR *, BOOL);
static void       UndoObjects(void);
static void       DelUndoObjects(BOOL);
static void       ChangeUpdateOptions(HWND, int, HWND, APPITEMPTR *, OLEOPT_UPDATE);
static void       MakeListBoxString(LPSTR, LPSTR, OLEOPT_UPDATE);

//--- MACROS ---

#define END_PROP_DLG(hDlg,pLinks) { \
   HANDLE handle; \
   handle = LocalHandle((WORD)pLinks); \
   LocalUnlock(handle); \
   LocalFree(handle); \
   Hourglass(FALSE); \
   hwndProp = NULL; \
   EndDialog(hDlg, TRUE); \
}

#define CHANGE_LISTBOX_STRING(hwnd,i,pItem,lpLinkData) {\
   char pString[CBMESSAGEMAX*4];\
   MakeListBoxString(lpLinkData,pString,pItem->uoObject);\
   SendMessage(hwndList,LB_DELETESTRING, i , 0L);\
   SendMessage(hwndList,LB_INSERTSTRING, i , (long)((LPSTR)pString));\
   SendMessage(hwndList,LB_SETSEL, 1, (long)i);\
}

#define CHECK_IF_STATIC(pItem) {\
   if (pItem->otObject == OT_STATIC)\
      continue;\
}

#define BLOCK_BUSY(fTest) {\
   if (fTest)\
   {\
      fTest = FALSE;\
      return TRUE;\
   }\
   if (cOleWait)\
   {\
      fTest = TRUE;\
      RetryMessage(NULL,RD_CANCEL);\
      fTest = FALSE;\
      return TRUE;\
   }\
}




 
