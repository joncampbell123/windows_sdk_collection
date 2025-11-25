// dptesdlg.cpp : implementation file
//

#include "stdafx.h"
#include "dptest.h"
#include "dptesdlg.h"
#include "dplay.h"
#include "mmsystem.h"
#include "cdlgsp.h"
#include "cdlggame.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#pragma pack(push, 1)
typedef struct
{
    USHORT  usType;
    DWORD   dwTicks;
    char    chText[128];
} TEST_MSG;
#pragma pack(pop, 1)

#define TEXTMSG        0x0001
#define BANDWIDTH   0x0002
#define LISTEN      0x0003
#define XMIT        0x0004
#define EXIT        0x0005

/////////////////////////////////////////////////////////////////////////////
// CDptestDlg dialog


IDirectPlay *pIDP = NULL;
BOOL        bServer = FALSE;


CDptestDlg::CDptestDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CDptestDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CDptestDlg)
    m_csMsg = _T("Hi");
    m_csFrom = _T("");
    m_csTo = _T("");
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDptestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDptestDlg)
    DDX_Control(pDX, IDC_RESULTS, m_cLBoxResults);
    DDX_Text(pDX, IDC_EDIT, m_csMsg);
    DDV_MaxChars(pDX, m_csMsg, 60);
    DDX_Text(pDX, IDC_EDIT_FROM, m_csFrom);
    DDV_MaxChars(pDX, m_csFrom, 3);
    DDX_Text(pDX, IDC_EDIT_TO, m_csTo);
    DDV_MaxChars(pDX, m_csTo, 3);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDptestDlg, CDialog)
    //{{AFX_MSG_MAP(CDptestDlg)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_CONNECT, OnConnect)
    ON_BN_CLICKED(IDC_CREATE, OnCreate)
    ON_BN_CLICKED(IDC_CREATEPLAYER, OnCreateplayer)
    ON_BN_CLICKED(IDC_GETMESSAGE, OnGetmessage)
    ON_BN_CLICKED(IDC_SENDMESSAGE, OnSendmessage)
    ON_BN_CLICKED(IDC_SAVESESSION, OnSavesession)
    ON_BN_CLICKED(IDC_OPENSESSION, OnOpensession)
    ON_BN_CLICKED(IDC_BANDWIDTH, OnBandwidth)
    ON_BN_CLICKED(IDC_CLOSE, OnClose)
    ON_BN_CLICKED(IDC_DESTROYPLAYER, OnDestroyplayer)
    ON_BN_CLICKED(IDC_ENABLENEWPLAYERS, OnEnablenewplayers)
    ON_BN_CLICKED(IDC_ENUMPLAYERS, OnEnumplayers)
    ON_BN_CLICKED(IDC_GETCAPS, OnGetcaps)
    ON_BN_CLICKED(IDC_GETPLAYERCAPS, OnGetplayercaps)
    ON_BN_CLICKED(IDC_GETPLAYERNAME, OnGetplayername)
    ON_BN_CLICKED(IDC_SETPLAYERNAME, OnSetplayername)
    ON_BN_CLICKED(IDC_ADDPLAYERTOGRP, OnAddplayertogrp)
    ON_BN_CLICKED(IDC_CREATEGROUP, OnCreategroup)
    ON_BN_CLICKED(IDC_DELPLAYERFROMGRP, OnDelplayerfromgrp)
    ON_BN_CLICKED(IDC_DESTROYGROUP, OnDestroygroup)
    ON_BN_CLICKED(IDC_ENUMGRP, OnEnumgrp)
    ON_BN_CLICKED(IDC_CLEAR, OnClear)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDptestDlg message handlers


GUID DPLAY_NETWORK_TCP = { /* 8cab4650-b1b6-11ce-920c-00aa006c4972 */
    0x8cab4650,
    0xb1b6,
    0x11ce,
    {0x92, 0x0c, 0x00, 0xaa, 0x00, 0x6c, 0x49, 0x72}
  };

GUID DPLAY_NETWORK_IPX = { /* 8cab4651-b1b6-11ce-920c-00aa006c4972 */
    0x8cab4651,
    0xb1b6,
    0x11ce,
    {0x92, 0x0c, 0x00, 0xaa, 0x00, 0x6c, 0x49, 0x72}
  };
GUID DPLAY_MODEM = { /* 8cab4652-b1b6-11ce-920c-00aa006c4972 */
    0x8cab4652,
    0xb1b6,
    0x11ce,
    {0x92, 0x0c, 0x00, 0xaa, 0x00, 0x6c, 0x49, 0x72}
  };

GUID DPTEST = { /* 5a55cf10-b841-11ce-9210-00aa006c4972 */
    0x5a55cf10,
    0xb841,
    0x11ce,
    {0x92, 0x10, 0x00, 0xaa, 0x00, 0x6c, 0x49, 0x72}
  };


