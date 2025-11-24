/*************************************************************************
** 
**    OLE 2 Sample Code
**    
**    linking.c
**    
**    This file contains the major interfaces, methods and related support
**    functions for implementing linking to items. The code
**    contained in this file is used by BOTH the Container and Server
**    (Object) versions of the Outline sample code. 
**    
**    As a server SVROUTL supports linking to the whole document object
**    (either a file-based document or as an embedded object). It also
**    supports linking to ranges (or PseudoObjects). 
**    
**    As a container CNTROUTL supports linking to embedded objects.
**    (see file svrpsobj.c for Pseudo Object implementation)
**    
**    OleDoc Object
**      exposed interfaces:
**          IPersistFile
**          IOleItemContainer
**    
**    (c) Copyright Microsoft Corp. 1992 - 1993 All Rights Reserved
**
*************************************************************************/

#include "outline.h"

OLEDBGDATA

extern LPOUTLINEAPP             g_lpApp;



/*************************************************************************
** OleDoc::IPersistFile interface implementation
*************************************************************************/

STDMETHODIMP OleDoc_PFile_QueryInterface(
        LPPERSISTFILE       lpThis,
        REFIID              riid,
        LPVOID FAR*         lplpvObj
)
{
    LPOLEDOC lpOleDoc = ((struct CDocPersistFileImpl FAR*)lpThis)->lpOleDoc;

    return OleDoc_QueryInterface(lpOleDoc, riid, lplpvObj);
}


STDMETHODIMP_(ULONG) OleDoc_PFile_AddRef(LPPERSISTFILE lpThis)
{
    LPOLEDOC lpOleDoc = ((struct CDocPersistFileImpl FAR*)lpThis)->lpOleDoc;

    OleDbgAddRefMethod(lpThis, "IPersistFile");

    return OleDoc_AddRef(lpOleDoc);
}


STDMETHODIMP_(ULONG) OleDoc_PFile_Release (LPPERSISTFILE lpThis)
{
    LPOLEDOC lpOleDoc = ((struct CDocPersistFileImpl FAR*)lpThis)->lpOleDoc;

    OleDbgReleaseMethod(lpThis, "IPersistFile");

    return OleDoc_Release(lpOleDoc);
}


STDMETHODIMP OleDoc_PFile_GetClassID (
        LPPERSISTFILE       lpThis,
        CLSID FAR*          lpclsid
)
{
    LPOLEDOC lpOleDoc = ((struct CDocPersistFileImpl FAR*)lpThis)->lpOleDoc;
    LPOLEAPP lpOleApp = (LPOLEAPP)g_lpApp;

    OleDbgOut2("OleDoc_PFile_GetClassID\r\n");

#if defined( OLE_SERVER ) && defined( SVR_TREATAS )

    /* OLE2NOTE: we must be carefull to return the correct CLSID here.
    **    if we are currently preforming a "TreatAs (aka. ActivateAs)"
    **    operation then we need to return the class of the object
    **    written in the storage of the object. otherwise we would
    **    return our own class id. 
    */
    return ServerDoc_GetClassID((LPSERVERDOC)lpOleDoc, lpclsid);
#else
    *lpclsid = CLSID_APP;
#endif 
    return NOERROR;
}


STDMETHODIMP  OleDoc_PFile_IsDirty(LPPERSISTFILE lpThis)
{
    LPOLEDOC lpOleDoc = ((struct CDocPersistFileImpl FAR*)lpThis)->lpOleDoc;

    OleDbgOut2("OleDoc_PFile_IsDirty\r\n");

    if (OutlineDoc_IsModified((LPOUTLINEDOC)lpOleDoc))
        return NOERROR;
    else
        return ResultFromScode(S_FALSE);
}


STDMETHODIMP OleDoc_PFile_Load (
        LPPERSISTFILE       lpThis,
        LPCSTR              lpszFileName,
        DWORD               grfMode
)
{
    LPOLEDOC lpOleDoc = ((struct CDocPersistFileImpl FAR*)lpThis)->lpOleDoc;
    SCODE sc;

    OLEDBG_BEGIN2("OleDoc_PFile_Load\r\n")

    /* OLE2NOTE: grfMode passed from the caller indicates if the caller
    **    needs Read or ReadWrite permissions. if appropriate the
    **    callee should open the file with the requested permissions.
    **    the caller will normally not impose sharing permissions.
    **    
    **    the sample code currently always opens its file ReadWrite.
    */

    if (OutlineDoc_LoadFromFile((LPOUTLINEDOC)lpOleDoc, (LPSTR)lpszFileName))
        sc = S_OK;
    else
        sc = S_FALSE;

    OLEDBG_END2
    return ResultFromScode(sc);
}


STDMETHODIMP OleDoc_PFile_Save (
        LPPERSISTFILE       lpThis,
        LPCSTR              lpszFileName,
        BOOL                fRemember
)
{
    LPOLEDOC lpOleDoc = ((struct CDocPersistFileImpl FAR*)lpThis)->lpOleDoc;
    LPOUTLINEDOC lpOutlineDoc = (LPOUTLINEDOC)lpOleDoc;
    SCODE sc;

    OLEDBG_BEGIN2("OleDoc_PFile_Save\r\n")

    /* OLE2NOTE: it is only legal to perform a Save or SaveAs operation
    **    on a file-based document. if the document is an embedded
    **    object then we can not be changed to a file-base object.
    **    
    **      fRemember   lpszFileName     Type of Save    
    **    ----------------------------------------------
    **        N/A         NULL           SAVE            
    **        TRUE        ! NULL         SAVE AS         
    **        FALSE       ! NULL         SAVE COPY AS    
    */
    if ( (lpszFileName==NULL || (lpszFileName != NULL && fRemember))
            && ((lpOutlineDoc->m_docInitType != DOCTYPE_FROMFILE 
                && lpOutlineDoc->m_docInitType != DOCTYPE_NEW)) ) {
        OLEDBG_END2
        return ResultFromScode(E_INVALIDARG);
    }

    if (OutlineDoc_SaveToFile(
            (LPOUTLINEDOC)lpOleDoc,
            lpszFileName,
            lpOutlineDoc->m_cfSaveFormat,
            fRemember)) {
        sc = S_OK;
    } else
        sc = E_FAIL;

    OLEDBG_END2
    return ResultFromScode(sc);
}


STDMETHODIMP OleDoc_PFile_SaveCompleted (
        LPPERSISTFILE       lpThis,
        LPCSTR              lpszFileName
)
{
    LPOLEDOC lpOleDoc = ((struct CDocPersistFileImpl FAR*)lpThis)->lpOleDoc;
    LPOUTLINEDOC lpOutlineDoc = (LPOUTLINEDOC)lpOleDoc;

    OLEDBG_BEGIN2("OleDoc_PFile_SaveCompleted\r\n")

    /* OLE2NOTE: this method should be called in the Save or SaveAs 
    **    situtations. it is illegal to call this method unless we are a 
    **    file-based object (we should also NOT be of type DOCTYPE_NEW
    **    either because IPersistFile::Save should have been called by
    **    now.
    */
    if (lpOutlineDoc->m_docInitType != DOCTYPE_FROMFILE) {
        OLEDBG_END2
        return ResultFromScode(E_INVALIDARG);
    }

    // Clear dirty flag upon save or saveAs
    OutlineDoc_SetModified(lpOutlineDoc, FALSE, FALSE, FALSE);

#if defined( OLE_SERVER )

    /* OLE2NOTE: this method should be called in the Save or SaveAs 
    **    situtations. it informs us that the caller is done with the data
    **    in the file. Until SaveCompleted call is finished, we may not 
    **    change the contents of the file. since we do not normally
    **    scribble in our file, we do not have to care about this.
    */
    ServerDoc_SendAdvise (
            (LPSERVERDOC)lpOleDoc, 
            OLE_ONSAVE, 
            NULL,	/* lpmkDoc -- not relevant here */
            0    	/* advf -- not relevant here */
    );

#endif  // OLE_SERVER

    OLEDBG_END2
    return NOERROR;
}


