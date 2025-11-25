// cdlgsp.cpp : implementation file
//

#include "stdafx.h"
#include "dptest.h"
#include "cdlgsp.h"
#include "dplay.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// cdlgsp dialog


cdlgsp::cdlgsp(CWnd* pParent /*=NULL*/)
    : CDialog(cdlgsp::IDD, pParent)
{
    //{{AFX_DATA_INIT(cdlgsp)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


void cdlgsp::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(cdlgsp)
    DDX_Control(pDX, IDC_LIST1, m_lboxconnect);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(cdlgsp, CDialog)
    //{{AFX_MSG_MAP(cdlgsp)
    ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// cdlgsp message handlers
extern IDirectPlay *pIDP;

void cdlgsp::OnDblclkList() 
{
    // TODO: Add your control notification handler code here
    int iIndex;

    iIndex = m_lboxconnect.GetCurSel();
    if (iIndex != LB_ERR)
    {
        DirectPlayCreate((LPGUID) m_lboxconnect.GetItemData(iIndex), &pIDP, NULL);
        EndDialog(0);
    }
    
}


BOOL FAR PASCAL EnumSP(LPGUID lpGuid, LPSTR lpDesc, DWORD dwMajorVer, DWORD dwMinorVer, LPVOID lpv)
{
    CListBox *clb = (CListBox *) lpv;
    int iIndex;

    iIndex = clb->AddString(lpDesc);
    clb->SetItemData(iIndex, (DWORD) lpGuid);

    return(TRUE);
}

BOOL cdlgsp::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    // TODO: Add extra initialization here

        DirectPlayEnumerate(EnumSP, (LPVOID) &m_lboxconnect);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}
