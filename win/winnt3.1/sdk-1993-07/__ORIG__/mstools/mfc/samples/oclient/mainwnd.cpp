// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// MAINWND.CXX : main window

#include "oclient.h"
#include "mainwnd.h"
#include "itemwnd.h"

static LPSTR CreateNewUniqueName(LPSTR lpstr);

/////////////////////////////////////////////////////////////////////////////


#pragma warning(disable:4355)
// C4355 is "'this' used in base initializer list" warning

CMainWnd::CMainWnd()
	: m_document(this)
{
	VERIFY(m_title.LoadString(IDS_APPNAME));
	VERIFY(LoadAccelTable(MAKEINTRESOURCE(ID_APPLICATION)));
	Create(NULL, m_title, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		rectDefault, NULL, MAKEINTRESOURCE(ID_APPLICATION));
	ASSERT(m_hWnd != NULL);
	m_pSelection = NULL;
	m_fDirty = FALSE;
}

#pragma warning(default:4355)

// Iterator over content
COleClientItem* CMainDocument::GetNextItem(POSITION& rPosition,
		BOOL* pIsSelected)
{
	CItemWnd* pItemWnd = (CItemWnd*) rPosition;
	if (pItemWnd == NULL)
		pItemWnd = (CItemWnd*)m_pView->GetTopWindow();
	else
		pItemWnd = (CItemWnd*)pItemWnd->GetNextWindow();

	*pIsSelected = (m_pView->GetSelection() == pItemWnd);
	rPosition = (POSITION)pItemWnd;
	return (pItemWnd == NULL) ? NULL : pItemWnd->GetEmbedded();
}


/////////////////////////////////////////////////////////////////////////////

void CMainWnd::OnAbout()
{
	CModalDialog about(MAKEINTRESOURCE(IDDT_ABOUT), this);
	about.DoModal();
}

void CMainWnd::Hourglass(BOOL bOn)
{
	static int count = 0;
	static HCURSOR hcurWait = AfxGetApp()->LoadStandardCursor(IDC_WAIT);
	static HCURSOR hcurLast;

	ASSERT(hcurWait != NULL);
	if (bOn)
	{
		if (count++ == 0)
			hcurLast = ::SetCursor(hcurWait);
	}
	else
	{
		if (--count == 0)
			::SetCursor(hcurLast);
	}
}

void CMainWnd::OnClose()
{
	if (COleClientItem::InWaitForRelease())
	{
		ErrorMessage(E_BUSY);
		return;
	}

	if (!SaveAsNeeded())
		return;     // don't quit

	DeregisterDoc();
	DestroyWindow();
}

BOOL CMainWnd::OnQueryEndSession()
{
	if (COleClientItem::InWaitForRelease())
		return FALSE;
	return CFrameWnd::OnQueryEndSession();
}

void CMainWnd::OnDestroy()
{
	GetDocument()->Revoke();    // revoke just in case
	CFrameWnd::OnDestroy(); // will close down the app
}

/////////////////////////////////////////////////////////////////////////////
// Error reporting

void CMainWnd::ErrorMessage(UINT id)
{
	// if in exit mode no message box
	if (!GetDocument()->IsOpen())
		return;

	CString str;
	str.LoadString(id);
	MessageBox(str, AfxGetAppName(), MB_OK | MB_ICONEXCLAMATION);
}

/////////////////////////////////////////////////////////////////////////////

void CMainWnd::OnExit()
{
	// close window - NOTE: Do _not_ call 'OnClose' message map function
	SendMessage(WM_CLOSE);
}

