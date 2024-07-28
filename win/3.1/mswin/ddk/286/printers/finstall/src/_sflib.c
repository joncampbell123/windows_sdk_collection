/**[f******************************************************************
* $sflib.c -
*
* Copyright (C) 1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
*   3 feb 92 rk Changed $ include files to _ include files
**f]*****************************************************************/
/*
 * $Header: 
 */

/*
 * $Log:
 */
  
//#define DEBUG
  
#include "nocrap.h"
#undef NOOPENFILE
#undef NOMSG
#undef NOCTLMGR
#undef NOWINMESSAGES
#undef NOSCROLL
#undef NOMEMMGR
#undef NOMB
  
#include "_windows.h"
#include "debug.h"
#include "_kludge.h"
#include "_cgifwin.h"
#include "_tmu.h"
#include "strings.h"
#include "_sflib.h"
#include "_utils.h"
#include "_sfidlg.h"
  
/****************************************************************************\
*  DEBUG definitions
\****************************************************************************/
  
#ifdef DEBUG
    #define DBGerr(msg)        /*DBMSG(msg)*/
    #define DBGtrace(msg)      /*DBMSG(msg)*/
    #define DBGSelLib(msg)     /*DBMSG(msg)*/
#else
    #define DBGerr(msg)        /*null*/
    #define DBGtrace(msg)      /*null*/
    #define DBGSelLib(msg)     /*null*/
#endif
  
  
/****************************************************************************\
*  LOACL Definitions
\****************************************************************************/
  
#define FILES_MEMORY 1000
#define EXTEND_SIZE  50
  
/****************************************************************************\
*
* External Procedures
*
\****************************************************************************/
  
LPSTR FAR PASCAL lmemcpy(LPSTR, LPSTR, WORD);
extern hLibInst;
extern int gPrintClass;
extern int goption;
  
  
/****************************************************************************\
*
*  name: AddDirEntry
*
*  description:
*
*  return value:
*
*  globals:
*
*  called by:
*
*  history:
*
\****************************************************************************/
  
BOOL FAR PASCAL AddDirEntry(lpFontLibrary, lpFileName, lpFontDescription, lpPath, lpSizes, Type, Listbox, lpTypeInfo, lpScrnFntInfo)
LPDIRECTORY lpFontLibrary;
LPSTR lpFileName;
LPSTR lpFontDescription;
LPSTR lpPath;
LPSTR lpSizes;
int Type;
int Listbox;
LPTYPEINFO lpTypeInfo;
LPSCRNFNTINFO lpScrnFntInfo;
{
  
    LPSTR lpVF = NULL;
    LPSTR lpLastEntry = NULL;
    LPSFI_FONTLIBENTRY lpDirLib;
    int LibEntryLen;
    int iFontDesLen = 0;
    int iFileNameLen = 0;
    HANDLE hTmp;
  
    DBGtrace(("AddDirEntry\n"));
  
    if (lpFontDescription != NULL) iFontDesLen = lstrlen(lpFontDescription)+1;
    if (lpFileName != NULL) iFileNameLen = lstrlen(lpFileName)+1;
  
    if (lpFontLibrary->hFiles == NULL)
    {
        if (((lpFontLibrary->hFiles=GlobalAlloc(GMEM_MOVEABLE,(DWORD) FILES_MEMORY)) != NULL))
        {
            lpFontLibrary->length = FILES_MEMORY;
            lpFontLibrary->NextEntry = 0;
            lpFontLibrary->NumFiles = 0;
        }
        else
        {
            DBGerr(("Unable to allocate initial block of memory\n"));
            return (FALSE);
        }
    }
  
    LibEntryLen = sizeof(SFI_FONTLIBENTRY) + iFontDesLen + iFileNameLen;
    if (lpFontLibrary->NextEntry+LibEntryLen > lpFontLibrary->length)
    {
  
        while ((GlobalFlags(lpFontLibrary->hFiles) & GMEM_LOCKCOUNT) > 0)
            GlobalUnlock(lpFontLibrary->hFiles);
  
        if((hTmp = GlobalReAlloc(lpFontLibrary->hFiles,
            (DWORD)lpFontLibrary->NextEntry+LibEntryLen,
            GMEM_MOVEABLE))==NULL)
        {
            DBGerr(("Unable to allocate extra memory\n"));
            return(FALSE);
        }
        else
        {
            lpFontLibrary->hFiles = hTmp;
            lpFontLibrary->length = lpFontLibrary->NextEntry+LibEntryLen;
        }
    }
  
    if ((lpVF = (LPSTR) GlobalLock(lpFontLibrary->hFiles)) == NULL)
    {
        DBGerr(("AddDirEntry: Unable to lock memory\n"));
        GlobalFree(lpFontLibrary->hFiles);
        return FALSE;
    }
  
    lpDirLib = (LPSFI_FONTLIBENTRY)lpVF;
    lpLastEntry = lpVF + lpFontLibrary->NextEntry;
    while(lpVF < lpLastEntry)
    {
        /*DBGLibEntry(lpDirLib, (LPSTR)"AddDirEntry");*/
        if (lstrcmpi(lpFontDescription, lpVF+lpDirLib->OffsetName) > 0)
        {
            lpVF += lpDirLib->Length;
            lpDirLib = (LPSFI_FONTLIBENTRY)lpVF;
        }
        else
            break;
    }
  
    if (lpVF < lpLastEntry)
    {
        lmemcpy(lpVF+LibEntryLen, lpVF, lpLastEntry-lpVF);
        /*DBGLibEntry(lpVF+LibEntryLen, (LPSTR)"This block was moved");*/
    }
    lpDirLib->Type = Type;
    lpDirLib->Length = LibEntryLen;
//  lpDirLib->next = NULL;
//  lpDirLib->prev = NULL;
    lpDirLib->usage = 1;
//  lpDirLib->Listbox = Listbox;
    lpDirLib->ListboxEntry = -1;
    lpDirLib->Selected = FALSE;
    lpDirLib->OffsetName = (WORD) 0;
    lpDirLib->OffsetPath = (WORD) 0;
//  if (lpSizes != (LPSTR) NULL)
//     lstrcpy((LPSTR) lpDirLib->ScreenSizes, lpSizes);
//  else
//     lpDirLib->ScreenSizes[0] = '\0' ;
  
    if (lpTypeInfo != (LPTYPEINFO) NULL)
        lpDirLib->TypeInfo = *lpTypeInfo;
  
    lpVF += sizeof(SFI_FONTLIBENTRY);
    if (iFontDesLen)
    {
        lstrcpy(lpVF, lpFontDescription);
        lpDirLib->OffsetName = sizeof(SFI_FONTLIBENTRY);
        lpVF += iFontDesLen;
    }
    if (iFileNameLen)
    {
        lstrcpy(lpVF, lpFileName);
        lpDirLib->OffsetPath = sizeof(SFI_FONTLIBENTRY)+iFontDesLen;
        lpVF += iFileNameLen;
    }
    ++lpFontLibrary->NumFiles;
    lpFontLibrary->NextEntry += LibEntryLen;
  
    GlobalUnlock(lpFontLibrary->hFiles);
  
    return TRUE;
}
  
