/*****************************************************************************\
*                                                                             *
*    OleExceptions.c                                                          *
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
#include "App.h"
#include "Const.h"
#include "OleXcept.h"
#include <Resources.h>

#if qOleInPlace
#include "Doc.h"
#include "Gopher.h"
#include "Util.h"
#include "OleDebug.h"
#endif  // qOleInPlace

#include "ole2ui.h"
#include "app.h"

OLEDBGDATA

// OLDNAME: OleContainerApp.c
//#pragma segment OleContainerAppInitSeg
void OleContainerAppInit(OleApplicationPtr pOleApp)
{
#if qOleInPlace
		OleInPlaceContainerAppInitInterfaces();
		OleInPlaceContainerDocInitInterfaces();
		OleInPlaceContainerSiteInitInterfaces();
		
		OleInPlaceContainerAppInit(pOleApp);
#endif
}

//#pragma segment OleContainerAppSeg
void OleContainerAppDispose(OleApplicationPtr pOleApp)
{
#if qOleInPlace
	OleInPlaceContainerAppDispose(pOleApp);
#endif
}

//#pragma segment OleContainerAppSeg
void OleContainerAppAddSingleObjGetFormat(OleApplicationPtr pOleApp, LPFORMATETC pFormatetc)
{
	if (pOleApp->container.m_nSingleObjGetFmts < kMaxNumOfMTS)
		pOleApp->container.m_arrSingleObjGetFmts[pOleApp->container.m_nSingleObjGetFmts++] = *pFormatetc;
}

//#pragma segment OleContainerAppSeg
HRESULT OleContainerAppLocalQueryInterface(OleApplicationPtr pOleApp, REFIID riid, void* * lplpvObj)
{
#if qOleInPlace
	return OleInPlaceContainerAppLocalQueryInterface(pOleApp, riid, lplpvObj);
#endif

	*lplpvObj = NULL;
	return ResultFromScode(E_NOINTERFACE);
}



// OLDNAME: OleOutlineContainer.c

//#pragma segment OleOutlineContainerSeg
void OleOutlineContainerInit(OleOutlineAppPtr pOleOutlineApp, OSType creator)
{
	OleApplicationPtr	pOleApp;
	MenuHandle			hMenu;
	
	pOleApp = &pOleOutlineApp->m_OleApp;

	hMenu = GetMenu(kObject_MENU);
	ASSERTCOND(hMenu != nil);
	InsertMenu(hMenu, hierMenu);
	
    /* Setup arrays used by IDataObject::EnumFormatEtc. This is used to
    **    support copy/paste and drag/drop operations.
    **
    ** OLE2NOTE: The order that the formats are listed for GetData is very
    **    significant. It should be listed in order of highest fidelity
    **    formats to least fidelity formats. A common ordering will be:
    **                  1. private app formats
    **                  2. CF_EMBEDSOURCE or CF_EMBEDOBJECT (as appropriate)
    **                  3. lower fidelity interchange formats
    **                  4. 'PICT'
    **                      (graphic-related apps might offer picture 1st!)
    **                  5. CF_OBJECTDESCRIPTOR
    **                  6. CF_LINKSOURCE
    **                  6. CF_LINKSRCDESCRIPTOR
    */

    /* m_arrDocGetFmts array enumerates the formats that a ContainerDoc
    **    object can offer (give) through a IDataObject::GetData call
	**    when the selection copied is NOT a single embedded object.
	**    when a single embedded object this list of formats available
	**    is built dynamically depending on the object copied. (see
	**    ContainerDoc_SetupDocGetFmts).
    **    The formats are listed in priority order.
    **    ContainerDoc objects accept data formats in the following order:
    **                  1. CF_CNTROUTL
    **                  2. CF_OUTLINE
    **                  3. 'TEXT'
    **                  4. CF_OBJECTDESCRIPTOR
	**
	**    OLE2NOTE: CF_OBJECTDESCRIPTOR format is used to describe the
	**    data on the clipboard. this information is intended to be
	**    used, for example, to drive the PasteSpecial dialog. it is
	**    useful to render CF_OBJECTDESCRIPTOR format even when the
	**    data on the clipboard does NOT include CF_EMBEDDEDOBJECT
	**    format or CF_EMBEDSOURCE format as when a selection that is
	**    not a single OLE object is copied from the container only
	**    version CNTROUTL. by rendering CF_OBJECTDESCRIPTOR format the
	**    app can indicate a useful string to identifiy the source of
	**    the copy to the user.
    */
	{
		FORMATETC		Fmt;

	    Fmt.cfFormat = OleOutlineAppGetClipboardFormat(pOleOutlineApp);
	    Fmt.ptd      = NULL;
	    Fmt.dwAspect = DVASPECT_CONTENT;
	    Fmt.tymed    = TYMED_ISTORAGE;
	    Fmt.lindex   = -1;
		OleAppAddDocGetFormat(pOleApp, &Fmt);

		Fmt.cfFormat	= 'TEXT';
		Fmt.ptd			= NULL;
		Fmt.dwAspect	= DVASPECT_CONTENT;
		Fmt.tymed		= TYMED_HGLOBAL;
		Fmt.lindex		= -1;
		OleAppAddDocGetFormat(pOleApp, &Fmt);

		Fmt.cfFormat	= pOleApp->m_cfObjectDescriptor;
		Fmt.ptd			= NULL;
		Fmt.dwAspect	= DVASPECT_CONTENT;
		Fmt.tymed		= TYMED_HGLOBAL;
		Fmt.lindex		= -1;
		OleAppAddDocGetFormat(pOleApp, &Fmt);

	    /* m_arrSingleObjGetFmts array enumerates the formats that a
	    **    ContainerDoc object can offer (give) through a
	    **    IDataObject::GetData call when the selection copied IS a
	    **    single OLE object.
	    **    ContainerDoc objects accept data formats in the following order:
	    **                  1. CF_CNTROUTL
	    **                  2. CF_EMBEDDEDOBJECT
	    **                  3. CF_OBJECTDESCRIPTOR
	    **                  4. 'PICT'  (note DVASPECT will vary)
	    **                  5. CF_OUTLINE
	    **                  6. 'TEXT'
	    **                  7. CF_LINKSOURCE *
	    **                  8. CF_LINKSRCDESCRIPTOR *
	    **
	    **    * OLE2NOTE: CF_LINKSOURCE and CF_LINKSRCDESCRIPTOR is only
		**    offered if the OLE object is allowed to be linked to from the
	    **    inside (ie. we are allowed to give out a moniker which binds
	    **    to the running OLE object), then we want to offer
	    **    CF_LINKSOURCE format. if the object is an OLE 2.0 embedded
	    **    object then it is allowed to be linked to from the inside. if
	    **    the object is either an OleLink or an OLE 1.0 embedding then
	    **    it can not be linked to from the inside. if we were a
	    **    container/server app then we could offer linking to the
	    **    outside of the object (ie. a pseudo object within our
	    **    document). we are a container only app that does not support
	    **    linking to ranges of its data.
	    **    the simplest way to determine if an object can be linked to
	    **    on the inside is to call IOleObject::GetMiscStatus and test
	    **    to see if the OLEMISC_CANTLINKINSIDE bit is NOT set.
	    **
	    **    OLE2NOTE: optionally, a container that wants to have a
	    **    potentially richer data transfer, can enumerate the data
	    **    formats from the OLE object's cache and offer them too. if
	    **    the object has a special handler, then it might be able to
	    **    render additional data formats.
	    */
	    Fmt.cfFormat = OleOutlineAppGetClipboardFormat(pOleOutlineApp);
	    Fmt.ptd      = NULL;
	    Fmt.dwAspect = DVASPECT_CONTENT;
	    Fmt.tymed    = TYMED_ISTORAGE;
	    Fmt.lindex   = -1;
	
		OleContainerAppAddSingleObjGetFormat(pOleApp, &Fmt);
		
	    Fmt.cfFormat = pOleApp->m_cfEmbeddedObject;
	    Fmt.ptd      = NULL;
	    Fmt.dwAspect = DVASPECT_CONTENT;
	    Fmt.tymed    = TYMED_ISTORAGE;
	    Fmt.lindex   = -1;
	
		OleContainerAppAddSingleObjGetFormat(pOleApp, &Fmt);
	
	    Fmt.cfFormat = pOleApp->m_cfObjectDescriptor;
	    Fmt.ptd      = NULL;
	    Fmt.dwAspect = DVASPECT_CONTENT;
	    Fmt.tymed    = TYMED_HGLOBAL;
	    Fmt.lindex   = -1;
	
		OleContainerAppAddSingleObjGetFormat(pOleApp, &Fmt);
	
	    Fmt.cfFormat = 'PICT';
	    Fmt.ptd      = NULL;
	    Fmt.dwAspect = DVASPECT_CONTENT;
	    Fmt.tymed    = TYMED_MFPICT;
	    Fmt.lindex   = -1;
	
		OleContainerAppAddSingleObjGetFormat(pOleApp, &Fmt);
		
	    Fmt.cfFormat = 'TEXT';
	    Fmt.ptd      = NULL;
	    Fmt.dwAspect = DVASPECT_CONTENT;
	    Fmt.tymed    = TYMED_HGLOBAL;
	    Fmt.lindex   = -1;
	
		OleContainerAppAddSingleObjGetFormat(pOleApp, &Fmt);
	
	    Fmt.cfFormat = pOleApp->m_cfLinkSource;
	    Fmt.ptd      = NULL;
	    Fmt.dwAspect = DVASPECT_CONTENT;
	    Fmt.tymed    = TYMED_ISTREAM;
	    Fmt.lindex   = -1;
	
		OleContainerAppAddSingleObjGetFormat(pOleApp, &Fmt);
	
	    Fmt.cfFormat = pOleApp->m_cfLinkSrcDescriptor;
	    Fmt.ptd      = NULL;
	    Fmt.dwAspect = DVASPECT_CONTENT;
	    Fmt.tymed    = TYMED_HGLOBAL;
	    Fmt.lindex   = -1;
	
		OleContainerAppAddSingleObjGetFormat(pOleApp, &Fmt);
	}

    /* NOTE: the Container-Only version of Outline does NOT offer
    **    IDataObject interface from its User documents and the
    **    IDataObject interface available from DataTransferDoc's do NOT
    **    support SetData. IDataObject interface is required by objects
    **    which can be embedded or linked. the Container-only app only
    **    allows linking to its contained objects, NOT the data of the
    **    container itself.
    */

    /*    m_arrPasteEntries array enumerates the formats that a ContainerDoc
    **    object can accept from the clipboard. this array is used to
    **    support the PasteSpecial dialog.
    **    The formats are listed in priority order.
    **    ContainerDoc objects accept data formats in the following order:
    **                  1. CF_CNTROUTL
    **                  2. CF_OUTLINE
    **                  3. CF_EMBEDDEDOBJECT
    **                  4. 'TEXT'
    **                  5. 'PICT'
    **                  6. 'DIB '
    **                  7. 'BMAP'
    **                  8. CF_LINKSOURCE
	**
	**    NOTE: specifying CF_EMBEDDEDOBJECT in the PasteEntry array
    **    indicates that the caller is interested in pasting OLE
    **    objects (ie. the caller calls OleCreateFromData). the
    **    OleUIPasteSpecial dialog and OleStdGetPriorityClipboardFormat
    **    call OleQueryCreateFromData to see if an OLE object format is
    **    available. thus, in fact if CF_EMBEDSOURCE or CF_FILENAME are
    **    available from the data source then and OLE object can be
    **    created and this entry will be matched. the caller should
    **    only specify one object type format.
    **    CF_FILENAME format (as generated by copying a file to
	**    the clipboard from the FileManager) is considered an object
    **    format; OleCreatFromData creates an object if the file has an
	**    associated class (see GetClassFile API) or if no class it
    **    creates an OLE 1.0 Package object. this format can also be
    **    paste linked by calling OleCreateLinkFromData.
    */
	{
		OLEUIPASTEENTRY		PasteEntry;
		Handle				h;

	    PasteEntry.fmtetc.cfFormat = OleOutlineAppGetClipboardFormat(pOleOutlineApp);
	    PasteEntry.fmtetc.ptd      = NULL;
	    PasteEntry.fmtetc.dwAspect = DVASPECT_CONTENT;
	    PasteEntry.fmtetc.tymed    = TYMED_ISTORAGE;
	    PasteEntry.fmtetc.lindex   = -1;
	
		h = Get1Resource('CSTR', kOutlineFormat_CSTR);
		ASSERTCOND(h != nil);
		FailNIL(h);
		PasteEntry.lpstrFormatName	= *h;
	
		h = Get1Resource('CSTR', kOutlineResult_CSTR);
		ASSERTCOND(h != nil);
		FailNIL(h);
		PasteEntry.lpstrResultText	= *h;
	
	    PasteEntry.dwFlags         = OLEUIPASTE_PASTEONLY;
	
		OleAppAddPasteEntry(pOleApp, &PasteEntry);

	    PasteEntry.fmtetc.cfFormat = pOleApp->m_cfEmbeddedObject;
	    PasteEntry.fmtetc.ptd      = NULL;
	    PasteEntry.fmtetc.dwAspect = DVASPECT_CONTENT;
	    PasteEntry.fmtetc.tymed    = TYMED_ISTORAGE;
	    PasteEntry.fmtetc.lindex   = -1;

		h = Get1Resource('CSTR', kEmbeddedObjectFormat_CSTR);
		ASSERTCOND(h != nil);
		FailNIL(h);
		PasteEntry.lpstrFormatName	= *h;

		h = Get1Resource('CSTR', kEmbeddedObjectResult_CSTR);
		ASSERTCOND(h != nil);
		FailNIL(h);
		PasteEntry.lpstrResultText	= *h;

	    PasteEntry.dwFlags         = OLEUIPASTE_PASTE | OLEUIPASTE_ENABLEICON;

		OleAppAddPasteEntry(pOleApp, &PasteEntry);

		PasteEntry.fmtetc.cfFormat	= 'TEXT';
		PasteEntry.fmtetc.ptd		= NULL;
		PasteEntry.fmtetc.dwAspect	= DVASPECT_CONTENT;
		PasteEntry.fmtetc.tymed		= TYMED_HGLOBAL;
		PasteEntry.fmtetc.lindex	= -1;

		h = Get1Resource('CSTR', kTextFormat_CSTR);
		ASSERTCOND(h != nil);
		FailNIL(h);
		PasteEntry.lpstrFormatName	= *h;

		h = Get1Resource('CSTR', kTextResult_CSTR);
		ASSERTCOND(h != nil);
		FailNIL(h);
		PasteEntry.lpstrResultText	= *h;

		PasteEntry.dwFlags			= OLEUIPASTE_PASTEONLY;

		OleAppAddPasteEntry(pOleApp, &PasteEntry);

	    PasteEntry.fmtetc.cfFormat = 'PICT';
	    PasteEntry.fmtetc.ptd      = NULL;
	    PasteEntry.fmtetc.dwAspect = DVASPECT_CONTENT;
	    PasteEntry.fmtetc.tymed    = TYMED_MFPICT;
	    PasteEntry.fmtetc.lindex   = -1;

		h = Get1Resource('CSTR', kPictFormat_CSTR);
		ASSERTCOND(h != nil);
		FailNIL(h);
		PasteEntry.lpstrFormatName	= *h;

		h = Get1Resource('CSTR', kPictResult_CSTR);
		ASSERTCOND(h != nil);
		FailNIL(h);
		PasteEntry.lpstrResultText	= *h;

	    PasteEntry.dwFlags         = OLEUIPASTE_PASTEONLY;
	
		OleAppAddPasteEntry(pOleApp, &PasteEntry);
	
	    PasteEntry.fmtetc.cfFormat = 'DIB ';
	    PasteEntry.fmtetc.ptd      = NULL;
	    PasteEntry.fmtetc.dwAspect = DVASPECT_CONTENT;
	    PasteEntry.fmtetc.tymed    = TYMED_HGLOBAL;
	    PasteEntry.fmtetc.lindex   = -1;

		h = Get1Resource('CSTR', kDIBFormat_CSTR);
		ASSERTCOND(h != nil);
		FailNIL(h);
		PasteEntry.lpstrFormatName	= *h;

		h = Get1Resource('CSTR', kDIBResult_CSTR);
		ASSERTCOND(h != nil);
		FailNIL(h);
		PasteEntry.lpstrResultText	= *h;

	    PasteEntry.dwFlags         = OLEUIPASTE_PASTEONLY;
	
		OleAppAddPasteEntry(pOleApp, &PasteEntry);
	
	    PasteEntry.fmtetc.cfFormat = 'BMAP';
	    PasteEntry.fmtetc.ptd      = NULL;
	    PasteEntry.fmtetc.dwAspect = DVASPECT_CONTENT;
	    PasteEntry.fmtetc.tymed    = TYMED_HGLOBAL;
	    PasteEntry.fmtetc.lindex   = -1;

		h = Get1Resource('CSTR', kBitmapFormat_CSTR);
		ASSERTCOND(h != nil);
		FailNIL(h);
		PasteEntry.lpstrFormatName	= *h;

		h = Get1Resource('CSTR', kBitmapResult_CSTR);
		ASSERTCOND(h != nil);
		FailNIL(h);
		PasteEntry.lpstrResultText	= *h;

	    PasteEntry.dwFlags         = OLEUIPASTE_PASTEONLY;
	
		OleAppAddPasteEntry(pOleApp, &PasteEntry);
	
	    PasteEntry.fmtetc.cfFormat = pOleApp->m_cfLinkSource;
	    PasteEntry.fmtetc.ptd      = NULL;
	    PasteEntry.fmtetc.dwAspect = DVASPECT_CONTENT;
	    PasteEntry.fmtetc.tymed    = TYMED_ISTREAM;
	    PasteEntry.fmtetc.lindex   = -1;

		h = Get1Resource('CSTR', kLinkSourceFormat_CSTR);
		ASSERTCOND(h != nil);
		FailNIL(h);
		PasteEntry.lpstrFormatName	= *h;

		h = Get1Resource('CSTR', kLinkSourceResult_CSTR);
		ASSERTCOND(h != nil);
		FailNIL(h);
		PasteEntry.lpstrResultText	= *h;

	    PasteEntry.dwFlags         = OLEUIPASTE_LINKTYPE1 | OLEUIPASTE_ENABLEICON;
	
		OleAppAddPasteEntry(pOleApp, &PasteEntry);
	
	}

	{
	    /*    m_arrLinkTypes array enumerates the link types that a ContainerDoc
    	**    object can accept from the clipboard
    	*/
    	OleAppAddLinkType(pOleApp, pOleApp->m_cfLinkSource);
	}

}