HANDLE hPlayerEvent[16];

BOOL CDptestDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    CenterWindow();

    DWORD ii;
    for (ii = 0; ii < 16; ii++)
        hPlayerEvent[ii] = NULL;

    cdlgsp dlg;
    dlg.DoModal();

    // TODO: Add extra initialization here
    
    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDptestDlg::OnPaint() 
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDptestDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}

void CDptestDlg::OnConnect() 
{
    cdlggame dlg;
    dlg.SetOpenFlags(0);
    dlg.DoModal();
}

void CDptestDlg::OnCreate() 
{
    HRESULT hr;
    char    chBuffer[128];

    if (pIDP == NULL)
    {
        m_cLBoxResults.AddString("No DirectPlay Object");
        return;
    }


    // TODO: Add your control notification handler code here
    DPSESSIONDESC dpDesc;


    memset(&dpDesc, 0x00, sizeof(DPSESSIONDESC));
    dpDesc.dwSize = sizeof(dpDesc);
    dpDesc.dwMaxPlayers = 8;
    dpDesc.dwFlags = DPOPEN_CREATESESSION;
    dpDesc.guidSession = DPTEST;
    wsprintf( chBuffer, "John's Test %d", GetTickCount() & 0x000000ff);
    strcpy( dpDesc.szSessionName, chBuffer);

    if ((hr = pIDP->Open(&dpDesc)) != DP_OK)
    {
        wsprintf( chBuffer, "Failed Open %8x", hr);
        m_cLBoxResults.AddString(chBuffer);
    }
    else
    {
        m_cLBoxResults.AddString("DirectPlay creation succeeded.");
        m_cLBoxResults.AddString(chBuffer);
        bServer = TRUE;
    }

    
}

void CDptestDlg::OnCreateplayer() 
{
    HRESULT hr;
    DWORD   dwPlayer;
    char    chBuffer[256];
    static  int iNumber = 1;
    HANDLE  hEvent;

    
    if (pIDP == NULL)
    {
        m_cLBoxResults.AddString("No DirectPlay Object");
        return;
    }


    if (bServer)
    {
        wsprintf( chBuffer, "Master %d", iNumber++);
        m_cLBoxResults.AddString(chBuffer);
        hr = pIDP->CreatePlayer(&dwPlayer, chBuffer, "Server Side", &hEvent);
        if (dwPlayer < 16)
            hPlayerEvent[dwPlayer] = hEvent;
        wsprintf( chBuffer, "Event %8x %d", hEvent, dwPlayer);
        m_cLBoxResults.AddString(chBuffer);
    }
    else
    {
        wsprintf( chBuffer, "Slave %d", iNumber++);
        m_cLBoxResults.AddString(chBuffer);
        hr = pIDP->CreatePlayer(&dwPlayer, chBuffer, "Client Side", &hEvent);
        if (dwPlayer < 16)
            hPlayerEvent[dwPlayer] = hEvent;
        wsprintf( chBuffer, "Event %8x %d", hEvent, dwPlayer);
        m_cLBoxResults.AddString(chBuffer);
    }

    if (hr == DP_OK)
    {
        wsprintf( chBuffer, "Create Player(%d) Succeeded", dwPlayer);
    }
    else
    {
        wsprintf( chBuffer, "Create Player Failure (%8x)", hr);
    }
    m_cLBoxResults.AddString(chBuffer);
}

typedef struct
{
    DPID    dpId;
    CDptestDlg *pDlg;
} LISTEN_PROC_PARAM;

DWORD WINAPI ListenProcStart(LPVOID lpv)
{
    LISTEN_PROC_PARAM *pparam =  (LISTEN_PROC_PARAM *) lpv;

    pparam->pDlg->ListenProc((DWORD)pparam->dpId);
    return(0);
}

