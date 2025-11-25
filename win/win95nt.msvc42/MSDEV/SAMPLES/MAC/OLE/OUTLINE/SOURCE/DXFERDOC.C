/*****************************************************************************\
*                                                                             *
*    OleDataXferDoc.c                                                         *
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
#include "OleDebug.h"
#include "DXferDoc.h"
#include "Const.h"
#include "Util.h"
#include "App.h"
#include "OleXcept.h"
#include <ToolUtils.h>


OLEDBGDATA


// OLDNAME: OleDataXferDocInterface.c

static IDataObjectVtbl			gDataXferDocDataObjectVtbl;

#ifdef qOleDragDrop
static IDropSourceVtbl          gDataXferDocDropSourceVtbl;
#endif

//#pragma segment OleDataXferDocInterface
void OleDataXferDocInitInterfaces(void)
{
	// OleDataXferDoc::IDataObject method table
	{
		IDataObjectVtbl*	p;

		p = &gDataXferDocDataObjectVtbl;

		p->QueryInterface			= OleDataXferDocIntDataObjectQueryInterface;
		p->AddRef					= OleDataXferDocIntDataObjectAddRef;
		p->Release					= OleDataXferDocIntDataObjectRelease;
		p->GetData					= OleDataXferDocIntDataObjectGetData;
		p->GetDataHere				= OleDataXferDocIntDataObjectGetDataHere;
		p->QueryGetData				= OleDataXferDocIntDataObjectQueryGetData;
		p->GetCanonicalFormatEtc	= OleDataXferDocIntDataObjectGetCanonicalFormatEtc;
		p->SetData					= OleDataXferDocIntDataObjectSetData;
		p->EnumFormatEtc			= OleDataXferDocIntDataObjectEnumFormatEtc;
		p->DAdvise					= OleDataXferDocIntDataObjectDAdvise;
		p->DUnadvise				= OleDataXferDocIntDataObjectDUnadvise;
		p->EnumDAdvise				= OleDataXferDocIntDataObjectEnumDAdvise;

		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}

#if qOleDragDrop	
	// IDropSource method table
	{
		IDropSourceVtbl*	p;
		
		p = &gDataXferDocDropSourceVtbl;
		
		p->QueryInterface			= OleDataXferDocIntDropSourceQueryInterface;
		p->AddRef					= OleDataXferDocIntDropSourceAddRef;
		p->Release					= OleDataXferDocIntDropSourceRelease;
		p->QueryContinueDrag		= OleDataXferDocIntDropSourceQueryContinueDrag;
		p->GiveFeedback				= OleDataXferDocIntDropSourceGiveFeedback;

		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}
#endif // qOleDragDrop

}

//#pragma segment OleDataXferDocInterface
void DataXferDocIDataObjectInit(struct DocDataObjectImpl* pDocDataObjectImpl, struct OleDocumentRec* pOleDoc)
{
	pDocDataObjectImpl->lpVtbl		= &gDataXferDocDataObjectVtbl;
	pDocDataObjectImpl->lpOleDoc	= pOleDoc;
	pDocDataObjectImpl->cRef		= 0;
}

//#pragma segment OleDataXferDocInterface
STDMETHODIMP OleDataXferDocIntDataObjectQueryInterface(LPDATAOBJECT lpThis, REFIID riid, void* * lplpvObj)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((DocDataObjectImplPtr)lpThis)->lpOleDoc;

	return OleDocQueryInterface(pOleDoc, riid, lplpvObj);
}

//#pragma segment OleDataXferDocInterface
STDMETHODIMP_(unsigned long) OleDataXferDocIntDataObjectAddRef(LPDATAOBJECT lpThis)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((DocDataObjectImplPtr)lpThis)->lpOleDoc;

	return OleDocAddRef(pOleDoc);
}

//#pragma segment OleDataXferDocInterface
STDMETHODIMP_(unsigned long) OleDataXferDocIntDataObjectRelease (LPDATAOBJECT lpThis)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((DocDataObjectImplPtr)lpThis)->lpOleDoc;

	return OleDocRelease(pOleDoc);
}

//#pragma segment OleDataXferDocInterface
STDMETHODIMP OleDataXferDocIntDataObjectGetData(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium)
{
	OleDocumentPtr		pOleDoc;
	HRESULT				hrErr;
	
	OleDbgEnterInterface();

	pOleDoc = ((DocDataObjectImplPtr)lpThis)->lpOleDoc;

	hrErr = OleDataXferDocDataObjectGetData(pOleDoc, lpFormatetc, lpMedium);

	return hrErr;
}

//#pragma segment OleDataXferDocInterface
STDMETHODIMP OleDataXferDocIntDataObjectGetDataHere(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium)
{
	OleDocumentPtr	pOleDoc;
	HRESULT			hrErr;
		
	OleDbgEnterInterface();

	pOleDoc = ((DocDataObjectImplPtr)lpThis)->lpOleDoc;

	hrErr = OleDataXferDocDataObjectGetDataHere(pOleDoc, lpFormatetc, lpMedium);

	return hrErr;
}

//#pragma segment OleDataXferDocInterface
STDMETHODIMP OleDataXferDocIntDataObjectQueryGetData(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc)
{
	OleDocumentPtr	pOleDoc;
	HRESULT			hrErr;
	
	OleDbgEnterInterface();
	
	pOleDoc = ((DocDataObjectImplPtr)lpThis)->lpOleDoc;

	hrErr = OleDataXferDocDataObjectQueryGetData(pOleDoc, lpFormatetc);
	
	return hrErr;
}

//#pragma segment OleDataXferDocInterface
STDMETHODIMP OleDataXferDocIntDataObjectGetCanonicalFormatEtc(LPDATAOBJECT lpThis, LPFORMATETC lpformatetc, LPFORMATETC lpformatetcOut)
{
	HRESULT			hrErr;

	OleDbgEnterInterface();

    if (!lpformatetcOut)
        return ResultFromScode(E_INVALIDARG);

    /* OLE2NOTE: we must make sure to set all out parameters to NULL. */
    lpformatetcOut->ptd = NULL;

    if (!lpformatetc)
        return ResultFromScode(E_INVALIDARG);

    // OLE2NOTE: we must validate that the format requested is supported
    if ((hrErr=lpThis->lpVtbl->QueryGetData(lpThis,lpformatetc)) != NOERROR)
        return hrErr;

    /* OLE2NOTE: an app that is insensitive to target device (as the
    **    Outline Sample is) should fill in the lpformatOut parameter
    **    but NULL out the "ptd" field; it should return NOERROR if the
    **    input formatetc->ptd what non-NULL. this tells the caller
    **    that it is NOT necessary to maintain a separate screen
    **    rendering and printer rendering. if should return
    **    DATA_S_SAMEFORMATETC if the input and output formatetc's are
    **    identical.
    */

    *lpformatetcOut = *lpformatetc;
    if (lpformatetc->ptd == NULL)
        return ResultFromScode(DATA_S_SAMEFORMATETC);
    else {
        lpformatetcOut->ptd = NULL;
        return NOERROR;
    }
}

