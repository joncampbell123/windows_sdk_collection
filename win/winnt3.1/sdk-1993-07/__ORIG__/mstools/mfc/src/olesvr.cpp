// This is a part of the Microsoft Foundation Classes C++ library. 
// Copyright (C) 1992 Microsoft Corporation 
// All rights reserved. 
//  
// This source code is only intended as a supplement to the 
// Microsoft Foundation Classes Reference and Microsoft 
// QuickHelp documentation provided with the library. 
// See these sources for detailed information regarding the 
// Microsoft Foundation Classes product. 

#include "afxole.h"
#pragma hdrstop

#include "oleptr_.h"
#include <limits.h>

#include "shellapi.h"

#ifdef AFX_OLE_SEG
#pragma code_seg(AFX_OLE_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define OLEEXPORT FAR PASCAL _export

/////////////////////////////////////////////////////////////////////////////
// Helper class for locking out main message pump during callbacks

class LOCK_PUMP
{
public:
	LOCK_PUMP()
	{
#ifdef _DEBUG
		AfxGetApp()->EnablePump(FALSE);
#endif
	}

	~LOCK_PUMP()
	{
#ifdef _DEBUG
		AfxGetApp()->EnablePump(TRUE);
#endif
	}
};

#define OLE_TRACE(string)   \
	if (afxTraceFlags & 0x10)   \
		TRACE(string)

/////////////////////////////////////////////////////////////////////////////
// OLEOBJECT callbacks mapping to COleServerItem virtual functions

inline COleServerItem * COleServerItem::FromLp(LPOLEOBJECT lpObject)
{
	ASSERT(lpObject != NULL);
	COleServerItem* pItem;
	pItem = (COleServerItem*) GetPtrFromFarPtr(lpObject, sizeof(CObject));
	ASSERT(lpObject == &pItem->m_oleObject);
	return pItem;
}

// friend class to get access to COleServerItem protected implementations
struct _afxOleServerItemImplementation
{

	static LPVOID OLEEXPORT
	QueryProtocol(LPOLEOBJECT lpObject, OLE_LPCSTR lpszProtocol)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServerItem::OnQueryProtocol()\n");
		return COleServerItem::FromLp(lpObject)->OnQueryProtocol(lpszProtocol);
	}

	static OLESTATUS OLEEXPORT
	Release(LPOLEOBJECT lpObject)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServerItem::OnRelease()\n");
		return COleServerItem::FromLp(lpObject)->OnRelease();
	}

	static OLESTATUS OLEEXPORT
	Show(LPOLEOBJECT lpObject, BOOL bTakeFocus)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServerItem::OnShow()\n");
		return COleServerItem::FromLp(lpObject)->OnShow(bTakeFocus);
	}

	static OLESTATUS OLEEXPORT
	DoVerb(LPOLEOBJECT lpObject, UINT nVerb, BOOL bShow, BOOL bTakeFocus)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServerItem::OnDoVerb()\n");

		OLESTATUS status;
		status = COleServerItem::FromLp(lpObject)->OnDoVerb(nVerb, bShow,
			bTakeFocus);
#ifdef _DEBUG
		if ((afxTraceFlags & 0x10) && status != OLE_OK)
			TRACE("COleServerItem::OnDoVerb(%d) failed\n", nVerb);
#endif
		return status;
	}

	static OLESTATUS OLEEXPORT
	GetData(LPOLEOBJECT lpObject, OLECLIPFORMAT nFormat,
		LPHANDLE lphDataReturn)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServerItem::OnGetData()\n");
		OLESTATUS status;
		status = COleServerItem::FromLp(lpObject)->OnGetData(nFormat, lphDataReturn);
#ifdef _DEBUG
		if ((afxTraceFlags & 0x10) && status != OLE_OK)
			TRACE("COleServerItem::OnGetData() failed to get format 0x%x\n",
					nFormat);
