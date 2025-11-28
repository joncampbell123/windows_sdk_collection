// scribdoc.cpp : implementation of the CScribDoc class
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

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScribDoc

IMPLEMENT_DYNCREATE(CScribDoc, CDocument)

BEGIN_MESSAGE_MAP(CScribDoc, CDocument)
	//{{AFX_MSG_MAP(CScribDoc)
	ON_COMMAND(ID_EDIT_CLEAR_ALL, OnEditClearAll)
	ON_COMMAND(ID_PEN_THICK_OR_THIN, OnPenThickOrThin)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR_ALL, OnUpdateEditClearAll)
	ON_UPDATE_COMMAND_UI(ID_PEN_THICK_OR_THIN, OnUpdatePenThickOrThin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScribDoc construction/destruction

CScribDoc::CScribDoc()
{
	// TODO: add one-time construction code here
}

CScribDoc::~CScribDoc()
{
}

BOOL CScribDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	InitDocument();
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CScribDoc serialization

void CScribDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
	m_strokeList.Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CScribDoc diagnostics

#ifdef _DEBUG
void CScribDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CScribDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CScribDoc commands

BOOL CScribDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	InitDocument();
	return TRUE;
}

void CScribDoc::DeleteContents()
{
	while (!m_strokeList.IsEmpty())
	{
		delete m_strokeList.RemoveHead();
	}
	CDocument::DeleteContents();
}

void CScribDoc::InitDocument()
{
	m_bThickPen = FALSE;
	m_nThinWidth = 2;   // default thin pen is 2 pixels wide
	m_nThickWidth = 5;  // default thick pen is 5 pixels wide
	ReplacePen();       // initialze pen according to current width
}

CStroke* CScribDoc::NewStroke()
{
	CStroke* pStrokeItem = new CStroke(m_nPenWidth);
	m_strokeList.AddTail(pStrokeItem);
	SetModifiedFlag();  // Mark the document as having been modified, for
						// purposes of confirming File Close.
	return pStrokeItem;
}




/////////////////////////////////////////////////////////////////////////////
// CStroke

IMPLEMENT_SERIAL(CStroke, CObject, 1)
CStroke::CStroke()
{
	// This empty constructor should be used by serialization only
}

CStroke::CStroke(UINT nPenWidth)
{
	m_nPenWidth = nPenWidth;
}

void CStroke::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << (WORD)m_nPenWidth;
		m_pointArray.Serialize(ar);
	}
	else
	{
		WORD w;
		ar >> w;
		m_nPenWidth = w;
		m_pointArray.Serialize(ar);
	}
}

BOOL CStroke::DrawStroke(CDC* pDC)
{
	CPen penStroke;
	if (!penStroke.CreatePen(PS_SOLID, m_nPenWidth, RGB(0,0,0)))
		return FALSE;
	CPen* pOldPen = pDC->SelectObject(&penStroke);
	pDC->MoveTo(m_pointArray[0]);
	for (int i=1; i < m_pointArray.GetSize(); i++)
	{
		pDC->LineTo(m_pointArray[i]);
	}

	pDC->SelectObject(pOldPen);
	return TRUE;
}

void CScribDoc::OnEditClearAll()
{
	DeleteContents();
	SetModifiedFlag();  // Mark the document as having been modified, for
						// purposes of confirming File Close.
	UpdateAllViews(NULL);
}

void CScribDoc::OnPenThickOrThin()
{
	// Toggle the state of the pen between thin or thick.
	m_bThickPen = !m_bThickPen;

	// Change the current pen to reflect the new user-specified width.
	ReplacePen();
}


void CScribDoc::ReplacePen()
{
	m_nPenWidth = m_bThickPen? m_nThickWidth : m_nThinWidth;

	// Change the current pen to reflect the new user-specified width.
	m_penCur.DeleteObject();
	m_penCur.CreatePen(PS_SOLID, m_nPenWidth, RGB(0,0,0)); // solid black
}

void CScribDoc::OnUpdateEditClearAll(CCmdUI* pCmdUI)
{
	// Enable the command user interface object (menu item or tool bar
	// button) if the document is non-empty, i.e., has at least one stroke.
	pCmdUI->Enable(!m_strokeList.IsEmpty());
}


void CScribDoc::OnUpdatePenThickOrThin(CCmdUI* pCmdUI)
{
	// Add check mark to Draw Thick Line menu item, if the current
	// pen width is "thick".
	pCmdUI->SetCheck(m_bThickPen);
}