//#pragma segment OleDataXferDocInterface
STDMETHODIMP OleDataXferDocIntDataObjectSetData(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium, unsigned long fRelease)
{
	OleDbgEnterInterface();

	return ResultFromScode(E_NOTIMPL);
}

//#pragma segment OleDataXferDocInterface
STDMETHODIMP OleDataXferDocIntDataObjectEnumFormatEtc(LPDATAOBJECT lpThis, unsigned long dwDirection, LPENUMFORMATETC * lplpenumFormatEtc)
{
	OleDocumentPtr	pOleDoc;
	HRESULT			hrErr;
	
	OleDbgEnterInterface();
	
	pOleDoc = ((DocDataObjectImplPtr)lpThis)->lpOleDoc;

	hrErr = OleDataXferDocDataObjectEnumFormatEtc(pOleDoc, dwDirection, lplpenumFormatEtc);

	return hrErr;
}

//#pragma segment OleDataXferDocInterface
STDMETHODIMP OleDataXferDocIntDataObjectDAdvise(LPDATAOBJECT lpThis, FORMATETC * lpFormatetc, unsigned long advf, LPADVISESINK lpAdvSink, unsigned long * lpdwConnection)
{
	OleDbgEnterInterface();
	
	return ResultFromScode(E_NOTIMPL);
}

//#pragma segment OleDataXferDocInterface
STDMETHODIMP OleDataXferDocIntDataObjectDUnadvise(LPDATAOBJECT lpThis, unsigned long dwConnection)
{
	OleDbgEnterInterface();
	
	return ResultFromScode(E_NOTIMPL);
}

//#pragma segment OleDataXferDocInterface
STDMETHODIMP OleDataXferDocIntDataObjectEnumDAdvise(LPDATAOBJECT lpThis, LPENUMSTATDATA * lplpenumAdvise)
{
	OleDbgEnterInterface();

	return ResultFromScode(E_NOTIMPL);
}

#if qOleDragDrop

//#pragma segment OleDataXferDocInterface
void DataXferDocIDropSourceInit(DocDropSourceImplPtr pDocDropSourceImpl, struct OleDocumentRec* pOleDoc)
{
	pDocDropSourceImpl->lpVtbl		= &gDataXferDocDropSourceVtbl;
	pDocDropSourceImpl->lpOleDoc	= pOleDoc;
	pDocDropSourceImpl->cRef		= 0;
}

//#pragma segment OleDataXferDocInterface
STDMETHODIMP OleDataXferDocIntDropSourceQueryInterface(LPDROPSOURCE lpThis, REFIID riid, void* * lplpvObj)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((DocDropSourceImplPtr)lpThis)->lpOleDoc;

	return OleDocQueryInterface(pOleDoc, riid, lplpvObj);
}

//#pragma segment OleDataXferDocInterface
STDMETHODIMP_(unsigned long) OleDataXferDocIntDropSourceAddRef(LPDROPSOURCE lpThis)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((DocDropSourceImplPtr)lpThis)->lpOleDoc;

	return OleDocAddRef(pOleDoc);
}

//#pragma segment OleDataXferDocInterface
STDMETHODIMP_(unsigned long) OleDataXferDocIntDropSourceRelease(LPDROPSOURCE lpThis)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((DocDropSourceImplPtr)lpThis)->lpOleDoc;

	return OleDocRelease(pOleDoc);
}

//#pragma segment OleDataXferDocInterface
STDMETHODIMP OleDataXferDocIntDropSourceQueryContinueDrag(LPDROPSOURCE lpThis, unsigned long fEscapePressed, unsigned long grfKeyState)
{
    if (fEscapePressed)
        return ResultFromScode(DRAGDROP_S_CANCEL);
    else if (!Button())
        return ResultFromScode(DRAGDROP_S_DROP);
    else
        return NOERROR;
}


//#pragma segment OleDataXferDocInterface
STDMETHODIMP OleDataXferDocIntDropSourceGiveFeedback(LPDROPSOURCE lpThis, unsigned long dwEffect)
{
#if qOleCustomDragDropCursors
	CursHandle		hCursor = nil;

	if (dwEffect & DROPEFFECT_SCROLL) {
		hCursor = GetCursor ( kCursorDragScroll );
	}
	else if (dwEffect & DROPEFFECT_COPY) {
		hCursor = GetCursor ( kCursorDragCopy );
	}
	else if (dwEffect & DROPEFFECT_MOVE) {
		hCursor = GetCursor ( kCursorDragMove );
	}
	else if (dwEffect & DROPEFFECT_LINK) {
		hCursor = GetCursor ( kCursorDragLink );
	}
	else if (dwEffect == DROPEFFECT_NONE) {
		hCursor = GetCursor ( kCursorDragNone );
	}

	if (hCursor)
		SetCursor(*hCursor);

    return NOERROR;

#else
    // Tell OLE to use the standard drag/drop feedback cursors
    return ResultFromScode(DRAGDROP_S_USEDEFAULTCURSORS);


#endif // qOleCustomDragDropCursors

}

#endif // qOleDragDrop

// OLDNAME: OleDataXferDoc.c

#ifndef _MSC_VER
#define STGMEDIUM_HGLOBAL	u.hGlobal
#define STGMEDIUM_PSTG		u.pstg
#else
#define STGMEDIUM_HGLOBAL	hGlobal
#define STGMEDIUM_PSTG		pstg
#endif

extern OleApplicationPtr		gOleApp;

