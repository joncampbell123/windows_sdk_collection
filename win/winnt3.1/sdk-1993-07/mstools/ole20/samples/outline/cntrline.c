/*************************************************************************
** 
**    OLE 2 Container Sample Code
**    
**    cntrline.c
**    
**    This file contains ContainerLine methods.
**    
**    (c) Copyright Microsoft Corp. 1992 - 1993 All Rights Reserved
**    
*************************************************************************/

#include "outline.h"

OLEDBGDATA



extern LPOUTLINEAPP         g_lpApp;
extern IUnknownVtbl         g_CntrLine_UnknownVtbl;
extern IOleClientSiteVtbl   g_CntrLine_OleClientSiteVtbl;
extern IAdviseSinkVtbl      g_CntrLine_AdviseSinkVtbl;

#if defined( INPLACE_CNTR )
extern IOleInPlaceSiteVtbl  g_CntrLine_OleInPlaceSiteVtbl;
extern BOOL g_fInsideOutContainer;
#endif  // INPLACE_CNTR

// REVIEW: should use string resource for messages
char ErrMsgDoVerb[] = "OLE object action failed!";


/* prototype for static functions */
static void InvertDiffRect(LPRECT lprcPix, LPRECT lprcObj, HDC hDC);


/*************************************************************************
** ContainerLine
**    This object represents the location within the container where
**    the embedded/linked object lives. It exposes interfaces to the
**    object that allow the object to get information about its
**    embedding site and to announce notifications of important events
**    (changed, closed, saved)
**    
**    The ContainerLine exposes the following interfaces:
**      IOleClientSite
**      IAdviseSink
*************************************************************************/

    

/* ContainerLine_Init
** ------------------
**  Initialize fields in a newly constructed ContainerLine line object. 
**  NOTE: ref cnt of ContainerLine initialized to 0
*/
void ContainerLine_Init(LPCONTAINERLINE lpContainerLine, int nTab, HDC hDC)
{
    Line_Init((LPLINE)lpContainerLine, nTab, hDC);  // init base class fields

    ((LPLINE)lpContainerLine)->m_lineType           = CONTAINERLINETYPE;
    ((LPLINE)lpContainerLine)->m_nWidthInHimetric   = DEFOBJWIDTH;
    ((LPLINE)lpContainerLine)->m_nHeightInHimetric  = DEFOBJHEIGHT;
    lpContainerLine->m_cRef                         = 0;
    lpContainerLine->m_szStgName[0]                 = '\0';
    lpContainerLine->m_fObjWinOpen                  = FALSE;
    lpContainerLine->m_fMonikerAssigned             = FALSE;
    lpContainerLine->m_dwDrawAspect                 = DVASPECT_CONTENT;

    /* when we initially create the ContainerLine we do not know the
    **    extents of the object. force the extents to be updated the
	**    first time the line is drawn. 
    */
	lpContainerLine->m_fDoGetExtent					= TRUE;
    lpContainerLine->m_sizeInHimetric.cx			= -1;
    lpContainerLine->m_sizeInHimetric.cy			= -1;

    lpContainerLine->m_lpStg                        = NULL;
    lpContainerLine->m_lpOleObj                     = NULL;
    lpContainerLine->m_lpViewObj                    = NULL;
    lpContainerLine->m_lpPersistStg                 = NULL;
    lpContainerLine->m_lpDoc                        = NULL;
    lpContainerLine->m_fIsLink                      = FALSE;
    lpContainerLine->m_fLinkUnavailable             = FALSE;
    lpContainerLine->m_lpszShortType                = NULL;

#if defined( INPLACE_CNTR )
    lpContainerLine->m_fIpActive                    = FALSE;
    lpContainerLine->m_fUIActive                    = FALSE;
    lpContainerLine->m_fIpVisible                   = FALSE;
	lpContainerLine->m_lpOleIPObj					= NULL;
    lpContainerLine->m_fInsideOutObj                = FALSE;
    lpContainerLine->m_fIpChangesUndoable           = FALSE;
    lpContainerLine->m_fIpServerRunning             = FALSE;
#endif  // INPLACE_CNTR

    INIT_INTERFACEIMPL(
            &lpContainerLine->m_Unknown,
            &g_CntrLine_UnknownVtbl,
            lpContainerLine
    );

    INIT_INTERFACEIMPL(
            &lpContainerLine->m_OleClientSite,
            &g_CntrLine_OleClientSiteVtbl,
            lpContainerLine
    );

    INIT_INTERFACEIMPL(
            &lpContainerLine->m_AdviseSink,
            &g_CntrLine_AdviseSinkVtbl,
            lpContainerLine
    );

#if defined( INPLACE_CNTR )
    INIT_INTERFACEIMPL(
            &lpContainerLine->m_OleInPlaceSite,
            &g_CntrLine_OleInPlaceSiteVtbl,
            lpContainerLine
    );
#endif  // INPLACE_CNTR
}


/* Create an ContainerLine object and return the pointer */
LPCONTAINERLINE ContainerLine_Create(
        DWORD                   dwOleCreateType,
        HDC                     hDC, 
        UINT                    nTab, 
        LPCONTAINERDOC          lpContainerDoc, 
        LPCLSID                 lpclsid, 
        LPSTR                   lpszFileName,
		BOOL					fDisplayAsIcon,
		HGLOBAL					hMetaPict,
        LPSTR                   lpszStgName
)
{
    LPCONTAINERLINE lpContainerLine = NULL;
    LPOLEOBJECT     lpObj = NULL;
    LPSTORAGE       lpDocStg = ContainerDoc_GetStg(lpContainerDoc);
	DWORD			dwDrawAspect = 
						(fDisplayAsIcon ? DVASPECT_ICON : DVASPECT_CONTENT);
	DWORD			dwOleRenderOpt = 
						(fDisplayAsIcon ? OLERENDER_NONE : OLERENDER_DRAW);
    HRESULT         hrErr;

    OLEDBG_BEGIN3("ContainerLine_Create\r\n")

    if (! OleDbgVerifySz(lpDocStg != NULL, "Doc storage is NULL")) 
        goto error;

    lpContainerLine=(LPCONTAINERLINE) New((DWORD)sizeof(CONTAINERLINE));
    if (lpContainerLine == NULL) {
        OleDbgAssertSz(
                lpContainerLine!=NULL, "Error allocating ContainerLine");
        goto error; 
    }

    ContainerLine_Init(lpContainerLine, nTab, hDC);

    /* OLE2NOTE: In order to have a stable ContainerLine object we must
	**    AddRef the object's refcnt. this will be later released when
	**    the ContainerLine is deleted.
    */
    ContainerLine_AddRef(lpContainerLine);
    
    lstrcpy(lpContainerLine->m_szStgName, lpszStgName);
    lpContainerLine->m_lpDoc = lpContainerDoc;

    /* Create a new storage for the object inside the doc's storage */
    lpContainerLine->m_lpStg = OleStdCreateChildStorage(lpDocStg,lpszStgName);
    if (! OleDbgVerifySz(lpContainerLine->m_lpStg != NULL, 
                                                "Error creating child stg"))
        goto error;

	lpContainerLine->m_fIsLink = FALSE;
	
    switch (dwOleCreateType) {

        case IOF_SELECTCREATENEW:

            OLEDBG_BEGIN2("OleCreate called\r\n")
            hrErr = OleCreate (
                    lpclsid,
                    &IID_IOleObject,
                    dwOleRenderOpt,
                    NULL,
                    (LPOLECLIENTSITE)&lpContainerLine->m_OleClientSite,
                    lpContainerLine->m_lpStg,
                    (LPVOID FAR*)&lpContainerLine->m_lpOleObj
            );
            OLEDBG_END2

#if defined( INPLACE_CNTR )
            /* OLE2NOTE: an inside-out container should check if
            **    the object is an inside-out and prefers to be
            **    activated when visible type of object. if not the
            **    object should not be allowed to keep its window
            **    up after it gets UIDeactivated.
            */
            if (g_fInsideOutContainer && 
                lpContainerLine->m_dwDrawAspect == DVASPECT_CONTENT) {
                    DWORD mstat;
                    OLEDBG_BEGIN2("IOleObject::GetMiscStatus called\r\n")
                    lpContainerLine->m_lpOleObj->lpVtbl->GetMiscStatus(
                            lpContainerLine->m_lpOleObj,
                            DVASPECT_CONTENT,
                            (DWORD FAR*)&mstat
                    );
                    OLEDBG_END2
       
                    lpContainerLine->m_fInsideOutObj = (BOOL)
                          (mstat & 
                             (OLEMISC_INSIDEOUT|OLEMISC_ACTIVATEWHENVISIBLE));
                }
#endif  // INPLACE_CNTR

            break;

        case IOF_SELECTCREATEFROMFILE:

            OLEDBG_BEGIN2("OleCreateFromFile called\r\n")

            hrErr = OleCreateFromFile (
                    &CLSID_NULL,
                    lpszFileName,
                    &IID_IOleObject,
                    dwOleRenderOpt,
                    NULL,
                    (LPOLECLIENTSITE)&lpContainerLine->m_OleClientSite,
                    lpContainerLine->m_lpStg,
                    (LPVOID FAR*)&lpContainerLine->m_lpOleObj
            );
         
            OLEDBG_END2
            break;

        case IOF_CHECKLINK:

            OLEDBG_BEGIN2("OleCreateLinkToFile called\r\n")

            hrErr = OleCreateLinkToFile (
                    lpszFileName,
                    &IID_IOleObject,
                    dwOleRenderOpt,
                    NULL,
                    (LPOLECLIENTSITE)&lpContainerLine->m_OleClientSite,
                    lpContainerLine->m_lpStg,
                    (LPVOID FAR*)&lpContainerLine->m_lpOleObj
            );
         
            OLEDBG_END2
			lpContainerLine->m_fIsLink = TRUE;
            break;
    }
        
    if (hrErr != NOERROR) {
        OutlineApp_ErrorMessage(g_lpApp, "Could not create object!");
        goto error;
    }

    /* Setup the Advises (OLE notifications) that we are interested 
    ** in receiving.
    */
    OleStdSetupAdvises(
            lpContainerLine->m_lpOleObj, 
            dwDrawAspect, 
            (LPSTR)APPNAME, 
            ((LPOUTLINEDOC)lpContainerDoc)->m_lpszDocTitle,
            (LPADVISESINK)&lpContainerLine->m_AdviseSink
    );

	if (fDisplayAsIcon) {
		BOOL fMustUpdate;	

		/* user has requested to display icon aspect instead of content 
		**    aspect. 
		**    NOTE: we do not have to delete the previous aspect cache
		**    because one did not get set up.
		*/
		OleStdSwitchDisplayAspect(
				lpContainerLine->m_lpOleObj, 
				&lpContainerLine->m_dwDrawAspect,
				dwDrawAspect, 
				hMetaPict,
				FALSE,	/* fDeleteOldAspect */
				TRUE,	/* fSetupViewAdvise */
				(LPADVISESINK)&lpContainerLine->m_AdviseSink,
				(BOOL FAR*)&fMustUpdate		// this can be ignored; update
											// for switch to icon not req'd
		);
	}
    OLEDBG_END3
    return lpContainerLine;
    
error:
    // Destroy partially created OLE object
    if (lpContainerLine) 
        ContainerLine_Delete(lpContainerLine);  
    OLEDBG_END3
    return NULL;
}   


