/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

//---------------------------------------------------------------------------
//
// File: err.c
//
//  This files contains all error handling routines.
//
//---------------------------------------------------------------------------


#include "suiprv.h"     // common headers

#ifdef DEBUG

#pragma data_seg(DATASEG_READONLY)

char const FAR c_szNewline[] = "\r\n";
char const FAR c_szTrace[] = "t SERIALUI  ";
char const FAR c_szDbg[] = "SERIALUI  ";
char const FAR c_szAssertFailed[] = "SERIALUI  Assertion failed in %s on line %d\r\n";

#pragma data_seg()


void PUBLIC MyAssertFailed(
    LPCSTR pszFile, 
    int line)
    {
    LPCSTR psz;
    char ach[256];

    // Strip off path info from filename string, if present.
    //
    for (psz = pszFile + lstrlen(pszFile); psz != pszFile; psz=AnsiPrev(pszFile, psz))
        {
#ifdef  DBCS
        if ((AnsiPrev(pszFile, psz) != (psz-2)) && *(psz - 1) == '\\')
#else
        if (*(psz - 1) == '\\')
#endif
            break;
        }
    wsprintf(ach, c_szAssertFailed, psz, line);
    OutputDebugString(ach);
    
    DebugBreak();
    }


void CPUBLIC MyAssertMsg(
    BOOL f, 
    LPCSTR pszMsg, ...)
    {
    char ach[MAX_PATH+40];    // Largest path plus extra

    if (!f)
        {
        lstrcpy(ach, c_szTrace);
        wvsprintf(&ach[sizeof(c_szTrace)-1], pszMsg, ((LPCSTR FAR*)&pszMsg + 1));
        OutputDebugString(ach);
        OutputDebugString(c_szNewline);
        }
    }

#endif


/*----------------------------------------------------------
Purpose: Invoke a user error message box.  Default values are
         obtained from vappinfo struct.
Returns: value of MessageBox()
Cond:    --
*/
int PUBLIC MsgBoxSz(
    HWND hwndParent,    // parent window (may be NULL)
    LPCSTR lpsz,        // message
    UINT idsCaption,    // resource ID for caption
    UINT nBoxType,      // message box type (ERR_ERROR, ERR_INFO, ERR_QUESTION)
    HANDLE hres)        // Resource instance handle (may be NULL)
    {
    int nRet = 0;
    
    if (hres == NULL)
        {
        hres = g_hinst;
        }
    
    ASSERT(hres != NULL);
    
    // Load in error description string
    //
    if (lpsz)
        {
        char szCap[MAXBUFLEN];

        UINT nType = MB_OK;
        
        // Load the caption string
        //
        SzFromIDS(idsCaption, szCap, sizeof(szCap));

        // Determine type of message box
        //
        nType |= (nBoxType == MSG_ERROR) ? MB_ICONEXCLAMATION : 0;
        nType |= (nBoxType == MSG_INFO) ? MB_ICONINFORMATION : 0;
        if (nBoxType == MSG_QUESTION)
            nType = MB_ICONQUESTION | MB_YESNO;
        
        nRet = MessageBox(hwndParent, lpsz, szCap, nType);
        }
    
    return nRet;
    }


/*----------------------------------------------------------
Purpose: Invoke a user error message box.  
Returns: value of MessageBox()
Cond:    --
*/
int PUBLIC MsgBoxIds(
    HWND hwndParent,    // parent window (may be NULL)
    UINT ids,           // message resource ID
    UINT idsCaption,    // resource ID for caption
    UINT nBoxType)      // message box type (ERR_ERROR, ERR_INFO, ERR_QUESTION)
    {
    char sz[MAXMSGLEN];
    int nRet = 0;
    HINSTANCE hinst;
    
    hinst = g_hinst;

    // Load in error description string
    //
    if (LoadString(hinst, ids, sz, sizeof(sz)) > 0)
        nRet = MsgBoxSz(hwndParent, sz, idsCaption, nBoxType, hinst);
    
    return nRet;
    }