STDMETHODIMP OleDoc_PFile_GetCurFile (
        LPPERSISTFILE   lpThis,
        LPSTR FAR*      lplpszFileName
)
{
    LPOLEDOC lpOleDoc = ((struct CDocPersistFileImpl FAR*)lpThis)->lpOleDoc;
    LPOUTLINEDOC lpOutlineDoc = (LPOUTLINEDOC)lpOleDoc;
    LPMALLOC lpMalloc;
    LPSTR lpsz;
    SCODE sc;

    OleDbgOut2("OleDoc_PFile_GetCurFile\r\n");

    /* OLE2NOTE: we must make sure to set all out ptr parameters to NULL. */
    *lplpszFileName = NULL;

    /*********************************************************************
    ** OLE2NOTE: memory returned for the lplpszFileName must be
    **    allocated appropriately using the current registered IMalloc
    **    interface. the allows the ownership of the memory to be
    **    passed to the caller (even if in another process).
    *********************************************************************/

    CoGetMalloc(MEMCTX_TASK, &lpMalloc);
    if (! lpMalloc) {
        return ResultFromScode(E_FAIL);
    }

    if (lpOutlineDoc->m_docInitType == DOCTYPE_FROMFILE) {
        /* valid filename associated; return file name */
        lpsz = (LPSTR)lpMalloc->lpVtbl->Alloc(
                lpMalloc,
                lstrlen((LPSTR)lpOutlineDoc->m_szFileName)+1
        );
        lstrcpy(lpsz, (LPSTR)lpOutlineDoc->m_szFileName);
        sc = S_OK;
    } else {
        /* no file associated; return default file name prompt */
        lpsz=(LPSTR)lpMalloc->lpVtbl->Alloc(lpMalloc, sizeof(DEFEXTENSION)+3);
        wsprintf(lpsz, "*.%s", DEFEXTENSION);
        sc = S_FALSE;
    }

    OleStdRelease((LPUNKNOWN)lpMalloc);
    *lplpszFileName = lpsz;
    return ResultFromScode(sc);
}


/*************************************************************************
** OleDoc::IOleItemContainer interface implementation
*************************************************************************/

STDMETHODIMP OleDoc_ItemCont_QueryInterface(
        LPOLEITEMCONTAINER  lpThis,
        REFIID              riid,
        LPVOID FAR*         lplpvObj
)
{
    LPOLEDOC lpOleDoc =
            ((struct CDocOleItemContainerImpl FAR*)lpThis)->lpOleDoc;

    return OleDoc_QueryInterface(lpOleDoc, riid, lplpvObj);
}


STDMETHODIMP_(ULONG) OleDoc_ItemCont_AddRef(LPOLEITEMCONTAINER lpThis)
{
    LPOLEDOC lpOleDoc =
            ((struct CDocOleItemContainerImpl FAR*)lpThis)->lpOleDoc;

    OleDbgAddRefMethod(lpThis, "IOleItemContainer");

    return OleDoc_AddRef((LPOLEDOC)lpOleDoc);
}


STDMETHODIMP_(ULONG) OleDoc_ItemCont_Release(LPOLEITEMCONTAINER lpThis)
{
    LPOLEDOC lpOleDoc =
            ((struct CDocOleItemContainerImpl FAR*)lpThis)->lpOleDoc;

    OleDbgReleaseMethod(lpThis, "IOleItemContainer");

    return OleDoc_Release((LPOLEDOC)lpOleDoc);
}


STDMETHODIMP OleDoc_ItemCont_ParseDisplayName(
        LPOLEITEMCONTAINER  lpThis,
        LPBC                lpbc,
        LPSTR               lpszDisplayName,
        ULONG FAR*          lpchEaten,
        LPMONIKER FAR*      lplpmkOut
)
{
    LPOLEDOC lpOleDoc =
            ((struct CDocOleItemContainerImpl FAR*)lpThis)->lpOleDoc;
    char szItemName[MAXNAMESIZE];
    LPUNKNOWN lpUnk;
    HRESULT hrErr;

    OleDbgOut2("OleDoc_ItemCont_ParseDisplayName\r\n");

    /* OLE2NOTE: we must make sure to set all out ptr parameters to NULL. */
    *lplpmkOut = NULL;

    *lpchEaten = OleStdGetItemToken(
            lpszDisplayName,
            szItemName,
            sizeof(szItemName)
    );

    /* OLE2NOTE: get a pointer to a running instance of the object. we
    **    should force the object to go running if necessary (even if
    **    this means launching its server EXE). this is the meaining of
    **    BINDSPEED_INDEFINITE. Parsing a Moniker is known to be an
    **    "EXPENSIVE" operation.
    */
    hrErr = OleDoc_ItemCont_GetObject(
            lpThis, 
            szItemName, 
            BINDSPEED_INDEFINITE,
            lpbc, 
            &IID_IUnknown,
            (LPVOID FAR*)&lpUnk
    );

    if (hrErr == NOERROR) {
        OleStdRelease(lpUnk);   // item name FOUND; don't need obj ptr.
        CreateItemMoniker(OLESTDDELIM, szItemName, lplpmkOut);
    } else
        *lpchEaten = 0;     // item name is NOT valid

    return hrErr;
}


STDMETHODIMP OleDoc_ItemCont_EnumObjects(
        LPOLEITEMCONTAINER  lpThis,
        DWORD               grfFlags,
        LPENUMUNKNOWN FAR*  lplpenumUnknown
)
{
    LPOLEDOC lpOleDoc =
            ((struct CDocOleItemContainerImpl FAR*)lpThis)->lpOleDoc;

    OLEDBG_BEGIN2("OleDoc_ItemCont_EnumObjects\r\n")

    /* OLE2NOTE: we must make sure to set all out ptr parameters to NULL. */
    *lplpenumUnknown = NULL;

    /* OLE2NOTE: this method should be implemented to allow programatic
    **    clients the ability to what elements the container holds.
    **    this method is NOT called in the standard linking scenarios.
    **    
    **    grfFlags can be one of the following:
    **        OLECONTF_EMBEDDINGS   -- enumerate embedded objects
    **        OLECONTF_LINKS        -- enumerate linked objects
    **        OLECONTF_OTHERS       -- enumerate non-OLE compound doc objs
    **        OLECONTF_ONLYUSER     -- enumerate only objs named by user
    **        OLECONTF_ONLYIFRUNNING-- enumerate only objs in running state
    */

    OleDbgAssertSz(0, "NOT YET IMPLEMENTED!");

    OLEDBG_END2
    return ResultFromScode(E_NOTIMPL);
}


