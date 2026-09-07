// TablePage.cpp : implementation file
//

#include "stdafx.h"
#include "catalog2.h"
#include "TabPage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTablePage property page

IMPLEMENT_DYNCREATE(CTablePage, CPropertyPage)

CTablePage::CTablePage() : CPropertyPage(CTablePage::IDD)
{
	//{{AFX_DATA_INIT(CTablePage)
	m_bSynonyms = FALSE;
	m_bSystemTables = FALSE;
	m_bViews = FALSE;
	//}}AFX_DATA_INIT
}

CTablePage::~CTablePage()
{
}

void CTablePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTablePage)
	DDX_Check(pDX, IDC_ALIAI, m_bSynonyms);
	DDX_Check(pDX, IDC_SYSTABLES, m_bSystemTables);
	DDX_Check(pDX, IDC_VIEWS, m_bViews);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTablePage, CPropertyPage)
	//{{AFX_MSG_MAP(CTablePage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTablePage message handlers
