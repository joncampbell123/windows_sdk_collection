/*****************************************************************************\
*                                                                             *
*    OleInPlaceContainerDoc.c                                                 *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#if !defined(_MSC_VER) && !defined(THINK_C)
#include "OLine.h"
#endif

#include "Types.h"

#if defined(USEHEADER)
#include "OleHdrs.h"
#endif
#include "Debug.h"
#include "Doc.h"
#include "App.h"
#include "OleXcept.h"
#include "OleDebug.h"
#include "Line.h"									  	
#include "Const.h"
#include "Util.h"
#include "Script.h"
#include "stdio.h"
#include <Resources.h>
#include <TextUtils.h>
#include <ole2ui.h>
OLEDBGDATA

// OLDNAME: OleContainerDoc.c


extern OleApplicationPtr		gOleApp;
extern CursHandle				gWatchCursor;

//#pragma segment OleContainerDocSeg
void OleContainerDocInit(OleDocumentPtr pOleDoc)
{
	OleContainerDocPtr	pContainerDoc;
	
	pContainerDoc = &pOleDoc->container;
	
	pContainerDoc->m_pCopiedEmbedding = nil;
	pContainerDoc->m_pCopiedEmbeddingSource = nil;	
	pContainerDoc->m_AspectOleObjCopied = 0;
	pContainerDoc->m_clsidOleObjCopied = CLSID_NULL;
	
#if qOleInPlace
	OleInPlaceContainerDocInit(pOleDoc);
#endif
}

//#pragma segment OleContainerDocSeg
void OleContainerDocDispose(OleDocumentPtr pOleDoc)
{
#if qOleInPlace
	OleInPlaceContainerDocDispose(pOleDoc);
#endif
}

//#pragma segment OleContainerDocSeg
HRESULT OleContainerDocLocalQueryInterface(OleDocumentPtr pOleDoc, REFIID riid, void* * lplpvObj)
{
#if qOleInPlace
	return OleInPlaceContainerDocLocalQueryInterface(pOleDoc, riid, lplpvObj);
#endif

	*lplpvObj = NULL;
	return ResultFromScode(E_NOINTERFACE);
}

//#pragma segment OleContainerDocSeg
void OleContainerDocDoCopySingleEmbedding(
		OleDocumentPtr		pOleDoc,
		LPOLEOBJECT			pSrcOleObj,
		unsigned long		DrawAspect,
		void*				pCopiedItem,
		void*				pCopiedItemSource
)
{
	OleContainerDocPtr 	pContainerDoc = &pOleDoc->container;
	unsigned long	 	Status;
		
	pContainerDoc->m_pCopiedEmbedding = pCopiedItem;
	pContainerDoc->m_pCopiedEmbeddingSource = pCopiedItemSource
	;
	pSrcOleObj->lpVtbl->GetUserClassID(
			pSrcOleObj,
			&pContainerDoc->m_clsidOleObjCopied
	);
			
	pContainerDoc->m_AspectOleObjCopied = DrawAspect;

    /* OLE2NOTE: if the object is allowed to be linked
    **    to from the inside (ie. we are allowed to
    **    give out a moniker which binds to the running
    **    OLE object), then we want to offer
    **    CF_LINKSOURCE format. if the object is an OLE
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
	pSrcOleObj->lpVtbl->GetMiscStatus(
					pSrcOleObj,
					DVASPECT_CONTENT, /* aspect is not important */
					&Status
	);
	if (! (Status & OLEMISC_CANTLINKINSIDE)) {
		/* Our container supports linking to an embedded
		**    object. We want the lpDestContainerDoc to
		**    offer CF_LINKSOURCE via the IDataObject
		**    interface that it gives to the clipboard or
        **    the drag/drop operation. The link source will
        **    be identified by a composite moniker
		**    comprised of the FileMoniker of the source
		**    document and an ItemMoniker which identifies
		**    the OLE object inside the container. we do
		**    NOT want to force moniker assignment to the
		**    OLE object now (at copy time); we only want
		**    to FORCE moniker assignment later if a Paste
		**    Link occurs (ie. GetData for CF_LINKSOURCE).
		**    thus we store a pointer to the source document
		**    and the source ContainerLine so we can
		**    generate a proper ItemMoniker later when
		**    Paste Link occurs.
		*/
		pOleDoc->m_fLinkSourceAvail = true;
	}
}


//#pragma segment OleContainerDocSeg
void OleContainerDocPasteLink(OleDocumentPtr pOleDoc)
{
	OleApplicationPtr	pOleApp;
	LPDATAOBJECT		pClipboardDataObj;
	HRESULT				hrErr;
	Handle				hMem = NULL;
	STGMEDIUM			medium;
	Boolean				fDisplayAsIcon = false;
	PicHandle			hMetaPict = NULL;
	
	pOleApp = gOleApp;
	
    hrErr = OleGetClipboard(&pClipboardDataObj);
    ASSERTNOERROR(hrErr);
    FailOleErr(hrErr);

    // this could take a minute
    SetCursor(*gWatchCursor);

    /* OLE2NOTE: we need to check what dwDrawAspect is being
    **    transfered. if the data is an object that is displayed as an
    **    icon in the source, then we want to keep it as an icon. the
    **    aspect the object is displayed in at the source is transfered
    **    via the CF_LINKSOURCEDESCRIPTOR format for a PasteLink
    **    operation.
    */
	hMem = OleStdGetData(
				pClipboardDataObj,
				pOleApp->m_cfLinkSrcDescriptor,
				NULL,
				DVASPECT_CONTENT,
				&medium
			);
	if (hMem)
	{
		LPOBJECTDESCRIPTOR	pOD;
		
		HLock(hMem);
		pOD = (LPOBJECTDESCRIPTOR)*hMem;
		fDisplayAsIcon = (pOD->dwDrawAspect == DVASPECT_ICON ? TRUE : FALSE);
		HUnlock(hMem);
		ReleaseStgMedium(&medium);
		
		if (fDisplayAsIcon)
		{
			hMetaPict = (PicHandle)OleStdGetData(
							pClipboardDataObj,
							'PICT',
							NULL,
							DVASPECT_ICON,
							&medium
						);
			if (hMetaPict == NULL)
				fDisplayAsIcon = false;		
		}
	}
		
	OleDocPasteFormatFromData(
				pOleDoc,
				pOleApp->m_cfLinkSource,
				pClipboardDataObj,
				TRUE,
				fDisplayAsIcon,
				hMetaPict
			);
	
	if (hMetaPict)
		ReleaseStgMedium(&medium);
			
    if (pClipboardDataObj)
    	OleStdRelease((LPUNKNOWN)pClipboardDataObj);
}

