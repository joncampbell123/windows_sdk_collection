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

// ---------------------------------------------------------------------------
// banner.c
// 
// ---------------------------------------------------------------------------

#include <windows.h>
#include <winspool.h>
#include <winsplp.h>

#include "local.h"
#include "winprint.h"

#define DI_DIRECT   0x00010000

// text out records with these strings are replaced with job/name/date values
char szParamJob[]  = "%1%";
char szParamName[] = "%2%";
char szParamDate[] = "%3%";

#define Absolute(i) (i < 0 ? -i : i)

// ---------------------------------------------------------------------------
// Compound metafile data definitions.
#pragma pack(1)

#define  ALDUSKEY        0x9AC6CDD7
#define  CLP_ID          0xC350

// placeable metafile data definitions 
typedef struct tagRECT16 {
    short   left;
    short   top;
    short   right;
    short   bottom;
} RECT16;

// placeable metafile header 
typedef struct {
        DWORD   key;
        WORD	hmf;
        RECT16	bbox;
        WORD    inch;
        DWORD   reserved;
        WORD    checksum;
}ALDUSMFHEADER;

// Clipboard file header.
typedef struct {
        WORD FileIdentifier;
        WORD FormatCount;
} CLIPFILEHEADER;
typedef CLIPFILEHEADER FAR* LPCLIPFILEHEADER;

// Clipboard format header.
typedef struct {
        WORD  FormatID;
        DWORD DataLen;
        DWORD DataOffset;
        char  Name[79];
} CLIPFILEFORMAT;
typedef CLIPFILEFORMAT FAR* LPCLIPFILEFORMAT;

typedef METAHEADER FAR* LPMETAHEADER;

// 16 bit metafile pict structure
typedef struct tagMETAFILEPICT16
{
    SHORT       mm;
    SHORT       xExt;
    SHORT       yExt;
    WORD        hMF;
} METAFILEPICT16, FAR *LPMETAFILEPICT16;

#pragma pack()
// ---------------------------------------------------------------------------


// If the file is a clipboard file with a metafile in it then return TRUE
// and fill out the clip header structure.
BOOL
IsClipboardMeta(
    HFILE fh,
    CLIPFILEFORMAT *ClipHeader
)
{
    WORD i;
    CLIPFILEHEADER FileHeader;
    DWORD HeaderPos;

    _llseek(fh, 0, 0);

    /* read the clipboard file header */
    FileHeader.FormatCount = 0;
    _lread(fh, (LPSTR)&FileHeader, sizeof(CLIPFILEHEADER));

    if (FileHeader.FileIdentifier != CLP_ID)  
        return (FALSE);

    HeaderPos = sizeof(CLIPFILEHEADER);

    // Search the formats contained within the clipboard file looking
    // for a metafile.  Break if and when it is found.

    for (i=0; i < FileHeader.FormatCount; i++)  
        {
        _llseek(fh, HeaderPos, 0);

        // Read the clipboard header found at current position.
        if(_lread(fh, (LPSTR)ClipHeader, sizeof(CLIPFILEFORMAT)) < sizeof(ClipHeader))  
            return (FALSE);

        HeaderPos += sizeof(CLIPFILEFORMAT);

        // If a metafile was found break */
        if (ClipHeader->FormatID == CF_METAFILEPICT)
            break;
        }

    // Was it really so?
    if (ClipHeader->FormatID == CF_METAFILEPICT)
        return (TRUE);
    else
        return (FALSE);
}