// OLDNAME: OleInPlaceContainerApp.c


#if qOleInPlace

//#pragma segment OleInPlaceContainerAppInitSeg
void OleInPlaceContainerAppInit(OleApplicationPtr pOleApp)
{
	OleInPlaceContainerIFrameInit(&pOleApp->container.inplace.m_Frame, pOleApp);
}

//#pragma segment OleInPlaceContainerAppSeg
void OleInPlaceContainerAppDispose(OleApplicationPtr pOleApp)
{
}

//#pragma segment OleInPlaceContainerAppSeg
HRESULT OleInPlaceContainerAppLocalQueryInterface(OleApplicationPtr pOleApp, REFIID riid, void* * lplpvObj)
{
	if (IsEqualIID(riid, &IID_IOleWindow)
			|| IsEqualIID(riid, &IID_IOleInPlaceUIWindow)
			|| IsEqualIID(riid, &IID_IOleInPlaceFrame))
	{
		*lplpvObj = &pOleApp->container.inplace.m_Frame;
		OleAppAddRef(pOleApp);
		return NOERROR;
	}

	*lplpvObj = NULL;
	return ResultFromScode(E_NOINTERFACE);
}

//#pragma segment OleInPlaceContainerAppSeg
LPOLEINPLACEFRAME OleInPlaceContainerAppGetIPFrame(OleApplicationPtr pOleApp)
{
	OleAppAddRef(pOleApp);
	return (LPOLEINPLACEFRAME)&pOleApp->container.inplace.m_Frame;
}

