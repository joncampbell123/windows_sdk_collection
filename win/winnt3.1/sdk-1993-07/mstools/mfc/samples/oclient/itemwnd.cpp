// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// user interface to OLE embedded objects
//
// Each OLE embedded object is given it's own window to draw in
// We use the windows thick frame to provide sizing (NOTE: this is not
//      UISG conformant!)

#include "oclient.h"

#include "mainwnd.h"
#include "itemwnd.h"

/////////////////////////////////////////////////////////////////////////////
// Static members for dragging state

CRect CItemWnd::dragRect;
CPoint CItemWnd::dragPt;

/////////////////////////////////////////////////////////////////////////////
// Message map for ItemWnd

BEGIN_MESSAGE_MAP(CItemWnd, CWnd)
	// windows messages
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Creation

#pragma warning(disable:4355)
// C4355 is "'this' used in base initializer list" warning

CItemWnd::CItemWnd(CMainWnd* pContainer)
	: m_embedded(pContainer->GetDocument(), this)
{
	m_pContainer = pContainer;
	m_fVisible = m_fTrackSize = FALSE;
	m_fCaptured = FALSE;
}

#pragma warning(default:4355)


BOOL CItemWnd::CreateItemWindow(BOOL fShow)
{
	ASSERT(m_pContainer != NULL);
	CRect   rectBounds;

	if (!GetEmbedded()->GetBounds(&rectBounds))
		rectBounds.SetRectEmpty(); // server doesn't know about the bounds

	FixObjectBounds(rectBounds);

	rectBounds.OffsetRect(2 * GetSystemMetrics(SM_CXFRAME),
					2 * GetSystemMetrics(SM_CYFRAME));

	if (!CWnd::Create(NULL, NULL /* no title */,
		WS_BORDER | WS_CHILD | WS_CLIPSIBLINGS | WS_THICKFRAME,
		rectBounds, m_pContainer, 0))
	{
		return FALSE;
	}

	m_fVisible = fShow;
	m_fTrackSize = TRUE;

	GetEmbedded()->SetNames();
	/* Make the object visible, and paint it if fShow == TRUE */
	if (fShow)
	{
		ShowWindow(SW_SHOW);
		m_pContainer->SetSelection(this);
	}

	return TRUE;
}