#endif
		return status;
	}

	static OLESTATUS OLEEXPORT
	SetData(LPOLEOBJECT lpObject, OLECLIPFORMAT nFormat, HANDLE hData)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServerItem::OnSetData()\n");
		return COleServerItem::FromLp(lpObject)->OnSetData(nFormat, hData);
	}

	static OLESTATUS OLEEXPORT
	SetTargetDevice(LPOLEOBJECT lpObject, HANDLE hData)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServerItem::OnSetTargetDevice()\n");

		OLESTATUS status;
		status = COleServerItem::FromLp(lpObject)->OnSetTargetDevice(
			(LPOLETARGETDEVICE)((hData == NULL) ? NULL : ::GlobalLock(hData)));

		if (hData != NULL)
		{
			::GlobalUnlock(hData);
			::GlobalFree(hData);
		}
		return status;
	}

	static OLESTATUS OLEEXPORT
	SetBounds(LPOLEOBJECT lpObject, OLE_CONST RECT FAR* lpRect)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServerItem::SetBounds()\n");
		return COleServerItem::FromLp(lpObject)->OnSetBounds((LPRECT)lpRect);
	}

	static OLECLIPFORMAT OLEEXPORT
	EnumFormats(LPOLEOBJECT lpObject, OLECLIPFORMAT nFormat)
	{
		OLECLIPFORMAT wNext;
		LOCK_PUMP lock;
		wNext = COleServerItem::FromLp(lpObject)->OnEnumFormats(nFormat);
#ifdef _DEBUG
		if (afxTraceFlags & 0x10)
			TRACE("COleServerItem::OnEnumFormats(0x%x) returns 0x%x\n",
				nFormat, wNext);
#endif
		return wNext;
	}

	static OLESTATUS OLEEXPORT
	SetColorScheme(LPOLEOBJECT lpObject, OLE_CONST LOGPALETTE FAR* lpLogPalette)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServerItem::SetColorScheme()\n");
		return COleServerItem::FromLp(lpObject)->
				OnSetColorScheme((LPLOGPALETTE)lpLogPalette);
	}

};

static struct _OLEOBJECTVTBL NEAR objectVtbl =
{
	&_afxOleServerItemImplementation::QueryProtocol,
	&_afxOleServerItemImplementation::Release,
	&_afxOleServerItemImplementation::Show,
	&_afxOleServerItemImplementation::DoVerb,
	&_afxOleServerItemImplementation::GetData,
	&_afxOleServerItemImplementation::SetData,
	&_afxOleServerItemImplementation::SetTargetDevice,
	&_afxOleServerItemImplementation::SetBounds,
	&_afxOleServerItemImplementation::EnumFormats,
	&_afxOleServerItemImplementation::SetColorScheme,
};

//////////////////////////////////////////////////////////////////////////////
// Server view of embedded OLEOBJECT (includes back pointer to OLECLIENT)

IMPLEMENT_DYNAMIC(COleServerItem, CObject)

COleServerItem::COleServerItem()
{
	m_oleObject.lpvtbl = &objectVtbl;
	m_pDocument = NULL;
	m_hPalette = NULL;
	m_lpClient = NULL;  // will be set later
	m_rectBounds.SetRectEmpty();
}

COleServerItem::~COleServerItem()
{
	ASSERT(m_lpClient == NULL);     // must be released first
	if (m_hPalette != NULL)
		::DeleteObject(m_hPalette);
}

void
COleServerItem::BeginRevoke()   // Start revoking the client connection
{
	ASSERT(m_lpClient != NULL);

	OLESTATUS status = ::OleRevokeObject(m_lpClient);
	ASSERT(status == OLE_OK || status == OLE_WAIT_FOR_RELEASE);
	// revoke will not be finished until OnRelease called
}


int
COleServerItem::NotifyClient(OLE_NOTIFICATION wNotify)
{
	ASSERT(m_lpClient != NULL);
	ASSERT(wNotify <= OLE_QUERY_RETRY); // last valid notification code

#ifdef _DEBUG
	if (afxTraceFlags & 0x10)
		TRACE("Notifying client item (wNotification = %d)\n", wNotify);
#endif
	return (*m_lpClient->lpvtbl->CallBack)(m_lpClient, wNotify, &m_oleObject);
}

//////////////////////////////////////////////////////////////////////////////
// Default implementations

OLESTATUS
COleServerItem::OnRelease()
{
	ASSERT(m_lpClient != NULL);
	m_lpClient = NULL;

	return OLE_OK;
}


OLESTATUS
COleServerItem::OnSetTargetDevice(LPOLETARGETDEVICE /*lpTargetDevice*/)
{
	// default to ignore request
	return OLE_OK;
}


BOOL
COleServerItem::OnGetTextData(CString& /*rStringReturn*/)
{
	// default to not supported
	return FALSE;
}


OLESTATUS
COleServerItem::OnExtraVerb(UINT /*nVerb*/)
{
	return OLE_ERROR_DOVERB;    // Error in sending do verb, or invalid
}


// Overridables you do not have to override
LPVOID
COleServerItem::OnQueryProtocol(LPCSTR lpszProtocol) const
{
	if (_fstrcmp(lpszProtocol, "StdFileEditing") == 0)
		return (LPVOID) &m_oleObject;

	return NULL;        // not supported
}


OLESTATUS
COleServerItem::OnSetColorScheme(LPLOGPALETTE lpLogPalette)
{
	// save in item palette
	HPALETTE hNewPal = ::CreatePalette(lpLogPalette);

	if (hNewPal == NULL)
		return OLE_ERROR_PALETTE;

	if (m_hPalette != NULL)
		::DeleteObject(m_hPalette);
	m_hPalette = hNewPal;
	return OLE_OK;
}

OLESTATUS
COleServerItem::OnSetBounds(LPRECT lpRect)
{
	m_rectBounds = lpRect;
	return OLE_OK;
}