STDMETHODIMP OleDoc_ItemCont_LockContainer(
        LPOLEITEMCONTAINER  lpThis,
        BOOL                fLock
)
{
    LPOLEDOC lpOleDoc =
            ((struct CDocOleItemContainerImpl FAR*)lpThis)->lpOleDoc;
    HRESULT hrErr;

    OLEDBG_BEGIN2("OleDoc_ItemCont_LockContainer\r\n")

#if defined( _DEBUG )
    if (fLock) {
        ++lpOleDoc->m_cLock;

        OleDbgOutRefCnt3(
                "OleDoc_ItemCont_LockContainer: cLock++\r\n", 
                lpOleDoc, lpOleDoc->m_cLock);
    } else {

        /* OLE2NOTE: when there are no open documents and the app is not
        **    under the control of the user and there are no outstanding
        **    locks on the app, then revoke our ClassFactory to enable the
        **    app to shut down.
        */
        OleDbgAssertSz (lpOleDoc->m_cLock > 0,
                "OleDoc_ItemCont_LockContainer(FALSE) called with cLock == 0"
        );

        --lpOleDoc->m_cLock;

        if (lpOleDoc->m_cLock == 0) {
            OleDbgOutRefCnt2(
                    "OleDoc_ItemCont_LockContainer: UNLOCKED\r\n", 
                    lpOleDoc, lpOleDoc->m_cLock);
        } else {
            OleDbgOutRefCnt3(
                    "OleDoc_ItemCont_LockContainer: cLock--\r\n",
                    lpOleDoc, lpOleDoc->m_cLock);
        }
    }
#endif  // _DEBUG
        
    /* OLE2NOTE: in order to hold the document alive we call
    **    CoLockObjectExternal to add a strong reference to our Doc
    **    object. this will keep the Doc alive when all other external
    **    references release us. whenever an embedded object goes
    **    running a LockContainer(TRUE) is called. when the embedded
    **    object shuts down (ie. transitions from running to loaded)
    **    LockContainer(FALSE) is called. if the user issues File.Close
    **    the document will shut down in any case ignoring any
    **    outstanding LockContainer locks because CoDisconnectObject is
    **    called in OleDoc_Close. this will forceably break any
    **    existing strong reference counts including counts that we add
    **    ourselves by calling CoLockObjectExternal and guarantee that
    **    the Doc object gets its final release (ie. cRefs goes to 0).
    */
    hrErr = OleDoc_Lock(lpOleDoc, fLock, TRUE /* fLastUnlockReleases */);

    OLEDBG_END2
    return hrErr;
}


STDMETHODIMP OleDoc_ItemCont_GetObject(
        LPOLEITEMCONTAINER  lpThis,
        LPSTR               lpszItem,
        DWORD               dwSpeedNeeded,
        LPBINDCTX           lpbc,
        REFIID              riid,
        LPVOID FAR*         lplpvObject
)
{
    LPOLEDOC lpOleDoc =
            ((struct CDocOleItemContainerImpl FAR*)lpThis)->lpOleDoc;
    HRESULT hrErr;

    OLEDBG_BEGIN2("OleDoc_ItemCont_GetObject\r\n")

    /* OLE2NOTE: we must make sure to set all out ptr parameters to NULL. */
    *lplpvObject = NULL;

#if defined( OLE_SERVER )

    /* OLE2NOTE: SERVER ONLY version should return PseudoObjects with
    **    BINDSPEED_IMMEDIATE, thus the dwSpeedNeeded is not important
    **    in the case of a pure server.
    */
    hrErr = ServerDoc_GetObject(
            (LPSERVERDOC)lpOleDoc, lpszItem,riid,lplpvObject);

#elif defined( OLE_CNTR )

    /* OLE2NOTE: dwSpeedNeeded indicates how long the caller is willing
    **    to wait for us to get the object:
    **      BINDSPEED_IMMEDIATE -- only if obj already loaded && IsRunning
    **      BINDSPEED_MODERATE  -- load obj if necessary && if IsRunning
    **      BINDSPEED_INDEFINITE-- force obj to load and run if necessary
    */
    hrErr = ContainerDoc_GetObject(
            (LPCONTAINERDOC)lpOleDoc,lpszItem,dwSpeedNeeded,riid,lplpvObject);
#endif

    OLEDBG_END2
    return hrErr;
}


STDMETHODIMP OleDoc_ItemCont_GetObjectStorage(
        LPOLEITEMCONTAINER  lpThis,
        LPSTR               lpszItem,
        LPBINDCTX           lpbc,
        REFIID              riid,
        LPVOID FAR*         lplpvStorage
)
{
    LPOLEDOC lpOleDoc =
            ((struct CDocOleItemContainerImpl FAR*)lpThis)->lpOleDoc;

    OleDbgOut2("OleDoc_ItemCont_GetObjectStorage\r\n");

    /* OLE2NOTE: we must make sure to set all out ptr parameters to NULL. */
    *lplpvStorage = NULL;

#if defined( OLE_SERVER )

    /* OLE2NOTE: in the SERVER ONLY version, item names identify pseudo
    **    objects. pseudo objects, do NOT have identifiable storage.
    */
    return ResultFromScode(E_FAIL);


#elif defined( OLE_CNTR )

    // We can only return an IStorage* type pointer
    if (! IsEqualIID(riid, &IID_IStorage))
        return ResultFromScode(E_FAIL);

    return ContainerDoc_GetObjectStorage(
            (LPCONTAINERDOC)lpOleDoc,
            lpszItem,
            (LPSTORAGE FAR*)lplpvStorage
    );
#endif
}


STDMETHODIMP OleDoc_ItemCont_IsRunning(
        LPOLEITEMCONTAINER  lpThis,
        LPSTR               lpszItem
)
{
    LPOLEDOC lpOleDoc =
            ((struct CDocOleItemContainerImpl FAR*)lpThis)->lpOleDoc;
    HRESULT hrErr;

    OLEDBG_BEGIN2("OleDoc_ItemCont_IsRunning\r\n")

    /* OLE2NOTE: Check if item name is valid. if so then return if
    **    Object is running. PseudoObjects in the Server version are
    **    always considered running. Ole objects in the container must
    **    be checked if they are running. 
    */

#if defined( OLE_SERVER )

    hrErr = ServerDoc_IsRunning((LPSERVERDOC)lpOleDoc, lpszItem);

#elif defined( OLE_CNTR )

    hrErr = ContainerDoc_IsRunning((LPCONTAINERDOC)lpOleDoc, lpszItem);

#endif

    OLEDBG_END2
    return hrErr;
}


/*************************************************************************
** OleDoc Common Support Functions
*************************************************************************/