//#pragma segment OleInPlaceContainerAppSeg
HRESULT OleInPlaceContainerAppInsertMenus(OleApplicationPtr pOleApp, OleMBarHandle hOleMBar)
{
	HRESULT		hrErr;
	
	ASSERTCOND(pOleApp->m_pIApp->lpVtbl->m_InPlaceInsertMenusProcPtr != nil);
	hrErr = (*pOleApp->m_pIApp->lpVtbl->m_InPlaceInsertMenusProcPtr)(pOleApp->m_pIApp, hOleMBar);
	ASSERTNOERROR(hrErr);
	
	OlePatchGetMHandle(hOleMBar);
	
	return NOERROR;
}

//#pragma segment OleInPlaceContainerAppSeg
HRESULT OleInPlaceContainerAppAdjustMenus(OleApplicationPtr pOleApp, OleMBarHandle hOleMBar)
{
	HRESULT		hrErr;
	
	UpdateMenus();
	
	return NOERROR;
}

//#pragma segment OleInPlaceContainerAppSeg
HRESULT OleInPlaceContainerAppRemoveMenus(OleApplicationPtr pOleApp, OleMBarHandle hOleMBar)
{
	HRESULT		hrErr;
	
	OleUnpatchGetMHandle(hOleMBar);
	
	return NOERROR;
}

