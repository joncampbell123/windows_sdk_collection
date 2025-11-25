/*************************************************************************
**
**    OLE 2 Standard Utilities
**
**    olestd.c
**
**    This file contains utilities that are useful for dealing with
**    target devices.
**
**    (c) Copyright Microsoft Corp. 1992-1994 All Rights Reserved
**
*************************************************************************/

#if defined(USEHEADER)
#include "OleHeaders.h"
#endif


#if defined(__MWERKS__) && !defined(__powerc)
#pragma pointers_in_D0
#endif


#if defined(_MSC_VER) && defined(__powerc)
#include <msvcmac.h>
#endif

#include <string.h>

#include <ole2.h>

#include "ole2ui.h"
#include "uidebug.h"


OLEDBGDATA

#ifdef LATER

/*
 * OleStdCreateDC()
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 *    SCODE  -  S_OK if successful
 */
#ifndef _MSC_VER
#pragma segment TargtDevSeg
#else
#pragma code_seg("TargtDevSeg", "SWAPPABLE")
#endif
STDAPI_(WindowPtr) OleStdCreateDC(DVTARGETDEVICE * ptd)
{
    WindowPtr hdc=NULL;
    LPDEVNAMES lpDevNames;
    LPDEVMODE lpDevMode;
    char* lpszDriverName;
    char* lpszDeviceName;
    char* lpszPortName;

    if (ptd == NULL) {
        hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
        goto errReturn;
    }

    lpDevNames = (LPDEVNAMES) ptd; // offset for size field

    if (ptd->tdExtDevmodeOffset == 0) {
        lpDevMode = NULL;
    }else{
        lpDevMode  = (LPDEVMODE) ((char*)ptd + ptd->tdExtDevmodeOffset);
    }

    lpszDriverName = (char*) lpDevNames + ptd->tdDriverNameOffset;
    lpszDeviceName = (char*) lpDevNames + ptd->tdDeviceNameOffset;
    lpszPortName   = (char*) lpDevNames + ptd->tdPortNameOffset;

    hdc = CreateDC(lpszDriverName, lpszDeviceName, lpszPortName, lpDevMode);

errReturn:
    return hdc;
}


/*
 * OleStdCreateIC()
 *
 * Purpose: Same as OleStdCreateDC, except that information context is
 *          created, rather than a whole device context.  (CreateIC is
 *          used rather than CreateDC).
 *          OleStdDeleteDC is still used to delete the information context.
 *
 * Parameters:
 *
 * Return Value:
 *    SCODE  -  S_OK if successful
 */
#ifndef _MSC_VER
#pragma segment TargtDevSeg
#else
#pragma code_seg("TargtDevSeg", "SWAPPABLE")
#endif
STDAPI_(WindowPtr) OleStdCreateIC(DVTARGETDEVICE * ptd)
{
    WindowPtr hdcIC=NULL;
    LPDEVNAMES lpDevNames;
    LPDEVMODE lpDevMode;
    char* lpszDriverName;
    char* lpszDeviceName;
    char* lpszPortName;

    if (ptd == NULL) {
        hdcIC = CreateIC("DISPLAY", NULL, NULL, NULL);
        goto errReturn;
    }

    lpDevNames = (LPDEVNAMES) ptd; // offset for size field

    lpDevMode  = (LPDEVMODE) ((char*)ptd + ptd->tdExtDevmodeOffset);

    lpszDriverName = (char*) lpDevNames + ptd->tdDriverNameOffset;
    lpszDeviceName = (char*) lpDevNames + ptd->tdDeviceNameOffset;
    lpszPortName   = (char*) lpDevNames + ptd->tdPortNameOffset;

    hdcIC = CreateIC(lpszDriverName, lpszDeviceName, lpszPortName, lpDevMode);

errReturn:
    return hdcIC;
}


/*
 * OleStdCreateTargetDevice()
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 *    SCODE  -  S_OK if successful
 */
