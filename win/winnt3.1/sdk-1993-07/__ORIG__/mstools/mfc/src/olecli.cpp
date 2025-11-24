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

#ifdef AFX_OLE_SEG
#pragma code_seg(AFX_OLE_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define OLEEXPORT FAR PASCAL _export

/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
// character strings to use for debug traces

static char BASED_CODE szCHANGED[] = "OLE_CHANGED";
static char BASED_CODE szSAVED[] = "OLE_SAVED";
static char BASED_CODE szCLOSED[] = "OLE_CLOSED";
static char BASED_CODE szRENAMED[] = "OLE_RENAMED";
static char BASED_CODE szQUERY_PAINT[] = "OLE_QUERY_PAINT";
static char BASED_CODE szRELEASE[] = "OLE_RELEASE";
static char BASED_CODE szQUERY_RETRY[] = "OLE_QUERY_RETRY";

static char FAR* BASED_CODE notifyStrings[] =
{
	szCHANGED,
	szSAVED,
	szCLOSED,
	szRENAMED,
	szQUERY_PAINT,
	szRELEASE,
	szQUERY_RETRY,
};
#endif //_DEBUG

// Standard protocol strings
static char BASED_CODE lpszStaticProtocol[] = "Static";
static char BASED_CODE lpszStdProtocol[] = "StdFileEditing";

/////////////////////////////////////////////////////////////////////////////
// Client view of OLEOBJECT (includes OLECLIENT)

IMPLEMENT_DYNAMIC(COleClientItem, CObject)

inline COleClientItem *
COleClientItem::FromLp(LPOLECLIENT lpClient)
{
	ASSERT(lpClient != NULL);
	COleClientItem* pOleClient;
	pOleClient = (COleClientItem*) GetPtrFromFarPtr(lpClient, sizeof(CObject));
	ASSERT(lpClient == &pOleClient->m_oleClient);
	return pOleClient;
}

// friend class to get access to COleClientItem protected implementations
struct _afxOleClientItemImplementation
{
	static int OLEEXPORT
	Client_CallBack(LPOLECLIENT lpClient, OLE_NOTIFICATION wNotification,
		LPOLEOBJECT lpObject)
	{
		COleClientItem* pOleClient = COleClientItem::FromLp(lpClient);
		ASSERT(pOleClient != NULL);

	#ifdef _DEBUG
		if (wNotification != OLE_QUERY_PAINT)
		{
			if (afxTraceFlags & 0x10)
				TRACE("OLE Client_Callback %d [%Fs] for $%lx\n",
					wNotification, (LPCSTR)notifyStrings[wNotification], lpObject);
		}
	#else
		(void)lpObject; // not used
	#endif
		return pOleClient->ClientCallBack(wNotification);
	}
};

static struct _OLECLIENTVTBL NEAR clientVtbl =
{
	&_afxOleClientItemImplementation::Client_CallBack
};

// Many creation variants
COleClientItem::COleClientItem(COleClientDoc* pContainerDoc)
{
	ASSERT(pContainerDoc != NULL);
	ASSERT(pContainerDoc->IsOpen());

	m_oleClient.lpvtbl = &clientVtbl;
	m_lpObject = NULL;
	m_lastStatus = OLE_OK;
	m_pDocument = pContainerDoc;
}

COleClientItem::~COleClientItem()
{
	if (m_lpObject != NULL)
	{
		// wait for object to be not busy
		UINT nType = GetType();

		if (nType != OT_STATIC)
			WaitForServer();
		// release linked, delete others
		CheckAsync((nType == OT_LINK) ?
		  ::OleRelease(m_lpObject) : ::OleDelete(m_lpObject));
	}
}

void COleClientItem::Release()
{
	if (m_lpObject == NULL)
		return;

	CheckAsync(::OleRelease(m_lpObject));

	// detach
	m_lpObject = NULL;
}

void COleClientItem::Delete()
{
	if (m_lpObject == NULL)
		return;

	CheckAsync(::OleDelete(m_lpObject));

	// detach
	m_lpObject = NULL;
}

//////////////////////////////////////////////////////////////////////////////
// Create error handling

