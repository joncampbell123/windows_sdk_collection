// shapesht.cpp : implementation of the CModelessShapePropSheet class
//

#include "stdafx.h"
#include "propdlg.h"
#include "resource.h"
#include "shapeobj.h"
#include "colorpge.h"
#include "stylepge.h"
#include "propsht2.h"

IMPLEMENT_DYNAMIC(CModelessShapePropSheet, CPropertySheet)

BEGIN_MESSAGE_MAP(CModelessShapePropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CModelessShapePropSheet)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CModelessShapePropSheet::CModelessShapePropSheet(CWnd* pWndParent)
	: CPropertySheet(AFX_IDS_APP_TITLE, pWndParent)
{
	AddPage(&m_stylePage);
	AddPage(&m_colorPage);
}

int CModelessShapePropSheet::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectWnd;
	GetWindowRect(rectWnd);
	SetWindowPos(NULL, 0, 0,
		rectWnd.Width(), rectWnd.Height()  - m_sizeButton.cy,
		SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

	return 0;
}

void CModelessShapePropSheet::PostNcDestroy()
{
	CPropertySheet::PostNcDestroy();
	delete this;
}

void CModelessShapePropSheet::SetSheetPropsFromShape(CShape* pShape)
{
	m_stylePage.m_nShapeStyle = pShape->m_shapestyle;
	m_stylePage.SetModified(FALSE);

	m_colorPage.m_nColor = pShape->m_shapecolor;
	m_colorPage.SetModified(FALSE);

	// Reflect the new shape properties in the controls of the
	// currently active property page.
	GetActivePage()->UpdateData(FALSE);
}

void CModelessShapePropSheet::SetShapePropsFromSheet(CShape* pShape)
{
	pShape->m_shapecolor = m_colorPage.m_nColor;
	pShape->m_shapestyle = (SHAPE_STYLE)m_stylePage.m_nShapeStyle;

	m_colorPage.SetModified(FALSE);
	m_stylePage.SetModified(FALSE);
}