/* OleDoc_GetFullMoniker
** ---------------------
**    Return the full, absolute moniker of the document.
**
**    NOTE: the caller must release the pointer returned when done.
*/
LPMONIKER OleDoc_GetFullMoniker(LPOLEDOC lpOleDoc, DWORD dwAssign)
{
    LPMONIKER lpMoniker = NULL;

    OLEDBG_BEGIN3("OleDoc_GetFullMoniker\r\n")

    if (lpOleDoc->m_lpSrcDocOfCopy) {
        /* CASE I: this document was created for a copy or drag/drop
        **    operation. generate the moniker which identifies the
        **    source document of the original copy.
        */
        if (! lpOleDoc->m_fLinkSourceAvail)
            goto done;        // we already know a moniker is not available

        lpMoniker = OleDoc_GetFullMoniker(
                lpOleDoc->m_lpSrcDocOfCopy,
                dwAssign
        );
    }
    else if (lpOleDoc->m_lpFileMoniker) {

        /* CASE II: this document is a top-level user document (either
        **    file-based or untitled). return the FileMoniker stored
        **    with the document; it uniquely identifies the document.
        */

        // we must AddRef the moniker to pass out a ptr
        lpOleDoc->m_lpFileMoniker->lpVtbl->AddRef(lpOleDoc->m_lpFileMoniker);

        lpMoniker = lpOleDoc->m_lpFileMoniker;
    }

#if defined( OLE_SERVER )

    else if (((LPSERVERDOC)lpOleDoc)->m_lpOleClientSite) {

        /* CASE III: this document is an embedded object, ask our
        **    container for our moniker.
        */
        OLEDBG_BEGIN2("IOleClientSite::GetMoniker called\r\n");
        ((LPSERVERDOC)lpOleDoc)->m_lpOleClientSite->lpVtbl->GetMoniker(
                ((LPSERVERDOC)lpOleDoc)->m_lpOleClientSite,
                dwAssign,
                OLEWHICHMK_OBJFULL,
                &lpMoniker
        );
        OLEDBG_END2
    }

#endif

    else {
        lpMoniker = NULL;
    }

done:
    OLEDBG_END3
    return lpMoniker;
}


/* OleDoc_DocRenamedUpdate
** -----------------------
**    Update the documents registration in the running object table (ROT).
**    Also inform all embedded OLE objects (container only) and/or psedudo
**    objects (server only) that the name of the document has changed.
*/
void OleDoc_DocRenamedUpdate(LPOLEDOC lpOleDoc, LPMONIKER lpmkDoc)
{
    OLEDBG_BEGIN3("OleDoc_DocRenamedUpdate\r\n")

    OleDoc_AddRef(lpOleDoc);

    /* OLE2NOTE: we must re-register ourselves as running when we
    **    get a new moniker assigned (ie. when we are renamed).
    */
    OLEDBG_BEGIN3("OleStdRegisterAsRunning called\r\n")
    OleStdRegisterAsRunning(
            (LPUNKNOWN)&lpOleDoc->m_Unknown,
            lpmkDoc,
            &lpOleDoc->m_dwRegROT
    );
    OLEDBG_END3

#if defined( OLE_SERVER )
    {
        LPSERVERDOC lpServerDoc = (LPSERVERDOC)lpOleDoc;

        /* OLE2NOTE: inform any linking clients that the document has been
        **    renamed. in addition, any currently active pseudo objects
        **    should also inform their clients.
        */
        ServerDoc_SendAdvise (
                lpServerDoc,
                OLE_ONRENAME,
                lpmkDoc,
                0        /* advf -- not relevant here */
        );
    }

#elif defined( OLE_CNTR )
    {
        LPCONTAINERDOC lpContainerDoc = (LPCONTAINERDOC)lpOleDoc;

        /* OLE2NOTE: must tell all OLE objects that our container
        **    moniker changed.
        */
        ContainerDoc_InformAllOleObjectsDocRenamed(
                lpContainerDoc,
                lpmkDoc
        );
    }
#endif

    OleDoc_Release(lpOleDoc);       // release artificial AddRef above
    OLEDBG_END3
}



#if defined( OLE_SERVER )

/*************************************************************************
** ServerDoc Supprt Functions Used by Server versions
*************************************************************************/


/* ServerDoc_PseudoObjLockDoc
** --------------------------
**    Add a lock on the Doc on behalf of the PseudoObject. the Doc may not 
**    close while the Doc exists.
**
**    when a pseudo object is first created, it calls this method to
**    guarantee that the document stays alive (PseudoObj_Init).
**    when a pseudo object is destroyed, it call
**    ServerDoc_PseudoObjUnlockDoc to release this hold on the document.
*/
void ServerDoc_PseudoObjLockDoc(LPSERVERDOC lpServerDoc)
{
	LPOLEDOC lpOleDoc = (LPOLEDOC)lpServerDoc;
    ULONG cPseudoObj;

    cPseudoObj = ++lpServerDoc->m_cPseudoObj;

    OleDbgOutRefCnt3(
            "ServerDoc_PseudoObjLockDoc: cPseudoObj++\r\n", 
            lpServerDoc, 
            cPseudoObj
    );

    OleDoc_Lock(lpOleDoc, TRUE /* fLock */, 0 /* not applicable */);
    return;
}


/* ServerDoc_PseudoObjUnlockDoc
** ----------------------------
**    Release the lock on the Doc on behalf of the PseudoObject. if this was 
**    the last lock on the Doc, then it will shutdown.
*/
void ServerDoc_PseudoObjUnlockDoc(
		LPSERVERDOC			lpServerDoc, 
		LPPSEUDOOBJ			lpPseudoObj
)
{
    ULONG cPseudoObj;
    LPOLEDOC lpOleDoc = (LPOLEDOC)lpServerDoc;

    OLEDBG_BEGIN3("ServerDoc_PseudoObjUnlockDoc\r\n")

    /* OLE2NOTE: when there are no active pseudo objects in the Doc and
    **    the Doc is not visible, and if there are no outstanding locks 
    **    on the Doc, then this is a "silent update"
    **    situation. our Doc is being used programatically by some
    **    client; it is NOT accessible to the user because it is
    **    NOT visible. thus since all Locks have been released, we
    **    will close the document. if the app is only running due
    **    to the presence of this document, then the app will now
    **    also shut down.
    */
    OleDbgAssertSz (
			lpServerDoc->m_cPseudoObj > 0, 
			"PseudoObjUnlockDoc called with cPseudoObj == 0"
	);

    cPseudoObj = --lpServerDoc->m_cPseudoObj;

    OleDbgOutRefCnt3(
            "ServerDoc_PseudoObjUnlockDoc: cPseudoObj--\r\n", 
            lpServerDoc, 
            cPseudoObj
    );

    OleDoc_Lock(lpOleDoc, FALSE /* fLock */, TRUE /* fLastUnlockReleases */);
    
    OLEDBG_END3
    return;
}


/* ServerDoc_GetObject
** -------------------
**    
**    Return a pointer to an object identified by an item string
**    (lpszItem). For a server-only app, the object returned will be a
**    pseudo object.
*/
HRESULT ServerDoc_GetObject(
        LPSERVERDOC             lpServerDoc, 
        LPSTR                   lpszItem, 
        REFIID                  riid, 
        LPVOID FAR*             lplpvObject
)
{
    LPPSEUDOOBJ lpPseudoObj;
    LPSERVERNAMETABLE lpServerNameTable = 
            (LPSERVERNAMETABLE)((LPOUTLINEDOC)lpServerDoc)->m_lpNameTable;
    
    *lplpvObject = NULL;

    /* Get the PseudoObj which corresponds to an item name. if the item
    **    name does NOT exist in the name table then NO object is
    **    returned. the ServerNameTable_GetPseudoObj routine finds a
    **    name entry corresponding to the item name, it then checks if
    **    a PseudoObj has already been allocated. if so, it returns the
    **    existing object, otherwise it allocates a new PseudoObj.
    */
    lpPseudoObj = ServerNameTable_GetPseudoObj(
            lpServerNameTable,
            lpszItem,
            lpServerDoc
    );

    if (! lpPseudoObj) {
        *lplpvObject = NULL;        
        return ResultFromScode(MK_E_NOOBJECT);
    }

    // return the desired interface pointer of the pseudo object.
    return PseudoObj_QueryInterface(lpPseudoObj, riid, lplpvObject);
}