//#pragma segment OleDataXferDocSeg
HRESULT OleDataXferDocDataObjectGetData(OleDocumentPtr pOleDoc, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium)
{
#if qOleServerApp
	OleApplicationPtr	pOleApp;
	OleServerDocPtr 	pServerDoc;
	HRESULT				hrErr;

	pOleApp = gOleApp;
	pServerDoc = &pOleDoc->server;
	lpMedium->tymed = 0;
	lpMedium->pUnkForRelease = NULL;	// we transfer ownership to caller
	lpMedium->STGMEDIUM_HGLOBAL = NULL;
		
	if (lpFormatetc->cfFormat == 'PICT' &&
		(lpFormatetc->dwAspect & (DVASPECT_CONTENT | DVASPECT_DOCPRINT)))
	{
		if (!(lpFormatetc->tymed & TYMED_MFPICT))
			return ResultFromScode(DATA_E_FORMATETC);
			
		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetMetafilePictDataProcPtr != nil);
		lpMedium->STGMEDIUM_HGLOBAL = (*pOleDoc->m_pIDoc->lpVtbl->m_GetMetafilePictDataProcPtr)(pOleDoc->m_pIDoc, nil);
		ASSERTCOND(lpMedium->STGMEDIUM_HGLOBAL != nil);
		
		if (!lpMedium->STGMEDIUM_HGLOBAL) {
			hrErr = ResultFromScode(E_OUTOFMEMORY);
			goto exit;
		}
		
		lpMedium->tymed = TYMED_MFPICT;
		
		hrErr = NOERROR;
		goto exit;
	}
	else if (lpFormatetc->cfFormat == 'PICT' &&
		(lpFormatetc->dwAspect & DVASPECT_ICON))
	{
		if (!(lpFormatetc->tymed & TYMED_MFPICT)) {
			hrErr = ResultFromScode(DATA_E_FORMATETC);
			goto exit;
		}
		
		hrErr = ResultFromScode(DATA_E_FORMATETC);
		goto exit;
	}
	else if (lpFormatetc->cfFormat == 'TEXT')
	{
		if (!(lpFormatetc->tymed & TYMED_HGLOBAL)) {
			hrErr = ResultFromScode(DATA_E_FORMATETC);
			goto exit;
		}
			
		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetTextDataProcPtr != nil);
		lpMedium->STGMEDIUM_HGLOBAL = (*pOleDoc->m_pIDoc->lpVtbl->m_GetTextDataProcPtr)(pOleDoc->m_pIDoc, nil);
		ASSERTCOND(lpMedium->STGMEDIUM_HGLOBAL != nil);
		
		if (!lpMedium->STGMEDIUM_HGLOBAL) {
			hrErr = ResultFromScode(E_OUTOFMEMORY);
		goto exit;
	}

		lpMedium->tymed = TYMED_HGLOBAL;
		
		hrErr = NOERROR;
		goto exit;
			
	}

	/* OLE2NOTE: ObjectDescriptor and LinkSrcDescriptor will
	**    contain the same data for the pure container and pure server
	**    type applications. only a container/server application may
	**    have different content for ObjectDescriptor and
	**    LinkSrcDescriptor. if a container/server copies a link for
	**    example, then the ObjectDescriptor would give the class
	**    of the link source but the LinkSrcDescriptor would give the
	**    class of the container/server itself. in this situation if a
	**    paste operation occurs, an equivalent link is pasted, but if
	**    a pastelink operation occurs, then a link to a pseudo object
	**    in the container/server is created.
	*/
	if (lpFormatetc->cfFormat == pOleApp->m_cfObjectDescriptor ||
			(lpFormatetc->cfFormat == pOleApp->m_cfLinkSrcDescriptor &&
				pOleDoc->m_fLinkSourceAvail)) {
        // Verify caller asked for correct medium
        if (!(lpFormatetc->tymed & TYMED_HGLOBAL))
            return ResultFromScode(DATA_E_FORMATETC);

        lpMedium->STGMEDIUM_HGLOBAL = OleDataXferDocGetObjectDescriptorData(pOleDoc);

        if (! lpMedium->STGMEDIUM_HGLOBAL)
            return ResultFromScode(E_OUTOFMEMORY);

        lpMedium->tymed = TYMED_HGLOBAL;
		return NOERROR;

	} else if (lpFormatetc->cfFormat == pOleApp->m_cfEmbedSource) {
        hrErr = OleStdGetOleObjectData(
                (LPPERSISTSTORAGE)&pServerDoc->m_PersistStorage,
                lpFormatetc,
                lpMedium,
                FALSE   /* fUseMemory -- (use file-base stg) */

        );
        if (hrErr != NOERROR)
        	goto exit;

		hrErr = NOERROR;
		goto exit;

    }
    else if (lpFormatetc->cfFormat == pOleApp->m_cfLinkSource) {
        if (pOleDoc->m_fLinkSourceAvail) {
            LPMONIKER lpmk;
            void* pCopiedRange;
            OleDocumentPtr pSrcOleDoc = pOleDoc->m_pSrcDocOfCopy;

            ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetCopiedRangeProcPtr != nil);
            pCopiedRange = (pOleDoc->m_pIDoc->lpVtbl->m_GetCopiedRangeProcPtr)(pOleDoc->m_pIDoc);
			
			ASSERTCOND(pSrcOleDoc->m_pIDoc->lpVtbl->m_GetRangeFullMonikerProcPtr != nil);
			lpmk = (pSrcOleDoc->m_pIDoc->lpVtbl->m_GetRangeFullMonikerProcPtr)(
						pSrcOleDoc->m_pIDoc,
						pCopiedRange,
						OLEGETMONIKER_FORCEASSIGN
            );
            if (lpmk) {
                hrErr = OleStdGetLinkSourceData(
                        lpmk,
                        (LPCLSID)&CLSID_Application,
                        lpFormatetc,
                        lpMedium
                );
                OleStdRelease((LPUNKNOWN)lpmk);
                if (hrErr != NOERROR)
                    return hrErr;
				return NOERROR;

            } else
                return ResultFromScode(E_FAIL);
        } else
            return ResultFromScode(DATA_E_FORMATETC);

    }
	hrErr = ResultFromScode(DATA_E_FORMATETC);
	
exit:
	return hrErr;