BOOL CItemWnd::RestoreItemWindow(const RECT& rect)
{
	ASSERT(m_pContainer != NULL);
	CRect rectBounds = rect;

	rectBounds.OffsetRect(2 * GetSystemMetrics(SM_CXFRAME),
					2 * GetSystemMetrics(SM_CYFRAME));

	if (!CWnd::Create(NULL, NULL /* no title */,
		WS_BORDER | WS_CHILD | WS_CLIPSIBLINGS | WS_THICKFRAME,
		rectBounds, m_pContainer, 0))
	{
		return FALSE;
	}

	GetEmbedded()->SetNames();

	if (m_fVisible)
		ShowWindow(SW_SHOW);
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////

void CItemWnd::PostNcDestroy()
{
	ASSERT(m_hWnd == NULL);     // must be detached

	if (m_pContainer->GetSelection() == this)
		m_pContainer->SetSelection(NULL);

	// finally free up the C++ object and memory
		// (will destroy embedded object as needed)
	delete this;
}


BOOL CItemWnd::OnEraseBkgnd(CDC* pDC)
{
	CBrush myBrush(GetSysColor(COLOR_WINDOW));
	CRect rect;
	GetClientRect(&rect);
	pDC->FillRect(rect, &myBrush);
	return TRUE;        // we handled it
}


void CItemWnd::OnPaint()
{
	CPaintDC dc(this);
	CRect   rect;

	// set up a reasonable default context
	dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
	dc.SetBkColor(::GetSysColor(COLOR_WINDOW));

	// Draw the item
	GetClientRect(&rect);
	GetEmbedded()->Draw(&dc, &rect, NULL, &dc);
		// ignore if can't draw
}


void CItemWnd::OnSize(UINT, int, int)
{
	Dirty();

	// Stop tracking size.  If user didn't change size, the flag
	// will be reset later.
	m_fTrackSize = FALSE;
}


void CItemWnd::DoVerb(UINT nVerb)
	// "run" the object
{
	if (GetEmbedded()->GetType() == OT_STATIC)
		return;
	CRect   rect;
	GetClientRect(&rect);

	TRY
	{
		GetEmbedded()->Activate(nVerb, TRUE, TRUE, this, &rect);
	}
	CATCH (COleException, e)
	{
		GetEmbedded()->ReportError(e->m_status);
	}
	AND_CATCH (CException, e)
	{
		// general error when playing
		m_pContainer->ErrorMessage(E_FAILED_TO_LAUNCH);
	}
	END_CATCH
}

/////////////////////////////////////////////////////////////////////////////
// Mouse messages

void CItemWnd::OnLButtonDblClk(UINT, CPoint)
{
	DoVerb(OLEVERB_PRIMARY);
}

void CItemWnd::OnLButtonDown(UINT, CPoint point)
{
	m_pContainer->SetSelection(this);

	GetWindowRect(&dragRect);
	GetParent()->ScreenToClient(&dragRect);

	dragPt = point;

	ClientToScreen(&dragPt);
	GetParent()->ScreenToClient(&dragPt);

	SetCapture();
	m_fCaptured = TRUE;
}

void CItemWnd::OnLButtonUp(UINT, CPoint)
{
	if (!m_fCaptured)
		return;

	ReleaseCapture();
	m_fCaptured = FALSE;

	/* The object moved */
	Dirty();
}

void CItemWnd::OnMouseMove(UINT, CPoint point)
{
	if (!m_fCaptured)
		return;

	ClientToScreen(&point);
	GetParent()->ScreenToClient(&point);

	dragRect.OffsetRect(point.x - dragPt.x, point.y - dragPt.y);
	MoveWindow(dragRect);
	dragPt = point;
}

/////////////////////////////////////////////////////////////////////////////
// Serialization

// first WORD in stream is 0x5500 + extra bits

void CItemWnd::Serialize(CArchive& ar)
{
	// save the window information + embedded
	CRect   rect;

	if (ar.IsStoring())
	{
		// First save our window part
		ASSERT(m_fVisible);     // only serialize visible window
		WORD w = 0x5500;        // magic value
		if (m_fTrackSize)
			w += 1;
		ar << w;

		// get window position (parent relative)
		GetClientRect(&rect);
		ClientToScreen(&rect);
		GetParent()->ScreenToClient(&rect);
		rect -= CPoint(GetSystemMetrics(SM_CXFRAME),
			 GetSystemMetrics(SM_CYFRAME));
		ar << rect;
	}
	else // loading
	{
		WORD w;
		ar >> w;

		// First load our window part
		if (HIBYTE(w) != 0x55)
		{
			TRACE("Bad magic number in front of an item wnd\n");
			AfxThrowArchiveException(CArchiveException::generic);
		}
		m_fVisible = TRUE;
		m_fTrackSize = (w & 1) != 0;
		ar >> rect;
	}

	// now do the OLE Embedded part
	GetEmbedded()->Serialize(ar);

	if (ar.IsLoading())
	{
		// Wrap-up loading - create an ItemWnd as appropriate
		if (!RestoreItemWindow(rect))
			AfxThrowArchiveException(CArchiveException::generic);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Special handling for OLE Client notification

void CEmbeddedItem::SetNames()
{
	if (GetType() == OT_EMBEDDED)
		SetHostNames(AfxGetAppName(), GetName());
}

// turn on hourglass when waiting for server

void CEmbeddedItem::WaitForServer()
{
	m_pView->m_pContainer->Hourglass(TRUE);
	COleClientItem::WaitForServer();
	m_pView->m_pContainer->Hourglass(FALSE);
}

void CEmbeddedItem::OnChange(OLE_NOTIFICATION wNotification)
{
	/* Item just created or we are updating size */
	if (m_pView->m_hWnd == NULL)
		return;         // no window created yet

	if (m_pView->CanChangeBounds())
	{
		CRect rect;

		if (GetBounds(&rect))
		{
			FixObjectBounds(rect);
			m_pView->SetInitialBounds(rect);
		}
		else
		{
			// Blank object
			if (wNotification == OLE_CLOSED)
			{
				// no data received for the object - destroy it
				// we can't call destroy window here since we are
				//   and OLE callback - so we post a close message instead
				m_pView->PostMessage(WM_CLOSE);
				return;
			}
		}
	}

	m_pView->InvalidateRect(NULL, TRUE);    // erase it
	m_pView->Dirty();
}

void CItemWnd::SetInitialBounds(const CRect& rect)
{
	BOOL fTrackSizeSave = m_fTrackSize; // save since OnSize will change it
	SetWindowPos(NULL, 0, 0,
		rect.right - rect.left + 2*GetSystemMetrics(SM_CXFRAME),
		rect.bottom - rect.top + 2*GetSystemMetrics(SM_CYFRAME),
		SWP_NOZORDER | SWP_NOMOVE | SWP_DRAWFRAME);
	m_fTrackSize = fTrackSizeSave;

	// show it
	m_fVisible = TRUE;
	ShowWindow(SW_SHOW);
	m_pContainer->SetSelection(this);
}

/////////////////////////////////////////////////////////////////////////////
// A way to get the thick frame window to look good
	// not a generally useful trick

void CItemWnd::Select(BOOL bOn)
{
	if (m_hWnd != NULL)
		SendMessage(WM_NCACTIVATE, bOn);
}

/////////////////////////////////////////////////////////////////////////////
// Diagnostics

#ifdef _DEBUG
void CItemWnd::AssertValid() const
{
	ASSERT(m_pContainer != NULL);
}
#endif

/////////////////////////////////////////////////////////////////////////////
