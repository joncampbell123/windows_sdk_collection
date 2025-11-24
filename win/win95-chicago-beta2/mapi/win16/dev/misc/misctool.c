/*
 -  M I S C T O O L S . C
 -
 *  Copyright (C) 1994 Microsoft Corporation
 *  Purpose:
 *      Contains utilities to support the sample applications.
 */


#ifdef WIN32
#ifdef CHICAGO
#define _INC_OLE
#endif
#define INC_OLE2
#define INC_RPC
#endif

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <string.h>

#ifdef WIN16
#include <compobj.h>
#endif

#ifdef WIN32
#include <objbase.h>
#include <objerror.h>
#ifdef CHICAGO
#include <ole2.h>
#endif
#endif

#include <mapiwin.h>
#include <mapix.h>

// for EMS entryids in IsEntryID()
// #incldue <nonmapi.h>

#include <stdlib.h>
#include <strtbl.h>
#include <pvalloc.h>
#include <misctool.h>

#ifdef WIN16
#include <time.h>
#endif

#define MAX_LOG_BUFF    255

// size of character string assoicated with binary byte of data  0x44 is space, 0, x, 2 bytes data
#define BINARY_CHAR_SIZE     5  

typedef int EC;
/*
 -  EcStringFromFile
 -
 *  Purpose:
 *      Read a file into a string (Allocates needed memory)
 *
 *  Parameters:
 *      lpsz                - Pointer returned with read in string
 *      lpszFileName        - File to be read into string
 *      lpv                 - Void pointer to chain memory to (optional)
 *
 *  Returns:
 *      0 if successful
 *      non 0 if failure
 *
 */