//#pragma segment OleInPlaceContainerAppSeg
HRESULT OleInPlaceContainerAppTranslateAccelerator(OleApplicationPtr pOleApp, EventRecord* pEvent, long mResult)
{
	ASSERTCOND(pOleApp->m_pIApp->lpVtbl->m_InPlaceDoMenuProcPtr != nil);
	return (*pOleApp->m_pIApp->lpVtbl->m_InPlaceDoMenuProcPtr)(pOleApp->m_pIApp, pEvent, mResult);
}

//#pragma segment OleInPlaceContainerAppSeg
HRESULT OleInPlaceContainerAppGetBorder(OleApplicationPtr pOleApp, Rect * lprectBorder)
{
	ASSERTCOND(pOleApp->m_pIApp->lpVtbl->m_InPlaceGetBorderProcPtr != nil);
	(*pOleApp->m_pIApp->lpVtbl->m_InPlaceGetBorderProcPtr)(pOleApp->m_pIApp, lprectBorder);

	return NOERROR;
}

//#pragma segment OleInPlaceContainerAppSeg
HRESULT OleInPlaceContainerAppSetBorderSpace(OleApplicationPtr pOleApp, LPCBORDERWIDTHS lpborderwidths)
{
	ASSERTCOND(pOleApp->m_pIApp->lpVtbl->m_InPlaceSetBorderSpaceProcPtr != nil);
	return (*pOleApp->m_pIApp->lpVtbl->m_InPlaceSetBorderSpaceProcPtr)(pOleApp->m_pIApp, lpborderwidths);
}