//#pragma segment OleContainerDocSeg
/* OleContainerDocPasteFormatFromData
** ----------------------------------
**
**    Paste a particular data format from a IDataObject*. The
**    IDataObject* may come from the clipboard (GetClipboard) or from a
**    drag/drop operation.
**
*/
void OleContainerDocPasteFormatFromData(
        OleDocumentPtr			pOleDoc,
        ResType              	cfFormat,
        LPDATAOBJECT            pSrcDataObj,
        Boolean                 fLink,
        Boolean                 fDisplayAsIcon,
        PicHandle               hMetaPict
)
{
    OleApplicationPtr pOleApp = gOleApp;
    Handle      hData;
    STGMEDIUM   medium;
    FORMATETC	formatetc;
    HRESULT		hrErr;

	
    if (fLink)
    {
        /* We should paste a Link to the data */

		if (cfFormat != pOleApp->m_cfLinkSource)
			return;   // we only support OLE object type links
		
		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_PasteOleObjectProcPtr != nil);
		(*pOleDoc->m_pIDoc->lpVtbl->m_PasteOleObjectProcPtr)(
                pOleDoc->m_pIDoc,
                pSrcDataObj,
                OLECREATEFROMDATA_LINK,
                cfFormat,
                fDisplayAsIcon,
                hMetaPict
            );
    }
    else
    {

		ASSERTCOND(pOleApp->m_pIApp->lpVtbl->m_GetClipboardFormatProcPtr != nil);
        if (cfFormat == (*pOleApp->m_pIApp->lpVtbl->m_GetClipboardFormatProcPtr)(pOleApp->m_pIApp)) {

#if LATER
			// NOTE: this case is considered just for efficiency purpose. Case 2 will always work
			
            if (fLocalDataObj) {

                /* CASE I: IDataObject* is local to our app
                **
                **    if the source of the data is local to our
                **    application instance, then we can get direct
                **    access to the original OleDoc object that
                **    corresponds to the IDataObject* given.
                **    CF_CNTROUTL data is passed through a LPSTORAGE.
                **    if we call OleGetData asking for CF_CNTROUTL, we
                **    will be returned a copy of the existing open pStg
                **    of the original source document. we can NOT open
                **    streams and sub-storages again via this pStg
                **    since it is already open within our same
                **    application instance. we must copy the data from
                **    the original OleDoc source document.
                */
                LPLINELIST lpSrcLL;
                LPOLEDOC lpLocalSrcDoc =
                    ((struct CDocDataObjectImpl FAR*)lpSrcDataObj)->lpOleDoc;

                /* copy all lines from SrcDoc to DestDoc. */
                lpSrcLL = &((LPOUTLINEDOC)lpLocalSrcDoc)->m_LineList;
                nCount = LineList_CopySelToDoc(
                        lpSrcLL,
                        NULL,
                        (LPOUTLINEDOC)lpContainerDoc
                );

            } else
#endif // LATER

            {
                /* CASE II: IDataObject* is NOT local to our app
                **
                **    if the source of the data comes from another
                **    application instance. we can call GetDataHere to
                **    retrieve the CF_CNTROUTL data. CF_CNTROUTL data
                **    is passed through a LPSTORAGE. we MUST use
                **    IDataObject::GetDataHere. calling
                **    IDataObject::GetData does NOT work because OLE
                **    currently does NOT support remoting of a callee
                **    allocated root storage back to the caller. this
                **    hopefully will be supported in a future version.
                **    in order to call GetDataHere we must allocate an
                **    IStorage instance for the callee to write into.
                **    we will allocate an IStorage docfile that will
                **    delete-on-release. we could use either a
                **    memory-based storage or a file-based storage.
                */
                LPSTORAGE lpTmpStg = OleStdCreateTempStorage(
                        FALSE /*fUseMemory*/,
                        STGM_READWRITE | STGM_TRANSACTED |STGM_SHARE_EXCLUSIVE
                );
                if (! lpTmpStg)
                    return;

                formatetc.cfFormat = cfFormat;
                formatetc.ptd = NULL;
                formatetc.dwAspect = DVASPECT_CONTENT;
                formatetc.tymed = TYMED_ISTORAGE;
                formatetc.lindex = -1;

                medium.tymed = TYMED_ISTORAGE;
#ifndef _MSC_VER
                medium.u.pstg = lpTmpStg;
#else
                medium.pstg = lpTmpStg;
#endif
                medium.pUnkForRelease = NULL;

                hrErr = pSrcDataObj->lpVtbl->GetDataHere(
                        pSrcDataObj,
                        (LPFORMATETC)&formatetc,
                        (LPSTGMEDIUM)&medium
                );

                if (hrErr == NOERROR) {
					ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_PasteOutlineDataProcPtr != nil);
					(*pOleDoc->m_pIDoc->lpVtbl->m_PasteOutlineDataProcPtr)(pOleDoc->m_pIDoc, lpTmpStg);
                }
                OleStdRelease((LPUNKNOWN)lpTmpStg);

                return;
            }

        } else if (cfFormat == pOleApp->m_cfEmbedSource ||
            cfFormat == pOleApp->m_cfEmbeddedObject) {
            /* OLE2NOTE: OleCreateFromData API creates an OLE object if
            **    CF_EMBEDDEDOBJECT, CF_EMBEDSOURCE, or CF_FILENAME are
            **    available from the source data object. the
            **    CF_FILENAME case arises when a file is copied to the
            **    clipboard from the FileManager. if the file has an
            **    associated class (see GetClassFile API), then an
            **    object of that class is created. otherwise an OLE 1.0
            **    Packaged object is created.
            */
			ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_PasteOleObjectProcPtr != nil);
			(*pOleDoc->m_pIDoc->lpVtbl->m_PasteOleObjectProcPtr)(
                    pOleDoc->m_pIDoc,
                    pSrcDataObj,
                    OLECREATEFROMDATA_OBJECT,
                    cfFormat,
                    fDisplayAsIcon,
                    hMetaPict
            );
            return;

        } else if (cfFormat == 'PICT'
                    || cfFormat == 'DIB '
                    || cfFormat == 'BMAP') {

            /* OLE2NOTE: OleCreateStaticFromData API creates an static
            **    OLE object if 'PICT', 'DIB, or 'BMAP' is
            **    CF_EMBEDDEDOBJECT, CF_EMBEDSOURCE, or CF_FILENAME are
            **    available from the source data object.
            */
			ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_PasteOleObjectProcPtr != nil);
			(*pOleDoc->m_pIDoc->lpVtbl->m_PasteOleObjectProcPtr)(
                    pOleDoc->m_pIDoc,
                    pSrcDataObj,
                    OLECREATEFROMDATA_STATIC,
                    cfFormat,
                    fDisplayAsIcon,
                    hMetaPict
            );
			return;

        } else

        if(cfFormat == 'TEXT') {

            hData = OleStdGetData(
                    pSrcDataObj,
                    'TEXT',
                    NULL,
                    DVASPECT_CONTENT,
                    (LPSTGMEDIUM)&medium
            );
			if (!hData)
				return;

			ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_PasteTextDataProcPtr != nil);
			(*pOleDoc->m_pIDoc->lpVtbl->m_PasteTextDataProcPtr)(pOleDoc->m_pIDoc, hData);

           // OLE2NOTE: we must free data handle by releasing the medium
            ReleaseStgMedium((LPSTGMEDIUM)&medium);

        } else {
            return;   // no acceptable format available to paste
        }
    }
}

//#pragma segment OleContainerDocSeg
void OleContainerDocRenamedUpdate(OleDocumentPtr pOleDoc, LPMONIKER lpmkDoc)
{
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_InformAllOleObjectsDocRenamedProcPtr != nil);
	(*pOleDoc->m_pIDoc->lpVtbl->m_InformAllOleObjectsDocRenamedProcPtr)(pOleDoc->m_pIDoc, lpmkDoc);
}

//#pragma segment OleContainerDocSeg
void OleContainerDocDoActivate(OleDocumentPtr pOleDoc, Boolean becomingActive)
{
#if qOleInPlace
	if (becomingActive)
		OleInPlaceContainerDocDoActivate(pOleDoc, becomingActive);
#endif
}

// OLDNAME: OleOutlineContainerDocInt.c
extern CursHandle						gWatchCursor;

static IOleUILinkContainerVtbl			gOleOutlineDocOleUILinkContainerVtbl;

//#pragma segment OleOutlineContainerDocIntSeg
void OleOutlineContainerDocInitInterfaces(void)
{
	// OleOutlineContainerDoc::IOleUILinkContainer method table
	{
		IOleUILinkContainerVtbl* p;
		
		p = &gOleOutlineDocOleUILinkContainerVtbl;
		
		p->QueryInterface = 						IOleOutlineDocLinkContQueryInterface;
		p->AddRef = 								IOleOutlineDocLinkContAddRef;
		p->Release = 								IOleOutlineDocLinkContRelease;
		p->GetNextLink = 							IOleOutlineDocLinkContGetNextLink;
		p->SetLinkUpdateOptions = 					IOleOutlineDocLinkContSetLinkUpdateOptions;
		p->GetLinkUpdateOptions = 					IOleOutlineDocLinkContGetLinkUpdateOptions;
		p->SetLinkSource = 							IOleOutlineDocLinkContSetLinkSource;
		p->GetLinkSource = 							IOleOutlineDocLinkContGetLinkSource;
		p->OpenLinkSource = 						IOleOutlineDocLinkContOpenLinkSource;
		p->UpdateLink = 							IOleOutlineDocLinkContUpdateLink;
		p->CancelLink = 							IOleOutlineDocLinkContCancelLink;

		ASSERTCOND(ValidVtbl(p, sizeof(*p)));
	}
}

//#pragma segment OleOutlineContainerDocIntSeg
void OleOutlineDocIOleUILinkContainerInit(OutlineDocOleUILinkContainerImplPtr pOutlineDocOleUILinkContImpl, struct OleOutlineDocRec* pOleOutlineDoc)
{
	pOutlineDocOleUILinkContImpl->lpVtbl			= &gOleOutlineDocOleUILinkContainerVtbl;
	pOutlineDocOleUILinkContImpl->lpOleOutlineDoc	= pOleOutlineDoc;
	pOutlineDocOleUILinkContImpl->cRef				= 0;
}

/*************************************************************************
** OleOutlineContainerDoc::IOleUILinkContainer interface implementation
*************************************************************************/

//#pragma segment OleOutlineContainerDocIntSeg
STDMETHODIMP IOleOutlineDocLinkContQueryInterface(
        LPOLEUILINKCONTAINER    lpThis,
        REFIID             		riid,
        void**             		lplpvObj
)
{
    OleOutlineDocPtr pOleOutlineDoc =
            ((OutlineDocOleUILinkContainerImplPtr)lpThis)->lpOleOutlineDoc;

	OleDbgEnterInterface();

    return OleOutlineDocQueryInterface(pOleOutlineDoc, riid, lplpvObj);
}


//#pragma segment OleOutlineContainerDocIntSeg
STDMETHODIMP_(unsigned long) IOleOutlineDocLinkContAddRef(LPOLEUILINKCONTAINER lpThis)
{
    OleOutlineDocPtr pOleOutlineDoc =
            ((OutlineDocOleUILinkContainerImplPtr)lpThis)->lpOleOutlineDoc;

	OleDbgEnterInterface();

    return OleOutlineDocAddRef(pOleOutlineDoc);
}


//#pragma segment OleOutlineContainerDocIntSeg
STDMETHODIMP_(unsigned long) IOleOutlineDocLinkContRelease(LPOLEUILINKCONTAINER lpThis)
{
    OleOutlineDocPtr pOleOutlineDoc =
            ((OutlineDocOleUILinkContainerImplPtr)lpThis)->lpOleOutlineDoc;

	OleDbgEnterInterface();

    return OleOutlineDocRelease(pOleOutlineDoc);
}


//#pragma segment OleOutlineContainerDocIntSeg
STDMETHODIMP_(unsigned long) IOleOutlineDocLinkContGetNextLink(
        LPOLEUILINKCONTAINER    lpThis,
        unsigned long           dwLink
)
{
    OleOutlineDocPtr pOleOutlineDoc =
            ((OutlineDocOleUILinkContainerImplPtr)lpThis)->lpOleOutlineDoc;
	unsigned long dwNextLink = 0;

	OleDbgEnterInterface();

	dwNextLink = OleOutlineContainerDocGetNextLink(
			pOleOutlineDoc,
			dwLink
	);

    return dwNextLink;
}


