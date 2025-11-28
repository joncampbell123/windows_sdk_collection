// speedlg.cpp : implementation file
//

#include "stdafx.h"
#include "mtrecalc.h"
#include "speeddlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpeedDlg dialog


CSpeedDlg::CSpeedDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpeedDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpeedDlg)
	m_nRecalcSpeedSeconds = 0;
	//}}AFX_DATA_INIT
}

void CSpeedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpeedDlg)
	DDX_Text(pDX, IDC_SPEED, m_nRecalcSpeedSeconds);
	DDV_MinMaxInt(pDX, m_nRecalcSpeedSeconds, 1, 20);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSpeedDlg, CDialog)
	//{{AFX_MSG_MAP(CSpeedDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSpeedDlg message handlers