LPCONTAINERLINE ContainerLine_CreateFromData(
        HDC                     hDC, 
        UINT                    nTab, 
        LPCONTAINERDOC          lpContainerDoc, 
        LPDATAOBJECT            lpSrcDataObj,
        DWORD                   dwCreateType,
        CLIPFORMAT              cfFormat,
		BOOL					fDisplayAsIcon,
		HGLOBAL					hMetaPict,
        LPSTR                   lpszStgName
)
{
    HGLOBAL         hData = NULL;
    LPCONTAINERLINE lpContainerLine = NULL;
    LPOLEOBJECT     lpObj = NULL;
    LPSTORAGE       lpDocStg = ContainerDoc_GetStg(lpContainerDoc);
	DWORD			dwDrawAspect = 
						(fDisplayAsIcon ? DVASPECT_ICON : DVASPECT_CONTENT);
	DWORD			dwOleRenderOpt = 
						(fDisplayAsIcon ? OLERENDER_NONE : OLERENDER_DRAW);
    FORMATETC       renderFmtEtc;
    LPFORMATETC     lpRenderFmtEtc = NULL;
    HRESULT         hrErr;
	LPUNKNOWN		lpUnk = NULL;

    OLEDBG_BEGIN3("ContainerLine_CreateFromData\r\n")

    if (dwCreateType == OLECREATEFROMDATA_STATIC && cfFormat != 0) {
        // a particular type of static object should be created

        dwOleRenderOpt = OLERENDER_FORMAT;
        lpRenderFmtEtc = (LPFORMATETC)&renderFmtEtc;

        if (cfFormat == CF_METAFILEPICT) 
            SETDEFAULTFORMATETC(renderFmtEtc, cfFormat, TYMED_MFPICT); 
        else if (cfFormat == CF_BITMAP) 
            SETDEFAULTFORMATETC(renderFmtEtc, cfFormat, TYMED_GDI); 
        else 
            SETDEFAULTFORMATETC(renderFmtEtc, cfFormat, TYMED_HGLOBAL); 
    } else if (fDisplayAsIcon) {
        dwOleRenderOpt = OLERENDER_NONE;

    } else {
        dwOleRenderOpt = OLERENDER_DRAW;
    }

    if (! OleDbgVerifySz(lpDocStg != NULL, "Doc storage is NULL")) 
        goto error;

    lpContainerLine=(LPCONTAINERLINE) New((DWORD)sizeof(CONTAINERLINE));
    if (lpContainerLine == NULL) {
        OleDbgAssertSz(
                lpContainerLine!=NULL, "Error allocating ContainerLine");
        goto error; 
    }

    ContainerLine_Init(lpContainerLine, nTab, hDC);

    /* OLE2NOTE: In order to have a stable ContainerLine object we must
	**    AddRef the object's refcnt. this will be later released when
	**    the ContainerLine is deleted.
    */
    ContainerLine_AddRef(lpContainerLine);
    
    lstrcpy(lpContainerLine->m_szStgName, lpszStgName);
    lpContainerLine->m_lpDoc = lpContainerDoc;

    /* Create a new storage for the object inside the doc's storage */
    lpContainerLine->m_lpStg = OleStdCreateChildStorage(lpDocStg,lpszStgName);
    if (! OleDbgVerifySz(lpContainerLine->m_lpStg != NULL, 
                                                "Error creating child stg"))
        goto error;

    switch (dwCreateType) {

        case OLECREATEFROMDATA_LINK:

            OLEDBG_BEGIN2("OleCreateLinkFromData called\r\n")
            hrErr = OleCreateLinkFromData (
                    lpSrcDataObj,
                    &IID_IOleObject,
                    dwOleRenderOpt,
                    lpRenderFmtEtc,
                    (LPOLECLIENTSITE)&lpContainerLine->m_OleClientSite,
                    lpContainerLine->m_lpStg,
                    (LPVOID FAR*)&lpContainerLine->m_lpOleObj
            );
            OLEDBG_END2
		
            if (hrErr != NOERROR) {
                OleDbgOutHResult("OleCreateLinkFromData returned", hrErr);
            }

		    lpContainerLine->m_fIsLink = TRUE;			
            break;
        
        case OLECREATEFROMDATA_OBJECT:

            OLEDBG_BEGIN2("OleCreateFromData called\r\n")
            hrErr = OleCreateFromData (
                    lpSrcDataObj,
                    &IID_IOleObject,
                    dwOleRenderOpt,
                    lpRenderFmtEtc,
                    (LPOLECLIENTSITE)&lpContainerLine->m_OleClientSite,
                    lpContainerLine->m_lpStg,
                    (LPVOID FAR*)&lpContainerLine->m_lpOleObj
            );
            OLEDBG_END2
		
            if (hrErr != NOERROR) {
                OleDbgOutHResult("OleCreateFromData returned", hrErr);
            }

            lpUnk=ContainerLine_GetOleObject(lpContainerLine,&IID_IOleLink);
		    if (lpUnk) {
                OleStdRelease(lpUnk);
                lpContainerLine->m_fIsLink = TRUE;
            } 
            else {
                lpContainerLine->m_fIsLink = FALSE;

#if defined( INPLACE_CNTR )
                /* OLE2NOTE: an inside-out container should check if
                **    the object is an inside-out and prefers to be
                **    activated when visible type of object. if not the
                **    object should not be allowed to keep its window
                **    up after it gets UIDeactivated.
                */
                if (g_fInsideOutContainer && 
                    lpContainerLine->m_dwDrawAspect == DVASPECT_CONTENT) {
                        DWORD mstat;
                        OLEDBG_BEGIN2("IOleObject::GetMiscStatus called\r\n")
                        lpContainerLine->m_lpOleObj->lpVtbl->GetMiscStatus(
                                lpContainerLine->m_lpOleObj,
                                DVASPECT_CONTENT,
                                (DWORD FAR*)&mstat
                        );
                        OLEDBG_END2
        
                        lpContainerLine->m_fInsideOutObj = (BOOL)
                            (mstat & 
                             (OLEMISC_INSIDEOUT|OLEMISC_ACTIVATEWHENVISIBLE));
                }
#endif  // INPLACE_CNTR
                
            }
            break;

        case OLECREATEFROMDATA_STATIC:

            OLEDBG_BEGIN2("OleCreateStaticFromData called\r\n")
            hrErr = OleCreateStaticFromData (
                    lpSrcDataObj,
                    &IID_IOleObject,
                    dwOleRenderOpt,
                    lpRenderFmtEtc,
                    (LPOLECLIENTSITE)&lpContainerLine->m_OleClientSite,
                    lpContainerLine->m_lpStg,
                    (LPVOID FAR*)&lpContainerLine->m_lpOleObj
            );
            OLEDBG_END2

            if (hrErr != NOERROR) {
                OleDbgOutHResult("OleCreateStaticFromData returned", hrErr);
            }

		    lpContainerLine->m_fIsLink = FALSE;			
            break;
    }
	
    if (hrErr != NOERROR) {
        OutlineApp_ErrorMessage(g_lpApp, "Could not create object!");
        goto error;
    }

    /* Setup the Advises (OLE notifications) that we are interested 
    ** in receiving.
    */
    OleStdSetupAdvises(
            lpContainerLine->m_lpOleObj, 
            dwDrawAspect, 
            (LPSTR)APPNAME, 
            ((LPOUTLINEDOC)lpContainerDoc)->m_lpszDocTitle,
            (LPADVISESINK)&lpContainerLine->m_AdviseSink
    );

	if (fDisplayAsIcon) {
		BOOL fMustUpdate;	

		/* user has requested to display icon aspect instead of content 
		**    aspect. 
		**    NOTE: we do not have to delete the previous aspect cache
		**    because one did not get set up.
		*/
		OleStdSwitchDisplayAspect(
				lpContainerLine->m_lpOleObj, 
				&lpContainerLine->m_dwDrawAspect,
				dwDrawAspect, 
				hMetaPict,
				FALSE,	/* fDeleteOldAspect */
				TRUE,	/* fSetupViewAdvise */
				(LPADVISESINK)&lpContainerLine->m_AdviseSink,
				(BOOL FAR*)&fMustUpdate		// this can be ignored; update
											// for switch to icon not req'd
		);
	}

    OLEDBG_END3
    return lpContainerLine;
    
error:
    // Destroy partially created OLE object
    if (lpContainerLine) 
        ContainerLine_Delete(lpContainerLine);
    OLEDBG_END3
    return NULL;
}


/* ContainerLine_AddRef
** --------------------
**    
**  increment the ref count of the line object.
**    
**    Returns the new ref count on the object
*/
ULONG ContainerLine_AddRef(LPCONTAINERLINE lpContainerLine)
{
    ++lpContainerLine->m_cRef;

    OleDbgOutRefCnt4(
            "ContainerLine_AddRef: cRef++\r\n",
            lpContainerLine,
            lpContainerLine->m_cRef
    );

    return lpContainerLine->m_cRef;
}


/* ContainerLine_Release
** ---------------------
**    
**  decrement the ref count of the line object. 
**    if the ref count goes to 0, then the line is destroyed.
**    
**    Returns the remaining ref count on the object
*/
ULONG ContainerLine_Release(LPCONTAINERLINE lpContainerLine)
{
    ULONG cRef;
    
    OleDbgAssertSz(lpContainerLine->m_cRef > 0,"Release called with cRef == 0");

    /*********************************************************************
    ** OLE2NOTE: when the obj refcnt == 0, then destroy the object.     **
    **     otherwise the object is still in use.                        **
    *********************************************************************/

    cRef = --lpContainerLine->m_cRef;

    OleDbgOutRefCnt4(
            "ContainerLine_AddRef: cRef--\r\n", 
            lpContainerLine, 
            cRef
    );

    if (cRef == 0) 
        ContainerLine_Destroy(lpContainerLine);

    return cRef;
}


/* ContainerLine_QueryInterface
** ----------------------------
**
** Retrieve a pointer to an interface on the ContainerLine object.
**    
**    Returns NOERROR if interface is successfully retrieved.
**            E_NOINTERFACE if the interface is not supported
*/
HRESULT ContainerLine_QueryInterface(
        LPCONTAINERLINE         lpContainerLine, 
        REFIID                  riid, 
        LPVOID FAR*             lplpvObj
)
{
    SCODE sc = E_NOINTERFACE;

    /* OLE2NOTE: we must make sure to set all out ptr parameters to NULL. */
    *lplpvObj = NULL;

    if (IsEqualIID(riid, &IID_IUnknown)) {
        OleDbgOut4("ContainerLine_QueryInterface: IUnknown* RETURNED\r\n");

        *lplpvObj = (LPVOID) &lpContainerLine->m_Unknown;
        ContainerLine_AddRef(lpContainerLine);
        sc = S_OK;
    } 
    else if (IsEqualIID(riid, &IID_IOleClientSite)) {
        OleDbgOut4("ContainerLine_QueryInterface: IOleClientSite* RETURNED\r\n");

        *lplpvObj = (LPVOID) &lpContainerLine->m_OleClientSite;
        ContainerLine_AddRef(lpContainerLine);
        sc = S_OK;
    }
    else if (IsEqualIID(riid, &IID_IAdviseSink)) {
        OleDbgOut4("ContainerLine_QueryInterface: IAdviseSink* RETURNED\r\n");

        *lplpvObj = (LPVOID) &lpContainerLine->m_AdviseSink;
        ContainerLine_AddRef(lpContainerLine);
        sc = S_OK;
    }
#if defined( INPLACE_CNTR ) 
    else if (IsEqualIID(riid, &IID_IOleWindow) 
             || IsEqualIID(riid, &IID_IOleInPlaceSite)) {
        OleDbgOut4("ContainerLine_QueryInterface: IOleInPlaceSite* RETURNED\r\n");

        *lplpvObj = (LPVOID) &lpContainerLine->m_OleInPlaceSite;
        ContainerLine_AddRef(lpContainerLine);
        sc = S_OK;
    }   
#endif  // INPLACE_CNTR

    OleDbgQueryInterfaceMethod(*lplpvObj);

    return ResultFromScode(sc);
}
    