// Get a metfile from a clipboard file given a file handle and a clipheader
// (which specifies where in the file to get the metafile from) and fills
// out a metafilepict structure.
HMETAFILE
GetClipMetaFile(
    HFILE fh,
    CLIPFILEFORMAT *ClipHeader,
    LPMETAFILEPICT lpMFP
)
{
    HGLOBAL hMem;
    LPSTR lpMem;
    WORD wBytesRead;
    WORD nSize;
    LONG lOffset;
    HMETAFILE hMF;
    METAHEADER mfHeader;

    // Allocate enough memory to read the metafile bits into.
    if (!(hMem = GlobalAlloc(GHND, ClipHeader->DataLen - sizeof(METAFILEPICT))))
        return(0);

    if (!(lpMem = GlobalLock(hMem)))  
        {
        GlobalFree(hMem);
        return(0);
        }

    // Offset to the metafile bits.
    lOffset = ClipHeader->DataOffset + sizeof(METAFILEPICT);

    nSize = (WORD)(ClipHeader->DataLen - sizeof(METAFILEPICT));

    // Seek to the beginning of the metafile bits.
    _llseek(fh, lOffset, 0);

    // read the metafile bits.
    wBytesRead = _lread(fh, lpMem, nSize);

    // if unable to read the metafile bits return.
    if (wBytesRead == -1 || wBytesRead < nSize) 
        {
        GlobalUnlock(hMem);
        GlobalFree(hMem);
        return(0);
        }

    _llseek(fh, lOffset, 0);

    // Read the metafile header.
    wBytesRead = _lread(fh, (LPSTR)&mfHeader, sizeof(METAHEADER));

    // If unable to read the header return.
    if (wBytesRead == -1 || wBytesRead < sizeof(METAHEADER))
    {
FreeAndFail:
        GlobalUnlock(hMem);
        GlobalFree(hMem);
        return(0);
    }

    // set the metafile bits to the memory allocated for that purpose.
    if (NULL == (hMF = SetMetaFileBitsEx(ClipHeader->DataLen - sizeof(METAFILEPICT), lpMem)))
    {
        goto FreeAndFail;
    }

    GlobalUnlock(hMem);

    // Reposition to the start of the METAFILEPICT header.
    _llseek(fh, ClipHeader->DataOffset, 0);

    // Read the metafile pict structure.
    wBytesRead = _lread(fh, (LPSTR)lpMFP, sizeof(METAFILEPICT));

    if( wBytesRead == -1 || wBytesRead < sizeof(METAFILEPICT) )  
        goto FreeAndFail;

    // Update metafile handle.
    lpMFP->hMF = hMF;

    return(hMF);
}


// ---------------------------------------------------------------------------
// Returns TRUE if this the file is a placeable metafile, FALSE if it
// isn't or there's some error.
BOOL
IsPlaceableMeta(
    HFILE fh
)
{
    UINT uiBytesRead;
    DWORD dwIsAldus;

    _llseek(fh, 0, 0);

    // Read the first dword of the file to see if it is a placeable wmf.
    uiBytesRead = _lread(fh, (LPSTR)&dwIsAldus, sizeof(dwIsAldus));

    if ((uiBytesRead == HFILE_ERROR) || (uiBytesRead < sizeof(dwIsAldus)))
        return FALSE;

    if (dwIsAldus != ALDUSKEY)  
        return FALSE;
    else  
        return TRUE;
}