OLESTATUS
COleServerItem::OnDoVerb(UINT nVerb, BOOL bShow, BOOL bTakeFocus)
{
	OLESTATUS status;
	if (nVerb == OLEVERB_PRIMARY)
	{
		status = OLE_OK;
	}
	else
	{
		status = OnExtraVerb(nVerb);
	}

	if ((status == OLE_OK) && bShow)
		status = OnShow(bTakeFocus);
	return status;
}


// Clipboard formats

static UINT cfNative = ::RegisterClipboardFormat("Native");
static UINT cfOwnerLink = ::RegisterClipboardFormat("OwnerLink");
static UINT cfObjectLink = ::RegisterClipboardFormat("ObjectLink");

static UINT formats[] =
{
	cfNative,           // native data format
	CF_METAFILEPICT,    // item rendered as a metafile
	CF_TEXT,            // optionally supported if OnGetTextData implemented
	NULL
};

OLECLIPFORMAT
COleServerItem::OnEnumFormats(OLECLIPFORMAT nFormat) const
{
	int index = 0;
	if (nFormat != 0)
	{
		// find the last element
		while (formats[index++] != nFormat)
			;
	}
	return (OLECLIPFORMAT)formats[index];
}

HANDLE
COleServerItem::GetNativeData()
{
	// get native data via serialization
	CSharedFile memFile;

	TRY
	{
		CArchive    getArchive(&memFile, CArchive::store);
		this->Serialize(getArchive);        // store to archive
	}
	CATCH (CNotSupportedException, e)
	{
		memFile.Close();
		return NULL;        // not supported
	}
	AND_CATCH (CException, e)
	{
		memFile.Close();
		THROW_LAST();       // will be caught in GetData
	}
	END_CATCH
	return memFile.Detach();
}

HANDLE
COleServerItem::GetMetafileData()
{
	CMetaFileDC dc;
	if (!dc.Create())
		return NULL;

	if (m_hPalette)
	{
		::SelectPalette(dc.m_hDC, m_hPalette, TRUE);
		dc.RealizePalette();
	}

	// Paint directly into the metafile.
	if (!OnDraw(&dc))
	{
		OLE_TRACE("calling COleServerItem::OnDraw() failed\n");
		return NULL;    // will destroy DC
	}

	HMETAFILE hMF = (HMETAFILE)dc.Close();
	if (hMF == NULL)
		return NULL;

	HANDLE hPict;
	if ((hPict = ::GlobalAlloc(GMEM_DDESHARE, sizeof(METAFILEPICT))) == NULL)
	{
		DeleteMetaFile(hMF);
		return NULL;
	}

	LPMETAFILEPICT lpPict;
	if ((lpPict = (LPMETAFILEPICT)::GlobalLock(hPict)) == NULL)
	{
		DeleteMetaFile(hMF);
		::GlobalFree(hPict);
		return NULL;
	}

	lpPict->mm = MM_ANISOTROPIC;
	lpPict->hMF = hMF;
	lpPict->xExt = m_rectBounds.Width();
	lpPict->yExt = m_rectBounds.Height();   // will be negative for HiMetric
	::GlobalUnlock(hPict);
	return hPict;
}


OLESTATUS
COleServerItem::OnGetData(OLECLIPFORMAT nFormat, LPHANDLE lphReturn)
{
	HANDLE hData = NULL;    // default to not supported

	TRY
	{
		if (nFormat == cfNative)
		{
			hData = GetNativeData();
		}
		else if (nFormat == CF_METAFILEPICT)
		{
			hData = GetMetafileData();
		}
		else if (nFormat == CF_TEXT) 
		{
			CString text;
			if (OnGetTextData(text))
			{
				// allocate a global block for the string
				hData = ::GlobalAlloc(GMEM_DDESHARE, text.GetLength() + 1);
				if (hData == NULL)
					AfxThrowMemoryException();

				LPSTR  lpszText = (LPSTR)::GlobalLock(hData);
				ASSERT(lpszText != NULL);
				_fstrcpy(lpszText, (const char*) text);
				::GlobalUnlock(hData);
			}
		}
		else
		{
			TRACE("Warning: OLE get data, unknown format %d\n", nFormat);
		}
	}
	CATCH (COleException, e)
	{
		return e->m_status;
	}
	AND_CATCH (CException, e)
	{
		// other exceptions
		return OLE_ERROR_MEMORY;
	}
	END_CATCH

	if (hData == NULL)
		return OLE_ERROR_FORMAT;        // not supported

	// return the data
	*lphReturn = hData;
	return OLE_OK;
}


