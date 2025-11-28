// stylepge.cpp : implementation file
//

#include "stdafx.h"
#include "propdlg.h"
#include "stylepge.h"
#include "colorpge.h"
#include "shapeobj.h"
#include "preview.h"
#include "propsht.h"
#include "propsht2.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStylePage dialog


CStylePage::CStylePage() : CPropertyPage(CStylePage::IDD)
{
	//{{AFX_DATA_INIT(CStylePage)
	//}}AFX_DATA_INIT
	m_nShapeStyle = rectangle;
}

void CStylePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStylePage)
	DDX_Radio(pDX, IDC_RECTANGLE, m_nShapeStyle);
	//}}AFX_DATA_MAP
}

void CStylePage::OnStyleClicked(UINT /*nCmdID*/)
{
	// The CStylePage property page is used for both the
	// CModalShapePropSheet and the CModelessShapePropSheet.
	// Both these versions of the property sheet share a common
	// feature that they immediately update a shape.  In the
	// case of CModalShapePropSheet, the shape is in the preview
	// window.  In the case of CModelessShapePropSheet, the shape
	// is the currently selected shape in the view.

	CPropertySheet* pPropertySheet = (CPropertySheet*)GetParent();

	if (pPropertySheet->IsKindOf(RUNTIME_CLASS(CModalShapePropSheet)))
	{
		UpdateData();
		((CModalShapePropSheet*)pPropertySheet)->UpdateShapePreview();
		SetModified(); // enable Apply Now button
	}

	if (pPropertySheet->IsKindOf(RUNTIME_CLASS(CModelessShapePropSheet)))
	{
		CMDIFrameWnd* pMDIFrameWnd = (CMDIFrameWnd*)AfxGetMainWnd();
		ASSERT(pMDIFrameWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)));
		CView* pView = pMDIFrameWnd->MDIGetActive()->GetActiveView();
		UpdateData();
		pView->SendMessage(WM_USER_CHANGE_OBJECT_PROPERTIES, 0, 0);
	}
}


BEGIN_MESSAGE_MAP(CStylePage, CPropertyPage)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RECTANGLE, IDC_ELLIPSE, OnStyleClicked)

	//{{AFX_MSG_MAP(CStylePage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CStylePage message handlers