//#pragma segment OleInPlaceContainerAppSeg
HRESULT OleInPlaceContainerAppSetActiveObject(OleApplicationPtr pOleApp, LPOLEINPLACEACTIVEOBJECT pActiveObject)
{
	if (pActiveObject == pOleApp->container.inplace.m_pActiveObject)
		return NOERROR;

	if (pOleApp->container.inplace.m_pActiveObject)
	{
		pOleApp->container.inplace.m_pActiveObject->lpVtbl->Release(pOleApp->container.inplace.m_pActiveObject);
		pOleApp->container.inplace.m_pActiveObject = nil;
	}
		
	if (pActiveObject)
	{
		pOleApp->container.inplace.m_pActiveObject = pActiveObject;
		pOleApp->container.inplace.m_pActiveObject->lpVtbl->AddRef(pOleApp->container.inplace.m_pActiveObject);
	}
	
	return NOERROR;
}

//#pragma segment OleInPlaceContainerAppSeg
void OleInPlaceContainerAppDoNew(OleApplicationPtr pOleApp)
{
	OleDocumentPtr		pOleDoc;

	ASSERTCOND(pOleApp->m_pIApp->lpVtbl->m_GetCurrentOleDocProcPtr != nil);
	pOleDoc = (*pOleApp->m_pIApp->lpVtbl->m_GetCurrentOleDocProcPtr)(pOleApp->m_pIApp);

	if (pOleDoc && OleInPlaceContainerDocIsUIVisible(pOleDoc))
	{
		OleInPlaceContainerDocDoActivate(pOleDoc, false);

		// ignore the next resume event
		pOleApp->container.inplace.m_fIgnoreResumeEvent = true;

		// bring ourselves to the foreground
		ASSERTCOND(pOleApp->m_pIApp->lpVtbl->m_ShowProcPtr != nil);
		(*pOleApp->m_pIApp->lpVtbl->m_ShowProcPtr)(pOleApp->m_pIApp);
	}
}