BOOL ContainerLine_LoadOleObject(LPCONTAINERLINE lpContainerLine)
{
    LPOUTLINEDOC lpOutlineDoc = (LPOUTLINEDOC)lpContainerLine->m_lpDoc;
    LPSTORAGE lpDocStg = ContainerDoc_GetStg(lpContainerLine->m_lpDoc);
    LPOLECLIENTSITE lpOleClientSite;
    LPMONIKER lpmkObj;
    HRESULT hrErr;

    if (lpContainerLine->m_lpOleObj) 
        return TRUE;                // object already loaded

    OLEDBG_BEGIN3("ContainerLine_LoadOleObject\r\n")

    /* if object storage is not already open, then open it */
    if (! lpContainerLine->m_lpStg) {
        lpContainerLine->m_lpStg = OleStdOpenChildStorage(
                lpDocStg, 
                lpContainerLine->m_szStgName,
                STGM_READWRITE
        );
        if (lpContainerLine->m_lpStg == NULL) {
            OleDbgAssertSz(
                lpContainerLine->m_lpStg != NULL, "Error opening child stg");
            OLEDBG_END3
            return FALSE;
        }
    }

    /* OLE2NOTE: if the OLE object being loaded is in a data transfer
    **    document, then we should NOT pass a IOleClientSite* pointer
    **    to the OleLoad call. This particularly critical if the OLE
    **    object is an OleLink object. If a non-NULL client site is
    **    passed to the OleLoad function, then the link will bind to
    **    the source if its is running. in the situation that we are
    **    loading the object as part of a data transfer document we do
    **    not want this connection to be established. even worse, if
    **    the link source is currently blocked or busy, then this could
    **    hang the system. it is simplest to never pass a
    **    IOleClientSite* when loading an object in a data transfer
    **    document. 
    */
    lpOleClientSite = (lpOutlineDoc->m_fDataTransferDoc ?
            NULL : (LPOLECLIENTSITE)&lpContainerLine->m_OleClientSite);

    OLEDBG_BEGIN2("OleLoad called\r\n")

    hrErr = OleLoad (
           lpContainerLine->m_lpStg,
           &IID_IOleObject,
           lpOleClientSite,
           (LPVOID FAR*)&lpContainerLine->m_lpOleObj
    );
         
    OLEDBG_END2

    if (! OleDbgVerifySz(hrErr == NOERROR, "Could not load object!")) {
        OleDbgOutHResult("OleLoad returned", hrErr);
        goto error;
    }

    // Cache a pointer to the IViewObject* interface. 
    //      we need this everytime we draw the object.
	lpContainerLine->m_lpViewObj = (LPVIEWOBJECT)OleStdQueryInterface(
			(LPUNKNOWN)lpContainerLine->m_lpOleObj, &IID_IViewObject);
	if (! lpContainerLine->m_lpViewObj) {
		OleDbgAssert(lpContainerLine->m_lpViewObj);
		goto error;
	}

    // Cache a pointer to the IPersistStorage* interface. 
    //      we need this everytime we save the object.
	lpContainerLine->m_lpPersistStg = (LPPERSISTSTORAGE)OleStdQueryInterface(
			(LPUNKNOWN)lpContainerLine->m_lpOleObj, &IID_IPersistStorage);
	if (! lpContainerLine->m_lpPersistStg) {
		OleDbgAssert(lpContainerLine->m_lpPersistStg);
		goto error;
	}

    /* OLE2NOTE: similarly, if the OLE object being loaded is in a data
    **    transfer document, then we do NOT need to setup any advises,
    **    call SetHostNames, SetMoniker, etc. 
    */
    if (lpOleClientSite) {
        /* Setup the Advises (OLE notifications) that we are interested 
        ** in receiving.
        */
        OleStdSetupAdvises(
                lpContainerLine->m_lpOleObj, 
                lpContainerLine->m_dwDrawAspect, 
                (LPSTR)APPNAME, 
                lpOutlineDoc->m_lpszDocTitle,
                (LPADVISESINK)&lpContainerLine->m_AdviseSink
        );

        /* OLE2NOTE: if the OLE object has a moniker assigned, we need to
        **    inform the object by calling IOleObject::SetMoniker. this
        **    will force the OLE object to register in the
        **    RunningObjectTable when it enters the running state.
        */
        if (lpContainerLine->m_fMonikerAssigned) {
            lpmkObj = ContainerLine_GetRelMoniker(
                    lpContainerLine, 
                    GETMONIKER_ONLYIFTHERE
            );

            if (lpmkObj) {
                OLEDBG_BEGIN2("IOleObject::SetMoniker called\r\n")
                lpContainerLine->m_lpOleObj->lpVtbl->SetMoniker(
                        lpContainerLine->m_lpOleObj,
                        OLEWHICHMK_OBJREL,
                        lpmkObj
                );
                OLEDBG_END2
                OleStdRelease((LPUNKNOWN)lpmkObj);
            }
        }

        /* get the Short form of the user type name of the object. this
        **    is used all the time when we have to build the object
        **    verb menu. we will cache this information to make it
        **    quicker to build the verb menu.
        */
        OLEDBG_BEGIN2("IOleObject::GetUserType called\r\n")
        lpContainerLine->m_lpOleObj->lpVtbl->GetUserType(
                lpContainerLine->m_lpOleObj,
                USERCLASSTYPE_SHORT,
                (LPSTR FAR*)&lpContainerLine->m_lpszShortType
        );
        OLEDBG_END2
            
        /* OLE2NOTE: if the object we just loaded is a link then it
        **    might have immediately bound to its link source if it was
        **    already running. when this happens the link will get an
        **    immediate update of data if the link is automatic. in
        **    this situation, it is possible that the extents of the
        **    linked object have changed.
        */
        if (lpContainerLine->m_fIsLink) {
            if (OleIsRunning(lpContainerLine->m_lpOleObj)) {
                ContainerLine_UpdateExtent(lpContainerLine, NULL);
            }
        } 
#if defined( INPLACE_CNTR )
        /* OLE2NOTE: an inside-out container should check if the object
        **    is an inside-out and prefers to be activated when visible
        **    type of object. if so, the object should be immediately
        **    activated in-place, BUT NOT UIActived. 
        */
        else if (g_fInsideOutContainer && 
                lpContainerLine->m_dwDrawAspect == DVASPECT_CONTENT) {
            DWORD mstat;
            OLEDBG_BEGIN2("IOleObject::GetMiscStatus called\r\n")
            lpContainerLine->m_lpOleObj->lpVtbl->GetMiscStatus(
                    lpContainerLine->m_lpOleObj,
                    DVASPECT_CONTENT,
                    (DWORD FAR*)&mstat
            );
            OLEDBG_END2
        
            lpContainerLine->m_fInsideOutObj = (BOOL)
                   (mstat & (OLEMISC_INSIDEOUT|OLEMISC_ACTIVATEWHENVISIBLE));

            if ( lpContainerLine->m_fInsideOutObj ) { 
                HWND hWndDoc = OutlineDoc_GetWindow(lpOutlineDoc);

                ContainerLine_DoVerb(
                        lpContainerLine,OLEIVERB_INPLACEACTIVATE,FALSE,FALSE);

                /* OLE2NOTE: following this DoVerb(INPLACEACTIVATE) the
                **    object may have taken focus. but because the
                **    object is NOT UIActive it should NOT have focus.
                **    we will make sure our document has focus.
                */
                SetFocus(hWndDoc);
            }
        }
#endif  // INPLACE_CNTR
        OLEDBG_END2
            
    }

    OLEDBG_END2
    return TRUE;

error:
    OLEDBG_END2
    return FALSE;
}   


/* ContainerLine_CloseOleObject
** ----------------------------
**    Close the OLE object associated with the ContainerLine.
**
**    Closing the object forces the object to transition from the
**    running state to the loaded state. if the object was not running,
**    then there is no effect. it is necessary to close the OLE object
**    before releasing the pointers to the OLE object.
**    
**    Returns TRUE if successfully closed, 
**            FALSE if closing was aborted.
*/
BOOL ContainerLine_CloseOleObject(LPCONTAINERLINE lpContainerLine)
{
    HRESULT hrErr;
    SCODE   sc;

    if (lpContainerLine->m_lpOleObj) {

        OLEDBG_BEGIN2("IOleObject::Close called\r\n")
        hrErr = lpContainerLine->m_lpOleObj->lpVtbl->Close(
                lpContainerLine->m_lpOleObj, 
                OLECLOSE_SAVEIFDIRTY
        );
        OLEDBG_END2

        if (hrErr != NOERROR) {
            OleDbgOutHResult("IOleObject::Close returned", hrErr);
            sc = GetScode(hrErr);
            if (sc == RPC_E_CALL_REJECTED || sc==OLE_E_PROMPTSAVECANCELLED) {
                return FALSE;   // object aborted shutdown
            }
        }
    }
    return TRUE;
}   
    

/* ContainerLine_UnloadOleObject
** -----------------------------
**    Close the OLE object associated with the ContainerLine and
**    release all pointer held to the object.
**
**    Closing the object forces the object to transition from the
**    running state to the loaded state. if the object was not running,
**    then there is no effect. it is necessary to close the OLE object
**    before releasing the pointers to the OLE object. releasing all
**    pointers to the object allows the object to transition from
**    loaded to unloaded (or passive).
*/
void ContainerLine_UnloadOleObject(LPCONTAINERLINE lpContainerLine)
{
    if (lpContainerLine->m_lpOleObj) {

        OLEDBG_BEGIN2("IOleObject::Close called\r\n")
        lpContainerLine->m_lpOleObj->lpVtbl->Close(
                lpContainerLine->m_lpOleObj, 
                OLECLOSE_SAVEIFDIRTY
        );
        OLEDBG_END2
		
		OleStdRelease((LPUNKNOWN)lpContainerLine->m_lpOleObj);
		lpContainerLine->m_lpOleObj = NULL;

        if (lpContainerLine->m_lpViewObj) {
            OleStdRelease((LPUNKNOWN)lpContainerLine->m_lpViewObj);
            lpContainerLine->m_lpViewObj = NULL;
        }
        if (lpContainerLine->m_lpPersistStg) {
            OleStdRelease((LPUNKNOWN)lpContainerLine->m_lpPersistStg);
            lpContainerLine->m_lpPersistStg = NULL;
        }
    }
}   
    

/* ContainerLine_Delete
** --------------------
**    Delete the ContainerLine.
**    
**    NOTE: we can NOT directly destroy the memory for the
**    ContainerLine; the ContainerLine maintains a reference count. a
**    non-zero reference count indicates that the object is still
**    in-use. The OleObject keeps a reference-counted pointer to the
**    ClientLine object. we must take the actions necessary so that the
**    ContainerLine object receives Releases for outstanding
**    references. when the reference count of the ContainerLine reaches
**    zero, then the memory for the object will actually be destroyed
**    (ContainerLine_Destroy called).
**    
*/
void ContainerLine_Delete(LPCONTAINERLINE lpContainerLine)
{
    OLEDBG_BEGIN2("ContainerLine_Delete\r\n")

#if defined( INPLACE_CNTR )
    if (lpContainerLine == lpContainerLine->m_lpDoc->m_lpLastIpActiveLine)
        lpContainerLine->m_lpDoc->m_lpLastIpActiveLine = NULL;
    if (lpContainerLine == lpContainerLine->m_lpDoc->m_lpLastUIActiveLine)
        lpContainerLine->m_lpDoc->m_lpLastUIActiveLine = NULL;
#endif      

    /* OLE2NOTE: in order to have a stable line object during the
    **    process of deleting, we intially AddRef the line ref cnt and 
    **    later Release it. This initial AddRef is artificial; it is 
    **    simply done to guarantee that our object does not destroy 
    **    itself until the END of this routine.
    */
    ContainerLine_AddRef(lpContainerLine);

    // Unload the loaded OLE object
    if (lpContainerLine->m_lpOleObj) 
        ContainerLine_UnloadOleObject(lpContainerLine);

    /* OLE2NOTE: we can NOT directly free the memory for the ContainerLine
    **    data structure until everyone holding on to a pointer to our
    **    ClientSite interface and IAdviseSink interface has released
    **    their pointers. There is one refcnt on the ContainerLine object
    **    which is held by the container itself. we will release this
	**    refcnt here.
    */
    ContainerLine_Release(lpContainerLine);

    ContainerLine_Release(lpContainerLine); // release artificial AddRef above
    OLEDBG_END2
}   


/* ContainerLine_Destroy
** ---------------------
**    Destroy (Free) the memory used by a ContainerLine structure. 
**    This function is called when the ref count of the ContainerLine goes
**    to zero. the ref cnt goes to zero after ContainerLine_Delete forces
**    the OleObject to unload and release its pointers to the
**    ContainerLine IOleClientSite and IAdviseSink interfaces. 
*/

void ContainerLine_Destroy(LPCONTAINERLINE lpContainerLine)
{
    LPUNKNOWN lpTmpObj;

    OLEDBG_BEGIN2("ContainerLine_Destroy\r\n")

    // Release the storage opened for the OLE object
    if (lpContainerLine->m_lpStg) {
        lpTmpObj = (LPUNKNOWN)lpContainerLine->m_lpStg;
        lpContainerLine->m_lpStg = NULL;

        OleStdVerifyRelease(lpTmpObj, "Object stg not released");
    }
    
    if (lpContainerLine->m_lpszShortType) 
        OleStdFreeString(lpContainerLine->m_lpszShortType, NULL);

    Delete(lpContainerLine);        // Free the memory for the structure itself
    OLEDBG_END2
}


/* ContainerLine_CopyToDoc
 * -----------------------
 *
 *      Copy a ContainerLine to another Document (usually ClipboardDoc)
 */
BOOL ContainerLine_CopyToDoc(
        LPCONTAINERLINE         lpSrcLine, 
        LPOUTLINEDOC            lpDestDoc, 
        int                     nIndex
)
{
    LPCONTAINERLINE lpDestLine = NULL;
    LPLINELIST  lpDestLL = &lpDestDoc->m_LineList;
    HDC         hDC;
    HRESULT     hrErr;
    BOOL        fStatus;
    LPSTORAGE   lpDestDocStg = ((LPCONTAINERDOC)lpDestDoc)->m_lpStg;
    LPSTORAGE   lpDestObjStg = NULL;
   
    lpDestLine = (LPCONTAINERLINE) New((DWORD)sizeof(CONTAINERLINE));
    if (lpDestLine == NULL) {
        OleDbgAssertSz(
                lpDestLine!=NULL, "Error allocating ContainerLine");
        return FALSE; 
    }
    
    hDC = LineList_GetDC(lpDestLL); 
    ContainerLine_Init(lpDestLine, ((LPLINE)lpSrcLine)->m_nTabLevel, hDC);
    LineList_ReleaseDC(lpDestLL, hDC);

    /* OLE2NOTE: In order to have a stable ContainerLine object we must
	**    AddRef the object's refcnt. this will be later released when
	**    the ContainerLine is deleted.
    */
    ContainerLine_AddRef(lpDestLine);

    lpDestLine->m_lpDoc = (LPCONTAINERDOC)lpDestDoc;

    // Copy data of the original source ContainerLine.
    ((LPLINE)lpDestLine)->m_nWidthInHimetric = 
            ((LPLINE)lpSrcLine)->m_nWidthInHimetric;
    ((LPLINE)lpDestLine)->m_nHeightInHimetric = 
            ((LPLINE)lpSrcLine)->m_nHeightInHimetric;
    lpDestLine->m_fMonikerAssigned = lpSrcLine->m_fMonikerAssigned;
    lpDestLine->m_dwDrawAspect = lpSrcLine->m_dwDrawAspect;
    lpDestLine->m_sizeInHimetric = lpSrcLine->m_sizeInHimetric;
    lpDestLine->m_fIsLink = lpSrcLine->m_fIsLink;
	
    
    /* We must create a new sub-storage for the embedded object within
    **    the destination document's storage. We will first attempt to
    **    use the same storage name as the source line. if this name is
    **    in use, then we will allocate a new name. in this way we try
    **    to keep the name associated with the OLE object unchanged
    **    through a Cut/Paste operation.
    */
    lpDestObjStg = OleStdCreateChildStorage(
            lpDestDocStg, 
            lpSrcLine->m_szStgName
    );
    if (lpDestObjStg) {
        lstrcpy(lpDestLine->m_szStgName, lpSrcLine->m_szStgName);
    } else {
        /* the original name was in use, make up a new name. */
        ContainerDoc_GetNextStgName(
                (LPCONTAINERDOC)lpDestDoc, 
                lpDestLine->m_szStgName, 
                sizeof(lpDestLine->m_szStgName)
        );
        lpDestObjStg = OleStdCreateChildStorage(
                lpDestDocStg, 
                lpDestLine->m_szStgName
        );
    }
    if (! OleDbgVerifySz(lpDestObjStg != NULL, "Error creating child stg"))
        goto error;

    // Copy over storage of the embedded object itself

    if (! lpSrcLine->m_lpOleObj) {

        /*****************************************************************
        ** CASE 1: object is NOT loaded.
        **    because the object is not loaded, we can simply copy the
        **    object's current storage to the new storage. 
        *****************************************************************/

        /* if current object storage is not already open, then open it */
        if (! lpSrcLine->m_lpStg) {
            LPSTORAGE lpSrcDocStg = lpSrcLine->m_lpDoc->m_lpStg;

            if (! lpSrcDocStg) goto error;

            // open object storage.
            lpSrcLine->m_lpStg = OleStdOpenChildStorage(
                    lpSrcDocStg,    
                    lpSrcLine->m_szStgName,
                    STGM_READWRITE
            );
            if (! OleDbgVerifySz(lpSrcLine->m_lpStg != NULL,
                                                "Error opening child stg"))
                goto error;
        }

        hrErr = lpSrcLine->m_lpStg->lpVtbl->CopyTo(
                lpSrcLine->m_lpStg, 
                0,
                NULL,
                NULL,
                lpDestObjStg
        );
        if (hrErr != NOERROR) {
            OleDbgOutHResult("WARNING: lpSrcObjStg->CopyTo returned", hrErr);
            goto error;
        }

        fStatus = OleStdCommitStorage(lpDestObjStg); 
            
    } else {

        /*****************************************************************
        ** CASE 2: object IS loaded.
        **    we must tell the object to save into the new storage.
        *****************************************************************/
        
        hrErr = ContainerLine_SaveOleObject(
				lpSrcLine, 
				lpDestObjStg, 
				FALSE,	/* fSameAsLoad */
				FALSE,	/* fRemember */
                TRUE    /* fForceUpdate */
		);

        if (hrErr != NOERROR) 
            goto error;
    }
    
    OutlineDoc_AddLine(lpDestDoc, (LPLINE)lpDestLine, nIndex);
    OleStdVerifyRelease(
            (LPUNKNOWN)lpDestObjStg,
            "Copied object stg not released"
    );

    return TRUE;

error:

    // Delete any partially created storage.
    if (lpDestObjStg) {
        OleStdVerifyRelease(
                (LPUNKNOWN)lpDestObjStg,
                "Copied object stg not released"
        );

        lpDestDocStg->lpVtbl->DestroyElement(
                lpDestDocStg, 
                lpDestLine->m_szStgName
        );
        lpDestLine->m_szStgName[0] = '\0';
    }
	
    // destroy partially created ContainerLine
    if (lpDestLine) 
        ContainerLine_Delete(lpDestLine);       
    return FALSE;
}


