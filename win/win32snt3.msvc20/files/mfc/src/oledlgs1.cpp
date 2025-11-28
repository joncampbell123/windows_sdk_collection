// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1993 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp and/or WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"

#ifdef AFX_OLE2_SEG
#pragma code_seg(AFX_OLE2_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

UINT CALLBACK
_AfxOleHookProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

////////////////////////////////////////////////////////////////////////////
// InsertObject dialog wrapper

COleInsertDialog::COleInsertDialog(DWORD dwFlags, CWnd* pParentWnd)
	: COleDialog(pParentWnd)
{
	memset(&m_io, 0, sizeof(m_io)); // initialize structure to 0/NULL

	// fill in common part
	m_io.cbStruct = sizeof(m_io);
	m_io.dwFlags = dwFlags;
	if (AfxHelpEnabled())
		m_io.dwFlags |= IOF_SHOWHELP;
	m_io.lpfnHook = _AfxOleHookProc;
	m_nIDHelp = AFX_IDD_INSERTOBJECT;

	// specific to this dialog
	m_io.lpszFile = m_szFileName;
	m_io.cchFile = _countof(m_szFileName);
	m_szFileName[0] = '\0';
}

COleInsertDialog::~COleInsertDialog()
{
	_AfxDeleteMetafilePict(m_io.hMetaPict);
}

int COleInsertDialog::DoModal()
{
	ASSERT_VALID(this);
	ASSERT(m_io.lpfnHook != NULL);  // can still be a user hook

	m_io.hWndOwner = PreModal();
	int iResult = MapResult(::OleUIInsertObject(&m_io));
	PostModal();
	return iResult;
}

UINT COleInsertDialog::GetSelectionType() const
{
	ASSERT_VALID(this);

	if (m_io.dwFlags & IOF_SELECTCREATEFROMFILE)
	{
		if (m_io.dwFlags & IOF_CHECKLINK)
			return linkToFile;
		else
			return insertFromFile;
	}
	ASSERT(m_io.dwFlags & IOF_SELECTCREATENEW);
	return createNewItem;
}