/* ServerDoc_IsRunning
** -------------------
**    
**    Check if the object identified by an item string (lpszItem) is in
**    the running state. For a server-only app, if the item name exists in 
**    in the NameTable then the item name is considered running. 
**    IOleItemContainer::GetObject would succeed.
*/

HRESULT ServerDoc_IsRunning(LPSERVERDOC lpServerDoc, LPSTR lpszItem)
{
    LPOUTLINENAMETABLE lpOutlineNameTable = 
            ((LPOUTLINEDOC)lpServerDoc)->m_lpNameTable;
    LPSERVERNAME lpServerName;
    
    lpServerName = (LPSERVERNAME)OutlineNameTable_FindName(
            lpOutlineNameTable,
            lpszItem
    );

    if (lpServerName) 
        return NOERROR;
    else 
        return ResultFromScode(MK_E_NOOBJECT);
}


/* ServerDoc_GetSelRelMoniker
** --------------------------
**    Retrieve the relative item moniker which identifies the given
**    selection (lplrSel). 
**    
**    Returns NULL if a moniker can NOT be created.
*/

LPMONIKER ServerDoc_GetSelRelMoniker(
        LPSERVERDOC             lpServerDoc, 
        LPLINERANGE             lplrSel,    
        DWORD                   dwAssign
)
{
    LPOUTLINEAPP lpOutlineApp = (LPOUTLINEAPP)g_lpApp;
    LPOUTLINEDOC lpOutlineDoc = (LPOUTLINEDOC)lpServerDoc;
    LPSERVERNAMETABLE lpServerNameTable = 
            (LPSERVERNAMETABLE)lpOutlineDoc->m_lpNameTable;
    LPOUTLINENAMETABLE lpOutlineNameTable = 
            (LPOUTLINENAMETABLE)lpServerNameTable;
    LPOUTLINENAME lpOutlineName;
    LPMONIKER lpmk;

    lpOutlineName=OutlineNameTable_FindNamedRange(lpOutlineNameTable,lplrSel);

    if (lpOutlineName) {
        /* the selection range already has a name assigned */
        CreateItemMoniker(OLESTDDELIM, lpOutlineName->m_szName, &lpmk);
    } else {
        char szbuf[MAXNAMESIZE];

        switch (dwAssign) {

            case GETMONIKER_FORCEASSIGN:
            
                /* Force the assignment of the name. This is called when a
                **    Paste Link actually occurs. At this point we want to
                **    create a Name and add it to the NameTable in order to
                **    track the source of the link. This name (as all
                **    names) will be updated upon editing of the document.
                */
                wsprintf(
                        szbuf, 
                        "%s %ld",
                        (LPSTR)DEFRANGENAMEPREFIX, 
                        ++(lpServerDoc->m_nNextRangeNo)
                );

                lpOutlineName = OutlineApp_CreateName(lpOutlineApp);

                if (lpOutlineName) {
                    lstrcpy(lpOutlineName->m_szName, szbuf);
                    lpOutlineName->m_nStartLine = lplrSel->m_nStartLine;
                    lpOutlineName->m_nEndLine = lplrSel->m_nEndLine;
                    OutlineDoc_AddName(lpOutlineDoc, lpOutlineName);
                } else {
                    // REVIEW: do we need "Out-of-Memory" error message here?
                }
                break;
                
            case GETMONIKER_TEMPFORUSER:

                /* Create a name to show to the user in the Paste
                **    Special dialog but do NOT yet incur the overhead
                **    of adding a Name to the NameTable. The Moniker
                **    generated should be useful to display to the user
                **    to indicate the source of the copy, but will NOT
                **    be used to create a link directly (the caller
                **    should ask again for a moniker specifying FORCEASSIGN).
                **    we will generate the name that would be the next
                **    auto-generated range name, BUT will NOT actually
                **    increment the range counter.
                */
                wsprintf(
                        szbuf, 
                        "%s %ld",
                        (LPSTR)DEFRANGENAMEPREFIX, 
                        (lpServerDoc->m_nNextRangeNo)+1
                );
                break;

            case GETMONIKER_ONLYIFTHERE:

                /* the caller only wants a name if one has already been
                **    assigned. we have already above checked if the
                **    current selection has a name, so we will simply
                **    return NULL here. 
                */
                return NULL;    // no moniker is assigned
                
            default:
                return NULL;    // unknown flag given
        }

        CreateItemMoniker(OLESTDDELIM, szbuf, &lpmk);
    }
    return lpmk;
}


/* ServerDoc_GetSelFullMoniker
** ---------------------------
**    Retrieve the full absolute moniker which identifies the given 
**    selection (lplrSel).
**    this moniker is created as a composite of the absolute moniker for 
**    the entire document appended with an item moniker which identifies 
**    the selection relative to the document.
**    Returns NULL if a moniker can NOT be created.
*/
LPMONIKER ServerDoc_GetSelFullMoniker(
        LPSERVERDOC             lpServerDoc, 
        LPLINERANGE             lplrSel,    
        DWORD                   dwAssign
)
{
    LPMONIKER lpmkDoc = NULL;
    LPMONIKER lpmkItem = NULL;
    LPMONIKER lpmkFull = NULL;

    lpmkDoc = OleDoc_GetFullMoniker(
            (LPOLEDOC)lpServerDoc,
            dwAssign
    );
    if (! lpmkDoc) return NULL;

    lpmkItem = ServerDoc_GetSelRelMoniker(
            lpServerDoc,
            lplrSel,
            dwAssign
    );
    if (lpmkItem) {
        CreateGenericComposite(lpmkDoc, lpmkItem, (LPMONIKER FAR*)&lpmkFull);
        OleStdRelease((LPUNKNOWN)lpmkItem);
    }

    if (lpmkDoc) 
        OleStdRelease((LPUNKNOWN)lpmkDoc);

    return lpmkFull;
}


/* ServerNameTable_EditLineUpdate
 * -------------------------------
 *
 *      Update the table when a line at nEditIndex is edited.
 */
void ServerNameTable_EditLineUpdate(
        LPSERVERNAMETABLE       lpServerNameTable, 
        int                     nEditIndex
)
{
    LPOUTLINENAMETABLE lpOutlineNameTable = 
                                        (LPOUTLINENAMETABLE)lpServerNameTable;
    LPOUTLINENAME lpOutlineName;
    LINERANGE lrSel;
    LPPSEUDOOBJ lpPseudoObj;
    int i;

    for(i = 0; i < lpOutlineNameTable->m_nCount; i++) {
        lpOutlineName=OutlineNameTable_GetName(lpOutlineNameTable, i);

        lpPseudoObj = ((LPSERVERNAME)lpOutlineName)->m_lpPseudoObj;
        
        /* if there is a pseudo object associated with this name, then
        **    check if the line that was modified is included within
        **    the named range.
        */
        if (lpPseudoObj) {
            OutlineName_GetSel(lpOutlineName, &lrSel);

            if(((int)lrSel.m_nStartLine <= nEditIndex) &&
                ((int)lrSel.m_nEndLine >= nEditIndex)) {

                // inform linking clients data has changed
                PseudoObj_SendAdvise(
						lpPseudoObj, 
						OLE_ONDATACHANGE, 
						NULL,	/* lpmkDoc -- not relevant here */
						0    	/* advf -- no flags necessary */
				);
            }
            
        }
    }
}


