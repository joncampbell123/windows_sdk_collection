// SendTimeDlg.cpp : implementation file
//
//THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
//EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
//MERCHANTIBILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
//
//Copyright (C) 1996 Microsoft Corporation. All Rights Reserved.

#include "stdafx.h"
#include <afxconv.h>
#include "SendTime.h"
#include "SendTDlg.h"
#include "..\\settime\settime.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
        CAboutDlg();

// Dialog Data
        //{{AFX_DATA(CAboutDlg)
        enum { IDD = IDD_ABOUTBOX };
        //}}AFX_DATA

        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CAboutDlg)
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

// Implementation
protected:
        //{{AFX_MSG(CAboutDlg)
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
        //{{AFX_DATA_INIT(CAboutDlg)
        //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CAboutDlg)
        //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
        //{{AFX_MSG_MAP(CAboutDlg)
                // No message handlers
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendTimeDlg dialog

CSendTimeDlg::CSendTimeDlg(CWnd* pParent /*=NULL*/)
        : CDialog(CSendTimeDlg::IDD, pParent)
{
        //{{AFX_DATA_INIT(CSendTimeDlg)
                // NOTE: the ClassWizard will add member initialization here
        //}}AFX_DATA_INIT
        // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
        m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
        myComLoc.m_pDlg = this;     //set pointer to this dialog
}

void CSendTimeDlg::DoDataExchange(CDataExchange* pDX)
{
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CSendTimeDlg)
        DDX_Control(pDX, IDC_STATIC_CURRENT_MACHINE, m_CurrentMachine);
        DDX_Control(pDX, IDC_SETTIME_PROGRESS, m_SettimeProgress);
        //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSendTimeDlg, CDialog)
        //{{AFX_MSG_MAP(CSendTimeDlg)
        ON_WM_SYSCOMMAND()
        ON_WM_PAINT()
        ON_WM_QUERYDRAGICON()
        ON_BN_CLICKED(IDC_BUTTON_FIRST, OnButtonFirst)
        ON_BN_CLICKED(IDC_BUTTON_PREV, OnButtonPrev)
        ON_BN_CLICKED(IDC_BUTTON_NEXT, OnButtonNext)
        ON_BN_CLICKED(IDC_BUTTON_LAST, OnButtonLast)
        ON_BN_CLICKED(IDC_BUTTON_FIND, OnButtonFind)
        ON_BN_CLICKED(IDC_BUTTON_INSERT, OnButtonInsert)
        ON_BN_CLICKED(IDC_BUTTON_UPDATE, OnButtonUpdate)
        ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
        ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
        ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnButtonCancel)
        ON_BN_CLICKED(IDC_BUTTON_SETTIME, OnButtonSettime)
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendTimeDlg message handlers

BOOL CSendTimeDlg::OnInitDialog()
{
        CDialog::OnInitDialog();

        // Add "About..." menu item to system menu.

        // IDM_ABOUTBOX must be in the system command range.
        ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
        ASSERT(IDM_ABOUTBOX < 0xF000);

        CMenu* pSysMenu = GetSystemMenu(FALSE);
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty())
        {
                pSysMenu->AppendMenu(MF_SEPARATOR);
                pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }

        // Set the icon for this dialog.  The framework does this automatically
        //  when the application's main window is not a dialog
        SetIcon(m_hIcon, TRUE);         // Set big icon
        SetIcon(m_hIcon, FALSE);        // Set small icon

        // TODO: Add extra initialization here
        activeButtonInd = ' ';
        this->DisableEditBoxes();

        //Initialize controls
        m_CurrentMachine.SetWindowText(_T(""));
        m_SettimeProgress.SetRange(0,1);
        m_SettimeProgress.SetPos(0);

        return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSendTimeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
        if ((nID & 0xFFF0) == IDM_ABOUTBOX)
        {
                CAboutDlg dlgAbout;
                dlgAbout.DoModal();
        }
        else
        {
                CDialog::OnSysCommand(nID, lParam);
        }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSendTimeDlg::OnPaint()
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
HCURSOR CSendTimeDlg::OnQueryDragIcon()
{
        return (HCURSOR) m_hIcon;
}


void CSendTimeDlg::OnButtonFirst()
{
        myComLoc.First();
        this->SetText();
}

void CSendTimeDlg::OnButtonPrev()
{
        myComLoc.Prev();
        this->SetText();
}

void CSendTimeDlg::OnButtonNext()
{
        myComLoc.Next();
        this->SetText();
}

void CSendTimeDlg::OnButtonLast()
{
        myComLoc.Last();
        this->SetText();
}