/* ContainerLine_UpdateExtent
** --------------------------
**   Update the size of the ContainerLine because the extents of the
**    object may have changed.
**    
**    NOTE: because we are using a Windows OwnerDraw ListBox, we must
**    constrain the maximum possible height of a line. the ListBox has
**    a limitation (unfortunately) that no line can become larger than
**    255 pixels. thus we force the object to scale maintaining its
**    aspect ratio if this maximum line height limit is reached. the
**    actual maximum size for an object at 100% Zoom is
**    255-2*LINE_BOUNDARY_WIDTH.
**    
**    RETURNS TRUE -- if the extents of the object changed
**			  FALSE -- if the extents did NOT change
*/
BOOL ContainerLine_UpdateExtent(
		LPCONTAINERLINE		lpContainerLine, 
		LPSIZEL				lpsizelHim
)
{
    LPOUTLINEDOC lpOutlineDoc = (LPOUTLINEDOC)lpContainerLine->m_lpDoc;
    LPLINELIST lpLL = OutlineDoc_GetLineList(lpOutlineDoc);
    LPLINE lpLine = (LPLINE)lpContainerLine;
    int nIndex = LineList_GetLineIndex(lpLL, lpLine);
    UINT nOrgWidthInHimetric = lpLine->m_nWidthInHimetric;
    UINT nOrgHeightInHimetric = lpLine->m_nHeightInHimetric;
    BOOL fWidthChanged = FALSE;
    BOOL fHeightChanged = FALSE;
	SIZEL sizelHim;
	HRESULT hrErr;
	
    if (!lpContainerLine || !lpContainerLine->m_lpOleObj)
        return FALSE;

    OLEDBG_BEGIN3("ContainerLine_UpdateExtent\r\n");

	lpContainerLine->m_fDoGetExtent = FALSE;

	if (! lpsizelHim) {
		OLEDBG_BEGIN2("IOleObject::GetExtent called\r\n")
		hrErr = lpContainerLine->m_lpOleObj->lpVtbl->GetExtent(
				lpContainerLine->m_lpOleObj,
				lpContainerLine->m_dwDrawAspect,
				(LPSIZEL)&sizelHim
		);
		OLEDBG_END2
		if (hrErr != NOERROR)
			sizelHim.cx = sizelHim.cy = 0;

		lpsizelHim = (LPSIZEL)&sizelHim;
	}

	if (lpsizelHim->cx == lpContainerLine->m_sizeInHimetric.cx &&
		lpsizelHim->cy == lpContainerLine->m_sizeInHimetric.cy) {
		goto noupdate;
	}

    if (lpsizelHim->cx > 0 || lpsizelHim->cy > 0) {
        lpContainerLine->m_sizeInHimetric = *lpsizelHim;
    } else {
        /* object does not have any extents; let's use our container
        **    chosen arbitrary size for OLE objects. 
        */
        lpContainerLine->m_sizeInHimetric.cx = (long)DEFOBJWIDTH;
        lpContainerLine->m_sizeInHimetric.cy = (long)DEFOBJHEIGHT;
    }

	ContainerLine_CalcExtents(
			lpContainerLine,
			(LPSIZEL)&lpContainerLine->m_sizeInHimetric);
	
    // if height of object changed, then reset the height of line in LineList
    if (nOrgHeightInHimetric != lpLine->m_nHeightInHimetric) {
        LineList_SetLineHeight(lpLL, nIndex, lpLine->m_nHeightInHimetric);
        fHeightChanged = TRUE;
    }
	
    fWidthChanged = LineList_RecalcMaxLineWidthInHimetric(
			lpLL, 
			nOrgWidthInHimetric
	);
	fWidthChanged |= (nOrgWidthInHimetric != lpLine->m_nWidthInHimetric);

    if (fHeightChanged || fWidthChanged) {
        OutlineDoc_ForceRedraw(lpOutlineDoc, TRUE);

        // mark ContainerDoc as now dirty
        OutlineDoc_SetModified(lpOutlineDoc, TRUE, TRUE, TRUE);
    }

    OLEDBG_END3
	return TRUE;

noupdate:
    OLEDBG_END3
	return FALSE;	// No UPDATE necessary
}


/* ContainerLine_DoVerb
** --------------------
**    Activate the OLE object and perform a specific verb.
*/
BOOL ContainerLine_DoVerb(LPCONTAINERLINE lpContainerLine, LONG iVerb, BOOL fMessage, BOOL fAction)
{
    HRESULT hrErr;
    LPOUTLINEDOC lpOutlineDoc = (LPOUTLINEDOC)lpContainerLine->m_lpDoc;
	RECT rcPosRect;

    OLEDBG_BEGIN3("ContainerLine_DoVerb\r\n")
    /* if object is not already loaded, then load it now. objects are
    **    loaded lazily in this manner.
    */
    if (! lpContainerLine->m_lpOleObj) 
        ContainerLine_LoadOleObject(lpContainerLine);

    if (! lpContainerLine->m_lpOleObj) {
        OleDbgAssertSz(lpContainerLine->m_lpOleObj != NULL, 
                                                    "OLE object not loaded"); 
        goto error;
    }
	
#if defined( INPLACE_CNTR )     
    /* OLE2NOTE: we want to keep only 1 inplace server active at any
    **    given time. so when we start to do a DoVerb on another line,
    **    then we want to shut down the previously activated server. in
    **    this way we keep at most one inplace server active at a time.
    */
    if (!g_fInsideOutContainer) {
        ContainerDoc_ShutDownLastInPlaceServerIfNotNeeded(
                lpContainerLine->m_lpDoc, lpContainerLine);
    }
#endif  // INPLACE_CNTR

	ContainerLine_GetOleObjectRectInPixels(
			lpContainerLine,
			(LPRECT)&rcPosRect
	);

	// run the object
	hrErr = ContainerLine_RunOleObject(lpContainerLine);
    if (hrErr != NOERROR) 
        goto error;

    /* Tell object server to perform a "verb". */
    OLEDBG_BEGIN2("IOleObject::DoVerb called\r\n")
    hrErr = lpContainerLine->m_lpOleObj->lpVtbl->DoVerb (
            lpContainerLine->m_lpOleObj,
            iVerb,
            NULL,
            (LPOLECLIENTSITE)&lpContainerLine->m_OleClientSite,
            -1, 
			OutlineDoc_GetWindow(lpOutlineDoc),
			(LPCRECT)&rcPosRect
    );
    OLEDBG_END2

    /* OLE2NOTE: IOleObject::DoVerb may return a success code
    **    OLE_S_INVALIDVERB. this SCODE should NOT be considered an
    **    error; thus it is important to use the "FAILED" macro to
    **    check for an error SCODE.
    */
    if (FAILED(GetScode(hrErr))) {
        OleDbgOutHResult("WARNING: lpOleObj->DoVerb returned", hrErr);
        goto error;
    }

    OLEDBG_END3
    return TRUE;

error:
	if (lpContainerLine->m_fIsLink)
		lpContainerLine->m_fLinkUnavailable = TRUE;

    /* OLE2NOTE: if an error occurs we must give the appropriate error
    **    message box. there are many potential errors that can occur.
    **    the OLE2.0 user model has specific guidelines as to the
    **    dialogs that should be displayed given the various potential
    **    errors (eg. server not registered, unavailable link source.
    **    the OLE2UI library includes support for most of the
    **    recommended message dialogs. (see OleUIPrompUser function)
    */
	if (fMessage) {
		ProcessError(hrErr, lpContainerLine, fAction);
	}
	
	OLEDBG_END3
	return FALSE;
}


/* ContainerLine_GetOleObject
** --------------------------
**    return pointer to desired interface of embedded/linked object.
**
**    NOTE: this function causes an AddRef to the object. when the caller is 
**          finished with the object, it must call Release.
**          this function does not AddRef the ContainerLine object.
*/
LPUNKNOWN ContainerLine_GetOleObject(
        LPCONTAINERLINE         lpContainerLine, 
        REFIID                  riid
)
{
    /* if object is not already loaded, then load it now. objects are
    **    loaded lazily in this manner.
    */
    if (! lpContainerLine->m_lpOleObj) 
        ContainerLine_LoadOleObject(lpContainerLine);

    if (lpContainerLine->m_lpOleObj) 
        return OleStdQueryInterface(
                (LPUNKNOWN)lpContainerLine->m_lpOleObj, 
                riid
        );
    else 
        return NULL;
}



/* ContainerLine_RunOleObject
** --------------------------
**    Load and run the object. Upon running and if size of object has changed,
**	  use SetExtent to change to new size.
**
*/
HRESULT ContainerLine_RunOleObject(LPCONTAINERLINE lpContainerLine)
{
	LPUNKNOWN lpUnk;
	LPLINE lpLine = (LPLINE)lpContainerLine;
	SIZEL	sizelNew;
	HRESULT hrErr;
    HCURSOR  hPrevCursor;	

	if (! lpContainerLine)
		return NOERROR;

	if (lpContainerLine->m_lpOleObj && 
		OleIsRunning(lpContainerLine->m_lpOleObj))
		return NOERROR;		// object already running 

    // this may take a while, put up hourglass cursor
    hPrevCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));

	OLEDBG_BEGIN3("ContainerLine_RunOleObject\r\n")

	lpUnk = ContainerLine_GetOleObject(lpContainerLine, &IID_IUnknown);
	
	OLEDBG_BEGIN2("OleRun called\r\n")	
	hrErr = OleRun(lpUnk);		// FORCE object to RUN
	OLEDBG_END2

	OleStdRelease(lpUnk);

    SetCursor(hPrevCursor);     // restore original cursor

	if (hrErr != NOERROR) {
		OleDbgOutHResult("OleRun returned", hrErr);
		OLEDBG_END3
		return hrErr;
	}

	sizelNew.cx = lpLine->m_nWidthInHimetric - XformWidthInPixelsToHimetric(
			NULL, LINE_BOUNDARY_WIDTH * 2);
	sizelNew.cy = lpLine->m_nHeightInHimetric - XformHeightInPixelsToHimetric(
			NULL, LINE_BOUNDARY_WIDTH * 2);

	if ((sizelNew.cx != lpContainerLine->m_sizeInHimetric.cx) || 
		(sizelNew.cy != lpContainerLine->m_sizeInHimetric.cy)) {
		
		OLEDBG_BEGIN2("IOleObject::SetExtent called\r\n")	

		lpContainerLine->m_lpOleObj->lpVtbl->SetExtent(
				lpContainerLine->m_lpOleObj,
				lpContainerLine->m_dwDrawAspect,
				(LPSIZEL)&sizelNew);
				
		OLEDBG_END2
			
	}

	OLEDBG_END3
	return NOERROR;

}


