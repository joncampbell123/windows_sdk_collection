// cdlggame.cpp : implementation file
//

#include "stdafx.h"
#include "dptest.h"
#include "cdlggame.h"
#include "dplay.h"
#include "cplist.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// cdlggame dialog

extern IDirectPlay *pIDP;


cdlggame::cdlggame(CWnd* pParent /*=NULL*/)
    : CDialog(cdlggame::IDD, pParent)
{
    //{{AFX_DATA_INIT(cdlggame)
    //}}AFX_DATA_INIT
}


void cdlggame::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(cdlggame)
    DDX_Control(pDX, IDC_LB_SESSION, m_lbox);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(cdlggame, CDialog)
    //{{AFX_MSG_MAP(cdlggame)
    ON_LBN_DBLCLK(IDC_LB_SESSION, OnDblclkLbSession)
    ON_BN_CLICKED(IDC_GETPLAYERS, OnGetplayers)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// cdlggame message handlers

extern IDirectPlay *pIDP;
extern bConnected;


BOOL FAR PASCAL EnumSession(LPDPSESSIONDESC lpDPGameDesc, LPVOID pContext, LPDWORD lpdwTimeout, DWORD dwFlags)
{
    CListBox *pList = (CListBox *) pContext;
    char chBuffer[256];
    int  iIndex;

    if (dwFlags & DPESC_TIMEDOUT)
        return(FALSE);

    wsprintf( chBuffer, "Session %d Name %s", lpDPGameDesc->dwSession,
        lpDPGameDesc->szSessionName);
    iIndex = pList->AddString(chBuffer);
    pList->SetItemData(iIndex, lpDPGameDesc->dwSession);


    return(TRUE);
}

extern GUID DPTEST;
void cdlggame::OnDblclkLbSession() 
{
    // TODO: Add your control notification handler code here
    

    int iIndex;

    DPSESSIONDESC dpDesc;

    memset(&dpDesc, 0x00, sizeof(DPSESSIONDESC));
    dpDesc.dwSize = sizeof(dpDesc);
    dpDesc.guidSession = DPTEST;
    dpDesc.dwFlags = DPOPEN_OPENSESSION;

    iIndex = m_lbox.GetCurSel();
    if (iIndex != LB_ERR)
    {
        dpDesc.dwSession = m_lbox.GetItemData(iIndex);
        pIDP->Open(&dpDesc);
        EndDialog(0);
    }
    else
    {
        Beep(2000, 30);
    }
}


BOOL cdlggame::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    // TODO: Add extra initialization here
    
    DPSESSIONDESC dpDesc;

    if (bConnected == FALSE && pIDP)
    {
        memset(&dpDesc, 0x00, sizeof(DPSESSIONDESC));
        dpDesc.dwSize = sizeof(dpDesc);
        dpDesc.guidSession = DPTEST;
        
        pIDP->EnumSessions(&dpDesc, 1500, EnumSession, (LPVOID) &m_lbox, m_dwFlags);
    }



    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void cdlggame::OnOK() 
{
    OnDblclkLbSession();
}

void cdlggame::OnCancel() 
{
    CDialog::OnCancel();
}

void cdlggame::OnGetplayers() 
{
    int iIndex;
    DWORD dwSession;

    iIndex = m_lbox.GetCurSel();
    if (iIndex != LB_ERR)
    {
        dwSession = m_lbox.GetItemData(iIndex);

        cplist dlg;
        dlg.m_dwSession = dwSession;
        dlg.DoModal();

    }
    else
    {
        Beep(2000, 30);
    }


}