void CSendTimeDlg::OnButtonFind()
{
        if (activeButtonInd == ' ')     //begin find process
        {
                //Set button indicator
                activeButtonInd = 'F';

                //Store row in case of cancellation
                myComLoc.Store();

                //Clear all edit boxes
                this->ClearText();

                //Hide edit boxes
                this->HideBoxes();

                //Enable only Find edit boxes
                this->DisableEditBoxes();
                GetDlgItem(IDC_EDIT2)->EnableWindow(TRUE);

                //Disable buttons (but enable Apply and Cancel)
                this->DisableButtons();

                //Stick Find button
                CButton* pFind = (CButton*)this->GetDlgItem(IDC_BUTTON_FIND);
                pFind->SetState(1);
        }
        else                          //complete find process
        {
                //Set button indicator
                activeButtonInd = ' ';

                //Get text from Find edit boxes
                CEdit* pEdit2 = (CEdit*)this->GetDlgItem(IDC_EDIT2);
                pEdit2->GetWindowText((char*)myComLoc.cl_machineFind, cb_CL_MACHINE + 1);

                //Find row
                myComLoc.Find();

                //Set the text
                this->SetText();

                //Show edit boxes
                this->ShowBoxes();

                //Disable all edit boxes
                this->DisableEditBoxes();

                //Enable buttons (but disable Apply and Cancel)
                this->EnableButtons();

                //Unstick Find button
                CButton* pFind = (CButton*)this->GetDlgItem(IDC_BUTTON_FIND);
                pFind->SetState(0);
        }
}

void CSendTimeDlg::OnButtonInsert()
{
        if (activeButtonInd == ' ')     //begin insert process
        {
                //Set button indicator
                activeButtonInd = 'I';

                //Store row in case of cancellation
                myComLoc.Store();

                //Clear all edit boxes
                this->ClearText();

                //Enable all edit boxes
                this->EnableEditBoxes();

                //Disable buttons (but enable Apply and Cancel)
                this->DisableButtons();

                //Stick Insert button
                CButton* pInsert = (CButton*)this->GetDlgItem(IDC_BUTTON_INSERT);
                pInsert->SetState(1);
        }
        else                            //complete insert process
        {
                //Set indicator
                activeButtonInd = ' ';

                //Get text from edit boxes
                this->GetText();

                //Insert row
                myComLoc.Insert();

                //Disable all edit boxes
                this->DisableEditBoxes();

                //Enable buttons (but disable Apply and Cancel)
                this->EnableButtons();

                //Unstick Insert button
                CButton* pInsert = (CButton*)this->GetDlgItem(IDC_BUTTON_INSERT);
                pInsert->SetState(0);
        }
}

void CSendTimeDlg::OnButtonUpdate()
{
        if (activeButtonInd == ' ')         //begin update process
        {
                //Set button indicator
                activeButtonInd = 'U';

                //Store row in case of cancellation
                myComLoc.Store();

                //Enable all edit boxes
                this->EnableEditBoxes();

                //Disable buttons (but enable Apply and Cancel)
                this->DisableButtons();

                //Stick Update button
                CButton* pUpdate = (CButton*)this->GetDlgItem(IDC_BUTTON_UPDATE);
                pUpdate->SetState(1);
        }
        else                                //complete update process
        {
                //Set button indicator
                activeButtonInd = ' ';

                //Get text from edit boxes
                this->GetText();

                //Update row
                myComLoc.Update();

                //Disable all edit boxes
                this->DisableEditBoxes();

                //Enable buttons (but disable Apply and Cancel)
                this->EnableButtons();

                //Unstick Update button
                CButton* pUpdate = (CButton*)this->GetDlgItem(IDC_BUTTON_UPDATE);
                pUpdate->SetState(0);
        }
}

void CSendTimeDlg::OnButtonDelete()
{
        if (activeButtonInd == ' ')             //begin delete process
        {
                //Set button indicator
                activeButtonInd = 'D';

                //Store row in case of cancellation
                myComLoc.Store();

                //Disable buttons (but enable Apply and Cancel)
                this->DisableButtons();

                //Stick Delete button
                CButton* pDelete = (CButton*)this->GetDlgItem(IDC_BUTTON_DELETE);
                pDelete->SetState(1);
        }
        else                                    //complete delete process
        {
                //Set button indicator
                activeButtonInd = ' ';

                //Delete the row
                myComLoc.Delete();

                //Get first record and set the text
                myComLoc.First();
                this->SetText();

                //Disable all edit boxes
                this->DisableEditBoxes();

                //Enable buttons (but disable Apply and Cancel)
                this->EnableButtons();

                //Unstick Delete button
                CButton* pDelete = (CButton*)this->GetDlgItem(IDC_BUTTON_DELETE);
                pDelete->SetState(0);
        }
}