/* ServerNameTable_InformAllPseudoObjectsDocRenamed
 * ------------------------------------------------
 *
 *      Inform all pseudo object clients that the name of the pseudo 
 *      object has changed.
 */
void ServerNameTable_InformAllPseudoObjectsDocRenamed(
        LPSERVERNAMETABLE       lpServerNameTable, 
        LPMONIKER               lpmkDoc
)
{
    LPOUTLINENAMETABLE lpOutlineNameTable = 
                                        (LPOUTLINENAMETABLE)lpServerNameTable;
    LPOUTLINENAME lpOutlineName;
    LPPSEUDOOBJ lpPseudoObj;
    LPMONIKER lpmkObj;
    int i;

    OLEDBG_BEGIN2("ServerNameTable_InformAllPseudoObjectsDocRenamed\r\n");

    for(i = 0; i < lpOutlineNameTable->m_nCount; i++) {
        lpOutlineName=OutlineNameTable_GetName(lpOutlineNameTable, i);

        lpPseudoObj = ((LPSERVERNAME)lpOutlineName)->m_lpPseudoObj;
        
        /* if there is a pseudo object associated with this name, then
        **    send OnRename advise to its linking clients.
        */
        if (lpPseudoObj && 
            ((lpmkObj=PseudoObj_GetFullMoniker(lpPseudoObj,lpmkDoc))!=NULL)) {

            // inform the clients that the name has changed
            PseudoObj_SendAdvise (
					lpPseudoObj, 
					OLE_ONRENAME, 
					lpmkObj,
					0    		/* advf -- not relevant here */
			);
        }
    }
    OLEDBG_END2
}


/* ServerNameTable_InformAllPseudoObjectsDocSaved
 * ------------------------------------------------
 *
 *      Inform all pseudo object clients that the name of the pseudo 
 *      object has changed.
 */
void ServerNameTable_InformAllPseudoObjectsDocSaved(
        LPSERVERNAMETABLE       lpServerNameTable, 
        LPMONIKER               lpmkDoc
)
{
    LPOUTLINENAMETABLE lpOutlineNameTable = 
                                        (LPOUTLINENAMETABLE)lpServerNameTable;
    LPOUTLINENAME lpOutlineName;
    LPPSEUDOOBJ lpPseudoObj;
    LPMONIKER lpmkObj;
    int i;

    OLEDBG_BEGIN2("ServerNameTable_InformAllPseudoObjectsDocSaved\r\n");

    for(i = 0; i < lpOutlineNameTable->m_nCount; i++) {
        lpOutlineName=OutlineNameTable_GetName(lpOutlineNameTable, i);

        lpPseudoObj = ((LPSERVERNAME)lpOutlineName)->m_lpPseudoObj;
        
        /* if there is a pseudo object associated with this name, then
        **    send OnSave advise to its linking clients.
        */
        if (lpPseudoObj && 
            ((lpmkObj=PseudoObj_GetFullMoniker(lpPseudoObj,lpmkDoc))!=NULL)) {

            // inform the clients that the name has been saved
            PseudoObj_SendAdvise (
					lpPseudoObj, 
					OLE_ONSAVE, 
					NULL,	/* lpmkDoc -- not relevant here */
					0   	/* advf -- not relevant here */
			);
        }
    }
    OLEDBG_END2
}


/* ServerNameTable_SendPendingAdvises
 * ----------------------------------
 *
 *      Send any pending change notifications for pseudo objects.
 *  while ReDraw is diabled on the ServerDoc, then change advise 
 *  notifications are not sent to pseudo object clients.
 */
void ServerNameTable_SendPendingAdvises(LPSERVERNAMETABLE lpServerNameTable)
{
    LPOUTLINENAMETABLE lpOutlineNameTable = 
                                        (LPOUTLINENAMETABLE)lpServerNameTable;
    LPSERVERNAME lpServerName;
    int i;

    for(i = 0; i < lpOutlineNameTable->m_nCount; i++) {
        lpServerName = (LPSERVERNAME)OutlineNameTable_GetName(
                lpOutlineNameTable, 
                i
        );
        ServerName_SendPendingAdvises(lpServerName);
    }
}


/* ServerNameTable_GetPseudoObj 
** ----------------------------
** 
**    Return a pointer to a pseudo object identified by an item string
**    (lpszItem). if the pseudo object already exists, then return the
**    existing object, otherwise allocate a new pseudo object.
*/
LPPSEUDOOBJ ServerNameTable_GetPseudoObj(
        LPSERVERNAMETABLE       lpServerNameTable, 
        LPSTR                   lpszItem, 
        LPSERVERDOC             lpServerDoc
)
{
    LPSERVERNAME lpServerName;
    
    lpServerName = (LPSERVERNAME)OutlineNameTable_FindName(
            (LPOUTLINENAMETABLE)lpServerNameTable,
            lpszItem
    );

    if (lpServerName) 
        return ServerName_GetPseudoObj(lpServerName, lpServerDoc);
    else 
        return NULL;
}


/* ServerNameTable_CloseAllPseudoObjs
 * ----------------------------------
 *
 *  Force all pseudo objects to close. this results in sending OnClose 
 *  notification to each pseudo object's linking clients.
 */
void ServerNameTable_CloseAllPseudoObjs(LPSERVERNAMETABLE lpServerNameTable)
{
    LPOUTLINENAMETABLE lpOutlineNameTable = 
                                        (LPOUTLINENAMETABLE)lpServerNameTable;
    LPSERVERNAME lpServerName;
    int i;

	OLEDBG_BEGIN3("ServerNameTable_CloseAllPseudoObjs\r\n")

    for(i = 0; i < lpOutlineNameTable->m_nCount; i++) {
        lpServerName = (LPSERVERNAME)OutlineNameTable_GetName(
                lpOutlineNameTable, 
                i
        );
        ServerName_ClosePseudoObj(lpServerName);
    }
	
	OLEDBG_END3
}



/* ServerName_SetSel
 * -----------------
 *
 *      Change the line range of a  name.
 */
void ServerName_SetSel(
        LPSERVERNAME            lpServerName, 
        LPLINERANGE             lplrSel, 
        BOOL                    fRangeModified
)
{
    LPOUTLINENAME lpOutlineName = (LPOUTLINENAME)lpServerName;
    BOOL fPseudoObjChanged = fRangeModified;

    if (lpOutlineName->m_nStartLine != lplrSel->m_nStartLine) {
        lpOutlineName->m_nStartLine = lplrSel->m_nStartLine;
        fPseudoObjChanged = TRUE;
    }

    if (lpOutlineName->m_nEndLine != lplrSel->m_nEndLine) {
        lpOutlineName->m_nEndLine = lplrSel->m_nEndLine;
        fPseudoObjChanged = TRUE;
    }

    /* OLE2NOTE: if the range of an active pseudo object has
    **    changed, then inform any linking clients that the object
    **    has changed.
    */
    if (lpServerName->m_lpPseudoObj && fPseudoObjChanged) {
        PseudoObj_SendAdvise(
                lpServerName->m_lpPseudoObj, 
				OLE_ONDATACHANGE, 
				NULL,	/* lpmkDoc -- not relevant here */
				0   	/* advf -- no flags necessary */
		);
    }
}