OLESTATUS
COleServerItem::OnSetData(OLECLIPFORMAT nFormat, HANDLE hData)
{
	if (nFormat != cfNative)
	{
		::GlobalFree(hData);
		return OLE_ERROR_FORMAT;    // Requested format is not available
	}

	// set native data via serialization
	CSharedFile memFile;
	memFile.SetHandle(hData);

	TRY
	{
		CArchive    setArchive(&memFile, CArchive::load);
		this->Serialize(setArchive);        // load me
	}
	CATCH (CException, e)
	{
		memFile.Close();
		return OLE_ERROR_GENERIC;
	}
	END_CATCH

	return OLE_OK;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// COleServerDoc

inline COleServerDoc * COleServerDoc::FromLp(LPOLESERVERDOC lpServerDoc)
{
	COleServerDoc* pDoc;
	pDoc = (COleServerDoc*) GetPtrFromFarPtr(lpServerDoc, sizeof(CObject));
	ASSERT(lpServerDoc == &pDoc->m_oleServerDoc);
	return pDoc;
}

// friend class to get access to COleServerDoc protected implementations
struct _afxOleServerDocImplementation
{

	static OLESTATUS OLEEXPORT
	Save(LPOLESERVERDOC lpServerDoc)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServerDoc::OnSave()\n");
		return COleServerDoc::FromLp(lpServerDoc)->OnSave();
	}

	static OLESTATUS OLEEXPORT
	Close(LPOLESERVERDOC lpServerDoc)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServerDoc::OnClose()\n");
		return COleServerDoc::FromLp(lpServerDoc)->OnClose();
	}

	static OLESTATUS OLEEXPORT
	SetHostNames(LPOLESERVERDOC lpServerDoc,
		OLE_LPCSTR lpszClient, OLE_LPCSTR lpszDoc)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServerDoc::OnSetHostNames()\n");
		OLESTATUS status = COleServerDoc::FromLp(lpServerDoc)->OnSetHostNames(
			lpszClient, lpszDoc);
		return status;
	}

	static OLESTATUS OLEEXPORT
	SetDocDimensions(LPOLESERVERDOC lpServerDoc, OLE_CONST RECT FAR* lpRect)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServerDoc::OnSetDocDimensions()\n");
		return COleServerDoc::FromLp(lpServerDoc)->OnSetDocDimensions((LPRECT)lpRect);
	}

	static OLESTATUS OLEEXPORT
	Release(LPOLESERVERDOC lpServerDoc)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServerDoc::OnRelease()\n");
		return COleServerDoc::FromLp(lpServerDoc)->OnRelease();
	}

	static OLESTATUS OLEEXPORT
	SetColorScheme(LPOLESERVERDOC lpServerDoc,
		OLE_CONST LOGPALETTE FAR* lpLogPalette)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServerDoc::OnSetColorScheme()\n");
		OLESTATUS status = COleServerDoc::FromLp(lpServerDoc)->OnSetColorScheme(
			(LPLOGPALETTE)lpLogPalette);
		return status;
	}

	static OLESTATUS OLEEXPORT
	Execute(LPOLESERVERDOC lpServerDoc, HANDLE hCommands)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServerDoc::OnExecute()\n");
		LPVOID  lpCommands = ::GlobalLock(hCommands);
		ASSERT(lpCommands != NULL);
		OLESTATUS status;
		status = COleServerDoc::FromLp(lpServerDoc)->OnExecute(lpCommands);
		::GlobalUnlock(hCommands);
		return status;
	}

	static OLESTATUS OLEEXPORT
	GetObject(LPOLESERVERDOC lpServerDoc, OLE_LPCSTR lpszObjname,
		LPOLEOBJECT FAR * lplpObject, LPOLECLIENT lpClient)
	{
		COleServerDoc* pDoc = COleServerDoc::FromLp(lpServerDoc);
		COleServerItem* pItem;
		LOCK_PUMP lock;

		TRY
		{
			if (lpszObjname == NULL || *lpszObjname == '\0')
			{
				OLE_TRACE("calling COleServerDoc::OnGetDocument\n");
				pItem = pDoc->OnGetDocument();
			}
			else
			{
#ifdef _DEBUG
				if (afxTraceFlags & 0x10)
					TRACE("calling COleServerDoc::GetItem(%Fs)\n", lpszObjname);
#endif
				pItem = pDoc->OnGetItem(lpszObjname);
			}
		}
		CATCH (CException, e)
		{
			return COleException::Process(e);
		}
		END_CATCH

		if (pItem == NULL)
			return OLE_ERROR_GENERIC;

		pDoc->AddItem(pItem, lpClient);     // attaches everything
		*lplpObject = &pItem->m_oleObject;
		return OLE_OK;
	}
};

static struct _OLESERVERDOCVTBL NEAR serverDocVtbl =
{
	&_afxOleServerDocImplementation::Save,
	&_afxOleServerDocImplementation::Close,
	&_afxOleServerDocImplementation::SetHostNames,
	&_afxOleServerDocImplementation::SetDocDimensions,
	&_afxOleServerDocImplementation::GetObject,
	&_afxOleServerDocImplementation::Release,
	&_afxOleServerDocImplementation::SetColorScheme,
	&_afxOleServerDocImplementation::Execute,
};