void CSendTimeDlg::OnButtonApply()
{
        //Check which button
        if (activeButtonInd == 'I')
        {
                this->OnButtonInsert();
        }
        else
        if (activeButtonInd == 'U')
        {
                this->OnButtonUpdate();
        }
        else
        if (activeButtonInd == 'D')
        {
                this->OnButtonDelete();
        }
        else
        {
                this->OnButtonFind();
        }
}

void CSendTimeDlg::OnButtonCancel()
{
        //Restore row from hold area
        myComLoc.Restore();
        this->SetText();

        //Show boxes
        if (activeButtonInd == 'F')
        {
                this->ShowBoxes();
        }

        //Disable edit boxes
        this->DisableEditBoxes();

        //Enable buttons
        this->EnableButtons();

        //Unstick buttons
        CButton* pInsert = (CButton*)this->GetDlgItem(IDC_BUTTON_INSERT);
        pInsert->SetState(0);

        CButton* pUpdate = (CButton*)this->GetDlgItem(IDC_BUTTON_UPDATE);
        pUpdate->SetState(0);

        CButton* pDelete = (CButton*)this->GetDlgItem(IDC_BUTTON_DELETE);
        pDelete->SetState(0);

        CButton* pFind = (CButton*)this->GetDlgItem(IDC_BUTTON_FIND);
        pFind->SetState(0);

        activeButtonInd = ' ';

}

void CSendTimeDlg::GetText()
{
        CEdit* pEdit2 = (CEdit*)this->GetDlgItem(IDC_EDIT2);
        pEdit2->GetWindowText((char*)myComLoc.cl.cl_machine, cb_CL_MACHINE + 1);

        CEdit* pEdit1 = (CEdit*)this->GetDlgItem(IDC_EDIT1);
        pEdit1->GetWindowText((char*)myComLoc.cl.cl_progid, cb_CL_PROGID + 1);
}

void CSendTimeDlg::SetText()
{
        CEdit* pEdit2 = (CEdit*)this->GetDlgItem(IDC_EDIT2);
        pEdit2->SetWindowText((char*)myComLoc.cl.cl_machine);

        CEdit* pEdit1 = (CEdit*)this->GetDlgItem(IDC_EDIT1);
        pEdit1->SetWindowText((char*)myComLoc.cl.cl_progid);
}

void CSendTimeDlg::ClearText()
{
        CEdit* pEdit2 = (CEdit*)this->GetDlgItem(IDC_EDIT2);
        pEdit2->SetWindowText("");

        CEdit* pEdit1 = (CEdit*)this->GetDlgItem(IDC_EDIT1);
        pEdit1->SetWindowText("");
}


void CSendTimeDlg::DisableEditBoxes()
{
        GetDlgItem(IDC_EDIT2)->EnableWindow(FALSE);
        GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);
}

void CSendTimeDlg::EnableEditBoxes()
{
        GetDlgItem(IDC_EDIT2)->EnableWindow(TRUE);
        GetDlgItem(IDC_EDIT1)->EnableWindow(TRUE);
}


void CSendTimeDlg::EnableButtons()  //enable most buttons
{
        CButton* pFirst = (CButton*)this->GetDlgItem(IDC_BUTTON_FIRST);
        pFirst->EnableWindow(TRUE);

        CButton* pLast = (CButton*)this->GetDlgItem(IDC_BUTTON_LAST);
        pLast->EnableWindow(TRUE);

        CButton* pPrev = (CButton*)this->GetDlgItem(IDC_BUTTON_PREV);
        pPrev->EnableWindow(TRUE);

        CButton* pNext = (CButton*)this->GetDlgItem(IDC_BUTTON_NEXT);
        pNext->EnableWindow(TRUE);

        CButton* pFind = (CButton*)this->GetDlgItem(IDC_BUTTON_FIND);
        pFind->EnableWindow(TRUE);

        CButton* pInsert = (CButton*)this->GetDlgItem(IDC_BUTTON_INSERT);
        pInsert->EnableWindow(TRUE);

        CButton* pChange = (CButton*)this->GetDlgItem(IDC_BUTTON_UPDATE);
        pChange->EnableWindow(TRUE);

        CButton* pDelete = (CButton*)this->GetDlgItem(IDC_BUTTON_DELETE);
        pDelete->EnableWindow(TRUE);

        CButton* pApply = (CButton*)this->GetDlgItem(IDC_BUTTON_APPLY);
        pApply->EnableWindow(FALSE);

        CButton* pCancel = (CButton*)this->GetDlgItem(IDC_BUTTON_CANCEL);
        pCancel->EnableWindow(FALSE);
}