#elif qOleContainerApp

	OleApplicationPtr	pOleApp = gOleApp;
	HRESULT				hrErr;
	OleContainerDocPtr	pContainerDoc = &pOleDoc->container;	
	
	lpMedium->tymed = 0;
	lpMedium->pUnkForRelease = NULL;	// we transfer ownership to caller
	lpMedium->STGMEDIUM_HGLOBAL = NULL;
		
	ASSERTCOND(pOleApp->m_pIApp->lpVtbl->m_GetClipboardFormatProcPtr != nil);
    if (lpFormatetc->cfFormat == (*pOleApp->m_pIApp->lpVtbl->m_GetClipboardFormatProcPtr)(pOleApp->m_pIApp)) {

        /* OLE2NOTE: currently OLE does NOT support remoting a root
        **    level IStorage (either memory or file based) as an OUT
        **    parameter. thus, we can NOT support GetData for this
        **    TYMED_ISTORAGE based format. the caller MUST call GetDataHere.
        */
		hrErr = ResultFromScode(DATA_E_FORMATETC);
		goto exit;
    }

	else if (lpFormatetc->cfFormat == 'TEXT')
	{
		if (!(lpFormatetc->tymed & TYMED_HGLOBAL)) {
			hrErr = ResultFromScode(DATA_E_FORMATETC);
			goto exit;
		}
			
		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetTextDataProcPtr != nil);
		lpMedium->STGMEDIUM_HGLOBAL = (*pOleDoc->m_pIDoc->lpVtbl->m_GetTextDataProcPtr)(pOleDoc->m_pIDoc, nil);
		ASSERTCOND(lpMedium->STGMEDIUM_HGLOBAL != nil);
		
		if (!lpMedium->STGMEDIUM_HGLOBAL) {
			hrErr = ResultFromScode(E_OUTOFMEMORY);
			goto exit;
		}

		lpMedium->tymed = TYMED_HGLOBAL;
		
		hrErr = NOERROR;
		goto exit;
			
	}

	/* OLE2NOTE: ObjectDescriptor and LinkSrcDescriptor will
	**    contain the same data for the pure container and pure server
	**    type applications. only a container/server application may
	**    have different content for ObjectDescriptor and
	**    LinkSrcDescriptor. if a container/server copies a link for
	**    example, then the ObjectDescriptor would give the class
	**    of the link source but the LinkSrcDescriptor would give the
	**    class of the container/server itself. in this situation if a
	**    paste operation occurs, an equivalent link is pasted, but if
	**    a pastelink operation occurs, then a link to a pseudo object
	**    in the container/server is created.
	*/
	else if (lpFormatetc->cfFormat == pOleApp->m_cfObjectDescriptor
			 || (lpFormatetc->cfFormat == pOleApp->m_cfLinkSrcDescriptor &&
         		 pOleDoc->m_fLinkSourceAvail))
    {
        // Verify caller asked for correct medium
        if (!(lpFormatetc->tymed & TYMED_HGLOBAL))
            return ResultFromScode(DATA_E_FORMATETC);

        lpMedium->STGMEDIUM_HGLOBAL = OleDataXferDocGetObjectDescriptorData(pOleDoc);

        if (! lpMedium->STGMEDIUM_HGLOBAL)
            return ResultFromScode(E_OUTOFMEMORY);

        lpMedium->tymed = TYMED_HGLOBAL;
		return NOERROR;

	}
	
	else if (pContainerDoc->m_pCopiedEmbedding) {

        /* OLE2NOTE: if this document contains a single OLE object
        **    (ie. cfEmbeddedObject data format is available), then
        **    the formats offered via our IDataObject must include
        **    the formats available from the OLE object itself.
        **    thus, we delegate this call to the IDataObject* of the
        **    OLE object.
        */

        if (lpFormatetc->cfFormat == pOleApp->m_cfEmbeddedObject)
        {
            LPPERSISTSTORAGE pPersistStg;

            ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetObjectInterfaceProcPtr != nil);
            pPersistStg = (LPPERSISTSTORAGE)(*pOleDoc->m_pIDoc->lpVtbl->m_GetObjectInterfaceProcPtr)(
            					pOleDoc->m_pIDoc,
            					pContainerDoc->m_pCopiedEmbedding,
            					&IID_IPersistStorage);

            if (!pPersistStg)
                return ResultFromScode(DATA_E_FORMATETC);

            /* render CF_EMBEDDEDOBJECT by asking the storage to save
            **    into the pStg of the caller.
            */
            hrErr = OleStdGetOleObjectData(
                    pPersistStg,
                    lpFormatetc,
                    lpMedium,
                    FALSE   /* fUseMemory -- (use file-base stg) */
            );
            OleStdRelease((LPUNKNOWN)pPersistStg);

            return hrErr;

        } else if (lpFormatetc->cfFormat == 'PICT') {

            /* OLE2NOTE: as a container which draws objects, when a single
            **    OLE object is copied, we can give the Metafile picture of
            **    the object. we will offer this picture when asked either
            **    for the actual drawing aspect that we are displaying for
            **    the object or for DVASPECT_DOCPRINT (if asked to render
            **    it to a printer)
            */
            LPOLEOBJECT pOleObj;

            // Verify caller asked for correct medium
            if (!(lpFormatetc->tymed & TYMED_MFPICT)) {
                hrErr = ResultFromScode(DATA_E_FORMATETC);
                goto exit;
            }

            ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetObjectInterfaceProcPtr != nil);
            pOleObj = (LPOLEOBJECT)(*pOleDoc->m_pIDoc->lpVtbl->m_GetObjectInterfaceProcPtr)(
            					pOleDoc->m_pIDoc,
            					pContainerDoc->m_pCopiedEmbedding,
            					&IID_IOleObject);

            if (! pOleObj) {
                hrErr = ResultFromScode(E_OUTOFMEMORY);     // could not load object
                goto exit;
            }
            if (lpFormatetc->dwAspect &
                    (pContainerDoc->m_AspectOleObjCopied | DVASPECT_DOCPRINT)) {
                /* render 'PICT' by drawing the object into
                **    a metafile DC
                */
                lpMedium->STGMEDIUM_HGLOBAL = OleStdGetMetafilePictFromOleObject(
                        pOleObj, pContainerDoc->m_AspectOleObjCopied);
                OleStdRelease((LPUNKNOWN)pOleObj);
                if (! lpMedium->STGMEDIUM_HGLOBAL) {
                    hrErr = ResultFromScode(E_OUTOFMEMORY);
                    goto exit;
                }

                lpMedium->tymed = TYMED_MFPICT;

                return NOERROR;
            }
            else {
                // improper aspect requested
                OleStdRelease((LPUNKNOWN)pOleObj);
                return ResultFromScode(DATA_E_FORMATETC);
            }

        } else if (lpFormatetc->cfFormat == pOleApp->m_cfLinkSource) {
            if (pOleDoc->m_fLinkSourceAvail) {
                LPMONIKER lpmk;
                LPOLEOBJECT pOleObject;
				LPOLECLIENTSITE pOleClientSite;

            ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetObjectInterfaceProcPtr != nil);
            pOleObject = (LPOLEOBJECT)(*pOleDoc->m_pIDoc->lpVtbl->m_GetObjectInterfaceProcPtr)(
            					pOleDoc->m_pIDoc,
            					pContainerDoc->m_pCopiedEmbeddingSource,
            					&IID_IOleObject);
            if (!pOleObject)
                return ResultFromScode(DATA_E_FORMATETC);
			pOleObject->lpVtbl->GetClientSite(pOleObject, &pOleClientSite);
            OleStdRelease((LPUNKNOWN)pOleObject);
			
            if (!pOleClientSite)
                return ResultFromScode(DATA_E_FORMATETC);
					
			pOleClientSite->lpVtbl->GetMoniker(
					pOleClientSite,
                	OLEGETMONIKER_FORCEASSIGN,
                	OLEWHICHMK_OBJFULL,
                	&lpmk
        	);

            OleStdRelease((LPUNKNOWN)pOleClientSite);

			if (lpmk)
			{
				hrErr = OleStdGetLinkSourceData(
							lpmk,
							&pContainerDoc->m_clsidOleObjCopied,
							lpFormatetc,
							lpMedium
				);
				OleStdRelease((LPUNKNOWN)lpmk);

				return hrErr;
			}
			else {
				return ResultFromScode(DATA_E_FORMATETC);
			}
		}
		else {
			return ResultFromScode(DATA_E_FORMATETC);
			goto exit;
		}

	}

#if qOleOptionalAdvancedDataTransfer
        /* OLE2NOTE: optionally, a container that wants to have a
        **    potentially richer data transfer, can enumerate the data
        **    formats from the OLE object's cache and offer them too. if
        **    the object has a special handler, then it might be able to
        **    render additional data formats. in this case, the
        **    container must delegate the GetData call to the object if
        **    it does not directly support the format.
        **
        **    CNTROUTL does NOT enumerate the cache; it implements the
        **    simpler strategy of offering a static list of formats.
        **    thus the delegation is NOT required.
        */
      else {

            /* OLE2NOTE: we delegate this call to the IDataObject* of the
            **    OLE object.
            */
            LPDATAOBJECT pDataObj;

            ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetObjectInterfaceProcPtr != nil);
            pDataObj = (LPDATAOBJECT)(*pOleDoc->m_pIDoc->lpVtbl->m_GetObjectInterfaceProcPtr)(
            					pOleDoc->m_pIDoc,
            					pContainerDoc->m_pItem,
            					&IID_IDataObject);
			
            if (!pDataObj)
                return ResultFromScode(DATA_E_FORMATETC);

            hrErr=pDataObj->lpVtbl->GetData(pDataObj,lpFormatetc,lpMedium);

            OleStdRelease((LPUNKNOWN)pDataObj);
            return hrErr;

        }