void CDptestDlg::ListenProc(DWORD dw)
{
    DPID    dpId = dw;
    HANDLE  hEvent = hPlayerEvent[dw];
    HRESULT hr;
    DPID    dpIdFrom, dpIdTo;
    char chBuffer[256];
    TEST_MSG      *ptMsg;
    DWORD   dwLength;


    m_cLBoxResults.AddString("ListenProc Starts for real.");
    wsprintf( chBuffer, "ListenProc This %8x Pid %d\r\n", this, dw);
    OutputDebugString(chBuffer);

    wsprintf( chBuffer, "ListenProc Event %8x\r\n", hEvent);
    OutputDebugString(chBuffer);


    if (hEvent == NULL)
    {
        m_cLBoxResults.AddString("ListenProc doesn't have an event to wait on.");
        return;
    }

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    while(TRUE)
    {
        // wsprintf( chBuffer, "Listen on %8x", hEvent);
        // m_cLBoxResults.AddString(chBuffer);

        if (WaitForSingleObject(hEvent, 15000) == WAIT_TIMEOUT)
        {
            m_cLBoxResults.AddString("ListenProc Timeout, exiting.");
        }
        // m_cLBoxResults.AddString("ListenProc wakes up.");
        dpIdTo = dpId;
        hr = pIDP->Receive(&dpIdFrom, &dpIdTo, DPRECEIVE_TOPLAYER, chBuffer, &dwLength);
        if (hr == DP_OK)
        {
            if (dpId != dpIdTo)
                m_cLBoxResults.AddString("Unexpected 'To' recipiant.");
            else
            {
                ptMsg = (TEST_MSG *) chBuffer;
                switch(ptMsg->usType)
                {
                case TEXTMSG:
                    m_cLBoxResults.AddString("Unexpected TEXTMSG recipiant.");
                    break;
                case LISTEN:
                    m_cLBoxResults.AddString("Unexpected LISTEN recipiant.");
                    break;
                case XMIT:
                    m_cLBoxResults.AddString("Unexpected XMIT recipiant.");
                    break;
                case EXIT:
                    m_cLBoxResults.AddString("Test Stops, exiting.");
                    return;
                case BANDWIDTH:
                    hr = pIDP->Send(dpIdTo, dpIdFrom, 0, (LPVOID) ptMsg, dwLength);
                    // if (FAILED(hr))
                    //     m_cLBoxResults.AddString("Trouble Sending return msg.");
                    // else
                    //     m_cLBoxResults.AddString("Return Bandwidth.");
                    // 
                    break;


                }
            }
        }
        else
        {
            m_cLBoxResults.AddString("Unexpected Error on Receive.");
        }
    }
}



LISTEN_PROC_PARAM param;
void CDptestDlg::OnGetmessage() 
{
    // TODO: Add your control notification handler code here
    DPID    dpIdFrom, dpIdTo;
    char chBuffer[256];
    char chOutput[256];
    DWORD   dwLength = 128;
    HRESULT hr;
    DPMSG_GENERIC *pGeneric;
    TEST_MSG      *ptMsg;

    if (pIDP == NULL)
    {
        m_cLBoxResults.AddString("No DirectPlay Object");
        return;
    }


    hr = pIDP->Receive(&dpIdFrom, &dpIdTo, DPRECEIVE_ALL, chBuffer, &dwLength);
    if (hr == DP_OK)
    {
        if (dpIdFrom != 0)
        {
            ptMsg = (TEST_MSG *) chBuffer;
            wsprintf( chOutput, "Receive user Msg from %d to %d Succeeded:",
                dpIdFrom, dpIdTo);
            m_cLBoxResults.AddString(chOutput);
            {
                switch(ptMsg->usType)
                {
                case TEXTMSG:
                    chBuffer[dwLength] = 0x00;
                    m_cLBoxResults.AddString(ptMsg->chText);
                    break;
                case BANDWIDTH:
                    m_cLBoxResults.AddString("Get Message shouldn't get a BANDWIDTH msg.");
                    break;
                case XMIT:
                    m_cLBoxResults.AddString("Get Message shouldn't get a Transmit msg.");
                    break;
                case EXIT:
                    m_cLBoxResults.AddString("Get Message shouldn't get an Exit msg.");
                    break;
                case LISTEN:
                    {
                        HANDLE            hThread;
                        DWORD             dwID;

                        param.pDlg = this;
                        param.dpId  = dpIdTo;

                        if (hPlayerEvent[dpIdTo] != NULL)
                        {
                            wsprintf( chOutput, "Starting Listen Player %d Event %x\r\n",
                                dpIdTo, hPlayerEvent[dpIdTo]);
                            OutputDebugString(chOutput);
                            wsprintf( chOutput, "This pointer %8x\r\n",
                                this);
                            OutputDebugString(chOutput);
                            m_cLBoxResults.AddString("Got a Listen message.");
                            hThread = CreateThread(NULL, 0, ListenProcStart, (LPVOID) &param,
                                0, &dwID);
                            if (hThread == NULL)
                            {
                                m_cLBoxResults.AddString("Failed to start listen thread.");
                            }
                            else
                            {
                                m_cLBoxResults.AddString("Listen Thread started.");
                                CloseHandle(hThread);
                            }
                            
                            ptMsg->usType = XMIT;
                            wsprintf( chOutput, "Send XMIT from %d to %d", dpIdTo, dpIdFrom);
                            m_cLBoxResults.AddString(chOutput);
                            hr = pIDP->Send(dpIdTo, dpIdFrom, 0, (LPVOID) ptMsg, dwLength);
                            if (SUCCEEDED(hr) && hr != DP_OK)
                            {
                                LONG ll;
                                ll = (LONG) hr;
                                if (ll > 10)
                                {
                                    wsprintf( chOutput, "Backup reached %d, Sleeping.", ll);
                                    m_cLBoxResults.AddString(chOutput);
                                    ll *= 10;
                                    Sleep(ll);
                                }
                            }

                        }
                        else
                        {
                            wsprintf( chOutput, "No Event for Player %d", dpIdTo);
                            m_cLBoxResults.AddString(chBuffer);
                        }

                                            

                        break;
                        
                    }
                }

            }
        }
        else
        {
            pGeneric = (DPMSG_GENERIC *) chBuffer;
            switch(pGeneric->dwType)
            {
            default:
                wsprintf( chOutput, "Receive unknown System Msg %4x", pGeneric->dwType);
                m_cLBoxResults.AddString(chOutput);
                break;
            case DPSYS_CONNECT:
                wsprintf( chOutput, "Sys Msg: CONNECT");
                m_cLBoxResults.AddString(chOutput);
                break;
            case DPSYS_ADDPLAYER:
                {
                DPMSG_ADDPLAYER *pMsg;

                pMsg = (DPMSG_ADDPLAYER *) pGeneric;
                if (pMsg->dwPlayerType)
                    wsprintf( chOutput, "Sys Msg: AddPlayer dpId %d: %s(%s)",
                        pMsg->dpId, pMsg->szLongName, pMsg->szShortName);
                else
                    wsprintf( chOutput, "Sys Msg: AddGroup  dpId %d: %s(%s)",
                        pMsg->dpId, pMsg->szLongName, pMsg->szShortName);

                m_cLBoxResults.AddString(chOutput);
                }
                break;
            case DPSYS_DELETEPLAYER:
                {
                DPMSG_DELETEPLAYER *pMsg;
                pMsg = (DPMSG_DELETEPLAYER *) pGeneric;
                wsprintf( chOutput, "Sys Msg: DeletePlayer %d", pMsg->dpId);
                m_cLBoxResults.AddString(chOutput);
                }
                break;
            case DPSYS_ADDPLAYERTOGROUP:
                {
                DPMSG_GROUPADD  *pMsg;

                pMsg = (DPMSG_GROUPADD *) pGeneric;
                wsprintf( chOutput, "Sys Msg: Add Player %d to Group %d",
                    pMsg->dpIdPlayer, pMsg->dpIdGroup);
                m_cLBoxResults.AddString(chOutput);
                }
                break;
            case DPSYS_DELETEGROUP:
                {
                DPMSG_DELETEPLAYER *pMsg;
                pMsg = (DPMSG_DELETEPLAYER *) pGeneric;
                wsprintf( chOutput, "Sys Msg: DeleteGroup %d", pMsg->dpId);
                m_cLBoxResults.AddString(chOutput);
                }
                break;
            case DPSYS_DELETEPLAYERFROMGRP:
                {
                DPMSG_GROUPADD  *pMsg;

                pMsg = (DPMSG_GROUPADD *) pGeneric;
                wsprintf( chOutput, "Sys Msg: Delete Player %d to Group %d",
                    pMsg->dpIdPlayer, pMsg->dpIdGroup);
                m_cLBoxResults.AddString(chOutput);
                }
                break;
            }

            m_csMsg = _T("");
        }
        UpdateData(FALSE);
    }
    else
    {
        wsprintf( chOutput, "Receive Error %8x", hr);
        m_cLBoxResults.AddString(chOutput);
    }

    
}