#ifdef _NTWIN
#pragma warning(disable: 4062)
#endif

BOOL COleClientItem::CheckCreate(OLESTATUS status)
{
	m_lastStatus = status;

	switch (status)
	{
	case OLE_OK:
		ASSERT(m_lpObject != NULL);
		return TRUE;            // immediate create success

	case OLE_WAIT_FOR_RELEASE:  // synchronous create
		ASSERT(m_lpObject != NULL);
		WaitForServer();
		return (m_lastStatus == OLE_OK);

	// cases to treat as exceptions
	case OLE_ERROR_PROTECT_ONLY:
	case OLE_ERROR_MEMORY:
	case OLE_ERROR_OBJECT:
	case OLE_ERROR_OPTION:
		TRACE("Warning: COleClientItem::Create?() failed %d\n", status);
		AfxThrowOleException(status);
		break;
	}

	// the rest are non-exceptional conditions for create
	TRACE("Warning: COleClientItem::Create?() failed %d, returning FALSE\n", status);
	m_lpObject = NULL;      // just in case
	return FALSE;           // create failed
}

#ifdef _NTWIN
#pragma warning(default: 4062)
#endif


void COleClientItem::CheckAsync(OLESTATUS status)
	// special case for possible Async requests
{
	if (status == OLE_WAIT_FOR_RELEASE)
	{
		ASSERT(m_lpObject != NULL);
		WaitForServer();
		status = m_lastStatus;      // set by ASYNC release
		ASSERT(status != OLE_WAIT_FOR_RELEASE);
	}
	CheckGeneral(status);   // may throw an exception
}

void COleClientItem::CheckGeneral(OLESTATUS status)
	// set 'm_lastStatus'
	// throw exception if not ok to continue
{
	ASSERT(status != OLE_WAIT_FOR_RELEASE);
		// Async must be handled as a special case before this

	m_lastStatus = status;
	if (status == OLE_OK || status > OLE_WARN_DELETE_DATA)
	{
		// ok, or just a warning
		return;
	}

	// otherwise this error wasn't expected, so throw an exception
	TRACE("Warning: COleClientItem operation failed %d, throwing exception\n", status);
	AfxThrowOleException(status);
}


//////////////////////////////////////////////////////////////////////////////
// Create variants for real OLEOBJECTs

// From clipboard
BOOL COleClientItem::CanPaste(OLEOPT_RENDER renderopt,
		OLECLIPFORMAT cfFormat)
{
	return ::OleQueryCreateFromClip(lpszStdProtocol,
		renderopt, cfFormat) == OLE_OK ||
		::OleQueryCreateFromClip(lpszStaticProtocol,
		renderopt, cfFormat) == OLE_OK;
}

BOOL COleClientItem::CanPasteLink(OLEOPT_RENDER renderopt,
		OLECLIPFORMAT cfFormat)
{
	return ::OleQueryLinkFromClip(lpszStdProtocol,
	  renderopt, cfFormat) == OLE_OK;
}


BOOL COleClientItem::CreateFromClipboard(LPCSTR lpszObjname,
	OLEOPT_RENDER renderopt, OLECLIPFORMAT cfFormat)
{
	ASSERT(m_lpObject == NULL);     // one time only
	ASSERT(m_pDocument != NULL);
	ASSERT(m_pDocument->IsOpen());
	ASSERT(lpszObjname != NULL);

	return CheckCreate(::OleCreateFromClip(lpszStdProtocol,
		&m_oleClient, m_pDocument->m_lhClientDoc, lpszObjname,
		&m_lpObject, renderopt, cfFormat));
}

BOOL COleClientItem::CreateStaticFromClipboard(LPCSTR lpszObjname,
	OLEOPT_RENDER renderopt, OLECLIPFORMAT cfFormat)
{
	ASSERT(m_lpObject == NULL);     // one time only
	ASSERT(m_pDocument != NULL);
	ASSERT(m_pDocument->IsOpen());
	ASSERT(lpszObjname != NULL);

	return CheckCreate(::OleCreateFromClip(lpszStaticProtocol,
		&m_oleClient, m_pDocument->m_lhClientDoc, lpszObjname,
		&m_lpObject, renderopt, cfFormat));
}