// ---------------------------------------------------------------------------
// Returns a handle to a metafile given a placeable metafile and fills out
// the aldusmfh stucture.
HMETAFILE
GetPlaceableMetaFile(
    HFILE fh,
    ALDUSMFHEADER *aldusMFHeader
)
{
    HGLOBAL hMem;
    LPSTR lpMem;
    UINT uiBytesRead;
    HMETAFILE hMF;
    METAHEADER mfHeader;

    _llseek(fh, 0, 0);

    // Read the placeable header.
    uiBytesRead = _lread(fh, (LPSTR)aldusMFHeader, sizeof(ALDUSMFHEADER));

    if ((uiBytesRead == HFILE_ERROR) || (uiBytesRead < sizeof(ALDUSMFHEADER)))
        return 0;

    // Return to read metafile header.
    _llseek(fh, sizeof(ALDUSMFHEADER), 0);

    // Read the metafile header.
    uiBytesRead = _lread(fh, (LPSTR)&mfHeader, sizeof(METAHEADER));

    if ((uiBytesRead == HFILE_ERROR) || (uiBytesRead < sizeof(METAHEADER)))
        return 0;

    // Allocate memory for the metafile bits.
    if (!(hMem = GlobalAlloc(GHND, (mfHeader.mtSize * 2))))
        return 0;

    // Lock the memory.
    if (!(lpMem = GlobalLock(hMem)))
    {
        GlobalFree(hMem);
        return 0;
    }

    // Seek to the metafile bits.
    _llseek(fh, sizeof(ALDUSMFHEADER), 0);

    // Read metafile bits.
    uiBytesRead = _lread(fh, lpMem, (mfHeader.mtSize * 2));

    if (uiBytesRead == HFILE_ERROR)
    {
        GlobalUnlock(hMem);
        GlobalFree(hMem);
        return 0;
    }

    // Set the metafile bits to the memory that we allocated.
    if (!(hMF = SetMetaFileBitsEx(mfHeader.mtSize * 2, lpMem)))
    {
        GlobalUnlock(hMem);
        GlobalFree(hMem);	  
        return 0;
    }

    GlobalUnlock(hMem);

    return hMF;
}

// ---------------------------------------------------------------------------
// Sets the extents for the given DC according to the contents of an
// aldus metafile header.
// ---------------------------------------------------------------------------
void
SetPlaceableExtents(
    HDC hDC,
    ALDUSMFHEADER *amfh
)
{
    POINT point;
    SIZE size;

    SetMapMode(hDC, MM_ANISOTROPIC);

    // Set the windows origin to correspond to the bounding box origin
    // contained in the placeable header.
    SetWindowOrgEx(hDC, amfh->bbox.left, amfh->bbox.top, &point);

    // set the window extents based on the abs value of the bbox coords.
    SetWindowExtEx(hDC,Absolute(amfh->bbox.left) + Absolute(amfh->bbox.right),
        Absolute(amfh->bbox.top) + Absolute(amfh->bbox.bottom), &size);

    // Set the viewport origin and extents.
    SetViewportOrgEx(hDC, 0, 0, &point);
    SetViewportExtEx(hDC,GetDeviceCaps(hDC, HORZRES), GetDeviceCaps(hDC, VERTRES), &size);
}


// ---------------------------------------------------------------------------
// SystemToLocalTime
//
// ---------------------------------------------------------------------------
void
SystemTimeString(
    LPSYSTEMTIME pst,
    LPSTR pszText
)
{
    FILETIME ftUTC, ftLocal;
    SYSTEMTIME st;
    char szTmp[64];

    // Convert from UTC to Local time
    SystemTimeToFileTime(pst, &ftUTC);
    FileTimeToLocalFileTime(&ftUTC, &ftLocal);
    FileTimeToSystemTime(&ftLocal, &st);

    // Generate string
    GetDateFormatA(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, NULL, pszText, sizeof(szTmp));
    lstrcat(pszText, " ");
    GetTimeFormatA(LOCALE_USER_DEFAULT, 0, &st, NULL, szTmp, sizeof(szTmp));
    lstrcat(pszText, szTmp);
}