/////////////////////////////////////////////////////////////////////////////
// COleServerDoc construction and other operations

COleServerDoc::COleServerDoc()
{
	m_oleServerDoc.lpvtbl = &serverDocVtbl;
	m_pServer = NULL;
	m_lhServerDoc = NULL;
	m_hPalette = NULL;
	m_bWaiting = FALSE;
}

IMPLEMENT_DYNAMIC(COleServerDoc, CObject)

COleServerDoc::~COleServerDoc()
{
	if (IsOpen())
		BeginRevoke();
	if (m_hPalette != NULL)
		::DeleteObject(m_hPalette);
}

void
COleServerDoc::CheckAsync(OLESTATUS status)
	// throw exception if not ok to continue
{
	ASSERT(!m_bWaiting);

	if (status == OLE_WAIT_FOR_RELEASE)
	{
		m_bWaiting = TRUE;
		while (m_bWaiting)
		{
			OLE_TRACE("OLE Server Doc waiting for release\n");
			AfxGetApp()->PumpMessage();
		}
		m_bWaiting = FALSE;

		return;     // assume it worked
	}

	if (status == OLE_OK || status > OLE_WARN_DELETE_DATA)
	{
		// ok, or just a warning
		return;
	}

	// otherwise this error wasn't expected, so throw an exception
	TRACE("Warning: COleServerDoc operation failed %d, throwing exception\n", status);
	AfxThrowOleException(status);
}

OLESTATUS
COleServerDoc::BeginRevoke()
	// do not wait for async completion
{
	ASSERT(IsOpen());
	LHCLIENTDOC lh = m_lhServerDoc;
	ASSERT(lh != NULL);
	if (m_pServer != NULL)
		m_pServer->RemoveDocument(this);
	m_lhServerDoc = NULL;
	OLESTATUS status = ::OleRevokeServerDoc(lh);
	ASSERT(status == OLE_OK || status == OLE_WAIT_FOR_RELEASE);
	// revoke will not be finished until OnRelease called
	return status;
}

void
COleServerDoc::Revoke()
	// wait for async completion
{
	ASSERT(IsOpen());
	CheckAsync(BeginRevoke());
}

/////////////////////////////////////////////////////////////////////////////
// Interesting operations

BOOL
COleServerDoc::Register(COleServer* pServer, LPCSTR lpszDoc)
{
	ASSERT(m_lhServerDoc == NULL);      // one time only
	ASSERT(pServer != NULL);
	ASSERT(pServer->IsOpen());

	LHSERVERDOC lhDoc;
	if (::OleRegisterServerDoc(pServer->m_lhServer, lpszDoc,
		&m_oleServerDoc, &lhDoc) != OLE_OK)
	{
		return FALSE;
	}

	pServer->AddDocument(this, lhDoc);
	ASSERT(m_lhServerDoc == lhDoc); // make sure it connected it
	return TRUE;
}

void
COleServerDoc::NotifyRename(LPCSTR lpszNewName)
{
	ASSERT(IsOpen());
	ASSERT(lpszNewName != NULL);
	CheckAsync(::OleRenameServerDoc(m_lhServerDoc, lpszNewName));
}

void
COleServerDoc::NotifyRevert()
{
	ASSERT(IsOpen());
	CheckAsync(::OleRevertServerDoc(m_lhServerDoc));
}

void
COleServerDoc::NotifySaved()
{
	ASSERT(IsOpen());
	CheckAsync(::OleSavedServerDoc(m_lhServerDoc));
}

void
COleServerDoc::NotifyAllClients(OLE_NOTIFICATION wNotify)
{
	POSITION pos = GetStartPosition();
	COleServerItem* pItem;

	while ((pItem = GetNextItem(pos)) != NULL)
	{
		pItem->NotifyClient(wNotify);
	}
}

void
COleServerDoc::AddItem(COleServerItem* pItem, LPOLECLIENT lpClient)
{
	ASSERT_VALID(pItem);

	// Attach handle to un-attached item
	ASSERT(pItem->m_lpClient == NULL);  // must be unattached !
	ASSERT(pItem->m_pDocument == NULL); // must be unconnected !
	pItem->m_lpClient = lpClient;
	pItem->m_pDocument = this;
}

/////////////////////////////////////////////////////////////////////////////
// COleServerDoc standard implementation of overridables

OLESTATUS
COleServerDoc::OnRelease()
{
	OLE_TRACE("COleServerDoc::OnRelease\n");
	m_bWaiting = FALSE;
	// close connection to OLE server doc
	m_lhServerDoc = NULL;
	return OLE_OK;
}


OLESTATUS
COleServerDoc::OnSave()
{
	return OLE_OK;
}


OLESTATUS
COleServerDoc::OnClose()
{
	ASSERT(IsOpen());
	BeginRevoke();
	return OLE_OK;
}