#ifdef DEBUG
/****************************************************************************\
*
*  name: DBGLibEntry
*
*  description:
*
*  return value:
*
*  globals:
*
*  called by:
*
*  history:
*
\****************************************************************************/
  
VOID FAR PASCAL DBGLibEntry(lpLib, lpCalledFrom)
LPSFI_FONTLIBENTRY lpLib;
LPSTR lpCalledFrom;
{
    LPSTR lpData;
  
  
    DBMSG(("|* [%ls] --------------------------- *|\n", lpCalledFrom ));
    DBMSG(("| lpLib               = [%lp]\n",lpLib       ));
    DBMSG(("| lpLib->Type         = %d\n",   lpLib->Type     ));
    DBMSG(("| lpLib->Length       = %d\n",   lpLib->Length   ));
    DBMSG(("| lpLib->next         = %d\n",   lpLib->next     ));
    DBMSG(("| lpLib->prev         = %d\n",   lpLib->prev         ));
    DBMSG(("| lpLib->Listbox      = %d\n",   lpLib->Listbox      ));
    DBMSG(("| lpLib->ListboxEntry = %d\n",   lpLib->ListboxEntry ));
    DBMSG(("| lpLib->Selected     = %d\n",   lpLib->Selected     ));
    DBMSG(("| lpLib->OffsetName   = %d\n",   lpLib->OffsetName   ));
    DBMSG(("| lpLib->OffsetPath   = %d\n",   lpLib->OffsetPath   ));
  
    if(lpLib->OffsetName)
        DBMSG(("| [%lp] = <%ls>\n",
        (LPSTR)lpLib+lpLib->OffsetName,
        (LPSTR)lpLib+lpLib->OffsetName));
  
    if(lpLib->OffsetPath)
        DBMSG(("| [%lp] = <%ls>\n",
        (LPSTR)lpLib+lpLib->OffsetPath,
        (LPSTR)lpLib+lpLib->OffsetPath));
#ifdef NODEF
    if(lpLib->ScreenSizes[0])
    {
        lpData = lpLib->ScreenSizes;
        DBMSG(("| [%lp] = <", (LPSTR) lpLib->ScreenSizes));
        while (*lpData)
        {
            DBMSG(("%d ",(int)*lpData));
            ++lpData;
        }
        DBMSG((">\n"));
    }
#endif
    DBMSG(("|*--------------------------- *|\n"));
}
#endif
  
  