//#pragma segment OleOutlineContainerDocIntSeg
STDMETHODIMP IOleOutlineDocLinkContSetLinkUpdateOptions(
        LPOLEUILINKCONTAINER    lpThis,
        unsigned long           dwLink,
        unsigned long           dwUpdateOpt
)
{
    OleOutlineDocPtr pOleOutlineDoc =
            ((OutlineDocOleUILinkContainerImplPtr)lpThis)->lpOleOutlineDoc;
    OleContainerLinePtr pContainerLine = (OleContainerLinePtr)dwLink;
    LPOLELINK pOleLink;
    HRESULT hrErr;

	OleDbgEnterInterface();

    ASSERTCOND(pContainerLine);

    pOleLink = (LPOLELINK)OleContainerLineGetOleObject(
            pContainerLine,
            &IID_IOleLink
    );

    if (! pOleLink) {
        return ResultFromScode(E_FAIL);
    }

    hrErr = pOleLink->lpVtbl->SetUpdateOptions(
            pOleLink,
            dwUpdateOpt
    );

    OleStdRelease((LPUNKNOWN)pOleLink);

    return hrErr;
}


//#pragma segment OleOutlineContainerDocIntSeg
STDMETHODIMP IOleOutlineDocLinkContGetLinkUpdateOptions(
        LPOLEUILINKCONTAINER    	lpThis,
        unsigned long               dwLink,
        unsigned long*              lpdwUpdateOpt
)
{
    OleOutlineDocPtr pOleOutlineDoc =
            ((OutlineDocOleUILinkContainerImplPtr)lpThis)->lpOleOutlineDoc;
    OleContainerLinePtr pContainerLine = (OleContainerLinePtr)dwLink;
    LPOLELINK pOleLink;
    HRESULT hrErr;

    ASSERTCOND(pContainerLine);

    pOleLink = (LPOLELINK)OleContainerLineGetOleObject(
            pContainerLine,
            &IID_IOleLink
    );

    if (! pOleLink) {
        return ResultFromScode(E_FAIL);
    }

    hrErr = pOleLink->lpVtbl->GetUpdateOptions(
            pOleLink,
            lpdwUpdateOpt
    );

    OleStdRelease((LPUNKNOWN)pOleLink);

    return hrErr;
}


//#pragma segment OleOutlineContainerDocIntSeg
STDMETHODIMP IOleOutlineDocLinkContSetLinkSource(
        LPOLEUILINKCONTAINER	lpThis,
		unsigned long			dwLink,
        char*					lpszDisplayName,
		unsigned long			lenFileName,		
        unsigned long*			lpchEaten,
		Boolean					fValidateSource
)
{
    OleOutlineDocPtr pOleOutlineDoc =
            ((OutlineDocOleUILinkContainerImplPtr)lpThis)->lpOleOutlineDoc;
    OleContainerLinePtr pContainerLine = (OleContainerLinePtr)dwLink;
    SCODE       sc = S_OK;
    HRESULT     hrErr = NOERROR;
    LPOLELINK   lpOleLink = NULL;
    LPBC        lpbc = NULL;
    LPMONIKER   lpmk = NULL;
	LPOLEOBJECT lpOleObj = NULL;
	CLSID       clsid = CLSID_NULL;

	OleDbgEnterInterface();

	OleContainerLineSetLinkUnavailable(pContainerLine, true);
	
	if (fValidateSource) {

        /* OLE2NOTE: validate the link source by parsing the string
        **    into a Moniker. if this is successful, then the string is
        **    valid.
        */
        hrErr = CreateBindCtx(0, (LPBC *)&lpbc);
        if (hrErr != NOERROR) {
            sc = GetScode(hrErr);   // ERROR: OOM
            goto cleanup;
        }

		hrErr = MkParseDisplayName(
				lpbc, lpszDisplayName, lpchEaten, (LPMONIKER *)&lpmk);

		if (hrErr != NOERROR) {
			sc = GetScode(hrErr);	// ERROR in parsing moniker!
			goto cleanup;
		}
        /* OLE2NOTE: the link source was validated; it successfully
        **    parsed into a Moniker. we can set the source of the link
        **    directly with this Moniker. if we want the link to be
        **    able to know the correct class for the new link source,
        **    we must bind to the moniker and get the CLSID. if we do
        **    not do this then methods like IOleObject::GetUserType
        **    will return nothing (NULL strings).
        */

        hrErr = lpmk->lpVtbl->BindToObject(lpmk,lpbc,NULL,&IID_IOleObject,(void**)&lpOleObj);
		if (hrErr == NOERROR) {
			hrErr = lpOleObj->lpVtbl->GetUserClassID(
							lpOleObj,
							(CLSID *)&clsid);
			OleStdRelease((LPUNKNOWN)lpOleObj);
			OleContainerLineSetLinkUnavailable(pContainerLine, false);			
		}
		else
			OleContainerLineSetLinkUnavailable(pContainerLine, true);			
	}
	else {
		LPMONIKER	lpmkFile = NULL;
		LPMONIKER	lpmkItem = NULL;
		char		szName[kMaxPathLen];
		
		strncpy(szName, lpszDisplayName, (int)lenFileName);
		szName[lenFileName] = '\0';
		
		CreateFileMoniker(szName, (LPMONIKER *)&lpmkFile);
			
		if (!lpmkFile)
			goto cleanup;

        if (strlen(lpszDisplayName) > (int)lenFileName) {	// have item name
            strcpy(szName, lpszDisplayName + lenFileName + 1);
		
            CreateItemMoniker(
					OLESTDDELIM,
					szName,
					(LPMONIKER *)&lpmkItem);
			
            if (!lpmkItem) {
                OleStdRelease((LPUNKNOWN)lpmkFile);
                goto cleanup;
            }
			
			CreateGenericComposite(lpmkFile, lpmkItem, (LPMONIKER *)&lpmk);
	
			if (lpmkFile)
				OleStdRelease((LPUNKNOWN)lpmkFile);
			if (lpmkItem)
				OleStdRelease((LPUNKNOWN)lpmkItem);
		
			if (!lpmk)
				goto cleanup;
		}
		else
			lpmk = lpmkFile;
	}
	
    lpOleLink = (LPOLELINK)OleContainerLineGetOleObject(
            pContainerLine,
            &IID_IOleLink
    );

    if (! lpOleLink) {
        OleDbgAssert(lpOleLink != NULL);
        sc = E_FAIL;
        goto cleanup;
    }

    if (lpmk) {

        hrErr = lpOleLink->lpVtbl->SetSourceMoniker(
                lpOleLink, lpmk, (REFCLSID)&clsid);

        if (hrErr != NOERROR) {
            OleDbgOutHResult("IOleLink::SetSourceMoniker returned",hrErr);
            sc = GetScode(hrErr);
            goto cleanup;
        }

    } else {
        /* OLE2NOTE: the link source was NOT validated; it was NOT
        **    successfully parsed into a Moniker. we can only set the
        **    display name string as the source of the link. this link
        **    is not able to bind.
        */
        hrErr = lpOleLink->lpVtbl->SetSourceDisplayName(
                lpOleLink, (const char*)lpszDisplayName);

        if (hrErr != NOERROR) {
            OleDbgOutHResult("IOleLink::SetSourceDisplayName returned",hrErr);
            sc = GetScode(hrErr);
            goto cleanup;
        }
    }

cleanup:
    if (lpOleLink)
        OleStdRelease((LPUNKNOWN)lpOleLink);
    if (lpmk)
        OleStdRelease((LPUNKNOWN)lpmk);
    if (lpbc)
        OleStdRelease((LPUNKNOWN)lpbc);

	LineListUpdateView(((OutlineDocPtr)pOleOutlineDoc)->m_LineList);

    OLEDBG_END2
    return ResultFromScode(sc);
}


