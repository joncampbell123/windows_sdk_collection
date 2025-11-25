/*************************************************************************
**
**    OLE 2 Standard Utilities
**
**    olestd.c
**
**    This file contains utilities that are useful for most standard
**        OLE 2.0 compound document type applications.
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

#include <Events.h>
#include <Dialogs.h>
#include <Menus.h>
#ifndef _MSC_VER
#include <AppleEvents.h>
#include <Processes.h>
#include <GestaltEqu.h>
#else
#include <AppleEve.h>
#include <Processe.h>
#include <GestaltE.h>
#endif
#include <stdio.h>
#include <string.h>

#include <ole2.h>

// #include "olestd.h"
#include "ole2ui.h"
#include "uidebug.h"
#include "regdb.h"
#include "common.h"


OLEDBGDATA


#ifndef _MSC_VER
#define STGMEDIUM_HGLOBAL	u.hGlobal
#define STGMEDIUM_PSTM		u.pstm
#define STDMEDIUM_PSTG		u.pstg
#else
#define STGMEDIUM_HGLOBAL	hGlobal
#define STGMEDIUM_PSTM		pstm
#define STDMEDIUM_PSTG		pstg
#endif

static char szAssertMemAlloc[] = "CoGetMalloc failed";


/* OleStdSetupAdvises
** ------------------
**    Setup the standard View and Ole advises required by a standard,
**    compound document-oriented container. Such a container relies on
**    Ole to manage the presentation of the Ole object. The container
**    call IViewObject::Draw to render (display) the object.
**
**    This helper routine performs the following tasks:
**                      setup View advise
**                      setup Ole advise
**                      Call IOleObject::SetHostNames
**                      Call OleSetContainedObject
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(unsigned long) OleStdSetupAdvises(LPOLEOBJECT lpOleObject, unsigned long dwDrawAspect,
                    char* lpszContainerApp, char* lpszContainerObj,
                    LPADVISESINK lpAdviseSink)
{
    LPVIEWOBJECT lpViewObject;
    HRESULT hrErr;
    unsigned long dwTemp;
    unsigned long fStatus = true;

    hrErr = lpOleObject->lpVtbl->QueryInterface(
            lpOleObject,
            &IID_IViewObject,
            (void* *)&lpViewObject
    );

    /* Setup View advise */
    if (hrErr == NOERROR) {

        OLEDBG_BEGIN2("IViewObject::SetAdvise called\r\n")
        lpViewObject->lpVtbl->SetAdvise(
                lpViewObject,
                dwDrawAspect,
                0,
                lpAdviseSink
        );
        OLEDBG_END2

        OleStdRelease((LPUNKNOWN)lpViewObject);
    } else {
        fStatus = false;
    }

    /* Setup OLE advise.
    **    OLE2NOTE: normally containers do NOT need to setup an OLE
    **    advise. this advise connection is only useful for the OLE's
    **    DefHandler and the OleLink object implementation. some
    **    special container's might need to setup this advise for
    **    programatic reasons.
    **
    **    NOTE: this advise will be torn down automatically by the
    **    server when we release the object, therefore we do not need
    **    to store the connection id.
    */
    OLEDBG_BEGIN2("IOleObject::Advise called\r\n")
    hrErr = lpOleObject->lpVtbl->Advise(
            lpOleObject,
            lpAdviseSink,
            (unsigned long *)&dwTemp
    );
    OLEDBG_END2
    if (hrErr != NOERROR) fStatus = false;

	if (lpszContainerApp && lpszContainerObj) {
		/* Setup the host names for the OLE object. */
		OLEDBG_BEGIN2("IOleObject::SetHostNames called\r\n")
		hrErr = lpOleObject->lpVtbl->SetHostNames(
				lpOleObject,
				lpszContainerApp,
				lpszContainerObj
		);
		OLEDBG_END2
		if (hrErr != NOERROR) fStatus = false;
	}

    /* Inform the loadded object's handler/inproc-server that it is in
    **    its embedding container's process.
    */
    OLEDBG_BEGIN2("OleSetContainedObject(true) called\r\n")
    OleSetContainedObject((LPUNKNOWN)lpOleObject, true);
    OLEDBG_END2

    return fStatus;
}

/* OleStdSwitchDisplayAspect
** -------------------------
**    Switch the currently cached display aspect between DVASPECT_ICON
**    and DVASPECT_CONTENT.
**
**    NOTE: when setting up icon aspect, any currently cached content
**    cache is discarded and any advise connections for content aspect
**    are broken.
**
**    RETURNS:
**      true -- new display aspect setup successfully
**      false -- could NOT setup up new display aspect; current display
**              aspect and cache contents unchanged.
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI OleStdSwitchDisplayAspect(
        LPOLEOBJECT             lpOleObj,
        unsigned long*          lpdwCurAspect,
        unsigned long           dwNewAspect,
        PicHandle               hMetaPict,
        unsigned long           fDeleteOldAspect,
        unsigned long           fSetupViewAdvise,
        LPADVISESINK            lpAdviseSink,
        Boolean*		        lpfMustUpdate
)
{
    LPOLECACHE      lpOleCache = NULL;
    LPVIEWOBJECT    lpViewObj = NULL;
    LPENUMSTATDATA  lpEnumStatData = NULL;
    STATDATA        StatData;
    FORMATETC       FmtEtc;
    STGMEDIUM       Medium;
    unsigned long           dwAdvf;
    unsigned long           dwNewConnection;
    unsigned long           dwOldAspect = *lpdwCurAspect;
    HRESULT         hrErr;
    unsigned long            fStatus = true;

    if (lpfMustUpdate)
	    *lpfMustUpdate = false;

    lpOleCache = (LPOLECACHE)OleStdQueryInterface(
                                        (LPUNKNOWN)lpOleObj,&IID_IOleCache);

    // if IOleCache* is NOT available, do nothing
    if (! lpOleCache)
        return ResultFromScode(E_INVALIDARG);

    // Setup new cache with the new aspect
    FmtEtc.cfFormat = 'PICT';
    FmtEtc.ptd      = NULL;
    FmtEtc.dwAspect = dwNewAspect;
    FmtEtc.lindex   = -1;
    FmtEtc.tymed    = TYMED_MFPICT;

    /* OLE2NOTE: if we are setting up Icon aspect with a custom icon
    **    then we do not want DataAdvise notifications to ever change
    **    the contents of the data cache. thus we set up a NODATA
    **    advise connection. otherwise we set up a standard DataAdvise
    **    connection.
    */
    if (dwNewAspect == DVASPECT_ICON && hMetaPict)
        dwAdvf = ADVF_NODATA | ADVF_PRIMEFIRST | ADVF_ONLYONCE;
    else
        dwAdvf = ADVF_PRIMEFIRST;

    OLEDBG_BEGIN2("IOleCache::Cache called\r\n")
    hrErr = lpOleCache->lpVtbl->Cache(
            lpOleCache,
            (LPFORMATETC)&FmtEtc,
            dwAdvf,
            (unsigned long*)&dwNewConnection
    );
    OLEDBG_END2

    if (! SUCCEEDED(hrErr)) {
        OleDbgOutHResult("IOleCache::Cache returned", hrErr);
        OleStdRelease((LPUNKNOWN)lpOleCache);
        return hrErr;
    }

    *lpdwCurAspect = dwNewAspect;

    /* OLE2NOTE: if we are setting up Icon aspect with a custom icon,
    **    then stuff the icon into the cache. otherwise force the cache
    **    to be updated. this will run the object if necessary.
    */
    if (dwNewAspect == DVASPECT_ICON && hMetaPict) {

        Medium.tymed             = TYMED_MFPICT;
        Medium.STGMEDIUM_HGLOBAL = (Handle)hMetaPict;
        Medium.pUnkForRelease    = NULL;

        OLEDBG_BEGIN2("IOleCache::SetData called\r\n")
        hrErr = lpOleCache->lpVtbl->SetData(
                lpOleCache,
                (LPFORMATETC)&FmtEtc,
                (LPSTGMEDIUM)&Medium,
                false   /* fRelease */
        );
        OLEDBG_END2
    } else {
    	if (lpfMustUpdate)
        	*lpfMustUpdate = true;

#if defined( OBSOLETE )
        OLEDBG_BEGIN2("IOleObj::Update called\r\n")
        hrErr = lpOleObj->lpVtbl->Update(lpOleObj);
        OLEDBG_END2

        if (hrErr != NOERROR) {
            OleDbgOutHResult("IOleObject::Update returned", hrErr);
            fStatus = false;
        }
#endif
    }

    if (fSetupViewAdvise && lpAdviseSink) {
        /* OLE2NOTE: re-establish the ViewAdvise connection */
        lpViewObj = (LPVIEWOBJECT)OleStdQueryInterface(
                                        (LPUNKNOWN)lpOleObj,&IID_IViewObject);

        if (lpViewObj) {

            OLEDBG_BEGIN2("IViewObject::SetAdvise called\r\n")
            lpViewObj->lpVtbl->SetAdvise(
                    lpViewObj,
                    dwNewAspect,
                    0,
                    lpAdviseSink
            );
            OLEDBG_END2

            OleStdRelease((LPUNKNOWN)lpViewObj);
        }
    }

    /* OLE2NOTE: remove any existing caches that are set up for the old
    **    display aspect. It WOULD be possible to retain the caches set
    **    up for the old aspect, but this would increase the storage
    **    space required for the object and possibly require additional
    **    overhead to maintain the unused cachaes. For these reasons the
    **    strategy to delete the previous caches is prefered. if it is a
    **    requirement to quickly switch between Icon and Content
    **    display, then it would be better to keep both aspect caches.
    */

    if (fDeleteOldAspect) {
        OLEDBG_BEGIN2("IOleCache::EnumCache called\r\n")
        hrErr = lpOleCache->lpVtbl->EnumCache(
                lpOleCache,
                (LPENUMSTATDATA *)&lpEnumStatData
        );
        OLEDBG_END2

        while(hrErr == NOERROR) {
            hrErr = lpEnumStatData->lpVtbl->Next(
                    lpEnumStatData,
                    1,
                    (LPSTATDATA)&StatData,
                    NULL
            );
            if (hrErr != NOERROR)
                break;              // DONE! no more caches.

            if (StatData.formatetc.dwAspect == dwOldAspect) {

                // Remove previous cache with old aspect
                OLEDBG_BEGIN2("IOleCache::Uncache called\r\n")
                lpOleCache->lpVtbl->Uncache(lpOleCache,StatData.dwConnection);
                OLEDBG_END2
            }
        }

        if (lpEnumStatData) {
            OleStdVerifyRelease((LPUNKNOWN)lpEnumStatData);
        }
    }

    if (lpOleCache)
        OleStdRelease((LPUNKNOWN)lpOleCache);

    return NOERROR;
}


/* OleStdSetIconInCache
** --------------------
**    SetData a new icon into the existing DVASPECT_ICON cache.
**
**    RETURNS:
**      HRESULT returned from IOleCache::SetData
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI OleStdSetIconInCache(LPOLEOBJECT lpOleObj, PicHandle hMetaPict)
{
    LPOLECACHE      lpOleCache = NULL;
    FORMATETC       FmtEtc;
    STGMEDIUM       Medium;
    HRESULT         hrErr;

    if (! hMetaPict)
        return ResultFromScode(E_FAIL);   // invalid icon

    lpOleCache = (LPOLECACHE)OleStdQueryInterface(
                                        (LPUNKNOWN)lpOleObj,&IID_IOleCache);
    if (! lpOleCache)
        return ResultFromScode(E_FAIL);   // if IOleCache* is NOT available, do nothing

    FmtEtc.cfFormat = 'PICT';
    FmtEtc.ptd      = NULL;
    FmtEtc.dwAspect = DVASPECT_ICON;
    FmtEtc.lindex   = -1;
    FmtEtc.tymed    = TYMED_MFPICT;

    // stuff the icon into the cache.
    Medium.tymed             = TYMED_MFPICT;
    Medium.STGMEDIUM_HGLOBAL = (Handle)hMetaPict;
    Medium.pUnkForRelease    = NULL;

    OLEDBG_BEGIN2("IOleCache::SetData called\r\n")
    hrErr = lpOleCache->lpVtbl->SetData(
            lpOleCache,
            (LPFORMATETC)&FmtEtc,
            (LPSTGMEDIUM)&Medium,
            false   /* fRelease */
    );
    OLEDBG_END2

    OleStdRelease((LPUNKNOWN)lpOleCache);

    return hrErr;
}