OLESTATUS
COleServerDoc::OnExecute(LPVOID /*lpCommands*/) // DDE commands
{
	return OLE_ERROR_COMMAND;
}


OLESTATUS
COleServerDoc::OnSetDocDimensions(LPRECT /*lpRect*/)
{
	return OLE_OK;      // default to ignore it
}


// Overridables you do not have to override
OLESTATUS
COleServerDoc::OnSetHostNames(LPCSTR /*lpszHost*/, LPCSTR /*lpszHostObj*/)
{
	return OLE_OK;
}


OLESTATUS
COleServerDoc::OnSetColorScheme(LPLOGPALETTE lpLogPalette)
{
	// save in global document palette
	HPALETTE hNewPal = ::CreatePalette(lpLogPalette);

	if (hNewPal == NULL)
		return OLE_ERROR_PALETTE;

	if (m_hPalette != NULL)
		::DeleteObject(m_hPalette);
	m_hPalette = hNewPal;
	return OLE_OK;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// COleServer

inline COleServer * COleServer::FromLp(LPOLESERVER lpServer)
{
	COleServer* pOleServer;
	pOleServer = (COleServer*) GetPtrFromFarPtr(lpServer, sizeof(CObject));
	ASSERT(lpServer == &pOleServer->m_oleServer);
	return pOleServer;
}

// friend class to get access to COleServer protected implementations
struct _afxOleServerImplementation
{
	static OLESTATUS OLEEXPORT
	Exit(LPOLESERVER lpServer)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServer::OnExit()\n");
		return COleServer::FromLp(lpServer)->OnExit();
	}

	static OLESTATUS OLEEXPORT
	Release(LPOLESERVER lpServer)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServer::OnRelease()\n");
		return COleServer::FromLp(lpServer)->OnRelease();
	}

	static OLESTATUS OLEEXPORT
	Execute(LPOLESERVER lpServer, HANDLE hCommands)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServer::Execute()\n");

		LPVOID  lpCommands = ::GlobalLock(hCommands);
		ASSERT(lpCommands != NULL);
		OLESTATUS status;
		status = COleServer::FromLp(lpServer)->OnExecute(lpCommands);
		::GlobalUnlock(hCommands);
		return status;
	}

	static OLESTATUS OLEEXPORT
	Open(LPOLESERVER lpServer, LHSERVERDOC lhServerDoc,
		OLE_LPCSTR lpszDoc, LPOLESERVERDOC FAR * lplpServerDoc)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServer::OnOpenDoc()\n");

		COleServer* pServer = COleServer::FromLp(lpServer);
		COleServerDoc* pDoc;

		TRY
			pDoc = pServer->OnOpenDoc(lpszDoc);
		CATCH (CException, e)
			return COleException::Process(e);
		END_CATCH

		if (pDoc == NULL)
			return OLE_ERROR_GENERIC;
		pServer->AddDocument(pDoc, lhServerDoc);
		*lplpServerDoc = &pDoc->m_oleServerDoc;
		return OLE_OK;
	}

	static OLESTATUS OLEEXPORT
	Create(LPOLESERVER lpServer, LHSERVERDOC lhServerDoc,
		OLE_LPCSTR lpszClass, OLE_LPCSTR lpszDoc, LPOLESERVERDOC FAR * lplpServerDoc)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServer::OnCreateDoc()\n");

		COleServer* pServer = COleServer::FromLp(lpServer);
		COleServerDoc* pDoc;

		TRY
			pDoc = pServer->OnCreateDoc(lpszClass, lpszDoc);
		CATCH (CException, e)
			return COleException::Process(e);
		END_CATCH

		if (pDoc == NULL)
			return OLE_ERROR_GENERIC;

		pServer->AddDocument(pDoc, lhServerDoc);
		*lplpServerDoc = &pDoc->m_oleServerDoc;
		return OLE_OK;
	}

	static OLESTATUS OLEEXPORT
	CreateFromTemplate(LPOLESERVER lpServer, LHSERVERDOC lhServerDoc,
		OLE_LPCSTR lpszClass, OLE_LPCSTR lpszDoc, OLE_LPCSTR lpszTemplate,
		LPOLESERVERDOC FAR * lplpServerDoc)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServer::OnCreateDocFromTemplate()\n");

		COleServer* pServer = COleServer::FromLp(lpServer);
		COleServerDoc* pDoc;

		TRY
			pDoc = pServer->OnCreateDocFromTemplate(lpszClass,
			  lpszDoc, lpszTemplate);
		CATCH (CException, e)
			return COleException::Process(e);
		END_CATCH

		if (pDoc == NULL)
			return OLE_ERROR_GENERIC;
		pServer->AddDocument(pDoc, lhServerDoc);
		*lplpServerDoc = &pDoc->m_oleServerDoc;
		return OLE_OK;
	}

	static OLESTATUS OLEEXPORT
	Edit(LPOLESERVER lpServer, LHSERVERDOC lhServerDoc,
		OLE_LPCSTR lpszClass, OLE_LPCSTR lpszDoc, LPOLESERVERDOC FAR * lplpServerDoc)
	{
		LOCK_PUMP lock;
		OLE_TRACE("COleServer::OnEditDoc()\n");

		COleServer* pServer = COleServer::FromLp(lpServer);
		COleServerDoc* pDoc;

		TRY
			pDoc = pServer->OnEditDoc(lpszClass, lpszDoc);
		CATCH (CException, e)
			return COleException::Process(e);
		END_CATCH

		if (pDoc == NULL)
			return OLE_ERROR_GENERIC;
		pServer->AddDocument(pDoc, lhServerDoc);
		*lplpServerDoc = &pDoc->m_oleServerDoc;
		return OLE_OK;
	}
};