//#pragma segment OleInPlaceContainerAppSeg
Boolean OleInPlaceContainerAppDoSuspend(OleApplicationPtr pOleApp)
{
	// we need to ignore this suspend;
	if (pOleApp->container.inplace.m_fIgnoreSuspendEvent)
	{
		OleDocumentPtr		pOleDoc;
		WindowPtr			pWindow;

		ASSERTCOND(pOleApp->m_pIApp->lpVtbl->m_GetCurrentOleDocProcPtr != nil);
		pOleDoc = (*pOleApp->m_pIApp->lpVtbl->m_GetCurrentOleDocProcPtr)(pOleApp->m_pIApp);
		ASSERTCOND(pOleDoc != nil);

		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr != nil);
		pWindow = (*pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr)(pOleDoc->m_pIDoc);
		ASSERTCOND(pWindow != nil);

		HiliteWindow(pWindow, true);

		pOleApp->container.inplace.m_fIgnoreSuspendEvent = false;
		return false;
	}
	else
	{
		OleDocumentPtr		pOleDoc;
		HRESULT				hrErr;

		ASSERTCOND(pOleApp->m_pIApp->lpVtbl->m_GetCurrentOleDocProcPtr != nil);
		pOleDoc = (*pOleApp->m_pIApp->lpVtbl->m_GetCurrentOleDocProcPtr)(pOleApp->m_pIApp);
		
		if (pOleDoc && OleInPlaceContainerDocIsUIVisible(pOleDoc))
		{
			ASSERTCOND(pOleDoc->container.inplace.m_pActiveObject != nil);
			hrErr = pOleDoc->container.inplace.m_pActiveObject->lpVtbl->OnFrameWindowActivate(pOleDoc->container.inplace.m_pActiveObject, false);
			ASSERTNOERROR(hrErr);
		}
	}
	
	return true;
}

//#pragma segment OleInPlaceContainerAppSeg
Boolean OleInPlaceContainerAppDoResume(OleApplicationPtr pOleApp)
{
	// we need to ignore this suspend;
	if (pOleApp->container.inplace.m_fIgnoreResumeEvent)
	{
		pOleApp->container.inplace.m_fIgnoreResumeEvent = false;
		return false;
	}
	else
	{
		EventRecord			theEvent;
		OleDocumentPtr		pOleDoc;

		if (EventAvail(mDownMask, &theEvent))
		{
			WindowPtr	pWindow;

			FindWindow(theEvent.where, &pWindow);

			// when resuming we normally check if an inplace session is active in the
			// front window and resume that session. if a mousedown has occurred in a
			// background window we want to skip the process of resuming inplace and
			// simply activate the other document.
			if (pWindow != FrontWindow())
			{
				SelectWindow(pWindow);
				return false;
			}
		}

		ASSERTCOND(pOleApp->m_pIApp->lpVtbl->m_GetCurrentOleDocProcPtr != nil);
		pOleDoc = (*pOleApp->m_pIApp->lpVtbl->m_GetCurrentOleDocProcPtr)(pOleApp->m_pIApp);

		if (pOleDoc && OleInPlaceContainerDocIsUIActive(pOleDoc))
		{	
			HRESULT		hrErr;

			// if an open document ae is pending we want to ignore this resume
			// event. we have special-case code in the 'ODOC' ae handler that will
			// resume the inplace session if we don't actually open another document
			// (the user might've opened the same document which is hosting the
			// inplace session in which case we simply resume inplace).
			if (EventAvail(highLevelEventMask, &theEvent))
			{
				if (theEvent.message == kCoreEventClass && *(long *)&theEvent.where == kAEOpenDocuments)
					return false;
			}

			if (OleInPlaceContainerDocIsUIVisible(pOleDoc))
			{
				// if the object is uivisible then the user is switching from the server
				// to the container using the application menu. we need to go uideactive
				// in this case.
				pOleDoc->container.inplace.m_fIgnoreOnUIVisible = true;
				OleInPlaceContainerDocUIDeactivate(pOleDoc);
				return false;
			}
			else
			{
				// if the object isn't uivisible then the user is switching from a third application
				// to the container and we want to go frame active.
				ASSERTCOND(pOleDoc->container.inplace.m_pActiveObject != nil);
				hrErr = pOleDoc->container.inplace.m_pActiveObject->lpVtbl->OnFrameWindowActivate(pOleDoc->container.inplace.m_pActiveObject, true);
				ASSERTNOERROR(hrErr);
			}
		}
	}

	return true;
}