/* OleStdDoConvert
** ---------------
** Do the container-side responsibilities for converting an object.
**    This function would be used in conjunction with the OleUIConvert
**    dialog. If the user selects to convert an object then the
**    container must do the following:
**          1. unload the object.
**          2. write the NEW CLSID and NEW user type name
**              string into the storage of the object,
**              BUT write the OLD format tag.
**          3. force an update of the object to force the actual
**              conversion of the data bits.
**
**    This function takes care of step 2.
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI OleStdDoConvert(LPSTORAGE lpStg, REFCLSID rClsidNew)
{
    HRESULT error;
    CLSID clsidOld;
    ResType cfOld;
    char* lpszOld = NULL;
    char szNew[OLEUI_CCHKEYMAX];

    if ((error = ReadClassStg(lpStg, &clsidOld)) != NOERROR) {
        clsidOld = CLSID_NULL;
        goto errRtn;
    }

    // read old fmt/old user type; sets out params to NULL on error
    error = ReadFmtUserTypeStg(lpStg, &cfOld, &lpszOld);
    OleDbgAssert(error == NOERROR || (cfOld == NULL && lpszOld == NULL));

    // get new user type name; if error, set to NULL string
    if (OleStdGetUserTypeOfClass(
            (LPCLSID)rClsidNew, szNew, sizeof(szNew), 0 /* hKey */) == 0)
        szNew[0] = 0;

    // write class stg
    if ((error = WriteClassStg(lpStg, rClsidNew)) != NOERROR)
        goto errRtn;

    // write old fmt/new user type;
    if ((error = WriteFmtUserTypeStg(lpStg, cfOld, szNew)) != NOERROR)
        goto errRewriteInfo;

    // set convert bit
    if ((error = SetConvertStg(lpStg, true)) != NOERROR)
        goto errRewriteInfo;

    // Commit the changes to make them permanent in the storage
    OleStdCommitStorage(lpStg);
    goto okRtn;

errRewriteInfo:
    (void)WriteClassStg(lpStg, &clsidOld);
    (void)WriteFmtUserTypeStg(lpStg, cfOld, lpszOld);

errRtn:

okRtn:
    OleStdFreeString(lpszOld, NULL);
    return error;
}


/* OleStdGetTreatAsFmtUserType
** ---------------------------
**    Determine if the application should perform a TreatAs (ActivateAs
**    object or emulation) operation for the object that is stored in
**    the storage.
**
**    if the CLSID written in the storage is not the same as the
**    application's own CLSID (clsidApp), then a TreatAs operation
**    should take place. if so determine the format the data should be
**    written and the user type name of the object the app should
**    emulate (ie. pretend to be). if this information is not written
**    in the storage then it is looked up in the REGDB. if it can not
**    be found in the REGDB, then the TreatAs operation can NOT be
**    executed.
**
**    RETURNS: true -- if TreatAs should be performed.
**               valid lpclsid, lplpszType, lpcfFmt to TreatAs are returned
**                      (NOTE: lplpszType must be freed by caller)
**             false -- NO TreatAs. lpszType will be NULL.
**               lpclsid = CLSID_NULL; lplpszType = lpcfFmt = NULL;
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(unsigned long) OleStdGetTreatAsFmtUserType(
        REFCLSID        rclsidApp,
        LPSTORAGE       lpStg,
        CLSID *      	lpclsid,
        ResType * 		lpcfFmt,
        char* *      	lplpszType
)
{
    HRESULT hrErr;
    HKEY    hKey;
    long    lRet;
    unsigned int    lSize;
    char    szBuf[OLEUI_CCHKEYMAX];

    *lpclsid    = CLSID_NULL;
    *lpcfFmt    = 0;
    *lplpszType = NULL;

    hrErr = ReadClassStg(lpStg, lpclsid);
    if (hrErr == NOERROR &&
        ! IsEqualCLSID(lpclsid, &CLSID_NULL) &&
        ! IsEqualCLSID(lpclsid, rclsidApp)) {

        hrErr = ReadFmtUserTypeStg(lpStg,(ResType *)lpcfFmt,lplpszType);
        if (hrErr == NOERROR && lplpszType && *lpcfFmt != 0)
            return true;    // Do TreatAs. info was in lpStg.

        /* read info from REGDB
        **    *lpcfFmt = value of field: CLSID\{...}\DataFormats\DefaultFile
        **    *lplpszType = value of field: CLSID\{...}
        */
        //Open up the root key.
        lRet=RegOpenKey(HKEY_CLASSES_ROOT, NULL, &hKey);
        if (lRet != (long)ERROR_SUCCESS)
            return false;
        *lpcfFmt = OleStdGetDefaultFileFormatOfClass(lpclsid, hKey);
        if (*lpcfFmt == 0)
            return false;
        lSize = OleStdGetUserTypeOfClass(lpclsid,szBuf,sizeof(szBuf),hKey);
        if (lSize == 0)
            return false;
        *lplpszType = OleStdCopyString(szBuf, NULL);
    } else {
        return false;       // NO TreatAs
    }
}


/* OleStdDoTreatAsClass
** --------------------
** Do the container-side responsibilities for "ActivateAs" (aka.
**    TreatAs) for an object.
**    This function would be used in conjunction with the OleUIConvert
**    dialog. If the user selects to ActivateAs an object then the
**    container must do the following:
**          1. unload ALL objects of the OLD class that app knows about
**          2. add the TreatAs tag in the registration database
**              by calling CoTreatAsClass().
**          3. lazily it can reload the objects; when the objects
**              are reloaded the TreatAs will take effect.
**
**    This function takes care of step 2.
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI OleStdDoTreatAsClass(char* lpszUserType, REFCLSID rclsid, REFCLSID rclsidNew)
{
    HRESULT hrErr;
    char*   lpszCLSID = NULL;
    long    lRet;
    HKEY    hKey;

    OLEDBG_BEGIN2("CoTreatAsClass called\r\n")
    hrErr = CoTreatAsClass(rclsid, rclsidNew);
    OLEDBG_END2

    if ((hrErr != NOERROR) && lpszUserType) {
        lRet = RegOpenKey(HKEY_CLASSES_ROOT, (char*)"CLSID",
                (HKEY  *)&hKey);
        StringFromCLSID(rclsid, (char* *)&lpszCLSID);
        RegSetValue(hKey, lpszCLSID, REG_SZ, lpszUserType,
                strlen(lpszUserType));

        if (lpszCLSID)
            OleStdFreeString(lpszCLSID, NULL);

        hrErr = CoTreatAsClass(rclsid, rclsidNew);
        RegCloseKey(hKey);
    }

    return hrErr;
}

/* OleStdIsOleLink
** ---------------
**    Returns true if the OleObject is infact an OLE link object. this
**    checks if IOleLink interface is supported. if so, the object is a
**    link, otherwise not.
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(unsigned long) OleStdIsOleLink(LPUNKNOWN lpUnk)
{
    LPOLELINK lpOleLink;

    lpOleLink = (LPOLELINK)OleStdQueryInterface(lpUnk, &IID_IOleLink);

    if (lpOleLink) {
        OleStdRelease((LPUNKNOWN)lpOleLink);
        return true;
    } else
        return false;
}


/* OleStdQueryInterface
** --------------------
**    Returns the desired interface pointer if exposed by the given object.
**    Returns NULL if the interface is not available.
**    eg.:
**      lpDataObj = OleStdQueryInterface(lpOleObj, &IID_DataObject);
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(LPUNKNOWN) OleStdQueryInterface(LPUNKNOWN lpUnk, REFIID riid)
{
    LPUNKNOWN lpInterface;
    HRESULT hrErr;

    hrErr = lpUnk->lpVtbl->QueryInterface(
            lpUnk,
            riid,
            (void* *)&lpInterface
    );

    if (hrErr == NOERROR)
        return lpInterface;
    else
        return NULL;
}

/* OleStdGetData
** -------------
**    Retrieve data from an IDataObject in a specified format on a
**    global memory block. This function ALWAYS returns a private copy
**    of the data to the caller. if necessary a copy is made of the
**    data (ie. if lpMedium->pUnkForRelease != NULL). The caller assumes
**    ownership of the data block in all cases and must free the data
**    when done with it. The caller may directly free the data handle
**    returned (taking care whether it is a simple HGLOBAL or a HANDLE
**    to a MetafilePict) or the caller may call
**    ReleaseStgMedium(lpMedium). this OLE helper function will do the
**    right thing.
**
**    PARAMETERS:
**        LPDATAOBJECT lpDataObj  -- object on which GetData should be
**                                                         called.
**        ResType cfFormat     -- desired clipboard format (eg. 'TEXT')
**        DVTARGETDEVICE * lpTargetDevice -- target device for which
**                                  the data should be composed. This may
**                                  be NULL. NULL can be used whenever the
**                                  data format is insensitive to target
**                                  device or when the caller does not care
**                                  what device is used.
**        LPSTGMEDIUM lpMedium    -- ptr to STGMEDIUM struct. the
**                                  resultant medium from the
**                                  IDataObject::GetData call is
**                                  returned.
**
**    RETURNS:
**       HGLOBAL -- global memory handle of retrieved data block.
**       NULL    -- if error.
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(Handle) OleStdGetData(
        LPDATAOBJECT        lpDataObj,
        ResType          cfFormat,
        DVTARGETDEVICE * lpTargetDevice,
        unsigned long               dwAspect,
        LPSTGMEDIUM         lpMedium
)
{
    HRESULT hrErr;
    FORMATETC formatetc;
    Handle hGlobal = NULL;
	Handle hCopy;

    formatetc.cfFormat = cfFormat;
    formatetc.ptd = lpTargetDevice;
    formatetc.dwAspect = dwAspect;
    formatetc.lindex = -1;

    switch (cfFormat) {
        case 'PICT':
            formatetc.tymed = TYMED_MFPICT;
            break;

        case 'BMAP':
            formatetc.tymed = TYMED_GDI;
            break;

        default:
            formatetc.tymed = TYMED_HGLOBAL;
            break;
    }

    OLEDBG_BEGIN2("IDataObject::GetData called\r\n")
    hrErr = lpDataObj->lpVtbl->GetData(
            lpDataObj,
            (LPFORMATETC)&formatetc,
            lpMedium
    );
    OLEDBG_END2

    if (hrErr != NOERROR)
        return NULL;

    if ((hGlobal = lpMedium->STGMEDIUM_HGLOBAL) == NULL)
        return NULL;

    // Check if hGlobal really points to valid memory
    if (*hGlobal == nil)
    	return nil;    // ERROR: memory is NOT valid

    if (hGlobal != NULL && lpMedium->pUnkForRelease != NULL) {
        /* OLE2NOTE: the callee wants to retain ownership of the data.
        **    this is indicated by passing a non-NULL pUnkForRelease.
        **    thus, we will make a copy of the data and release the
        **    callee's copy.
        */
        hCopy = OleDuplicateData(hGlobal, cfFormat, 0);

        ReleaseStgMedium(lpMedium); // release callee's copy of data

        hGlobal = hCopy;
        lpMedium->STGMEDIUM_HGLOBAL = hCopy;
        lpMedium->pUnkForRelease = NULL;
    }

    return hGlobal;
}