/* ServerName_SendPendingAdvises
 * -----------------------------
 *
 *      Send any pending change notifications for the associated 
 *  pseudo objects for this name (if one exists).
 *  while ReDraw is diabled on the ServerDoc, then change advise 
 *  notifications are not sent to pseudo object clients.
 */
void ServerName_SendPendingAdvises(LPSERVERNAME lpServerName)
{
    if (! lpServerName->m_lpPseudoObj) 
        return;     // no associated pseudo object

    if (lpServerName->m_lpPseudoObj->m_fDataChanged) 
        PseudoObj_SendAdvise(
                lpServerName->m_lpPseudoObj, 
				OLE_ONDATACHANGE, 
				NULL,	/* lpmkDoc -- not relevant here */
				0   	/* advf -- no flags necessary */
		);
}


/* ServerName_GetPseudoObj
** -----------------------
** 
**    Return a pointer to a pseudo object associated to a ServerName.
**    if the pseudo object already exists, then return the
**    existing object, otherwise allocate a new pseudo object.
**    
**    NOTE: the PseudoObj is returned with a 0 refcnt if first created,
**    else the existing refcnt is unchanged. 
*/
LPPSEUDOOBJ ServerName_GetPseudoObj(
        LPSERVERNAME            lpServerName, 
        LPSERVERDOC             lpServerDoc
)
{
    // Check if a PseudoObj already exists
    if (lpServerName->m_lpPseudoObj) 
        return lpServerName->m_lpPseudoObj;

    // A PseudoObj does NOT already exist, allocate a new one.
    lpServerName->m_lpPseudoObj=(LPPSEUDOOBJ) New((DWORD)sizeof(PSEUDOOBJ));
    if (lpServerName->m_lpPseudoObj == NULL) {
        OleDbgAssertSz(lpServerName->m_lpPseudoObj != NULL, 
                                                "Error allocating PseudoObj");
        return NULL; 
    }

    PseudoObj_Init(lpServerName->m_lpPseudoObj, lpServerName, lpServerDoc);
    return lpServerName->m_lpPseudoObj;
}


/* ServerName_ClosePseudoObj
 * -------------------------
 *
 *      if there is an associated pseudo objects for this name (if one 
 *  exists), then close it. this results in sending OnClose 
 *  notification to the pseudo object's linking clients.
 */
void ServerName_ClosePseudoObj(LPSERVERNAME lpServerName)
{
    if (! lpServerName->m_lpPseudoObj) 
        return;     // no associated pseudo object

    PseudoObj_Close(lpServerName->m_lpPseudoObj);
}


#endif  // OLE_SERVER


#if defined( OLE_CNTR )


/*************************************************************************
** ContainerDoc Supprt Functions Used by Container versions
*************************************************************************/


/* ContainerDoc_InformAllOleObjectsDocRenamed
** ------------------------------------------
**    Inform all OLE objects that the name of the ContainerDoc has changed.
*/
void ContainerDoc_InformAllOleObjectsDocRenamed(
        LPCONTAINERDOC          lpContainerDoc,
        LPMONIKER               lpmkDoc
)
{
    LPLINELIST lpLL = &((LPOUTLINEDOC)lpContainerDoc)->m_LineList;
    int i;
    LPLINE lpLine;

    for (i = 0; i < lpLL->m_nNumLines; i++) {
        lpLine=LineList_GetLine(lpLL, i);

        if (lpLine && (Line_GetLineType(lpLine)==CONTAINERLINETYPE)) {
            LPCONTAINERLINE lpContainerLine = (LPCONTAINERLINE)lpLine;

            /* OLE2NOTE: if the OLE object is already loaded AND the
            **    object already has a moniker assigned, then we need
            **    to inform it that the moniker of the ContainerDoc has
            **    changed. of course, this means the full moniker of
            **    the object has changed. to do this we call
            **    IOleObject::SetMoniker. this will force the OLE
            **    object to re-register in the RunningObjectTable if it
            **    is currently in the running state. it is not in the
            **    running state, the object handler can make not that
            **    the object has a new moniker. if the object is not
            **    currently loaded, SetMoniker will be called
            **    automatically later when the object is loaded by the
            **    function ContainerLine_LoadOleObject.
            **    also if the object is a linked object, we always want
            **    to call SetMoniker on the link so that in case the
            **    link source is contained within our same container,
            **    the link source will be tracked. the link rebuilds
            **    its absolute moniker if it has a relative moniker.
            */
            if (lpContainerLine->m_lpOleObj) {
                if (lpContainerLine->m_fMonikerAssigned ||
                    lpContainerLine->m_fIsLink) {
                    OLEDBG_BEGIN2("IOleObject::SetMoniker called\r\n")
                    lpContainerLine->m_lpOleObj->lpVtbl->SetMoniker(
                            lpContainerLine->m_lpOleObj,
                            OLEWHICHMK_CONTAINER,
                            lpmkDoc
                    );
                    OLEDBG_END2
                }
                
                /* OLE2NOTE: we must call IOleObject::SetHostNames so
                **    any open objects can update their window titles.
                */
                OLEDBG_BEGIN2("IOleObject::SetHostNames called\r\n")
                lpContainerLine->m_lpOleObj->lpVtbl->SetHostNames(
                        lpContainerLine->m_lpOleObj,
                        (LPSTR)APPNAME,
                        ((LPOUTLINEDOC)lpContainerDoc)->m_lpszDocTitle
                );
                OLEDBG_END2
            }
        }
    }
}