// ---------------------------------------------------------------------------
// EnumMetaFileProc
//
// This is the call back proc when we enum our full banner page to patch
// the user name and stuff
// ---------------------------------------------------------------------------
int CALLBACK
EnumMetaFileProc(
    HDC hdc,
    HANDLETABLE FAR* lpHTable,
    METARECORD FAR* lpMFR,
    int cObj,
    LPARAM lpClientData
)
{
    char szBuf[128];
    PJOB_INFO_2 pJobInfo;

    if (lpMFR->rdFunction == META_TEXTOUT)
    {
        pJobInfo = (PJOB_INFO_2)lpClientData;

        // Replace %1% with the job name.
        if (lstrcmp((LPSTR) &lpMFR->rdParm[1], szParamJob) == 0)
        {
            TextOut(hdc, lpMFR->rdParm[lpMFR->rdSize-4], lpMFR->rdParm[lpMFR->rdSize-5],
                pJobInfo->pDocument, lstrlen(pJobInfo->pDocument));
        }
        // Replace %2% with the user name.
        else if (lstrcmp((LPSTR) &lpMFR->rdParm[1], szParamName) == 0)
        {
            TextOut(hdc, lpMFR->rdParm[lpMFR->rdSize-4], lpMFR->rdParm[lpMFR->rdSize-5],
                pJobInfo->pUserName, lstrlen(pJobInfo->pUserName));
        }
        else if (lstrcmp((LPSTR) &lpMFR->rdParm[1], szParamDate) == 0)
        {
            SystemTimeString(&pJobInfo->Submitted, szBuf);

            TextOut(hdc, lpMFR->rdParm[lpMFR->rdSize-4], lpMFR->rdParm[lpMFR->rdSize-5],
                szBuf, lstrlen(szBuf));
        }                                                                             
        else
            PlayMetaFileRecord(hdc, lpHTable, lpMFR, cObj);
    }
    else if (lpMFR->rdFunction == META_EXTTEXTOUT)
    {
        LPSTR	lpch;
        LPRECT	lprt;

        pJobInfo = (PJOB_INFO_2)lpClientData;

        lprt = (lpMFR->rdParm[3] & (ETO_OPAQUE|ETO_CLIPPED)) ? (LPRECT)&lpMFR->rdParm[4] : 0;
        lpch = (LPSTR)&lpMFR->rdParm[4] + ((lprt) ?  sizeof(RECT) : 0);

        // Replace %1% with the job name.
        if (lstrcmp((LPSTR) lpch, szParamJob) == 0)
        {
            ExtTextOut(hdc, lpMFR->rdParm[1], lpMFR->rdParm[0],lpMFR->rdParm[3], lprt,
                pJobInfo->pDocument, lstrlen(pJobInfo->pDocument), NULL);
        }
        // Replace %2% with the user name.
        else if (lstrcmp((LPSTR) lpch, szParamName) == 0)
        {
            ExtTextOut(hdc, lpMFR->rdParm[1], lpMFR->rdParm[0],lpMFR->rdParm[3], lprt,
                pJobInfo->pUserName, lstrlen(pJobInfo->pUserName), NULL);
        }
        else if (lstrcmp((LPSTR) lpch, szParamDate) == 0)
        {
            SystemTimeString(&pJobInfo->Submitted, szBuf);

            ExtTextOut(hdc, lpMFR->rdParm[1], lpMFR->rdParm[0],lpMFR->rdParm[3], lprt,
                 szBuf, lstrlen(szBuf), NULL);
        }                                                                             
        else
            PlayMetaFileRecord(hdc, lpHTable, lpMFR, cObj);
    }
    else
        PlayMetaFileRecord(hdc, lpHTable, lpMFR, cObj);

    return 1;
}    


// ---------------------------------------------------------------------------
// Get a metafile from some Windows Metafile data in our resource
// ---------------------------------------------------------------------------
HMETAFILE
GetMetafileRes(
    LPMETAHEADER lpMetafile,
    LPMETAFILEPICT lpMFP
)
{
    HMETAFILE hMF;

    lpMFP->mm = MM_ANISOTROPIC;

    if (!(hMF = SetMetaFileBitsEx((lpMetafile->mtSize * 2), (LPBYTE)lpMetafile)))
        return (0);

    // Update metafile handle.
    lpMFP->hMF = hMF;

    return(hMF);
}


// Set the extents for the given DC according to the contents of a 
// metafilepict.
void
SetClipMetaExts(
    HDC hDC,
    METAFILEPICT *MFP
)
{
    POINT point;
    SIZE size;

    SetMapMode(hDC, MFP->mm);
    SetViewportOrgEx(hDC, 0, 0, &point);

    // Given the mapping mode specified in the metafilepict.
    switch (MFP->mm)  
    {
        case MM_ISOTROPIC:
            if (MFP->xExt && MFP->yExt)
                SetWindowExtEx(hDC, MFP->xExt, MFP->yExt, &size);

            // Fall through...

        case MM_TEXT:
        case MM_ANISOTROPIC:
            SetViewportExtEx(hDC, GetDeviceCaps(hDC, HORZRES), GetDeviceCaps(hDC, VERTRES), &size);
            break;

        default:
            break;
    }
}