//#pragma segment OleInPlaceContainerAppSeg
#ifndef _MSC_VER
pascal OSErr
#else
OSErr __pascal
#endif
OleInPlaceContainerRemoteEvent(AppleEvent* theAppleEvent, AppleEvent* reply, OleApplicationPtr pOleApp)
{
	OSErr			theErr;
	long			cb;
	DescType		descType;
	EventRecord		theEvent;

	theErr = AEGetKeyPtr(theAppleEvent, keyDirectObject, typeWildCard,  &descType, (Ptr) &theEvent, sizeof(theEvent), &cb);
	ASSERTCOND(theErr == noErr);
	
	ASSERTCOND(pOleApp->m_pIApp->lpVtbl->m_ProcessEventProcPtr != nil);
	(*pOleApp->m_pIApp->lpVtbl->m_ProcessEventProcPtr)(pOleApp->m_pIApp, &theEvent);

	return theErr;
}

// OLDNAME: OleInPlaceContainerAppInt.c
static IOleInPlaceFrameVtbl		gOleInPlaceContainerAppFrameVtbl;

//#pragma segment OleInPlaceContainerAppIntInitSeg
void OleInPlaceContainerAppInitInterfaces(void)
{
	// OleInPlace::Frame method table
	{
		IOleInPlaceFrameVtbl*		p;
		
		p = &gOleInPlaceContainerAppFrameVtbl;
		
		p->QueryInterface			= IOleInPlaceContainerAppFrameQueryInterface;
		p->AddRef					= IOleInPlaceContainerAppFrameAddRef;
		p->Release					= IOleInPlaceContainerAppFrameRelease;
		p->GetWindow				= IOleInPlaceContainerAppFrameGetWindow;
		p->ContextSensitiveHelp		= IOleInPlaceContainerAppFrameContextSensitiveHelp;
		p->GetBorder				= IOleInPlaceContainerAppFrameGetBorder;
		p->RequestBorderSpace		= IOleInPlaceContainerAppFrameRequestBorderSpace;
		p->SetBorderSpace			= IOleInPlaceContainerAppFrameSetBorderSpace;
		p->SetActiveObject			= IOleInPlaceContainerAppFrameSetActiveObject;
		p->InsertMenus				= IOleInPlaceContainerAppFrameInsertMenus;
		p->AdjustMenus				= IOleInPlaceContainerAppFrameAdjustMenus;
		p->RemoveMenus				= IOleInPlaceContainerAppFrameRemoveMenus;
		p->SetStatusText			= IOleInPlaceContainerAppFrameSetStatusText;
		p->EnableModeless			= IOleInPlaceContainerAppFrameEnableModeless;
		p->TranslateAccelerator		= IOleInPlaceContainerAppFrameTranslateAccelerator;

		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}
}

//#pragma segment OleInPlaceContainerAppIntSeg
void OleInPlaceContainerIFrameInit(OleInPlaceContainerFrameImplPtr pOleInPlaceContainerFrameImpl, struct OleApplicationRec* pOleApp)
{
	pOleInPlaceContainerFrameImpl->lpVtbl				= &gOleInPlaceContainerAppFrameVtbl;
	pOleInPlaceContainerFrameImpl->lpOleApp				= pOleApp;
	pOleInPlaceContainerFrameImpl->cRef					= 0;
}

//#pragma segment OleInPlaceContainerAppIntSeg
STDMETHODIMP IOleInPlaceContainerAppFrameQueryInterface(LPOLEINPLACEFRAME lpThis, REFIID riid, void * * ppvObj)
{
	OleApplicationPtr		pOleApp;
	
	OleDbgEnterInterface();
	
	pOleApp = ((OleInPlaceContainerFrameImplPtr)lpThis)->lpOleApp;

	return OleAppQueryInterface(pOleApp, riid, ppvObj);
}

//#pragma segment OleInPlaceContainerAppIntSeg
STDMETHODIMP_(unsigned long) IOleInPlaceContainerAppFrameAddRef(LPOLEINPLACEFRAME lpThis)
{
	OleApplicationPtr		pOleApp;
	
	OleDbgEnterInterface();
	
	pOleApp = ((OleInPlaceContainerFrameImplPtr)lpThis)->lpOleApp;

	return OleAppAddRef(pOleApp);
}

//#pragma segment OleInPlaceContainerAppIntSeg
STDMETHODIMP_(unsigned long) IOleInPlaceContainerAppFrameRelease(LPOLEINPLACEFRAME lpThis)
{
	OleApplicationPtr		pOleApp;
	
	OleDbgEnterInterface();
	
	pOleApp = ((OleInPlaceContainerFrameImplPtr)lpThis)->lpOleApp;

	return OleAppRelease(pOleApp);
}

//#pragma segment OleInPlaceContainerAppIntSeg
STDMETHODIMP IOleInPlaceContainerAppFrameGetWindow(LPOLEINPLACEFRAME lpThis, WindowPtr * lphwnd)
{
	OleDbgEnterInterface();

	ASSERTCOND(lphwnd != nil);
	*lphwnd = nil;

	return NOERROR;
}

//#pragma segment OleInPlaceContainerAppIntSeg
STDMETHODIMP IOleInPlaceContainerAppFrameContextSensitiveHelp(LPOLEINPLACEFRAME lpThis, unsigned long fEnterMode)
{
	OleDbgEnterInterface();

	ASSERTCONDSZ(0, "IOleInPlaceFrame::ContextSensitiveHelp: Please implement me!");

	return NOERROR;
}