// allocate an item first, then call this fuction to create it
BOOL COleInsertDialog::CreateItem(COleClientItem* pNewItem)
{
	ASSERT_VALID(pNewItem);

	// switch on selection type
	UINT selType = GetSelectionType();
	BOOL bResult;

	switch (selType)
	{
	case linkToFile:
		// link to file selected
		ASSERT(m_szFileName[0] != 0);
		bResult = pNewItem->CreateLinkFromFile(m_szFileName);
		break;
	case insertFromFile:
		// insert file selected
		ASSERT(m_szFileName[0] != 0);
		bResult = pNewItem->CreateFromFile(m_szFileName);
		break;
	default:
		// otherwise must be create new
		ASSERT(selType == createNewItem);
		bResult = pNewItem->CreateNewItem(m_io.clsid);
		break;
	}

	// deal with Display As Iconic option
	if (bResult && GetDrawAspect() == DVASPECT_ICON)
	{
		// setup iconic cache (it will draw iconic by default as well)
		if (!pNewItem->SetIconicMetafile(m_io.hMetaPict))
		{
			TRACE0("Warning: failed to set iconic aspect in CreateItem.\n");
			return TRUE;
		}

		// since picture was set OK, draw as iconic as well...
		pNewItem->SetDrawAspect(DVASPECT_ICON);
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// COleInsertDialog diagnostics

#ifdef _DEBUG
void COleInsertDialog::Dump(CDumpContext& dc) const
{
	COleDialog::Dump(dc);

	dc << "m_szFileName = " << m_szFileName;
	dc << "\nm_io.cbStruct = " << m_io.cbStruct;
	dc << "\nm_io.dwFlags = " << (LPVOID)m_io.dwFlags;
	dc << "\nm_io.hWndOwner = " << (UINT)m_io.hWndOwner;
	dc << "\nm_io.lpszCaption = " << m_io.lpszCaption;
	dc << "\nm_io.lCustData = " << (LPVOID)m_io.lCustData;
	dc << "\nm_io.hInstance = " << (UINT)m_io.hInstance;
	dc << "\nm_io.lpszTemplate = " << (LPVOID)m_io.lpszTemplate;
	dc << "\nm_io.hResource = " << (UINT)m_io.hResource;
	if (m_io.lpfnHook == _AfxOleHookProc)
		dc << "\nhook function set to standard MFC hook function";
	else
		dc << "\nhook function set to non-standard hook function";
	dc << "\nm_io.hMetaPict = " << (UINT)m_io.hMetaPict;

	dc << "\n";
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COleConvertDialog

COleConvertDialog::COleConvertDialog(COleClientItem* pItem, DWORD dwFlags,
	CLSID* pClassID, CWnd* pParentWnd) : COleDialog(pParentWnd)
{
	if (pItem != NULL)
		ASSERT_VALID(pItem);
	ASSERT(pClassID == NULL || AfxIsValidAddress(pClassID, sizeof(CLSID), FALSE));

	memset(&m_cv, 0, sizeof(m_cv)); // initialize structure to 0/NULL
	if (pClassID != NULL)
		m_cv.clsid = *pClassID;

	// fill in common part
	m_cv.cbStruct = sizeof(m_cv);
	m_cv.dwFlags = dwFlags;
	if (AfxHelpEnabled())
		m_cv.dwFlags |= CF_SHOWHELPBUTTON;
	m_cv.lpfnHook = _AfxOleHookProc;
	m_nIDHelp = AFX_IDD_CONVERT;

	// specific to this dialog
	m_cv.fIsLinkedObject = pItem->GetType() == OT_LINK;
	m_cv.dvAspect = pItem->GetDrawAspect();
	if (pClassID == NULL && !m_cv.fIsLinkedObject)
	{
		// for embeddings, attempt to get class ID from the storage
		if (ReadClassStg(pItem->m_lpStorage, &m_cv.clsid) == NOERROR)
			pClassID = &m_cv.clsid;

		// attempt to get user type from storage
		CLIPFORMAT cf = 0;
		ReadFmtUserTypeStg(pItem->m_lpStorage, &cf, &m_cv.lpszUserType);
		m_cv.wFormat = (WORD)cf;
	}
	// get class id if neded
	if (pClassID == NULL)
	{
		// no class ID in the storage, use class ID of the object
		pItem->GetClassID(&m_cv.clsid);
	}

	// get user type if needed
	if (m_cv.lpszUserType == NULL)
	{
		TCHAR szUserType[256];

		// no user type in storge, get user type from class ID
		if (!_AfxOleGetUserTypeOfClass(m_cv.clsid, szUserType,
			sizeof(szUserType), NULL))
		{
			szUserType[0] = '?';
			szUserType[1] = 0;
			VERIFY(AfxLoadString(IDS_PSUNKNOWNTYPE, szUserType) != 0);
		}
		// copy it to OLE string (allocated by task allocator)
		m_cv.lpszUserType = (LPTSTR)
			AfxAllocTaskMem((lstrlen(szUserType)+1)*sizeof(TCHAR));
		if (m_cv.lpszUserType != NULL)
			lstrcpy(m_cv.lpszUserType, szUserType);
	}
	m_cv.hMetaPict = pItem->GetIconicMetafile();
}

COleConvertDialog::~COleConvertDialog()
{
	_AfxDeleteMetafilePict(m_cv.hMetaPict);
}

int COleConvertDialog::DoModal()
{
	ASSERT_VALID(this);
	ASSERT(m_cv.lpfnHook != NULL);  // can still be a user hook

	m_cv.hWndOwner = PreModal();
	int iResult = MapResult(::OleUIConvert(&m_cv));
	PostModal();
	return iResult;
}

BOOL COleConvertDialog::DoConvert(COleClientItem* pItem)
{
	ASSERT_VALID(pItem);

	BeginWaitCursor();

	UINT selType = GetSelectionType();
	BOOL bResult = TRUE;

	switch (selType)
	{
	case convertItem:
		bResult = pItem->ConvertTo(m_cv.clsidNew);
		break;
	case activateAs:
		bResult = pItem->ActivateAs(m_cv.lpszUserType, m_cv.clsid, m_cv.clsidNew);
		break;
	default:
		ASSERT(selType == noConversion);
		break;
	}

	if (!bResult)
	{
		// if unable to convert the object show message box
		EndWaitCursor();
		AfxMessageBox(AFX_IDP_FAILED_TO_CONVERT);
		return FALSE;
	}

	// change to iconic/content view if changed
	if ((DVASPECT)m_cv.dvAspect != pItem->GetDrawAspect())
	{
		pItem->OnChange(OLE_CHANGED_ASPECT, (DWORD)m_cv.dvAspect);
		pItem->SetDrawAspect((DVASPECT)m_cv.dvAspect);
	}

	// change the actual icon as well
	if (m_cv.fObjectsIconChanged)
	{
		pItem->SetIconicMetafile(m_cv.hMetaPict);
		if (pItem->GetDrawAspect() == DVASPECT_ICON)
			pItem->OnChange(OLE_CHANGED, (DWORD)DVASPECT_ICON);
	}

	EndWaitCursor();
	return TRUE;
}

UINT COleConvertDialog::GetSelectionType() const
{
	ASSERT_VALID(this);

	if (m_cv.clsid != m_cv.clsidNew)
	{
		if (m_cv.dwFlags & CF_SELECTCONVERTTO)
			return convertItem;
		else if (m_cv.dwFlags & CF_SELECTACTIVATEAS)
			return activateAs;
	}
	return noConversion;
}

/////////////////////////////////////////////////////////////////////////////
// COleConvertDialog diagnostics

#ifdef _DEBUG
void COleConvertDialog::Dump(CDumpContext& dc) const
{
	COleDialog::Dump(dc);

	dc << "m_cv.cbStruct = " << m_cv.cbStruct;
	dc << "\nm_cv.dwFlags = " << (LPVOID)m_cv.dwFlags;
	dc << "\nm_cv.hWndOwner = " << (UINT)m_cv.hWndOwner;
	dc << "\nm_cv.lpszCaption = " << m_cv.lpszCaption;
	dc << "\nm_cv.lCustData = " << (LPVOID)m_cv.lCustData;
	dc << "\nm_cv.hInstance = " << (UINT)m_cv.hInstance;
	dc << "\nm_cv.lpszTemplate = " << (LPVOID)m_cv.lpszTemplate;
	dc << "\nm_cv.hResource = " << (UINT)m_cv.hResource;
	if (m_cv.lpfnHook == _AfxOleHookProc)
		dc << "\nhook function set to standard MFC hook function";
	else
		dc << "\nhook function set to non-standard hook function";
	dc << "\nm_cv.dvAspect = " << (UINT)m_cv.dvAspect;
	dc << "\nm_cv.wFormat = " << (UINT)m_cv.wFormat;
	dc << "\nm_cv.fIsLinkedObject = " << m_cv.fIsLinkedObject;
	dc << "\nm_cv.hMetaPict = " << (UINT)m_cv.hMetaPict;
	dc << "\nm_cv.lpszUserType = " << m_cv.lpszUserType;
	dc << "\nm_cv.fObjectsIconChanged = " << m_cv.fObjectsIconChanged;

	dc << "\n";
}
#endif

/////////////////////////////////////////////////////////////////////////////
// COleChangeIconDialog

COleChangeIconDialog::COleChangeIconDialog(COleClientItem* pItem,
	DWORD dwFlags, CWnd* pParentWnd) : COleDialog(pParentWnd)
{
	if (pItem != NULL)
		ASSERT_VALID(pItem);

	memset(&m_ci, 0, sizeof(m_ci)); // initialize structure to 0/NULL

	// fill in common part
	m_ci.cbStruct = sizeof(m_ci);
	m_ci.dwFlags = dwFlags;
	if (AfxHelpEnabled())
		m_ci.dwFlags |= CIF_SHOWHELP;
	m_ci.lpfnHook = _AfxOleHookProc;
	m_nIDHelp = AFX_IDD_CHANGEICON;

	// specific to this dialog
	if (pItem != NULL)
	{
		pItem->GetClassID(&m_ci.clsid);
		m_ci.hMetaPict = pItem->GetIconicMetafile();
	}
}

COleChangeIconDialog::~COleChangeIconDialog()
{
	_AfxDeleteMetafilePict(m_ci.hMetaPict);
}

int COleChangeIconDialog::DoModal()
{
	ASSERT_VALID(this);
	ASSERT(m_ci.lpfnHook != NULL);  // can still be a user hook

	m_ci.hWndOwner = PreModal();
	int iResult = MapResult(::OleUIChangeIcon(&m_ci));
	PostModal();
	return iResult;
}

BOOL COleChangeIconDialog::DoChangeIcon(COleClientItem* pItem)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pItem);

	// set the picture
	if (!pItem->SetIconicMetafile(GetIconicMetafile()))
		return FALSE;

	// notify the item of the change if the current draw aspect is ICON
	if (pItem->GetDrawAspect() == DVASPECT_ICON)
		pItem->OnChange(OLE_CHANGED, (DWORD)DVASPECT_ICON);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// COleChangeIconDialog diagnostics

#ifdef _DEBUG
void COleChangeIconDialog::Dump(CDumpContext& dc) const
{
	COleDialog::Dump(dc);

	dc << "m_ci.cbStruct = " << m_ci.cbStruct;
	dc << "\nm_ci.dwFlags = " << (LPVOID)m_ci.dwFlags;
	dc << "\nm_ci.hWndOwner = " << (UINT)m_ci.hWndOwner;
	dc << "\nm_ci.lpszCaption = " << m_ci.lpszCaption;
	dc << "\nm_ci.lCustData = " << (LPVOID)m_ci.lCustData;
	dc << "\nm_ci.hInstance = " << (UINT)m_ci.hInstance;
	dc << "\nm_ci.lpszTemplate = " << (LPVOID)m_ci.lpszTemplate;
	dc << "\nm_ci.hResource = " << (UINT)m_ci.hResource;
	if (m_ci.lpfnHook == _AfxOleHookProc)
		dc << "\nhook function set to standard MFC hook function";
	else
		dc << "\nhook function set to non-standard hook function";
	dc << "\nm_ci.hMetaPict = " << (UINT)m_ci.hMetaPict;

	dc << "\n";
}
#endif

/////////////////////////////////////////////////////////////////////////////
// COleLinksDialog

COleLinksDialog::COleLinksDialog(
	COleDocument* pDoc, CView* pView,
	DWORD dwFlags, CWnd* pParentWnd) : COleDialog(pParentWnd)
{
	ASSERT_VALID(pDoc);
	if (pView != NULL)
		ASSERT_VALID(pView);

	memset(&m_el, 0, sizeof(m_el)); // initialize structure to 0/NULL

	// fill in common part
	m_el.cbStruct = sizeof(m_el);
	m_el.dwFlags = dwFlags;
	if (AfxHelpEnabled())
		m_el.dwFlags |= ELF_SHOWHELP;
	m_el.lpfnHook = _AfxOleHookProc;
	m_nIDHelp = AFX_IDD_EDITLINKS;

	// specific to this dialog
	m_pDocument = pDoc;
	if (pView != NULL)
		m_pSelectedItem = m_pDocument->GetPrimarySelectedItem(pView);
	else
		m_pSelectedItem = NULL;
	m_el.lpOleUILinkContainer = &m_xOleUILinkContainer;
}

COleLinksDialog::~COleLinksDialog()
{
}

int COleLinksDialog::DoModal()
{
	ASSERT_VALID(this);
	ASSERT(m_el.lpfnHook != NULL);  // can still be a user hook

	// this function is always used for updating links
	m_bUpdateEmbeddings = FALSE;
	m_bUpdateLinks = TRUE;

	m_el.hWndOwner = PreModal();
	int iResult = MapResult(::OleUIEditLinks(&m_el));
	PostModal();
	return iResult;
}

BEGIN_INTERFACE_MAP(COleLinksDialog, COleDialog)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COleLinksDialog::XOleUILinkContainer

STDMETHODIMP_(ULONG) COleLinksDialog::XOleUILinkContainer::AddRef()
{
	METHOD_PROLOGUE_EX(COleLinksDialog, OleUILinkContainer)
	return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) COleLinksDialog::XOleUILinkContainer::Release()
{
	METHOD_PROLOGUE_EX(COleLinksDialog, OleUILinkContainer)
	return pThis->ExternalRelease();
}

STDMETHODIMP COleLinksDialog::XOleUILinkContainer::QueryInterface(
	REFIID iid, LPVOID* ppvObj)
{
	METHOD_PROLOGUE_EX(COleLinksDialog, OleUILinkContainer)
	return pThis->ExternalQueryInterface(&iid, ppvObj);
}

STDMETHODIMP_(DWORD) COleLinksDialog::XOleUILinkContainer::GetNextLink(
	DWORD dwLink)
{
	METHOD_PROLOGUE_EX(COleLinksDialog, OleUILinkContainer)
	ASSERT_VALID(pThis);

	if (dwLink == 0)
	{
		// start enumerating from the beginning
		pThis->m_pos = pThis->m_pDocument->GetStartPosition();
	}
	COleDocument* pDoc = pThis->m_pDocument;
	COleClientItem* pItem;
	while ((pItem = pDoc->GetNextClientItem(pThis->m_pos)) != NULL)
	{
		// check for links
		OLE_OBJTYPE objType = pItem->GetType();
		if (pThis->m_bUpdateLinks && objType == OT_LINK)
		{
			// link found -- return it
			return (DWORD)(void*)pItem;
		}
		// check for embeddings
		if (pThis->m_bUpdateEmbeddings && objType == OT_EMBEDDED)
		{
			// embedding w/mismatched target device
			return (DWORD)(void*)pItem;
		}
	}
	return 0;   // link not found
}

STDMETHODIMP COleLinksDialog::XOleUILinkContainer::SetLinkUpdateOptions(
	DWORD dwLink, DWORD dwUpdateOpt)
{
	METHOD_PROLOGUE_EX(COleLinksDialog, OleUILinkContainer)
	ASSERT_VALID(pThis);

	COleClientItem* pItem = (COleClientItem*)dwLink;
	ASSERT_VALID(pItem);
	ASSERT(pItem->IsKindOf(RUNTIME_CLASS(COleClientItem)));
	ASSERT(pItem->GetType() == OT_LINK);

	SCODE sc;
	TRY
	{
		// item is a link -- get its link options
		pItem->SetLinkUpdateOptions((OLEUPDATE)dwUpdateOpt);
		sc = S_OK;
	}
	CATCH_ALL(e)
	{
		sc = COleException::Process(e);
		DELETE_EXCEPTION(e);
	}
	END_CATCH_ALL

	return sc;
}

STDMETHODIMP COleLinksDialog::XOleUILinkContainer::GetLinkUpdateOptions(
	DWORD dwLink, DWORD* lpdwUpdateOpt)
{
	METHOD_PROLOGUE_EX(COleLinksDialog, OleUILinkContainer)
	ASSERT_VALID(pThis);

	COleClientItem* pItem = (COleClientItem*)dwLink;
	ASSERT_VALID(pItem);
	ASSERT(pItem->IsKindOf(RUNTIME_CLASS(COleClientItem)));

	SCODE sc;
	TRY
	{
		if (pItem->GetType() == OT_LINK)
			*lpdwUpdateOpt = pItem->GetLinkUpdateOptions();
		else
			*lpdwUpdateOpt = OLEUPDATE_ALWAYS;  // make believe it is auto-link
		sc = S_OK;
	}
	CATCH_ALL(e)
	{
		sc = COleException::Process(e);
		DELETE_EXCEPTION(e);
	}
	END_CATCH_ALL

	return sc;
}

STDMETHODIMP COleLinksDialog::XOleUILinkContainer::SetLinkSource(
	DWORD dwLink, LPTSTR lpszDisplayName, ULONG lenFileName,
	ULONG* pchEaten, BOOL  fValidateSource)
{
	METHOD_PROLOGUE_EX(COleLinksDialog, OleUILinkContainer)
	ASSERT_VALID(pThis);

	COleClientItem* pItem = (COleClientItem*)dwLink;
	ASSERT_VALID(pItem);
	ASSERT(pItem->IsKindOf(RUNTIME_CLASS(COleClientItem)));
	ASSERT(pItem->GetType() == OT_LINK);

	SCODE sc;
	LPMONIKER lpmk;
	CLSID clsid = CLSID_NULL;

	// assume link is not available
	pItem->m_bLinkUnavail = FALSE;

	if (fValidateSource)
	{
		// validate the source by parsing it into a moniker
		LPBC lpbc;
		sc = CreateBindCtx(0, &lpbc);
		if (sc != NOERROR)
			return sc;

		// parse the moniker
		sc = MkParseDisplayName(lpbc, lpszDisplayName, pchEaten, &lpmk);
		if (sc != NOERROR)
		{
			lpbc->Release();
			return sc;
		}
		ASSERT(lpmk != NULL);

		// attempt to bind to the moniker that was successfully parse
		//      (this rarely fails -- but might)
		LPOLEOBJECT lpObject;
		sc = lpmk->BindToObject(lpbc, NULL, IID_IOleObject, (LPLP)&lpObject);
		if (sc != NOERROR)
		{
			pItem->m_bLinkUnavail = TRUE;
			lpbc->Release();
			RELEASE(lpObject);
			return sc;
		}
		ASSERT(lpObject != NULL);

		// have to update the class id in the default handler
		//      (or GetUserType will no longer work)
		// this is done by calling GetUserClassID while running
		lpObject->GetUserClassID(&clsid);
		pItem->m_bLinkUnavail = FALSE;

		// cleanup
		lpObject->Release();
		lpbc->Release();
	}
	else
	{
		LPMONIKER       lpmkFile = NULL;
		LPMONIKER       lpmkItem = NULL;
		TCHAR           szName[_MAX_PATH];

		lstrcpyn(szName, lpszDisplayName, (int)lenFileName + 1);
		sc = CreateFileMoniker(szName, &lpmkFile);
		if (lpmkFile == NULL)
			return sc;

		if (lstrlen(lpszDisplayName) > (int)lenFileName)
		{
			// have item name
			lstrcpy(szName, lpszDisplayName + lenFileName + 1);
			sc = CreateItemMoniker(OLESTDDELIM, szName, &lpmkItem);
			if (lpmkItem == NULL)
			{
				lpmkFile->Release();
				return sc;
			}

			// create composite
			sc = ::CreateGenericComposite(lpmkFile, lpmkItem, &lpmk);

			// release component monikers
			RELEASE(lpmkFile);
			RELEASE(lpmkItem);

			// creating composite moniker may fail...
			if (sc != NOERROR)
				return sc;
		}
		else
		{
			// file moniker is full moniker
			lpmk = lpmkFile;
		}
	}

	// get IOleLink interface
	LPOLELINK lpOleLink = WRAPINTERFACE(pItem->m_lpObject, IOleLink);
	ASSERT(lpOleLink != NULL);

	if (lpmk != NULL)
	{
		// link source validated -- go ahead and set it
		sc = lpOleLink->SetSourceMoniker(lpmk, clsid);
		lpmk->Release();
		if (sc != NOERROR)
		{
			lpOleLink->Release();
			return sc;
		}
	}
	else
	{
		// link source not validated -- just set the display name
		//      (this link will not bind properly)
		sc = lpOleLink->SetSourceDisplayName(lpszDisplayName);
		if (sc != NOERROR)
		{
			lpOleLink->Release();
			return sc;
		}
	}
	lpOleLink->Release();

	return NOERROR;
}

STDMETHODIMP COleLinksDialog::XOleUILinkContainer::GetLinkSource(
	DWORD       dwLink,
	LPTSTR*  lplpszDisplayName,
	ULONG*  lplenFileName,
	LPTSTR*  lplpszFullLinkType,
	LPTSTR*  lplpszShortLinkType,
	BOOL*   lpfSourceAvailable,
	BOOL*   lpfIsSelected)
{
	METHOD_PROLOGUE_EX(COleLinksDialog, OleUILinkContainer)
	ASSERT_VALID(pThis);

	COleClientItem* pItem = (COleClientItem*)dwLink;
	ASSERT_VALID(pItem);
	ASSERT(pItem->IsKindOf(RUNTIME_CLASS(COleClientItem)));
	ASSERT(pItem->GetType() == OT_LINK);

	// set OUT params to NULL
	*lplpszDisplayName  = NULL;
	*lplpszFullLinkType = NULL;
	*lplpszShortLinkType= NULL;
	*lplenFileName      = NULL;
	*lpfSourceAvailable = !pItem->m_bLinkUnavail;

	// get IOleLink interface
	LPOLELINK lpOleLink = WRAPINTERFACE(pItem->m_lpObject, IOleLink);
	ASSERT(lpOleLink != NULL);

	// get moniker & object information
	LPMONIKER lpmk;
	if (lpOleLink->GetSourceMoniker(&lpmk) == NOERROR)
	{
		pItem->m_lpObject->GetUserType(USERCLASSTYPE_FULL, lplpszFullLinkType);
		pItem->m_lpObject->GetUserType(USERCLASSTYPE_SHORT, lplpszShortLinkType);
		*lplenFileName = _AfxOleGetLenFilePrefixOfMoniker(lpmk);
		lpmk->Release();
	}

	// get source display name for moniker
	SCODE sc = lpOleLink->GetSourceDisplayName(lplpszDisplayName);
	lpOleLink->Release();
	if (sc != NOERROR)
		return sc;

	// see if item is selected if specified
	if (lpfIsSelected)
		*lpfIsSelected = (pThis->m_pSelectedItem == pItem);

	return NOERROR;
}

STDMETHODIMP COleLinksDialog::XOleUILinkContainer::OpenLinkSource(DWORD dwLink)
{
	METHOD_PROLOGUE_EX(COleLinksDialog, OleUILinkContainer)
	ASSERT_VALID(pThis);

	COleClientItem* pItem = (COleClientItem*)dwLink;
	ASSERT_VALID(pItem);
	ASSERT(pItem->IsKindOf(RUNTIME_CLASS(COleClientItem)));
	ASSERT(pItem->GetType() == OT_LINK);

	SCODE sc;
	TRY
	{
		// Note: no need for valid CView* since links don't activate inplace
		pItem->DoVerb(OLEIVERB_SHOW, NULL);
		sc = S_OK;
	}
	CATCH_ALL(e)
	{
		sc = COleException::Process(e);
		DELETE_EXCEPTION(e);
	}
	END_CATCH_ALL

	return sc;
}

STDMETHODIMP COleLinksDialog::XOleUILinkContainer::UpdateLink(
	DWORD dwLink, BOOL /*fErrorMessage*/, BOOL /*fErrorAction*/)
{
	METHOD_PROLOGUE_EX(COleLinksDialog, OleUILinkContainer)
	ASSERT_VALID(pThis);

	COleClientItem* pItem = (COleClientItem*)dwLink;
	ASSERT_VALID(pItem);
	ASSERT(pItem->IsKindOf(RUNTIME_CLASS(COleClientItem)));

	SCODE sc;
	TRY
	{
		// link not up-to-date, attempt to update it
		if (!pItem->UpdateLink())
			AfxThrowOleException(pItem->GetLastStatus());
		pItem->m_bLinkUnavail = FALSE;
		sc = S_OK;
	}
	CATCH_ALL(e)
	{
		pItem->m_bLinkUnavail = TRUE;
		sc = COleException::Process(e);
		pItem->ReportError(sc);
		DELETE_EXCEPTION(e);
	}
	END_CATCH_ALL

	return sc;
}

STDMETHODIMP COleLinksDialog::XOleUILinkContainer::CancelLink(DWORD dwLink)
{
	METHOD_PROLOGUE_EX(COleLinksDialog, OleUILinkContainer)
	ASSERT_VALID(pThis);

	COleClientItem* pItem = (COleClientItem*)dwLink;
	ASSERT_VALID(pItem);
	ASSERT(pItem->IsKindOf(RUNTIME_CLASS(COleClientItem)));
	ASSERT(pItem->GetType() == OT_LINK);

	SCODE sc = E_FAIL;
	TRY
	{
		if (pItem->FreezeLink())
			sc = S_OK;
	}
	CATCH_ALL(e)
	{
		sc = COleException::Process(e);
		DELETE_EXCEPTION(e);
	}
	END_CATCH_ALL

	// report error
	if (sc != S_OK)
		pItem->ReportError(sc);

	return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////
// COleLinksDialog diagnostics

#ifdef _DEBUG
void COleLinksDialog::Dump(CDumpContext& dc) const
{
	COleDialog::Dump(dc);

	if (dc.GetDepth() != 0)
	{
		dc << "with document: " << m_pDocument;
		if (m_pSelectedItem != NULL)
			dc << "with selected item: " << m_pSelectedItem;
		else
			dc << "with no selected item\n";
	}
	dc << "m_pos = " << m_pos;
	dc << "\nm_bUpdateLinks = " << m_bUpdateLinks;
	dc << "\nm_bUpdateEmbeddings = " << m_bUpdateEmbeddings;
	dc << "\nm_el.cbStruct = " << m_el.cbStruct;
	dc << "\nm_el.dwFlags = " << (void*)m_el.dwFlags;
	dc << "\nm_el.hWndOwner = " << (UINT)m_el.hWndOwner;
	dc << "\nm_el.lpszCaption = " << m_el.lpszCaption;
	dc << "\nm_el.lCustData = " << (void*)m_el.lCustData;
	dc << "\nm_el.hInstance = " << (UINT)m_el.hInstance;
	dc << "\nm_el.lpszTemplate = " << (void*)m_el.lpszTemplate;
	dc << "\nm_el.hResource = " << (UINT)m_el.hResource;
	if (m_el.lpfnHook == _AfxOleHookProc)
		dc << "\nhook function set to standard MFC hook function";
	else
		dc << "\nhook function set to non-standard hook function";

	dc << "\n";
}

void COleLinksDialog::AssertValid() const
{
	COleDialog::AssertValid();
	m_pDocument->AssertValid();
	if (m_pSelectedItem != NULL)
		m_pSelectedItem->AssertValid();
}
#endif

/////////////////////////////////////////////////////////////////////////////
// COleUpdateDialog

COleUpdateDialog::COleUpdateDialog(COleDocument* pDoc,
	BOOL bUpdateLinks, BOOL bUpdateEmbeddings, CWnd* pParentWnd)
		: COleLinksDialog(pDoc, NULL, 0, pParentWnd)
{
	ASSERT_VALID(pDoc);
	ASSERT(bUpdateLinks || bUpdateEmbeddings);

	// base class COleLinksDialog should have set these up
	ASSERT(m_pDocument == pDoc);
	ASSERT(m_pSelectedItem == NULL);

	// non-base class parameters
	m_bUpdateLinks = bUpdateLinks;
	m_bUpdateEmbeddings = bUpdateEmbeddings;
	m_strCaption.LoadString(AFX_IDS_UPDATING_ITEMS);
}

COleUpdateDialog::~COleUpdateDialog()
{
}

int COleUpdateDialog::DoModal()
{
	ASSERT_VALID(this);

	// first count number of links/embeddings to be updated
	DWORD dwLink = 0;
	int cLinks = 0;
	while ((dwLink = m_el.lpOleUILinkContainer->GetNextLink(dwLink)) != 0)
		++cLinks;
	// when no links are out-of-date, don't bother
	if (cLinks == 0)
		return IDCANCEL;

	// bring up the dialog that processes all the links
	HWND hWndParent = PreModal();
	BOOL bResult = OleUIUpdateLinks(m_el.lpOleUILinkContainer,
		hWndParent, (LPTSTR)(LPCTSTR)m_strCaption, cLinks);
	PostModal();
	return bResult ? IDOK : IDABORT;
}

/////////////////////////////////////////////////////////////////////////////
// COleUpdateDialog diagnostics

#ifdef _DEBUG
void COleUpdateDialog::Dump(CDumpContext& dc) const
{
	COleLinksDialog::Dump(dc);

	dc << "m_strCaption = " << m_strCaption;
	dc << "\n";
}
#endif

/////////////////////////////////////////////////////////////////////////////
// COlePasteSpecialDialog

COlePasteSpecialDialog::COlePasteSpecialDialog(DWORD dwFlags,
	COleDataObject* pDataObject, CWnd* pParentWnd) : COleDialog(pParentWnd)
{
	memset(&m_ps, 0, sizeof(m_ps)); // initialize structure to 0/NULL

	// fill in common part
	m_ps.cbStruct = sizeof(m_ps);
	m_ps.dwFlags = dwFlags;
	if (AfxHelpEnabled())
		m_ps.dwFlags |= PSF_SHOWHELP;
	m_ps.lpfnHook = _AfxOleHookProc;
	m_nIDHelp = AFX_IDD_PASTESPECIAL;

	// get LPDATAOBJECT for paste special dialog
	COleDataObject dataObject;
	if (pDataObject == NULL)
	{
		VERIFY(dataObject.AttachClipboard());
		pDataObject = &dataObject;
	}
	ASSERT(pDataObject != NULL);
	m_ps.lpSrcDataObj = pDataObject->GetIDataObject(TRUE);

	// complete initialization
	m_ps.arrPasteEntries = NULL;
	m_ps.cPasteEntries = 0;
	m_ps.arrLinkTypes = m_arrLinkTypes;
	m_ps.cLinkTypes = 0;
}

COlePasteSpecialDialog::~COlePasteSpecialDialog()
{
	_AfxDeleteMetafilePict(m_ps.hMetaPict);

	for (int i = 0; i < m_ps.cPasteEntries; i++)
	{
		free((void*)m_ps.arrPasteEntries[i].lpstrFormatName);
		free((void*)m_ps.arrPasteEntries[i].lpstrResultText);
	}
	free(m_ps.arrPasteEntries);

	RELEASE(m_ps.lpSrcDataObj);
}

int COlePasteSpecialDialog::DoModal()
{
	ASSERT_VALID(this);
	ASSERT(m_ps.lpfnHook != NULL);  // can still be a user hook

	// return error if IDataObject* not available
	if (m_ps.lpSrcDataObj == NULL)
		return IDABORT;

	m_ps.hWndOwner = PreModal();
	int iResult = MapResult(::OleUIPasteSpecial(&m_ps));
	PostModal();
	return iResult;
}

UINT COlePasteSpecialDialog::GetSelectionType() const
{
	ASSERT_VALID(this);
	ASSERT(m_ps.dwFlags & (PSF_SELECTPASTE|PSF_SELECTPASTELINK));

	UINT cf = m_ps.arrPasteEntries[m_ps.nSelectedIndex].fmtetc.cfFormat;
	Selection selType = pasteOther;
	if (m_ps.dwFlags & PSF_SELECTPASTELINK)
	{
		selType = pasteLink;
	}
	else if (cf == _oleData.cfEmbedSource || cf == _oleData.cfEmbeddedObject ||
			cf == _oleData.cfLinkSource)
	{
		selType = pasteNormal;
	}
	else if (cf == CF_METAFILEPICT || cf == CF_DIB || cf == CF_BITMAP)
	{
		selType = pasteStatic;
	}
	return selType;
}

/////////////////////////////////////////////////////////////////////////////
// COlePasteSpecialDialog diagnostics

#ifdef _DEBUG
void COlePasteSpecialDialog::Dump(CDumpContext& dc) const
{
	COleDialog::Dump(dc);

	dc << "m_ps.cbStruct = " << m_ps.cbStruct;
	dc << "\nm_ps.dwFlags = " << (LPVOID)m_ps.dwFlags;
	dc << "\nm_ps.hWndOwner = " << (UINT)m_ps.hWndOwner;
	dc << "\nm_ps.lpszCaption = " << m_ps.lpszCaption;
	dc << "\nm_ps.lCustData = " << (LPVOID)m_ps.lCustData;
	dc << "\nm_ps.hInstance = " << (UINT)m_ps.hInstance;
	dc << "\nm_ps.lpszTemplate = " << (LPVOID)m_ps.lpszTemplate;
	dc << "\nm_ps.hResource = " << (UINT)m_ps.hResource;
	if (m_ps.lpfnHook == _AfxOleHookProc)
		dc << "\nhook function set to standard MFC hook function";
	else
		dc << "\nhook function set to non-standard hook function";
	dc << "\nm_ps.lpSrcDataObj = " << (LPVOID)m_ps.lpSrcDataObj;
	dc << "\nm_ps.cPasteEntries = " << m_ps.cPasteEntries;
	dc << "\nm_ps.cLinkTypes = " << m_ps.cLinkTypes;
	dc << "\nm_ps.nSelectedIndex = " << m_ps.nSelectedIndex;
	dc << "\nm_ps.fLink = " << m_ps.fLink;

	dc << "\n";
}

void COlePasteSpecialDialog::AssertValid() const
{
	COleDialog::AssertValid();
	ASSERT(m_ps.cPasteEntries == 0 || m_ps.arrPasteEntries != NULL);
	ASSERT(m_ps.arrLinkTypes != NULL);
	ASSERT(m_ps.cLinkTypes <= 8);
}
#endif

////////////////////////////////////////////////////////////////////////////

OLEUIPASTEFLAG COlePasteSpecialDialog::AddLinkEntry(UINT cf)
{
	ASSERT_VALID(this);
	ASSERT(m_ps.cLinkTypes <= 8);
	for (int i = 0; i < m_ps.cLinkTypes; i++)
	{
		if (m_ps.arrLinkTypes[i] == cf)
			break;
	}
	if (i == 8)
		return (OLEUIPASTEFLAG)0;
	m_ps.arrLinkTypes[i] = cf;
	if (i == m_ps.cLinkTypes)
		m_ps.cLinkTypes++;
	return (OLEUIPASTEFLAG) (OLEUIPASTE_LINKTYPE1 << i);
}

void COlePasteSpecialDialog::AddFormat(UINT cf, DWORD tymed, UINT nFormatID,
	BOOL bEnableIcon, BOOL bLink)
{
	TCHAR szFormat[256];
	if (AfxLoadString(nFormatID, szFormat) == 0)
		AfxThrowResourceException();

	// the format and result strings are delimited by a newline
	LPTSTR lpszResult = _tcschr(szFormat, '\n');
	ASSERT(lpszResult != NULL);  // must contain a newline
	*lpszResult = '\0';
	++lpszResult;    // one char past newline

	// add it to the array of acceptable formats
	m_ps.arrPasteEntries = (OLEUIPASTEENTRY *)realloc(m_ps.arrPasteEntries,
		sizeof(OLEUIPASTEENTRY) * (m_ps.cPasteEntries +1));

	OLEUIPASTEENTRY* pEntry = &m_ps.arrPasteEntries[m_ps.cPasteEntries];
	pEntry->fmtetc.cfFormat = (CLIPFORMAT)cf;
	pEntry->fmtetc.dwAspect = DVASPECT_CONTENT;
	pEntry->fmtetc.ptd = NULL;
	pEntry->fmtetc.tymed = tymed;
	pEntry->fmtetc.lindex = -1;
	pEntry->lpstrFormatName = _tcsdup(szFormat);
	pEntry->lpstrResultText = _tcsdup(lpszResult);
	pEntry->dwFlags = OLEUIPASTE_PASTE;

	if (bEnableIcon)
		pEntry->dwFlags |= OLEUIPASTE_ENABLEICON;
	if (bLink)
		pEntry->dwFlags |= AddLinkEntry(cf);
	if (pEntry->dwFlags == OLEUIPASTE_PASTE)
		pEntry->dwFlags = OLEUIPASTE_PASTEONLY;
	pEntry->dwScratchSpace = NULL;
	m_ps.cPasteEntries++;
}

// if the flags parameter includes a LINKTYPE# flag, it should be obtained
// from AddLinkEntry
void COlePasteSpecialDialog::AddFormat(const FORMATETC& formatEtc,
	LPTSTR lpszFormat, LPTSTR lpszResult, DWORD dwFlags)
{
	ASSERT_VALID(this);

	m_ps.arrPasteEntries = (OLEUIPASTEENTRY *)realloc(
		m_ps.arrPasteEntries, sizeof(OLEUIPASTEENTRY) * (m_ps.cPasteEntries +1));
	OLEUIPASTEENTRY* pEntry = &m_ps.arrPasteEntries[m_ps.cPasteEntries];
	pEntry->fmtetc = formatEtc;
	pEntry->lpstrFormatName = _tcsdup(lpszFormat);
	pEntry->lpstrResultText = _tcsdup(lpszResult);
	pEntry->dwFlags = dwFlags;
	pEntry->dwScratchSpace = NULL;
	m_ps.cPasteEntries++;
}

void COlePasteSpecialDialog::AddStandardFormats(BOOL bEnableLink)
{
	// Note: only need to add Embedded Object because Embed Source is
	//  automatically recognized by the paste special dialog implementation.
	ASSERT(_oleData.cfEmbeddedObject != NULL);
	AddFormat(_oleData.cfEmbeddedObject, TYMED_ISTORAGE, AFX_IDS_EMBED_FORMAT,
		TRUE, FALSE);

	// add link source if requested
	if (bEnableLink)
	{
		ASSERT(_oleData.cfLinkSource != NULL);
		AddFormat(_oleData.cfLinkSource, TYMED_ISTREAM, AFX_IDS_LINKSOURCE_FORMAT,
			TRUE, TRUE);
	}

	// add formats that can be used for 'static' items
	AddFormat(CF_METAFILEPICT, TYMED_MFPICT, AFX_IDS_METAFILE_FORMAT,
		FALSE, FALSE);
	AddFormat(CF_DIB, TYMED_HGLOBAL, AFX_IDS_DIB_FORMAT, FALSE, FALSE);
	AddFormat(CF_BITMAP, TYMED_GDI, AFX_IDS_BITMAP_FORMAT, FALSE, FALSE);
}

BOOL COlePasteSpecialDialog::CreateItem(COleClientItem *pNewItem)
{
	ASSERT_VALID(this);
	ASSERT(pNewItem != NULL);
	ASSERT(m_ps.lpSrcDataObj != NULL);

	BeginWaitCursor();

	COleDataObject dataObject;
	dataObject.Attach(m_ps.lpSrcDataObj, FALSE);

	UINT selType = GetSelectionType();
	BOOL bResult = TRUE;

	switch (selType)
	{
	case pasteLink:
		// paste link
		if (!pNewItem->CreateLinkFromData(&dataObject))
		{
			TRACE0("Warning: CreateLinkFromData failed.\n");
			bResult = FALSE;
		}
		break;
	case pasteStatic:
		if (!pNewItem->CreateStaticFromData(&dataObject))
		{
			TRACE0("Warning: CreateStaticFromData failed.\n");
			bResult = FALSE;
		}
		break;
	default:
		ASSERT(selType == pasteNormal);
		if (!pNewItem->CreateFromData(&dataObject))
		{
			TRACE0("Warning: CreateFromData failed.\n");
			bResult = FALSE;
		}
		break;
	}

	// deal with Display As Iconic option
	if (bResult && GetDrawAspect() == DVASPECT_ICON)
	{
		// setup iconic cache (it will draw iconic by default as well)
		if (!pNewItem->SetIconicMetafile(m_ps.hMetaPict))
		{
			TRACE0("Warning: failed to set iconic aspect.\n");
			bResult = FALSE;
		}
		else
		{
			// since picture was set OK, draw as iconic as well...
			pNewItem->SetDrawAspect(DVASPECT_ICON);
		}
	}
	EndWaitCursor();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// Inline function declarations expanded out-of-line

#ifndef _AFX_ENABLE_INLINES

// expand inlines for OLE dialog APIs
static char _szAfxOleInl[] = "afxole.inl";
#undef THIS_FILE
#define THIS_FILE _szAfxOleInl
#define _AFXODLGS_INLINE
#include "afxole.inl"

#endif //!_AFX_ENABLE_INLINES

#undef new
#ifdef AFX_INIT_SEG
#pragma code_seg(AFX_INIT_SEG)
#endif

IMPLEMENT_DYNAMIC(COleInsertDialog, COleDialog)
IMPLEMENT_DYNAMIC(COleConvertDialog, COleDialog)
IMPLEMENT_DYNAMIC(COleChangeIconDialog, COleDialog)
IMPLEMENT_DYNAMIC(COleLinksDialog, COleDialog)
IMPLEMENT_DYNAMIC(COleUpdateDialog, COleLinksDialog)
IMPLEMENT_DYNAMIC(COlePasteSpecialDialog, COleDialog)

/////////////////////////////////////////////////////////////////////////////