BOOL
DrawFullBannerPage(
    HDC hdcPrn,
    PJOB_INFO_2 pJobInfo,
    LPSTR pSepFile
)
{
    HMETAFILE hMf;
    HGLOBAL hRes = 0;
    LPBYTE lpb;
    METAFILEPICT mfp;
    BOOL fReturn = TRUE;

    if (!pSepFile)
    {
        // Win32 note:
        // Both UnlockResource and FreeResource are obsolete, they don't
        // need to be called

        // Load from the resource.
        hRes = LoadResource(hInst, FindResource(hInst, MAKEINTRESOURCE(IDC_STANDBAN), MAKEINTRESOURCE(RT_CLIPFILE)));
        if (!(lpb = LockResource(hRes)))
            return FALSE;
        
        // The resource should be in Windows Metafile (.WMF) format...
        if (!(hMf = GetMetafileRes((LPMETAHEADER)lpb, &mfp)))
            return FALSE;

        SetClipMetaExts(hdcPrn, &mfp);
    }
    else
    {
        CLIPFILEFORMAT ClipHeader;
        ALDUSMFHEADER amfh;
        HFILE fh;

        // Load from a file.
        fh = _lopen(pSepFile, OF_READ);
        if (fh == -1)
            return FALSE;
    
        if (IsPlaceableMeta(fh))
        {
            hMf = GetPlaceableMetaFile(fh, &amfh);
            SetPlaceableExtents(hdcPrn, &amfh);
        }
        else if (IsClipboardMeta(fh, &ClipHeader))
        {
            hMf = GetClipMetaFile(fh, &ClipHeader, &mfp);
            SetClipMetaExts(hdcPrn, &mfp);
        }
        else 
        {
            // It's hard to check if the file is a valid metafile because
            // it doesn't have any magic bytes.
            hMf = GetMetaFile(pSepFile);

            // mfp.mm = MM_TEXT;
            mfp.mm = MM_ANISOTROPIC;

            SetClipMetaExts(hdcPrn, &mfp);
        }

        _lclose(fh);
    }

    if (!EnumMetaFile(hdcPrn, hMf, (MFENUMPROC)EnumMetaFileProc,
        (LPARAM)(LPSTR)pJobInfo))
    {
        // Not everything got played.
        fReturn = FALSE;
    }

    if (!hRes)
        DeleteMetaFile(hMf);

    return fReturn;
}


void NEAR cdecl printat(HDC hdc, int x, int y, LPSTR fmt, ...)
{
    char	buf[80];

    wvsprintf(buf, fmt, (LPSTR)(&fmt + 1));
    TextOut(hdc, x, y, buf, lstrlen(buf));
}