//#pragma segment OleInPlaceContainerAppIntSeg
STDMETHODIMP IOleInPlaceContainerAppFrameGetBorder(LPOLEINPLACEFRAME lpThis, Rect * lprectBorder)
{
	OleApplicationPtr		pOleApp;
	
	OleDbgEnterInterface();
	
	pOleApp = ((OleInPlaceContainerFrameImplPtr)lpThis)->lpOleApp;

	return OleInPlaceContainerAppGetBorder(pOleApp, lprectBorder);
}

//#pragma segment OleInPlaceContainerAppIntSeg
STDMETHODIMP IOleInPlaceContainerAppFrameRequestBorderSpace(LPOLEINPLACEFRAME lpThis, LPCBORDERWIDTHS lpborderwidths)
{
	OleDbgEnterInterface();

	// we allow the object to have as much border space as it wants.
	return NOERROR;
}

//#pragma segment OleInPlaceContainerAppIntSeg
STDMETHODIMP IOleInPlaceContainerAppFrameSetBorderSpace(LPOLEINPLACEFRAME lpThis, LPCBORDERWIDTHS lpborderwidths)
{
	OleApplicationPtr		pOleApp;
	
	OleDbgEnterInterface();
	
	pOleApp = ((OleInPlaceContainerFrameImplPtr)lpThis)->lpOleApp;

	return OleInPlaceContainerAppSetBorderSpace(pOleApp, lpborderwidths);
}

//#pragma segment OleInPlaceContainerAppIntSeg
STDMETHODIMP IOleInPlaceContainerAppFrameSetActiveObject(LPOLEINPLACEFRAME lpThis, LPOLEINPLACEACTIVEOBJECT lpActiveObject, const char * lpszObjName)
{
	OleApplicationPtr		pOleApp;
	
	OleDbgEnterInterface();
	
	pOleApp = ((OleInPlaceContainerFrameImplPtr)lpThis)->lpOleApp;

	return OleInPlaceContainerAppSetActiveObject(pOleApp, lpActiveObject);
}

//#pragma segment OleInPlaceContainerAppIntSeg
STDMETHODIMP IOleInPlaceContainerAppFrameInsertMenus(LPOLEINPLACEFRAME lpThis, OleMBarHandle hOleMBar)
{
	OleApplicationPtr		pOleApp;
	
	OleDbgEnterInterface();
	
	pOleApp = ((OleInPlaceContainerFrameImplPtr)lpThis)->lpOleApp;

	return OleInPlaceContainerAppInsertMenus(pOleApp, hOleMBar);
}

//#pragma segment OleInPlaceContainerAppIntSeg
STDMETHODIMP IOleInPlaceContainerAppFrameAdjustMenus(LPOLEINPLACEFRAME lpThis, OleMBarHandle hOleMBar)
{
	OleApplicationPtr		pOleApp;
	
	OleDbgEnterInterface();
	
	pOleApp = ((OleInPlaceContainerFrameImplPtr)lpThis)->lpOleApp;

	return OleInPlaceContainerAppAdjustMenus(pOleApp, hOleMBar);
}

//#pragma segment OleInPlaceContainerAppIntSeg
STDMETHODIMP IOleInPlaceContainerAppFrameRemoveMenus(LPOLEINPLACEFRAME lpThis, OleMBarHandle hOleMBar)
{
	OleApplicationPtr		pOleApp;
	
	OleDbgEnterInterface();
	
	pOleApp = ((OleInPlaceContainerFrameImplPtr)lpThis)->lpOleApp;

	return OleInPlaceContainerAppRemoveMenus(pOleApp, hOleMBar);
}

//#pragma segment OleInPlaceContainerAppIntSeg
STDMETHODIMP IOleInPlaceContainerAppFrameSetStatusText(LPOLEINPLACEFRAME lpThis, const char * lpszStatusText)
{
	OleDbgEnterInterface();

	ASSERTCONDSZ(0, "IOleInPlaceFrame::SetStatusText: Please implement me!");

	return NOERROR;
}

//#pragma segment OleInPlaceContainerAppIntSeg
STDMETHODIMP IOleInPlaceContainerAppFrameEnableModeless(LPOLEINPLACEFRAME lpThis, unsigned long fEnable)
{
	OleDbgEnterInterface();

	ASSERTCONDSZ(0, "IOleInPlaceFrame::EnableModeless: Please implement me!");

	return NOERROR;
}

//#pragma segment OleInPlaceContainerAppIntSeg
STDMETHODIMP IOleInPlaceContainerAppFrameTranslateAccelerator(LPOLEINPLACEFRAME lpThis, EventRecord * lpmsg, long ID)
{
	OleApplicationPtr		pOleApp;
	
	OleDbgEnterInterface();
	
	pOleApp = ((OleInPlaceContainerFrameImplPtr)lpThis)->lpOleApp;

	return OleInPlaceContainerAppTranslateAccelerator(pOleApp, lpmsg, ID);
}

#endif // qOleInPlace