BOOL COleClientItem::CreateLinkFromClipboard(LPCSTR lpszObjname,
	OLEOPT_RENDER renderopt, OLECLIPFORMAT cfFormat)
{
	ASSERT(m_lpObject == NULL);     // one time only
	ASSERT(m_pDocument != NULL);
	ASSERT(m_pDocument->IsOpen());
	ASSERT(lpszObjname != NULL);

	return CheckCreate(::OleCreateLinkFromClip(lpszStdProtocol,
		&m_oleClient, m_pDocument->m_lhClientDoc, lpszObjname,
		&m_lpObject, renderopt, cfFormat));
}


// create from a protocol name or other template
BOOL COleClientItem::CreateNewObject(LPCSTR lpszClass, LPCSTR lpszObjname,
	OLEOPT_RENDER renderopt, OLECLIPFORMAT cfFormat)
{
	ASSERT(m_lpObject == NULL);     // one time only
	ASSERT(m_pDocument != NULL);
	ASSERT(m_pDocument->IsOpen());
	ASSERT(lpszClass != NULL);
	ASSERT(lpszObjname != NULL);

	return CheckCreate(::OleCreate(lpszStdProtocol,
		&m_oleClient, lpszClass,
		m_pDocument->m_lhClientDoc, lpszObjname,
		&m_lpObject, renderopt, cfFormat));
}

// create invisible
BOOL COleClientItem::CreateInvisibleObject(LPCSTR lpszClass, LPCSTR lpszObjname,
	OLEOPT_RENDER renderopt, OLECLIPFORMAT cfFormat, BOOL bActivate)
{
	ASSERT(m_lpObject == NULL);     // one time only
	ASSERT(m_pDocument != NULL);
	ASSERT(m_pDocument->IsOpen());
	ASSERT(lpszClass != NULL);
	ASSERT(lpszObjname != NULL);

	return CheckCreate(::OleCreateInvisible(lpszStdProtocol,
		&m_oleClient, lpszClass,
		m_pDocument->m_lhClientDoc, lpszObjname,
		&m_lpObject, renderopt, cfFormat, bActivate));
}


/////////////////////////////////////////////////////////////////////////////
// More advanced creation

BOOL COleClientItem::CreateCloneFrom(COleClientItem* pSrcObject,
	LPCSTR lpszObjname)
{
	ASSERT(m_lpObject == NULL);     // one time only
	ASSERT(pSrcObject != NULL);
	ASSERT(m_pDocument != NULL);
	ASSERT(m_pDocument->IsOpen());
	ASSERT(lpszObjname != NULL);

	return CheckCreate(::OleClone(pSrcObject->m_lpObject,
		&m_oleClient, m_pDocument->m_lhClientDoc, lpszObjname,
		&m_lpObject));
}


/////////////////////////////////////////////////////////////////////////////
// Default implementations

int COleClientItem::ClientCallBack(OLE_NOTIFICATION wNotification)
{
	switch (wNotification)
	{
	case OLE_CHANGED:   // OLE linked item updated
	case OLE_SAVED:     // OLE server document saved
	case OLE_CLOSED:    // OLE server document closed
		OnChange(wNotification);
		break;
	case OLE_RENAMED:
		OnRenamed();
		break;
	case OLE_RELEASE:
		OnRelease();
		break;
	default:
		// ignore it (eg: QueryPaint and QueryRetry)
		break;
	}

	return TRUE;    // return TRUE in general
}

void COleClientItem::OnRenamed()
{
	// ignore normally
}

void COleClientItem::OnRelease()
{
	ASSERT(m_lpObject != NULL);
	// default will store the release error
	m_lastStatus = ::OleQueryReleaseError(m_lpObject);
	ASSERT(m_lastStatus != OLE_WAIT_FOR_RELEASE);

	if (m_lastStatus != OLE_OK)
	{
		// operation failed
#ifdef _DEBUG
		TRACE("Warning: COleClientItem::OnRelease with error %d ($%lx)\n", m_lastStatus,
				m_lpObject);
#endif
		return;
	}

	// Success
	OLE_RELEASE_METHOD nWhyReleased = ::OleQueryReleaseMethod(m_lpObject);

	if (nWhyReleased == OLE_DELETE)
		m_lpObject = NULL;  // detach
}

