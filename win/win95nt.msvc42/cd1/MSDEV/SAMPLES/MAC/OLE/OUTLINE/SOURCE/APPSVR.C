#if !defined(_MSC_VER) && !defined(THINK_C)
#include "oline.h"
#endif

#include "Types.h"

#if defined(USEHEADER)
#include "OleHdrs.h"
#endif
#include "Debug.h"
#include "App.h"
#include "PObj.h"
#include "Doc.h"
#include "OleXcept.h"
#include "OleDebug.h"
#include "Util.h"
#include <Resources.h>

// The class factory stuff is in the server header.
#if qOleContainerApp
    #include "AppSvr.h"
#endif

// OLDNAME: OleServerApplication.c

//#pragma segment OleServerApplicationInitSeg
/* OleServerAppInit
 * ----------------
 *
 *	Purpose: Do the server specific initialization of OleApp
 */
void OleServerAppInit(OleApplicationPtr pOleApp)
{
	OleServerDocInitInterfaces();
	OlePseudoObjInitInterfaces();
	
#if qOleInPlace
		OleInPlaceServerInitInterfaces();
#endif
}

//#pragma segment OleServerApplicationSeg
void OleServerAppDispose(OleApplicationPtr pOleApp)
{
}


// OLDNAME: OleInPlaceServerApplication.c


#if qOleInPlace
//#pragma segment OleInPlaceServerAppSeg
Boolean OleInPlaceServerAppDoSuspend(OleApplicationPtr pOleApp)
{
	// we need to ignore this suspend;
	if (pOleApp->server.inplace.m_fIgnoreSuspendEvent)
	{
		pOleApp->server.inplace.m_fIgnoreSuspendEvent = false;
		return false;
	}
	
	return true;
}

//#pragma segment OleInPlaceServerAppSeg
Boolean OleInPlaceServerAppDoResume(OleApplicationPtr pOleApp)
{
	// we need to ignore this suspend;
	if (pOleApp->server.inplace.m_fIgnoreResumeEvent)
	{
		pOleApp->server.inplace.m_fIgnoreResumeEvent = false;
		return false;
	}
	
	return true;
}


#endif

// OLDNAME: OleOutlineServer.c
//#pragma segment OleOutlineServerDocumentSeg
void OleOutlineServerInit(OleOutlineAppPtr pOleOutlineApp, OSType creator)
{
	OleApplicationPtr	pOleApp;

	pOleApp = &pOleOutlineApp->m_OleApp;

    /* Setup arrays used by IDataObject::EnumFormatEtc.
    **
    ** OLE2NOTE: The order that the formats are listed for GetData is very
    **    significant. It should be listed in order of highest fidelity
    **    formats to least fidelity formats. A common ordering will be:
    **                  1. private app formats
    **                  2. EmbedSource
    **                  3. lower fidelity interchange formats
    **                  4. pictures (metafile, dib, etc.)
    **                      (graphic-related apps offer pictures 1st!)
    **                  5. LinkSource
    */

    /* m_arrDocGetFmts array enumerates the formats that a ServerDoc
    **    DataTransferDoc object can offer (give) through a
    **    IDataObject::GetData call. a ServerDoc DataTransferDoc offers
    **    data formats in the following order:
    **                  1. CF_OUTLINE
    **                  2. CF_EMBEDSOURCE
    **                  3. CF_OBJECTDESCRIPTOR
    **                  4. 'TEXT'
    **                  5. 'PICT'
    **                  6. CF_LINKSOURCE *
    **                  7. CF_LINKSRCDESCRIPTOR *
    **
    **    * NOTE: CF_LINKSOURCE and CF_LINKSRCDESCRIPTOR is only
    **    offered if the doc is able to give
    **    a Moniker which references the data. CF_LINKSOURCE is
    **    deliberately listed last in this array of possible formats.
    **    if the doc does not have a Moniker then the last element of
    **    this array is not used. (see SvrDoc_DataObj_EnumFormatEtc).
    **
    **    NOTE: The list of formats that a USER ServerDoc document can
    **    offer is a static list and is registered in the registration
    **    database for the SVROUTL class. The
    **    IDataObject::EnumFormatEtc method returns OLE_S_USEREG in the
    **    case the document is a user docuemt (ie. created via
    **    File.New, File.Open, InsertObject in a container, or
    **    IPersistFile::Load during binding a link source). this tells
    **    OLE to enumerate the formats automatically using the data the
    **    the REGDB.
    */

	{
		FORMATETC		Fmt;

		Fmt.cfFormat	= pOleApp->m_cfEmbedSource;
		Fmt.ptd			= NULL;
		Fmt.dwAspect	= DVASPECT_CONTENT;
		Fmt.tymed		= TYMED_ISTORAGE;
		Fmt.lindex		= -1;
		OleAppAddDocGetFormat(pOleApp, &Fmt);

		Fmt.cfFormat	= 'TEXT';
		Fmt.ptd			= NULL;
		Fmt.dwAspect	= DVASPECT_CONTENT;
		Fmt.tymed		= TYMED_HGLOBAL;
		Fmt.lindex		= -1;
		OleAppAddDocGetFormat(pOleApp, &Fmt);

		Fmt.cfFormat	= 'PICT';
		Fmt.ptd			= NULL;
		Fmt.dwAspect	= DVASPECT_CONTENT;
		Fmt.tymed		= TYMED_MFPICT;
		Fmt.lindex		= -1;
		OleAppAddDocGetFormat(pOleApp, &Fmt);

		Fmt.cfFormat	= pOleApp->m_cfObjectDescriptor;
		Fmt.ptd			= NULL;
		Fmt.dwAspect	= DVASPECT_CONTENT;
		Fmt.tymed		= TYMED_HGLOBAL;
		Fmt.lindex		= -1;
		OleAppAddDocGetFormat(pOleApp, &Fmt);

		Fmt.cfFormat	= pOleApp->m_cfLinkSource;
		Fmt.ptd			= NULL;
		Fmt.dwAspect	= DVASPECT_CONTENT;
		Fmt.tymed		= TYMED_ISTREAM;
		Fmt.lindex		= -1;
		OleAppAddDocGetFormat(pOleApp, &Fmt);

		Fmt.cfFormat	= pOleApp->m_cfLinkSrcDescriptor;
		Fmt.ptd			= NULL;
		Fmt.dwAspect	= DVASPECT_CONTENT;
		Fmt.tymed		= TYMED_HGLOBAL;
		Fmt.lindex		= -1;
		OleAppAddDocGetFormat(pOleApp, &Fmt);
	}

    /* m_arrPasteEntries array enumerates the formats that a ServerDoc
    **    object can accept (get) from the clipboard.
    **    The formats are listed in priority order.
    **    ServerDoc accept data formats in the following order:
    **                  1. CF_OUTLINE
    **                  2. 'TEXT'
    */
	{
		OLEUIPASTEENTRY		PasteEntry;
		Handle				h;

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
	}

   /**    m_arrLinkTypes array enumerates the link types that a ServerDoc
    **    object can accept from the clipboard. ServerDoc does NOT
    **    accept any type of link from the clipboard. ServerDoc can
    **    only be the source of a link. it can not contain links.
    */
	{
	}
}

//#pragma segment OleOutlineServerDocumentSeg
LPSTORAGE OleOutlineDocGetStorage(OleOutlineDocPtr pOleOutlineDoc)
{
	return nil;
}
