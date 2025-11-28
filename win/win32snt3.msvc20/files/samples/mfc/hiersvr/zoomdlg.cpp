// zoomdlg.cpp : implementation file
//

#include "stdafx.h"
#include "hiersvr.h"
#include "zoomdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CZoomDlg dialog

CZoomDlg::CZoomDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CZoomDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CZoomDlg)
	m_zoomX = 0;
	m_zoomY = 0;
	//}}AFX_DATA_INIT
}

void CZoomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CZoomDlg)
	DDX_Text(pDX, IDC_EDIT1, m_zoomX);
	DDV_MinMaxInt(pDX, m_zoomX, 10, 500);
	DDX_Text(pDX, IDC_EDIT2, m_zoomY);
	DDV_MinMaxInt(pDX, m_zoomY, 10, 500);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CZoomDlg, CDialog)
	//{{AFX_MSG_MAP(CZoomDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZoomDlg message handlers
