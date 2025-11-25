//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;
//
// playvw.cpp : implementation of the CPlayerView class
//

#include "stdafx.h"
#include "mfcplay.h"

#include "mfcdoc.h"
#include "mfcvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlayerView

IMPLEMENT_DYNCREATE(CPlayerView, CView)

BEGIN_MESSAGE_MAP(CPlayerView, CView)
	//{{AFX_MSG_MAP(CPlayerView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlayerView construction/destruction

CPlayerView::CPlayerView()
{
	// TODO: add construction code here

}

CPlayerView::~CPlayerView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CPlayerView drawing

void CPlayerView::OnDraw(CDC* pDC)
{
	CPlayerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CPlayerView diagnostics

#ifdef _DEBUG
void CPlayerView::AssertValid() const
{
	CView::AssertValid();
}

void CPlayerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPlayerDoc* CPlayerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPlayerDoc)));
	return (CPlayerDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlayerView message handlers