#endif  // qOleOptionalAdvancedDataTransfer

    }


	hrErr = ResultFromScode(DATA_E_FORMATETC);
	
exit:
	return hrErr;
	
#endif	
}

//#pragma segment OleDataXferDocSeg
HRESULT OleDataXferDocDataObjectGetDataHere(OleDocumentPtr pOleDoc, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium)
{
#if qOleServerApp
	OleServerDocPtr		pServerDoc;
    OleApplicationPtr	pOleApp;
    HRESULT         	hrErr;
    SCODE           	sc;

	pServerDoc = &pOleDoc->server;
    pOleApp = gOleApp;
		
    // OLE2NOTE: we must set out pointer parameters to NULL
    lpMedium->pUnkForRelease = NULL;
	
	/* our user document does not support any formats for GetDataHere.
	**    if the document is used for
	**    purposes of data transfer, then additional formats are offered.
	*/
    if (lpFormatetc->cfFormat == pOleApp->m_cfEmbedSource) {
        hrErr = OleStdGetOleObjectData(
                (LPPERSISTSTORAGE)&pServerDoc->m_PersistStorage,
                lpFormatetc,
                lpMedium,
                FALSE   /* fUseMemory -- (use file-base stg) */
        );
        if (hrErr != NOERROR)
            goto exit;

        OleDbgOut3("ServerDoc_GetDataHere: rendered CF_EMBEDSOURCE\r\n");
		goto exit;

    }
    else if (lpFormatetc->cfFormat == pOleApp->m_cfLinkSource) {
        if (pOleDoc->m_fLinkSourceAvail) {
            LPMONIKER lpmk;
            void* pCopiedRange;
            OleDocumentPtr pSrcOleDoc = pOleDoc->m_pSrcDocOfCopy;

            ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetCopiedRangeProcPtr != nil);
            pCopiedRange = (pOleDoc->m_pIDoc->lpVtbl->m_GetCopiedRangeProcPtr)(pOleDoc->m_pIDoc);

			ASSERTCOND(pSrcOleDoc->m_pIDoc->lpVtbl->m_GetRangeFullMonikerProcPtr != nil);
			lpmk = (pSrcOleDoc->m_pIDoc->lpVtbl->m_GetRangeFullMonikerProcPtr)(
						pSrcOleDoc->m_pIDoc,
						pCopiedRange,
						OLEGETMONIKER_FORCEASSIGN
            );
            if (lpmk) {
                hrErr = OleStdGetLinkSourceData(
                        lpmk,
                        (LPCLSID)&CLSID_Application,
                        lpFormatetc,
                        lpMedium
                );
                OleStdRelease((LPUNKNOWN)lpmk);
                if (hrErr != NOERROR) {
                    return hrErr;
                }

				return NOERROR;

            } else {
                return ResultFromScode(E_FAIL);
            }
        } else {
            sc = DATA_E_FORMATETC;
            return ResultFromScode(DATA_E_FORMATETC);
        }
    } else {

        /* Caller is requesting data to be returned in Caller allocated
        **    medium, but we do NOT support this. we only support
        **    global memory blocks that WE allocate for the caller.
        */
        return ResultFromScode(DATA_E_FORMATETC);
    }

	hrErr = ResultFromScode(DATA_E_FORMATETC);
	
exit:
    return hrErr;