//#pragma segment OleOutlineContainerDocIntSeg
STDMETHODIMP IOleOutlineDocLinkContGetLinkSource(
        LPOLEUILINKCONTAINER    lpThis,
        unsigned long       	dwLink,
        char**              	lplpszDisplayName,
        unsigned long*      	lplenFileName,
        char**              	lplpszFullLinkType,
        char**              	lplpszShortLinkType,
        Boolean*            	lpfSourceAvailable,
        Boolean*            	lpfIsSelected
)
{
    OleOutlineDocPtr pOleOutlineDoc =
            ((OutlineDocOleUILinkContainerImplPtr)lpThis)->lpOleOutlineDoc;
    OleContainerLinePtr pContainerLine = (OleContainerLinePtr)dwLink;
    LPOLELINK       lpOleLink = NULL;
    LPOLEOBJECT     lpOleObj = NULL;
    LPMONIKER       lpmk = NULL;
    LPMONIKER       lpmkFirst = NULL;
    LPBC            lpbc = NULL;
    SCODE           sc = S_OK;
    HRESULT         hrErr;

	OleDbgEnterInterface();

    /* OLE2NOTE: we must make sure to set all out parameters to NULL. */
    *lplpszDisplayName  = NULL;
    *lplpszFullLinkType = NULL;
    *lplpszShortLinkType= NULL;
    *lplenFileName      = 0;
    *lpfSourceAvailable = !OleContainerLineIsLinkUnavailable(pContainerLine);

    ASSERTCOND(pContainerLine);

    lpOleLink = (LPOLELINK)OleContainerLineGetOleObject(
            pContainerLine,
            &IID_IOleLink
    );

    if (! lpOleLink) {
        return ResultFromScode(E_FAIL);
    }

    hrErr = lpOleLink->lpVtbl->GetSourceMoniker(
            lpOleLink,
            (LPMONIKER *)&lpmk
    );

    if (hrErr == NOERROR) {
        /* OLE2NOTE: the link has the Moniker form of the link source;
        **    this is therefore a validated link source. if the first
        **    part of the Moniker is a FileMoniker, then we need to
        **    return the length of the filename string. we need to
        **    return the ProgID associated with the link source as the
        **    "lpszShortLinkType". we need to return the
        **    FullUserTypeName associated with the link source as the
        **    "lpszFullLinkType".
        */
		
        lpOleObj = (LPOLEOBJECT)OleStdQueryInterface(
                (LPUNKNOWN)lpOleLink, &IID_IOleObject);
        if (lpOleObj) {
            lpOleObj->lpVtbl->GetUserType(
                    lpOleObj,
                    USERCLASSTYPE_FULL,
                    lplpszFullLinkType
            );
            lpOleObj->lpVtbl->GetUserType(
                    lpOleObj,
                    USERCLASSTYPE_SHORT,
                    lplpszShortLinkType
            );
            OleStdRelease((LPUNKNOWN)lpOleObj);
        }
        *lplenFileName = OleStdGetLenFilePrefixOfMoniker(lpmk);
        lpmk->lpVtbl->Release(lpmk);
    }

    hrErr = lpOleLink->lpVtbl->GetSourceDisplayName(
            lpOleLink,
            lplpszDisplayName
    );

    OleStdRelease((LPUNKNOWN)lpOleLink);

    if (hrErr != NOERROR) {
        return hrErr;
    }

    if (lpfIsSelected) {
    	LinePtr	pLine = (LinePtr)pContainerLine;

        *lpfIsSelected = LineListIsLineSelected(pLine->m_LineList, pLine);
	}
	
    return NOERROR;
}


//#pragma segment OleOutlineContainerDocIntSeg
STDMETHODIMP IOleOutlineDocLinkContOpenLinkSource(
        LPOLEUILINKCONTAINER    lpThis,
        unsigned long           dwLink
)
{
    OleOutlineDocPtr pOleOutlineDoc =
            ((OutlineDocOleUILinkContainerImplPtr)lpThis)->lpOleOutlineDoc;
    OleContainerLinePtr pContainerLine = (OleContainerLinePtr)dwLink;
    SCODE sc = S_OK;

	OleDbgEnterInterface();

    ASSERTCOND(pContainerLine);

	SetCursor(*gWatchCursor);	

	TRY
	{
    	OleContainerLineDoVerb(pContainerLine, OLEIVERB_SHOW, true, false);
    }
    CATCH
    {
    	sc = E_FAIL;

		NO_PROPAGATE;
    }
    ENDTRY;
	
	SetCursor(&qd.arrow);	
	
	OleContainerLineSetLinkUnavailable(pContainerLine, (Boolean)(sc != S_OK));

    return ResultFromScode(sc);
}


//#pragma segment OleOutlineContainerDocIntSeg
STDMETHODIMP IOleOutlineDocLinkContUpdateLink(
        LPOLEUILINKCONTAINER    lpThis,
        unsigned long           dwLink,
		Boolean					fErrorMessage,
		Boolean					fErrorAction		// ignore if fErrorMessage
													//		is FALSE
)
{
    OleOutlineDocPtr pOleOutlineDoc =
            ((OutlineDocOleUILinkContainerImplPtr)lpThis)->lpOleOutlineDoc;
    OleContainerLinePtr pContainerLine = (OleContainerLinePtr)dwLink;
    SCODE sc = S_OK;
    HRESULT hrErr;

	OleDbgEnterInterface();

    ASSERTCOND(pContainerLine);

    OleContainerLineLoadOleObject(pContainerLine);

	if (!fErrorMessage) {
		hrErr = pContainerLine->site.m_pOleObj->lpVtbl->IsUpToDate(
				pContainerLine->site.m_pOleObj
		);
	}	
	
	if (hrErr != NOERROR) {
		hrErr = pContainerLine->site.m_pOleObj->lpVtbl->Update(
				pContainerLine->site.m_pOleObj
		);
		
		if (GetScode(hrErr) == MK_E_MUSTBOTHERUSER) {
			LPOLELINK	pOleLink = nil;
			
			hrErr = pContainerLine->site.m_pOleObj->lpVtbl->QueryInterface(
						pContainerLine->site.m_pOleObj,
						&IID_IOleLink,
						(void**)&pOleLink);
						
			if (pOleLink) {
				IBindCtx	*pbc = nil;
				BIND_OPTS	bindopts;
				
	            CreateBindCtx(0, (LPBC *)&pbc);
	
	            bindopts.cbStruct = sizeof(BIND_OPTS);
	            pbc->lpVtbl->GetBindOptions(pbc, &bindopts);
	            bindopts.grfFlags |= BIND_MAYBOTHERUSER;
	            pbc->lpVtbl->SetBindOptions(pbc, &bindopts);
	
	            hrErr = pOleLink->lpVtbl->Update(pOleLink, pbc);
	
				pOleLink->lpVtbl->Release(pOleLink);
				pbc->lpVtbl->Release(pbc);
			}
		}		
	}

	OleContainerLineSetLinkUnavailable(pContainerLine, (Boolean)(hrErr != NOERROR));
		
    if (hrErr != NOERROR) {
        sc = GetScode(hrErr);
		if (fErrorMessage) {
#if LATER		
			OleContainerLineProcessOleRunError(
				pContainerLine, hrErr, fErrorAction, FALSE /* menu involved */);
#else				
			ASSERTCONDSZ(0, "IOleOutlineDocLinkContUpdateLink");
#endif			
		}
    }

	LineListUpdateView(((OutlineDocPtr)pOleOutlineDoc)->m_LineList);

    return ResultFromScode(sc);
}


//#pragma segment OleOutlineContainerDocIntSeg
/* IOleOutlineDocLinkContCancelLink
** --------------------------------
**    Convert the link to a static picture.
**
**    OLE2NOTE: OleCreateStaticFromData can be used to create a static
**    picture object.
*/
STDMETHODIMP IOleOutlineDocLinkContCancelLink(
        LPOLEUILINKCONTAINER    lpThis,
        unsigned long           dwLink
)
{
    OleOutlineDocPtr	pOleOutlineDoc =
            				((OutlineDocOleUILinkContainerImplPtr)lpThis)->lpOleOutlineDoc;
    DocumentPtr			pDoc;
    OleContainerLinePtr pContainerLine = (OleContainerLinePtr)dwLink;
    LineListPtr         pLineList = ((OutlineDocPtr)pOleOutlineDoc)->m_LineList;
    char                szStgName[CWCSTORAGENAME];
    OleContainerLinePtr pNewContainerLine = nil;
    LPDATAOBJECT        pSrcDataObj;
    LPOLELINK           pOleLink;
    Cell				theCell;

	OleDbgEnterInterface();

    LineListFindLineCell(pLineList, (LinePtr)pContainerLine, &theCell);

    /* we will first break the connection of the link to its source. */
    pOleLink = (LPOLELINK)OleContainerLineGetOleObject(
            pContainerLine,
            &IID_IOleLink
    );
    if (pOleLink) {
        pOleLink->lpVtbl->SetSourceMoniker(
                pOleLink, NULL, (REFCLSID)&CLSID_NULL);
        OleStdRelease((LPUNKNOWN)pOleLink);
    }

    pSrcDataObj = (LPDATAOBJECT)OleContainerLineGetOleObject(
            pContainerLine,&IID_IDataObject);
    if (! pSrcDataObj)
        goto error;

    OleOutlineContainerDocNewStorageName(pOleOutlineDoc, szStgName);

    pNewContainerLine = OleContainerLineCreateFromData(
            pOleOutlineDoc,
            pSrcDataObj,
            OLECREATEFROMDATA_STATIC,
            0,		/* no special format required */
            FALSE,  /* fDisplayAsIcon */
            NULL,   /* hMetaPict */
            szStgName
    );

    OleStdRelease((LPUNKNOWN)pSrcDataObj);

    if (! pNewContainerLine)
        goto error;

	LineDispose((LinePtr)pContainerLine);
	LineListSetLine(pLineList, theCell.v, (LinePtr)pNewContainerLine);
		
	pDoc = (DocumentPtr)pOleOutlineDoc;
	
	ASSERTCOND(pDoc->vtbl->m_SetDirtyProcPtr != nil);
    pDoc->vtbl->m_SetDirtyProcPtr(pDoc, true);

	return ResultFromScode(NOERROR);

error:
    DebugStr("\pCould not break the link.");
    return ResultFromScode(E_FAIL);

}

// OLDNAME: OleOutlineContainerDoc.c
extern	ApplicationPtr		gApplication;
extern CursHandle			gWatchCursor;
extern OleApplicationPtr	gOleApp;


//#pragma segment OleOutlineContainerDocSeg
void OleOutlineContainerDocInit(OleOutlineDocPtr pOleOutlineDoc)
{
	OleOutlineDocIOleUILinkContainerInit(&pOleOutlineDoc->container.m_IOleUILinkContainer, pOleOutlineDoc);

	pOleOutlineDoc->container.m_VerbMenuRec.ConvertMenuID		= 0;
	pOleOutlineDoc->container.m_VerbMenuRec.ConvertMenuItem		= 0;
	pOleOutlineDoc->container.m_VerbMenuRec.verbCount			= 0;
	pOleOutlineDoc->container.m_VerbMenuRec.verbRec				= nil;

	pOleOutlineDoc->container.m_nNextObjNo						= 0;
	
	pOleOutlineDoc->container.m_pStorage						= nil;
}

