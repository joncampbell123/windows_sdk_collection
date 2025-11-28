// palette.cpp : implementation of the Floating tool palette class
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"

#include "palette.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define CYCAPTION 9     /* height of the caption */

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

BEGIN_MESSAGE_MAP(CPaletteBar, CToolBar)
	//{{AFX_MSG_MAP(CPaletteBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaletteBar construction/destruction

CPaletteBar::CPaletteBar()
{
	m_nColumns = 2;
	m_cxLeftBorder = 5;
	m_cxRightBorder = 5;
	m_cyTopBorder = 5;
	m_cyBottomBorder = 5;
}

CPaletteBar::~CPaletteBar()
{
}

/////////////////////////////////////////////////////////////////////////////
// CPaletteBar diagnostics

#ifdef _DEBUG
void CPaletteBar::AssertValid() const
{
	CToolBar::AssertValid();
}

void CPaletteBar::Dump(CDumpContext& dc) const
{
	CToolBar::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPaletteBar message handlers

void CPaletteBar::DoPaint(CDC* pDC)
{
	CControlBar::DoPaint(pDC);      // draws any borders

	BOOL bHorz = ((m_dwStyle & CBRS_ORIENT_HORZ)==TRUE);
	CRect rect;

	GetClientRect(rect);
	CalcInsideRect(rect, bHorz);
	int Startx = rect.left;

	rect.bottom = rect.top + m_sizeButton.cy;
	rect.right = rect.left + m_sizeButton.cx;

	// We need to initialize the bitmap selection process.
	DrawState ds;
	if (!PrepareDrawButton(ds))
		return;     // something went wrong

	// Now draw each visible button
	for (int iButton = 0; iButton < m_nCount; )
	{
		rect.left = Startx;
		for (UINT nCol = 0; nCol < m_nColumns; nCol++, iButton++)
		{
			rect.right = rect.left + m_sizeButton.cx;
			UINT nID, nStyle;
			int iImage;
			GetButtonInfo(iButton, nID, nStyle, iImage);
			DrawButton(pDC, rect.left, rect.top, iImage, nStyle);
			rect.left = rect.right - m_cxSharedBorder;  // overlap
		}
		rect.top = rect.bottom - m_cySharedBorder;
		rect.bottom = rect.top + m_sizeButton.cy;
	}

	EndDrawButton(ds);
}

void CPaletteBar::SetColumns(UINT nColumns)
{
	m_nColumns=nColumns;
	Invalidate();
	GetParentFrame()->RecalcLayout();
}

CSize CPaletteBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	int nCount = GetCount();
	CRect rect;
	rect.SetRectEmpty();
	CalcInsideRect(rect,bHorz);

	CSize size;
	size.cx =(m_sizeButton.cx - m_cxSharedBorder) *
		m_nColumns + m_cxSharedBorder - rect.Width();
	size.cy =(m_sizeButton.cy - m_cySharedBorder) *
		((nCount + m_nColumns - 1) / m_nColumns) +
		m_cySharedBorder - rect.Height();

	size.cx = (bStretch && bHorz ? 32767 : size.cx);
	size.cy = (bStretch && !bHorz ? 32767 : size.cy);
	return size;
}

void CPaletteBar::GetItemRect(int nIndex, LPRECT lpRect) const
{
	ASSERT(nIndex >= 0 && nIndex < m_nCount);
	ASSERT(AfxIsValidAddress(lpRect, sizeof(RECT)));

	CRect rect;
	BOOL bHorz = ((m_dwStyle & CBRS_ORIENT_HORZ)==TRUE);

	GetClientRect(rect);
	CalcInsideRect(rect, bHorz);

	lpRect->left = rect.left +
		(nIndex - (nIndex / m_nColumns) * m_nColumns) *
		(m_sizeButton.cx-m_cxSharedBorder);
	lpRect->right = lpRect->left + m_sizeButton.cx;

	lpRect->top = rect.top + (nIndex / m_nColumns) *
		(m_sizeButton.cy-m_cySharedBorder);
	lpRect->bottom = lpRect->top + m_sizeButton.cy;
}

int CPaletteBar::HitTest(CPoint point)  // in window relative coords
{
	CRect rect;
	BOOL bHorz = ((m_dwStyle & CBRS_ORIENT_HORZ)==TRUE);

	GetClientRect(rect);
	CalcInsideRect(rect, bHorz);

	if (point.x < rect.left || point.x >=
			(int)(rect.left + (((m_sizeButton.cx - m_cxSharedBorder) *
			m_nColumns) + m_cxSharedBorder)))
		return -1;      // no X hit

	UINT nRows = (m_nCount + m_nColumns - 1) / m_nColumns;
	if (point.y < rect.top || point.y >=
			(int)(rect.top + (((m_sizeButton.cy - m_cySharedBorder) * nRows) +
			m_cySharedBorder)))
		return -1;      // no Y hit

	int iButton = ((point.y - rect.top) / (m_sizeButton.cy-m_cySharedBorder) *
		m_nColumns + (point.x - rect.left) /
		(m_sizeButton.cx-m_cxSharedBorder));
	return ( iButton < m_nCount ) ? iButton : -1;
}