void CDptestDlg::OnSendmessage() 
{
    // TODO: Add your control notification handler code here
    DPID    dpIdTo, dpIdFrom;
    DWORD   dwLength;
    HRESULT hr;
    char    chBuffer[128];
    TEST_MSG tMsg;


    if (pIDP == NULL)
    {
        m_cLBoxResults.AddString("No DirectPlay Object");
        return;
    }

    UpdateData(TRUE);
    dwLength = m_csMsg.GetLength();

    if (dwLength)
    {
        dpIdTo   = (DPID) atol(  m_csTo.GetBuffer(2));
        dpIdFrom = (DPID) atol(m_csFrom.GetBuffer(2));

        dwLength + 1;
        tMsg.usType  = TEXTMSG;
        tMsg.dwTicks = 0;
        lstrcpy( tMsg.chText, m_csMsg.GetBuffer(dwLength));

        hr = pIDP->Send(dpIdFrom, dpIdTo, 0, (LPVOID) &tMsg,
            dwLength + 6);
        if (SUCCEEDED(hr))
        {
            if (hr == 0)
                wsprintf( chBuffer, "Send Succeeded.");
            else
                wsprintf( chBuffer, "Send Succeeded with backup %d.", hr);

            m_cLBoxResults.AddString(chBuffer);
        }
        else
        {
            wsprintf( chBuffer, "Send Error %8x", hr);
            m_cLBoxResults.AddString(chBuffer);
        }
    }
}

void CDptestDlg::OnCancel() 
{
    // TODO: Add extra cleanup here
    
    if (pIDP)
    {
        pIDP->Close();
        pIDP->Release();
    }

    CDialog::OnCancel();
}