/* OleStdMalloc
** ------------
**    allocate memory using the currently active IMalloc* allocator
*/
STDAPI_(void*) OleStdMalloc(unsigned long ulSize)
{
    void* pout;
    LPMALLOC pmalloc;

    if (CoGetMalloc(MEMCTX_TASK, &pmalloc) != S_OK) {
        OleDbgAssertSz(0, szAssertMemAlloc);
        return NULL;
    }

    pout = (void*)pmalloc->lpVtbl->Alloc(pmalloc, ulSize);

    if (pmalloc != NULL) {
        unsigned long refs = pmalloc->lpVtbl->Release(pmalloc);
    }

    return pout;
}


/* OleStdRealloc
** -------------
**    re-allocate memory using the currently active IMalloc* allocator
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(void*) OleStdRealloc(void* pmem, unsigned long ulSize)
{
    void* pout;
    LPMALLOC pmalloc;

    if (CoGetMalloc(MEMCTX_TASK, &pmalloc) != S_OK) {
        OleDbgAssertSz(0, szAssertMemAlloc);
        return NULL;
    }

    pout = (void*)pmalloc->lpVtbl->Realloc(pmalloc, pmem, ulSize);

    if (pmalloc != NULL) {
        unsigned long refs = pmalloc->lpVtbl->Release(pmalloc);
    }

    return pout;
}



/* OleStdFree
** ----------
**    free memory using the currently active IMalloc* allocator
*/
STDAPI_(void) OleStdFree(void* pmem)
{
    LPMALLOC pmalloc;

    if (pmem == NULL)
        return;

    if (CoGetMalloc(MEMCTX_TASK, &pmalloc) != S_OK) {
        OleDbgAssertSz(0, szAssertMemAlloc);
        return;
    }

    pmalloc->lpVtbl->Free(pmalloc, pmem);

    if (pmalloc != NULL) {
        unsigned long refs = pmalloc->lpVtbl->Release(pmalloc);
    }

}


/* OleStdGetSize
** -------------
**    Get the size of a memory block that was allocated using the
**    currently active IMalloc* allocator.
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(unsigned long) OleStdGetSize(void* pmem)
{
    unsigned long ulSize;
    LPMALLOC pmalloc;

    if (CoGetMalloc(MEMCTX_TASK, &pmalloc) != S_OK) {
        OleDbgAssertSz(0, szAssertMemAlloc);
        return (unsigned long)-1;
    }

    ulSize = pmalloc->lpVtbl->GetSize(pmalloc, pmem);

    if (pmalloc != NULL) {
        unsigned long refs = pmalloc->lpVtbl->Release(pmalloc);
    }

    return ulSize;
}


/* OleStdFreeString
** ----------------
**    Free a string that was allocated with the currently active
**    IMalloc* allocator.
**
**    if the caller has the current IMalloc* handy, then it can be
**    passed as a argument, otherwise this function will retrieve the
**    active allocator and use it.
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(void) OleStdFreeString(char* lpsz, LPMALLOC lpMalloc)
{
    unsigned long fMustRelease = false;

    if (! lpMalloc) {
        if (CoGetMalloc(MEMCTX_TASK, &lpMalloc) != NOERROR)
            return;
        fMustRelease = true;
    }

    lpMalloc->lpVtbl->Free(lpMalloc, lpsz);

    if (fMustRelease)
        OleStdRelease((LPUNKNOWN)lpMalloc);
}



/* OleStdCopyString
** ----------------
**    Copy a string into memory allocated with the currently active
**    IMalloc* allocator.
**
**    if the caller has the current IMalloc* handy, then it can be
**    passed as a argument, otherwise this function will retrieve the
**    active allocator and use it.
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(char*) OleStdCopyString(char* lpszSrc, LPMALLOC lpMalloc)
{
    char* lpszDest = NULL;
    unsigned long fMustRelease = false;
    unsigned int lSize = strlen(lpszSrc);

    if (!lpMalloc) {
        if (CoGetMalloc(MEMCTX_TASK, &lpMalloc) != NOERROR)
            return NULL;
        fMustRelease = true;
    }

    lpszDest = lpMalloc->lpVtbl->Alloc(lpMalloc, lSize+1);

    if (lpszDest)
        strcpy(lpszDest, lpszSrc);

    if (fMustRelease)
        OleStdRelease((LPUNKNOWN)lpMalloc);

    return lpszDest;
}

/* OleStdMemSet
** ------------
**  Set the first count byte of dest to character c.
**  memset() from the library would have been used but it doesn't work
**		
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(void*) OleStdMemSet(void* pDest, int c, size_t count)
{
	char	*pcDest = pDest;
	
	for (; count; count--)
		*pcDest++ = c;
	
	return pDest;
}



/*
 * OleStdCreateStorageOnHGlobal()
 *
 * Purpose:
 *  Create a memory based IStorage*.
 *
 *  OLE2NOTE: if fDeleteOnRelease==true, then the ILockBytes is created
 *            such that it will delete them memory on its last release.
 *            the IStorage on created on top of the ILockBytes in NOT
 *            created with STGM_DELETEONRELEASE. when the IStorage receives
 *            its last release, it will release the ILockBytes which will
 *            in turn free the memory. it is in fact an error to specify
 *            STGM_DELETEONRELEASE in this situation.
 *
 * Parameters:
 *  hGlobal --  handle to MEM_SHARE allocated memory. may be NULL and
 *              memory will be automatically allocated.
 *  fDeleteOnRelease -- controls if the memory is freed on the last release.
 *  grfMode --  flags passed to StgCreateDocfileOnILockBytes
 *
 *  NOTE: if hGlobal is NULL, then a new IStorage is created and
 *              STGM_CREATE flag is passed to StgCreateDocfileOnILockBytes.
 *        if hGlobal is non-NULL, then it is assumed that the hGlobal already
 *              has an IStorage inside it and STGM_CONVERT flag is passed
 *              to StgCreateDocfileOnILockBytes.
 *
 * Return Value:
 *    SCODE  -  S_OK if successful
 */
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(LPSTORAGE) OleStdCreateStorageOnHGlobal(
        Handle hGlobal,
        unsigned long fDeleteOnRelease,
        unsigned long grfMode
)
{
    unsigned long grfCreateMode=grfMode | (hGlobal==NULL ? STGM_CREATE:STGM_CONVERT);
    HRESULT hrErr;
    LPLOCKBYTES lpLockBytes = NULL;
    unsigned long reserved = 0;
    LPSTORAGE lpStg = NULL;

    hrErr = CreateILockBytesOnHGlobal(
            hGlobal,
            fDeleteOnRelease,
            (LPLOCKBYTES *)&lpLockBytes
    );
    if (hrErr != NOERROR)
        return NULL;

    hrErr = StgCreateDocfileOnILockBytes(
            lpLockBytes,
            grfCreateMode,
            reserved,
            (LPSTORAGE *)&lpStg
    );
    if (hrErr != NOERROR) {
        OleStdRelease((LPUNKNOWN)lpLockBytes);
        return NULL;
    }
    return lpStg;
}



/*
 * OleStdCreateTempStorage()
 *
 * Purpose:
 *  Create a temporay IStorage* that will DeleteOnRelease.
 *  this can be either memory based or file based.
 *
 * Parameters:
 *  fUseMemory -- controls if memory-based or file-based stg is created
 *  grfMode --  storage mode flags
 *
 * Return Value:
 *    LPSTORAGE  -  if successful, NULL otherwise
 */
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(LPSTORAGE) OleStdCreateTempStorage(unsigned long fUseMemory, unsigned long grfMode)
{
    LPSTORAGE   lpstg;
    HRESULT     hrErr;
    unsigned long       reserved = 0;

    if (fUseMemory) {
        lpstg = OleStdCreateStorageOnHGlobal(
                NULL,  /* auto allocate */
                true,  /* delete on release */
                grfMode
        );
    } else {
        /* allocate a temp docfile that will delete on last release */
        hrErr = StgCreateDocfile(
                NULL,
                grfMode | STGM_DELETEONRELEASE | STGM_CREATE,
                reserved,
                &lpstg
        );
        if (hrErr != NOERROR)
            return NULL;
    }
    return lpstg;
}


/* OleStdGetOleObjectData
** ----------------------
**    Render CF_EMBEDSOURCE/CF_EMBEDDEDOBJECT data on an TYMED_ISTORAGE
**    medium by asking the object to save into the storage.
**    the object must support IPersistStorage.
**
**    if lpMedium->tymed == TYMED_NULL, then a delete-on-release
**    storage is allocated (either file-based or memory-base depending
**    the value of fUseMemory). this is useful to support an
**    IDataObject::GetData call where the callee must allocate the
**    medium.
**
**    if lpMedium->tymed == TYMED_ISTORAGE, then the data is writen
**    into the passed in IStorage. this is useful to support an
**    IDataObject::GetDataHere call where the caller has allocated his
**    own IStorage.
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI OleStdGetOleObjectData(
        LPPERSISTSTORAGE        lpPStg,
        LPFORMATETC             lpformatetc,
        LPSTGMEDIUM             lpMedium,
        unsigned long           fUseMemory)
{
#ifndef _MSC_VER
#pragma unused(fUseMemory)
#endif

    LPSTORAGE   lpstg = NULL;
    unsigned long       reserved = 0;
    SCODE       sc = S_OK;
    HRESULT     hrErr;

    lpMedium->pUnkForRelease = NULL;

    if (lpMedium->tymed == TYMED_NULL) {

        if (lpformatetc->tymed & TYMED_ISTORAGE) {

            /* allocate a temp docfile that will delete on last release */
            lpstg = OleStdCreateTempStorage(
                    true /*fUseMemory*/,
                    STGM_READWRITE | STGM_TRANSACTED | STGM_SHARE_EXCLUSIVE
            );
            if (!lpstg)
                return ResultFromScode(E_OUTOFMEMORY);

            lpMedium->STDMEDIUM_PSTG = lpstg;
            lpMedium->tymed = TYMED_ISTORAGE;
            lpMedium->pUnkForRelease = NULL;
        } else {
            return ResultFromScode(DATA_E_FORMATETC);
        }
    } else if (lpMedium->tymed == TYMED_ISTORAGE) {
        lpMedium->tymed = TYMED_ISTORAGE;
    } else {
        return ResultFromScode(DATA_E_FORMATETC);
    }

    // OLE2NOTE: even if OleSave returns an error you should still call
    // SaveCompleted.

    OLEDBG_BEGIN2("OleSave called\r\n")
    hrErr = OleSave(lpPStg, lpMedium->STDMEDIUM_PSTG, false /* fSameAsLoad */);
    OLEDBG_END2

    if (hrErr != NOERROR) {
        OleDbgOutHResult("WARNING: OleSave returned", hrErr);
        sc = GetScode(hrErr);
    }
    OLEDBG_BEGIN2("IPersistStorage::SaveCompleted called\r\n")
    hrErr = lpPStg->lpVtbl->SaveCompleted(lpPStg, NULL);
    OLEDBG_END2

    if (hrErr != NOERROR) {
        OleDbgOutHResult("WARNING: SaveCompleted returned",hrErr);
        if (sc == S_OK)
            sc = GetScode(hrErr);
    }

    return ResultFromScode(sc);
}


