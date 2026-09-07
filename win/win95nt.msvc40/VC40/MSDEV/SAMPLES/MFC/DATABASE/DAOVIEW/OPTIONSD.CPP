// OptionsD.cpp : implementation file
//

#include "stdafx.h"
#include "DaoView.h"
#include "OptionsD.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog


COptionsDlg::COptionsDlg(UINT nIDCaption /*=0*/)
	: CPropertyPage(COptionsDlg::IDD, nIDCaption)
{
	//{{AFX_DATA_INIT(COptionsDlg)
	m_bShowSystemObjects = FALSE;
	m_bShowWarnings = FALSE;
	m_bOpenODBC = FALSE;
	m_nMaxRecords = 0;
	//}}AFX_DATA_INIT
}


void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDlg)
	DDX_Check(pDX, IDC_BSYSTEMOBJECTS, m_bShowSystemObjects);
	DDX_Check(pDX, IDC_BWARNINGS, m_bShowWarnings);
	DDX_Check(pDX, IDC_BOPENODBC, m_bOpenODBC);
	DDX_Text(pDX, IDC_MAXRECS, m_nMaxRecords);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDlg, CPropertyPage)
	//{{AFX_MSG_MAP(COptionsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg message handlers