/* ContainerLine_IsOleLink
** -----------------------
**    
**    return TRUE if the ContainerLine has an OleLink.
**           FALSE if the ContainerLine has an embedding
*/
BOOL ContainerLine_IsOleLink(LPCONTAINERLINE lpContainerLine)
{
	if (!lpContainerLine)
		return FALSE;

	return lpContainerLine->m_fIsLink;
}


/*  ContainerLine_Draw
**  ------------------
**
**		Draw a ContainerLine object on a DC.
**      
**	Parameters:
**		hDC		- DC to which the line will be drawn
**		lpRect	- the object rect in logical coordinates
*/
void ContainerLine_Draw(
        LPCONTAINERLINE			lpContainerLine,
        HDC						hDC,
        LPRECT					lpRect
)
{
    LPLINE	lpLine = (LPLINE) lpContainerLine;
	HRESULT hrErr = NOERROR;
	RECTL	rclHim;
	RECT	rcHim;

    /* if object is not already loaded, then load it now. objects are
    **    loaded lazily in this manner.
    */
    if (! lpContainerLine->m_lpOleObj) 
        ContainerLine_LoadOleObject(lpContainerLine);

	if (! lpContainerLine->m_lpViewObj) {
		lpContainerLine->m_lpViewObj = (LPVIEWOBJECT)OleStdQueryInterface(
				(LPUNKNOWN)lpContainerLine->m_lpOleObj, &IID_IViewObject);
		if (! lpContainerLine->m_lpViewObj) {
			OleDbgAssert(lpContainerLine->m_lpViewObj);
			return;		// Error: no IViewObject* available
		}
	}

    /* construct bounds rectangle for the object.
    **  offset origin for object to correct tab indentation
    */
    rclHim.left		= (long) lpRect->left;
    rclHim.bottom	= (long) lpRect->bottom;
    rclHim.top		= (long) lpRect->top;
    rclHim.right	= (long) lpRect->right;

	rclHim.left += (long) ((LPLINE)lpContainerLine)->m_nTabWidthInHimetric;
	rclHim.right += (long) ((LPLINE)lpContainerLine)->m_nTabWidthInHimetric;

#if defined( INPLACE_CNTR )
    /* OLE2NOTE: if the OLE object is currently does has a visible in-place
    **    window, then we do NOT want to draw on top of its window.
    **    this could interfere with the object's display.
    */
    if ( !lpContainerLine->m_fIpVisible )
#endif 
{
    hrErr = lpContainerLine->m_lpViewObj->lpVtbl->Draw(
            lpContainerLine->m_lpViewObj, 
            lpContainerLine->m_dwDrawAspect,
			-1,
			NULL,
			NULL,
			NULL,
			hDC,
			(LPRECTL)&rclHim,
			(LPRECTL)&rclHim,
			NULL,
			0
	);
	if (hrErr != NOERROR) 
		OleDbgOutHResult("IViewObject::Draw returned", hrErr);

    if (lpContainerLine->m_fObjWinOpen) {

		rcHim.left		= (int) rclHim.left;
		rcHim.top		= (int) rclHim.top;
		rcHim.right		= (int) rclHim.right;
		rcHim.bottom	= (int) rclHim.bottom;

        /* OLE2NOTE: if the object servers window is Open (ie. not active
        **    in-place) then we must shade the object in our document to
        **    indicate to the user that the object is open elsewhere.
        */
        OleUIDrawShading((LPRECT)&rcHim, hDC, OLEUI_SHADE_FULLRECT, 0);
    } 
}
    return;
}


void ContainerLine_DrawSelHilight(
        LPCONTAINERLINE lpContainerLine,
        HDC             hDC,			// MM_TEXT mode
        LPRECT          lprcPix,		// listbox rect
        UINT            itemAction,
        UINT            itemState
)
{
    LPLINE  lpLine = (LPLINE)lpContainerLine;
    RECT    rcObj;
	DWORD	dwFlags = OLEUI_HANDLES_INSIDE | OLEUI_HANDLES_USEINVERSE;
	int		nHandleSize;
	LPCONTAINERDOC lpContainerDoc;

	if (!lpContainerLine || !hDC || !lprcPix)
		return;

	lpContainerDoc = lpContainerLine->m_lpDoc;
	
    // Get size of OLE object
    ContainerLine_GetOleObjectRectInPixels(lpContainerLine, (LPRECT)&rcObj);
	
	InflateRect(&rcObj, 1, 1);		// get feedback rect
	nHandleSize = GetProfileInt("windows", "oleinplaceborderwidth", 
			DEFAULT_HATCHBORDER_WIDTH) + 1;
			
    if (itemAction & ODA_SELECT) {
        // check if there is a selection state change
        if (itemState & ODS_SELECTED) {
            if (!lpLine->m_fSelected) {
                lpLine->m_fSelected = TRUE;
                OleUIDrawHandles((LPRECT)&rcObj, hDC, dwFlags, nHandleSize, 
						TRUE);
				InvertDiffRect(lprcPix, (LPRECT)&rcObj, hDC);
            }
        } else {
            if (lpLine->m_fSelected) {
                lpLine->m_fSelected = FALSE;
                OleUIDrawHandles((LPRECT)&rcObj, hDC, dwFlags, nHandleSize, 
						FALSE);
				InvertDiffRect(lprcPix, (LPRECT)&rcObj, hDC);
            }
        }
    } else if (itemAction & ODA_DRAWENTIRE) {
        lpLine->m_fSelected=((itemState & ODS_SELECTED) ? TRUE : FALSE);
        if (lpLine->m_fSelected) {
            OleUIDrawHandles((LPRECT)&rcObj, hDC, dwFlags, nHandleSize, TRUE);
			InvertDiffRect(lprcPix, (LPRECT)&rcObj, hDC);
        } else {
            OleUIDrawHandles((LPRECT)&rcObj, hDC, dwFlags, nHandleSize, 
					FALSE);        
			InvertDiffRect(lprcPix, (LPRECT)&rcObj, hDC);
        }
        
	}
}
	
/* InvertDiffRect
** --------------
**    
**    Paint the surrounding of the Obj rect black but within lprcPix 
**		(similar to the lprcPix minus lprcObj)
*/
static void InvertDiffRect(LPRECT lprcPix, LPRECT lprcObj, HDC hDC)
{
	RECT rcBlack;
	
    // draw black in all space outside of object's rectangle
	rcBlack.top = lprcPix->top;
	rcBlack.bottom = lprcPix->bottom;

	rcBlack.left = lprcPix->left + 1;
	rcBlack.right = lprcObj->left - 1;
	InvertRect(hDC, (LPRECT)&rcBlack);
	
	rcBlack.left = lprcObj->right + 1;
	rcBlack.right = lprcPix->right - 1;
	InvertRect(hDC, (LPRECT)&rcBlack);

	rcBlack.top = lprcPix->top;
	rcBlack.bottom = lprcPix->top + 1;
	rcBlack.left = lprcObj->left - 1;
	rcBlack.right = lprcObj->right + 1;
	InvertRect(hDC, (LPRECT)&rcBlack);
	
	rcBlack.top = lprcPix->bottom;
	rcBlack.bottom = lprcPix->bottom - 1;
	rcBlack.left = lprcObj->left - 1;
	rcBlack.right = lprcObj->right + 1;
	InvertRect(hDC, (LPRECT)&rcBlack);
}


/* Edit the ContainerLine line object. 
**      returns TRUE if line was changed
**              FALSE if the line was NOT changed
*/
BOOL ContainerLine_Edit(LPCONTAINERLINE lpContainerLine, HWND hWndDoc,HDC hDC)
{
    ContainerLine_DoVerb(lpContainerLine, OLEIVERB_PRIMARY, TRUE, TRUE);
    
    /* assume object was NOT changed, if it was obj will send Changed
    **    or Saved notification.
    */
    return FALSE;
}



/* ContainerLine_SetHeightInHimetric
** ---------------------------------
**
** Set the height of a ContainerLine object. The widht will be changed
** to keep the aspect ratio
*/
void ContainerLine_SetHeightInHimetric(LPCONTAINERLINE lpContainerLine, int nHeight)
{
	LPLINE	lpLine = (LPLINE)lpContainerLine;
	SIZEL	sizelOleObject;
	HRESULT hrErr;
	
	if (!lpContainerLine)
		return;

	if (nHeight != -1) {

		/* if object is not already loaded, then load it now. objects are
		**    loaded lazily in this manner.
		*/
		if (! lpContainerLine->m_lpOleObj) 
			ContainerLine_LoadOleObject(lpContainerLine);

		sizelOleObject.cy = nHeight - XformHeightInPixelsToHimetric(NULL, 
				LINE_BOUNDARY_WIDTH * 2);

		sizelOleObject.cx = (int)(sizelOleObject.cy * 
				lpContainerLine->m_sizeInHimetric.cx /
				lpContainerLine->m_sizeInHimetric.cy);

		hrErr = lpContainerLine->m_lpOleObj->lpVtbl->SetExtent(
				lpContainerLine->m_lpOleObj,
				lpContainerLine->m_dwDrawAspect,
				(LPSIZEL)&sizelOleObject);

		OleDbgOutHResult("IOleObj::SetExtent returned",hrErr);
		
		if (hrErr == NOERROR) {
			/*****************************************************************
			**  OLE Object accepts size changes, update the extents in the  **
			**    ContainerLine and Line									**
			*****************************************************************/
			ContainerLine_UpdateExtent(lpContainerLine, 
					(LPSIZEL)&sizelOleObject);
		}
		else {
			/*****************************************************************
			** OLE Object refuses size changes, change extents of Line only **
			*****************************************************************/
			ContainerLine_CalcExtents(lpContainerLine, 
					(LPSIZEL)&sizelOleObject);
		}
	}
	else {
		/*****************************************************************
		** Use default OLE Object size									**
		*****************************************************************/
		ContainerLine_CalcExtents(
				lpContainerLine,
				(LPSIZEL)&lpContainerLine->m_sizeInHimetric);
	}

}


/*	ContainerLine_CalcExtents
 *
 *	Purpose:
 *		Calculate the corresponding line height from the OleObject size
 *		Scale the line height to fit the limit if necessary
 *
 *	Parameters:
 *		lpsizelOleObject		pointer to size of OLE Object
 *
 *	Returns:
 *		nil
 */
void ContainerLine_CalcExtents(LPCONTAINERLINE lpContainerLine, LPSIZEL lpsizelOleObject)
{
	LPLINE lpLine = (LPLINE)lpContainerLine;
	
	UINT uMaxObjectHeight = XformHeightInPixelsToHimetric(NULL, 
			LISTBOX_HEIGHT_LIMIT - 2 * LINE_BOUNDARY_WIDTH);
	
	if (!lpContainerLine || !lpsizelOleObject)
		return;

	lpLine->m_nWidthInHimetric = (int)lpsizelOleObject->cx;
	lpLine->m_nHeightInHimetric = (int)lpsizelOleObject->cy;

	// Rescale the object if height is greater than the limit
	if (lpLine->m_nHeightInHimetric > (UINT)uMaxObjectHeight) {

		lpLine->m_nWidthInHimetric = (UINT)
				((long)lpLine->m_nWidthInHimetric * 
				(long)uMaxObjectHeight / 
				(long)lpLine->m_nHeightInHimetric);
							
		lpLine->m_nHeightInHimetric = uMaxObjectHeight;
	}
	
	// Add boundary space
    lpLine->m_nWidthInHimetric += 
			XformWidthInPixelsToHimetric(NULL, LINE_BOUNDARY_WIDTH) * 2;
    lpLine->m_nHeightInHimetric += 
			XformHeightInPixelsToHimetric(NULL, LINE_BOUNDARY_WIDTH) * 2;
}	