#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI OleStdGetLinkSourceData(
        LPMONIKER           lpmk,
        LPCLSID             lpClsID,
        LPFORMATETC         lpformatetc,
        LPSTGMEDIUM         lpMedium
)
{
    LPSTREAM    lpstm = NULL;
    unsigned long       reserved = 0;
    HRESULT     hrErr;

    if (lpMedium->tymed == TYMED_NULL) {
        if (lpformatetc->tymed & TYMED_ISTREAM) {
            hrErr = CreateStreamOnHGlobal(
                    NULL, /* auto allocate */
                    true, /* delete on release */
                    (LPSTREAM *)&lpstm
            );
            if (hrErr != NOERROR) {
                lpMedium->pUnkForRelease = NULL;
                return ResultFromScode(E_OUTOFMEMORY);
            }
            lpMedium->STGMEDIUM_PSTM = lpstm;
            lpMedium->tymed = TYMED_ISTREAM;
            lpMedium->pUnkForRelease = NULL;
        } else {
            lpMedium->pUnkForRelease = NULL;
            return ResultFromScode(DATA_E_FORMATETC);
        }
    } else {
        if (lpMedium->tymed == TYMED_ISTREAM) {
            lpMedium->tymed = TYMED_ISTREAM;
            lpMedium->STGMEDIUM_PSTM = lpMedium->STGMEDIUM_PSTM;
            lpMedium->pUnkForRelease = NULL;
        } else {
            lpMedium->pUnkForRelease = NULL;
            return ResultFromScode(DATA_E_FORMATETC);
        }
    }

    hrErr = OleSaveToStream((LPPERSISTSTREAM)lpmk, lpMedium->STGMEDIUM_PSTM);
    if (hrErr != NOERROR) return hrErr;
    return WriteClassStm(lpMedium->STGMEDIUM_PSTM, lpClsID);
}


/*
 * OleStdGetObjectDescriptorData
 *
 * Purpose:
 *  Fills and returns a OBJECTDESCRIPTOR structure.
 *  See OBJECTDESCRIPTOR for more information.
 *
 * Parameters:
 *  clsid           CLSID   CLSID of object being transferred
 *  dwAspect        unsigned long   Display Aspect of object
 *  sizel           SIZEL   Size of object in HIMETRIC
 *  pointl          POINTL  Offset from upper-left corner of object where mouse went
 *                          down for drag. Meaningful only when drag-drop is used.
 *  dwStatus        unsigned long   OLEMISC flags
 *  lpszFullUserTypeName  char* Full User Type Name
 *  lpszSrcOfCopy   char*   Source of Copy
 *
 * Return Value:
 *  HBGLOBAL         Handle to OBJECTDESCRIPTOR structure.
 */
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(Handle) OleStdGetObjectDescriptorData(
    CLSID     clsid,
    unsigned long     dwAspect,
    SIZEL     sizel,
    POINTL    pointl,
    unsigned long     dwStatus,
    char*     lpszFullUserTypeName,
    char*     lpszSrcOfCopy
)
{
    Handle            hMem = NULL;
    IBindCtx       *pbc = NULL;
    LPOBJECTDESCRIPTOR lpOD;
    unsigned long              dwObjectDescSize;
    unsigned long			   dwFullUserTypeNameLen;
    unsigned long			   dwSrcOfCopyLen;

    // Get the length of Full User Type Name; Add 1 for the null terminator
    dwFullUserTypeNameLen = lpszFullUserTypeName ? strlen(lpszFullUserTypeName)+1 : 0;

    // Get the Source of Copy string and it's length; Add 1 for the null terminator
    if (lpszSrcOfCopy)
       dwSrcOfCopyLen = strlen(lpszSrcOfCopy)+1;
    else {
       // No src moniker so use user type name as source string.
       lpszSrcOfCopy =  lpszFullUserTypeName;
       dwSrcOfCopyLen = dwFullUserTypeNameLen;
    }

    // Allocate space for OBJECTDESCRIPTOR and the additional string data
    dwObjectDescSize = sizeof(OBJECTDESCRIPTOR);
    hMem = NewHandle(dwObjectDescSize + dwFullUserTypeNameLen + dwSrcOfCopyLen);
    if (NULL == hMem)
        goto error;

    lpOD = (LPOBJECTDESCRIPTOR)*hMem;

    // Set the FullUserTypeName offset and copy the string
    if (lpszFullUserTypeName)
    {
        lpOD->dwFullUserTypeName = dwObjectDescSize;
        strcpy((char*)lpOD+lpOD->dwFullUserTypeName , lpszFullUserTypeName);
    }
    else lpOD->dwFullUserTypeName = 0;  // zero offset indicates that string is not present

    // Set the SrcOfCopy offset and copy the string
    if (lpszSrcOfCopy)
    {
        lpOD->dwSrcOfCopy = dwObjectDescSize + dwFullUserTypeNameLen;
        strcpy((char*)lpOD+lpOD->dwSrcOfCopy , lpszSrcOfCopy);
    }
    else lpOD->dwSrcOfCopy = 0;  // zero offset indicates that string is not present

    // Initialize the rest of the OBJECTDESCRIPTOR
    lpOD->cbSize       = dwObjectDescSize + dwFullUserTypeNameLen + dwSrcOfCopyLen;
    lpOD->clsid        = clsid;
    lpOD->dwDrawAspect = dwAspect;
    lpOD->sizel        = sizel;
    lpOD->pointl       = pointl;
    lpOD->dwStatus     = dwStatus;
    lpOD->dwOutline    = 0;
    lpOD->dwExtra	   = 0;

    HUnlock(hMem);
    return hMem;

error:
   if (hMem)
   {
       HUnlock(hMem);
       DisposHandle(hMem);
   }
   return NULL;
}

/*
 * OleStdGetObjectDescriptorDataFromOleObject
 *
 * Purpose:
 *  Fills and returns a OBJECTDESCRIPTOR structure. Information for the structure is
 *  obtained from an OLEOBJECT.
 *  See OBJECTDESCRIPTOR for more information.
 *
 * Parameters:
 *  lpOleObj        LPOLEOBJECT OleObject from which OBJECTDESCRIPTOR info
 *                  is obtained.
 *  lpszSrcOfCopy   char* string to identify source of copy.
 *                  May be NULL in which case IOleObject::GetMoniker is called
 *                  to get the moniker of the object. if the object is loaded
 *                  as part of a data transfer document, then usually
 *                  lpOleClientSite==NULL is passed to OleLoad when loading
 *                  the object. in this case the IOleObject:GetMoniker call
 *                  will always fail (it tries to call back to the object's
 *                  client site). in this situation a non-NULL lpszSrcOfCopy
 *                  parameter should be passed.
 *  dwAspect        unsigned long   Display Aspect of object
 *  pointl          POINTL  Offset from upper-left corner of object where mouse went
 *                          down for drag. Meaningful only when drag-drop is used.
 *
 * Return Value:
 *  HBGLOBAL         Handle to OBJECTDESCRIPTOR structure.
 */

#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(Handle) OleStdGetObjectDescriptorDataFromOleObject(
        LPOLEOBJECT lpOleObj,
        char*       lpszSrcOfCopy,
        unsigned long       dwAspect,
        POINTL      pointl
)
{
    CLSID clsid;
    char* lpszFullUserTypeName = NULL;
    LPMONIKER lpSrcMonikerOfCopy = NULL;
    Handle hObjDesc;
    IBindCtx    *pbc = NULL;
    HRESULT hrErr;
    SIZEL sizel;
    unsigned long  fFreeSrcOfCopy = false;
    unsigned long dwStatus = 0;

    // Get CLSID
    OLEDBG_BEGIN2("IOleObject::GetUserClassID called\r\n")
    hrErr = lpOleObj->lpVtbl->GetUserClassID(lpOleObj, &clsid);
    OLEDBG_END2
    if (hrErr != NOERROR)
        clsid = CLSID_NULL;

    // Get FullUserTypeName
    OLEDBG_BEGIN2("IOleObject::GetUserType called\r\n")
    hrErr = lpOleObj->lpVtbl->GetUserType(
            lpOleObj,
            USERCLASSTYPE_FULL,
            (char* *)&lpszFullUserTypeName
    );
    OLEDBG_END2

    // Get Source Of Copy
    if (lpszSrcOfCopy == NULL) {
        OLEDBG_BEGIN2("IOleObject::GetMoniker called\r\n")
        hrErr = lpOleObj->lpVtbl->GetMoniker(
                lpOleObj,
                OLEGETMONIKER_TEMPFORUSER,
                OLEWHICHMK_OBJFULL,
                (LPMONIKER *)&lpSrcMonikerOfCopy
        );
        OLEDBG_END2
        if (hrErr == NOERROR)
        {
            CreateBindCtx(0, (LPBC *)&pbc);
            lpSrcMonikerOfCopy->lpVtbl->GetDisplayName(
                    lpSrcMonikerOfCopy, pbc, NULL, &lpszSrcOfCopy);
            pbc->lpVtbl->Release(pbc);
            fFreeSrcOfCopy = true;
        }
    }

    // Get SIZEL
    OLEDBG_BEGIN2("IOleObject::GetExtent called\r\n")
    hrErr = lpOleObj->lpVtbl->GetExtent(
                lpOleObj,
                dwAspect,
                &sizel
    );
    OLEDBG_END2
    if (hrErr != NOERROR)
        sizel.cx = sizel.cy = 0;

    // Get DWSTATUS
    OLEDBG_BEGIN2("IOleObject::GetMiscStatus called\r\n")
    hrErr = lpOleObj->lpVtbl->GetMiscStatus(
                lpOleObj,
                dwAspect,
                &dwStatus
    );
    OLEDBG_END2
    if (hrErr != NOERROR)
        dwStatus = 0;

    // Get OBJECTDESCRIPTOR
    hObjDesc = OleStdGetObjectDescriptorData(
            clsid,
            dwAspect,
            sizel,
            pointl,
            dwStatus,
            lpszFullUserTypeName,
            lpszSrcOfCopy
    );
    if (! hObjDesc)
        goto error;

    // Clean up
    if (lpszFullUserTypeName)
        OleStdFreeString(lpszFullUserTypeName, NULL);
    if (fFreeSrcOfCopy && lpszSrcOfCopy)
        OleStdFreeString(lpszSrcOfCopy, NULL);
    if (lpSrcMonikerOfCopy)
        OleStdRelease((LPUNKNOWN)lpSrcMonikerOfCopy);

    return hObjDesc;

error:
    if (lpszFullUserTypeName)
        OleStdFreeString(lpszFullUserTypeName, NULL);
    if (fFreeSrcOfCopy && lpszSrcOfCopy)
        OleStdFreeString(lpszSrcOfCopy, NULL);
    if (lpSrcMonikerOfCopy)
        OleStdRelease((LPUNKNOWN)lpSrcMonikerOfCopy);

    return NULL;
}

/*
 * OleStdFillObjectDescriptorFromData
 *
 * Purpose:
 *  Fills and returns a OBJECTDESCRIPTOR structure. The source object will offer CF_OBJECTDESCRIPTOR
 *  if it is an OLE2 object, CF_OWNERLINK if it is an OLE1 object, or CF_FILENAME if it has been
 *  copied to the clipboard by FileManager.
 *
 * Parameters:
 *  lpDataObject    LPDATAOBJECT Source object
 *  lpmedium        LPSTGMEDIUM  Storage medium
 *  lpcfFmt         ResType  * Format offered by lpDataObject (OUT parameter)
 *
 * Return Value:
 *  HBGLOBAL         Handle to OBJECTDESCRIPTOR structure.
 */

