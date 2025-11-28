// scribvw.cpp : implementation of the CScribView class
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
#include "scribble.h"

#include "scribdoc.h"
#include "scribvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScribView

IMPLEMENT_DYNCREATE(CScribView, CView)


BEGIN_MESSAGE_MAP(CScribView, CView)
	//{{AFX_MSG_MAP(CScribView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP

	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScribView construction/destruction

CScribView::CScribView()
{
	// TODO: add construction code here
}

CScribView::~CScribView()
{
}



/////////////////////////////////////////////////////////////////////////////
// CScribView drawing

void CScribView::OnDraw(CDC* pDC)
{
	CScribDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);


	// The view delegates the drawing of individual strokes to
	// CStroke::DrawStroke().
	CTypedPtrList<CObList,CStroke*>& strokeList = pDoc->m_strokeList;
	POSITION pos = strokeList.GetHeadPosition();
	while (pos != NULL)
	{
		CStroke* pStroke = strokeList.GetNext(pos);
		pStroke->DrawStroke(pDC);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CScribView printing

BOOL CScribView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CScribView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CScribView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


/////////////////////////////////////////////////////////////////////////////
// CScribView diagnostics

#ifdef _DEBUG
void CScribView::AssertValid() const
{
	CView::AssertValid();
}

void CScribView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CScribDoc* CScribView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CScribDoc)));
	return (CScribDoc*) m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CScribView message handlers

void CScribView::OnLButtonDown(UINT, CPoint point)
{  
    // When the user presses the mouse button, she may be
	// starting a new stroke, or selecting or de-selecting a stroke.

	m_pStrokeCur = GetDocument()->NewStroke();
	// Add first point to the new stroke
	m_pStrokeCur->m_pointArray.Add(point);

	SetCapture();       // Capture the mouse until button up.
	m_ptPrev = point;   // Serves as the MoveTo() anchor point for the
						// LineTo() the next point, as the user drags the
						// mouse.

	return;

}


void CScribView::OnLButtonUp(UINT, CPoint point)
{
	// Mouse button up is interesting in the Scribble application
	// only if the user is currently drawing a new stroke by dragging
	// the captured mouse.

	if (GetCapture() != this)
		return; // If this window (view) didn't capture the mouse,
				// then the user isn't drawing in this window.

	CScribDoc* pDoc = GetDocument();

	CClientDC dc(this);

	CPen* pOldPen = dc.SelectObject(pDoc->GetCurrentPen());
	dc.MoveTo(m_ptPrev);
	dc.LineTo(point);
	dc.SelectObject(pOldPen);
	m_pStrokeCur->m_pointArray.Add(point);


	ReleaseCapture();   // Release the mouse capture established at
						// the beginning of the mouse drag.
	return;
}


void CScribView::OnMouseMove(UINT, CPoint point)
{
	// Mouse movement is interesting in the Scribble application
	// only if the user is currently drawing a new stroke by dragging
	// the captured mouse.

	if (GetCapture() != this)
		return; // If this window (view) didn't capture the mouse,
				// then the user isn't drawing in this window.

	CClientDC dc(this);
	m_pStrokeCur->m_pointArray.Add(point);

	// Draw a line from the previous detected point in the mouse
	// drag to the current point.
	CPen* pOldPen = dc.SelectObject(GetDocument()->GetCurrentPen());
	dc.MoveTo(m_ptPrev);
	dc.LineTo(point);
	dc.SelectObject(pOldPen);
	m_ptPrev = point;
	return;
}

