// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// class CItemWnd - the window containing an embedded OLE object

#ifndef __AFXOLE_H__
#include <afxole.h>
#endif

class CItemWnd;

// A special COleClientItem that points to the UI for it
class CEmbeddedItem : public COleClientItem
	// embedded or linked
{
	CItemWnd*   m_pView;        // view on this item

public:
	CEmbeddedItem(COleClientDoc* pContainer, CItemWnd* pView)
		: COleClientItem(pContainer)
		{ m_pView = pView; }

// Operations
	void    SetNames();

// Callbacks
protected:
	virtual void OnChange(OLE_NOTIFICATION wNotification);
	virtual void WaitForServer();   // special hourglass
};


// Special class for tieing an OLEOBJECT and a WINDOW
class CItemWnd : public CWnd
{
public:
	CItemWnd(CMainWnd* pContainer);

	BOOL    CreateItemWindow(BOOL fShow);
	BOOL    RestoreItemWindow(const RECT& rect);

// Attributes
	BOOL IsComplete()       // BLANK objects are incomplete 
			{ return m_fVisible; }
	BOOL CanChangeBounds()
			{ return (!m_fVisible || m_fTrackSize); }
	CEmbeddedItem*  GetEmbedded()
				{ return &m_embedded; }


// Operations
	void Dirty()
			{ m_pContainer->Dirty(); }

	void DoVerb(UINT nVerb);
	void SetInitialBounds(const CRect& rect);
	void Select(BOOL bOn);

// Callbacks - for Window part
protected:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void PostNcDestroy();     // for cleanup

// Implementation
public:
	virtual void Serialize(CArchive& ar);       // from CObject
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif

protected:
	CMainWnd* m_pContainer;     // our parent window/container
	BOOL    m_fVisible;       // is item to be displayed ?
	BOOL    m_fTrackSize;     // is item's size autoupdate ?

	// Capture/dragging support
	BOOL    m_fCaptured;
	static CRect dragRect;
	static CPoint dragPt;

// Item bound to item window
	CEmbeddedItem   m_embedded;
	void OnCommonChange(BOOL fDestroyOnError);

	DECLARE_MESSAGE_MAP()

	friend class CEmbeddedItem;
};