/////////////////////////////////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(CMainWnd, CFrameWnd)
	// windows messages
	ON_WM_INITMENUPOPUP()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_QUERYENDSESSION()
	ON_WM_ACTIVATE()

	// file menu commands
	ON_COMMAND(IDM_NEW, OnFileNew)
	ON_COMMAND(IDM_OPEN, OnFileOpen)
	ON_COMMAND(IDM_SAVE, OnFileSave)
	ON_COMMAND(IDM_SAVEAS, OnFileSaveAs)
	ON_COMMAND(IDM_EXIT, OnExit)
	ON_COMMAND(IDM_ABOUT, OnAbout)
	// edit menu commands
	ON_COMMAND(IDM_CUT, OnCut)
	ON_COMMAND(IDM_COPY, OnCopy)
	ON_COMMAND(IDM_PASTE, OnPaste)
	ON_COMMAND(IDM_PASTELINK, OnPasteLink)
	ON_COMMAND(IDM_CLEAR, OnClear)
	ON_COMMAND(IDM_CLEARALL, OnClearAll)
	ON_COMMAND(IDM_INSERT_OBJECT, OnInsertObject)
	ON_COMMAND(IDM_LINKS, OnEditLinks)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Most Edit Commands

void CMainWnd::OnInitMenuPopup(CMenu* pMenu, UINT iMenu, BOOL bSysMenu)
{
	if (bSysMenu || iMenu != IMENU_EDIT)
		return; // system and file menu don't do any dynamic updating

	ASSERT(pMenu != NULL);
	// only enable/diable items on the Edit menu
	CItemWnd* pSelection = GetSelection();

	//BLOCK: Simple editing
	{
		UINT    mfCanEdit = MF_GRAYED|MF_DISABLED;

		if (pSelection != NULL)
			mfCanEdit = MF_ENABLED; // have some content

		// Now enable appropriate menu items
		pMenu->EnableMenuItem(IDM_CUT, mfCanEdit);
		pMenu->EnableMenuItem(IDM_COPY, mfCanEdit);
		pMenu->EnableMenuItem(IDM_CLEAR, mfCanEdit);
		pMenu->EnableMenuItem(IDM_CLEARALL, mfCanEdit);
	}

	//BLOCK: paste variants
	{
		pMenu->EnableMenuItem(IDM_PASTE, COleClientItem::CanPaste() ?
			 MF_ENABLED : (MF_GRAYED|MF_DISABLED));
		pMenu->EnableMenuItem(IDM_PASTELINK, COleClientItem::CanPasteLink() ?
			 MF_ENABLED : (MF_GRAYED|MF_DISABLED));
	}

	//BLOCK: special case if any linked objects
	{
		UINT    mfLinks = MF_GRAYED|MF_DISABLED;

		CItemWnd* pItemWnd;
		for (pItemWnd = (CItemWnd*)GetTopWindow(); pItemWnd != NULL;
			pItemWnd = (CItemWnd*)pItemWnd->GetNextWindow())
		{
			if (pItemWnd->GetEmbedded()->GetType() == OT_LINK)
			{
				mfLinks = MF_ENABLED;
				break;
			}
		}
		pMenu->EnableMenuItem(IDM_LINKS, mfLinks);
	}

	//BLOCK: Object menu specific
	{
		static int iItemObjectVerb = -1;    // position on the Edit menu

		if (iItemObjectVerb == -1)
		{
			// one-time init - find the position of the "Object ?" menuitem
			int nItems = pMenu->GetMenuItemCount();
			for (int iItem = 0; iItem < nItems; iItem++)
			{
				if (pMenu->GetMenuItemID(iItem) == IDM_OBJECT_VERB_MIN)
				{
					iItemObjectVerb = iItem;
					break;
				}
			}
			ASSERT(iItemObjectVerb != -1);
				// MUST BE IN THE MENU TEMPLATE ON INIT !
		}

		AfxOleSetEditMenu(pSelection == NULL ? NULL : pSelection->GetEmbedded(),
			pMenu, iItemObjectVerb, IDM_OBJECT_VERB_MIN);
	}
}