void CDptestDlg::OnSavesession() 
{
    HRESULT hr;
    char    chBuffer[128];

    if (pIDP)
    {
        hr = pIDP->SaveSession("My Session");
        if (hr == DP_OK)
        {
            m_cLBoxResults.AddString("Session Saved");
        }
        else
        {
            wsprintf( chBuffer, "SaveSession Error %8x", hr);
            m_cLBoxResults.AddString(chBuffer);
        }
    }

}

BOOL bConnected;

void CDptestDlg::OnOpensession() 
{
    cdlggame dlg;
    dlg.SetOpenFlags(DPENUMSESSIONS_PREVIOUS);
    dlg.DoModal();
    
}

int iCalled = 0;
VOID CALLBACK TimerProc(UINT uID, UINT uR1, HANDLE hEvent,
                        DWORD dw2, DWORD dw3)
{
    iCalled++;
    SetEvent(hEvent);
}

void CDptestDlg::OnBandwidth() 
{
    HANDLE hEvent;
    TIMECAPS            timecaps;
    UINT                iId;
    TEST_MSG            tMsg;
    DPID                dpIdTo, dpIdFrom;
    DPID                dpIdTo2, dpIdFrom2;
    DWORD               dwLength;
    DWORD               dwTickTotal;
    DWORD               kk;
    DWORD               dwSent;
    DWORD               dwGot;
    char                chBuffer[256];
    char                chOutput[256];
    TEST_MSG           *ptMsg = (TEST_MSG *) chBuffer;
    HRESULT             hr;
    DWORD               dwStartTickCount;
    HANDLE              aEvents[2];
    BOOL                bGo;


    hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    if (hEvent == NULL)
    {
        return;
    }

    UpdateData(TRUE);

    dpIdTo   = (DPID) atol(  m_csTo.GetBuffer(2));
    dpIdFrom = (DPID) atol(m_csFrom.GetBuffer(2));

    tMsg.usType  = LISTEN;
    tMsg.dwTicks = 0;

    dwLength = 6;
    hr = pIDP->Send(dpIdFrom, dpIdTo, 0, (LPVOID) &tMsg, dwLength);
    if (FAILED(hr))
    {
        m_cLBoxResults.AddString("Send LISTEN in bandwidth test failed.");
        CloseHandle(hEvent);
        return;
    }

    if (WaitForSingleObject(hPlayerEvent[dpIdFrom], 5000) == WAIT_TIMEOUT)
    {
        wsprintf( chOutput, "Wait for XMIT failed.  Event %8x Player %d",
            hPlayerEvent[dpIdFrom], dpIdFrom);
        m_cLBoxResults.AddString(chOutput);
        return;
    }
    dpIdTo2 = dpIdFrom;
    hr = pIDP->Receive(&dpIdFrom2, &dpIdTo2, DPRECEIVE_TOPLAYER, (LPVOID) ptMsg, &dwLength);
    if (hr != DP_OK || ptMsg->usType != XMIT)
    {
        m_cLBoxResults.AddString("Retrieve XMIT didn't work.");
        return;
    }
    // wsprintf( chOutput, "Our Event is %8x Player %d",
    //     hPlayerEvent[dpIdFrom], dpIdFrom);
    // m_cLBoxResults.AddString(chOutput);
                       
    timeGetDevCaps(&timecaps, sizeof(timecaps));

    if (timecaps.wPeriodMin < 4)
        timecaps.wPeriodMin = 4;

    timeBeginPeriod(timecaps.wPeriodMin);


    aEvents[0] = hPlayerEvent[dpIdFrom];
    aEvents[1] = hEvent;

    tMsg.usType  = BANDWIDTH;
    for (kk = 10; kk < 130; kk += 10)
    {
        iCalled = 0;
        dwSent  = 0;
        dwGot   = 0;
        dwTickTotal = 0;
        bGo = TRUE;
        dwStartTickCount = GetTickCount();
        iId = timeSetEvent(1000 / kk, timecaps.wPeriodMin, (LPTIMECALLBACK) TimerProc,
                            (DWORD) hEvent, TIME_PERIODIC);
        while (dwSent < kk * 5 && bGo)
        {
            switch(WaitForMultipleObjects(2, aEvents, FALSE, INFINITE))
            {
            case WAIT_OBJECT_0 + 1:
                {
                    if (dwSent < (25 + dwGot))
                    {
                        dwSent++;
                        tMsg.dwTicks = GetTickCount();
                        hr = pIDP->Send(dpIdFrom, dpIdTo, 0, (LPVOID) &tMsg, dwLength);
                        break;
                    }
                    else
                    {
                        if (WaitForSingleObject(hPlayerEvent[dpIdFrom], 1000) == WAIT_TIMEOUT)
                        {
                            bGo = FALSE;
                            break;
                        }
                        //
                        // Fall Through
                        //
                    }
                }
            case WAIT_OBJECT_0:
                {
                    dpIdTo2 = dpIdFrom;
                    hr = pIDP->Receive(&dpIdFrom2, &dpIdTo2, DPRECEIVE_TOPLAYER, (LPVOID) ptMsg, &dwLength);
                                        
                    if (   dwLength != 6
                        || dpIdTo2 != dpIdFrom
                        || ptMsg->usType != BANDWIDTH)
                    {
                    
                        wsprintf( chOutput, "Bad Message %d: Len %d To %d(%d) Msg %d",
                            __LINE__, dwLength, dpIdTo2, dpIdFrom, ptMsg->usType);
                        m_cLBoxResults.AddString(chOutput);
                        wsprintf( chOutput, "dwGot %d dwSent %d iCalled %d",
                            dwGot, dwSent, iCalled);
                        m_cLBoxResults.AddString(chOutput);
                        return;
                    }
                    else
                    {
                        dwTickTotal += GetTickCount() - ptMsg->dwTicks;
                        dwGot++;
                    }
                break;    
                }
            }
        }
        timeKillEvent(iId);

        while (   bGo
               && dwGot < dwSent
               && WaitForSingleObject(hPlayerEvent[dpIdFrom], 1000) != WAIT_TIMEOUT)
        {
            dpIdTo2 = dpIdFrom;
            hr = pIDP->Receive(&dpIdFrom2, &dpIdTo2, DPRECEIVE_TOPLAYER, (LPVOID) ptMsg, &dwLength);
            
            if (   dwLength != 6
                || dpIdTo2 != dpIdFrom
                || ptMsg->usType != BANDWIDTH)
            {
                wsprintf( chOutput, "Bad Message %d: Len %d To %d(%d) Msg %d",
                    __LINE__, dwLength, dpIdTo2, dpIdFrom, ptMsg->usType);
                m_cLBoxResults.AddString(chOutput);
                wsprintf( chOutput, "dwGot %d dwSent %d iCalled %d",
                    dwGot, dwSent, iCalled);
                m_cLBoxResults.AddString(chOutput);
                return;
            }
            else
            {
                dwTickTotal += GetTickCount() - ptMsg->dwTicks;
                dwGot++;
            }
        }
            // wsprintf( chOutput, "Total Ticks %d, total delta %d",
            //     GetTickCount() - dwStartTickCount, dwTickTotal);
            // m_cLBoxResults.AddString(chOutput);

            
            wsprintf( chBuffer, "Bandwidth Hz %d: Latency %d Sent %d Called %d Length %d",
                kk, (dwGot) ? dwTickTotal / dwGot : 99999, dwSent, iCalled, dwLength);
            OutputDebugString(chBuffer);
            OutputDebugString("\r\n");
            m_cLBoxResults.AddString(chBuffer);
            SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE);
            Sleep(5000);
            SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);

        }
    tMsg.usType = EXIT;
    hr = pIDP->Send(dpIdFrom, dpIdTo, 0, (LPVOID) &tMsg, dwLength);
    timeEndPeriod(timecaps.wPeriodMin);
}

