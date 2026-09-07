// RenameDl.cpp : implementation file
//

#include "stdafx.h"
#include "DaoView.h"
#include "RenameDl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRenameDlg dialog


CRenameDlg::CRenameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRenameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRenameDlg)
	m_strName = _T("");
	//}}AFX_DATA_INIT
}


void CRenameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRenameDlg)
	DDX_Text(pDX, IDC_ITEMNAME, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRenameDlg, CDialog)
	//{{AFX_MSG_MAP(CRenameDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRenameDlg message handlers