#elif qOleContainerApp

	OleContainerDocPtr	pContainerDoc;
    OleApplicationPtr	pOleApp;
    HRESULT         	hrErr;
    SCODE           	sc;

	pContainerDoc = &pOleDoc->container;
    pOleApp = gOleApp;
		
    // OLE2NOTE: we must set out pointer parameters to NULL
    lpMedium->pUnkForRelease = NULL;

	ASSERTCOND(pOleApp->m_pIApp->lpVtbl->m_GetClipboardFormatProcPtr != nil);
    if (lpFormatetc->cfFormat == (*pOleApp->m_pIApp->lpVtbl->m_GetClipboardFormatProcPtr)(pOleApp->m_pIApp))
    {
        // we only support  IStorage medium
        if (!(lpFormatetc->tymed & TYMED_ISTORAGE))
            return ResultFromScode(DATA_E_FORMATETC);

        if (lpMedium->tymed == TYMED_ISTORAGE)
        {
            /* Caller has allocated the storage. we must copy all of our
            **    data into his storage.
            */

            /*  OLE2NOTE: we must be sure to write our class ID into our
            **    storage. this information is used by OLE to determine the
            **    class of the data stored in our storage.
            */
            if((hrErr = WriteClassStg(lpMedium->STGMEDIUM_PSTG, &CLSID_Application)) != NOERROR)
                return hrErr;

			ASSERTCOND(pOleApp->m_pIApp->lpVtbl->m_GetClipboardFormatProcPtr != nil);
			ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_SaveToStgProcPtr != nil);			
            (*pOleDoc->m_pIDoc->lpVtbl->m_SaveToStgProcPtr)(
                    pOleDoc->m_pIDoc,
                    (*pOleApp->m_pIApp->lpVtbl->m_GetClipboardFormatProcPtr)(pOleApp->m_pIApp),
                    lpMedium->STGMEDIUM_PSTG,
                    false
            );
            OleStdCommitStorage(lpMedium->STGMEDIUM_PSTG);

            OleDbgOut3("ContainerDoc_GetDataHere: rendered CF_CNTROUTL\r\n");
            return NOERROR;
        }
        else
        {
            // we only support IStorage medium
            return ResultFromScode(DATA_E_FORMATETC);
        }

    }

    else if (pContainerDoc->m_pCopiedEmbedding)
    {
        /* OLE2NOTE: if this document contains a single OLE object
        **    (ie. cfEmbeddedObject data format is available), then
        **    the formats offered via our IDataObject must include
        **    CF_EMBEDDEDOBJECT and the formats available from the OLE
        **    object itself.
        */
        if (lpFormatetc->cfFormat == pOleApp->m_cfEmbeddedObject)
        {
            LPPERSISTSTORAGE pPersistStg;

            ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetObjectInterfaceProcPtr != nil);
            pPersistStg = (LPPERSISTSTORAGE)(*pOleDoc->m_pIDoc->lpVtbl->m_GetObjectInterfaceProcPtr)(
            					pOleDoc->m_pIDoc,
            					pContainerDoc->m_pCopiedEmbedding,
            					&IID_IPersistStorage);

            if (!pPersistStg)
                return ResultFromScode(DATA_E_FORMATETC);

            /* render CF_EMBEDDEDOBJECT by asking the storage to save
            **    into the pStg of the caller.
            */

            hrErr = OleStdGetOleObjectData(
                    pPersistStg,
                    lpFormatetc,
                    lpMedium,
                    FALSE   /* fUseMemory -- (use file-base stg) */
            );
            OleStdRelease((LPUNKNOWN)pPersistStg);

            return hrErr;
        }
        else if (lpFormatetc->cfFormat == 'PICT')
        {
            /* OLE2NOTE: as a container which draws objects, when a single
            **    OLE object is copied, we can give the Metafile picture of
            **    the object. we will offer this picture when asked either
            **    for the actual drawing aspect that we are displaying for
            **    the object or for DVASPECT_DOCPRINT (if asked to render
            **    it to a printer)
            */
            LPOLEOBJECT pOleObj;

            // Verify caller asked for correct medium
            if (!(lpFormatetc->tymed & TYMED_MFPICT)) {
                hrErr = ResultFromScode(DATA_E_FORMATETC);
                goto exit;
            }

            ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetObjectInterfaceProcPtr != nil);
            pOleObj = (LPOLEOBJECT)(*pOleDoc->m_pIDoc->lpVtbl->m_GetObjectInterfaceProcPtr)(
            					pOleDoc->m_pIDoc,
            					pContainerDoc->m_pCopiedEmbedding,
            					&IID_IOleObject);

            if (! pOleObj) {
                hrErr = ResultFromScode(E_OUTOFMEMORY);     // could not load object
                goto exit;
            }

            if (lpFormatetc->dwAspect &
                    (pContainerDoc->m_AspectOleObjCopied | DVASPECT_DOCPRINT)) {
                /* render 'PICT' by drawing the object into
                **    a metafile DC
                */
                lpMedium->STGMEDIUM_HGLOBAL = OleStdGetMetafilePictFromOleObject(
                        pOleObj, pContainerDoc->m_AspectOleObjCopied);
                OleStdRelease((LPUNKNOWN)pOleObj);
                if (! lpMedium->STGMEDIUM_HGLOBAL) {
                    hrErr = ResultFromScode(E_OUTOFMEMORY);
                    goto exit;
                }

                lpMedium->tymed = TYMED_MFPICT;

                return NOERROR;
            }
            else {
                // improper aspect requested
                OleStdRelease((LPUNKNOWN)pOleObj);
                return ResultFromScode(DATA_E_FORMATETC);
            }

        }
        else if (lpFormatetc->cfFormat == pOleApp->m_cfLinkSource)
        {
            if (pOleDoc->m_fLinkSourceAvail)
            {
                LPMONIKER lpmk;
				LPOLECLIENTSITE pOleClientSite;

            	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetObjectInterfaceProcPtr != nil);
            	pOleClientSite = (LPOLECLIENTSITE)(*pOleDoc->m_pIDoc->lpVtbl->m_GetObjectInterfaceProcPtr)(
            						pOleDoc->m_pIDoc,
            						pContainerDoc->m_pCopiedEmbeddingSource,
            						&IID_IOleClientSite);
			
            	if (!pOleClientSite)
                	return ResultFromScode(DATA_E_FORMATETC);
					
				pOleClientSite->lpVtbl->GetMoniker(
						pOleClientSite,
	                	OLEGETMONIKER_FORCEASSIGN,
	                	OLEWHICHMK_OBJFULL,
	                	&lpmk
	        	);
	
	            OleStdRelease((LPUNKNOWN)pOleClientSite);
	
				if (lpmk)
				{
					hrErr = OleStdGetLinkSourceData(
								lpmk,
								&pContainerDoc->m_clsidOleObjCopied,
								lpFormatetc,
								lpMedium
					);
					OleStdRelease((LPUNKNOWN)lpmk);
	
					return hrErr;
				}
				else {
					return ResultFromScode(DATA_E_FORMATETC);
				}
			}
			else {
				return ResultFromScode(DATA_E_FORMATETC);
				goto exit;
			}
        }

#if qOleOptionalAdvancedDataTransfer
        /* OLE2NOTE: optionally, a container that wants to have a
        **    potentially richer data transfer, can enumerate the data
        **    formats from the OLE object's cache and offer them too. if
        **    the object has a special handler, then it might be able to
        **    render additional data formats. in this case, the
        **    container must delegate the GetData call to the object if
        **    it does not directly support the format.
        **
        **    CNTROUTL does NOT enumerate the cache; it implements the
        **    simpler strategy of offering a static list of formats.
        **    thus the delegation is NOT required.
        */
      	else {

            /* OLE2NOTE: we delegate this call to the IDataObject* of the
            **    OLE object.
            */
            LPDATAOBJECT pDataObj;

            ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetObjectInterfaceProcPtr != nil);
            pDataObj = (LPDATAOBJECT)(*pOleDoc->m_pIDoc->lpVtbl->m_GetObjectInterfaceProcPtr)(
            					pOleDoc->m_pIDoc,
            					&IID_IDataObject);
			
            if (!pDataObj)
                return ResultFromScode(DATA_E_FORMATETC);

            hrErr=pDataObj->lpVtbl->GetDataHere(pDataObj,lpFormatetc,lpMedium);

            OleStdRelease((LPUNKNOWN)pDataObj);
            return hrErr;

		}
#endif  // qOleOptionalAdvancedDataTransfer

	}
	
	hrErr = ResultFromScode(DATA_E_FORMATETC);
	
exit:
    return hrErr;

#endif
}