void CDptestDlg::OnClose() 
{
    HRESULT hr;
    char    chBuffer[128];

    if (pIDP)
    {
        if ((hr = pIDP->Close()) == DP_OK)
        {
            m_cLBoxResults.AddString("Close Succeeded.");
        }
        else
        {
            wsprintf( chBuffer, "Close failed %8x", hr);
            m_cLBoxResults.AddString(chBuffer);
        }
    }
}

void CDptestDlg::OnDestroyplayer() 
{
    HRESULT hr;
    char    chBuffer[128];
    DPID    dpId;

    UpdateData(TRUE);

    dpId = (DPID) atol(  m_csTo.GetBuffer(2));
    if (pIDP)
    {
        if ((hr = pIDP->DestroyPlayer(dpId)) == DP_OK)
        {
            m_cLBoxResults.AddString("Delete Player Succeeded.");
        }
        else
        {
            wsprintf( chBuffer, "Delete Player failed %8x", hr);
            m_cLBoxResults.AddString(chBuffer);
        }
    }
}

void CDptestDlg::OnEnablenewplayers() 
{
    HRESULT hr;
    char    chBuffer[128];
    BOOL    bb;

    UpdateData(TRUE);

    bb = (BOOL) atol(  m_csTo.GetBuffer(2));

    if (pIDP)
    {
    DPID    dpId;

    UpdateData(TRUE);

    dpId = (DPID) atol(  m_csTo.GetBuffer(2));
        if ((hr = pIDP->EnableNewPlayers(bb)) == DP_OK)
        {
            m_cLBoxResults.AddString("Enable New Players Succeeded.");
        }
        else
        {
            wsprintf( chBuffer, "Enable New Players failed %8x", hr);
            m_cLBoxResults.AddString(chBuffer);
        }
    }
}

