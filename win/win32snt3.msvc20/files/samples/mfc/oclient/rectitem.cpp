// rectitem.cpp : implementation of the CRectItem class
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp and/or WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


#include "stdafx.h"
#include <afxpriv.h>
#include "oclient.h"

#include "maindoc.h"
#include "mainview.h"
#include "rectitem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CRectItem, COleClientItem, 0)

CRectItem::CRectItem(COleDocument* pContainer) : COleClientItem(pContainer)
{
	m_extent.cx = 0;
	m_extent.cy = 0;
	m_rect.SetRect(10, -10, 10, -10);
}

CRectItem::CRectItem()
{
	m_extent.cx = 0;
	m_extent.cy = 0;
	m_rect.SetRect(10, -10, 10, -10);
}

CRectItem::~CRectItem()
{
}

void CRectItem::Invalidate(CView* pNotThisView)
{
	GetDocument()->UpdateAllViews(pNotThisView, 0, this);
}

/////////////////////////////////////////////////////////////////////////////

BOOL CRectItem::UpdateExtent()
{
	// get size in pixels
	CSize size;
	if (!GetCachedExtent(&size))
		return FALSE;       // blank
	Invalidate();   // invalidate the old size/position
	if (size == m_extent)
		return FALSE;
	// if new object (i.e. m_extent is empty) setup position
	if (m_extent == CSize(0,0))
	{
		m_rect.right = m_rect.left + MulDiv(size.cx, 10, 254);
		m_rect.bottom = m_rect.top - MulDiv(size.cy, 10, 254);
	}
	else
	{
		if (!IsInPlaceActive() && size != m_extent)
		{
			// data changed and not inplace, so scale up rect as well
			m_rect.right = m_rect.left +
				MulDiv(m_rect.Width(), size.cx, m_extent.cx);
			m_rect.bottom = m_rect.top -
				MulDiv(-m_rect.Height(), size.cy, m_extent.cy);
		}
	}
	m_extent = size;
	Invalidate();   // as well as the new size/position
	return TRUE;
}

BOOL CRectItem::OnChangeItemPosition(const CRect& rectPos)
{
	CMainView* pView = GetActiveView();
	ASSERT_VALID(pView);

	CRect rc = rectPos;
	pView->ClientToDoc(rc);

	if (rc != m_rect)
	{
		// invalidate old item
		Invalidate();
		// update to new rectangle
		m_rect = rc;

		GetDocument()->SetModifiedFlag();
		GetCachedExtent(&m_extent);

		// and invalidate new
		Invalidate();
	}
	return COleClientItem::OnChangeItemPosition(rectPos);
}

void CRectItem::OnActivate()
{
	// allow only one item to be in place active at a time in a frame
	CMainView* pView = GetActiveView();
	ASSERT_VALID(pView);
	COleClientItem* pItem = GetDocument()->GetInPlaceActiveItem(pView);
	if (pItem != NULL && pItem != this)
		pItem->Close();

	COleClientItem::OnActivate();

	// set selection to an item when it becomes active
	pView->SetSelection(this);
}

void CRectItem::OnDeactivateUI(BOOL bUndoable)
{
	COleClientItem::OnDeactivateUI(bUndoable);

	// Close an in-place active item whenever it removes the user
	//  interface.  The action here should match as closely as possible
	//  to the handling of the escape key in the view.

	Deactivate();   // nothing fancy here -- just deactivate the object
}

void CRectItem::OnChange(OLE_NOTIFICATION nCode, DWORD dwParam)
{
	COleClientItem::OnChange(nCode, dwParam);
	switch(nCode)
	{
		case OLE_CHANGED:
			UpdateExtent();
			Invalidate();
			break;
		case OLE_CHANGED_ASPECT:
		case OLE_CHANGED_STATE:
			Invalidate();
			break;
	}
}

void CRectItem::OnGetItemPosition(CRect& rPosition)
{
	ASSERT_VALID(this);

	if (m_rect.Size() == CSize(0,0))
		UpdateExtent();

	// copy m_rect, which is in document coordinates
	rPosition = m_rect;
	CMainView* pView = GetActiveView();
	ASSERT_VALID(pView);
	pView->DocToClient(rPosition);
}

void CRectItem::Move(CRect &rc)
{
	// invalidate old rect
	Invalidate();
	// invalidate new
	m_rect = rc;
	Invalidate();

	// update item rect when in-place active
	if (IsInPlaceActive())
		SetItemRects();
}

void CRectItem::Serialize(CArchive& ar)
{
	CRect rect;

	// IMPORTANT: when using "easy" serialize -- call base class FIRST!
	//  (not strictly necessary, but a good idea)
	COleClientItem::Serialize(ar);

	// now store/retrieve data specific to CRectItem
	if (ar.IsStoring())
	{
		WORD w = 0x5500;        // magic value
		ar << w << m_rect << m_extent;
	}
	else
	{
		WORD w;
		ar >> w >> m_rect >> m_extent;
		if (w != 0x5500)
		{
			TRACE0("Bad magic number in front of an item wnd\n");
			AfxThrowArchiveException(CArchiveException::generic);
		}
	}
}

void CRectItem::ResetSize()
{
	ASSERT_VALID(this);
	Invalidate();
	m_extent = CSize(0, 0);
	UpdateExtent();
}

// OnGetClipboardData is used by CopyToClipboard and DoDragDrop
COleDataSource* CRectItem::OnGetClipboardData(
	BOOL bIncludeLink, LPPOINT lpOffset, LPSIZE lpSize)
{
	ASSERT_VALID(this);

	COleDataSource* pDataSource = new COleDataSource;
	TRY
	{
		GetNativeClipboardData(pDataSource);
		GetClipboardData(pDataSource, bIncludeLink, lpOffset, lpSize);
	}
	CATCH_ALL(e)
	{
		delete pDataSource;
		THROW_LAST();
	}
	END_CATCH_ALL

	ASSERT_VALID(pDataSource);
	return pDataSource;
}

void CRectItem::GetNativeClipboardData(COleDataSource* pDataSource)
{
	ASSERT_VALID(this);
	ASSERT_VALID(GetDocument());

	// Create a shared file and associate a CArchive with it
	CSharedFile file;
	CArchive ar(&file, CArchive::store);

	// Serialize selected objects to the archive
	Serialize(ar);
	ar.Close();
	pDataSource->CacheGlobalData(CMainDoc::m_cfPrivate, file.Detach());
}

/////////////////////////////////////////////////////////////////////////////