//#pragma segment OleOutlineContainerDocSeg
void OleOutlineContainerDocDispose(OleOutlineDocPtr pOleOutlineDoc)
{
	if (pOleOutlineDoc->container.m_VerbMenuRec.verbRec)
	{
		OleStdFree(pOleOutlineDoc->container.m_VerbMenuRec.verbRec);
		pOleOutlineDoc->container.m_VerbMenuRec.verbRec = nil;
	}

	if (pOleOutlineDoc->container.m_pStorage != nil)
	{
		OleStdRelease((LPUNKNOWN)pOleOutlineDoc->container.m_pStorage);
		pOleOutlineDoc->container.m_pStorage = nil;
	}
}

//#pragma segment OleOutlineContainerDocSeg
void OleOutlineContainerDocDoNew(OleOutlineDocPtr pOleOutlineDoc)
{
	FSSpec			fs;
	HRESULT			hrErr;

	{
		StringHandle	h;
		
		h = GetString(kTemporaryFileNamePrefix_STR);
		ASSERTCOND(h != nil);
		HLock((Handle)h);
		HNoPurge((Handle)h);
		
		CreateTemporaryFSSpec(*h, &fs);
		
		HPurge((Handle)h);
		HUnlock((Handle)h);
	}
	
	// create a temporary file based storage for the new document
	hrErr = StgCreateDocfileFSp(
					&fs,
					'    ',		// may want to set this to something?
					'    ',
					smSystemScript,
					STGM_READWRITE | STGM_TRANSACTED | STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_DELETEONRELEASE,
					0,
					&pOleOutlineDoc->container.m_pStorage);
	ASSERTNOERROR(hrErr);
	FailOleErr(hrErr);
}

//#pragma segment OleOutlineContainerDocSeg
YNCResult OleOutlineContainerDocDoClose(OleOutlineDocPtr pOleOutlineDoc, Boolean askUserToSave, YNCResult defaultAnswer, Boolean quitting)
{
	Boolean		canClose;
	
	OleOutlineDocAddRef(pOleOutlineDoc);
	OleAppAddRef(gOleApp);
	
	canClose = LineListDoClose(((OutlineDocPtr)pOleOutlineDoc)->m_LineList);

	OleOutlineDocRelease(pOleOutlineDoc);
	OleAppRelease(gOleApp);

	return canClose ? yesResult : cancelResult;
}

//#pragma segment OleOutlineContainerDocSeg
void OleOutlineContainerDocPasteSpecialDlg(OleOutlineDocPtr pOleOutlineDoc)
{
	HRESULT				hrErr;
	LPDATAOBJECT		lpDataObj = NULL;
	OLEUIPASTESPECIAL	psDialog;
	int					result;
	

	bzero(&psDialog, sizeof(psDialog));
	hrErr = OleGetClipboard(&lpDataObj);
	if (hrErr != NOERROR)
		return;

	psDialog.cbStruct        = sizeof(OLEUIPASTESPECIAL);
	psDialog.dwFlags         = PSF_SELECTPASTE | PSF_SHOWHELP;
	psDialog.arrPasteEntries = gOleApp->m_arrPasteEntries;
	psDialog.cPasteEntries   = gOleApp->m_nPasteEntries;
	psDialog.lpSrcDataObj    = lpDataObj;
	psDialog.arrLinkTypes    = gOleApp->m_arrLinkTypes;
	psDialog.cLinkTypes      = gOleApp->m_nLinkTypes;
	psDialog.lpfnHook 		 = OleOutlineDocUIDialogHook;
	
	OleDocEnableDialog(&pOleOutlineDoc->m_OleDoc);

	result = OleUIPasteSpecial(&psDialog);
	
	// only resume inplace if the result is different than OLEUI_SUCCESS
	OleDocDisableDialog(&pOleOutlineDoc->m_OleDoc, (Boolean)(result != OLEUI_SUCCESS));

	if (result == OLEUI_SUCCESS)
	{
			OleDocPasteFormatFromData(
					&pOleOutlineDoc->m_OleDoc,
					gOleApp->m_arrPasteEntries[psDialog.nSelectedIndex].fmtetc.cfFormat,
					lpDataObj,
					psDialog.fLink,
					(psDialog.dwFlags & PSF_CHECKDISPLAYASICON ? TRUE : FALSE),
				psDialog.hMetaPict);
				
		if (psDialog.hMetaPict)
			OleUIPictIconFree(psDialog.hMetaPict);
	}

	OleStdRelease((LPUNKNOWN)lpDataObj);
}

//#pragma segment OleOutlineContainerDocSeg
void OleOutlineContainerDocInsertDlg(OleOutlineDocPtr pOleOutlineDoc)
{
	OLEUIINSERTOBJECT	ioDialog;
	FSSpec				fssFile;

	bzero(&ioDialog, sizeof(ioDialog));
	ioDialog.cbStruct = sizeof(OLEUIINSERTOBJECT);
	ioDialog.dwFlags  = IOF_SELECTCREATENEW;
	ioDialog.pfssFile = &fssFile;
	ioDialog.lpfnHook = OleOutlineDocUIDialogHook;
	
	if (OleUIInsertObject(&ioDialog) == OLEUI_SUCCESS)
	{
		char	storageName[CWCSTORAGENAME];

		SetCursor(*gWatchCursor);
		
		OleOutlineContainerDocNewStorageName(pOleOutlineDoc, storageName);

		LineListNewContainerLine(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, pOleOutlineDoc, ioDialog.dwFlags, &ioDialog.clsid, ioDialog.pfssFile, ioDialog.hMetaPict, pOleOutlineDoc->container.m_pStorage, storageName);

		// make sure we set dirty
		ASSERTCOND(((DocumentPtr)pOleOutlineDoc)->vtbl->m_SetDirtyProcPtr != nil);
		(*((DocumentPtr)pOleOutlineDoc)->vtbl->m_SetDirtyProcPtr)((DocumentPtr)pOleOutlineDoc, true);
	}
	
	if (ioDialog.hMetaPict)
	{
		KillPicture(ioDialog.hMetaPict);
		ioDialog.hMetaPict = nil;
	}
}

//#pragma segment OleOutlineContainerDocSeg
void OleOutlineContainerDocEditLinksDlg(OleOutlineDocPtr pOleOutlineDoc)
{
	OLEUIEDITLINKS	elDialog;

	bzero(&elDialog, sizeof(elDialog));
	elDialog.lpOleUILinkContainer = (LPOLEUILINKCONTAINER)&pOleOutlineDoc->container.m_IOleUILinkContainer;
	elDialog.cbStruct = sizeof(OLEUIEDITLINKS);
	elDialog.dwFlags = ELF_SHOWHELP;
	elDialog.lpfnHook = OleOutlineDocUIDialogHook;

	OleDocEnableDialog(&pOleOutlineDoc->m_OleDoc);

	OleUIEditLinks(&elDialog);
	
	OleDocDisableDialog(&pOleOutlineDoc->m_OleDoc, true);
}

//#pragma segment OleOutlineContainerDocSeg
void OleOutlineContainerDocConvert(
		OleOutlineDocPtr 	pOleOutlineDoc,
		Boolean				fServerNotRegistered)
{
	LineListPtr		pLineList = ((OutlineDocPtr)pOleOutlineDoc)->m_LineList;
	LinePtr			pLine;
	OleContainerLinePtr	pContainerLine = NULL;
	LineRangeRec	LineRange;

	if (LineListGetSelection(pLineList, &LineRange) != 1)
		return;
		
	pLine = LineListGetLine(pLineList, LineRange.m_nStartLine);
	ASSERTCOND(pLine != nil);
	FailNIL(pLine);
	
	ASSERTCOND(LineGetType(pLine) == kContainerLineType);
	pContainerLine = (OleContainerLinePtr)pLine;
	
	OleContainerLineDoConvert(pContainerLine, fServerNotRegistered);
}

void OleOutlineContainerDocUpdateLinksDlg(OleOutlineDocPtr pOleOutlineDoc)
{
	LPOLEUILINKCONTAINER	pLinkCont;
	
	OleOutlineDocQueryInterface(pOleOutlineDoc, &IID_IOleUILinkContainer, (void**)&pLinkCont);
	if (pLinkCont) {
//		OleUIUpdateLinks(pLinkCont, 10);
		pLinkCont->lpVtbl->Release(pLinkCont);
	}
}

/* OleOutlineContainerDocPasteOutlineData
 * --------------------------------------
 *
 *      Load the lines stored in a lpSrcStg (stored in CF_CNTROUTL format)
 *  into the document.
 *
 * Return the number of items added
 */