/* ContainerDoc_GetObject
** ----------------------
**    Return a pointer to the desired interface of an object identified
**    by an item string (lpszItem). the object returned will be an OLE
**    object (either link or embedding).
**
**      OLE2NOTE: we must force the object to run because we are
**          REQUIRED to return a pointer the OLE object in the
**          RUNNING state.
**
**    dwSpeedNeeded indicates how long the caller is willing
**    to wait for us to get the object:
**      BINDSPEED_IMMEDIATE -- only if obj already loaded && IsRunning
**      BINDSPEED_MODERATE  -- load obj if necessary && if IsRunning
**      BINDSPEED_INDEFINITE-- force obj to load and run if necessary
*/
HRESULT ContainerDoc_GetObject(
        LPCONTAINERDOC          lpContainerDoc,
        LPSTR                   lpszItem,
        DWORD                   dwSpeedNeeded,
        REFIID                  riid,
        LPVOID FAR*             lplpvObject
)
{
    LPLINELIST  lpLL = &((LPOUTLINEDOC)lpContainerDoc)->m_LineList;
    int         i;
    LPLINE      lpLine;
    BOOL        fMatchFound = FALSE;
    DWORD       dwStatus;
    HRESULT     hrErr;

    *lplpvObject = NULL;

    for (i = 0; i < lpLL->m_nNumLines; i++) {
        lpLine=LineList_GetLine(lpLL, i);

        if (lpLine && (Line_GetLineType(lpLine)==CONTAINERLINETYPE)) {
            LPCONTAINERLINE lpContainerLine = (LPCONTAINERLINE)lpLine;

            if (lstrcmp(lpContainerLine->m_szStgName, lpszItem) == 0) {

                fMatchFound = TRUE;     // valid item name

                // check if object is loaded.
                if (lpContainerLine->m_lpOleObj == NULL) {

                    // if BINDSPEED_IMMEDIATE is requested, object must
                    // ALREADY be loadded.
                    if (dwSpeedNeeded == BINDSPEED_IMMEDIATE)
                        return ResultFromScode(MK_E_EXCEEDEDDEADLINE);

                    ContainerLine_LoadOleObject(lpContainerLine);
                    if (! lpContainerLine->m_lpOleObj) 
                        return ResultFromScode(E_OUTOFMEMORY);
                }

                /* OLE2NOTE: check if the object is allowed to be linked
                **    to from the inside (ie. we are allowed to
                **    give out a moniker which binds to the running
                **    OLE object). if the object is an OLE
                **    2.0 embedded object then it is allowed to be
                **    linked to from the inside. if the object is
                **    either an OleLink or an OLE 1.0 embedding
                **    then it can not be linked to from the inside.
                **    if we were a container/server app then we
                **    could offer linking to the outside of the
                **    object (ie. a pseudo object within our
                **    document). we are a container only app that
                **    does not support linking to ranges of its data.
                */
                OLEDBG_BEGIN2("IOleObject::GetMiscStatus called\r\n");
                lpContainerLine->m_lpOleObj->lpVtbl->GetMiscStatus(
                        lpContainerLine->m_lpOleObj, 
                        DVASPECT_CONTENT, /* aspect is not important */
                        (LPDWORD)&dwStatus
                );
                OLEDBG_END2
                if (dwStatus & OLEMISC_CANTLINKINSIDE) 
                    return ResultFromScode(MK_E_NOOBJECT);

                // check if object is running.
                if (! OleIsRunning(lpContainerLine->m_lpOleObj)) {

                    // if BINDSPEED_MODERATE is requested, object must
                    // ALREADY be running.
                    if (dwSpeedNeeded == BINDSPEED_MODERATE)
                        return ResultFromScode(MK_E_EXCEEDEDDEADLINE);

                    /* OLE2NOTE: we have found a match for the item name.
                    **    now we must return a pointer to the desired
                    **    interface on the RUNNING object. we must
                    **    carefully load the object and initially ask for
                    **    an interface that we are sure the loaded form of
                    **    the object supports. if we immediately ask the
                    **    loaded object for the desired interface, the
                    **    QueryInterface call might fail if it is an
                    **    interface that is supported only when the object
                    **    is running. thus we force the object to load and
                    **    return its IUnknown*. then we force the object to
                    **    run, and then finally, we can ask for the
                    **    actually requested interface.
                    */
                    hrErr = ContainerLine_RunOleObject(lpContainerLine);
                    if (hrErr != NOERROR) {
                        /* OLE2NOTE: this demonstrates an example use of
                        **    PropagateResult. this allows us to return an
                        **    SCODE of our choosing, but still passing
                        **    along the error context of the previous
                        **    error. in the future this will be used to
                        **    stack up error contexts.
                        */
                        return PropagateResult(hrErr, E_FAIL);
                    }
                }

                // Retrieve the requested interface
                *lplpvObject = OleStdQueryInterface(
                        (LPUNKNOWN)lpContainerLine->m_lpOleObj, riid);

                break;  // Match FOUND!
            }
        }
    }

    if (*lplpvObject != NULL) {
        return NOERROR;
    } else
        return (fMatchFound ? ResultFromScode(E_NOINTERFACE) 
                            : ResultFromScode(MK_E_NOOBJECT));
}


/* ContainerDoc_GetObjectStorage
** -----------------------------
**    Return a pointer to the IStorage* used by the object identified
**    by an item string (lpszItem). the object identified could be either
**    an OLE object (either link or embedding).
*/
HRESULT ContainerDoc_GetObjectStorage(
        LPCONTAINERDOC          lpContainerDoc,
        LPSTR                   lpszItem,
        LPSTORAGE FAR*          lplpStg
)
{
    LPLINELIST lpLL = &((LPOUTLINEDOC)lpContainerDoc)->m_LineList;
    int i;
    LPLINE lpLine;

    *lplpStg = NULL;

    for (i = 0; i < lpLL->m_nNumLines; i++) {
        lpLine=LineList_GetLine(lpLL, i);

        if (lpLine && (Line_GetLineType(lpLine)==CONTAINERLINETYPE)) {
            LPCONTAINERLINE lpContainerLine = (LPCONTAINERLINE)lpLine;

            if (lstrcmp(lpContainerLine->m_szStgName, lpszItem) == 0) {

                *lplpStg = lpContainerLine->m_lpStg;
                break;  // Match FOUND!
            }
        }
    }

    if (*lplpStg != NULL) {
        return NOERROR;
    } else
        return ResultFromScode(MK_E_NOOBJECT);
}


/* ContainerDoc_IsRunning
** ----------------------
**    Check if the object identified by an item string (lpszItem) is in
**    the running state.
**    For a container-only app, a check is made if the OLE object
**    associated with the item name is running.
*/
HRESULT ContainerDoc_IsRunning(LPCONTAINERDOC   lpContainerDoc, LPSTR lpszItem)
{
    LPLINELIST  lpLL = &((LPOUTLINEDOC)lpContainerDoc)->m_LineList;
    int         i;
    LPLINE      lpLine;
    DWORD       dwStatus;

    for (i = 0; i < lpLL->m_nNumLines; i++) {
        lpLine=LineList_GetLine(lpLL, i);

        if (lpLine && (Line_GetLineType(lpLine)==CONTAINERLINETYPE)) {
            LPCONTAINERLINE lpContainerLine = (LPCONTAINERLINE)lpLine;

            if (lstrcmp(lpContainerLine->m_szStgName, lpszItem) == 0) {

                /* OLE2NOTE: we have found a match for the item name.
                **    now we must check if the OLE object is running.
                **    we will load the object if not already loaded.
                */
                if (! lpContainerLine->m_lpOleObj) {
                    ContainerLine_LoadOleObject(lpContainerLine);
                    if (! lpContainerLine->m_lpOleObj) 
                        return ResultFromScode(E_OUTOFMEMORY);
                }
				
                /* OLE2NOTE: check if the object is allowed to be linked
                **    to from the inside (ie. we are allowed to
                **    give out a moniker which binds to the running
                **    OLE object). if the object is an OLE
                **    2.0 embedded object then it is allowed to be
                **    linked to from the inside. if the object is
                **    either an OleLink or an OLE 1.0 embedding
                **    then it can not be linked to from the inside.
                **    if we were a container/server app then we
                **    could offer linking to the outside of the
                **    object (ie. a pseudo object within our
                **    document). we are a container only app that
                **    does not support linking to ranges of its data.
                */
                OLEDBG_BEGIN2("IOleObject::GetMiscStatus called\r\n")
                lpContainerLine->m_lpOleObj->lpVtbl->GetMiscStatus(
                        lpContainerLine->m_lpOleObj, 
                        DVASPECT_CONTENT, /* aspect is not important */
                        (LPDWORD)&dwStatus
                );
                OLEDBG_END2
                if (dwStatus & OLEMISC_CANTLINKINSIDE) 
                    return ResultFromScode(MK_E_NOOBJECT);

                if (OleIsRunning(lpContainerLine->m_lpOleObj))
                    return NOERROR;
                else
                    return ResultFromScode(S_FALSE);
            }
        }
    }

    // no object was found corresponding to the item name
    return ResultFromScode(MK_E_NOOBJECT);
}

#endif  // OLE_CNTR