BOOL CMainWnd::OnCommand(UINT wParam, LONG lParam)
{
	// if we are waiting for release - ignore all commands
	if (COleClientItem::InWaitForRelease())
	{
		ErrorMessage(E_BUSY);
		return TRUE;        // handled
	}

	if (LOWORD(lParam) == 0 && wParam >= IDM_OBJECT_VERB_MIN &&
		wParam <= IDM_OBJECT_VERB_MAX)
	{
		// activate the current selection with the appropriate verb
		CItemWnd* pItemWnd = GetSelection();
		ASSERT(pItemWnd != NULL);
		pItemWnd->DoVerb(wParam - IDM_OBJECT_VERB_MIN);
		return TRUE;        // handled
	}
	return CFrameWnd::OnCommand(wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
// Selection support

void CMainWnd::SetSelection(CItemWnd* pNewSelection)
{
	if (pNewSelection != GetTopWindow() && pNewSelection != NULL)
		pNewSelection->BringWindowToTop();

	if (m_pSelection != pNewSelection)
	{
		// de-select old
		if (m_pSelection != NULL)
			m_pSelection->Select(FALSE);
		// select the new
		if (pNewSelection != NULL)
		{
			pNewSelection->Select(TRUE);
		}
		m_pSelection = pNewSelection;
	}
}

void CMainWnd::OnActivate(UINT nState, CWnd*, BOOL)
{
	if (m_pSelection)
		m_pSelection->Select(nState != 0);
}

/////////////////////////////////////////////////////////////////////////////

void CMainWnd::OnCut()
{
	if (!DoCopySelection())
	{
		ErrorMessage(E_CLIPBOARD_CUT_FAILED);
		return;
	}
	// cut it out
	OnClear();
	Dirty();
}


void CMainWnd::OnCopy()
{
	if (!DoCopySelection())
		ErrorMessage(E_CLIPBOARD_COPY_FAILED);
}

BOOL CMainWnd::DoCopySelection()
{
	CItemWnd* pSel = GetSelection();
	ASSERT(pSel != NULL);

	if (!OpenClipboard())
		return FALSE;

	/* Empty the clipboard */
	EmptyClipboard();

	TRY
		pSel->GetEmbedded()->CopyToClipboard();
	CATCH (CException, e)
		return FALSE;
	END_CATCH

	CloseClipboard();
	return TRUE;
}


void CMainWnd::OnClear()
{
	ASSERT(GetSelection() != NULL);
	CItemWnd* pSel = GetSelection();

	if (pSel)
		pSel->DestroyWindow();

	Dirty();
}

void CMainWnd::ClearAll()
{
	CWnd* pKid;
	while ((pKid = GetTopWindow()) != NULL)
	{
		VERIFY(pKid->DestroyWindow());
	}
}

void CMainWnd::OnClearAll()
{
	ClearAll();      /* Wipes out all items */
	Dirty();
}

void CMainWnd::OnPaste()
{
	if (!DoPaste(FALSE))
		ErrorMessage(E_GET_FROM_CLIPBOARD_FAILED);
	Dirty();
}

void CMainWnd::OnPasteLink()
{
	if (!DoPaste(TRUE))
		ErrorMessage(E_GET_FROM_CLIPBOARD_FAILED);
}

BOOL CMainWnd::DoPaste(BOOL fLink)
{
	if (!OpenClipboard())
		return NULL;                    /* Couldn't open the clipboard */

	Hourglass(TRUE);

	/* Don't replace the current object unless we're successful */
	CItemWnd* pItemWnd = new CItemWnd(this);
	CEmbeddedItem* pItem = pItemWnd->GetEmbedded();
	char szName[OLE_MAXNAMESIZE];
	CreateNewUniqueName(szName);

	TRY
	{
		if (fLink)
		{
			if (!pItem->CreateLinkFromClipboard(szName))
				AfxThrowMemoryException();      // any exception will do
		}
		else
		{
			// paste embedded
			if (pItem->CreateFromClipboard(szName))
			{
				TRACE("embedded an embedded OLE object\n");
			}
			else if (pItem->CreateStaticFromClipboard(szName))
			{
				TRACE("embedded a static picture object\n");
			}
			else
			{
				AfxThrowMemoryException();      // any exception will do
			}
		}
	}
	CATCH (CException, e)
	{
		// general cleanup
		TRACE("failed to embed/link an OLE object\n");
		delete pItemWnd;
		pItemWnd = NULL;
	}
	END_CATCH

	CloseClipboard();
	Hourglass(FALSE);

	if (pItemWnd == NULL)
		return FALSE;
	
	// create as a live health ItemWnd
	if (!pItemWnd->CreateItemWindow(TRUE))
	{
		delete pItemWnd;
		return FALSE;
	}

	// Success !
	//  (from this point on the PostNcCreate hook will clean up memory)
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// Insert new object

void CMainWnd::OnInsertObject()
{
	CString className;

	if (!AfxOleInsertDialog(className))
		return;

	TRACE("Trying to Insert OLE object (class '%s')\n", (const char*)className);
	CItemWnd* pItemWnd = new CItemWnd(this);
	CEmbeddedItem* pItem = pItemWnd->GetEmbedded();

	char szTmp[OLE_MAXNAMESIZE];
	TRY
	{
		if (!pItem->CreateNewObject(className, CreateNewUniqueName(szTmp)))
			AfxThrowMemoryException();      // any exception will do
	}
	CATCH (CException, e)
	{
		ErrorMessage(E_FAILED_TO_CREATE);
		// clean up item
		delete pItemWnd;
		return;
	}
	END_CATCH

	// Create invisible (will become visible later when updated)
	if (!pItemWnd->CreateItemWindow(FALSE))
		delete pItemWnd;

	// Success!
	//  (from this point on the PostNcCreate hook will clean up memory)

	Dirty();
}

/////////////////////////////////////////////////////////////////////////////

void FixObjectBounds(CRect& rect)
{
	/* If we have an empty rectangle, start at default size */
	if (rect.IsRectNull())
	{
		rect.SetRect(0, 0, CXDEFAULT, CYDEFAULT);
	}
	else
	{
		// First map from HIMETRIC back to screen coordinates
		{
			CClientDC screenDC(NULL);

			short oldMode = screenDC.SetMapMode(MM_HIMETRIC);
			screenDC.LPtoDP(&rect);
			screenDC.SetMapMode(oldMode);
		}

		/* Preserve the Aspect Ratio of the picture */
		DWORD xDiff = (DWORD) (rect.right - rect.left + 1);
		DWORD yDiff = (DWORD) (rect.bottom - rect.top + 1);

		/* Don't use *= here because of integer arithmetic. */
		if (xDiff > CXDEFAULT || yDiff > CYDEFAULT)
		{
			if ((xDiff * CYDEFAULT) > (yDiff * CXDEFAULT))
			{
				yDiff = ((yDiff * CXDEFAULT) / xDiff);
				xDiff = CXDEFAULT;
			}
			else
			{
				xDiff = ((xDiff * CYDEFAULT) / yDiff);
				yDiff = CYDEFAULT;
			}
		}
		rect.SetRect(0, 0, (int)xDiff - 1, (int)yDiff - 1);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Name generator helper

LPSTR CreateNewUniqueName(LPSTR lpstr)
{
	static int CurrentNumber = 0;
	wsprintf(lpstr, "%s%04d", (LPSTR)OBJ_NAME_PREFIX, CurrentNumber++);
	return(lpstr);
}

/////////////////////////////////////////////////////////////////////////////

void CMainWnd::OnEditLinks()
{
	CItemWnd* pSelection = GetSelection();

	AfxOleLinksDialog(GetDocument());
}

/////////////////////////////////////////////////////////////////////////////
// Diagnostics

#ifdef _DEBUG
void CMainWnd::AssertValid() const
{
	ASSERT(GetDocument()->IsOpen());
}
#endif

/////////////////////////////////////////////////////////////////////////////