HRESULT OleOutlineContainerDocPasteOutlineData(
        OleOutlineDocPtr		pOleOutlineDoc,
        LPSTORAGE               pStorage
)
{
	ApplicationPtr			pApp = gApplication;
    volatile DocumentPtr	pTempDoc;
	LineListPtr				pTempLineList;
	HRESULT					hrErr;

	hrErr = NOERROR;

	TRY
	{
	    // create a temp document that will be used to load the pStorage data.
    	pTempDoc = OleOutlineAppCreateDataXferDoc((OleOutlineAppPtr)gApplication);
	    if ( ! pTempDoc )
	        return ResultFromScode(E_FAIL);

		ASSERTCOND(pTempDoc->vtbl->m_DoNewProcPtr != nil);
		(*pTempDoc->vtbl->m_DoNewProcPtr)(pTempDoc);
	
	    hrErr = OleOutlineDocLoadFromStorage((OleOutlineDocPtr)pTempDoc, pStorage);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);

		pTempLineList = ((OutlineDocPtr)pTempDoc)->m_LineList;
		
	    /* copy all lines from the SrcDoc to the DestDoc. */
		LineListSelectRange(pTempLineList, nil);		// select all
	    LineListCopyToDoc(pTempLineList, (DocumentPtr)pOleOutlineDoc);
	
	    if (pTempDoc) {            // destroy temporary document.
	    	ASSERTCOND(pTempDoc->vtbl->m_DoCloseProcPtr != nil);
	        (*pTempDoc->vtbl->m_DoCloseProcPtr)(pTempDoc, false, noResult, false);
		}
	}
	CATCH
	{
	    if (pTempDoc) {          // destroy temporary document.
	    	ASSERTCOND(pTempDoc->vtbl->m_DoCloseProcPtr != nil);
	        (*pTempDoc->vtbl->m_DoCloseProcPtr)(pTempDoc, false, noResult, false);
	    }

		if (IsOleFailure())
			hrErr = GetOleFailure();
		else
			hrErr = ResultFromScode(E_FAIL);

		NO_PROPAGATE;
	}
	ENDTRY

	return hrErr;
}


/* ContainerDoc_PasteOleObject
** ---------------------------
**
**    Embed or link an OLE object. the source of the data is a pointer
**    to an IDataObject. normally this lpSrcDataObj comes from the
**    clipboard after call OleGetClipboard.
**
**    if fLink == TRUE, an OLE Link object will be created. otherwise
**    an embedded OLE object will be created. a CONTAINERLINE object is
**    created to manage the OLE object. this CONTAINERLINE is added to the
**    ContainerDoc after line nIndex.
**
*/
//#pragma segment OleContainerDocSeg
void OleOutlineContainerDocPasteOleObject(
        OleOutlineDocPtr		pOleOutlineDoc,
        LPDATAOBJECT            pSrcDataObj,
		unsigned long			dwCreateType,
		ResType					cfFormat,
        Boolean                 fDisplayAsIcon,
        PicHandle               hMetaPict
)
{
	char	storageName[CWCSTORAGENAME];
		
	OleOutlineContainerDocNewStorageName(pOleOutlineDoc, storageName);

	LineListNewContainerLineFromData(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, pOleOutlineDoc, pSrcDataObj, dwCreateType, cfFormat, fDisplayAsIcon, hMetaPict, storageName);

	// make sure we set dirty
	ASSERTCOND(((DocumentPtr)pOleOutlineDoc)->vtbl->m_SetDirtyProcPtr != nil);
	(*((DocumentPtr)pOleOutlineDoc)->vtbl->m_SetDirtyProcPtr)((DocumentPtr)pOleOutlineDoc, true);
}

/* OleOutlineContainerDocNewStorageName
** ---------------------------
**    Generate the next unused name for a sub-storage to be used by an
**    OLE object. The ContainerDoc keeps a counter. The storages for
**    OLE objects are simply numbered (eg. Obj 0, Obj 1). A "long"
**    integer worth of storage names should be more than enough than we
**    will ever need.
**
**    NOTE: when an OLE object is transfered via drag/drop or the
**    clipboard, we attempt to keep the currently assigned name for the
**    object (if not currently in use). thus it is possible that an
**    object with a the next default name (eg. "Obj 5") already exists
**    in the current document if an object with this name was privously
**    transfered (pasted or dropped). we therefore loop until we find
**    the next lowest unused name.
*/
//#pragma segment OleOutlineContainerDocumentSeg
void OleOutlineContainerDocNewStorageName(OleOutlineDocPtr pOleOutlineDoc, char* pStorageName)
{
	Handle	h;
	
	h = Get1Resource('CSTR', kObjectNamePrefix_CSTR);
	ASSERTCOND(h != nil);
	HNoPurge(h);
	
	do
	{
		sprintf(pStorageName, "%s %ld", *h, ++pOleOutlineDoc->container.m_nNextObjNo);
	}
	while(OleOutlineContainerDocStorageExists(pOleOutlineDoc, pStorageName));
	
	HPurge(h);
}

/* OleOutlineContainerDocStorageExists
** ---------------------------
**    Check if a given StgName is already in use.
*/
//#pragma segment OleOutlineContainerDocumentSeg
Boolean OleOutlineContainerDocStorageExists(OleOutlineDocPtr pOleOutlineDoc, char* pStorageName)
{
	LPSTORAGE	pStorage;
	LPSTORAGE	pChildStorage;
	HRESULT		hrErr;
	
	pStorage = pOleOutlineDoc->container.m_pStorage;
	ASSERTCOND(pStorage != nil);
	
	hrErr = pStorage->lpVtbl->OpenStorage(
				pStorage,
				pStorageName,
				NULL,
				STGM_READWRITE | STGM_TRANSACTED | STGM_SHARE_EXCLUSIVE,
				NULL,
				0,
				&pChildStorage
			);
	
	// if STG_E_FILENOTFOUND is return, storage does not exist
	if (hrErr == ResultFromScode(STG_E_FILENOTFOUND))
		return false;
		
	if (pChildStorage != nil)
		OleStdRelease((LPUNKNOWN)pChildStorage);
	
	return true;
}

//#pragma segment OleOutlineContainerDocumentSeg
LPSTORAGE OleOutlineDocGetStorage(OleOutlineDocPtr pOleOutlineDoc)
{
	return pOleOutlineDoc->container.m_pStorage;
}

//#pragma segment OleOutlineContainerDocumentSeg
HRESULT OleOutlineContainerDocGetItemObject(OleOutlineDocPtr pOleOutlineDoc, char* pszItem, unsigned long SpeedNeeded, LPBINDCTX pbc, REFIID riid, void** ppvObject)
{
	return LineListGetItemObject(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, pszItem, SpeedNeeded, riid, ppvObject);
}

//#pragma segment OleOutlineContainerDocumentSeg
HRESULT OleOutlineContainerDocIsItemRunning(OleOutlineDocPtr pOleOutlineDoc, char* pszItem)
{
	return LineListIsItemRunning(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, pszItem);
}

//#pragma segment OleOutlineContainerDocumentSeg
HRESULT OleOutlineContainerDocGetItemStorage(OleOutlineDocPtr pOleOutlineDoc, char* pszItem, void** lplpvStorage)
{
	return LineListGetItemStorage(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, pszItem, lplpvStorage);
}

//#pragma segment OleOutlineContainerDocumentSeg
void OleOutlineContainerDocInformAllOleObjectsDocRenamed(OleOutlineDocPtr pOleOutlineDoc, LPMONIKER pmkDoc)
{
	LineListInformAllOleObjectsDocRenamed(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, pmkDoc);
}

//#pragma segment OleOutlineContainerDocumentSeg
unsigned long OleOutlineContainerDocGetNextLink(OleOutlineDocPtr pOleOutlineDoc, unsigned long dwLink)
{
	return LineListGetNextLink(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, dwLink);
}

//#pragma segment OleOutlineContainerDocumentSeg
/* OleOutlineContainerDocUnloadAllOleObjectsOfClass
** ------------------------------------------------
**    Unload all OLE objects of a particular class. this is necessary
**    when a class level "ActivateAs" (aka. TreatAs) is setup. the user
**    can do this with the Convert dialog. for the TreatAs to take
**    effect, all objects of the class have to loaded and reloaded.
*/
void OleOutlineContainerDocUnloadAllOleObjectsOfClass(
        OleOutlineDocPtr 	pOleOutlineDoc,
        REFCLSID            rClsid,
        unsigned long		dwSaveOption
)
{
    LineListPtr  pLineList = ((OutlineDocPtr)pOleOutlineDoc)->m_LineList;
    short       i;
    LinePtr     pLine;
    CLSID       clsid;
    HRESULT     hrErr;
    short		lineCount;

	lineCount = LineListGetCount(pLineList);
	
    for (i = 0; i < lineCount; i++) {
        pLine = LineListGetLine(pLineList, i);

        if (pLine && (LineGetType(pLine) == kContainerLineType)) {

            OleContainerLinePtr pContainerLine = (OleContainerLinePtr)pLine;

            if (! pContainerLine->site.m_pOleObj)
                continue;       // this object is NOT loaded

            hrErr = pContainerLine->site.m_pOleObj->lpVtbl->GetUserClassID(
                    pContainerLine->site.m_pOleObj,
                    &clsid
            );
            if (hrErr == NOERROR &&
                    ( IsEqualCLSID(&clsid,rClsid)
                      || IsEqualCLSID(rClsid, &CLSID_NULL) ) ) {
                OleContainerLineUnloadOleObject(pContainerLine, dwSaveOption);
            }
        }
    }
}

#if qOleInPlace
void OleOutlineContainerDocUpdateWindowPosition(OleOutlineDocPtr pOleOutlineDoc)
{
	short		index;
	LinePtr		pLine;
	
	index = 0;
	
	pLine = LineListGetLine(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, index);
	while(pLine != nil)
	{
		if (LineListLineIsVisible(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, pLine))
		{
			if (LineGetType(pLine) == kContainerLineType)
			{
				if (OleContainerSiteIsUIVisible(&((OleContainerLinePtr)pLine)->site))
					OleInPlaceContainerSiteUpdatePosition(&((OleContainerLinePtr)pLine)->site);
			}
		}
	
		index++;
		pLine = LineListGetLine(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, index);
	}
}