/////////////////////////////////////////////////////////////////////////////
// COleClientItem - attributes


UINT COleClientItem::GetType()
{
	ASSERT(m_lpObject != NULL);
	LONG lType;

	CheckGeneral(::OleQueryType(m_lpObject, &lType));
	ASSERT(lType == OT_LINK || lType == OT_EMBEDDED || lType == OT_STATIC);
	return (UINT)lType;
}


CString COleClientItem::GetName()
{
	ASSERT(m_lpObject != NULL);
	char szT[OLE_MAXNAMESIZE];

	UINT cb = OLE_MAXNAMESIZE;
	CheckGeneral(::OleQueryName(m_lpObject, szT, &cb));

	ASSERT(cb == strlen(szT));
	return CString(szT);
}


BOOL COleClientItem::GetSize(LPPOINT lpSize)
{
	ASSERT(m_lpObject != NULL);

	m_lastStatus = ::OleQuerySize(m_lpObject, (DWORD FAR*)lpSize);
	if (m_lastStatus == OLE_ERROR_BLANK)
		return FALSE;   // no size set yet
	CheckGeneral(m_lastStatus);     // may throw exception
	return TRUE;
}

BOOL COleClientItem::GetBounds(LPRECT lpBounds)
{
	ASSERT(m_lpObject != NULL);

	m_lastStatus = ::OleQueryBounds(m_lpObject, lpBounds);
	if (m_lastStatus == OLE_ERROR_BLANK)
		return FALSE;   // no size set yet
	CheckGeneral(m_lastStatus);     // may throw exception
	return TRUE;
}