static struct _OLESERVERVTBL NEAR serverVtbl =
{
	&_afxOleServerImplementation::Open,
	&_afxOleServerImplementation::Create,
	&_afxOleServerImplementation::CreateFromTemplate,
	&_afxOleServerImplementation::Edit,
	&_afxOleServerImplementation::Exit,
	&_afxOleServerImplementation::Release,
	&_afxOleServerImplementation::Execute,
};

//////////////////////////////////////////////////////////////////////////////
// COleServer construction etc

COleServer::COleServer(BOOL bLaunchEmbedded)
{
	m_oleServer.lpvtbl = &serverVtbl;
	m_lhServer = NULL;

	m_cOpenDocuments = 0;
	m_bLaunchEmbedded = bLaunchEmbedded;
}

IMPLEMENT_DYNAMIC(COleServer, CObject)

COleServer::~COleServer()
{
	if (IsOpen())
		BeginRevoke();          // server death
}

void
COleServer::BeginRevoke()
{
	ASSERT(IsOpen());
	LHSERVER lhServer = m_lhServer;

	if (lhServer != NULL)
	{
		m_lhServer = NULL;      // closed for all intensive purposes
		OLESTATUS status = ::OleRevokeServer(lhServer);
		ASSERT(status == OLE_OK || status == OLE_WAIT_FOR_RELEASE);
	}
	// NOTE: will return before the Revoke is acknowledged
}

BOOL
COleServer::Register(LPCSTR lpszClass, BOOL bMultiInstance)
{
	ASSERT(m_lhServer == NULL);     // one time only
	ASSERT(lpszClass != NULL);

	OLE_SERVER_USE use = bMultiInstance ? OLE_SERVER_MULTI : OLE_SERVER_SINGLE;
	return ::OleRegisterServer(lpszClass, &m_oleServer, &m_lhServer,
		AfxGetInstanceHandle(), use) == OLE_OK;
}

//////////////////////////////////////////////////////////////////////////////
// COleServer manages COleServerDocs

void COleServer::AddDocument(COleServerDoc* pDoc, LHSERVERDOC lhServerDoc)
{
	// Attach handle to un-open server document
	ASSERT_VALID(pDoc);
	ASSERT(pDoc->m_lhServerDoc == NULL);    // must be unregistered !
	ASSERT(pDoc->m_pServer == NULL);        // must be dis-connected
	ASSERT(lhServerDoc != NULL);

	// connect to server
	pDoc->m_lhServerDoc = lhServerDoc;
	pDoc->m_pServer = this;
	m_cOpenDocuments++;
}

void COleServer::RemoveDocument(COleServerDoc* pDoc)
{
	ASSERT(pDoc != NULL);
	ASSERT(pDoc->m_pServer == this);
	ASSERT(m_cOpenDocuments > 0);

	pDoc->m_pServer = NULL;
	m_cOpenDocuments--;
}

//////////////////////////////////////////////////////////////////////////////
// COleServer default implementation of overridables

COleServerDoc* COleServer::OnOpenDoc(LPCSTR /*lpszDoc*/)
{
	return NULL;
}

COleServerDoc* COleServer::OnCreateDocFromTemplate(LPCSTR /*lpszClass*/,
	LPCSTR /*lpszDoc*/, LPCSTR /*lpszTemplate*/)
{
	return NULL;
}

OLESTATUS COleServer::OnExecute(LPVOID /*lpCommands*/)  // DDE commands
{
	return OLE_ERROR_COMMAND;
}

OLESTATUS COleServer::OnExit()
{
	if (IsOpen())
	{
		OLE_TRACE("COleServer::OnExit() Revoking server\n");
		BeginRevoke();
	}
	return OLE_OK;
}