void OleOutlineContainerDocScrollRect(OleOutlineDocPtr pOleOutlineDoc, short distHoriz, short distVert)
{
	short		index;
	LinePtr		pLine;
	
	index = 0;
	
	pLine = LineListGetLine(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, index);
	while(pLine != nil)
	{
		if (LineGetType(pLine) == kContainerLineType)
		{
			if (OleContainerSiteIsUIVisible(&((OleContainerLinePtr)pLine)->site))
				OleInPlaceContainerSiteUpdatePosition(&((OleContainerLinePtr)pLine)->site);
		}
	
		index++;
		pLine = LineListGetLine(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, index);
	}
}

Boolean OleOutlineContainerDocIsIPActive(OleOutlineDocPtr pOleOutlineDoc)
{
	short		index;
	LinePtr		pLine;
	
	index = 0;
	
	pLine = LineListGetLine(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, index);
	while(pLine != nil)
	{
		if (LineListLineIsVisible(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, pLine))
		{
			if (LineGetType(pLine) == kContainerLineType)
				return OleInPlaceContainerSiteIsIPActive(&((OleContainerLinePtr)pLine)->site);
		}
	
		index++;
		pLine = LineListGetLine(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, index);
	}

	return false;
}

Boolean OleOutlineContainerDocIsUIActive(OleOutlineDocPtr pOleOutlineDoc)
{
	LineRangeRec	lrSel;
	LinePtr			pLine;

	if (LineListGetSelection(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, &lrSel) == 1)
	{
		pLine = LineListGetLine(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, lrSel.m_nStartLine);
		if (pLine != nil)
		{
			if (LineGetType(pLine) == kContainerLineType)
				return OleInPlaceContainerSiteIsUIActive(&((OleContainerLinePtr)pLine)->site);
		}
	}

	return false;
}

Boolean OleOutlineContainerDocIsUIVisible(OleOutlineDocPtr pOleOutlineDoc)
{
	LineRangeRec	lrSel;
	LinePtr			pLine;

	if (LineListGetSelection(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, &lrSel) == 1)
	{
		pLine = LineListGetLine(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, lrSel.m_nStartLine);
		if (pLine != nil)
		{
			if (LineGetType(pLine) == kContainerLineType)
				return OleInPlaceContainerSiteIsUIVisible(&((OleContainerLinePtr)pLine)->site);
		}
	}

	return false;
}

void OleOutlineContainerDocDoIPDeactivate(OleOutlineDocPtr pOleOutlineDoc)
{
	short		index;
	LinePtr		pLine;
	
	index = 0;
	
	pLine = LineListGetLine(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, index);
	while(pLine != nil)
	{
		if (LineListLineIsVisible(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, pLine))
		{
			if (LineGetType(pLine) == kContainerLineType)
			{
				if (OleContainerSiteIsIPActive(&((OleContainerLinePtr)pLine)->site))
					OleInPlaceContainerSiteIPDeactivate(&((OleContainerLinePtr)pLine)->site);
			}
		}
	
		index++;
		pLine = LineListGetLine(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, index);
	}
}

void OleOutlineContainerDocDoUIDeactivate(OleOutlineDocPtr pOleOutlineDoc)
{
	short		index;
	LinePtr		pLine;
	
	index = 0;
	
	pLine = LineListGetLine(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, index);
	while(pLine != nil)
	{
		if (LineListLineIsVisible(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, pLine))
		{
			if (LineGetType(pLine) == kContainerLineType)
			{
				if (OleContainerSiteIsIPActive(&((OleContainerLinePtr)pLine)->site))
					OleInPlaceContainerSiteUIDeactivate(&((OleContainerLinePtr)pLine)->site);
			}
		}
	
		index++;
		pLine = LineListGetLine(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, index);
	}
}

#if qFrameTools
Boolean OleOutlineContainerDocFrameToolsEnabled(OleOutlineDocPtr pOleOutlineDoc)
{
	DocumentPtr		pDocument;

	pDocument = (DocumentPtr)pOleOutlineDoc;

	return pDocument->m_fShowFrameTools;
}
#endif

#endif

#if qOleInPlace

// OLDNAME: OleInPlaceContainerDocInt.c

static IOleInPlaceUIWindowVtbl	gOleInPlaceContainerUIWindowVtbl;

//#pragma segment OleInPlaceContainerDocIntInitSeg
void OleInPlaceContainerDocInitInterfaces(void)
{
	// OleInPlace::UIWindow method table
	{
		IOleInPlaceUIWindowVtbl*	p;
		
		p = &gOleInPlaceContainerUIWindowVtbl;
		
		p->QueryInterface			= IOleInPlaceContainerDocUIWindowQueryInterface;
		p->AddRef					= IOleInPlaceContainerDocUIWindowAddRef;
		p->Release					= IOleInPlaceContainerDocUIWindowRelease;
		p->GetWindow				= IOleInPlaceContainerDocUIWindowGetWindow;
		p->ContextSensitiveHelp		= IOleInPlaceContainerDocUIWindowContextSensitiveHelp;
		p->GetBorder				= IOleInPlaceContainerDocUIWindowGetBorder;
		p->RequestBorderSpace		= IOleInPlaceContainerDocUIWindowRequestBorderSpace;
		p->SetBorderSpace			= IOleInPlaceContainerDocUIWindowSetBorderSpace;
		p->SetActiveObject			= IOleInPlaceContainerDocUIWindowSetActiveObject;

		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}
}

//#pragma segment OleInPlaceContainerDocIntSeg
void OleInPlaceContainerIUIWindowInit(OleInPlaceContainerUIWindowImplPtr pOleInPlaceContainerUIWindowImpl, struct OleDocumentRec* pOleDoc)
{
	pOleInPlaceContainerUIWindowImpl->lpVtbl				= &gOleInPlaceContainerUIWindowVtbl;
	pOleInPlaceContainerUIWindowImpl->lpOleDoc				= pOleDoc;
	pOleInPlaceContainerUIWindowImpl->cRef					= 0;
}

//#pragma segment OleInPlaceContainerDocIntSeg
STDMETHODIMP IOleInPlaceContainerDocUIWindowQueryInterface(LPOLEINPLACEUIWINDOW lpThis, REFIID riid, void * * ppvObj)
{
	OleDocumentPtr		pOleDoc;
	
	OleDbgEnterInterface();
	
	pOleDoc = ((OleInPlaceContainerUIWindowImplPtr)lpThis)->lpOleDoc;

	return OleDocQueryInterface(pOleDoc, riid, ppvObj);
}

//#pragma segment OleInPlaceContainerDocIntSeg
STDMETHODIMP_(unsigned long) IOleInPlaceContainerDocUIWindowAddRef(LPOLEINPLACEUIWINDOW lpThis)
{
	OleDocumentPtr		pOleDoc;
	
	OleDbgEnterInterface();
	
	pOleDoc = ((OleInPlaceContainerUIWindowImplPtr)lpThis)->lpOleDoc;

	return OleDocAddRef(pOleDoc);
}

//#pragma segment OleInPlaceContainerDocIntSeg
STDMETHODIMP_(unsigned long) IOleInPlaceContainerDocUIWindowRelease(LPOLEINPLACEUIWINDOW lpThis)
{
	OleDocumentPtr		pOleDoc;
	
	OleDbgEnterInterface();
	
	pOleDoc = ((OleInPlaceContainerUIWindowImplPtr)lpThis)->lpOleDoc;

	return OleDocRelease(pOleDoc);
}

//#pragma segment OleInPlaceContainerDocIntSeg
STDMETHODIMP IOleInPlaceContainerDocUIWindowGetWindow(LPOLEINPLACEUIWINDOW lpThis, WindowPtr * lphwnd)
{
	OleDocumentPtr		pOleDoc;
	
	OleDbgEnterInterface();
	
	pOleDoc = ((OleInPlaceContainerUIWindowImplPtr)lpThis)->lpOleDoc;

	ASSERTCOND(lphwnd != nil);
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr != nil);
	*lphwnd = (*pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr)(pOleDoc->m_pIDoc);
	ASSERTCOND(*lphwnd != nil);

	return NOERROR;
}

//#pragma segment OleInPlaceContainerDocIntSeg
STDMETHODIMP IOleInPlaceContainerDocUIWindowContextSensitiveHelp(LPOLEINPLACEUIWINDOW lpThis, unsigned long fEnterMode)
{
	OleDbgEnterInterface();

Debugger();
	return ResultFromScode(E_NOTIMPL);
}

//#pragma segment OleInPlaceContainerDocIntSeg
STDMETHODIMP IOleInPlaceContainerDocUIWindowGetBorder(LPOLEINPLACEUIWINDOW lpThis, Rect * lprectBorder)
{
	OleDbgEnterInterface();

	return ResultFromScode(INPLACE_E_NOTOOLSPACE);
}

//#pragma segment OleInPlaceContainerDocIntSeg
STDMETHODIMP IOleInPlaceContainerDocUIWindowRequestBorderSpace(LPOLEINPLACEUIWINDOW lpThis, LPCBORDERWIDTHS lpborderwidths)
{
	OleDbgEnterInterface();

Debugger();
	return ResultFromScode(INPLACE_E_NOTOOLSPACE);
}