#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(Handle) OleStdFillObjectDescriptorFromData(
        LPDATAOBJECT     lpDataObject,
        LPSTGMEDIUM      lpmedium,
        ResType * lpcfFmt
)
{
    CLSID              clsid;
    SIZEL              sizel;
    POINTL             pointl;
    char               *lpsz = NULL;
	char			   *szFullUserTypeName, *szSrcOfCopy, *szClassName, *szDocName, *szItemName;
    int                nClassName, nDocName, nItemName, nFullUserTypeName;
    char               *szBuf = NULL;
    Handle             hMem = NULL;
    HKEY               hKey = (HKEY)NULL;
    LPMALLOC           pIMalloc = NULL;
    unsigned long      dw = OLEUI_CCHKEYMAX;
    Handle             hObjDesc;
    HRESULT            hrErr;


    // GetData CF_OBJECTDESCRIPTOR format from the object on the clipboard.
    // Only OLE 2 objects on the clipboard will offer CF_OBJECTDESCRIPTOR
    if (NULL != (hMem = OleStdGetData(lpDataObject,
		cfObjectDescriptor, NULL, DVASPECT_CONTENT, lpmedium)))
    {
        *lpcfFmt = cfObjectDescriptor;
        return hMem;  // Don't drop to clean up at the end of this function
    }
    // If CF_OBJECTDESCRIPTOR is not available, i.e. if this is not an OLE2 object,
    //     check if this is an OLE 1 object. OLE 1 objects will offer CF_OWNERLINK
    else if (NULL != (hMem = OleStdGetData(lpDataObject,
		cfOwnerLink, NULL, DVASPECT_CONTENT, lpmedium)))
    {
        *lpcfFmt = cfOwnerLink;
        // CF_OWNERLINK contains null-terminated strings for class name, document name
        // and item name with two null terminating characters at the end
        HLock(hMem);
        szClassName = *hMem;
        nClassName = strlen(szClassName);
        szDocName   = szClassName + nClassName + 1;
        nDocName   = strlen(szDocName);
        szItemName  = szDocName + nDocName + 1;
        nItemName  =  strlen(szItemName);

        hrErr = CoGetMalloc(MEMCTX_TASK, &pIMalloc);
        if (hrErr != NOERROR)
            goto error;

        // Find FullUserTypeName from Registration database using class name
        if (RegOpenKey(HKEY_CLASSES_ROOT, NULL, &hKey) != ERROR_SUCCESS)
           goto error;

        // Allocate space for szFullUserTypeName & szSrcOfCopy. Maximum length of FullUserTypeName
        // is OLEUI_CCHKEYMAX. SrcOfCopy is constructed by concatenating FullUserTypeName, Document
        // Name and ItemName separated by spaces.
        szBuf = (char*)pIMalloc->lpVtbl->Alloc(pIMalloc,
                            (unsigned long)2*OLEUI_CCHKEYMAX+nDocName+nItemName+4);
        if (NULL == szBuf)
            goto error;
        szFullUserTypeName = szBuf;
        szSrcOfCopy = szFullUserTypeName+OLEUI_CCHKEYMAX+1;

        // Get FullUserTypeName
        if (RegQueryValue(hKey, lpsz, szFullUserTypeName, (long *)&dw) != ERROR_SUCCESS)
           goto error;

        // Build up SrcOfCopy string from FullUserTypeName, DocumentName & ItemName
        lpsz = szSrcOfCopy;
        strcpy(lpsz, szFullUserTypeName);
        nFullUserTypeName = strlen(szFullUserTypeName);
        lpsz[nFullUserTypeName]=' ';
        lpsz += nFullUserTypeName+1;
        strcpy(lpsz, szDocName);
        lpsz[nDocName] = ' ';
        lpsz += nDocName+1;
        strcpy(lpsz, szItemName);

        sizel.cx = sizel.cy = 0;
        pointl.x = pointl.y = 0;

        CLSIDFromProgID(szClassName, &clsid);

        hObjDesc = OleStdGetObjectDescriptorData(
                clsid,
                DVASPECT_CONTENT,
                sizel,
                pointl,
                0,
                szFullUserTypeName,
                szSrcOfCopy
        );
        if (!hObjDesc)
           goto error;
     }
     // Check if object is CF_FILENAME
     else if (NULL != (hMem = OleStdGetData(lpDataObject,
     	cfFileName, NULL, DVASPECT_CONTENT, lpmedium)))
     {
         *lpcfFmt = cfFileName;
         HLock(hMem);
         lpsz = *hMem;
         hrErr = GetClassFile(lpsz, &clsid);

         /* OLE2NOTE: if the file does not have an OLE class
         **    associated, then use the OLE 1 Packager as the class of
         **    the object to be created. this is the behavior of
         **    OleCreateFromData API
         */
         if (hrErr != NOERROR)
            CLSIDFromProgID("Package", &clsid);
         sizel.cx = sizel.cy = 0;
         pointl.x = pointl.y = 0;

         hrErr = CoGetMalloc(MEMCTX_TASK, &pIMalloc);
         if (hrErr != NOERROR)
            goto error;
         szBuf = (char*)pIMalloc->lpVtbl->Alloc(pIMalloc, (unsigned long)OLEUI_CCHKEYMAX);
         if (NULL == szBuf)
            goto error;

         OleStdGetUserTypeOfClass(&clsid, szBuf, OLEUI_CCHKEYMAX, (HKEY)NULL);

         hObjDesc = OleStdGetObjectDescriptorData(
                clsid,
                DVASPECT_CONTENT,
                sizel,
                pointl,
                0,
                szBuf,
                lpsz
        );
        if (!hObjDesc)
           goto error;
     }
     else goto error;

     // Clean up
     if (szBuf)
         pIMalloc->lpVtbl->Free(pIMalloc, (void*)szBuf);
     if (pIMalloc)
         OleStdRelease((LPUNKNOWN)pIMalloc);
     if (hMem)
     {
         HUnlock(hMem);
         DisposeHandle(hMem);
     }
     if (hKey)
         RegCloseKey(hKey);
     return hObjDesc;

error:
   if (szBuf)
       pIMalloc->lpVtbl->Free(pIMalloc, (void*)szBuf);
   if (pIMalloc)
       OleStdRelease((LPUNKNOWN)pIMalloc);
     if (hMem)
     {
         HUnlock(hMem);
         DisposeHandle(hMem);
     }
     if (hKey)
         RegCloseKey(hKey);
     return NULL;
}

#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI OleStdQueryOleObjectData(LPFORMATETC lpformatetc)
{
    if (lpformatetc->tymed & TYMED_ISTORAGE) {
        return NOERROR;
    } else {
        return ResultFromScode(DATA_E_FORMATETC);
    }
}

#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI OleStdQueryLinkSourceData(LPFORMATETC lpformatetc)
{
    if (lpformatetc->tymed & TYMED_ISTREAM)
        return NOERROR;
    else
        return ResultFromScode(DATA_E_FORMATETC);
}


#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI OleStdQueryObjectDescriptorData(LPFORMATETC lpformatetc)
{
    if (lpformatetc->tymed & TYMED_HGLOBAL) {
        return NOERROR;
    } else {
        return ResultFromScode(DATA_E_FORMATETC);
    }
}


#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI OleStdQueryFormatMedium(LPFORMATETC lpformatetc, TYMED tymed)
{
    if (lpformatetc->tymed & tymed) {
        return NOERROR;
    } else {
        return ResultFromScode(DATA_E_FORMATETC);
    }
}


#ifdef LATER

/*
 * OleStdCopyMetafilePict()
 *
 * Purpose:
 *    Make an independent copy of a MetafilePict
 * Parameters:
 *
 * Return Value:
 *    true if successful, else false.
 */
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(unsigned long) OleStdCopyMetafilePict(Handle hpictin, Handle * phpictout)
{
    Handle hpictout;
    LPMETAFILEPICT ppictin, ppictout;

    if (hpictin == NULL || phpictout == NULL) {
        OleDbgAssert(hpictin == NULL || phpictout == NULL);
        return false;
    }

    *phpictout = NULL;

    if ((ppictin = (LPMETAFILEPICT)GlobalLock(hpictin)) == NULL) {
        return false;
    }

    hpictout = GlobalAlloc(GHND|GMEM_SHARE, sizeof(METAFILEPICT));

    if (hpictout && (ppictout = (LPMETAFILEPICT)GlobalLock(hpictout))){
        ppictout->hMF  = CopyMetaFile(ppictin->hMF, NULL);
        ppictout->xExt = ppictin->xExt;
        ppictout->yExt = ppictin->yExt;
        ppictout->mm   = ppictin->mm;
        HUnlock(hpictout);
    }

    *phpictout = hpictout;

    return true;

}

#endif // LATER


/*
 * OleStdGetMetafilePictFromOleObject()
 *
 * Purpose:
 *      Generate a MetafilePict by drawing the OLE object.
 * Parameters:
 *
 * Return Value:
 *    HANDLE    -- handle of allocated METAFILEPICT
 */
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(Handle) OleStdGetMetafilePictFromOleObject(
        LPOLEOBJECT         lpOleObj,
        unsigned long       dwDrawAspect
)
{
    LPVIEWOBJECT 	lpViewObj = NULL;
	GrafPtr			oldPort;
	GrafPort		pictPort;
	CGrafPort		cpictPort;
    PicHandle 		pictHandle;
    Rect 			rcHim;
    RECTL 			rclHim;
    SIZEL 			sizelHim;
    HRESULT 		hrErr;
    OSErr			err;
    long			result;

	// Determine the Quick Draw version of the system
	err = Gestalt(gestaltQuickdrawVersion, &result);
	result = (err == noErr) ? (result >> 8) : 0;
		
    lpViewObj = (LPVIEWOBJECT)OleStdQueryInterface(
            (LPUNKNOWN)lpOleObj, &IID_IViewObject);
    if (! lpViewObj)
        return NULL;

    OLEDBG_BEGIN2("IOleObject::GetExtent called\r\n")
    lpOleObj->lpVtbl->GetExtent(
                lpOleObj,
                dwDrawAspect,
                (LPSIZEL)&sizelHim
    );
    OLEDBG_END2

    rclHim.left     = 0;
    rclHim.top      = 0;
    rclHim.right    = sizelHim.cx;
    rclHim.bottom   = sizelHim.cy;

    rcHim.left      = (int)rclHim.left;
    rcHim.top       = (int)rclHim.top;
    rcHim.right     = (int)rclHim.right;
    rcHim.bottom    = (int)rclHim.bottom;

	GetPort(&oldPort);
	
	if (result == 0)
		OpenPort(&pictPort);
	else
		OpenCPort(&cpictPort);
	ClipRect(&rcHim);
	
	pictHandle = OpenPicture(&rcHim);

    OLEDBG_BEGIN2("IViewObject::Draw called\r\n")
    hrErr = lpViewObj->lpVtbl->Draw(
            lpViewObj,
            dwDrawAspect,
            -1,
            NULL,
            NULL,
            NULL,
            (result == 0) ? (GrafPtr)&pictPort : (GrafPtr)&cpictPort,
            (LPRECTL)&rclHim,
            (LPRECTL)&rclHim,
            NULL,
            0
    );
    OLEDBG_END2

    OleStdRelease((LPUNKNOWN)lpViewObj);
    OleDbgAssert(hrErr == NOERROR);

	ClosePicture();

	if (result == 0)
		ClosePort(&pictPort);
	else	
		CloseCPort(&cpictPort);
	SetPort(oldPort);
	
	return (Handle)pictHandle;
}


/* Call Release on the object that is expected to go away.
**      if the refcnt of the object did no go to 0 then give a debug message.
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(unsigned long) OleStdVerifyRelease(LPUNKNOWN lpUnk)
{
    unsigned long cRef;

    cRef = lpUnk->lpVtbl->Release(lpUnk);

    return cRef;
}


/* Call Release on the object that is NOT necessarily expected to go away.
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(unsigned long) OleStdRelease(LPUNKNOWN lpUnk)
{
    unsigned long cRef;

    cRef = lpUnk->lpVtbl->Release(lpUnk);

#if defined( _DEBUG )
    {
      char szBuf[80];
        	sprintf(
               (char*)szBuf,
                "refcnt = %ld after object (0x%lx) release\n",
                cRef,
                lpUnk
      		);
       OleDbgOut4((char*)szBuf);
    }
#endif


    return cRef;
}

#ifdef LATER

/* OleStdInitVtbl
** --------------
**
**    Initialize an interface Vtbl to ensure that there are no NULL
**    function pointers in the Vtbl. All entries in the Vtbl are
**    set to a valid funtion pointer (OleStdNullMethod) that issues
**        debug assert message (message box) and returns E_NOTIMPL if called.
**
**    NOTE: this funtion does not initialize the Vtbl with usefull
**    function pointers, only valid function pointers to avoid the
**    horrible run-time crash when a call is made through the Vtbl with
**    a NULL function pointer. this API is only necessary when
**    initializing the Vtbl's in C. C++ guarantees that all interface
**    functions (in C++ terms -- pure virtual functions) are implemented.
*/

