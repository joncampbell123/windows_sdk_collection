// preview.cpp : implementation of the CModalShapePropSheet class
//

#include "stdafx.h"
#include "preview.h"
#include "resource.h"
#include "colorpge.h"
#include "stylepge.h"
#include "shapeobj.h"
#include "propsht.h"

BEGIN_MESSAGE_MAP(CShapePreviewWnd, CWnd)
//{{AFX_MSG_MAP(CShapePreviewWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CShapePreviewWnd::CShapePreviewWnd()
{
}

void CShapePreviewWnd::OnPaint()
{
	CPaintDC dc(this);
	CModalShapePropSheet* pShapePropSheet = (CModalShapePropSheet*)GetParent();
	ASSERT(pShapePropSheet->IsKindOf(RUNTIME_CLASS(CModalShapePropSheet)));
	CRect rect;
	GetClientRect(rect);
	CShape shape(
		(SHAPE_COLOR_ENUM)pShapePropSheet->m_colorPage.m_nColor,
		(SHAPE_STYLE)pShapePropSheet->m_stylePage.m_nShapeStyle,
		rect);
	shape.Draw(&dc, FALSE);
}

BOOL CShapePreviewWnd::OnEraseBkgnd(CDC* pDC)
{
	// Use the same background color as that of the dialog
	//  (property sheet).

	CWnd* pParentWnd = GetParent();
	HBRUSH hBrush = (HBRUSH)pParentWnd->SendMessage(WM_CTLCOLORDLG,
		(WPARAM)pDC->m_hDC, (LPARAM)pParentWnd->m_hWnd);
	CRect rect;
	GetClientRect(rect);
	pDC->FillRect(&rect, CBrush::FromHandle(hBrush));
	return TRUE;
}
