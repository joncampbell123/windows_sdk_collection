// DlgSQL.cpp : implementation file
//

#include "stdafx.h"
#include "DaoView.h"
#include "DlgSQL.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSQL dialog


CDlgSQL::CDlgSQL(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSQL::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSQL)
	m_strName = _T("");
	m_strSQL = _T("");
	//}}AFX_DATA_INIT
	m_bEditMode = FALSE;
}


void CDlgSQL::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSQL)
	DDX_Text(pDX, IDC_EDITNAME, m_strName);
	DDV_MaxChars(pDX, m_strName, 45);
	DDX_Text(pDX, IDC_EDITSQL, m_strSQL);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSQL, CDialog)
	//{{AFX_MSG_MAP(CDlgSQL)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSQL message handlers

BOOL CDlgSQL::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (m_bEditMode)
	{
		((CEdit *) GetDlgItem(IDC_EDITNAME))->SetReadOnly();
		((CEdit *) GetDlgItem(IDC_EDITSQL))->SetFocus();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