/* ContainerLine_SaveToStg
** -----------------------
**    Save a given ContainerLine and associated OLE object to an IStorage*.
*/
BOOL ContainerLine_SaveToStg(
        LPCONTAINERLINE         lpContainerLine, 
        UINT                    uFormat, 
        LPSTORAGE               lpSrcStg, 
        LPSTORAGE               lpDestStg, 
        LPSTREAM                lpLLStm, 
        BOOL                    fRemember
)
{
    LPCONTAINERAPP lpContainerApp = (LPCONTAINERAPP)g_lpApp;
    HRESULT hrErr;
    BOOL fStatus;
    ULONG nWritten;
    BOOL fSameAsLoad = (lpSrcStg==lpDestStg ? TRUE : FALSE);
    CONTAINERLINERECORD objLineRecord;
    LPSTORAGE lpObjDestStg;
    LARGE_INTEGER dlibSavePos;
    LARGE_INTEGER dlibZeroOffset;
    LISet32( dlibZeroOffset, 0 );

    // only save the ContainerLine (with OLE object) if format is compatible.
    if (uFormat != lpContainerApp->m_cfCntrOutl) 
        return FALSE;

    /* save seek position before line record is written in case of error */
    hrErr = lpLLStm->lpVtbl->Seek(
            lpLLStm,
            dlibZeroOffset,
            STREAM_SEEK_CUR,
            (ULARGE_INTEGER FAR*)&dlibSavePos
    );
    if (hrErr != NOERROR) goto error;

    lstrcpy(objLineRecord.m_szStgName, lpContainerLine->m_szStgName);
    objLineRecord.m_fMonikerAssigned = lpContainerLine->m_fMonikerAssigned;
    objLineRecord.m_dwDrawAspect = lpContainerLine->m_dwDrawAspect;
    objLineRecord.m_sizeInHimetric = lpContainerLine->m_sizeInHimetric;
    objLineRecord.m_fIsLink = lpContainerLine->m_fIsLink;

    /* write line record */
    hrErr = lpLLStm->lpVtbl->Write(
            lpLLStm,
            (LPVOID)&objLineRecord,
            sizeof(CONTAINERLINERECORD),
            &nWritten
    );

    if (! OleDbgVerifySz(hrErr == NOERROR, 
                                        "Could not write to LineList stream"))
        goto error;

    if (! lpContainerLine->m_lpOleObj) {

        /*****************************************************************
        ** CASE 1: object is NOT loaded.
        *****************************************************************/
        
        if (fSameAsLoad) {
            /*************************************************************
            ** CASE 1A: we are saving to the current storage. because
            **    the object is not loaded, it is up-to-date 
            **    (ie. nothing to do). 
            *************************************************************/

            ;

        } else {
            /*************************************************************
            ** CASE 1B: we are saving to a new storage. because
            **    the object is not loaded, we can simply copy the
            **    object's current storage to the new storage.
            *************************************************************/

            /* if current object storage is not already open, then open it */
            if (! lpContainerLine->m_lpStg) {
                lpContainerLine->m_lpStg = OleStdOpenChildStorage(
                        lpSrcStg, 
                        lpContainerLine->m_szStgName,
                        STGM_READWRITE
                    );
                if (! OleDbgVerifySz(lpContainerLine->m_lpStg != NULL,
                                                "Error opening child stg"))
                    goto error;
            }
            
            /* Create a child storage inside the destination storage. */
            lpObjDestStg = OleStdCreateChildStorage(
                    lpDestStg, 
                    lpContainerLine->m_szStgName
            );

            if (! OleDbgVerifySz(lpObjDestStg != NULL,
                                            "Could not create obj storage!"))
                goto error;

            hrErr = lpContainerLine->m_lpStg->lpVtbl->CopyTo(
                    lpContainerLine->m_lpStg,
                    0,
                    NULL,
                    NULL,
                    lpObjDestStg
            );
            // REVIEW: should we handle error here?
            fStatus = OleStdCommitStorage(lpObjDestStg); 
            
            /* if we are supposed to remember this storage as the new
            **    storage for the object, then release the old one and
            **    save the new one. else, throw away the new one.
            */
            if (fRemember) {
                OleStdVerifyRelease(
                        (LPUNKNOWN)lpContainerLine->m_lpStg, 
                        "Original object stg not released"
                );
                lpContainerLine->m_lpStg = lpObjDestStg;
            } else {
                OleStdVerifyRelease(
                        (LPUNKNOWN)lpObjDestStg, 
                        "Copied object stg not released"
                );
            }
        }

    } else {

        /*****************************************************************
        ** CASE 2: object IS loaded.
        *****************************************************************/

        if (fSameAsLoad) {
            /*************************************************************
            ** CASE 2A: we are saving to the current storage. if the object
            **    is not dirty, then the current storage is up-to-date 
            **    (ie. nothing to do). 
            *************************************************************/

            LPPERSISTSTORAGE lpPersistStg;

            if (! lpContainerLine->m_lpPersistStg) {
                lpContainerLine->m_lpPersistStg = 
                    (LPPERSISTSTORAGE)OleStdQueryInterface(
                        (LPUNKNOWN)lpContainerLine->m_lpOleObj, 
                        &IID_IPersistStorage);
            }

            lpPersistStg = lpContainerLine->m_lpPersistStg;
			OleDbgAssertSz(
                    lpPersistStg!=NULL,"IPersistStorage NOT supported");

		    if (! lpPersistStg)
				goto error;

			hrErr = lpPersistStg->lpVtbl->IsDirty(lpPersistStg);

            if (hrErr != NOERROR) {
				return TRUE;	/* OLE object is NOT dirty */
            } else {
                /* OLE object IS dirty */
                hrErr = ContainerLine_SaveOleObject(
                        lpContainerLine, 
                        lpContainerLine->m_lpStg, 
                        fSameAsLoad, 
                        fRemember,
                        TRUE    /* fForceUpdate */
                );

                return ((hrErr == NOERROR) ? TRUE : FALSE);
            }

        } else {
            /*************************************************************
            ** CASE 2B: we are saving to a new storage. we must
            **    tell the object to save into the new storage.
            *************************************************************/

            /* Create a child storage inside the destination storage. */
            lpObjDestStg = OleStdCreateChildStorage(
                    lpDestStg, 
                    lpContainerLine->m_szStgName
            );

            if (! OleDbgVerifySz(lpObjDestStg != NULL, 
                                        "Could not create object storage!")) {
                goto error;
            }

            hrErr = ContainerLine_SaveOleObject(
                    lpContainerLine, 
                    lpObjDestStg, 
                    fSameAsLoad, 
                    fRemember,
                    TRUE    /* fForceUpdate */
            );

            if (hrErr != NOERROR) 
                goto error;

            /* if we are supposed to remember this storage as the new
            **    storage for the object, then release the old one and
            **    save the new one. else, throw away the new one.
            */
            if (fRemember) {
                OleStdVerifyRelease(
                        (LPUNKNOWN)lpContainerLine->m_lpStg, 
                        "Original object stg not released"
                );
                lpContainerLine->m_lpStg = lpObjDestStg;
            } else {
                OleStdVerifyRelease(
                        (LPUNKNOWN)lpObjDestStg, 
                        "Copied object stg not released"
                );
            }
        }
        return TRUE;
    }

    return TRUE;

error:

    /* retore seek position prior to writing Line record */
    lpLLStm->lpVtbl->Seek(
            lpLLStm,
            dlibSavePos,
            STREAM_SEEK_SET,
            NULL
    );

    return FALSE;
}


/* ContainerLine_SaveOleObject
** ---------------------------
**    Save the OLE object associated with the ContainerLine.
**    
**    OLE2NOTE: this function demonstrates the most basic form of
**    saving an OLE object. see the OLE 2.0 documentation for a
**    discussion of other more advanced strategies for saving (eg.
**    saving with backup file and low memory save).
*/
HRESULT ContainerLine_SaveOleObject(
		LPCONTAINERLINE		lpContainerLine,
		LPSTORAGE			lpStg,
		BOOL				fSameAsLoad,
		BOOL				fRemember,
        BOOL                fForceUpdate
)
{
	LPPERSISTSTORAGE lpPersistStg;
	SCODE			 sc = S_OK;
	HRESULT			 hrErr;
	
	if (! lpContainerLine->m_lpPersistStg) {
		lpContainerLine->m_lpPersistStg = 
            (LPPERSISTSTORAGE)OleStdQueryInterface(
				(LPUNKNOWN)lpContainerLine->m_lpOleObj, &IID_IPersistStorage);
		if (! lpContainerLine->m_lpPersistStg) {
			OleDbgAssert(lpContainerLine->m_lpPersistStg);
            return ResultFromScode(E_FAIL);
		}
	}

    lpPersistStg = lpContainerLine->m_lpPersistStg;

    /* OLE2NOTE: the container should NOT call IOleObject::Update
    **    before calling OleSave on an embedded object. an earlier
    **    version of the Outline sample incorrectly called
    **    IOleObject::Update here in order to guarantee that we get the
    **    latest presentation of the object when it has a coarse update
    **    granularity (eg. OLE 1.0 embeddings). it is the
    **    responsibility of the Object application to send an
    **    OnDataChange when IPersistStorage::Save is called if there
    **    currently are un-broadcast change notifications BEFORE
    **    returning from IPersistStorage::Save. these data change
    **    notifications will get through and cause the cache to be
    **    updated prior to the saving of the cache. (this discussion is
    **    only pertinent to EXE based servers that use OLE's
    **    DefHandler. calling IOleObject::Update would have the
    **    undesireable side-effect of causing any nested manual links
    **    to be updated automatically.
    */
    
    OLEDBG_BEGIN2("OleSave called\r\n")
    hrErr = OleSave(lpPersistStg, lpStg, fSameAsLoad);
    OLEDBG_END2

	// OLE2NOTE: if OleSave returns an error, you must still call 
    //           SaveCompleted.
    if (hrErr != NOERROR) {
        OleDbgOutHResult("WARNING: OleSave returned", hrErr);
		sc = GetScode(hrErr);
	}

	/* OLE2NOTE: a root level container should immediately
	**    call IPersistStorage::SaveCompleted after calling OleSave. a
	**    nested level container should not call SaveCompleted now, but
	**    must wait until SaveCompleted is call on it by its container.
	**    since our container is not a container/server, then we always
	**    call SaveComplete here. 
    **    
    **    if this is a SaveAs operation, then we need to pass the lpStg
    **    back in SaveCompleted to inform the object of its new storage
    **    that it may hold on to.
    **    if this is a Save or a SaveCopyAs operation, then we simply
    **    pass NULL in SaveCompleted; the object can continue to hold
    **    its current storage. if an error occurs during the OleSave
    **    call we must still call SaveCompleted but we must pass NULL.
	*/
	OLEDBG_BEGIN2("IPersistStorage::SaveCompleted called\r\n")
	hrErr = lpPersistStg->lpVtbl->SaveCompleted(
			lpPersistStg,
			((FAILED(sc) || !fRemember || fSameAsLoad) ? NULL : lpStg)
	);
	OLEDBG_END2

	if (hrErr != NOERROR) {
		OleDbgOutHResult("WARNING: SaveCompleted returned",hrErr);
		if (sc == S_OK) 
			sc = GetScode(hrErr);
	}

	return ResultFromScode(sc);
}


/* ContainerLine_LoadFromStg
** -------------------------
**    Create a ContainerLine object and initialize it with data that
**    was previously writen to an IStorage*. this function does not
**    immediately OleLoad the associated OLE object, only the data of
**    the ContainerLine object itself is loaded from the IStorage*.
*/
LPLINE ContainerLine_LoadFromStg(
        LPSTORAGE               lpSrcStg, 
        LPSTREAM                lpLLStm, 
        LPOUTLINEDOC            lpDestDoc
)
{
    HDC         hDC;
    LPLINELIST  lpDestLL = &lpDestDoc->m_LineList;
    ULONG nRead;
    HRESULT hrErr;
    LPCONTAINERLINE lpContainerLine;
    CONTAINERLINERECORD objLineRecord;

    lpContainerLine=(LPCONTAINERLINE) New((DWORD)sizeof(CONTAINERLINE));
    if (lpContainerLine == NULL) {
        OleDbgAssertSz(
                lpContainerLine!=NULL, "Error allocating ContainerLine");
        return NULL; 
    }

    hDC = LineList_GetDC(lpDestLL); 
    ContainerLine_Init(lpContainerLine, 0, hDC);
    LineList_ReleaseDC(lpDestLL, hDC);

    /* OLE2NOTE: In order to have a stable ContainerLine object we must
	**    AddRef the object's refcnt. this will be later released when
	**    the ContainerLine is deleted.
    */
    ContainerLine_AddRef(lpContainerLine);
    
    lpContainerLine->m_lpDoc = (LPCONTAINERDOC) lpDestDoc;

    /* read line record */
    hrErr = lpLLStm->lpVtbl->Read(
            lpLLStm,
            (LPVOID)&objLineRecord,
            sizeof(CONTAINERLINERECORD),
            &nRead
    );

    if (!OleDbgVerifySz(hrErr==NOERROR,"Could not read from LineList stream"))
        goto error;

    lstrcpy(lpContainerLine->m_szStgName, objLineRecord.m_szStgName);
    lpContainerLine->m_fMonikerAssigned = objLineRecord.m_fMonikerAssigned;
    lpContainerLine->m_dwDrawAspect = objLineRecord.m_dwDrawAspect;
    lpContainerLine->m_sizeInHimetric = objLineRecord.m_sizeInHimetric;
    lpContainerLine->m_fIsLink = objLineRecord.m_fIsLink;
	
    return (LPLINE)lpContainerLine;

error:
    // destroy partially created ContainerLine
    if (lpContainerLine) 
        ContainerLine_Delete(lpContainerLine);      
    return NULL;
}


