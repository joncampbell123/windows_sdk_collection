/*************************************************************************
**
**    OLE 2 Standard Utilities
**
**    olestd.c
**
**    This file contains utilities that are useful for dealing with 
**    target devices.
**
**    (c) Copyright Microsoft Corp. 1992 All Rights Reserved
**
*************************************************************************/

#define STRICT  1
#include "ole2ui.h"
#ifndef WIN32
#include <print.h>
#endif 

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
STDAPI_(HDC) OleStdCreateDC(DVTARGETDEVICE FAR* ptd)
{
    HDC hdc=NULL;
    LPDEVNAMES lpDevNames;
    LPDEVMODE lpDevMode;
    LPSTR lpszDriverName;
    LPSTR lpszDeviceName;
    LPSTR lpszPortName;

    if (ptd == NULL) {
        hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
        goto errReturn;
    }

    lpDevNames = (LPDEVNAMES) ptd; // offset for size field

    if (ptd->tdExtDevmodeOffset == 0) {
        lpDevMode = NULL;
    }else{
        lpDevMode  = (LPDEVMODE) ((LPSTR)ptd + ptd->tdExtDevmodeOffset);
    }

    lpszDriverName = (LPSTR) lpDevNames + ptd->tdDriverNameOffset;
    lpszDeviceName = (LPSTR) lpDevNames + ptd->tdDeviceNameOffset;
    lpszPortName   = (LPSTR) lpDevNames + ptd->tdPortNameOffset;

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
STDAPI_(HDC) OleStdCreateIC(DVTARGETDEVICE FAR* ptd)
{
    HDC hdcIC=NULL;
    LPDEVNAMES lpDevNames;
    LPDEVMODE lpDevMode;
    LPSTR lpszDriverName;
    LPSTR lpszDeviceName;
    LPSTR lpszPortName;

    if (ptd == NULL) {
        hdcIC = CreateIC("DISPLAY", NULL, NULL, NULL);
        goto errReturn;
    }

    lpDevNames = (LPDEVNAMES) ptd; // offset for size field

    lpDevMode  = (LPDEVMODE) ((LPSTR)ptd + ptd->tdExtDevmodeOffset);

    lpszDriverName = (LPSTR) lpDevNames + ptd->tdDriverNameOffset;
    lpszDeviceName = (LPSTR) lpDevNames + ptd->tdDeviceNameOffset;
    lpszPortName   = (LPSTR) lpDevNames + ptd->tdPortNameOffset;

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
STDAPI_(DVTARGETDEVICE FAR*) OleStdCreateTargetDevice(LPPRINTDLG lpPrintDlg)
{
    DVTARGETDEVICE FAR* ptd=NULL;
    LPDEVNAMES lpDevNames, pDN;
    LPDEVMODE lpDevMode, pDM;
    UINT nMaxOffset;
    LPSTR pszName;
    DWORD dwDevNamesSize, dwDevModeSize, dwPtdSize;

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

    pszName = (LPSTR)pDN + nMaxOffset;

    dwDevNamesSize = (DWORD)(nMaxOffset+lstrlen(pszName) + 1/* NULL term */);
    dwDevModeSize = (DWORD) (pDM->dmSize + pDM->dmDriverExtra);

    dwPtdSize = sizeof(DWORD) + dwDevNamesSize + dwDevModeSize;

    if ((ptd = (DVTARGETDEVICE FAR*)OleStdMalloc(dwPtdSize)) != NULL) {

        // copy in the info
        ptd->tdSize = (UINT)dwPtdSize;

        lpDevNames = (LPDEVNAMES) &ptd->tdDriverNameOffset;
        _fmemcpy(lpDevNames, pDN, (size_t)dwDevNamesSize);

        lpDevMode=(LPDEVMODE)((LPSTR)&ptd->tdDriverNameOffset+dwDevNamesSize);
        _fmemcpy(lpDevMode, pDM, (size_t)dwDevModeSize);

        ptd->tdDriverNameOffset += 4 ;
        ptd->tdDeviceNameOffset += 4 ;
        ptd->tdPortNameOffset   += 4 ;
        ptd->tdExtDevmodeOffset = (UINT)dwDevNamesSize + 4 ;
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
STDAPI_(BOOL) OleStdDeleteTargetDevice(DVTARGETDEVICE FAR* ptd)
{
    BOOL res=TRUE;

    if (ptd != NULL) {
        OleStdFree(ptd);
    }

    return res;
}



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
STDAPI_(DVTARGETDEVICE FAR*) OleStdCopyTargetDevice(DVTARGETDEVICE FAR* ptdSrc)
{
  DVTARGETDEVICE FAR* ptdDest = NULL;

  if (ptdSrc == NULL) {
    return NULL;
  }

  if ((ptdDest = (DVTARGETDEVICE FAR*)OleStdMalloc(ptdSrc->tdSize)) != NULL) {
    _fmemcpy(ptdDest, ptdSrc, (size_t)ptdSrc->tdSize);
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
STDAPI_(BOOL) OleStdCopyFormatEtc(LPFORMATETC petcDest, LPFORMATETC petcSrc)
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