#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(void) OleStdInitVtbl(void* lpVtbl, unsigned int nSizeOfVtbl)
{
    void* * lpFuncPtrArr = (void* *)lpVtbl;
    unsigned int nMethods = nSizeOfVtbl/sizeof(void *);
    unsigned int i;

    for (i = 0; i < nMethods; i++) {
        lpFuncPtrArr[i] = OleStdNullMethod;
    }
}


/* OleStdCheckVtbl
** ---------------
**
**    Check if all entries in the Vtbl are properly initialized with
**    valid function pointers. If any entries are either NULL or
**    OleStdNullMethod, then this function returns false. If compiled
**    for _DEBUG this function reports which function pointers are
**    invalid.
**
**    RETURNS:  true if all entries in Vtbl are valid
**                              false otherwise.
*/

#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(unsigned long) OleStdCheckVtbl(void* lpVtbl, unsigned int nSizeOfVtbl, char* lpszIface)
{
    void* * lpFuncPtrArr = (void* *)lpVtbl;
    unsigned int nMethods = nSizeOfVtbl/sizeof(void *);
    unsigned int i;
    char szBuf[80];
    unsigned long fStatus = true;
    int nChar = 0;

    for (i = 0; i < nMethods; i++) {
        if (lpFuncPtrArr[i] == NULL || lpFuncPtrArr[i] == OleStdNullMethod) {
            sprintf(szBuf, "%s::method# %d NOT valid!", lpszIface, i);
            OleDbgOut1((char*)szBuf);
            fStatus = false;
        }
    }
    return fStatus;
}


/* OleStdNullMethod
** ----------------
**    Dummy method used by OleStdInitVtbl to initialize an interface
**    Vtbl to ensure that there are no NULL function pointers in the
**    Vtbl. All entries in the Vtbl are set to this function. this
**    function issues a debug assert message (message box) and returns
**    E_NOTIMPL if called. If all is done properly, this function will
**    NEVER be called!
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDMETHODIMP OleStdNullMethod(LPUNKNOWN lpThis)
{
    MessageBox(
            NULL,
            "ERROR: INTERFACE METHOD NOT IMPLEMENTED!\r\n",
            NULL,
            MB_SYSTEMMODAL | MB_ICONHAND | MB_OK
    );

    return ResultFromScode(E_NOTIMPL);
}



#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
static unsigned long  GetFileTimes(char* lpszFileName, FILETIME * pfiletime)
{
    static char sz[256];
    static struct _find_t fileinfo;

    _fstrcpy((char*)sz, lpszFileName);
    return (_dos_findfirst(sz,_A_NORMAL|_A_HIDDEN|_A_SUBDIR|_A_SYSTEM,
                     (struct _find_t *)&fileinfo) == 0 &&
        CoDosDateTimeToFileTime(fileinfo.wr_date,fileinfo.wr_time,pfiletime));
}

#endif // LATER


/* OleStdRegisterAsRunning
** -----------------------
**    Register a moniker in the RunningObjectTable.
**    if there is an existing registration (*lpdwRegister!=NULL), then
**    first revoke that registration.
**
**    new dwRegister key is returned via *lpdwRegister parameter.
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(void) OleStdRegisterAsRunning(LPUNKNOWN lpUnk, LPMONIKER lpmkFull, unsigned long * lpdwRegister)
{
    LPRUNNINGOBJECTTABLE lpROT;
    HRESULT hrErr;

    OLEDBG_BEGIN2("OleStdRegisterAsRunning\r\n")

    OLEDBG_BEGIN2("GetRunningObjectTable called\r\n")
    hrErr = GetRunningObjectTable(0,(LPRUNNINGOBJECTTABLE *)&lpROT);
    OLEDBG_END2

    if (hrErr == NOERROR) {

        // if already registered, revoke
        if (*lpdwRegister != 0) {


            OLEDBG_BEGIN2("IRunningObjectTable::Revoke called\r\n")
            lpROT->lpVtbl->Revoke(lpROT, *lpdwRegister);
            OLEDBG_END2

            *lpdwRegister = 0;
        }

        // register as running if a valid moniker is passed
        if (lpmkFull) {

            OLEDBG_BEGIN2("IRunningObjectTable::Register called\r\n")
            lpROT->lpVtbl->Register(lpROT, 0, lpUnk,lpmkFull,lpdwRegister);
            OLEDBG_END2

        }

        OleStdRelease((LPUNKNOWN) lpROT);
    } else {
        OleDbgAssertSz(
                lpROT != NULL,
                "OleStdRegisterAsRunning: GetRunningObjectTable FAILED\r\n"
        );
    }

    OLEDBG_END2
}



/* OleStdRevokeAsRunning
** ---------------------
**    Revoke a moniker from the RunningObjectTable if there is an
**    existing registration (*lpdwRegister!=NULL).
**
**    *lpdwRegister parameter will be set to NULL.
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(void) OleStdRevokeAsRunning(unsigned long * lpdwRegister)
{
    LPRUNNINGOBJECTTABLE lpROT;
    HRESULT hrErr;

    OLEDBG_BEGIN2("OleStdRevokeAsRunning\r\n")

    // if still registered, then revoke
    if (*lpdwRegister != 0) {

        OLEDBG_BEGIN2("GetRunningObjectTable called\r\n")
        hrErr = GetRunningObjectTable(0,(LPRUNNINGOBJECTTABLE *)&lpROT);
        OLEDBG_END2

        if (hrErr == NOERROR) {

#if _DEBUGLEVEL >= 2
            {
                char szBuf[80];

                sprintf(
                        szBuf,
                        "Moniker [0x%lx] REVOKED from ROT\r\n",
                        *lpdwRegister
                );
                OleDbgOut2(szBuf);
            }
#endif  // _DEBUGLEVEL >= 2

            OLEDBG_BEGIN2("IRunningObjectTable::Revoke called\r\n")
            lpROT->lpVtbl->Revoke(lpROT, *lpdwRegister);
            OLEDBG_END2

            *lpdwRegister = 0;

            OleStdRelease((LPUNKNOWN)lpROT);
        } else {
            OleDbgAssertSz(
                    lpROT != NULL,
                    "OleStdRevokeAsRunning: GetRunningObjectTable FAILED\r\n"
            );
        }
    }
    OLEDBG_END2
}


/* OleStdNoteFileChangeTime
** ------------------------
**    Note the time a File-Based object has been saved in the
**    RunningObjectTable. These change times are used as the basis for
**    IOleObject::IsUpToDate.
**    It is important to set the time of the file-based object
**    following a save operation to exactly the time of the saved file.
**    this helps IOleObject::IsUpToDate to give the correct answer
**    after a file has been saved.
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(void) OleStdNoteFileChangeTime(char* lpszFileName, unsigned long dwRegister)
{
#ifndef _MSC_VER
#pragma unused(lpszFileName)
#endif

    if (dwRegister != 0) {

        LPRUNNINGOBJECTTABLE lprot;
        FILETIME filetime;

        if (GetRunningObjectTable(0,&lprot) == NOERROR)
        {
            lprot->lpVtbl->NoteChangeTime( lprot, dwRegister, &filetime );
            lprot->lpVtbl->Release(lprot);

            OleDbgOut2("IRunningObjectTable::NoteChangeTime called\r\n");
        }
    }
}


/* OleStdNoteObjectChangeTime
** --------------------------
**    Set the last change time of an object that is registered in the
**    RunningObjectTable. These change times are used as the basis for
**    IOleObject::IsUpToDate.
**
**    every time the object sends out a OnDataChange notification, it
**    should update the Time of last change in the ROT.
**
**    NOTE: this function set the change time to the current time.
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(void) OleStdNoteObjectChangeTime(unsigned long dwRegister)
{
    if (dwRegister != 0) {

        LPRUNNINGOBJECTTABLE lprot;
        FILETIME filetime;

        if (GetRunningObjectTable(0,&lprot) == NOERROR)
        {
            CoFileTimeNow( &filetime );
            lprot->lpVtbl->NoteChangeTime( lprot, dwRegister, &filetime );
            lprot->lpVtbl->Release(lprot);

            OleDbgOut2("IRunningObjectTable::NoteChangeTime called\r\n");
        }
    }
}


/* OleStdCreateTempFileMoniker
** ---------------------------
**    return the next available FileMoniker that can be used as the
**    name of an untitled document.
**    the FileMoniker is built of the form:
**          <lpszPrefixString><number>
**      eg. "Outline1", "Outline2", etc.
**
**    The RunningObjectTable (ROT) is consulted to determine if a
**    FileMoniker is in use. If the name is in use then the number is
**    incremented and the ROT is checked again.
**
** Parameters:
**    FSSpecPtr pPrefixFSP      - (IN-OUT) prefix (with path info) used to build the name
**								- generated filename will be returned here
**    UINT FAR* lpuUnique       - (IN-OUT) last used number.
**                                  this number is used to make the
**                                  name unique. on entry, the input
**                                  number is incremented. on output,
**                                  the number used is returned. this
**                                  number should be passed again
**                                  unchanged on the next call.
**    LPMONIKER FAR* lplpmk     - (OUT) next unused FileMoniker
**
** Returns:
**    void
**
** Comments:
**    This function is similar in spirit to the Windows API
**    CreateTempFileName.
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(void) OleStdCreateTempFileMoniker(FSSpecPtr pPrefixFSP, unsigned int* lpuUnique, LPMONIKER* lplpmk)
{
    LPRUNNINGOBJECTTABLE 	lpROT = NULL;
    unsigned int 			i = (lpuUnique ? *lpuUnique : 1);
    HRESULT 				hrErr;
    char 					name[64];

	if (!pPrefixFSP || !lplpmk)
		return;
		
	strcpy(name, (char*)p2cstr(pPrefixFSP->name));
	
	if (i == 1) {
		strcpy((char*)pPrefixFSP->name, name);
		i++;
	}
	else
	    sprintf((char*)pPrefixFSP->name, "%s%lu", name, i++);
    c2pstr((char*)pPrefixFSP->name);
    CreateFileMonikerFSp(pPrefixFSP, lplpmk);

    OLEDBG_BEGIN2("GetRunningObjectTable called\r\n")
    hrErr = GetRunningObjectTable(0,(LPRUNNINGOBJECTTABLE *)&lpROT);
    OLEDBG_END2

    if (hrErr == NOERROR) {

        while (1) {
            if (! *lplpmk)
                break;  // failed to create FileMoniker

            OLEDBG_BEGIN2("IRunningObjectTable::IsRunning called\r\n")
            hrErr = lpROT->lpVtbl->IsRunning(lpROT,*lplpmk);
            OLEDBG_END2

            if (hrErr != NOERROR)
                break;  // the Moniker is NOT running; found unused one!

            OleStdVerifyRelease((LPUNKNOWN)*lplpmk);

			if (i == 1) {
				strcpy((char*)pPrefixFSP->name, name);
				i++;
			}
			else
			    sprintf((char*)pPrefixFSP->name, "%s%lu", name, i++);
		    c2pstr((char*)pPrefixFSP->name);
		    CreateFileMonikerFSp(pPrefixFSP, lplpmk);
        }

        OleStdRelease((LPUNKNOWN)lpROT);
    }

    if (lpuUnique != NULL)
        *lpuUnique = i;
}



/* OleStdGetFirstMoniker
** ---------------------
**    return the first piece of a moniker.
**
**    NOTE: if the given moniker is not a generic composite moniker,
**    then an AddRef'ed pointer to the given moniker is returned.
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(LPMONIKER) OleStdGetFirstMoniker(LPMONIKER lpmk)
{
    LPMONIKER       lpmkFirst = NULL;
    LPENUMMONIKER   lpenumMoniker;
    unsigned long           dwMksys;
    HRESULT         hrErr;

    if (! lpmk)
        return NULL;

    if (lpmk->lpVtbl->IsSystemMoniker(lpmk, (unsigned long*)&dwMksys) == NOERROR
        && dwMksys == MKSYS_GENERICCOMPOSITE) {

        /* OLE2NOTE: the moniker is a GenericCompositeMoniker.
        **    enumerate the moniker to pull off the first piece.
        */

        hrErr = lpmk->lpVtbl->Enum(
                lpmk,
                true /* fForward */,
                (LPENUMMONIKER *)&lpenumMoniker
        );
        if (hrErr != NOERROR)
            return NULL;    // ERROR: give up!

        hrErr = lpenumMoniker->lpVtbl->Next(
                lpenumMoniker,
                1,
                (LPMONIKER *)&lpmkFirst,
                NULL
        );
        lpenumMoniker->lpVtbl->Release(lpenumMoniker);
        return lpmkFirst;

    } else {
        /* OLE2NOTE: the moniker is NOT a GenericCompositeMoniker.
        **    return an AddRef'ed pointer to the input moniker.
        */
        lpmk->lpVtbl->AddRef(lpmk);
        return lpmk;
    }
}