BOOL FAR PASCAL EnumPlayers(DPID dpId, LPSTR lpShort, LPSTR lpLong, DWORD dwFlags,
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

void CDptestDlg::OnEnumplayers() 
{
    HRESULT hr;
    char    chBuffer[128];

    if (pIDP)
    {
        if ((hr = pIDP->EnumPlayers(0, EnumPlayers, (LPVOID) &m_cLBoxResults, DPENUMPLAYERS_GROUP)) == DP_OK)
        {
            m_cLBoxResults.AddString("EnumPlayers succeeded.");
        }
        else
        {
            wsprintf( chBuffer, "EnumPlayers %8x", hr);
            m_cLBoxResults.AddString(chBuffer);
        }
    }
}

void CDptestDlg::OnGetcaps() 
{
    HRESULT hr;
    char    chBuffer[128];
    DPCAPS  dpCaps;

    memset(&dpCaps, 0x00, sizeof(dpCaps));
    dpCaps.dwSize = sizeof(dpCaps);

    if (pIDP)
    {
        if ((hr = pIDP->GetCaps(&dpCaps)) == DP_OK)
        {
            m_cLBoxResults.AddString("GetCaps Succeeded.");
            wsprintf( chBuffer, "Caps Flags         %8x", dpCaps.dwFlags);
            m_cLBoxResults.AddString(chBuffer);
            wsprintf( chBuffer, "Caps Max Buffer    %4d", dpCaps.dwMaxBufferSize);
            m_cLBoxResults.AddString(chBuffer);
            wsprintf( chBuffer, "Caps Queue Size    %4d", dpCaps.dwMaxQueueSize);
            m_cLBoxResults.AddString(chBuffer);
            wsprintf( chBuffer, "Caps Max Players   %4d", dpCaps.dwMaxPlayers);
            m_cLBoxResults.AddString(chBuffer);
            wsprintf( chBuffer, "Caps Baud Rate/100 %4d", dpCaps.dwHundredBaud);
            m_cLBoxResults.AddString(chBuffer);
            wsprintf( chBuffer, "Caps Latency       %4d", dpCaps.dwLatency);
            m_cLBoxResults.AddString(chBuffer);
        }
        else
        {
            wsprintf( chBuffer, "GetCaps failed %8x", hr);
            m_cLBoxResults.AddString(chBuffer);
        }
    }
}

void CDptestDlg::OnGetplayercaps() 
{
    HRESULT hr;
    char    chBuffer[128];
    DPCAPS  dpCaps;
    DPID    dpId;

    UpdateData(TRUE);

    dpId = (DPID) atol(  m_csTo.GetBuffer(2));

    memset(&dpCaps, 0x00, sizeof(dpCaps));
    dpCaps.dwSize = sizeof(dpCaps);

    if (pIDP)
    {
        if ((hr = pIDP->GetPlayerCaps(dpId, &dpCaps)) == DP_OK)
        {
            m_cLBoxResults.AddString("GetPlayerCaps Succeeded.");
            wsprintf( chBuffer, "Caps Flags         %8x", dpCaps.dwFlags);
            m_cLBoxResults.AddString(chBuffer);
            wsprintf( chBuffer, "Caps Max Buffer    %4d", dpCaps.dwMaxBufferSize);
            m_cLBoxResults.AddString(chBuffer);
            wsprintf( chBuffer, "Caps Queue Size    %4d", dpCaps.dwMaxQueueSize);
            m_cLBoxResults.AddString(chBuffer);
            wsprintf( chBuffer, "Caps Max Players   %4d", dpCaps.dwMaxPlayers);
            m_cLBoxResults.AddString(chBuffer);
            wsprintf( chBuffer, "Caps Baud Rate/100 %4d", dpCaps.dwHundredBaud);
            m_cLBoxResults.AddString(chBuffer);
            wsprintf( chBuffer, "Caps Latency       %4d", dpCaps.dwLatency);
            m_cLBoxResults.AddString(chBuffer);
        }
        else
        {
            wsprintf( chBuffer, "GetCaps failed %8x", hr);
            m_cLBoxResults.AddString(chBuffer);
        }
    }
}

void CDptestDlg::OnGetplayername() 
{
    HRESULT hr;
    char    chBuffer[128];
    DPID    dpId;
    char    chShort[200];
    char    chLong[200];
    DWORD   dwShort = 200;
    DWORD   dwLong  = 200;


    UpdateData(TRUE);

    dpId = (DPID) atol(  m_csTo.GetBuffer(2));


    if (pIDP)
    {
        if ((hr = pIDP->GetPlayerName(dpId, chShort, &dwShort, chLong, &dwLong)) == DP_OK)
        {
            wsprintf( chBuffer, "Player %d Name is %s(%s).",
                dpId, chLong, chShort);
            m_cLBoxResults.AddString(chBuffer);
        }
        else
        {
            wsprintf( chBuffer, "Get Player Name failed %8x", hr);
            m_cLBoxResults.AddString(chBuffer);
        }
    }
}

void CDptestDlg::OnSetplayername() 
{
    char    chBuffer[256];
    DPID    dpId;
    HRESULT hr;

    UpdateData(TRUE);

    dpId = (DPID) atol(  m_csTo.GetBuffer(2));

    wsprintf( chBuffer, "SetNewName %d", dpId);

    if (pIDP)
    {
        if ((hr = pIDP->SetPlayerName(dpId, chBuffer, "TestSetName")) == DP_OK)
        {
            m_cLBoxResults.AddString("Set Player Name succeeded.");
        }
        else
        {
            wsprintf( chBuffer, "Set Player Name failed %8x", hr);
            m_cLBoxResults.AddString(chBuffer);
        }
    }

}

void CDptestDlg::OnAddplayertogrp() 
{
    char    chBuffer[256];
    DPID    dpIdGroup;
    DPID    dpIdPlayer;
    HRESULT hr;

    UpdateData(TRUE);

    dpIdGroup  = (DPID) atol(  m_csTo.GetBuffer(2));
    dpIdPlayer = (DPID) atol(  m_csFrom.GetBuffer(2));


    if (pIDP)
    {
        if ((hr = pIDP->AddPlayerToGroup(dpIdGroup, dpIdPlayer)) == DP_OK)
        {
            m_cLBoxResults.AddString("Add Player To Group succeeded.");
        }
        else
        {
            wsprintf( chBuffer, "Add Player To Group failed %8x.", hr);
            m_cLBoxResults.AddString(chBuffer);
        }
    }
}

void CDptestDlg::OnCreategroup() 
{
    HRESULT hr;
    DWORD   dwPlayer;
    char    chBuffer[256];
    static  int iNumber = 1;

    
    if (pIDP == NULL)
    {
        m_cLBoxResults.AddString("No DirectPlay Object");
        return;
    }

    wsprintf( chBuffer, "Group %d", iNumber++);
    m_cLBoxResults.AddString(chBuffer);
    hr = pIDP->CreateGroup(&dwPlayer, chBuffer, "Erics Whine");

    if (hr == DP_OK)
    {
        wsprintf( chBuffer, "Create Group(%d) Succeeded", dwPlayer);
    }
    else
    {
        wsprintf( chBuffer, "Create Group Failure (%8x)", hr);
    }
    m_cLBoxResults.AddString(chBuffer);
}

void CDptestDlg::OnDelplayerfromgrp() 
{
    char    chBuffer[256];
    DPID    dpIdGroup;
    DPID    dpIdPlayer;
    HRESULT hr;

    UpdateData(TRUE);

    dpIdGroup  = (DPID) atol(  m_csTo.GetBuffer(2));
    dpIdPlayer = (DPID) atol(  m_csFrom.GetBuffer(2));


    if (pIDP)
    {
        if ((hr = pIDP->DeletePlayerFromGroup(dpIdGroup, dpIdPlayer)) == DP_OK)
        {
            m_cLBoxResults.AddString("Delete Player From Group succeeded.");
        }
        else
        {
            wsprintf( chBuffer, "Delete Player From Group failed %8x.", hr);
            m_cLBoxResults.AddString(chBuffer);
        }
    }

    
}

void CDptestDlg::OnDestroygroup() 
{
    HRESULT hr;
    char    chBuffer[128];
    DPID    dpId;

    UpdateData(TRUE);

    dpId = (DPID) atol(  m_csTo.GetBuffer(2));
    if (pIDP)
    {
        if ((hr = pIDP->DestroyGroup(dpId)) == DP_OK)
        {
            m_cLBoxResults.AddString("Destroy Group Succeeded.");
        }
        else
        {
            wsprintf( chBuffer, "Destroy Group failed %8x", hr);
            m_cLBoxResults.AddString(chBuffer);
        }
    }
}

void CDptestDlg::OnEnumgrp() 
{
    HRESULT hr;
    char    chBuffer[128];
    DPID    dpId;

    UpdateData(TRUE);
    dpId = (DPID) atol(  m_csTo.GetBuffer(2));

    if (pIDP)
    {
        if ((hr = pIDP->EnumGroupPlayers(dpId, EnumPlayers, (LPVOID) &m_cLBoxResults, 0)) == DP_OK)
        {
            m_cLBoxResults.AddString("EnumPlayers succeeded.");
        }
        else
        {
            wsprintf( chBuffer, "EnumPlayers %8x", hr);
            m_cLBoxResults.AddString(chBuffer);
        }
    }
    
}

void CDptestDlg::OnClear() 
{
    // TODO: Add your control notification handler code here
    m_cLBoxResults.ResetContent();

}