void CSendTimeDlg::DisableButtons() //disable most buttons
{
        CButton* pFirst = (CButton*)this->GetDlgItem(IDC_BUTTON_FIRST);
        pFirst->EnableWindow(FALSE);

        CButton* pLast = (CButton*)this->GetDlgItem(IDC_BUTTON_LAST);
        pLast->EnableWindow(FALSE);

        CButton* pPrev = (CButton*)this->GetDlgItem(IDC_BUTTON_PREV);
        pPrev->EnableWindow(FALSE);

        CButton* pNext = (CButton*)this->GetDlgItem(IDC_BUTTON_NEXT);
        pNext->EnableWindow(FALSE);

        CButton* pFind = (CButton*)this->GetDlgItem(IDC_BUTTON_FIND);
        pFind->EnableWindow(FALSE);

        CButton* pInsert = (CButton*)this->GetDlgItem(IDC_BUTTON_INSERT);
        pInsert->EnableWindow(FALSE);

        CButton* pChange = (CButton*)this->GetDlgItem(IDC_BUTTON_UPDATE);
        pChange->EnableWindow(FALSE);

        CButton* pDelete = (CButton*)this->GetDlgItem(IDC_BUTTON_DELETE);
        pDelete->EnableWindow(FALSE);

        CButton* pApply = (CButton*)this->GetDlgItem(IDC_BUTTON_APPLY);
        pApply->EnableWindow(TRUE);

        CButton* pCancel = (CButton*)this->GetDlgItem(IDC_BUTTON_CANCEL);
        pCancel->EnableWindow(TRUE);

}


void CSendTimeDlg::HideBoxes()
{
        CEdit* pEdit1 = (CEdit*)this->GetDlgItem(IDC_EDIT1);
        pEdit1->ShowWindow(SW_HIDE);
}


void CSendTimeDlg::ShowBoxes()
{
        CEdit* pEdit1 = (CEdit*)this->GetDlgItem(IDC_EDIT1);
        pEdit1->ShowWindow(SW_SHOW);
}


CStringArray m_MachineList;


void CSendTimeDlg::OnButtonSettime()
{
        //Load list of machine names
        m_MachineList.RemoveAll();          //clear the array

        if (myComLoc.First() == 0)
        {
                m_MachineList.Add((char*)myComLoc.cl.cl_machine);

                while (myComLoc.Next() == 0)
                {
                        m_MachineList.Add((char*)myComLoc.cl.cl_machine);
                }
        }

        //Get size of list, set progress bar, clear text
        int n = m_MachineList.GetSize();
        m_CurrentMachine.SetWindowText(_T(""));
        m_SettimeProgress.SetRange(0,n);
        m_SettimeProgress.SetPos(0);
        CWinApp* pApp = AfxGetApp();
        pApp->DoWaitCursor(1);

        //Set time
        for (int k=0; k < n; k++)
        {
                CString szMsg;
                szMsg.Format(_T("Calling Machine %s"), m_MachineList[k]);
                m_CurrentMachine.SetWindowText(szMsg);
                m_SettimeProgress.SetPos(k+1);
                CallMachine(m_MachineList[k]);
        }

        pApp->DoWaitCursor(-1);
        m_CurrentMachine.SetWindowText(_T("Done!"));
}

void CSendTimeDlg::CallMachine(LPCTSTR lpszMachine)
{
        USES_CONVERSION;

        COSERVERINFO ServerInfo;
        ServerInfo.dwSize = sizeof(COSERVERINFO);
        ServerInfo.pszName = T2OLE(lpszMachine);

        MULTI_QI Result;
        Result.pIID = &IID_IMySettime;
        Result.pItf = NULL;
        Result.hr = S_OK;

        HRESULT hRes = CoCreateInstanceEx(CLSID_MySettime, NULL,
                        CLSCTX_SERVER, &ServerInfo, 1, &Result);

        if(!SUCCEEDED(hRes))
        {
                CString szMsg;
                szMsg.Format(_T("CoCreateInstanceEx() failed on %s"), lpszMachine);
                AfxMessageBox(szMsg);
                return;
        }

        _ASSERTE(SUCCEEDED(Result.hr));
        _ASSERTE(Result.pItf != NULL);
                IMySettime* pObj;
        pObj = (IMySettime*)Result.pItf;
        SYSTEMTIME st;
        GetSystemTime(&st);
        hRes = pObj->Set(st);

        if(!SUCCEEDED(hRes))
        {
                CString szMsg;
                szMsg.Format(_T("SetSystemTime() failed on %s"), lpszMachine);
                AfxMessageBox(szMsg);
        }

        pObj->Release();

}