//#pragma segment OleDataXferDocSeg
HRESULT OleDataXferDocDataObjectQueryGetData(OleDocumentPtr pOleDoc, LPFORMATETC lpFormatetc)
{
#if qOleServerApp

	OleApplicationPtr	pOleApp;
	HRESULT			hrErr;

	pOleApp = gOleApp;
	
	if (  // lpFormatetc->cfFormat == pOleApp->m_cfApplication ||
	      lpFormatetc->cfFormat == 'TEXT') {
		hrErr = OleStdQueryFormatMedium(lpFormatetc, TYMED_HGLOBAL);
		goto exit;
	}
	else if (lpFormatetc->cfFormat == 'PICT'
			&& (lpFormatetc->dwAspect & (DVASPECT_CONTENT | DVASPECT_CONTENT | DVASPECT_DOCPRINT))) {
		hrErr = OleStdQueryFormatMedium(lpFormatetc, TYMED_MFPICT);
		goto exit;
	}

	/* the above are the only formats supports by a user document (ie.
	**    a non-data transfer doc). if the document is used for
	**    purposes of data transfer, then additional formats are offered.
	*/
    if (lpFormatetc->cfFormat == pOleApp->m_cfEmbedSource) {
        hrErr = OleStdQueryOleObjectData(lpFormatetc);
    	goto exit;
    } else if (lpFormatetc->cfFormat == pOleApp->m_cfObjectDescriptor) {
        return OleStdQueryObjectDescriptorData(lpFormatetc);

    }
    else if (lpFormatetc->cfFormat == pOleApp->m_cfLinkSource &&
        pOleDoc->m_fLinkSourceAvail) {
        return OleStdQueryLinkSourceData(lpFormatetc);

    } else if (lpFormatetc->cfFormat == pOleApp->m_cfLinkSrcDescriptor &&
		        pOleDoc->m_fLinkSourceAvail) {
        return OleStdQueryObjectDescriptorData(lpFormatetc);
    }
	
	hrErr = ResultFromScode(DATA_E_FORMATETC);
	
exit:
	return hrErr;

#elif qOleContainerApp

	OleApplicationPtr	pOleApp;
	HRESULT				hrErr;
	OleContainerDocPtr	pContainerDoc;

	pOleApp = gOleApp;
	pContainerDoc = &pOleDoc->container;

    /* Caller is querying if we support certain format but does not
    **    want any data actually returned.
    */
    ASSERTCOND(pOleApp->m_pIApp->lpVtbl->m_GetClipboardFormatProcPtr != nil);
    if (lpFormatetc->cfFormat == (*pOleApp->m_pIApp->lpVtbl->m_GetClipboardFormatProcPtr)(pOleApp->m_pIApp)) {
        // we only support ISTORAGE medium
        return OleStdQueryFormatMedium(lpFormatetc, TYMED_ISTORAGE);

    } else if (lpFormatetc->cfFormat == pOleApp->m_cfEmbeddedObject &&
            pContainerDoc->m_pCopiedEmbedding) {
        return OleStdQueryOleObjectData(lpFormatetc);

    } else if (lpFormatetc->cfFormat == pOleApp->m_cfLinkSource &&
            pOleDoc->m_fLinkSourceAvail) {
        return OleStdQueryLinkSourceData(lpFormatetc);

    } else if (lpFormatetc->cfFormat == 'TEXT') {
        // we only support HGLOBAL medium
        return OleStdQueryFormatMedium(lpFormatetc, TYMED_HGLOBAL);

    } else if ( lpFormatetc->cfFormat == pOleApp->m_cfObjectDescriptor ||
        (lpFormatetc->cfFormat == pOleApp->m_cfLinkSrcDescriptor &&
            pOleDoc->m_fLinkSourceAvail) ) {
        return OleStdQueryObjectDescriptorData(lpFormatetc);

    } else if (lpFormatetc->cfFormat == 'PICT' &&
            pContainerDoc->m_pCopiedEmbedding &&
            (lpFormatetc->dwAspect & (pContainerDoc->m_AspectOleObjCopied | DVASPECT_DOCPRINT))) {
        /* OLE2NOTE: as a container which draws objects, when a single
        **    OLE object is copied, we can give the Metafile picture of
        **    the object. we will offer this picture when asked either
        **    for the actual drawing aspect that we are displaying for
        **    the object or for DVASPECT_DOCPRINT (if asked to render
        **    it to a printer)
        */
        // we only support MFPICT medium
        return OleStdQueryFormatMedium(lpFormatetc, TYMED_MFPICT);

    }
    else if (pContainerDoc->m_pCopiedEmbedding)
    {
    	LPDATAOBJECT pDataObj;
    	
        ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetObjectInterfaceProcPtr != nil);
        pDataObj = (LPDATAOBJECT)(*pOleDoc->m_pIDoc->lpVtbl->m_GetObjectInterfaceProcPtr)(
							pOleDoc->m_pIDoc,
           					pContainerDoc->m_pCopiedEmbedding,
							&IID_IDataObject);

        if (pDataObj)
        {
        	/* OLE2NOTE: if this document contains a single OLE object
        	**    (ie. cfEmbeddedObject data format is available), then
        	**    the formats offered via our IDataObject must include
        	**    the formats available from the OLE object itself.
        	**    thus we delegate this call to the IDataObject* of the
        	**    OLE object.
        	*/
        	hrErr = pDataObj->lpVtbl->QueryGetData(pDataObj, lpFormatetc);

	        OleStdRelease((LPUNKNOWN)pDataObj);
	
	        goto exit;
    	}
    }
	
	hrErr = ResultFromScode(DATA_E_FORMATETC);
	
exit:
	return hrErr;
	
#endif
}

//#pragma segment OleDataXferDocSeg
HRESULT OleDataXferDocDataObjectEnumFormatEtc(
        OleDocumentPtr			pOleDoc,
        unsigned long                   dwDirection,
        LPENUMFORMATETC *    lplpenumFormatEtc
)
{
#if qOleServerApp
    OleApplicationPtr  	pOleApp;
    int 		nActualFmts;
    SCODE 		sc = S_OK;

	pOleApp = gOleApp;
	*lplpenumFormatEtc = NULL;
	
    /* OLE2NOTE: the enumeration of formats for a data transfer
    **    document is not a static list. the list of formats offered
    **    may or may not include CF_LINKSOURCE depending on whether a
    **    moniker is available for our document. thus we can NOT use
    **    the default OLE enumerator which enumerates the formats that
    **    are registered for our app in the registration database.
    */
    if (dwDirection == DATADIR_GET) {
        nActualFmts = pOleApp->m_nDocGetFmts;

        /* If the document does not have a Moniker, then exclude
        **    CF_LINKSOURCE and CF_LINKSRCDESCRIPTOR from the list of
		**    formats available. these formats are deliberately listed
		**    last in the array of possible "Get" formats.
        */
        if (! pOleDoc->m_fLinkSourceAvail)
            nActualFmts -= 2;

        *lplpenumFormatEtc = OleStdEnumFmtEtc_Create(
                (short)(nActualFmts), pOleApp->m_arrDocGetFmts);
        if (*lplpenumFormatEtc == NULL) {
        	sc = E_OUTOFMEMORY;
        	goto exit;
       	}

    } else if (dwDirection == DATADIR_SET) {
        /* OLE2NOTE: a document that is used to transfer data
        **    (either via the clipboard or drag/drop does NOT
        **    accept SetData on ANY format!
        */
        sc = E_NOTIMPL;
        goto exit;
    } else {
        sc = E_INVALIDARG;
        goto exit;
    }

exit:
   return ResultFromScode(sc);

#elif qOleContainerApp

    OleApplicationPtr  	pOleApp;
    OleContainerAppPtr	pContainerApp;
    OleContainerDocPtr	pContainerDoc;
    int 		nActualFmts;
    SCODE 		sc = S_OK;
    int			i;

	pOleApp = gOleApp;
	pContainerApp = &pOleApp->container;
	pContainerDoc = &pOleDoc->container;
	
    /* OLE2NOTE: the enumeration of formats for a data transfer
    **    document is not a static list. the list of formats offered
    **    may or may not include CF_LINKSOURCE depending on whether a
    **    moniker is available for our document. thus we can NOT use
    **    the default OLE enumerator which enumerates the formats that
    **    are registered for our app in the registration database.
    */
    if (dwDirection == DATADIR_GET) {
        nActualFmts = pOleApp->m_nDocGetFmts;
        if (pContainerDoc->m_pCopiedEmbedding) {

            /* OLE2NOTE: if this document contains a single OLE object
            **    (ie. cfEmbeddedObject data format is available), then
            **    the formats offered via our enumerator must include
            **    the formats available from the OLE object itself. we
            **    have previously set up a special array of FORMATETC's
            **    in OutlineDoc_CreateDataTransferDoc routine which includes
            **    the combination of data we offer directly and data
            **    offered by the OLE object.
            */

            /* If the document does not have a Moniker, then exclude
            **    CF_LINKSOURCE CF_LINKSRCDESCRIPTOR from the list of
            **    formats available. these formats are deliberately
            **    listed last in the array of possible "Get" formats.
            */
            nActualFmts = pContainerApp->m_nSingleObjGetFmts;
            if (! pOleDoc->m_fLinkSourceAvail)
                nActualFmts -= 2;

            // set correct dwDrawAspect for METAFILEPICT of object copied
            for (i = 0; i < nActualFmts; i++) {
                if (pContainerApp->m_arrSingleObjGetFmts[i].cfFormat ==
                                                            'PICT') {
                    pContainerApp->m_arrSingleObjGetFmts[i].dwAspect =
                            pContainerDoc->m_AspectOleObjCopied;
                    break;  // DONE
                }
            }
            *lplpenumFormatEtc = OleStdEnumFmtEtc_Create(
                    (short)nActualFmts, pContainerApp->m_arrSingleObjGetFmts);
            if (*lplpenumFormatEtc == NULL)
                sc = E_OUTOFMEMORY;

        } else {

            /* This document does NOT offer cfEmbeddedObject,
            **    therefore we can simply enumerate the
            **    static list of formats that we handle directly.
            */
            *lplpenumFormatEtc = OleStdEnumFmtEtc_Create(
                    pOleApp->m_nDocGetFmts, pOleApp->m_arrDocGetFmts);
            if (*lplpenumFormatEtc == NULL)
                sc = E_OUTOFMEMORY;
        }

    } else if (dwDirection == DATADIR_SET) {
        /* OLE2NOTE: a document that is used to transfer data
        **    (either via the clipboard or drag/drop does NOT
        **    accept SetData on ANY format!
        */
        sc = E_NOTIMPL;
        goto exit;
    } else {
        sc = E_INVALIDARG;
        goto exit;
    }

exit:
   return ResultFromScode(sc);

#endif
}

