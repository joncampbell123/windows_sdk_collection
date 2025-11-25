// cplist.cpp : implementation file
//

#include "stdafx.h"
#include "dptest.h"
#include "cplist.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// cplist dialog

extern IDirectPlay *pIDP;

cplist::cplist(CWnd* pParent /*=NULL*/)
    : CDialog(cplist::IDD, pParent)
{
    //{{AFX_DATA_INIT(cplist)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


void cplist::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(cplist)
    DDX_Control(pDX, IDC_PLAYERLIST, m_pList);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(cplist, CDialog)
    //{{AFX_MSG_MAP(cplist)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// cplist message handlers

void cplist::OnCancel() 
{
    // TODO: Add extra cleanup here
    
    CDialog::OnCancel();
}

BOOL FAR PASCAL EnumPlayers2(DPID dpId, LPSTR lpShort, LPSTR lpLong, DWORD dwFlags,
            LPVOID pContext)
{
    char chBuffer[128];
    CListBox *pList = (CListBox *) pContext;

    if (pIDP && pList)
    {
        wsprintf( chBuffer, "Player %d %s(%s) Flags %8x",
            dpId, lpLong, lpShort, dwFlags);
        pList->AddString(chBuffer);
    }
    return(TRUE);

}

BOOL cplist::OnInitDialog() 
{
    CDialog::OnInitDialog();

    if (pIDP)
    {
        pIDP->EnumPlayers(m_dwSession, EnumPlayers2, (LPVOID) &m_pList, DPENUMPLAYERS_SESSION);
    }
    
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}