BOOL
DrawSimpleBannerPage(
    HDC hdcPrn,
    PJOB_INFO_2 pJobInfo
)
{
    int dxInch, dyInch;
    int dxPage, dyPage;
    POINT offset;
    TEXTMETRIC tm;
    int dyLine;
    int y;
    char szBuf[128];
    int iTmp;

    dxInch = GetDeviceCaps(hdcPrn, LOGPIXELSX);
    dyInch = GetDeviceCaps(hdcPrn, LOGPIXELSY);

    dxPage = GetDeviceCaps(hdcPrn, HORZSIZE);
    dyPage = GetDeviceCaps(hdcPrn, VERTSIZE);

    offset.x = offset.y = 0;
    Escape(hdcPrn, GETPRINTINGOFFSET, 0, NULL, &offset);

    GetTextMetrics(hdcPrn, &tm);

    dyLine = tm.tmHeight;

    y = dyInch;

    LoadString(hInst, IDS_BANNERTITLE1, szBuf, sizeof(szBuf));
    printat(hdcPrn, dxInch, y += dyLine, szBuf);
    LoadString(hInst, IDS_BANNERTITLE2, szBuf, sizeof(szBuf));
    printat(hdcPrn, dxInch, y += dyLine, szBuf);

    LoadString(hInst, IDS_BANNERJOB, szBuf, sizeof(szBuf));
    printat(hdcPrn, dxInch, y += dyLine, szBuf, pJobInfo->pDocument);
    LoadString(hInst, IDS_BANNERUSER, szBuf, sizeof(szBuf));
    printat(hdcPrn, dxInch, y += dyLine, szBuf, pJobInfo->pUserName);

    iTmp = LoadString(hInst, IDS_BANNERDATE, szBuf, sizeof(szBuf));
    SystemTimeString(&pJobInfo->Submitted, &szBuf[iTmp]);
    printat(hdcPrn, dxInch, y += dyLine, szBuf);

    return TRUE;
}


BOOL
DrawBannerPage(
    HDC hdcPrn,
    PJOB_INFO_2 pJobInfo,
    DWORD iBannerType,
    LPSTR pSepFile
)
{
    switch (iBannerType)
    {
    case BANNER_SIMPLE:
        return DrawSimpleBannerPage(hdcPrn, pJobInfo);

    case BANNER_FULL:
        return DrawFullBannerPage(hdcPrn, pJobInfo, NULL);

    case BANNER_CUSTOM:
        return DrawFullBannerPage(hdcPrn, pJobInfo, pSepFile);

    default:
        return 0;
    }
}


int CALLBACK AbortProc(HDC hdc, int code)
{
    // continue printing
    return TRUE;
}


BOOL
InsertBannerPage(
    HANDLE hPrinter,
    DWORD dwJobId,
    LPSTR pOutputFile,
    DWORD iBannerType,
    LPSTR pSepFile
)
{
    HDC hdcPrn;
    DOCINFO di;
    PJOB_INFO_2 pJobInfo;
    DWORD dwNeeded;

    GetJob(hPrinter, dwJobId, 2, NULL, 0, &dwNeeded);
    if (!(pJobInfo = (PJOB_INFO_2)LocalAlloc(LPTR, dwNeeded)))
        return FALSE;

    if (!GetJob(hPrinter, dwJobId, 2, (LPBYTE)pJobInfo, dwNeeded, &dwNeeded))
        goto DCError;

    if (!(hdcPrn = CreateDC(NULL, pJobInfo->pPrinterName, NULL, NULL)))
        goto DCError;
    
    if (SetAbortProc(hdcPrn, AbortProc) < 0)
    {
        DeleteDC(hdcPrn);
        goto DCError;
    }

    di.cbSize = sizeof(di);
    di.lpszDocName = pJobInfo->pDocument;
    di.lpszOutput = pOutputFile;
    di.lpszDatatype = NULL;
    di.fwType = DI_DIRECT;

    if (StartDoc(hdcPrn, (LPDOCINFO)&di) < 0)
    {
        DeleteDC(hdcPrn);
        goto DCError;
    }

    if (StartPage(hdcPrn) < 0)
        goto PrintError;

    if (!DrawBannerPage(hdcPrn, pJobInfo, iBannerType, pSepFile))
    {
        // Something went wrong drawing the banner page.
        AbortDoc(hdcPrn);
        DeleteDC(hdcPrn);
    }
    else
    {
        if (EndPage(hdcPrn) < 0)
            goto PrintError;
    
        if (EndDoc(hdcPrn) < 0)
            goto PrintError;
    
        DeleteDC(hdcPrn);
    }
    
    return TRUE;

PrintError:
    AbortDoc(hdcPrn);
    DeleteDC(hdcPrn);

DCError:
    LocalFree((HANDLE)pJobInfo);

    return FALSE;
}