/* OleDataXferDocGetObjectDescriptorData
 * -------------------------------------
 *
 * Return a handle to an object's data in CF_OBJECTDESCRIPTOR form
 *
 */
Handle OleDataXferDocGetObjectDescriptorData(OleDocumentPtr pOleDoc)
{
#if qOleServerApp
    {
        OleServerDocPtr	pServerDoc = &pOleDoc->server;
        SIZEL         	sizel;
        POINTL        	pointl;
        char*         	lpszSrcOfCopy = NULL;
        IBindCtx   	*pbc = NULL;
        Handle       	hObjDesc;
        unsigned long         	dwStatus = 0;
        OleDocumentPtr 	pSrcDocOfCopy = pOleDoc->m_pSrcDocOfCopy;

        IServerDocOleObjectGetMiscStatus(
                (LPOLEOBJECT)&pServerDoc->m_OleObject,
                DVASPECT_CONTENT,
                &dwStatus
        );

		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetExtentProcPtr != nil);
        (*pOleDoc->m_pIDoc->lpVtbl->m_GetExtentProcPtr)(pOleDoc->m_pIDoc, nil, &sizel);
        pointl.x = pointl.y = 0;

        if (pServerDoc->m_lpSrcMonikerOfCopy) {
            CreateBindCtx(0, (LPBC *)&pbc);
            pServerDoc->m_lpSrcMonikerOfCopy->lpVtbl->GetDisplayName(
                pServerDoc->m_lpSrcMonikerOfCopy, pbc, NULL, &lpszSrcOfCopy);
            pbc->lpVtbl->Release(pbc);
        } else {
            /* this document has no moniker; use our FullUserTypeName
            **    as the description of the source of copy.
            */
            lpszSrcOfCopy = kOutlineServerFullUserTypeName;
        }

        hObjDesc =  OleStdGetObjectDescriptorData(
                CLSID_Application,
                DVASPECT_CONTENT,
                sizel,
                pointl,
                dwStatus,
                kOutlineServerFullUserTypeName,
                lpszSrcOfCopy
        );

        if (pServerDoc->m_lpSrcMonikerOfCopy && lpszSrcOfCopy)
            OleStdFreeString(lpszSrcOfCopy, NULL);
	
		return hObjDesc;
	}
#elif qOleContainerApp
	{
		OleContainerDocPtr	pContainerDoc = &pOleDoc->container;
		Handle 				hObjDesc;
		LPOLEOBJECT 		pOleObj;
		SIZEL sizel;
		POINTL pointl;
	
		pointl.x = pointl.y = 0;
	
		if (pContainerDoc->m_pCopiedEmbedding) {
	
			OleDocumentPtr pSrcOleDoc = pOleDoc->m_pSrcDocOfCopy;
			FSSpec	file;
			char* lpszSrcFileName;
		
			/* OLE2NOTE: a single OLE object is being transfered via
			**    this DataTransferDoc. we need to generate the
			**    CF_ObjectDescrioptor which describes the OLE object.
			*/
	
			ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetObjectInterfaceProcPtr != nil);
			pOleObj = (LPOLEOBJECT)(*pOleDoc->m_pIDoc->lpVtbl->m_GetObjectInterfaceProcPtr)(
										pOleDoc->m_pIDoc,
										pContainerDoc->m_pCopiedEmbedding,
										&IID_IOleObject);
			
			if (!pOleObj)
				return nil;
				
			ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetFSSpecProcPtr != nil);
			(*pOleDoc->m_pIDoc->lpVtbl->m_GetFSSpecProcPtr)(pOleDoc->m_pIDoc, &file);
			p2cstr(file.name);
				
			lpszSrcFileName = (char*)file.name;

// BUG here	
			hObjDesc = OleStdGetObjectDescriptorDataFromOleObject(
						pOleObj,
						lpszSrcFileName,
						pContainerDoc->m_AspectOleObjCopied,
						pointl
					);
	
			OleStdRelease((LPUNKNOWN)pOleObj);
			
			return hObjDesc;
	
		} else {
			/* OLE2NOTE: the data being transfered via this
			**    DataTransferDoc is NOT a single OLE object. thus in
			**    this case the CF_ObjectDescriptor data should
			**    describe our container app itself.
			*/
			ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetExtentProcPtr != nil);
			(*pOleDoc->m_pIDoc->lpVtbl->m_GetExtentProcPtr)(pOleDoc->m_pIDoc, nil, &sizel);

			return OleStdGetObjectDescriptorData(
					CLSID_NULL, /* not used if no object formats */
					DVASPECT_CONTENT,
					sizel,
					pointl,
					0,
					NULL,       /* UserTypeName not used if no obj fmt's */
                    kOutlineContainerFullUserTypeName   /* string to identify source of copy */
			);
	
		}
	}
#endif
}