/* ContainerLine_GetRelMoniker
** ---------------------------
**    Retrieve the relative item moniker which identifies the OLE object
**    relative to the container document.
**    
**    Returns NULL if a moniker can NOT be created.
*/
LPMONIKER ContainerLine_GetRelMoniker(
        LPCONTAINERLINE         lpContainerLine, 
        DWORD                   dwAssign
)
{
    LPMONIKER lpmk = NULL;

	/* OLE2NOTE: we should only give out a moniker for the OLE object
	**    if the object is allowed to be linked to from the inside. if
	**    so we are allowed to give out a moniker which binds to the
	**    running OLE object). if the object is an OLE 2.0 embedded
	**    object then it is allowed to be linked to from the inside. if
	**    the object is either an OleLink or an OLE 1.0 embedding 
	**    then it can not be linked to from the inside.
	**    if we were a container/server app then we could offer linking
	**    to the outside of the object (ie. a pseudo object within our 
	**    document). we are a container only app that does not support
	**    linking to ranges of its data. 
	*/
		
    switch (dwAssign) {

        case GETMONIKER_FORCEASSIGN:

                /* Force the assignment of the name. This is called when a
                **    Paste Link actually occurs. From now on we want
                **    to inform the OLE object that its moniker is
                **    assigned and is thus necessary to register itself
                **    in the RunningObjectTable.
                */
                CreateItemMoniker(
                        OLESTDDELIM, 
                        lpContainerLine->m_szStgName, 
                        &lpmk
                );
    
                /* OLE2NOTE: if the OLE object is already loaded and it
                **    is being assigned a moniker for the first time,
                **    then we need to inform it that it now has a moniker
                **    assigned by calling IOleObject::SetMoniker. this
                **    will force the OLE object to register in the
                **    RunningObjectTable when it enters the running
                **    state. if the object is not currently loaded, 
                **    SetMoniker will be called automatically later when
                **    the object is loaded by the function
                **    ContainerLine_LoadOleObject.
                */
                if (lpContainerLine->m_lpOleObj && 
                                    !lpContainerLine->m_fMonikerAssigned) {
                    OLEDBG_BEGIN2("IOleObject::SetMoniker called\r\n")
                    lpContainerLine->m_lpOleObj->lpVtbl->SetMoniker(
                            lpContainerLine->m_lpOleObj,
                            OLEWHICHMK_OBJREL,
                            lpmk
                    );
                    OLEDBG_END2
                }

                /* we must remember forever more that this object has a
                **    moniker assigned.
                */
                lpContainerLine->m_fMonikerAssigned = TRUE;
                break;

        case GETMONIKER_ONLYIFTHERE:

                /* If the OLE object currently has a moniker assigned,
                **    then return it.
                */
                if (lpContainerLine->m_fMonikerAssigned) {
                    CreateItemMoniker(
                            OLESTDDELIM, 
                            lpContainerLine->m_szStgName, 
                            &lpmk
                    );
                }
                break;

        case GETMONIKER_TEMPFORUSER:

                /* Return the moniker that would be used for the OLE
                **    object but do NOT force moniker assignment at
                **    this point. Since our strategy is to use the
                **    storage name of the object as its item name, we
                **    can simply create the corresponding ItemMoniker
                **    (indepenedent of whether the moniker is currently
                **    assigned or not).
                */
                CreateItemMoniker(
                        OLESTDDELIM, 
                        lpContainerLine->m_szStgName, 
                        &lpmk
                );
                break;

        case GETMONIKER_UNASSIGN:

                lpContainerLine->m_fMonikerAssigned = FALSE;
                break;
                
    }

    return lpmk;
}


/* ContainerLine_GetFullMoniker
** ----------------------------
**    Retrieve the full absolute moniker which identifies the OLE object
**    in the container document.
**    this moniker is created as a composite of the absolute moniker for 
**    the entire document appended with an item moniker which identifies 
**    the OLE object relative to the document.
**    Returns NULL if a moniker can NOT be created.
*/
LPMONIKER ContainerLine_GetFullMoniker(
        LPCONTAINERLINE         lpContainerLine, 
        DWORD                   dwAssign
)
{
    LPMONIKER lpmkDoc = NULL;
    LPMONIKER lpmkItem = NULL;
    LPMONIKER lpmkFull = NULL;

    lpmkDoc = OleDoc_GetFullMoniker(
            (LPOLEDOC)lpContainerLine->m_lpDoc, 
            dwAssign
    );
    if (! lpmkDoc) return NULL;

    lpmkItem = ContainerLine_GetRelMoniker(lpContainerLine, dwAssign);

    if (lpmkItem) {
        CreateGenericComposite(lpmkDoc, lpmkItem, (LPMONIKER FAR*)&lpmkFull);
        OleStdRelease((LPUNKNOWN)lpmkItem);
    }

    if (lpmkDoc) 
        OleStdRelease((LPUNKNOWN)lpmkDoc);

    return lpmkFull;
}


/* ContainerLine_GetTextLen
 * ------------------------
 *
 * Return length of the string representation of the ContainerLine
 *  (not considering the tab level). we will use the following as the 
 *  string representation of a ContainerLine:
 *      "<" + user type name of OLE object + ">"
 *  eg:
 *      <Microsoft Excel Worksheet>
 */
int ContainerLine_GetTextLen(LPCONTAINERLINE lpContainerLine)
{
    LPSTR   lpszUserType = NULL;
    HRESULT hrErr;
    int     nLen;
    BOOL    fIsLink = ContainerLine_IsOleLink(lpContainerLine);

    /* if object is not already loaded, then load it now. objects are
    **    loaded lazily in this manner.
    */
    if (! lpContainerLine->m_lpOleObj) 
        ContainerLine_LoadOleObject(lpContainerLine);
    
    OLEDBG_BEGIN2("IOleObject::GetUserType called\r\n")
    hrErr = lpContainerLine->m_lpOleObj->lpVtbl->GetUserType(
            lpContainerLine->m_lpOleObj,
            USERCLASSTYPE_FULL,
            &lpszUserType
    );
    OLEDBG_END2

    if (hrErr != NOERROR)   {   
        // user type is NOT available
        nLen = sizeof(UNKNOWN_OLEOBJ_TYPE) + 2; // allow space for '<' + '>'
        nLen += lstrlen((LPSTR)(fIsLink ? szOLELINK : szOLEOBJECT)) + 1;
    } else {
        nLen = lstrlen(lpszUserType) + 2;   // allow space for '<' + '>'
        nLen += lstrlen((LPSTR)(fIsLink ? szOLELINK : szOLEOBJECT)) + 1;

        /* OLE2NOTE: we must free the string that was allocated by the
        **    IOleObject::GetUserType method.
        */
        OleStdFreeString(lpszUserType, NULL);
    }

    return nLen;
}


/* ContainerLine_GetTextData
 * -------------------------
 *
 * Return the string representation of the ContainerLine
 *  (not considering the tab level). we will use the following as the 
 *  string representation of a ContainerLine:
 *      "<" + user type name of OLE object + ">"
 *  eg:
 *      <Microsoft Excel Worksheet>
 */
void ContainerLine_GetTextData(LPCONTAINERLINE lpContainerLine, LPSTR lpszBuf)
{
    LPSTR   lpszUserType = NULL;
    BOOL    fIsLink = ContainerLine_IsOleLink(lpContainerLine);
    HRESULT hrErr;

    /* if object is not already loaded, then load it now. objects are
    **    loaded lazily in this manner.
    */
    if (! lpContainerLine->m_lpOleObj) 
        ContainerLine_LoadOleObject(lpContainerLine);
    
    hrErr = lpContainerLine->m_lpOleObj->lpVtbl->GetUserType(
            lpContainerLine->m_lpOleObj,
            USERCLASSTYPE_FULL,
            &lpszUserType
    );

    if (hrErr != NOERROR)   {
        // user type is NOT available
        wsprintf(
                lpszBuf, 
                "<%s %s>", 
                UNKNOWN_OLEOBJ_TYPE,
                (LPSTR)(fIsLink ? szOLELINK : szOLEOBJECT)
        );
    } else {
        wsprintf(
                lpszBuf, 
                "<%s %s>", 
                lpszUserType,
                (LPSTR)(fIsLink ? szOLELINK : szOLEOBJECT)
        );

        /* OLE2NOTE: we must free the string that was allocated by the
        **    IOleObject::GetUserType method.
        */
        OleStdFreeString(lpszUserType, NULL);
    }
}


/* ContainerLine_GetOutlineData
 * ----------------------------
 *
 * Return the CF_OUTLINE format data for the ContainerLine.
 */
BOOL ContainerLine_GetOutlineData(
        LPCONTAINERLINE         lpContainerLine, 
        LPTEXTLINE              lpBuf
)
{
    LPLINE      lpLine = (LPLINE)lpContainerLine;
    LPLINELIST  lpLL = &((LPOUTLINEDOC)lpContainerLine->m_lpDoc)->m_LineList;
    HDC         hDC;
    char        szTmpBuf[MAXSTRLEN+1];
    LPTEXTLINE  lpTmpTextLine;

    // Create a TextLine with the Text representation of the ContainerLine.
    ContainerLine_GetTextData(lpContainerLine, (LPSTR)szTmpBuf);
    
    hDC = LineList_GetDC(lpLL);
    lpTmpTextLine = TextLine_Create(hDC, lpLine->m_nTabLevel, szTmpBuf);
    LineList_ReleaseDC(lpLL, hDC);
    
    TextLine_Copy(lpTmpTextLine, lpBuf);
    
    // Delete the temporary TextLine
    TextLine_Delete(lpTmpTextLine);
    return TRUE;
}


/* ContainerLine_GetOleObjectRectInPixels
** --------------------------------------
**    Get the extent of the OLE Object contained in the given Line in
**    client coordinates after scaling. 
*/
void ContainerLine_GetOleObjectRectInPixels(LPCONTAINERLINE lpContainerLine, LPRECT lprc)
{
    LPOUTLINEDOC lpOutlineDoc;
	LPSCALEFACTOR lpscale;
    LPLINELIST lpLL;
    LPLINE lpLine;
    int nIndex;
    HDC hdcLL;
    
	if (!lpContainerLine || !lprc)
		return;
	
    lpOutlineDoc = (LPOUTLINEDOC)lpContainerLine->m_lpDoc;
	lpscale = OutlineDoc_GetScaleFactor(lpOutlineDoc);
    lpLL = OutlineDoc_GetLineList(lpOutlineDoc);
    lpLine = (LPLINE)lpContainerLine;
    nIndex = LineList_GetLineIndex(lpLL, lpLine);

    LineList_GetLineRect(lpLL, nIndex, lprc);

    hdcLL = GetDC(lpLL->m_hWndListBox);

    /* lprc is set to be size of Line Object (including the boundary) */
    lprc->left += (int)(
			(long)XformWidthInHimetricToPixels(hdcLL,
					lpLine->m_nTabWidthInHimetric +
					LOWORD(OutlineDoc_GetMargin(lpOutlineDoc))) * 
			lpscale->dwSxN / lpscale->dwSxD);
    lprc->right = (int)(
			lprc->left + (long)
			XformWidthInHimetricToPixels(hdcLL, lpLine->m_nWidthInHimetric) *
			lpscale->dwSxN / lpscale->dwSxD);

	// Remove the boundary from rect
	InflateRect(
			lprc, 
			(int)(-LINE_BOUNDARY_WIDTH * lpscale->dwSxN / lpscale->dwSxD), 
			(int)(-LINE_BOUNDARY_WIDTH * lpscale->dwSyN / lpscale->dwSyD)
	);
	
    ReleaseDC(lpLL->m_hWndListBox, hdcLL);
}


/* ContainerLine_GetOleObjectSizeInHimetric
** ----------------------------------------
**    Get the size of the OLE Object contained in the given Line
*/
void ContainerLine_GetOleObjectSizeInHimetric(LPCONTAINERLINE lpContainerLine, LPSIZEL lpsizel)
{
	if (!lpContainerLine || !lpsizel)
		return;

	*lpsizel = lpContainerLine->m_sizeInHimetric;
}
	
	

/*************************************************************************
** ContainerLine::IUnknown interface implementation
*************************************************************************/

STDMETHODIMP CntrLine_Unk_QueryInterface(
        LPUNKNOWN           lpThis, 
        REFIID              riid, 
        LPVOID FAR*         lplpvObj
)
{
    LPCONTAINERLINE lpContainerLine =  
            ((struct COleClientSiteImpl FAR*)lpThis)->lpContainerLine;
    
    return ContainerLine_QueryInterface(lpContainerLine, riid, lplpvObj);
}
    

STDMETHODIMP_(ULONG) CntrLine_Unk_AddRef(LPUNKNOWN lpThis)
{
    LPCONTAINERLINE lpContainerLine = 
            ((struct COleClientSiteImpl FAR*)lpThis)->lpContainerLine;

    OleDbgAddRefMethod(lpThis, "IUnknown");

    return ContainerLine_AddRef(lpContainerLine);
}


STDMETHODIMP_(ULONG) CntrLine_Unk_Release(LPUNKNOWN lpThis)
{
    LPCONTAINERLINE lpContainerLine = 
            ((struct COleClientSiteImpl FAR*)lpThis)->lpContainerLine;

    OleDbgReleaseMethod(lpThis, "IUnknown");

    return ContainerLine_Release(lpContainerLine);
}


/*************************************************************************
** ContainerLine::IOleClientSite interface implementation
*************************************************************************/

STDMETHODIMP CntrLine_CliSite_QueryInterface(
        LPOLECLIENTSITE     lpThis, 
        REFIID              riid,
        LPVOID FAR*         lplpvObj
)
{
    LPCONTAINERLINE lpContainerLine = 
            ((struct COleClientSiteImpl FAR*)lpThis)->lpContainerLine;
    
    return ContainerLine_QueryInterface(lpContainerLine, riid, lplpvObj);
}
    

STDMETHODIMP_(ULONG) CntrLine_CliSite_AddRef(LPOLECLIENTSITE lpThis)
{
    LPCONTAINERLINE lpContainerLine = 
            ((struct COleClientSiteImpl FAR*)lpThis)->lpContainerLine;

    OleDbgAddRefMethod(lpThis, "IOleClientSite");

    return ContainerLine_AddRef(lpContainerLine);
}


STDMETHODIMP_(ULONG) CntrLine_CliSite_Release(LPOLECLIENTSITE lpThis)
{
    LPCONTAINERLINE lpContainerLine = 
            ((struct COleClientSiteImpl FAR*)lpThis)->lpContainerLine;

    OleDbgReleaseMethod(lpThis, "IOleClientSite");

    return ContainerLine_Release(lpContainerLine);
}