OLESTATUS COleServer::OnRelease()
{
	CWnd* pMainWnd = AfxGetApp()->m_pMainWnd;

	if (IsOpen() && m_bLaunchEmbedded)
	{
		// there is a chance we should be shutting down
		// shut down if no open documents, or if main window is invisible
		if (m_cOpenDocuments == 0 || 
			(pMainWnd != NULL && !pMainWnd->IsWindowVisible()))
		{
			OLE_TRACE("COleServer::OnRelease() Revoking server\n");
			BeginRevoke();
		}
	}

	// if someone has already revoked us
	if (!IsOpen())
	{
		TRACE("COleServer::OnRelease() terminating server app\n");
		OLE_TRACE("\tcalling ::PostQuitMessage\n");
		::PostQuitMessage(0);
	}

	return OLE_OK;
}

//////////////////////////////////////////////////////////////////////////////
// Special register for server in case user does not run REGLOAD

BOOL AfxOleRegisterServerName(LPCSTR lpszClass, LPCSTR lpszLocalClassName)
{
	ASSERT(lpszClass != NULL && *lpszClass != '\0');
	LONG    lSize;
	char    szBuffer[OLE_MAXNAMESIZE];

	if (lpszLocalClassName == NULL || *lpszLocalClassName == '\0')
	{
		TRACE("Warning: no localized class name provided for server,"
			" using %Fs\n", lpszClass);
		lpszLocalClassName = lpszClass;
	}

	lSize = OLE_MAXNAMESIZE;
	if (::RegQueryValue(HKEY_CLASSES_ROOT, lpszClass, szBuffer,
		&lSize) == ERROR_SUCCESS)
	{
		// don't replace an existing localized name
		TRACE("Server already has localized class name (%s)\n", szBuffer);
	}
	else
	{
		// set localized (user visible) class name
		if (::RegSetValue(HKEY_CLASSES_ROOT, lpszClass, REG_SZ,
		  lpszLocalClassName, _fstrlen(lpszLocalClassName)) != ERROR_SUCCESS)
			return FALSE;
	}

	// set the path name for the server for "StdFileEditing" to this app

	// get name/path of executable
	char szPathName[256];
	::GetModuleFileName(AfxGetInstanceHandle(), szPathName,
		sizeof(szPathName)-1);

	wsprintf(szBuffer, "%s\\protocol\\StdFileEditing\\server",
		(LPCSTR)lpszClass);
	if (::RegSetValue(HKEY_CLASSES_ROOT, szBuffer, REG_SZ,
	  szPathName, strlen(szPathName)) != ERROR_SUCCESS)
		return FALSE;

	// if there are no verbs, throw in the default "Edit" verb
	// (Note: we hard code the English "Edit" so you should have
	//   a .REG file for localized verb names)

	char szOldVerb[256];
	wsprintf(szBuffer, "%s\\protocol\\StdFileEditing\\verb",
		(LPCSTR)lpszClass);
	lSize = OLE_MAXNAMESIZE;
	if (::RegQueryValue(HKEY_CLASSES_ROOT, szBuffer, szOldVerb,
		&lSize) != ERROR_SUCCESS)
	{
		// no verbs, add "Edit"
		if (::RegSetValue(HKEY_CLASSES_ROOT, szBuffer, REG_SZ,
		  "", 0) != ERROR_SUCCESS)
			return FALSE;

		strcat(szBuffer, "\\0");    // backslash + zero
		if (::RegSetValue(HKEY_CLASSES_ROOT, szBuffer, REG_SZ,
		  "Edit", 4) != ERROR_SUCCESS)
			return FALSE;
	}

	// all set
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// Diagnostics

#ifdef _DEBUG
void COleServerItem::AssertValid() const
{
	CObject::AssertValid();

	// must be completely detached or completely attached
	ASSERT((m_pDocument == NULL && m_lpClient == NULL) ||
		(m_pDocument != NULL && m_lpClient != NULL));
}

void COleServerItem::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);

	// shallow dump
	dc << "\n\tm_pDocument = " << (void*)m_pDocument;
	dc << "\n\tm_lpClient = " << m_lpClient;
	dc << "\n\tm_hPalette = " << m_hPalette;
	dc << "\n\tm_rectBounds (HIMETRIC) = " << m_rectBounds;
}


void COleServerDoc::AssertValid() const
{
	CObject::AssertValid();
	ASSERT(!m_bWaiting);    // waiting is an internal state
}

void COleServerDoc::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
	dc << "\n\tm_pServer = " << (void*)m_pServer;
	dc << "\n\tm_lhServerDoc = " << m_lhServerDoc;
	dc << "\n\tm_bWaiting = " << m_bWaiting;
}

void COleServer::AssertValid() const
{
	CObject::AssertValid();
	ASSERT(m_cOpenDocuments >= 0);
}

void COleServer::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
	dc << "\n\tm_lhServer = " << m_lhServer;
	dc << "\n\tm_bLaunchEmbedded = " << m_bLaunchEmbedded;
	dc << "\n\tm_cOpenDocuments = " << m_cOpenDocuments;
}

#endif //_DEBUG


//////////////////////////////////////////////////////////////////////////////