/* OleStdGetLenFilePrefixOfMoniker
** -------------------------------
**    if the first piece of the Moniker is a FileMoniker, then return
**    the length of the filename string.
**
**    lpmk      pointer to moniker
**
**    Returns
**      0       if moniker does NOT start with a FileMoniker
**      uLen    string length of filename prefix of the display name
**              retrieved from the given (lpmk) moniker.
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(unsigned long) OleStdGetLenFilePrefixOfMoniker(LPMONIKER lpmk)
{
    LPMONIKER       lpmkFirst = NULL;
    unsigned long           dwMksys;
    char*           lpsz = NULL;
    LPBC            lpbc = NULL;
    unsigned long           uLen = 0;
    HRESULT         hrErr;

    if (! lpmk)
        return 0;

    lpmkFirst = OleStdGetFirstMoniker(lpmk);
    if (lpmkFirst) {
        if ( (lpmkFirst->lpVtbl->IsSystemMoniker(
                            lpmkFirst, (unsigned long*)&dwMksys) == NOERROR)
                && dwMksys == MKSYS_FILEMONIKER) {

            hrErr = CreateBindCtx(0, (LPBC *)&lpbc);
            if (hrErr == NOERROR) {
                lpmkFirst->lpVtbl->GetDisplayName(
                        lpmkFirst,
                        lpbc,
                        NULL,   /* pmkToLeft */
                        (char* *)&lpsz
                );
                uLen = strlen(lpsz);
                OleStdFreeString(lpsz, NULL);
                OleStdRelease((LPUNKNOWN)lpbc);
            }
        }
        lpmkFirst->lpVtbl->Release(lpmkFirst);
    }
    return uLen;
}

/*
 * OleStdMarkPasteEntryList
 *
 * Purpose:
 *  Mark each entry in the PasteEntryList if its format is available from
 *  the source IDataObject*. the dwScratchSpace field of each PasteEntry
 *  is set to true if available, else false.
 *
 * Parameters:
 *  LPOLEUIPASTEENTRY   array of PasteEntry structures
 *  int                 count of elements in PasteEntry array
 *  LPDATAOBJECT        source IDataObject* pointer
 *
 * Return Value:
 *   none
 */
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(void) OleStdMarkPasteEntryList(
        LPDATAOBJECT        lpSrcDataObj,
        LPOLEUIPASTEENTRY   lpPriorityList,
        int                 cEntries
)
{
    LPENUMFORMATETC     lpEnumFmtEtc = NULL;
    FORMATETC           fmtetc;
    int                 i;
    HRESULT             hrErr;

    // Clear all marks
    for (i = 0; i < cEntries; i++) {
        lpPriorityList[i].dwScratchSpace = false;

        if (! lpPriorityList[i].fmtetc.cfFormat) {
            // caller wants this item always considered available
            // (by specifying a NULL format)
            lpPriorityList[i].dwScratchSpace = true;
        } else if (lpPriorityList[i].fmtetc.cfFormat == cfEmbeddedObject
                || lpPriorityList[i].fmtetc.cfFormat == cfEmbedSource
                || lpPriorityList[i].fmtetc.cfFormat == cfFileName) {

            // if there is an OLE object format, then handle it
            // specially by calling OleQueryCreateFromData. the caller
            // need only specify one object type format.
            OLEDBG_BEGIN2("OleQueryCreateFromData called\r\n")
            hrErr = OleQueryCreateFromData(lpSrcDataObj);
            OLEDBG_END2
            if(NOERROR == hrErr) {
                lpPriorityList[i].dwScratchSpace = true;
            }
        } else if (lpPriorityList[i].fmtetc.cfFormat == cfLinkSource) {

            // if there is OLE 2.0 LinkSource format, then handle it
            // specially by calling OleQueryLinkFromData.
            OLEDBG_BEGIN2("OleQueryLinkFromData called\r\n")
            hrErr = OleQueryLinkFromData(lpSrcDataObj);
            OLEDBG_END2
            if(NOERROR == hrErr) {
                lpPriorityList[i].dwScratchSpace = true;
            }
        }
    }

    OLEDBG_BEGIN2("IDataObject::EnumFormatEtc called\r\n")
    hrErr = lpSrcDataObj->lpVtbl->EnumFormatEtc(
            lpSrcDataObj,
            DATADIR_GET,
            (LPENUMFORMATETC *)&lpEnumFmtEtc
    );
    OLEDBG_END2

    if (hrErr != NOERROR)
        return;    // unable to get format enumerator

    // Enumerate the formats offered by the source
    // Loop over all formats offered by the source
    while (lpEnumFmtEtc->lpVtbl->Next(
            lpEnumFmtEtc, 1, (LPFORMATETC)&fmtetc, NULL) == S_OK)
    {
        for (i = 0; i < cEntries; i++)
        {
            if (! lpPriorityList[i].dwScratchSpace &&
                IsEqualFORMATETC(fmtetc, lpPriorityList[i].fmtetc))
            {
                lpPriorityList[i].dwScratchSpace = true;
            }
        }
    }

    // Clean up
    if (lpEnumFmtEtc)
        OleStdVerifyRelease(
            (LPUNKNOWN)lpEnumFmtEtc
        );
}


/* OleStdGetPriorityClipboardFormat
** --------------------------------
**
**    Retrieve the first clipboard format in a list for which data
**    exists in the source IDataObject*.
**
**    Returns -1 if no acceptable match is found.
**                        index of first acceptable match in the priority list.
**
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(int) OleStdGetPriorityClipboardFormat(LPDATAOBJECT lpSrcDataObj, LPOLEUIPASTEENTRY lpPriorityList, int cEntries)
{
    int i;
    int nFmtEtc = -1;

    // Mark all entries that the Source provides
    OleStdMarkPasteEntryList(lpSrcDataObj, lpPriorityList, cEntries);

    // Loop over the target's priority list of formats
    for (i = 0; i < cEntries; i++)
    {
        if (lpPriorityList[i].dwFlags != OLEUIPASTE_PASTEONLY &&
                !(lpPriorityList[i].dwFlags & OLEUIPASTE_PASTE))
            continue;

        // get first marked entry
        if (lpPriorityList[i].dwScratchSpace) {
            nFmtEtc = i;
            break;          // Found priority format; DONE
        }
    }

    return nFmtEtc;
}


/* OleStdIsDuplicateFormat
** -----------------------
**    Returns true if the lpFmtEtc->cfFormat is found in the array of
**    FormatEtc structures.
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(unsigned long) OleStdIsDuplicateFormat(
        LPFORMATETC         lpFmtEtc,
        LPFORMATETC         arrFmtEtc,
        int                 nFmtEtc
)
{
    int i;

    for (i = 0; i < nFmtEtc; i++) {
        if (lpFmtEtc->cfFormat == arrFmtEtc[i].cfFormat)
            return true;
    }

    return false;
}


/* OleStdGetItemToken
 * ------------------
 *
 * char* lpszSrc - Pointer to a source string
 * char* lpszDst - Pointer to destination buffer
 *
 * Will copy one token from the lpszSrc buffer to the lpszItem buffer.
 * It considers all alpha-numeric and white space characters as valid
 * characters for a token. the first non-valid character delimates the
 * token.
 *
 * returns the number of charaters eaten.
 */
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(unsigned long) OleStdGetItemToken(char* lpszSrc, char* lpszDst, int nMaxChars)
{
    unsigned long chEaten = 0L;

    // skip leading delimeter characters
    while (*lpszSrc && --nMaxChars > 0
                               && ((*lpszSrc=='/') || (*lpszSrc=='\\') ||
								   (*lpszSrc=='!') || (*lpszSrc==':'))) {
        lpszSrc++;
        chEaten++;
	}

    // Extract token string (up to first delimeter char or EOS)
    while (*lpszSrc && --nMaxChars > 0
                               && !((*lpszSrc=='/') || (*lpszSrc=='\\') ||
								   (*lpszSrc=='!') || (*lpszSrc==':'))) {
        *lpszDst++ = *lpszSrc++;
        chEaten++;
    }
    *lpszDst = 0;
    return chEaten;
}

#ifdef LATER


/*************************************************************************
** OleStdCreateRootStorage
**    create a root level Storage given a filename that is compatible
**    to be used by a top-level OLE container. if the filename
**    specifies an existing file, then an error is returned.
**    the root storage (Docfile) that is created by this function
**    is suitable to be used to create child storages for embedings.
**    (CreateChildStorage can be used to create child storages.)
**    NOTE: the root-level storage is opened in transacted mode.
*************************************************************************/

#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(LPSTORAGE) OleStdCreateRootStorage(const FSSpec* pspec, OSType creator, unsigned long grfMode)
{
    HRESULT hr;
    unsigned long grfCreateMode = STGM_READWRITE | STGM_TRANSACTED;
    unsigned long reserved = 0;
    LPSTORAGE lpRootStg;
    char szMsg[64];

    // if temp file is being created, enable delete-on-release
    if (! lpszStgName)
        grfCreateMode |= STGM_DELETEONRELEASE;

    hr = StgCreateDocfile(
            lpszStgName,
            grfMode | grfCreateMode,
            reserved,
            (LPSTORAGE *)&lpRootStg
        );

    if (hr == NOERROR)
        return lpRootStg;               // existing file successfully opened

    OleDbgOutHResult("StgCreateDocfile returned", hr);

    if (0 == LoadString(ghInst, IDS_OLESTDNOCREATEFILE, (char*)szMsg, 64))
      return NULL;

    MessageBox(NULL, (char*)szMsg, NULL,MB_ICONEXCLAMATION | MB_OK);
    return NULL;
}


/*************************************************************************
** OleStdOpenRootStorage
**    open a root level Storage given a filename that is compatible
**    to be used by a top-level OLE container. if the file does not
**    exist then an error is returned.
**    the root storage (Docfile) that is opened by this function
**    is suitable to be used to create child storages for embedings.
**    (CreateChildStorage can be used to create child storages.)
**    NOTE: the root-level storage is opened in transacted mode.
*************************************************************************/