#ifndef _MSC_VER
#pragma segment TargtDevSeg
#else
#pragma code_seg("TargtDevSeg", "SWAPPABLE")
#endif
STDAPI_(DVTARGETDEVICE *) OleStdCreateTargetDevice(LPPRINTDLG lpPrintDlg)
{
    DVTARGETDEVICE * ptd=NULL;
    LPDEVNAMES lpDevNames, pDN;
    LPDEVMODE lpDevMode, pDM;
    unsigned int nMaxOffset;
    char* pszName;
    unsigned long dwDevNamesSize, dwDevModeSize, dwPtdSize;

    if ((pDN = (LPDEVNAMES)GlobalLock(lpPrintDlg->hDevNames)) == NULL) {
        goto errReturn;
    }

    if ((pDM = (LPDEVMODE)GlobalLock(lpPrintDlg->hDevMode)) == NULL) {
        goto errReturn;
    }

    nMaxOffset =  (pDN->wDriverOffset > pDN->wDeviceOffset) ?
        pDN->wDriverOffset : pDN->wDeviceOffset ;

    nMaxOffset =  (pDN->wOutputOffset > nMaxOffset) ?
        pDN->wOutputOffset : nMaxOffset ;

    pszName = (char*)pDN + nMaxOffset;

    dwDevNamesSize = (unsigned long)(nMaxOffset+lstrlen(pszName) + 1/* NULL term */);
    dwDevModeSize = (unsigned long) (pDM->dmSize + pDM->dmDriverExtra);

    dwPtdSize = sizeof(unsigned long) + dwDevNamesSize + dwDevModeSize;

    if ((ptd = (DVTARGETDEVICE *)OleStdMalloc(dwPtdSize)) != NULL) {

        // copy in the info
        ptd->tdSize = (unsigned int)dwPtdSize;

        lpDevNames = (LPDEVNAMES) &ptd->tdDriverNameOffset;
        _fmemcpy(lpDevNames, pDN, (size_t)dwDevNamesSize);

        lpDevMode=(LPDEVMODE)((char*)&ptd->tdDriverNameOffset+dwDevNamesSize);
        _fmemcpy(lpDevMode, pDM, (size_t)dwDevModeSize);

        ptd->tdDriverNameOffset += 4 ;
        ptd->tdDeviceNameOffset += 4 ;
        ptd->tdPortNameOffset   += 4 ;
        ptd->tdExtDevmodeOffset = (unsigned int)dwDevNamesSize + 4 ;
    }

errReturn:
    GlobalUnlock(lpPrintDlg->hDevNames);
    GlobalUnlock(lpPrintDlg->hDevMode);

    return ptd;
}



/*
 * OleStdDeleteTargetDevice()
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 *    SCODE  -  S_OK if successful
 */
#ifndef _MSC_VER
#pragma segment TargtDevSeg
#else
#pragma code_seg("TargtDevSeg", "SWAPPABLE")
#endif
STDAPI_(BOOL) OleStdDeleteTargetDevice(DVTARGETDEVICE * ptd)
{
    unsigned long res=TRUE;

    if (ptd != NULL) {
        OleStdFree(ptd);
    }

    return res;
}

#endif // LATER

/*
 * OleStdCopyTargetDevice()
 *
 * Purpose:
 *  duplicate a TARGETDEVICE struct. this function allocates memory for
 *  the copy. the caller MUST free the allocated copy when done with it
 *  using the standard allocator returned from CoGetMalloc.
 *  (OleStdFree can be used to free the copy).
 *
 * Parameters:
 *  ptdSrc      pointer to source TARGETDEVICE
 *
 * Return Value:
 *    pointer to allocated copy of ptdSrc
 *    if ptdSrc==NULL then retuns NULL is returned.
 *    if ptdSrc!=NULL and memory allocation fails, then NULL is returned
 */
#ifndef _MSC_VER
#pragma segment TargtDevSeg
#else
#pragma code_seg("TargtDevSeg", "SWAPPABLE")
#endif
STDAPI_(DVTARGETDEVICE *) OleStdCopyTargetDevice(DVTARGETDEVICE * ptdSrc)
{
  DVTARGETDEVICE * ptdDest = NULL;

  if (ptdSrc == NULL) {
    return NULL;
  }

  if ((ptdDest = (DVTARGETDEVICE *)OleStdMalloc(ptdSrc->tdSize)) != NULL) {
    memcpy(ptdDest, ptdSrc, (size_t)ptdSrc->tdSize);
  }

  return ptdDest;
}


/*
 * OleStdCopyFormatEtc()
 *
 * Purpose:
 *  Copies the contents of a FORMATETC structure. this function takes
 *  special care to copy correctly copying the pointer to the TARGETDEVICE
 *  contained within the source FORMATETC structure.
 *  if the source FORMATETC has a non-NULL TARGETDEVICE, then a copy
 *  of the TARGETDEVICE will be allocated for the destination of the
 *  FORMATETC (petcDest).
 *
 *  OLE2NOTE: the caller MUST free the allocated copy of the TARGETDEVICE
 *  within the destination FORMATETC when done with it
 *  using the standard allocator returned from CoGetMalloc.
 *  (OleStdFree can be used to free the copy).
 *
 * Parameters:
 *  petcDest      pointer to destination FORMATETC
 *  petcSrc       pointer to source FORMATETC
 *
 * Return Value:
 *    pointer to allocated copy of ptdSrc; retuns NULL if not successful
 */
#ifndef _MSC_VER
#pragma segment TargtDevSeg
#else
#pragma code_seg("TargtDevSeg", "SWAPPABLE")
#endif
STDAPI_(unsigned long) OleStdCopyFormatEtc(LPFORMATETC petcDest, LPFORMATETC petcSrc)
{
  if ((petcDest == NULL) || (petcSrc == NULL)) {
    return FALSE;
  }

  petcDest->cfFormat = petcSrc->cfFormat;
  petcDest->ptd      = OleStdCopyTargetDevice(petcSrc->ptd);
  petcDest->dwAspect = petcSrc->dwAspect;
  petcDest->lindex   = petcSrc->lindex;
  petcDest->tymed    = petcSrc->tymed;

  return TRUE;

}