BOOL COleClientItem::IsOpen()
{
	ASSERT(m_lpObject != NULL);

	m_lastStatus = ::OleQueryOpen(m_lpObject);
	if (m_lastStatus == OLE_ERROR_NOT_OPEN)
		return FALSE;       // not open
	CheckGeneral(m_lastStatus);     // may throw exception
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Data exchange plus helpers

HANDLE
COleClientItem::GetData(OLECLIPFORMAT nFormat, BOOL& bMustDelete)
{
	HANDLE hData = NULL;

	CheckGeneral(::OleGetData(m_lpObject, nFormat, &hData));

	ASSERT(hData != NULL);
	bMustDelete = (m_lastStatus == OLE_WARN_DELETE_DATA);
	return hData;
}


void
COleClientItem::SetData(OLECLIPFORMAT nFormat, HANDLE hData)
{
	ASSERT(m_lpObject != NULL);
	CheckAsync(::OleSetData(m_lpObject, nFormat, hData));
}

void
COleClientItem::RequestData(OLECLIPFORMAT nFormat)
{
	ASSERT(m_lpObject != NULL);
	CheckAsync(::OleRequestData(m_lpObject, nFormat));
}

/////////////////////////////////////////////////////////////////////////////
// Rare or implementation specific attributes

BOOL
COleClientItem::IsEqual(COleClientItem* pObject)
{
	ASSERT(m_lpObject != NULL);
	ASSERT(pObject != NULL);
	ASSERT(pObject->m_lpObject != NULL);

	m_lastStatus = ::OleEqual(m_lpObject, pObject->m_lpObject);
	if (m_lastStatus == OLE_ERROR_NOT_EQUAL)
		return FALSE;       // FALSE => not equal
	CheckGeneral(m_lastStatus);     // may throw exception
	return TRUE;    // otherwise equal
}

HANDLE
COleClientItem::GetLinkFormatData()
// Return global HANDLE of block containing link information
//   will return NULL if error or not appropriate type
//   Both link formats are:  "szClass0szDocument0szItem00" */
{
	OLECLIPFORMAT cf = NULL;

	// first determine the format of the link data
	switch (GetType())
	{
	default:
		return NULL;    // Static or other (i.e. no link format)
	case OT_EMBEDDED:
		cf = (OLECLIPFORMAT)::RegisterClipboardFormat("OwnerLink");
		break;
	case OT_LINK:
		cf = (OLECLIPFORMAT)::RegisterClipboardFormat("ObjectLink");
		break;
	}
	ASSERT(cf != NULL);

	// now get the link data
	BOOL bMustDelete;
	HANDLE h;
	if ((h = GetData(cf, bMustDelete)) == NULL)
		return NULL;
	ASSERT(!bMustDelete);       // must not have to delete clip format data
	return h;
}


/////////////////////////////////////////////////////////////////////////////
// Special link attributes

OLEOPT_UPDATE COleClientItem::GetLinkUpdateOptions()
{
	ASSERT(m_lpObject != NULL);
	OLEOPT_UPDATE updateOpt;

	CheckGeneral(::OleGetLinkUpdateOptions(m_lpObject, &updateOpt));
	return updateOpt;
}

void
COleClientItem::SetLinkUpdateOptions(OLEOPT_UPDATE updateOpt)
{
	ASSERT(m_lpObject != NULL);

	CheckAsync(::OleSetLinkUpdateOptions(m_lpObject, updateOpt));
}

/////////////////////////////////////////////////////////////////////////////
// COleClientItem - general operations

BOOL
COleClientItem::Draw(CDC *pDC, LPRECT lpBounds, LPRECT lpWBounds,
	CDC* pFormatDC)
{
	ASSERT(m_lpObject != NULL);
	ASSERT(pDC != NULL);
	// pFormatDC may be null

	m_lastStatus = ::OleDraw(m_lpObject, pDC->m_hDC, lpBounds,
	   lpWBounds, pFormatDC->GetSafeHdc());

	if (m_lastStatus == OLE_ERROR_ABORT || m_lastStatus == OLE_ERROR_DRAW ||
	  m_lastStatus == OLE_ERROR_BLANK)
		return FALSE;       // expected errors

	CheckGeneral(m_lastStatus);     // may throw exception
	return TRUE;    // it worked
}

void
COleClientItem::Activate(UINT nVerb, BOOL bShow, BOOL bTakeFocus,
	CWnd* pWndContainer, LPRECT lpBounds)
{
	ASSERT(m_lpObject != NULL);

	CheckAsync(::OleActivate(m_lpObject, nVerb, bShow,
		bTakeFocus, pWndContainer->GetSafeHwnd(), lpBounds));
}

/////////////////////////////////////////////////////////////////////////////
// more advanced operations

void
COleClientItem::Rename(LPCSTR lpszNewname)
{
	ASSERT(m_lpObject != NULL);
	ASSERT(lpszNewname != NULL);
	CheckGeneral(::OleRename(m_lpObject, lpszNewname));
}

void
COleClientItem::CopyToClipboard()
{
	ASSERT(m_lpObject != NULL);

	CheckGeneral(::OleCopyToClipboard(m_lpObject));
}

void
COleClientItem::SetTargetDevice(HANDLE hData)
{
	ASSERT(m_lpObject != NULL);
	ASSERT(hData != NULL);
	CheckAsync(::OleSetTargetDevice(m_lpObject, hData));
}

/////////////////////////////////////////////////////////////////////////////
// Embedded COleClient operations

void
COleClientItem::SetHostNames(LPCSTR lpszHost, LPCSTR lpszHostObj)
{
	ASSERT(m_lpObject != NULL);
	ASSERT(lpszHost != NULL);
	ASSERT(lpszHostObj != NULL);

	CheckAsync(::OleSetHostNames(m_lpObject, lpszHost, lpszHostObj));
}

void
COleClientItem::SetBounds(LPRECT lpRect)
{
	ASSERT(m_lpObject != NULL);
	CheckAsync(::OleSetBounds(m_lpObject, lpRect));
}

void
COleClientItem::SetColorScheme(LPLOGPALETTE lpLogPalette)
{
	ASSERT(m_lpObject != NULL);
	CheckAsync(::OleSetColorScheme(m_lpObject, lpLogPalette));
}

/////////////////////////////////////////////////////////////////////////////
// Linked COleClient operations

void
COleClientItem::UpdateLink()
{
	ASSERT(m_lpObject != NULL);

	CheckAsync(::OleUpdate(m_lpObject));
}

void COleClientItem::CloseLink()
{
	if (m_lpObject == NULL)
		return;

	CheckAsync(::OleClose(m_lpObject));
	// Does not detach since this can be reactivated later
}

void
COleClientItem::ReconnectLink()
{
	ASSERT(m_lpObject != NULL);
	CheckAsync(::OleReconnect(m_lpObject));
}

BOOL COleClientItem::FreezeLink(LPCSTR lpszFrozenName)
{
	ASSERT(m_lpObject != NULL);
	ASSERT(m_pDocument != NULL);
	ASSERT(m_pDocument->IsOpen());
	ASSERT(lpszFrozenName != NULL);

	ASSERT(GetType() == OT_LINK);

	LPOLEOBJECT lpOriginalObject = m_lpObject;
	m_lpObject = NULL;
	if (!CheckCreate(::OleObjectConvert(lpOriginalObject,
		lpszStaticProtocol,
		&m_oleClient, m_pDocument->m_lhClientDoc, lpszFrozenName,
		&m_lpObject)))
	{
		m_lpObject = lpOriginalObject;
		return FALSE;
	}
	ASSERT(GetType() == OT_STATIC);

	// copy from link worked - now get rid of the original
	ASSERT(m_lpObject != lpOriginalObject);
	ASSERT(m_lpObject != NULL);

	LPOLEOBJECT lpNewObject = m_lpObject;
	m_lpObject = lpOriginalObject;
	ASSERT(GetType() == OT_LINK);
	Delete();

	ASSERT(m_lpObject == NULL);
	m_lpObject = lpNewObject;
	ASSERT(GetType() == OT_STATIC);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// _COleStream - implementation class connecting OLESTREAM and CArchive

struct _COleStream : public _OLESTREAM
{
	CArchive*   m_pArchive;
	_COleStream(CArchive& ar);
};

static UINT CalcSize(DWORD cbTotal, const void FAR* lpStart)
{
	// return size to read/write (16K max unless limited by segment bounds)
	DWORD cbThisSeg = 0x10000 - _AFX_FP_OFF(lpStart);
	DWORD cb = min(cbThisSeg, cbTotal);
	return (cb > 16384) ? 16384 : (UINT)cb;
}

// class for static exports
struct _afxOleStreamImplementation
{
	static DWORD OLEEXPORT
	Get(LPOLESTREAM lpStream, void FAR* lpBuffer, DWORD dwCount)
	{
		register _COleStream* pStream =
			(_COleStream*)GetPtrFromFarPtr(lpStream, 0);
		ASSERT(((LPVOID)pStream) == (LPVOID)lpStream);  // no near/far mismatch

		DWORD dwToRead = dwCount;
		while (dwToRead > 0)
		{
			UINT nRead = CalcSize(dwToRead, lpBuffer);
			pStream->m_pArchive->Read(lpBuffer, nRead);
			dwToRead -= nRead;
			lpBuffer = ((BYTE _huge*)lpBuffer) + nRead;
		}
		return dwCount;
	}

	static DWORD OLEEXPORT
	Put(LPOLESTREAM lpStream, OLE_CONST void FAR* lpBuffer, DWORD dwCount)
	{
		register _COleStream* pStream =
			(_COleStream*)GetPtrFromFarPtr(lpStream, 0);
		ASSERT(((LPVOID)pStream) == (LPVOID)lpStream);  // no near/far mismatch

		DWORD dwToWrite = dwCount;
		while (dwToWrite > 0)
		{
			UINT nWrite = CalcSize(dwToWrite, lpBuffer);
			pStream->m_pArchive->Write(lpBuffer, nWrite);
			dwToWrite -= nWrite;
			lpBuffer = ((OLE_CONST BYTE _huge*)lpBuffer) + nWrite;
		}
		return dwCount;
	}
};

static struct _OLESTREAMVTBL NEAR streamVtbl =
{
	&_afxOleStreamImplementation::Get,
	&_afxOleStreamImplementation::Put
};

_COleStream::_COleStream(CArchive& ar)
{
	m_pArchive = &ar;
	lpstbl = &streamVtbl;           // OLE VTable setup
}

/////////////////////////////////////////////////////////////////////////////
// COleClientItem - serialization

void COleClientItem::Serialize(CArchive& ar)
{
	ASSERT(GetDocument() != NULL);  // must 'SetDocument' first
	_COleStream oleStream(ar);

	if (ar.IsStoring())
	{
		ASSERT(m_lpObject != NULL);
		ar << (WORD) GetType();
		ar << GetName();        // save our document name

		// Save object
		CheckGeneral(::OleSaveToStream(m_lpObject, &oleStream));
	}
	else
	{
		ASSERT(m_lpObject == NULL);

		WORD nType;
		ar >> nType;
		LPCSTR lpszProtocol;

		if (nType == OT_LINK || nType == OT_EMBEDDED)
		{
			lpszProtocol = lpszStdProtocol;
		}
		else if (nType == OT_STATIC)
		{
			lpszProtocol = lpszStaticProtocol;
		}
		else
		{
			// unknown type (i.e. bad file format)
			AfxThrowOleException(OLE_ERROR_GENERIC);
		}

		CString name;
		ar >> name; // document name
		if (!CheckCreate(::OleLoadFromStream(&oleStream, lpszProtocol,
			&m_oleClient, m_pDocument->m_lhClientDoc, name, &m_lpObject)))
		{
			// throw an exception regardless
			AfxThrowOleException(GetLastStatus());
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// COleClientDoc - wrapper for LHCLIENTDOC

IMPLEMENT_DYNAMIC(COleClientDoc, CObject)

COleClientDoc::COleClientDoc()
{
	m_lhClientDoc = NULL;       // not open
}

void
COleClientDoc::Revoke()
{
	if (!IsOpen())
		return;
	LHCLIENTDOC lh = m_lhClientDoc;
	ASSERT(lh != NULL);
	m_lhClientDoc = NULL;
	CheckGeneral(::OleRevokeClientDoc(lh));
}

COleClientDoc::~COleClientDoc()
{
	Revoke();
}

void COleClientDoc::CheckGeneral(OLESTATUS status) const
	// throw exception if not ok to continue
{
	ASSERT(status != OLE_WAIT_FOR_RELEASE);

	if (status == OLE_OK || status > OLE_WARN_DELETE_DATA)
	{
		// ok, or just a warning
		return;
	}

	// otherwise this error wasn't expected, so throw an exception
	TRACE("Warning: COleClientDoc operation failed %d, throwing exception\n", status);
	AfxThrowOleException(status);
}

BOOL
COleClientDoc::Register(LPCSTR lpszClass, LPCSTR lpszDoc)
{
	ASSERT(m_lhClientDoc == NULL);      // one time only
	return ::OleRegisterClientDoc(lpszClass, lpszDoc,
		 0L /*reserved*/, &m_lhClientDoc) == OLE_OK;
}

void
COleClientDoc::NotifyRename(LPCSTR lpszNewName)
{
	ASSERT(IsOpen());
	ASSERT(lpszNewName != NULL);

	CheckGeneral(::OleRenameClientDoc(m_lhClientDoc, lpszNewName));
}

void
COleClientDoc::NotifyRevert()
{
	ASSERT(IsOpen());

	CheckGeneral(::OleRevertClientDoc(m_lhClientDoc));
}

void
COleClientDoc::NotifySaved()
{
	ASSERT(IsOpen());

	CheckGeneral(::OleSavedClientDoc(m_lhClientDoc));
}

/////////////////////////////////////////////////////////////////////////////
// Diagnostics

#ifdef _DEBUG
void COleClientItem::AssertValid() const
{
	CObject::AssertValid();
	ASSERT(m_pDocument != NULL);
}

void COleClientItem::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);

	// shallow dump
	dc << "\n\tm_pDocument = " << (void*) m_pDocument;
	dc << "\n\tm_lpObject = " << m_lpObject;
	dc << "\n\tm_lastStatus = " << (int)m_lastStatus;
}


void COleClientDoc::AssertValid() const
{
	CObject::AssertValid();
	ASSERT(m_lhClientDoc != NULL);
		// only valid if truely open
}

void COleClientDoc::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
	dc << "\n\tm_lhClientDoc = " << m_lhClientDoc;
}

#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