//#pragma segment OleInPlaceContainerDocIntSeg
STDMETHODIMP IOleInPlaceContainerDocUIWindowSetBorderSpace(LPOLEINPLACEUIWINDOW lpThis, LPCBORDERWIDTHS lpborderwidths)
{
	OleDocumentPtr		pOleDoc;
	
	OleDbgEnterInterface();

	pOleDoc = ((OleInPlaceContainerUIWindowImplPtr)lpThis)->lpOleDoc;

	return OleInPlaceContainerDocSetBorderSpace(pOleDoc, lpborderwidths);
}

//#pragma segment OleInPlaceContainerDocIntSeg
STDMETHODIMP IOleInPlaceContainerDocUIWindowSetActiveObject(LPOLEINPLACEUIWINDOW lpThis, LPOLEINPLACEACTIVEOBJECT lpActiveObject, const char * lpszObjName)
{
	OleDocumentPtr		pOleDoc;
	
	OleDbgEnterInterface();
	
	pOleDoc = ((OleInPlaceContainerUIWindowImplPtr)lpThis)->lpOleDoc;

	return OleInPlaceContainerDocSetActiveObject(pOleDoc, lpActiveObject, lpszObjName);
}

// OLDNAME: OleInPlaceContainerDoc.c
//#pragma segment OleInPlaceContainerDocInitSeg
void OleInPlaceContainerDocInit(OleDocumentPtr pOleDoc)
{
	OleInPlaceContainerIUIWindowInit(&pOleDoc->container.inplace.m_UIWindow, pOleDoc);
}

//#pragma segment OleInPlaceContainerDocSeg
void OleInPlaceContainerDocDispose(OleDocumentPtr pOleDoc)
{
}

//#pragma segment OleInPlaceContainerDocSeg
HRESULT OleInPlaceContainerDocLocalQueryInterface(OleDocumentPtr pOleDoc, REFIID riid, void* * lplpvObj)
{
	if (IsEqualIID(riid, &IID_IOleWindow)
			|| IsEqualIID(riid, &IID_IOleInPlaceUIWindow))
	{
		*lplpvObj = &pOleDoc->container.inplace.m_UIWindow;
		OleDocAddRef(pOleDoc);
		return NOERROR;
	}

	*lplpvObj = NULL;
	return ResultFromScode(E_NOINTERFACE);
}

//#pragma segment OleInPlaceContainerDocSeg
Boolean OleInPlaceContainerDocIsUIActive(OleDocumentPtr pOleDoc)
{
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_IsUIActiveProcPtr != nil);
	return (*pOleDoc->m_pIDoc->lpVtbl->m_IsUIActiveProcPtr)(pOleDoc->m_pIDoc);
}

//#pragma segment OleInPlaceContainerDocSeg
Boolean OleInPlaceContainerDocIsUIVisible(OleDocumentPtr pOleDoc)
{
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_IsUIVisibleProcPtr != nil);
	return (*pOleDoc->m_pIDoc->lpVtbl->m_IsUIVisibleProcPtr)(pOleDoc->m_pIDoc);
}

//#pragma segment OleInPlaceContainerDocSeg
void OleInPlaceContainerDocIPDeactivate(OleDocumentPtr pOleDoc)
{
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_DoIPDeactivateProcPtr != nil);
	(*pOleDoc->m_pIDoc->lpVtbl->m_DoIPDeactivateProcPtr)(pOleDoc->m_pIDoc);
}

//#pragma segment OleInPlaceContainerDocSeg
void OleInPlaceContainerDocUIDeactivate(OleDocumentPtr pOleDoc)
{
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_DoUIDeactivateProcPtr != nil);
	(*pOleDoc->m_pIDoc->lpVtbl->m_DoUIDeactivateProcPtr)(pOleDoc->m_pIDoc);
}

//#pragma segment OleInPlaceContainerDocSeg
LPOLEINPLACEUIWINDOW OleInPlaceContainerDocGetIPUIWindow(OleDocumentPtr pOleDoc)
{
	OleDocAddRef(pOleDoc);
	return (LPOLEINPLACEUIWINDOW)&pOleDoc->container.inplace.m_UIWindow;
}

//#pragma segment OleInPlaceContainerDocSeg
HRESULT OleInPlaceContainerDocSetBorderSpace(OleDocumentPtr pOleDoc, LPCBORDERWIDTHS lpborderwidths)
{
	if (lpborderwidths != nil)
		return ResultFromScode(INPLACE_E_NOTOOLSPACE);
	else
		return NOERROR;
}

//#pragma segment OleInPlaceContainerDocSeg
HRESULT OleInPlaceContainerDocSetActiveObject(OleDocumentPtr pOleDoc, LPOLEINPLACEACTIVEOBJECT pActiveObject, const char* pObjectName)
{
	// REVIEW: Should probably do something with active objecte name, like set the window title

	if (pActiveObject == pOleDoc->container.inplace.m_pActiveObject)
		return NOERROR;

	if (pOleDoc->container.inplace.m_pActiveObject)
	{
		pOleDoc->container.inplace.m_pActiveObject->lpVtbl->Release(pOleDoc->container.inplace.m_pActiveObject);
		pOleDoc->container.inplace.m_pActiveObject = nil;
	}

	if (pActiveObject)
	{
		pOleDoc->container.inplace.m_pActiveObject = pActiveObject;
		pOleDoc->container.inplace.m_pActiveObject->lpVtbl->AddRef(pOleDoc->container.inplace.m_pActiveObject);
	}
	
	return NOERROR;
}

//#pragma segment OleInPlaceContainerDocSeg
Boolean OleInPlaceContainerDocNeedsActivateEvent(OleDocumentPtr pOleDoc)
{
	Boolean		needsActivateEvent;
	
	needsActivateEvent = !pOleDoc->container.inplace.m_fIgnoreActivateEvent;
	pOleDoc->container.inplace.m_fIgnoreActivateEvent = false;
	
	return needsActivateEvent;
}

//#pragma segment OleInPlaceContainerDocSeg
void OleInPlaceContainerDocDoActivate(OleDocumentPtr pOleDoc, Boolean becomingActive)
{
	HRESULT		hrErr;

	ASSERTCOND(gOleApp->m_pIApp->lpVtbl->m_IsQuittingProcPtr != nil);
	if ((*gOleApp->m_pIApp->lpVtbl->m_IsQuittingProcPtr)(gOleApp->m_pIApp))
		return;

	if (OleInPlaceContainerDocIsUIActive(pOleDoc) && !OleInPlaceContainerDocIsUIVisible(pOleDoc))
	{
		ASSERTCOND(pOleDoc->container.inplace.m_pActiveObject != nil);
		hrErr = pOleDoc->container.inplace.m_pActiveObject->lpVtbl->OnDocWindowActivate(pOleDoc->container.inplace.m_pActiveObject, becomingActive);
		ASSERTNOERROR(hrErr);
	}
}

//#pragma segment OleInPlaceContainerDocSeg
void OleInPlaceContainerDocUpdateWindowPosition(OleDocumentPtr pOleDoc)
{
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_UpdateWindowPositionProcPtr != nil);
	(*pOleDoc->m_pIDoc->lpVtbl->m_UpdateWindowPositionProcPtr)(pOleDoc->m_pIDoc);
}


//#pragma segment OleInPlaceContainerDocSeg
void OleInPlaceContainerDocSetIdleCursor(OleDocumentPtr pOleDoc, CursPtr pCursor)
{
	WindowPtr	pWindow;
	HRESULT		hrErr;

	if (OleInPlaceContainerDocIsUIVisible(pOleDoc))
	{
		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr != nil);
		pWindow = (*pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr)(pOleDoc->m_pIDoc);

		OleSetCursor(pCursor, pWindow);
	}
}

// OLDNAME: OleOutlineInPlaceContainer.c
//#pragma segment OleOutlineInPlaceContainerSeg
long OleOutlineInPlaceContainerMenuItemToCmd(struct ApplicationRec* pApp, short menuID, short menuItem)
{
	OleOutlineAppPtr	pOleOutlineApp;
	
	pOleOutlineApp = (OleOutlineAppPtr)pApp;

	// this should be called anytime the menuID is in the reserved range
	if (menuID >= 15000 && menuID <= 17000)
		OleUnhashMenuID(&menuID);

	ASSERTCOND(pOleOutlineApp->vtbl->m_MenuItemToCmdProcPtr != nil);
	return (*pOleOutlineApp->vtbl->m_MenuItemToCmdProcPtr)(pApp, menuID, menuItem);
}

//#pragma segment OleOutlineInPlaceContainerSeg
void OleOutlineInPlaceContainerSetIdleCursor(struct ApplicationRec* pApp, CursPtr pCursor)
{
	OleOutlineAppPtr 	pOleOutlineApp;
	OleOutlineDocPtr	pOleOutlineDoc;

	pOleOutlineApp = (OleOutlineAppPtr)pApp;

	ASSERTCOND(pOleOutlineApp->vtbl->m_SetIdleCursorProcPtr != nil);
	(*pOleOutlineApp->vtbl->m_SetIdleCursorProcPtr)(pApp, pCursor);

	ASSERTCOND(pApp->vtbl->m_GetCurrentDocProcPtr != nil);
	pOleOutlineDoc = (OleOutlineDocPtr)(*pApp->vtbl->m_GetCurrentDocProcPtr)(pApp);

	// if m_fFloating, we got the toolbar.

	if ((pOleOutlineDoc == nil)  || (((DocumentRec *)pOleOutlineDoc)->m_fFloating))
		return;

	OleInPlaceContainerDocSetIdleCursor(&pOleOutlineDoc->m_OleDoc, pCursor);
}


#endif // qOleInPlace