STDMETHODIMP CntrLine_CliSite_SaveObject(LPOLECLIENTSITE lpThis)
{
    LPCONTAINERLINE lpContainerLine =
            ((struct COleClientSiteImpl FAR*)lpThis)->lpContainerLine;
    SCODE sc = E_FAIL;
    HRESULT hrErr;
    BOOL fSameAsLoad = TRUE;
    BOOL fRemember = TRUE;
    
    OLEDBG_BEGIN2("CntrLine_CliSite_SaveObject\r\n")

    if (lpContainerLine->m_lpOleObj == NULL) {
        OleDbgAssertSz(
                lpContainerLine->m_lpOleObj != NULL, "OLE object not loaded");
        sc = E_FAIL;
        goto error;
    }
    
    // mark ContainerDoc as now dirty
    OutlineDoc_SetModified(
            (LPOUTLINEDOC)lpContainerLine->m_lpDoc, TRUE, TRUE, FALSE);

    /* Tell OLE object to save itself */
	hrErr = ContainerLine_SaveOleObject(
			lpContainerLine, 
			lpContainerLine->m_lpStg, 
			fSameAsLoad, 
			fRemember,
            FALSE   /* fForceUpdate */
	);

    if (hrErr != NOERROR) {
        sc = GetScode(hrErr);
        goto error;
    }
    
    OLEDBG_END2
    return NOERROR;

error:
    OLEDBG_END2
    return ResultFromScode(sc);
}


STDMETHODIMP CntrLine_CliSite_GetMoniker(
        LPOLECLIENTSITE     lpThis,
        DWORD               dwAssign, 
        DWORD               dwWhichMoniker,
        LPMONIKER FAR*      lplpmk
)
{
    LPCONTAINERLINE lpContainerLine;

    lpContainerLine=((struct COleClientSiteImpl FAR*)lpThis)->lpContainerLine;

    OLEDBG_BEGIN2("CntrLine_CliSite_GetMoniker\r\n")

    // OLE2NOTE: we must make sure to set output pointer parameters to NULL
    *lplpmk = NULL;

    switch (dwWhichMoniker) {

        case OLEWHICHMK_CONTAINER:
            /* OLE2NOTE: create a FileMoniker which identifies the
            **    entire container document. 
            */
            *lplpmk = OleDoc_GetFullMoniker(
                    (LPOLEDOC)lpContainerLine->m_lpDoc, 
                    dwAssign
            );
            break;

        case OLEWHICHMK_OBJREL:

            /* OLE2NOTE: create an ItemMoniker which identifies the
            **    OLE object relative to the container document. 
            */
            *lplpmk = ContainerLine_GetRelMoniker(lpContainerLine, dwAssign);
            break;

        case OLEWHICHMK_OBJFULL:
        {
            /* OLE2NOTE: create an absolute moniker which identifies the
            **    OLE object in the container document. this moniker is
            **    created as a composite of the absolute moniker for the
            **    entire document appended with an item moniker which
            **    identifies the OLE object relative to the document.
            */

            *lplpmk = ContainerLine_GetFullMoniker(lpContainerLine, dwAssign);
            break;
        }
    }
    
    OLEDBG_END2

    if (*lplpmk != NULL) 
        return NOERROR;
    else 
        return ResultFromScode(E_FAIL);
}


STDMETHODIMP CntrLine_CliSite_GetContainer(
        LPOLECLIENTSITE     lpThis,
        LPOLECONTAINER FAR* lplpContainer
)
{
    LPCONTAINERLINE lpContainerLine;
    HRESULT hrErr;
    
    OLEDBG_BEGIN2("CntrLine_CliSite_GetContainer\r\n")

    lpContainerLine=((struct COleClientSiteImpl FAR*)lpThis)->lpContainerLine;
    
    hrErr = OleDoc_QueryInterface(
            (LPOLEDOC)lpContainerLine->m_lpDoc, 
            &IID_IOleContainer, 
            (LPVOID FAR*)lplpContainer
    );

    OLEDBG_END2
    return hrErr;
}


STDMETHODIMP CntrLine_CliSite_ShowObject(LPOLECLIENTSITE lpThis)
{
    LPCONTAINERLINE lpContainerLine = 
            ((struct COleClientSiteImpl FAR*)lpThis)->lpContainerLine;
    LPOUTLINEDOC lpOutlineDoc = (LPOUTLINEDOC)lpContainerLine->m_lpDoc;
    LPLINELIST lpLL = OutlineDoc_GetLineList(lpOutlineDoc);
    int nIndex = LineList_GetLineIndex(lpLL, (LPLINE)lpContainerLine);
    HWND hWndFrame = OutlineApp_GetFrameWindow(g_lpApp);

    OLEDBG_BEGIN2("CntrLine_CliSite_ShowObject\r\n")

    /* make sure our doc window is visible and not minimized.
    **    the OutlineDoc_ShowWindow function will cause the app window
    **    to show itself SW_SHOWNORMAL.
    */
    if (! IsWindowVisible(hWndFrame) || IsIconic(hWndFrame))       
        OutlineDoc_ShowWindow(lpOutlineDoc);

    /* make sure that the OLE object is currently in view. if necessary
    **    scroll the document in order to bring it into view.
    */
    LineList_ScrollLineIntoView(lpLL, nIndex);

#if defined( INPLACE_CNTR )	
	/* after the in-place object is scrolled into view, we need to ask
	**    it to update its rect for the new clip rect coordinates 
	*/
	ContainerDoc_UpdateInPlaceObjectRects((LPCONTAINERDOC)lpOutlineDoc, 0);
#endif 

    OLEDBG_END2
    return NOERROR;
}


STDMETHODIMP CntrLine_CliSite_OnShowWindow(LPOLECLIENTSITE lpThis, BOOL fShow)
{
    LPCONTAINERLINE lpContainerLine =
            ((struct COleClientSiteImpl FAR*)lpThis)->lpContainerLine;
    LPOUTLINEDOC lpOutlineDoc = (LPOUTLINEDOC)lpContainerLine->m_lpDoc;
    LPLINELIST lpLL = OutlineDoc_GetLineList(lpOutlineDoc);
    int nIndex = LineList_GetLineIndex(lpLL, (LPLINE)lpContainerLine);

    if (fShow) {
        OLEDBG_BEGIN2("CntrLine_CliSite_OnShowWindow(TRUE)\r\n")

        /* OLE2NOTE: we need to hatch out the OLE object now; it has
        **    just been opened in a window elsewhere (open editing as
        **    opposed to in-place activation). 
        **    force the line to re-draw with the hatch.
        */
        lpContainerLine->m_fObjWinOpen = TRUE;
        LineList_ForceLineRedraw(lpLL, nIndex, FALSE);

    } else {
        OLEDBG_BEGIN2("CntrLine_CliSite_OnShowWindow(FALSE)\r\n")
            
        /* OLE2NOTE: the object associated with this container site has
        **    just closed its server window. we should now remove the
        **    hatching that indicates that the object is open
        **    elsewhere. also our window should now come to the top.
        **    force the line to re-draw without the hatch.
        */
        lpContainerLine->m_fObjWinOpen = FALSE;
        LineList_ForceLineRedraw(lpLL, nIndex, TRUE);

        BringWindowToTop(lpOutlineDoc->m_hWndDoc);
        SetFocus(lpOutlineDoc->m_hWndDoc);
    }

    OLEDBG_END2
    return NOERROR;
}


STDMETHODIMP CntrLine_CliSite_RequestNewObjectLayout(LPOLECLIENTSITE lpThis)
{
    OleDbgOut2("CntrLine_CliSite_RequestNewObjectLayout\r\n");
		
	/* OLE2NOTE: this method is NOT yet used. it is for future layout
	**    negotiation support.
	*/
	return ResultFromScode(E_NOTIMPL);
}


/*************************************************************************
** ContainerLine::IAdviseSink interface implementation
*************************************************************************/

STDMETHODIMP CntrLine_AdvSink_QueryInterface(
        LPADVISESINK        lpThis,
        REFIID              riid, 
        LPVOID FAR*         lplpvObj
)
{
    LPCONTAINERLINE lpContainerLine = 
            ((struct COleClientSiteImpl FAR*)lpThis)->lpContainerLine;
    
    return ContainerLine_QueryInterface(lpContainerLine, riid, lplpvObj);
}


STDMETHODIMP_(ULONG) CntrLine_AdvSink_AddRef(LPADVISESINK lpThis)
{
    LPCONTAINERLINE lpContainerLine = 
            ((struct COleClientSiteImpl FAR*)lpThis)->lpContainerLine;

    OleDbgAddRefMethod(lpThis, "IAdviseSink");

    return ContainerLine_AddRef(lpContainerLine);
}


STDMETHODIMP_(ULONG) CntrLine_AdvSink_Release (LPADVISESINK lpThis)
{
    LPCONTAINERLINE lpContainerLine = 
            ((struct COleClientSiteImpl FAR*)lpThis)->lpContainerLine;

    OleDbgReleaseMethod(lpThis, "IAdviseSink");

    return ContainerLine_Release(lpContainerLine);
}


STDMETHODIMP_(void) CntrLine_AdvSink_OnDataChange(
        LPADVISESINK        lpThis,
        FORMATETC FAR*      lpFormatetc, 
        STGMEDIUM FAR*      lpStgmed
)
{
    OleDbgOut2("CntrLine_AdvSink_OnDataChange\r\n");

    // We are not interested in data changes (only view changes)
    //      (ie. nothing to do)
}


STDMETHODIMP_(void) CntrLine_AdvSink_OnViewChange(
        LPADVISESINK        lpThis, 
        DWORD               aspects, 
        LONG                lindex
)
{
    LPCONTAINERLINE lpContainerLine;
    LPOUTLINEDOC lpOutlineDoc;
    HWND hWndDoc;
    LPLINELIST lpLL;
	MSG msg;
    int nIndex;

    OLEDBG_BEGIN2("CntrLine_AdvSink_OnViewChange\r\n")

    lpContainerLine = ((struct CAdviseSinkImpl FAR*)lpThis)->lpContainerLine;
    lpOutlineDoc = (LPOUTLINEDOC)lpContainerLine->m_lpDoc;

	/* OLE2NOTE: at this point we simply invalidate the rectangle of
	**    the object to force a repaint in the future, we mark
	**    that the extents of the object may have changed
    **    (m_fDoGetExtent=TRUE), and we post a message
    **    (WM_U_UPDATEOBJECTEXTENT) to our document that one or more
    **    OLE objects may need to have their extents updated. later
    **    when this message is processed, the document loops through
    **    all lines to see if any are marked as needing an extent update.
	**    if infact the extents did change. this can be done by calling
	**    IOleObject::GetExtent to retreive the object's current
	**    extents and comparing with the last known extents for the
	**    object. if the extents changed, then relayout space for the
	**    object before drawing. we postpone the check to get
	**    the extents now because OnViewChange is an asyncronis method,
	**    and we have to careful not to call any syncronis methods back
	**    to the object. it is good practise to not call any object
	**    methods from within an asyncronis notification method.
    **    if there is already WM_U_UPDATEOBJECTEXTENT message waiting
    **    in our message queue, there is no need to post another one.
    **    in this way, if the server is updating quicker than we can
    **    keep up, we do not make unneccsary GetExtent calls. also if
    **    drawing is disabled, we postpone updating the extents of any
    **    objects until drawing is re-enabled.
	*/
	lpContainerLine->m_fDoGetExtent = TRUE;
    hWndDoc = OutlineDoc_GetWindow((LPOUTLINEDOC)lpContainerLine->m_lpDoc);

    if (lpOutlineDoc->m_nDisableDraw == 0 && 
        ! PeekMessage(&msg, hWndDoc, 
            WM_U_UPDATEOBJECTEXTENT, WM_U_UPDATEOBJECTEXTENT, 
            PM_NOREMOVE | PM_NOYIELD)) {
        PostMessage(
                hWndDoc, WM_U_UPDATEOBJECTEXTENT, 0, 0L);
    }

    // force the modified line to redraw.
    lpLL = OutlineDoc_GetLineList(lpOutlineDoc);
    nIndex = LineList_GetLineIndex(lpLL, (LPLINE)lpContainerLine);
    LineList_ForceLineRedraw(lpLL, nIndex, TRUE);
    
    // mark ContainerDoc as now dirty
    OutlineDoc_SetModified(lpOutlineDoc, TRUE, TRUE, FALSE);

    OLEDBG_END2
}


STDMETHODIMP_(void) CntrLine_AdvSink_OnRename(
        LPADVISESINK        lpThis, 
        LPMONIKER           lpmk
)
{
    OleDbgOut2("CntrLine_AdvSink_OnRename\r\n");

    /* OLE2NOTE: the Embedding Container has nothing to do here. this
    **    notification is important for linking situations. it tells
    **    the OleLink objects to update their moniker because the
    **    source object has been renamed (track the link source).
    */
}


STDMETHODIMP_(void) CntrLine_AdvSink_OnSave(LPADVISESINK lpThis)
{
    OleDbgOut2("CntrLine_AdvSink_OnSave\r\n");

    /* OLE2NOTE: the Embedding Container has nothing to do here. this
    **    notification is only useful to clients which have set up a
    **    data cache with the ADVFCACHE_ONSAVE flag.
    */
}


STDMETHODIMP_(void) CntrLine_AdvSink_OnClose(LPADVISESINK lpThis)
{
    OleDbgOut2("CntrLine_AdvSink_OnClose\r\n");

    /* OLE2NOTE: the Embedding Container has nothing to do here. this
    **    notification is important for the OLE's default object handler 
    **    and the OleLink object. it tells them the remote object is
    **    shutting down. 
    */
}