EC EcStringFromFile(LPSTR *lppsz, LPSTR lpszFileName, LPVOID lpv)
{
    EC      ec = 0;

#ifdef WIN32

    HANDLE  hFile;
    DWORD   cHOFileSize;         //High Order 32 bits of File Size
    DWORD   cLOFileSize;         //Low Order 32 bits of File Size
    DWORD   cBytesRead;

    hFile = CreateFile(lpszFileName, GENERIC_READ, 0, NULL,OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
    {
        ec = 1;
        goto err;
    }

    cLOFileSize = GetFileSize(hFile,&cHOFileSize);
    if((cLOFileSize == 0xFFFFFFFF) && (GetLastError() != NO_ERROR))
    {
        ec = 2;
        goto err;
    }

    if(lpv)
        *lppsz = (LPSTR)PvAllocMore(cLOFileSize+1, lpv);
    else
        *lppsz = (LPSTR)PvAlloc(cLOFileSize+1);

    if(!ReadFile(hFile, *lppsz, cLOFileSize, &cBytesRead, NULL))
    {
        ec = 3;
        goto err;
    }

    (*lppsz)[cLOFileSize] = '\0';

err:
    CloseHandle(hFile);

#endif

    return ec;

}
/*
 -  EcBinaryFromFile
 -
 *  Purpose:
 *      Read a file into a string (Allocates needed memory)
 *
 *  Parameters:
 *      lpsz                - Pointer returned with read in string
 *      lpszFileName        - File to be read into string
 *      lpv                 - Void pointer to chain memory to (optional)
 *
 *  Returns:
 *      0 if successful
 *      non 0 if failure
 *
 */

EC EcBinaryFromFile(LPSBinary lpsbin, LPSTR lpszFileName, LPVOID lpv)
{


    EC      ec = 0;

#ifdef WIN32

    HANDLE  hFile;
    DWORD   cHOFileSize;         //High Order 32 bits of File Size
    DWORD   cLOFileSize;         //Low Order 32 bits of File Size
    DWORD   cBytesRead;

    hFile = CreateFile(lpszFileName, GENERIC_READ, 0, NULL,OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
    {
        ec = 1;
        goto err;
    }

    cLOFileSize = GetFileSize(hFile,&cHOFileSize);
    if((cLOFileSize == 0xFFFFFFFF) && (GetLastError() != NO_ERROR))
    {
        ec = 2;
        goto err;
    }

    if(lpv)
        lpsbin->lpb = (LPBYTE)PvAllocMore(cLOFileSize, lpv);
    else
        lpsbin->lpb = (LPBYTE)PvAlloc(cLOFileSize);

    if(!ReadFile(hFile, (LPSTR)lpsbin->lpb, cLOFileSize, &cBytesRead, NULL))
    {
        ec = 3;
        goto err;
    }

    lpsbin->cb = cLOFileSize;

err:
    CloseHandle(hFile);

#endif

    return ec;

}


/***********************************************************************/
/*
 -  SzGetEntryID
 -
 *  Purpose:
 *      Converts a LPENTRYID struct to a printable string as
 *      best we can for a given data type.
 *
 *  Parameters:
 *      lpsz        - destination string buffer
 *      lp          - pointer to entryid
 *      cb          - length of binary data contained in entryid array
 *
 *  Returns:
 *      lpsz        - Stringized version of the entry id
 *
 */
/***********************************************************************/

LPTSTR SzGetEntryID(LPTSTR lpsz,LPENTRYID lp, ULONG cb)
{
    char    lpszHex[9];
    ULONG   ulTotal = 0;
    ULONG   ulCount = 0;
    LPBYTE  lpbData = lp->ab;
    LPBYTE  lpbFlags = lp->abFlags;
    ULONG   i;

    wsprintf(lpsz,"cb == %lu, *ab == ",cb);

    ulTotal += strlen(lpsz);

    // put flags in char form from binary
    for(i=0 ; i < 4 ; i++)
    {
        wsprintf(lpszHex,"%02X ",lpbFlags[i]);
        lstrcat(lpsz,lpszHex);
        ulTotal += 3;
    }

    cb = cb - 4;        // to ensure proper length of data entryid

    // put data in char form from binary
    while( (ulCount < cb) && (ulTotal < (MAX_LOG_BUFF - 20) ) )
    {
        wsprintf(lpszHex, "%02X ", *lpbData);
        lstrcat(lpsz, lpszHex);
        ulTotal += 3;
        lpbData++;
        ulCount++;
    }

    if( (cb * 3) > (MAX_LOG_BUFF) - 20  )
    {
        lstrcat(lpsz, " }etc");
    }

    return(lpsz);
}

/*
 -  SzGetPropValue
 -
 *  Purpose:
 *      Converts a PropValue struct to a printable string as
 *      best we can for a given data type.
 *
 *  Parameters:
 *      lpsz        - destination string buffer
 *      lpPV        - pointer to a PropValue data struct
 *
 *  Returns:
 *      lpsz        - Stringized version of the property value
 *
 */

LPSTR SzGetPropValue(LPSTR lpsz, LPSPropValue lpPV)
{
    SYSTEMTIME FAR  *lpSysTime          = NULL;
    int             dBytes              = 0;
    BOOL            fFlag               = FALSE;
    DWORD           dwRet               = 0;
    int             dSizeUnicode        = 0;
    LPCSTR          szDefaultChar       = NULL;
    BOOL far *      lpfDefaultCharUsed  = NULL;
    ULONG           idx;
    ULONG           cChars              = 0;
    char            szMV[MAX_LOG_BUFF];


    switch(PROP_TYPE(lpPV->ulPropTag))
    {
        case PT_I2:
            wsprintf(lpsz, "%hd", lpPV->Value.i);
            break;

        case PT_LONG:
            wsprintf(lpsz, "%ld", lpPV->Value.l);
            break;

        case PT_R4:
            sprintf(lpsz, "%e", lpPV->Value.flt);
            break;

        case PT_DOUBLE:
            sprintf(lpsz, "%le", lpPV->Value.dbl);
            break;

        case PT_CURRENCY:
            wsprintf(lpsz, "%ld %ld", lpPV->Value.cur.Lo, lpPV->Value.cur.Hi);
            break;

        case PT_APPTIME:
            sprintf(lpsz, "%le", lpPV->Value.at);
            break;

        case PT_BOOLEAN:
            wsprintf(lpsz, "%u", lpPV->Value.b);
            break;

        case PT_SYSTIME:
            lpSysTime = (LPSYSTEMTIME) PvAlloc(sizeof(SYSTEMTIME) );

            FileTimeToSystemTime( (FILETIME FAR *) &lpPV->Value.ft,(SYSTEMTIME FAR *)lpSysTime);

            wsprintf(lpsz,"%4d/%02d/%02d  %02d:%02d:%02d",
                        lpSysTime->wYear,
                        lpSysTime->wMonth,
                        lpSysTime->wDay,
                        lpSysTime->wHour,
                        lpSysTime->wMinute,
                        lpSysTime->wSecond);

            PvFree(lpSysTime);
            break;

        case PT_STRING8:
            if(lpPV->Value.lpszA)
            {
                if(!_memccpy(lpsz, lpPV->Value.lpszA, '\0', MAX_LOG_BUFF-1))
                {
                    lpsz[MAX_LOG_BUFF] = '\0';
                }
            }
            else
            {
                lstrcpy(lpsz, "(null)");
            }
            break;

        case PT_BINARY:
            {
                char    lpszHex[9];
                ULONG   cb = 0;
                LPBYTE  lpb = lpPV->Value.bin.lpb;
                cChars = 0;

                wsprintf(lpsz, "cb: %3lu\t*pb: ", lpPV->Value.bin.cb);
                cChars += strlen(lpsz);

                while((cb < lpPV->Value.bin.cb) && (cChars < MAX_LOG_BUFF-16 ) )
                {
                    wsprintf(lpszHex, "%02X ", *lpb);
                    lstrcat(lpsz, lpszHex);
                    cChars += 3;
                    lpb++;
                    cb++;
                }

                if( ((lpPV->Value.bin.cb)*3) > (MAX_LOG_BUFF)-16 )
                {
                    lstrcat(lpsz, " }etc");
                }

            }
            break;

        case PT_UNICODE:

#ifdef WIN32
//            dSizeUnicode = wcslen(lpPV->Value.lpszW);
//            dSizeUnicode = dSizeUnicode + 2;   // this is for two bytes of null termination

            dBytes = WideCharToMultiByte(   (UINT)      CP_ACP,             // code page
                                            (DWORD)     0,                  // perf/mapping flags
                                            (LPCWSTR)   lpPV->Value.lpszW,  // Unicode string
                                            (int)       -1 ,                // length of unicode string assume NULL terminated
                                            (LPSTR)     lpsz,               // String 8 string
                                            (int)       MAX_LOG_BUFF -1,     // length in bytes of string 8 string
                                            (LPCSTR)    szDefaultChar,      // default char to use(NULL)
                                            (BOOL far *)    lpfDefaultCharUsed);// default characters used (NULL)

            dwRet = GetLastError();

#endif

            break;


        case PT_I8:
            wsprintf(lpsz, "%ld %ld", lpPV->Value.li.LowPart, lpPV->Value.li.HighPart);
            break;

        case PT_CLSID:
            SzIIDToString(lpsz, lpPV->Value.lpguid);
            break;

        case PT_NULL:
            lstrcpy(lpsz, "<NULL>");
            break;

        case PT_OBJECT:
            lstrcpy(lpsz, "<MAPITEST: YOUR OBJECT HERE>");
            break;

        case PT_ERROR:
            GetString("MAPIErrors", (ULONG)lpPV->Value.err, lpsz);
            break;

        case PT_UNSPECIFIED:
            lstrcpy(lpsz, "(unspecified)");
            break;



        /*********** MULIT VALUE PROPERTIES ********/

        // format is as follows:
        // cValues : val1 ;; val2 ;; val3 ;; }etc

        case PT_MV_I2:
            cChars = 0;
            wsprintf(lpsz, "%lu :",lpPV->Value.MVi.cValues);

            for(idx = 0 ; idx < lpPV->Value.MVi.cValues ; idx++ )
            {
                wsprintf(szMV,"%hd ;;",lpPV->Value.MVi.lpi[idx] );
                lstrcat(lpsz,szMV);

                cChars = lstrlen(lpsz);

                if( cChars > (MAX_LOG_BUFF)-16 )
                {
                    lstrcat(lpsz, " }etc");
                    break;
                }
            }
            break;

        case PT_MV_LONG:
            cChars = 0;
            wsprintf(lpsz, "%lu :",lpPV->Value.MVl.cValues);

            for(idx = 0 ; idx < lpPV->Value.MVl.cValues ; idx++ )
            {
                wsprintf(szMV,"%ld ;;",lpPV->Value.MVl.lpl[idx] );
                lstrcat(lpsz,szMV);

                cChars = lstrlen(lpsz);

                if( cChars > (MAX_LOG_BUFF)-16 )
                {
                    lstrcat(lpsz, " }etc");
                    break;
                }
            }
            break;

        case PT_MV_R4:
            cChars = 0;
            wsprintf(lpsz, "%lu :",lpPV->Value.MVflt.cValues);

            for(idx = 0 ; idx < lpPV->Value.MVflt.cValues ; idx++ )
            {
                sprintf(szMV,"%e ;;",lpPV->Value.MVflt.lpflt[idx] );
                lstrcat(lpsz,szMV);

                cChars = lstrlen(lpsz);

                if( cChars > (MAX_LOG_BUFF)-16 )
                {
                    lstrcat(lpsz, " }etc");
                    break;
                }
            }
            break;

        case PT_MV_DOUBLE:
            cChars = 0;
            wsprintf(lpsz, "%lu :",lpPV->Value.MVdbl.cValues);

            for(idx = 0 ; idx < lpPV->Value.MVdbl.cValues ; idx++ )
            {
                sprintf(szMV,"%le ;;",lpPV->Value.MVdbl.lpdbl[idx] );
                lstrcat(lpsz,szMV);

                cChars = lstrlen(lpsz);

                if( cChars > (MAX_LOG_BUFF)-16 )
                {
                    lstrcat(lpsz, " }etc");
                    break;
                }
            }
            break;

        case PT_MV_CURRENCY:
            cChars = 0;
            wsprintf(lpsz, "%lu :",lpPV->Value.MVcur.cValues);

            for(idx = 0 ; idx < lpPV->Value.MVcur.cValues ; idx++ )
            {
                wsprintf(szMV,"%ld %ld ;;",
                        lpPV->Value.MVcur.lpcur[idx].Lo,
                        lpPV->Value.MVcur.lpcur[idx].Hi );

                lstrcat(lpsz,szMV);

                cChars = lstrlen(lpsz);

                if( cChars > (MAX_LOG_BUFF)-16 )
                {
                    lstrcat(lpsz, " }etc");
                    break;
                }
            }
            break;

        case PT_MV_APPTIME:
            cChars = 0;
            wsprintf(lpsz, "%lu :",lpPV->Value.MVat.cValues);

            for(idx = 0 ; idx < lpPV->Value.MVat.cValues ; idx++ )
            {
                sprintf(szMV,"%le ;;",lpPV->Value.MVat.lpat[idx] );
                lstrcat(lpsz,szMV);

                cChars = lstrlen(lpsz);

                if( cChars > (MAX_LOG_BUFF)-16 )
                {
                    lstrcat(lpsz, " }etc");
                    break;
                }
            }

            break;

        case PT_MV_SYSTIME:
            cChars = 0;
            // this has only one : on purpose, since two : is harder to strtok
            //  since there is a colon in between hour:Minute:second
            wsprintf(lpsz, "%lu :",lpPV->Value.MVft.cValues);

            for(idx = 0 ; idx < lpPV->Value.MVft.cValues ; idx++ )
            {
                lpSysTime = (LPSYSTEMTIME) PvAlloc(sizeof(SYSTEMTIME) );

                FileTimeToSystemTime( (FILETIME FAR *) &lpPV->Value.MVft.lpft[idx],(SYSTEMTIME FAR *)lpSysTime);

                wsprintf(szMV,"%4d/%02d/%02d  %02d:%02d:%02d ;;",
                        lpSysTime->wYear,
                        lpSysTime->wMonth,
                        lpSysTime->wDay,
                        lpSysTime->wHour,
                        lpSysTime->wMinute,
                        lpSysTime->wSecond);

                lstrcat(lpsz,szMV);

                PvFree(lpSysTime);

                cChars = lstrlen(lpsz);

                if( cChars > (MAX_LOG_BUFF)-16 )
                {
                    lstrcat(lpsz, " }etc");
                    break;
                }
            }
            break;

        case PT_MV_STRING8:
            cChars = 0;
            wsprintf(lpsz, "%lu :",lpPV->Value.MVszA.cValues);

            for(idx = 0 ; idx < lpPV->Value.MVszA.cValues ; idx++ )
            {
                if(lpPV->Value.MVszA.lppszA[idx])
                    wsprintf(szMV,"%s ;;", lpPV->Value.MVszA.lppszA[idx] );
                else
                    wsprintf(szMV,"%s ;;", "(NULL)" );

                lstrcat(lpsz,szMV);

                cChars = lstrlen(lpsz);

                if( cChars > (MAX_LOG_BUFF)-16 )
                {
                    lstrcat(lpsz, " }etc");
                    break;
                }
            }

            break;

        case PT_MV_BINARY:
            cChars = 0;
            wsprintf(lpsz, "%lu :",lpPV->Value.MVbin.cValues);

            for(idx = 0 ; idx < lpPV->Value.MVbin.cValues ; idx++ )
            {
                char    lpszHex[9];
                ULONG   cb          = 0;
                LPBYTE  lpb         = lpPV->Value.MVbin.lpbin[idx].lpb;

                wsprintf(szMV, "cb: %3lu\t*pb: ", lpPV->Value.MVbin.lpbin[idx].cb);
                lstrcat(lpsz,szMV);
                cChars = lstrlen(lpsz);

                while((cb < lpPV->Value.MVbin.lpbin[idx].cb) && (cChars < MAX_LOG_BUFF-16 ) )
                {
                    wsprintf(lpszHex, "%02X ", *lpb);
                    lstrcat(lpsz, lpszHex);
                    cChars += 3;
                    lpb++;
                    cb++;
                }

                lstrcat(lpsz," ;;");

                cChars = lstrlen(lpsz);

                if( cChars > (MAX_LOG_BUFF)-16 )
                {
                    lstrcat(lpsz, " }etc");
                    break;
                }
            }

            break;


        case PT_MV_UNICODE:
#ifdef WIN32
            cChars = 0;
            wsprintf(lpsz, "%lu :",lpPV->Value.MVszW.cValues);

            for(idx = 0 ; idx < lpPV->Value.MVszW.cValues ; idx++ )
            {

//                dSizeUnicode = 0;
//               dSizeUnicode = wcslen(lpPV->Value.MVszW.lppszW[idx]);
//                dSizeUnicode = dSizeUnicode + 2;   // this is for two bytes of null termination

                dBytes = WideCharToMultiByte((UINT)     CP_ACP,                         // code page
                                            (DWORD)     0,                              // perf/mapping flags
                                            (LPCWSTR)   lpPV->Value.MVszW.lppszW[idx],  // Unicode string
                                            (int)       -1,                             // length of unicode string assumed to Be NULL terimiated
                                            (LPSTR)     szMV,                           // String 8 string
                                            (int)       MAX_LOG_BUFF -1,                // length in bytes of string 8 string(max)
                                            (LPCSTR)    szDefaultChar,                  // default char to use(NULL)
                                            (BOOL far *)    lpfDefaultCharUsed);            // default characters used (NULL)

                dwRet = GetLastError();

                lstrcat(lpsz,szMV);

                lstrcat(lpsz," ;;");

                cChars = lstrlen(lpsz);

                if( cChars > (MAX_LOG_BUFF)-16 )
                {
                    lstrcat(lpsz, " }etc");
                    break;
                }
            }
#endif
            break;


        case PT_MV_CLSID:
            cChars = 0;
            wsprintf(lpsz, "%lu :",lpPV->Value.MVguid.cValues);

            for(idx = 0 ; idx < lpPV->Value.MVguid.cValues ; idx++ )
            {
                SzIIDToString(szMV,   &(lpPV->Value.MVguid.lpguid[idx]) );
                lstrcat(lpsz,szMV);

                cChars = lstrlen(lpsz);

                if( cChars > (MAX_LOG_BUFF)-16 )
                {
                    lstrcat(lpsz, " }etc");
                    break;
                }
            }
            break;

        case PT_MV_I8:
            cChars = 0;
            wsprintf(lpsz, "%lu :",lpPV->Value.MVli.cValues);

            for(idx = 0 ; idx < lpPV->Value.MVli.cValues ; idx++ )
            {
                wsprintf(szMV,"%ld %ld ;;",
                        lpPV->Value.MVli.lpli[idx].LowPart,
                        lpPV->Value.MVli.lpli[idx].HighPart );
                lstrcat(lpsz,szMV);

                cChars = lstrlen(lpsz);

                if( cChars > (MAX_LOG_BUFF)-16 )
                {
                    lstrcat(lpsz, " }etc");
                    break;
                }
            }
            break;

        default:
            wsprintf(lpsz, "%lu == UNKNOWN PROP TYPE", PROP_TYPE(lpPV->ulPropTag) );
            break;
    }

    return lpsz;
}


/*
 -  SzIIDToString
 -
 *  Purpose:
 *      Converts a IID to a string for display purposes.
 *
 *  Parameters:
 *      lpsz        - Destination string of conversion
 *      lpiid        - Pointer to the IID to be converted
 *
 *  Returns:
 *      lpsz        - Pointer to string buffer passed in
 *
 *  Functions that pass in a LPSTR should make sure it is at least
 *  37 bytes long.
 */

LPSTR SzIIDToString(LPSTR lpsz, LPIID lpiid)
{
    int     i;
    char    ch;

    if(lpiid)
    {
        wsprintf(lpsz, "%08lX %04X %04X  ", lpiid->Data1, lpiid->Data2, lpiid->Data3);

        for(i = 0; i < 8; i++)
        {
            lpsz[i*2+20] = ((ch=((lpiid->Data4[i] & 0xF0) >> 4)) < 10 ? ch+0x30 : ch+0x37);
            lpsz[i*2+21] = ((ch=(lpiid->Data4[i] & 0x0F)) < 10 ? ch+0x30 : ch+0x37);
        }

        lpsz[i*2+20] = '\0';
    }
    else
        lstrcpy(lpsz, "(null)");

    return lpsz;
}


/*
 -  CopyPropValue
 -
 *  Purpose:
 *      Copies the contents of one PropValue into another.
 *      This function does a member for member copy - freeing
 *      and allocating memory for the data as appropriate.
 *      Currently, Multi-Value properties are not supported.
 *      If dev. does than we will.
 *
 *  Parameters:
 *      lpspv1          - Pointer to the destination PropValue
 *      lpspv2          - Pointer to the source PropValue
 *      lpv             - Void pointer to a parent in a memory chain (optional)
 *
 *  Returns:
 *      Void.
 *
 */

void CopyPropValue(LPSPropValue lpspv1, LPSPropValue lpspv2, LPVOID lpv)
{
    ULONG idx;
    int             dSizeUnicode1        = 0;

    lpspv1->ulPropTag  = lpspv2->ulPropTag;
    lpspv1->dwAlignPad = lpspv2->dwAlignPad;

    /* switch() through all PT_'s and set values appropriately */

    switch(PROP_TYPE(lpspv2->ulPropTag))
    {
        case PT_I2:
            lpspv1->Value.i = lpspv2->Value.i;
            break;

        case PT_LONG:
            lpspv1->Value.l = lpspv2->Value.l;
            break;

        case PT_R4:
            lpspv1->Value.flt = lpspv2->Value.flt;
            break;

        case PT_DOUBLE:
            lpspv1->Value.dbl = lpspv2->Value.dbl;
            break;

        case PT_CURRENCY:
            lpspv1->Value.cur.Lo = lpspv2->Value.cur.Lo;
            lpspv1->Value.cur.Hi = lpspv2->Value.cur.Hi;
            break;

        case PT_APPTIME:
            lpspv1->Value.at = lpspv2->Value.at;
            break;

        case PT_BOOLEAN:
            lpspv1->Value.b = lpspv2->Value.b;
            break;

        case PT_SYSTIME:
            lpspv1->Value.ft.dwLowDateTime = lpspv2->Value.ft.dwLowDateTime;
            lpspv1->Value.ft.dwHighDateTime = lpspv2->Value.ft.dwHighDateTime;
            break;

        case PT_STRING8:
            if(lpv)
                lpspv1->Value.lpszA = (LPTSTR)PvAllocMore(lstrlen(lpspv2->Value.lpszA) + 1, lpv);
            else
                lpspv1->Value.lpszA = (LPTSTR)PvAlloc(lstrlen(lpspv2->Value.lpszA) + 1);

            lstrcpy(lpspv1->Value.lpszA, lpspv2->Value.lpszA);
            break;

        case PT_BINARY:
            lpspv1->Value.bin.cb = lpspv2->Value.bin.cb;

            if(lpspv2->Value.bin.cb)
            {
                ULONG   cb;
                LPBYTE  lpb1;
                LPBYTE  lpb2;

                cb  = lpspv2->Value.bin.cb;

                if(lpv)
                    lpspv1->Value.bin.lpb = (LPBYTE)PvAllocMore(cb, lpv);
                else
                    lpspv1->Value.bin.lpb = (LPBYTE)PvAlloc(cb);

                lpb1 = lpspv1->Value.bin.lpb;
                lpb2 = lpspv2->Value.bin.lpb;

                while(cb--)
                    *lpb1++ = *lpb2++;
            }
            else
                lpspv1->Value.bin.lpb = NULL;

            break;

        case PT_UNICODE:

#ifdef WIN32
            dSizeUnicode1 = (wcslen(lpspv2->Value.lpszW) * sizeof(WCHAR)) + 2;

            if(lpv)
                lpspv1->Value.lpszW = (LPWSTR)PvAllocMore(dSizeUnicode1, lpv);
            else
                lpspv1->Value.lpszW = (LPWSTR)PvAlloc(dSizeUnicode1);

            memcpy(lpspv1->Value.lpszW,lpspv2->Value.lpszW,(size_t)dSizeUnicode1);
#endif
            break;


            break;

        case PT_CLSID:
            if(!lpspv1->Value.lpguid)
            {
                if(lpv)
                    lpspv1->Value.lpguid = (LPGUID)PvAllocMore(sizeof(GUID), lpv);
                else
                    lpspv1->Value.lpguid = (LPGUID)PvAlloc(sizeof(GUID));
            }

            CopyGuid(lpspv1->Value.lpguid, lpspv2->Value.lpguid);
            break;

        /* Multi-Valued Properties go here!!! */

        case PT_MV_I2:
            if(lpv)
                lpspv1->Value.MVi.lpi = (short int *) PvAllocMore(
                                         (sizeof(short int) * lpspv2->Value.MVi.cValues) , lpv );
            else
                lpspv1->Value.MVi.lpi = (short int *) PvAlloc(
                            (sizeof(short int) * lpspv2->Value.MVi.cValues) );

            lpspv1->Value.MVi.cValues = lpspv2->Value.MVi.cValues;

            memcpy(lpspv1->Value.MVi.lpi,lpspv2->Value.MVi.lpi,(size_t)
                   (sizeof(short int) * lpspv2->Value.MVi.cValues) );

            break;

        case PT_MV_LONG:
            if(lpv)
                lpspv1->Value.MVl.lpl = (LONG *) PvAllocMore(
                                         (sizeof(LONG) * lpspv2->Value.MVl.cValues) , lpv );
            else
                lpspv1->Value.MVl.lpl = (LONG *) PvAlloc(
                            (sizeof(LONG) * lpspv2->Value.MVl.cValues) );

            lpspv1->Value.MVl.cValues = lpspv2->Value.MVl.cValues;

            memcpy(lpspv1->Value.MVl.lpl,lpspv2->Value.MVl.lpl,(size_t)
                   (sizeof(LONG) * lpspv2->Value.MVl.cValues) );

            break;

        case PT_MV_R4:
            if(lpv)
                lpspv1->Value.MVflt.lpflt = (float *) PvAllocMore(
                                         (sizeof(float) * lpspv2->Value.MVflt.cValues) , lpv );
            else
                lpspv1->Value.MVflt.lpflt = (float *) PvAlloc(
                            (sizeof(float) * lpspv2->Value.MVflt.cValues) );

            lpspv1->Value.MVflt.cValues = lpspv2->Value.MVflt.cValues;

            memcpy(lpspv1->Value.MVflt.lpflt,lpspv2->Value.MVflt.lpflt,(size_t)
                   (sizeof(float) * lpspv2->Value.MVflt.cValues) );

            break;

        case PT_MV_DOUBLE:
            if(lpv)
                lpspv1->Value.MVdbl.lpdbl = (double *) PvAllocMore(
                                         (sizeof(double) * lpspv2->Value.MVdbl.cValues) , lpv );
            else
                lpspv1->Value.MVdbl.lpdbl = (double *) PvAlloc(
                            (sizeof(double) * lpspv2->Value.MVdbl.cValues) );

            lpspv1->Value.MVdbl.cValues = lpspv2->Value.MVdbl.cValues;

            memcpy(lpspv1->Value.MVdbl.lpdbl,lpspv2->Value.MVdbl.lpdbl,(size_t)
                   (sizeof(double) * lpspv2->Value.MVdbl.cValues) );

            break;

        case PT_MV_CURRENCY:
            if(lpv)
                lpspv1->Value.MVcur.lpcur = (CURRENCY *) PvAllocMore(
                                         (sizeof(CURRENCY) * lpspv2->Value.MVcur.cValues) , lpv );
            else
                lpspv1->Value.MVcur.lpcur = (CURRENCY *) PvAlloc(
                            (sizeof(CURRENCY) * lpspv2->Value.MVcur.cValues) );

            lpspv1->Value.MVcur.cValues = lpspv2->Value.MVcur.cValues;

            memcpy(lpspv1->Value.MVcur.lpcur,lpspv2->Value.MVcur.lpcur,(size_t)
                   (sizeof(CURRENCY) * lpspv2->Value.MVcur.cValues) );

            break;

        case PT_MV_APPTIME:
            if(lpv)
                lpspv1->Value.MVat.lpat = (double *) PvAllocMore(
                                         (sizeof(double) * lpspv2->Value.MVat.cValues) , lpv );
            else
                lpspv1->Value.MVat.lpat = (double *) PvAlloc(
                            (sizeof(double) * lpspv2->Value.MVat.cValues) );

            lpspv1->Value.MVat.cValues = lpspv2->Value.MVat.cValues;

            memcpy(lpspv1->Value.MVat.lpat,lpspv2->Value.MVat.lpat,(size_t)
                   (sizeof(double) * lpspv2->Value.MVat.cValues) );

            break;

        case PT_MV_SYSTIME:
            if(lpv)
                lpspv1->Value.MVft.lpft = (FILETIME *) PvAllocMore(
                                         (sizeof(FILETIME) * lpspv2->Value.MVft.cValues) , lpv );
            else
                lpspv1->Value.MVft.lpft = (FILETIME *) PvAlloc(
                            (sizeof(FILETIME) * lpspv2->Value.MVft.cValues) );

            lpspv1->Value.MVft.cValues = lpspv2->Value.MVft.cValues;

            memcpy(lpspv1->Value.MVft.lpft,lpspv2->Value.MVft.lpft,(size_t)
                   (sizeof(FILETIME) * lpspv2->Value.MVft.cValues) );

            break;

        case PT_MV_STRING8:
            if(lpv)
                lpspv1->Value.MVszA.lppszA = (LPSTR *) PvAllocMore(
                                         (sizeof(LPSTR) * lpspv2->Value.MVszA.cValues) , lpv );
            else
                lpspv1->Value.MVszA.lppszA = (LPSTR *) PvAlloc(
                            (sizeof(LPSTR) * lpspv2->Value.MVszA.cValues) );

            lpspv1->Value.MVszA.cValues = lpspv2->Value.MVszA.cValues;

            for(idx = 0 ;idx < lpspv2->Value.MVszA.cValues ; idx++)
            {

                if(lpv)
                {
                    lpspv1->Value.MVszA.lppszA[idx] =
                        (LPTSTR) PvAllocMore(lstrlen(lpspv2->Value.MVszA.lppszA[idx]) + 1,
                        lpv);
                }
                else
                {
                    lpspv1->Value.MVszA.lppszA[idx] =
                         (LPTSTR) PvAllocMore(lstrlen(lpspv2->Value.MVszA.lppszA[idx]) + 1,
                           lpspv1->Value.MVszA.lppszA);
                }
                lstrcpy(lpspv1->Value.MVszA.lppszA[idx] , lpspv2->Value.MVszA.lppszA[idx] );
            }
            break;

        case PT_MV_UNICODE:
#ifdef WIN32

            if(lpv)
                lpspv1->Value.MVszW.lppszW = (LPWSTR *) PvAllocMore(
                                         (sizeof(LPWSTR) * lpspv2->Value.MVszW.cValues) , lpv );
            else
                lpspv1->Value.MVszW.lppszW = (LPWSTR *) PvAlloc(
                            (sizeof(LPWSTR) * lpspv2->Value.MVszW.cValues) );

            lpspv1->Value.MVszW.cValues = lpspv2->Value.MVszW.cValues;

            for(idx = 0 ;idx < lpspv2->Value.MVszW.cValues ; idx++)
            {

                dSizeUnicode1 = (wcslen(lpspv2->Value.lpszW) * sizeof(WCHAR)) + 2;

                if(lpv)
                    lpspv1->Value.MVszW.lppszW[idx] = (LPWSTR) PvAllocMore(dSizeUnicode1,lpv);
                else
                    lpspv1->Value.MVszW.lppszW[idx] =
                         (LPWSTR) PvAllocMore(dSizeUnicode1,lpspv1->Value.MVszW.lppszW);

                memcpy( lpspv1->Value.MVszW.lppszW[idx],
                        lpspv2->Value.MVszW.lppszW[idx],
                        (size_t)dSizeUnicode1);
            }
#endif
            break;

        case PT_MV_BINARY:
            if(lpv)
                lpspv1->Value.MVbin.lpbin = (SBinary *) PvAllocMore(
                                         (sizeof(SBinary) * lpspv2->Value.MVbin.cValues) , lpv );
            else
                lpspv1->Value.MVbin.lpbin = (SBinary *) PvAlloc(
                            (sizeof(SBinary) * lpspv2->Value.MVbin.cValues) );

            lpspv1->Value.MVbin.cValues = lpspv2->Value.MVbin.cValues;

            for(idx = 0; idx < lpspv1->Value.MVbin.cValues ; idx++)
            {
                lpspv1->Value.MVbin.lpbin[idx].cb = lpspv2->Value.MVbin.lpbin[idx].cb;

                if(lpspv2->Value.MVbin.lpbin[idx].cb)
                {
                    ULONG   cb;
                    LPBYTE  lpb1;
                    LPBYTE  lpb2;

                    cb  = lpspv2->Value.MVbin.lpbin[idx].cb;

                    if(lpv)
                        lpspv1->Value.MVbin.lpbin[idx].lpb = (LPBYTE)PvAllocMore(cb, lpv);
                    else
                        lpspv1->Value.MVbin.lpbin[idx].lpb = (LPBYTE)PvAlloc(cb);

                    lpb1 = lpspv1->Value.MVbin.lpbin[idx].lpb;
                    lpb2 = lpspv2->Value.MVbin.lpbin[idx].lpb;

                    while(cb--)
                        *lpb1++ = *lpb2++;
                }
                else
                    lpspv1->Value.MVbin.lpbin[idx].lpb = NULL;

            }
            break;

        case PT_MV_CLSID:
            if(lpv)
                lpspv1->Value.MVguid.lpguid = (GUID *) PvAllocMore(
                                         (sizeof(GUID) * lpspv2->Value.MVguid.cValues) , lpv );
            else
                lpspv1->Value.MVguid.lpguid = (GUID *) PvAlloc(
                            (sizeof(GUID) * lpspv2->Value.MVguid.cValues) );

            lpspv1->Value.MVguid.cValues = lpspv2->Value.MVguid.cValues;

            memcpy(lpspv1->Value.MVguid.lpguid,lpspv2->Value.MVguid.lpguid,(size_t)
                   (sizeof(GUID) * lpspv2->Value.MVguid.cValues) );

            break;

        case PT_MV_I8:
            if(lpv)
                lpspv1->Value.MVli.lpli = (LARGE_INTEGER *) PvAllocMore(
                                         (sizeof(LARGE_INTEGER) * lpspv2->Value.MVli.cValues) , lpv );
            else
                lpspv1->Value.MVli.lpli = (LARGE_INTEGER *) PvAlloc(
                            (sizeof(LARGE_INTEGER) * lpspv2->Value.MVli.cValues) );

            lpspv1->Value.MVli.cValues = lpspv2->Value.MVli.cValues;

            memcpy(lpspv1->Value.MVli.lpli,lpspv2->Value.MVli.lpli,(size_t)
                   (sizeof(LARGE_INTEGER) * lpspv2->Value.MVli.cValues) );

            break;

        default:
            lpspv1->Value.x = lpspv2->Value.x;
            break;
    }
}


/*
 -  MakePropValue
 -
 *  Purpose:
 *      Given a pointer to an SPropValue struct, a PropTag, and a
 *      string, make a prop value.  This is made to be compatible
 *      with the TableData text file format.  The lpv parameter
 *      is used to indicate that the memory should be allocated
 *      with MAPIAllocateMore().  lpv is passed in as the parent
 *      memory buffer to link to.
 *
 *  Parameters:
 *      lpspv       - Pointer to the destination prop value struct
 *      ulTag       - Property Tag identifying this property
 *      lpsz        - Stringized version of the data
 *      lpv         - Void pointer to a parent in a memory chain
 *
 *  WARNING:
 *      This may call strtok on lpsz. Strtok has the side effect of modifying
 *      the original string. So whatever lpsz points to may get modified.
 *
 *  Returns:
 *      BOOL    TRUE if successful, FALSE if not
 *
 */

BOOL MakePropValue(LPSPropValue lpspv, ULONG ulTag, LPSTR lpsz, LPVOID lpv)
{
    ULONG   idx = 0;
    BOOL    fRet = FALSE;
    ULONG   i, cVals;
    LPSTR   lpszF, lpszB;
    char    szTok[4096];
    char            *lpszToken      = NULL;
    SYSTEMTIME FAR  *lpSysTime  = NULL;
    LONG    l                   = 0;

    lpspv->ulPropTag = ulTag;

    switch(PROP_TYPE(ulTag))
    {
        case PT_I2:
            lpspv->Value.i = (short int)atoi(lpsz);
            break;

        case PT_LONG:
            lpspv->Value.l = strtol(lpsz, NULL, 0);
            break;

        case PT_R4:
            lpspv->Value.flt = (float)atof(lpsz);
            break;

        case PT_DOUBLE:
            lpspv->Value.dbl = strtod(lpsz, NULL);
            break;

        case PT_APPTIME:
            lpspv->Value.at = strtod(lpsz, NULL);
            break;

        case PT_ERROR:              
// NEW
            if(GetID("MAPIErrors",lpsz, &l ) )
                lpspv->Value.err = l;
            else  // throw in dummy value
                lpspv->Value.err = S_OK;

// OLD          lpspv->Value.err = strtoul(lpsz, NULL, 0);
            break;

        case PT_BOOLEAN:              //Convert String to ULONG
            lpspv->Value.b = ((USHORT)strtoul(lpsz, NULL, 10));
            break;

        case PT_CURRENCY:
        {
              char c;
#ifdef WIN32
              lpspv->Value.cur.Hi = strtol(&lpsz[4], NULL, 0 );
              c = lpsz[4];
              lpsz[4] = 0;
              lpspv->Value.cur.Lo = strtol(lpsz,  NULL,   0 );
              lpsz[4] = c;
#endif
#ifdef WIN16
              lpspv->Value.cur.Hi = strtol(&lpsz[4], NULL, 0 );
              c = lpsz[4];
              lpsz[4] = 0;
              lpspv->Value.cur.Lo = strtol(lpsz,  NULL,   0 );
              lpsz[4] = c;
#endif
        }
              break;

        case PT_SYSTIME:
            // The text format for this is <number> <number> where the two
            // numbers are seperated by a space or tab

            lpSysTime = (LPSYSTEMTIME) PvAlloc(sizeof(SYSTEMTIME) );
            memset(lpSysTime,0,(size_t) sizeof(SYSTEMTIME) );
            
            lpszToken   = strtok(lpsz,"/");
            lpSysTime->wYear   = atoi(lpszToken);
            lpszToken   = strtok(NULL,"/");
            lpSysTime->wMonth  = atoi(lpszToken);
            lpszToken   = strtok(NULL," ");
            lpSysTime->wDay    = atoi(lpszToken);
            lpszToken   = strtok(NULL,":");
            lpSysTime->wHour   = atoi(lpszToken);
            lpszToken   = strtok(NULL,":");
            lpSysTime->wMinute = atoi(lpszToken);
            lpszToken   = strtok(NULL," ");
            lpSysTime->wSecond = atoi(lpszToken);

            if( ! SystemTimeToFileTime(lpSysTime, &(lpspv->Value.ft) ) )
            {
                PvFree(lpSysTime);
                goto Cleanup;
            }    

            PvFree(lpSysTime);
            break;                

        case PT_BINARY:
            if (*lpsz != '~')
            {
                LPBYTE lpb;
                // it is 5  because the format for the binary data is as follows:
                //  0x01 0x11 0xFF
                ULONG   cb  = lstrlen(lpsz)/BINARY_CHAR_SIZE;

                // lpszF is Front pointer to string parsing
                // lpszB is Backk pointer to string parsing
                lstrcpy(szTok, lpsz);
                lpszF = lpszB = &szTok[0];

                // determine length of binary data, if not null term, add 1 to count
                if(lstrlen(lpsz)%BINARY_CHAR_SIZE != 0)
                    cb++;

                if(lpv)
                    lpb = (LPBYTE)PvAllocMore(cb, lpv);
                else
                    lpb = (LPBYTE)PvAlloc(cb);

                lpspv->Value.bin.lpb    =  lpb;
                lpspv->Value.bin.cb     =  cb;

                // travers the binary data, and parse out the space in between
                //  the hex values, with lpszB searching for next space or null
                while(*lpszB)
                {

                    while(*lpszB != ' ' && *lpszB != '\t' && *lpszB != '\0')
                        lpszB++;

                    if(*lpszB)
                    {
                        *lpszB = '\0';
                        lpszB++;
                    }

                    // strtoul() requries string to be in the following
                    // format to read the data as hex instead of string.
                    //  0x01 0x11 0xFF
                    //  with the prepending 0x before all numbers
                    *lpb++ = (BYTE)strtoul(lpszF, NULL, 0);
                    lpszF = lpszB;
                }
            }
            else
                EcBinaryFromFile(&lpspv->Value.bin, &lpsz[1], lpv);

            break;

        case PT_STRING8:
            if (*lpsz != '~')
            {
                if(lpv)
                    lpspv->Value.lpszA = (LPSTR)PvAllocMore(lstrlen(lpsz)+1,
                                                            lpv);
                else
                    lpspv->Value.lpszA = (LPSTR)PvAlloc(lstrlen(lpsz)+1);

                lstrcpy(lpspv->Value.lpszA, lpsz);
            }
            else
                EcStringFromFile(&lpspv->Value.lpszA, &lpsz[1], lpv);
            break;

        case PT_UNICODE:
            String8ToUnicode(lpsz,&lpspv->Value.lpszW,lpv);
            break;

        case PT_CLSID:
            if(lpv)
                lpspv->Value.lpguid = (LPGUID)PvAllocMore(sizeof(GUID), lpv);
            else
                lpspv->Value.lpguid = (LPGUID)PvAlloc(sizeof(GUID));

            lstrcpy(szTok, lpsz);
            lpszF = lpszB = &szTok[0];

            while(*lpszB != ' ' && *lpszB != '\t')
                lpszB++;

            *lpszB = '\0';
            lpszB++;

            lpspv->Value.lpguid->Data1 = strtoul(lpszF, NULL, 0);

            lpszF = lpszB;

            while(*lpszB != ' ' && *lpszB != '\t')
                lpszB++;

            *lpszB = '\0';
            lpszB++;

            lpspv->Value.lpguid->Data2 = (USHORT)strtoul(lpszF, NULL, 0);

            lpszF = lpszB;

            while(*lpszB != ' ' && *lpszB != '\t')
                lpszB++;

            *lpszB = '\0';
            lpszB++;

            lpspv->Value.lpguid->Data3 = (USHORT)strtoul(lpszF, NULL, 0);

            lpszF = lpszB;

            for(idx = 0; idx < 8; idx++)
                 lpspv->Value.lpguid->Data4[idx] = lpszF[idx];
            break;

        case PT_I8:
//$ FUTURE FIX THIS
            // Not sure what the best way to handle this is. 2 ideas:
            // 1. Handle it like PT_SYSTIME where the two parts of the number are
            //      separated by space or tab
            // 2. Try to read in the whole number and if it overflows the low part
            //      then start on the high part
            break;

        /**********************************************/
        /***** Multi-Valued Properties go here!!! *****/
        /**********************************************/

        case PT_MV_I2:
            if ( lpszF = strtok(lpsz, ":") )
            {
                if ( cVals = strtoul(lpszF, NULL, 0) )
                {
                    lpspv->Value.MVi.cValues = cVals;

                    // Allocate array
                    if (lpv)
                        lpspv->Value.MVi.lpi = (short int FAR *)PvAllocMore(
                            sizeof(short int) * cVals, lpv);
                    else
                        lpspv->Value.MVi.lpi = (short int FAR *)PvAlloc(
                            sizeof(short int) * cVals);

                    for (i = 0; i < cVals; i++)
                    {
                        if ( lpszF = strtok(NULL, ";") )
                            lpspv->Value.MVi.lpi[i] = (short int)atoi(lpszF);
                        else
                            goto Cleanup;
                    }
                }
                else
                    goto Cleanup;
            }
            break;

        case PT_MV_LONG:
            if ( lpszF = strtok(lpsz, ":") )
            {
                if ( cVals = strtoul(lpszF, NULL, 0) )
                {
                    lpspv->Value.MVl.cValues = cVals;

                    // Allocate array
                    if (lpv)
                        lpspv->Value.MVl.lpl = (LONG FAR *)PvAllocMore(
                            sizeof(LONG) * cVals, lpv);
                    else
                        lpspv->Value.MVl.lpl = (LONG FAR *)PvAlloc(
                            sizeof(LONG) * cVals);

                    for (i = 0; i < cVals; i++)
                    {
                        if ( lpszF = strtok(NULL, ";") )
                            lpspv->Value.MVl.lpl[i] = (LONG)strtol(lpszF, NULL, 0);
                        else
                            goto Cleanup;
                    }
                }
                else
                    goto Cleanup;
            }
            break;

        case PT_MV_R4:
            if ( lpszF = strtok(lpsz, ":") )
            {
                if ( cVals = strtoul(lpszF, NULL, 0) )
                {
                    lpspv->Value.MVflt.cValues = cVals;

                    // Allocate array
                    if (lpv)
                        lpspv->Value.MVflt.lpflt = (float FAR *)PvAllocMore(
                            sizeof(float) * cVals, lpv);
                    else
                        lpspv->Value.MVflt.lpflt = (float FAR *)PvAlloc(
                            sizeof(float) * cVals);

                    for (i = 0; i < cVals; i++)
                    {
                        if ( lpszF = strtok(NULL, ";") )
                            lpspv->Value.MVflt.lpflt[i] = (float)atof(lpszF);
                        else
                            goto Cleanup;
                    }
                }
                else
                    goto Cleanup;
            }
            break;

        case PT_MV_DOUBLE:
            if ( lpszF = strtok(lpsz, ":") )
            {
                if ( cVals = strtoul(lpszF, NULL, 0) )
                {
                    lpspv->Value.MVdbl.cValues = cVals;

                    // Allocate array
                    if (lpv)
                        lpspv->Value.MVdbl.lpdbl = (double FAR *)PvAllocMore(
                            sizeof(double) * cVals, lpv);
                    else
                        lpspv->Value.MVdbl.lpdbl = (double FAR *)PvAlloc(
                            sizeof(double) * cVals);

                    for (i = 0; i < cVals; i++)
                    {
                        if ( lpszF = strtok(NULL, ";") )
                            lpspv->Value.MVdbl.lpdbl[i] = (double)strtod(lpszF, NULL);
                        else
                            goto Cleanup;
                    }
                }
                else
                    goto Cleanup;
            }
            break;

        case PT_MV_CURRENCY:
            if ( lpszF = strtok(lpsz, ":") )
            {
                if ( cVals = strtoul(lpszF, NULL, 0) )
                {
                    lpspv->Value.MVcur.cValues = cVals;

                    // Allocate array
                    if (lpv)
                        lpspv->Value.MVcur.lpcur = (CURRENCY FAR *)PvAllocMore(
                            sizeof(CURRENCY) * cVals, lpv);
                    else
                        lpspv->Value.MVcur.lpcur = (CURRENCY FAR *)PvAlloc(
                            sizeof(CURRENCY) * cVals);

                    for (i = 0; i < cVals; i++)
                    {
                        if ( lpszF = strtok(NULL, ";") )
                        {
                            char c;
                            #ifdef WIN32
                            lpspv->Value.MVcur.lpcur[i].Hi = strtol(&lpsz[4], NULL, 0 );
                            c = lpsz[4];
                            lpsz[4] = 0;
                            lpspv->Value.MVcur.lpcur[i].Lo = strtol(lpsz,  NULL,   0 );
                            lpsz[4] = c;
                            #endif
                            #ifdef WIN16
                            lpspv->Value.MVcur.lpcur[i].Hi = strtol(&lpsz[4], NULL, 0 );
                            c = lpsz[4];
                            lpsz[4] = 0;
                            lpspv->Value.MVcur.lpcur[i].Lo = strtol(lpsz,  NULL,   0 );
                            lpsz[4] = c;
                            #endif
                        }
                        else
                            goto Cleanup;
                    }
                }
                else
                    goto Cleanup;
            }
            break;

        case PT_MV_APPTIME:
            if ( lpszF = strtok(lpsz, ":") )
            {
                if ( cVals = strtoul(lpszF, NULL, 0) )
                {
                    lpspv->Value.MVat.cValues = cVals;

                    // Allocate array
                    if (lpv)
                        lpspv->Value.MVat.lpat = (double FAR *)PvAllocMore(
                            sizeof(double) * cVals, lpv);
                    else
                        lpspv->Value.MVat.lpat = (double FAR *)PvAlloc(
                            sizeof(double) * cVals);

                    for (i = 0; i < cVals; i++)
                    {
                        if ( lpszF = strtok(NULL, ";") )
                            lpspv->Value.MVat.lpat[i] = (double)strtod(lpszF, NULL);
                        else
                            goto Cleanup;
                    }
                }
                else
                    goto Cleanup;
            }
            break;

        case PT_MV_SYSTIME:
            // The text format for a single systime is <number> <number> where
            // the two numbers are seperated by a space or tab
            if ( lpsz = strtok(lpsz, ":") )
            {
                if ( cVals = strtoul(lpsz, NULL, 0) )
                {
                    lpspv->Value.MVft.cValues = cVals;

                    // Allocate array
                    if (lpv)
                        lpspv->Value.MVft.lpft = (FILETIME FAR *)PvAllocMore(
                            sizeof(FILETIME) * cVals, lpv);
                    else
                        lpspv->Value.MVft.lpft = (FILETIME FAR *)PvAlloc(
                            sizeof(FILETIME) * cVals);

                    for (i = 0; i < cVals; i++)
                    {
                        if ( lpsz = strtok(NULL, ";") )
                        {
                            lstrcpy(szTok, lpsz);
                            lpszF = lpszB = &szTok[0];

                            while(*lpszB != ' ' && *lpszB != '\t')
                                lpszB++;

                            *lpszB = '\0';
                            lpszB++;

                            lpspv->Value.MVft.lpft[i].dwHighDateTime = strtoul(lpszF, NULL, 0);

                            lpszF = lpszB;

                            while(*lpszB != ' ' && *lpszB != '\t' && *lpszB != '\0')
                                lpszB++;

                            *lpszB = '\0';

                            lpspv->Value.MVft.lpft[i].dwLowDateTime = strtoul(lpszF, NULL, 0);
                        }
                        else
                            goto Cleanup;
                    }
                }
                else
                    goto Cleanup;
            }
            break;

        case PT_MV_BINARY:
            if ( lpszF = strtok(lpsz, ":") )
            {
                if ( cVals = strtoul(lpszF, NULL, 0) )
                {
                    lpspv->Value.MVbin.cValues = cVals;

                    // Allocate array
                    if (lpv)
                        lpspv->Value.MVbin.lpbin = (SBinary FAR *)PvAllocMore(
                            sizeof(SBinary) * cVals, lpv);
                    else
                        lpspv->Value.MVbin.lpbin = (SBinary FAR *)PvAlloc(
                            sizeof(SBinary) * cVals);

                    for (i = 0; i < cVals; i++)
                    {
                        if ( lpsz = strtok(NULL, ";") )
                        {
                            if (*lpsz != '~')
                            {
                                LPBYTE lpb;
                                // it is 5  because the format for the binary data is as follows:
                                //  0x01 0x11 0xFF
                                ULONG   cb  = lstrlen(lpsz)/BINARY_CHAR_SIZE;

                                // lpszF is Front pointer to string parsing
                                // lpszB is Backk pointer to string parsing
                                lstrcpy(szTok, lpsz);
                                lpszF = lpszB = &szTok[0];

                                // determine length of binary data, if not null term, add 1 to count
                                if(lstrlen(lpsz)%BINARY_CHAR_SIZE != 0)
                                    cb++;

                                if(lpv)
                                    lpb = (LPBYTE)PvAllocMore(cb, lpv);
                                else
                                    lpb = (LPBYTE)PvAlloc(cb);

                                lpspv->Value.MVbin.lpbin[i].lpb    =  lpb;
                                lpspv->Value.MVbin.lpbin[i].cb     =  cb;

                                // travers the binary data, and parse out the space in between
                                //  the hex values, with lpszB searching for next space or null
                                while(*lpszB)
                                {

                                    while(*lpszB != ' ' && *lpszB != '\t' && *lpszB != '\0')
                                        lpszB++;

                                    if(*lpszB)
                                    {
                                        *lpszB = '\0';
                                        lpszB++;
                                    }

                                    // strtoul() requries string to be in the following
                                    // format to read the data as hex instead of string.
                                    //  0x01 0x11 0xFF
                                    //  with the prepending 0x before all numbers
                                    *lpb++ = (BYTE)strtoul(lpszF, NULL, 0);
                                    lpszF = lpszB;
                                }
                            }
                            else
                                EcBinaryFromFile(&lpspv->Value.MVbin.lpbin[i], &lpsz[1], lpv);
                        }
                        else
                            goto Cleanup;
                    }
                }
                else
                    goto Cleanup;
            }
            break;

        case PT_MV_STRING8:
            if ( lpszF = strtok(lpsz, ":") )
            {
                if ( cVals = strtoul(lpszF, NULL, 0) )
                {
                    lpspv->Value.MVszA.cValues = cVals;

                    // Allocate array of string pointers
                    if (lpv)
                        lpspv->Value.MVszA.lppszA = (LPSTR FAR *)PvAllocMore(
                            sizeof(LPSTR) * cVals, lpv);
                    else
                        lpspv->Value.MVszA.lppszA = (LPSTR FAR *)PvAlloc(
                            sizeof(LPSTR) * cVals);

                    // Parse strings
                    for (i = 0; i < cVals; i++)
                    {
                        if ( lpszF = strtok(NULL, ";") )
                        {
                            if (*lpszF != '~')
                            {
                                if(lpv)
                                    lpspv->Value.MVszA.lppszA[i] = (LPSTR)
                                        PvAllocMore(lstrlen(lpszF)+1, lpv);
                                else
                                    lpspv->Value.MVszA.lppszA[i] = (LPSTR)
                                        PvAlloc(lstrlen(lpszF)+1);

                                lstrcpy(lpspv->Value.MVszA.lppszA[i], lpszF);
                            }
                            else
                                EcStringFromFile( &lpspv->Value.MVszA.lppszA[i],
                                                  &lpszF[1], lpv);
                        }
                        else
                            goto Cleanup;
                    }
                }
                else
                    goto Cleanup;
            }
            break;

        case PT_MV_UNICODE:
//$ FUTURE FIX THIS

            if ( lpszF = strtok(lpsz, ":") )
            {
                if ( cVals = strtoul(lpszF, NULL, 0) )
                {
                    lpspv->Value.MVszW.cValues = cVals;

                    // Allocate array of string pointers
                    if (lpv)
                        lpspv->Value.MVszW.lppszW = (LPWSTR FAR *)PvAllocMore(
                            sizeof(LPWSTR) * cVals, lpv);
                    else
                        lpspv->Value.MVszW.lppszW = (LPWSTR FAR *)PvAlloc(
                            sizeof(LPWSTR) * cVals);

                    // Parse strings
                    for (i = 0; i < cVals; i++)
                    {
                        if ( lpszF = strtok(NULL, ";") )
                        {
                            lpspv->Value.MVszW.lppszW[i] = NULL;
/*
                            if (*lpszF != '~')
                            {
                                if(lpv)
                                    lpspv->Value.MVszW.lppszW[i] = (LPWSTR)
                                        PvAllocMore(lstrlen(lpszF)+1, lpv);
                                else
                                    lpspv->Value.MVszW.lppszW[i] = (LPWSTR)
                                        PvAlloc(lstrlen(lpszF)+1);

                                lstrcpy(lpspv->Value.MVszW.lppszW[i], lpszF);
                            }
                            else
                                EcStringFromFile( &lpspv->Value.MVszW.lppszW[i],
                                                  &lpszF[1], lpv);
*/
                        }
                        else
                            goto Cleanup;
                    }
                }
                else
                    goto Cleanup;
            }
            break;

        case PT_MV_CLSID:
            if ( lpszF = strtok(lpsz, ":") )
            {
                if ( cVals = strtoul(lpszF, NULL, 0) )
                {
                    lpspv->Value.MVguid.cValues = cVals;

                    // Allocate array
                    if (lpv)
                        lpspv->Value.MVguid.lpguid = (LPGUID)PvAllocMore(
                            sizeof(GUID) * cVals, lpv);
                    else
                        lpspv->Value.MVguid.lpguid = (LPGUID)PvAlloc(
                            sizeof(GUID) * cVals);

                    for (i = 0; i < cVals; i++)
                    {
                        if ( lpszF = strtok(NULL, ";") )
                        {
                            lstrcpy(szTok, lpsz);
                            lpszF = lpszB = &szTok[0];

                            while(*lpszB != ' ' && *lpszB != '\t')
                                lpszB++;

                            *lpszB = '\0';
                            lpszB++;

                            lpspv->Value.MVguid.lpguid[i].Data1 = strtoul(lpszF, NULL, 0);

                            lpszF = lpszB;

                            while(*lpszB != ' ' && *lpszB != '\t')
                                lpszB++;

                            *lpszB = '\0';
                            lpszB++;

                            lpspv->Value.MVguid.lpguid[i].Data2 = (USHORT)strtoul(lpszF, NULL, 0);

                            lpszF = lpszB;

                            while(*lpszB != ' ' && *lpszB != '\t')
                                lpszB++;

                            *lpszB = '\0';
                            lpszB++;

                            lpspv->Value.MVguid.lpguid[i].Data3 = (USHORT)strtoul(lpszF, NULL, 0);

                            lpszF = lpszB;

                            for(idx = 0; idx < 8; idx++)
                                 lpspv->Value.MVguid.lpguid[i].Data4[idx] = lpszF[idx];
                        }
                        else
                            goto Cleanup;
                    }
                }
                else
                    goto Cleanup;
            }
            break;

        case PT_MV_I8:
//$ FUTURE FIX THIS
            // Not sure what the best way to handle this is. 2 ideas:
            // 1. Handle it like PT_SYSTIME where the two parts of the number are
            //      separated by space or tab
            // 2. Try to read in the whole number and if it overflows the low part
            //      then start on the high part

/*
            if ( lpszF = strtok(lpsz, ":") )
            {
                if ( cVals = strtoul(lpszF, NULL, 0) )
                {
                    lpspv->Value.MVli.cValues = cVals;

                    // Allocate array
                    lpspv->Value.MVli.lpli = (LARGE_INTEGER FAR *)PvAllocMore(
                        sizeof(LARGE_INTEGER) * cVals, lpv);

                    for (i = 0; i < cVals; i++)
                    {
                        if ( lpszF = strtok(NULL, ";") )
                            lpspv->Value.MVli.lpli[i] = (LARGE_INTEGER)atof(lpszF);
                        else
                            goto Cleanup;
                    }
                }
                else
                    goto Cleanup;
            }
*/
            break;


        case PT_UNSPECIFIED:
        case PT_NULL:
        case PT_OBJECT:
        default:
            lpspv->Value.x = 0L;
            break;
    }

    fRet = TRUE;


Cleanup:

    return(fRet);

}


/*
 -  CopyGuid
 -
 *  Purpose:
 *      Copies the contents of one GUID into another.
 *
 *  Parameters:
 *      lpg1        - Pointer to destination GUID
 *      lpg2        - Pointer to source GUID
 *
 *  Returns:
 *      Void.
 *
 */

void CopyGuid(LPGUID lpg1, LPGUID lpg2)
{
    int i = 0;

    if(!lpg1 || !lpg2)
        return;

    lpg1->Data1 = lpg2->Data1;
    lpg1->Data2 = lpg2->Data2;
    lpg1->Data3 = lpg2->Data3;

    for(; i < 8; i++)
        lpg1->Data4[i] = lpg2->Data4[i];

    return;
}



/*
 -  FreeRowSet(LPSRowSet)
 -
 *  Purpose:
 *      Free all the rows in a rowset then the rowset itself
 *
 *  Parameters:
 *      lpRows    - pointer to rows to free
 *
 *  Returns:
 *      BOOL Pass/Fail  - Pass == 1 FAIL == 0
 *
 */

BOOL FreeRowSet(LPSRowSet lpRows)
{
    ULONG   n;
    BOOL    fRet    = FALSE;

    if(lpRows)
    {
        for( n = 0 ; n < lpRows->cRows ; n++)
        {
            if( MAPIFreeBuffer(lpRows->aRow[n].lpProps ) )
            {
                goto end;
            }
        }
        if( MAPIFreeBuffer(lpRows) )
        {
            goto end;
        }
    }
    fRet = TRUE;
end:
    return(fRet);
}

/***********************************************************************
 *  Purpose:
 *      Determine if Prop Tag is an EntryID
 *
 *  Parameters:
 *      [in]
 *          ulPropTag   - PropTag to determine if entryid or not
 *
 *  Returns:
 *      TRUE it is an entryid, false otherwise
 *
 ***********************************************************************/

BOOL    FIsEntryID(ULONG ulPropTag)
{
    if(ulPropTag == PR_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_STORE_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_COMMON_VIEWS_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_FINDER_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_HEADER_FOLDER_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_IDENTITY_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_IPM_OUTBOX_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_IPM_SENTMAIL_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_IPM_SUBTREE_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_IPM_WASTEBASKET_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_ORIGINAL_AUTHOR_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_ORIGINAL_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_ORIGINAL_SENDER_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_ORIGINAL_SENT_REPRESENTING_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_OWN_STORE_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_PARENT_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_RCVD_REPRESENTING_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_READ_RECEIPT_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_RECEIVED_BY_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_REPORT_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_SENDER_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_SENT_REPRESENTING_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_SENTMAIL_ENTRYID)    
        return(TRUE);

    if(ulPropTag == PR_VIEWS_ENTRYID)    
        return(TRUE);


    return(FALSE);
                
}



static char szBinaryTestData[] = "0xDD 0xEE 0xAA 0xFF";
#define BINARY_TEST_FLAG_SIZE      4


/***********************************************************************
 *
 -  WritePropValArray
 -
 *  Purpose:
 *      Store a LPSPropValue Array in a file under a tag specified
 *
 *  Parameters:
 *      [in]
 *       lpszFilename   - name of the file to store prop value in
 *       lpszTag        - name of Tag Identifier in file delimited by [ ]
 *                          ex.  [FLD0001]
 *       cValues        - number of prop values in array
 *       lpspva         - prop value array to store in text format in file
 *       ulFlags        - used for determining things like whether to
 *                          dump actual binary data or not.(see enumerations
 *                          above.  i.e. DUMP_BINARY_DATA.
 *
 *  Returns:
 *      nothing
 *
 *    #define DUMP_BINARY_DATA        0x0001
 *    #define WRITE_NEW_DATA_ONLY     0x0010
 *
 ***********************************************************************/

// size of szBuffer for each data item to write to file
#define BUFFSIZE          4096


void WritePropValArray( LPSTR           lpszFileName,
                        LPSTR           lpszTag,
                        ULONG           cValues,
                        LPSPropValue    lpspva,
                        ULONG           ulFlags)    // defaults to 0 if not given
{
    ULONG   idx;
    char    szBuffer[BUFFSIZE];
    char    szLine1[2048];  // limitation of 2k of prop tags
    char    szLine2[20000];  // limitation of 12k of prop data, now has binary data
    FILE    *pFile              = NULL;
    int     len1                = 0;
    int     len2                = 0;


    // assert valid parameters
    if((lpszFileName == NULL) || (cValues == 0) || (lpszTag == NULL))
        return;

    // if set, only write data that has unique tag...ie. one that is not
    // already in the file.  If if exists in file already, then don't write data
    if( ulFlags & WRITE_NEW_DATA_ONLY)    
    {
        // if tag already in file, don't write anything now.
        pFile = fopen(lpszFileName, "r+");
        if (pFile == NULL)
            goto WriteInFile;
            
        // if this tag already in data file, don't write new data 
        if ( FSeekTag(pFile, lpszTag) )
        {
            fprintf(pFile,"Tag %s FOUND IN file %s\n",lpszTag,lpszFileName);
            fclose(pFile);
            return;
        }

        if (pFile)
        {
            fclose(pFile);
            pFile = NULL;
        }

    }

WriteInFile:

    // else not in file, write it out
    
    // determine within range of arrays

    // these are initialized so strcat works correctly
    szBuffer[0] = '\0';
    szLine1[0]  = '\0';
    szLine2[0]  = '\0';

    // in case the first data is a empty string, we need to 
    // buffer with one space for special case because read routines
    // will not parse a beginning comma.
    strcat(szLine2," ");

    // for every value in lpspva, convert to correct format to put in file
    for(idx = 0; idx < cValues ; idx++)
    {
        // get Prop tag, put in string for 1st line
        // append ", " to end of prop tag unless it's the last
        if( !GetString("PropIDs",PROP_ID(lpspva[idx].ulPropTag) , szBuffer) )
            wsprintf(szBuffer,"0x%08X",lpspva[idx].ulPropTag);      // id not found, use hex tag

        strcat(szLine1,szBuffer);
        strcat(szLine1,", ");

        // if error, then flag for Read routine later with <PT_ERROR>
        if(PROP_TYPE(lpspva[idx].ulPropTag) == PT_ERROR)
            strcat(szLine2,"<PT_ERROR>");
        

        // USUALLY PT_BINARY are like PR_ENTRYIDs and not
        //   comparsable byte for byte, so merely store tag
        //   and dummy data that is a flag it came from my
        //   store routine, and in the comparison, we need
        //   to look for this flag as a condition that we shouldn't
        //   campare byte for byte the PT_BINARY data

        if( PROP_TYPE(lpspva[idx].ulPropTag) == PT_BINARY)
        {
            // dump dummy stub value 0xDD 0xEE 0xAA 0xFF
            // OR dump actual binary data
            // in following format:  0x56 0x44 0x00 0x02 0x33
            // OR data is too big to log all of it > 204 == 1024/5          
            if( (!(ulFlags & DUMP_BINARY_DATA)) || (FIsEntryID(lpspva[idx].ulPropTag)) || (lpspva[idx].Value.bin.cb > (BUFFSIZE/BINARY_CHAR_SIZE)) )
            {
                wsprintf(szBuffer,"%s",szBinaryTestData);
            }
            else  // we want to write the binary data to file
            {
                char    lpszHex[9];
                ULONG   cb              = 0;
                int     cChars          = 0;
                LPBYTE  lpb             = lpspva[idx].Value.bin.lpb;

                szBuffer[0]             = '\0';

                // while still bytes to copy and within buffer limit    
                // buffer limit == szBuffer == 1024 bytes, 5 characters per byte of
                // data, our limit == 1024 / 5 == 204 bytes stored in file per property
                // if it is larger than 204 bytes, we should log 0xDD 0xEE 0xAA 0xFF
                while((cb < lpspva[idx].Value.bin.cb) && (cChars < BUFFSIZE ) )
                {
                    wsprintf(lpszHex, " 0x%02X", *lpb);
                    lstrcat(szBuffer, lpszHex);
                    cChars += BINARY_CHAR_SIZE;   // looks like 0x34 0x44 0x55, each taking space, 0, x, 2 bytes data == 5 spaces
                    lpb++;
                    cb++;
                }
            }
        }
        else
        {
            if( !SzGetPropValue(szBuffer, &(lpspva[idx]) ) )
                return;
        }
        strcat(szLine2,szBuffer);
        strcat(szLine2,", ");

    }

    // take off the last comma and space from both lines
    len1 = lstrlen(szLine1);
    szLine1[len1 - 2] = '\0';

    len2 = lstrlen(szLine2);
    szLine2[len2 - 2] = '\0';

    pFile = fopen(lpszFileName, "a");
    if (pFile == NULL)
        return;

    // fprintf Tag, proptags, propdata
    fprintf(pFile,"%s\n",lpszTag);
    fprintf(pFile,"%s\n",szLine1);
    fprintf(pFile,"%s\n",szLine2);
    fprintf(pFile,"\n");

    if (pFile)
        fclose(pFile);



}



/***********************************************************************
 *
 -  WritePropTagArray
 -
 *  Purpose:
 *      Store a LPSPropTagArray in a file under a tag specified
 *
 *  Parameters:
 *      [in]
 *       lpszFilename   - name of the file to store prop value in
 *       lpszTag        - name of Tag Identifier in file delimited by [ ]
 *                          ex.  [FLD0001]
 *       lpspva         - prop tag array to store in text format in file
 *
 *  Returns:
 *      nothing
 *
 *  Example:
 *      StorePropTag("C:\\MAPITEST\\DATA\\PROPVU.TXT",
 *                     "[FLD00001]"
 *                     lpsPTA);
 *
 ***********************************************************************/

void WritePropTagArray( LPSTR           lpszFileName,
                        LPSTR           lpszTag,
                        LPSPropTagArray lpspta)
{
    ULONG   idx;
    char    szBuffer[256];
    char    szLine1[2048];  // limitation of 2k of prop tags
    FILE    *pFile              = NULL;
    int     len1                = 0;

    // assert valid parameters
    if((lpszFileName == NULL) || (lpspta == NULL) || (lpszTag == NULL))
            return;

    // these are initialized so strcat works correctly
    szBuffer[0] = '\0';
    szLine1[0]  = '\0';

    // for every value in lpspva, convert to correct format to put in file
    for(idx = 0; idx < lpspta->cValues ; idx++)
    {
        // get Prop tag, put in string for 1st line
        // append ", " to end of prop tag unless it's the last
        if( !GetString("PropIDs",PROP_ID(lpspta->aulPropTag[idx]) , szBuffer) )
            wsprintf(szBuffer,"0x%08X",lpspta->aulPropTag[idx]);        // id not found, use hex
        strcat(szLine1,szBuffer);
        strcat(szLine1,", ");
    }

    // take off the last comma and space from both lines
    len1 = lstrlen(szLine1);
    szLine1[len1 - 2] = '\0';

    pFile = fopen(lpszFileName, "a");
    if (pFile == NULL)
        return;

    // fprintf Tag, proptags, propdata
    fprintf(pFile,"%s\n",lpszTag);
    fprintf(pFile,"%s\n",szLine1);
    fprintf(pFile,"\n");

    if (pFile)
        fclose(pFile);

}


/***********************************************************************
 *
 -  WriteProblemArray
 -
 *  Purpose:
 *      Store a LPSPropProblemArray in a file under a tag specified
 *
 *  Parameters:
 *      [in]
 *       lpszFilename   - name of the file to store prop value in
 *       lpszTag        - name of Tag Identifier in file delimited by [ ]
 *                          ex.  [FLD0001]
 *       lpProblem      - problem array to put in file lpszFilename under lpszTag
 *
 *  Returns:
 *      nothing
 *
 *  Example:
 *      StorePropValue("C:\\MAPITEST\\DATA\\PROPVU.TXT",
 *                     "[FLD00001]"
 *                     lpProblem);
 *
 ***********************************************************************/

void WriteProblemArray( LPSTR               lpszFileName,
                        LPSTR               lpszTag,
                        LPSPropProblemArray lpProblem)
{
    ULONG   idx;
    char    szBuffer[256];
    char    szLine1[256];   // limitation of 256 for indexes
    char    szLine2[2048];  // limitation of 2k for proptags
    char    szLine3[2048];  // limitation of 2k for error codes
    FILE    *pFile              = NULL;
    int     len1                = 0;
    int     len2                = 0;
    int     len3                = 0;

    // assert valid parameters
    if((lpszFileName == NULL) || (lpProblem == NULL) || (lpszTag == NULL))
        return;

    // these are initialized so strcat works correctly
    szBuffer[0] = '\0';
    szLine1[0]  = '\0';
    szLine2[0]  = '\0';
    szLine3[0]  = '\0';

    // for every value in lpspva, convert to correct format to put in file
    for(idx = 0; idx < lpProblem->cProblem ; idx++)
    {
        // INDEX
        wsprintf(szBuffer,"%lu, ",lpProblem->aProblem[idx].ulIndex);
        strcat(szLine1,szBuffer);

        // PROPTAG
        if( !GetString("PropIDs",PROP_ID(lpProblem->aProblem[idx].ulPropTag) , szBuffer) )
            wsprintf(szBuffer,"0x%08X",lpProblem->aProblem[idx].ulPropTag); // id not found, use hex tag

        strcat(szLine2,szBuffer);
        strcat(szLine2,", ");


        // MAPI ERROR
        GetString("MAPIErrors",lpProblem->aProblem[idx].scode , szBuffer);
        strcat(szLine3,szBuffer);
        strcat(szLine3,", ");
    }

    // take off the last comma and space from both lines
    len1 = lstrlen(szLine1);
    szLine1[len1 - 2] = '\0';

    len2 = lstrlen(szLine2);
    szLine2[len2 - 2] = '\0';

    len3 = lstrlen(szLine3);
    szLine3[len3 - 2] = '\0';

    pFile = fopen(lpszFileName, "a");
    if (pFile == NULL)
        return;

    // fprintf Tag, proptags, propdata
    fprintf(pFile,"%s\n",lpszTag);
    fprintf(pFile,"%s\n",szLine1);
    fprintf(pFile,"%s\n",szLine2);
    fprintf(pFile,"%s\n",szLine3);
    fprintf(pFile,"\n");

    if (pFile)
        fclose(pFile);
}



/*******************************************************************
 *
 -  GetLPIID
 -
 *  Purpose:
 *      Returns the appropiate interface ID for the type of object that
 *      is passed in.
 *
 *  Parameters:
 *      lpObj   - object pointer to get LPIID for
 *
 *  Comments:
 *      Must call PvFree on the LPIID returned once you're through with it.
 *
 *  Returns:
 *      LPIID - a pointer to the interface ID, NULL if not successful
 *
 *******************************************************************/

LPIID GetLPIID( LPMAPIPROP  lpObj )
{
    LPSPropTagArray     lpPropTagArray      = NULL;
    LPSPropValue        lpPropValArray      = NULL;
    LPIID               lpTmpIID            = NULL;
    ULONG               cValues;

    // Get the object type
    lpPropTagArray = (LPSPropTagArray) PvAlloc(
                                  sizeof(LPSPropTagArray) +sizeof(ULONG));
    lpPropTagArray->cValues = 1;
    lpPropTagArray->aulPropTag[0] = PR_OBJECT_TYPE;
    #ifdef MAPI_IF_CPP
    if ( lpObj->GetProps( lpPropTagArray, 0, &cValues, &lpPropValArray) )
    #else
    if ( lpObj->lpVtbl->GetProps( lpObj, lpPropTagArray, 0, &cValues, &lpPropValArray) )
    #endif
        goto Cleanup;

    lpTmpIID = (LPIID) PvAlloc(sizeof(IID));

    // Switch based on the object type
    switch(lpPropValArray->Value.l)
    {
        case MAPI_STORE:                // Message Store
            *lpTmpIID = IID_IMsgStore;
            break;

        case MAPI_ADDRBOOK:             // Address Book
            *lpTmpIID = IID_IAddrBook;
            break;

        case MAPI_FOLDER:               // Folder
            *lpTmpIID = IID_IMAPIFolder;
            break;

        case MAPI_ABCONT:               // Address Book Container
            *lpTmpIID = IID_IABContainer;
            break;

        case MAPI_MESSAGE:              // Message
            *lpTmpIID = IID_IMessage;
            break;

        case MAPI_MAILUSER:             // Individual Recipient
            *lpTmpIID = IID_IMailUser;
            break;

        case MAPI_ATTACH:               // Attachment
            *lpTmpIID = IID_IAttachment;
            break;

        case MAPI_DISTLIST:             // Distribution List Recipient
            *lpTmpIID = IID_IDistList;
            break;

        case MAPI_STATUS:               // Status
            *lpTmpIID = IID_IMAPIStatus;
            break;

        case MAPI_SESSION:              // Session
            *lpTmpIID = IID_IMAPISession;
            break;

        default:                        // Default to unknown
            *lpTmpIID = IID_IUnknown;
    }


Cleanup:
    if (lpPropTagArray)
        PvFree(lpPropTagArray);

    if (lpPropValArray)
        MAPIFreeBuffer(lpPropValArray);

    return(lpTmpIID);
}


/*******************************************************************
 *
 -  String8ToUnicode
 -
 *  Purpose:
 *      Converts a String8 string to a Unicode double byte
 *      string.  Use PvFree to free memory allocated for
 *      the Unicode string lppWideChar.  Since UNICODE support
 *      does not currently exist on 16 bit Windows, this function
 *      is only compiled on 32 bit version
 *
 *  Parameters:
 *      szMultiByte     - String8 string to comvert to Unicode
 *      lppWideChar     - address of pointer that takes the converted
 *                          string in Unicode form.  Free this
 *                          memory with PvFree();
 *      lpvParent       - parent to chain memory to, if NULL then
 *                          it doesn't chain the memory
 *  Returns:
 *      void
 *
 *******************************************************************/

void String8ToUnicode( LPSTR        szMultiByte,
                       LPWSTR       *lppWideChar,
                       void FAR     *lpvParent)     // = NULL if not given
{
    int             dBytes          = 0;
    int             dCopiedBytes    = 0;
    DWORD           dwRet           = 0;
    int             dBufferLen      = 0;
    int i = 0;

    if(szMultiByte)
        dBufferLen = lstrlen(szMultiByte) + 1;     // the number of bytes plus the null char

#ifdef WIN32
    // determines the number of Wide char bytes requried
    dBytes = MultiByteToWideChar(   (UINT)      CP_ACP,
                                    (DWORD)     0,
                                    (LPCTSTR)   szMultiByte,
                                    (int)       dBufferLen,
                                    (LPWSTR)    NULL,
                                    (int)       0);

    if (lpvParent)
        *lppWideChar = (LPWSTR) PvAllocMore( (dBytes * sizeof(WCHAR)), lpvParent );
    else
        *lppWideChar = (LPWSTR) PvAlloc( dBytes * sizeof(WCHAR) );

    // now actually convert the STRING8 into a UNICODE
    dCopiedBytes = MultiByteToWideChar( (UINT)      CP_ACP,
                                        (DWORD)     0,
                                        (LPCTSTR)   szMultiByte,
                                        (int)       dBufferLen,
                                        (LPWSTR)    *lppWideChar,
                                        (int)       dBytes );

    dwRet = GetLastError();

#else // WIN16
    // This is a hack for 16bit. Some MAPI calls such as GetIDsFromNames take
    // unicode strings in both 32bit & 16bit versions. Under 16 bit it just
    // treats the string as an array of WORD (WORD = 2 bytes). So here I'm
    // just promoting each byte of the string8 to a WORD. This will work for
    // English, but probably won't work for other languages. There are no
    // routines yet to do this for other languages that I know of.
    dBytes = sizeof(WCHAR) * dBufferLen;

    if (lpvParent)
        *lppWideChar = (LPWSTR) PvAllocMore( dBytes, lpvParent );
    else
        *lppWideChar = (LPWSTR) PvAlloc( dBytes );

    for (i = 0; i < dBufferLen; i++ )
    {
        *lppWideChar[i] = (WCHAR) szMultiByte[i];
    }
#endif
}



/*******************************************************************/
/*
 -  AsciiToHex
 -
 *  Purpose:
 *      Converts ascii string in hex format to hex number
 *
 *  Parameters:
 *      len             - length of ascii string to convert
 *      pch             - pointer to array of characters to convert
 *      &lpulOut        - hex string returned to calling app
 *
 *  Returns:
 *      BOOLEAN         - TRUE if successful, FALSE if fail conversion
 *
 */
/*******************************************************************/

BOOL PASCAL AsciiToHex (ULONG len, char *pch, ULONG * lpulOut)
{
    char ch;
    ULONG i;

    for (i = len; i > 0; i--)
    {
        ch = pch[i - 1];

        if (ch >= 'a' && ch <= 'f')

            ch = 10 + (ch - 'a');

        else if (ch >= 'A' && ch <= 'F')

            ch = 10 + (ch - 'A');

        else if (ch >= '0' && ch <= '9')

            ch = ch - '0';
        else
            return FALSE;

        *lpulOut += ((ULONG) ch) << (4 * (len - i));
    }
    return TRUE;
}



/*
 -  SzGetPropTag
 -
 *  Purpose:
 *      Converts a Property Tag to a 'Type' label and an ID value
 *      and stores the result in a character array.
 *
 *  Parameters:
 *      lpsz        - pointer to character string
 *      ulPropTag   - a property tag to be converted
 *
 *  Returns:
 *      LPSTR       - return the address of the destination string
 *
 */

LPSTR SzGetPropTag(LPSTR lpsz, ULONG ulPropTag)
{
    char rgch[32];      // Assumes no PropType string longer than 31 chars
    char rgch1[50];


    wsprintf(lpsz,"\t");
    GetString("PropIDs", PROP_ID(ulPropTag), rgch1);

    lstrcat(lpsz, rgch1);

    lstrcat(lpsz, " :\t");

    if( GetString("PropType", PROP_TYPE(ulPropTag), rgch) )
    {
        lstrcat(lpsz, rgch);
    }
    else
    {
        lstrcat(lpsz, "not found");
    }

    lstrcat(lpsz, "\t");

    return lpsz;
}



/*
 -  SzTextizeProp
 *
 *  Purpose:
 *      Textize the property tag and value of a property together
 *      in one string.
 *
 *  Arguments:
 *      lpszRet         - Allocated buffer to fill in.
 *      lpProp          - Pointer to property value to textize.
 *
 *  Returns:
 *      lpszRet         - Pointer to beginning of string in lpszRet.
 */

LPSTR SzTextizeProp(LPSTR lpszRet, LPSPropValue lpProp)
{
    char rgch[256];

    SzGetPropTag(lpszRet, lpProp->ulPropTag);
    lstrcat(lpszRet, " = " );
    return lstrcat(lpszRet, SzGetPropValue(rgch, lpProp));
}



/*
 -  MakeGuid
 -
 *  Purpose:
 *      Construct a GUID from the parameters passed in.
 *
 *  Parameters:
 *      lpg         - Pointer to GUID to assign
 *      ul          - Data1
 *      w1          - Data2
 *      w2          - Data3
 *      b1 - b8     - Data4[0] - Data4[7]
 *
 *  Returns:
 *      Void.
 *
 */

void MakeGuid(LPGUID lpg, ULONG ul, WORD w1, WORD w2, BYTE b1,
                                                      BYTE b2,
                                                      BYTE b3,
                                                      BYTE b4,
                                                      BYTE b5,
                                                      BYTE b6,
                                                      BYTE b7,
                                                      BYTE b8)
{
    if(!lpg)
        return;

    lpg->Data1 = ul;
    lpg->Data2 = w1;
    lpg->Data3 = w2;
    lpg->Data4[0] = b1;
    lpg->Data4[1] = b2;
    lpg->Data4[2] = b3;
    lpg->Data4[3] = b4;
    lpg->Data4[4] = b5;
    lpg->Data4[5] = b6;
    lpg->Data4[6] = b7;
    lpg->Data4[7] = b8;
}



/***********************************************************************
 -  FSeekTag
 -
 *  Purpose:
 *      Looks for the tag string in the given text file which should already
 *      be open. It positions the file pointer to the next line after the
 *      tag string.
 *
 *  Parameters:
 *      [in]
 *       pFile          - pointer to open file, opened with fopen
 *       lpszTag        - string to search for
 *
 *  Returns:
 *      TRUE if successful, FALSE if not
 *
 *  Example:
 *      FSeekTag( pFile, "[FLDTREE_TEMPLATE]" )
 *
 ***********************************************************************/

BOOL FSeekTag( FILE *pFile, LPTSTR lpszTag )
{
    char szBuffer[MAX_PATH];

    // seek to beginning of file, so order in file of Blocks/Tags is
    //   not important
    //   fseek returns 0 for success, nonzero for fail
    //   seek to beginning and 0 bytes offset from beginning
    if( fseek(pFile,0,SEEK_SET) )
        return(FALSE);

    while ( !feof( pFile ) )
    {
        // Get the next line
        if ( fgets( szBuffer, MAX_PATH, pFile ) == NULL )
            return(FALSE);

        // Check if this line contains lpszTag
        else if ( strstr( szBuffer, lpszTag ) != NULL)
            return(TRUE);
    }

    // Didn't find the tag
    return(FALSE);
}