#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(LPSTORAGE) OleStdOpenRootStorage(char* lpszStgName, unsigned long grfMode)
{
    HRESULT hr;
    unsigned long reserved = 0;
    LPSTORAGE lpRootStg;
    char  szMsg[64];

    if (lpszStgName) {

        hr = StgOpenStorage(
                lpszStgName,
                NULL,
                grfMode | STGM_TRANSACTED,
                NULL,
                reserved,
                (LPSTORAGE *)&lpRootStg
            );

        if (hr == NOERROR)
            return lpRootStg;     // existing file successfully opened

        OleDbgOutHResult("StgOpenStorage returned", hr);
    }

    if (0 == LoadString(ghInst, IDS_OLESTDNOOPENFILE, szMsg, 64))
      return NULL;

    MessageBox(NULL, (char*)szMsg, NULL,MB_ICONEXCLAMATION | MB_OK);
    return NULL;
}


/*************************************************************************
** OpenOrCreateRootStorage
**    open a root level Storage given a filename that is compatible
**    to be used by a top-level OLE container. if the filename
**    specifies an existing file, then it is open, otherwise a new file
**    with the given name is created.
**    the root storage (Docfile) that is created by this function
**    is suitable to be used to create child storages for embedings.
**    (CreateChildStorage can be used to create child storages.)
**    NOTE: the root-level storage is opened in transacted mode.
*************************************************************************/

#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(LPSTORAGE) OleStdOpenOrCreateRootStorage(char* lpszStgName, unsigned long grfMode)
{
    HRESULT hrErr;
    SCODE sc;
    unsigned long reserved = 0;
    LPSTORAGE lpRootStg;
    char      szMsg[64];

    if (lpszStgName) {

        hrErr = StgOpenStorage(
                lpszStgName,
                NULL,
                grfMode | STGM_READWRITE | STGM_TRANSACTED,
                NULL,
                reserved,
                (LPSTORAGE *)&lpRootStg
        );

        if (hrErr == NOERROR)
            return lpRootStg;      // existing file successfully opened

        OleDbgOutHResult("StgOpenStorage returned", hrErr);
        sc = GetScode(hrErr);

        if (sc!=STG_E_FILENOTFOUND && sc!=STG_E_FILEALREADYEXISTS) {
            return NULL;
        }
    }

    /* if file did not already exist, try to create a new one */
    hrErr = StgCreateDocfile(
            lpszStgName,
            grfMode | STGM_READWRITE | STGM_TRANSACTED,
            reserved,
            (LPSTORAGE *)&lpRootStg
    );

    if (hrErr == NOERROR)
        return lpRootStg;               // existing file successfully opened

    OleDbgOutHResult("StgCreateDocfile returned", hrErr);

    if (0 == LoadString(ghInst, IDS_OLESTDNOCREATEFILE, (char*)szMsg, 64))
      return NULL;

    MessageBox(NULL, (char*)szMsg, NULL, MB_ICONEXCLAMATION | MB_OK);
    return NULL;
}


/*
** OleStdCreateChildStorage
**    create a child Storage inside the given lpStg that is compatible
**    to be used by an embedded OLE object. the return value from this
**    function can be passed to OleCreateXXX functions.
**    NOTE: the child storage is opened in transacted mode.
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(LPSTORAGE) OleStdCreateChildStorage(LPSTORAGE lpStg, char* lpszStgName)
{
    if (lpStg != NULL) {
        LPSTORAGE lpChildStg;
        unsigned long grfMode = (STGM_READWRITE | STGM_TRANSACTED |
                STGM_SHARE_EXCLUSIVE);
        unsigned long reserved = 0;

        HRESULT hrErr = lpStg->lpVtbl->CreateStorage(
                lpStg,
                lpszStgName,
                grfMode,
                reserved,
                reserved,
                (LPSTORAGE *)&lpChildStg
            );

        if (hrErr == NOERROR)
            return lpChildStg;

        OleDbgOutHResult("lpStg->lpVtbl->CreateStorage returned", hrErr);
    }
    return NULL;
}


/*
** OleStdOpenChildStorage
**    open a child Storage inside the given lpStg that is compatible
**    to be used by an embedded OLE object. the return value from this
**    function can be passed to OleLoad function.
**    NOTE: the child storage is opened in transacted mode.
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(LPSTORAGE) OleStdOpenChildStorage(LPSTORAGE lpStg, char* lpszStgName, unsigned long grfMode)
{
    LPSTORAGE lpChildStg;
    LPSTORAGE lpstgPriority = NULL;
    unsigned long reserved = 0;
    HRESULT hrErr;

    if (lpStg  != NULL) {

        hrErr = lpStg->lpVtbl->OpenStorage(
                lpStg,
                lpszStgName,
                lpstgPriority,
                grfMode | STGM_TRANSACTED | STGM_SHARE_EXCLUSIVE,
                NULL,
                reserved,
                (LPSTORAGE *)&lpChildStg
            );

        if (hrErr == NOERROR)
            return lpChildStg;

        OleDbgOutHResult("lpStg->lpVtbl->OpenStorage returned", hrErr);
    }
    return NULL;
}

#endif // LATER

/* OleStdCommitStorage
** -------------------
**    Commit the changes to the given IStorage*. This routine can be
**    called on either a root-level storage as used by an OLE-Container
**    or by a child storage as used by an embedded object.
**
**    This routine first attempts to perform this commit in a safe
**    manner. if this fails it then attempts to do the commit in a less
**    robust manner (STGC_OVERWRITE).
*/
#ifndef _MSC_VER
#pragma segment OleStdSeg
#else
#pragma code_seg("OleStdSeg", "SWAPPABLE")
#endif
STDAPI_(unsigned long) OleStdCommitStorage(LPSTORAGE lpStg)
{
    HRESULT hrErr;

    // make the changes permanent
    hrErr = lpStg->lpVtbl->Commit(lpStg, STGC_DEFAULT);

    if (GetScode(hrErr) == STG_E_MEDIUMFULL) {
        // try to commit changes in less robust manner.
        OleDbgOut("Warning: commiting with STGC_OVERWRITE specified\n");
        hrErr = lpStg->lpVtbl->Commit(lpStg, STGC_OVERWRITE);
    }

    if (hrErr != NOERROR)
    {
#if LATER
        char szMsg[64];

        if (0 == LoadString(ghInst, IDS_OLESTDDISKFULL, (char*)szMsg, 64))
           return false;

        MessageBox(NULL, (char*)szMsg, NULL, MB_ICONEXCLAMATION | MB_OK);
#else
		Debugger();
#endif

        return false;
    }
    else {
        return true;
    }
}

STDAPI OleStdGetInplaceClipRgn(RgnHandle rgn)
{
//#define _wanttosavecliprgn
#ifdef _wanttosavecliprgn	
	RgnHandle	saveClipRgn;
#endif
	GrafPtr		savePort;
	GrafPtr		wmgrPort;
	WindowPtr	window;
	Rect		bigRect = {-32700, -32700, 32700, 32700};
#ifdef UIDLL
   void*    oldQD = SetLpqdFromA5();
#endif
	
	window = FrontWindow();
	if (!window)
   {
#ifdef UIDLL
      RestoreLpqd(oldQD);
#endif
      return ResultFromScode(E_UNEXPECTED);
   }
		

#ifdef _wanttosavecliprgn	
	saveClipRgn = NewRgn();				/* get an empty region */
	if (!saveClipRgn)
   {
#ifdef UIDLL
      RestoreLpqd(oldQD);
#endif
      return ResultFromScode(E_OUTOFMEMORY);
   }
		
#endif

	GetPort(&savePort);				/* save current port */
	GetWMgrPort(&wmgrPort);

	SetPort(window);
#ifdef _wanttosavecliprgn		
	GetClip( saveClipRgn );			/* save current cliprgn */
#endif
	ClipRect(&bigRect );			/* widen the front window's cliprgn */
	
	SetPort(wmgrPort);				
	ClipRect(&bigRect );			/* widen the wmgr port's cliprgn */
	ClipAbove((WindowRef) window);	/* clip to server task */
	GetClip(rgn);
	DiffRgn(GetGrayRgn(), rgn, rgn);/* invert the rgn */
	
#ifdef _wanttosavecliprgn	
	SetPort(wmgrPort);				
	SetClip( saveClipRgn );			/* restore previous value */
	DisposeRgn( saveClipRgn );		/* not needed any more */
#endif

	SetPort(savePort);
#ifdef UIDLL
      RestoreLpqd(oldQD);
#endif
	return NOERROR;
}

STDAPI OleStdBlastWindows(RgnHandle rgn)
{

#ifndef __powerc
	WindowPeek window;
	
	window = (WindowPeek) FrontWindow();
	
	while (window) {
		DiffRgn(window->contRgn,  rgn, window->contRgn);
		DiffRgn(window->strucRgn, rgn, window->strucRgn);
		window = window->nextWindow;
	}
#endif
	return NOERROR;
}

STDAPI OleStdBlastWindow(WindowPtr window, RgnHandle rgn)
{
#ifndef __powerc
	DiffRgn(((WindowPeek)window)->contRgn,  rgn, ((WindowPeek)window)->contRgn);
	DiffRgn(((WindowPeek)window)->strucRgn, rgn, ((WindowPeek)window)->strucRgn);
#endif
	return NOERROR;
}


#ifndef _MSC_VER
typedef pascal long (*WDEFPtr)(short, WindowPeek, short, long);
#else
typedef long (__pascal *WDEFPtr)(short, WindowPeek, short, long);
#endif


STDAPI OleStdUnblastWindows(RgnHandle rgn)
{
#ifndef __powerc
	WindowPeek window;
	WindowPtr	frontwindow;
	WDEFPtr	wdef;
	Handle	hWDEF;
	short	varCode;
	char	lockState;
	GrafPtr savePort;
	Point	pt = {0,0};
   void*   oldQD = NULL;
	
	window = (WindowPeek) FrontWindow();
	frontwindow = (WindowPtr) window;
	
	if (!window)
		return NOERROR;

#ifdef UIDLL
	oldQD = SetLpqdFromA5();
#endif

	while (window) {
		hWDEF = window->windowDefProc;
		varCode = GetWVariant((WindowPtr) window);
		lockState = HGetState(hWDEF);
		if (!(0x80 & lockState))
			HLock(hWDEF);
		wdef = (WDEFPtr) *(hWDEF);

#ifdef __powerc
		CallWindowDefProc(wdef, varCode, window, wCalcRgns, 0L);
#else
		(*wdef) (varCode, window, wCalcRgns, 0L);		// calc rgns
#endif

		if (!(0x80 & lockState))
			HSetState(hWDEF, lockState);
		window = window->nextWindow;
	}

	// need the following to uncache layer mgr data
	GetPort(&savePort);
	SetPort(frontwindow);
	LocalToGlobal(&pt);
	// this move window causes layer mgr to update info
	MoveWindow(frontwindow, pt.h, pt.v, false);	// move into same location
   if (rgn) {
		PaintBehind((WindowRef) frontwindow, rgn);
		CalcVisBehind((WindowRef) frontwindow, rgn);
	}
	SetPort(savePort);

#ifdef UIDLL
   RestoreLpqd(oldQD);
#endif
#endif
	return NOERROR;
}

STDAPI OleStdTranslateAccelerator(
		LPOLEINPLACEFRAME lpFrame,
		LPOLEINPLACEFRAMEINFO lpFrameInfo,
		EventRecord * lpEvent)
{
#ifndef _MSC_VER
#ifndef _DEBUG
#pragma unused(lpFrameInfo);
#endif
#endif

	long 	id;
	HRESULT	hresult;
	char	key;
	
	OleDbgAssert(lpFrame != NULL);
	OleDbgAssert(lpFrameInfo == NULL);	// this is not used today!
	OleDbgAssert(lpEvent != NULL);
	
	if ((lpEvent->what != keyDown) && (lpEvent->what != autoKey))
		return ResultFromScode(E_UNEXPECTED);	// not a cmd key

	key = (char) ( lpEvent->message & charCodeMask );

	id = MenuKey(key);
	hresult = lpFrame->lpVtbl->TranslateAccelerator(lpFrame, lpEvent, id);

	HiliteMenu(0);
	return hresult;
}
