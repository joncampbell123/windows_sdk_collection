// tracer.cpp : MFC Windows trace control application
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include <afxwin.h>

/////////////////////////////////////////////////////////////////////////////
// Dialog class

class CPromptDlg : public CModalDialog
{
	BOOL    m_bEnabled;
	UINT    m_flags;
public:
	CPromptDlg();

	void Save();

	BOOL OnInitDialog();
	void OnOK();
};

static char BASED_CODE szIniFile[] = "AFX.INI";
static char BASED_CODE szDiagSection[] = "Diagnostics";

CPromptDlg::CPromptDlg()
	: CModalDialog("PROMPT")
{
	m_bEnabled = ::GetPrivateProfileInt(szDiagSection, "TraceEnabled", 
		FALSE, szIniFile);
	m_flags = ::GetPrivateProfileInt(szDiagSection, "TraceFlags", 
		0, szIniFile);
}

void CPromptDlg::Save()
{
	char szT[32];
	wsprintf(szT, "%d", m_bEnabled);

	if (!::WritePrivateProfileString(szDiagSection, "TraceEnabled", 
		szT, szIniFile))
	{
		MessageBox("Unable to Write to Initialization File",
			"Tracer", MB_ICONEXCLAMATION);
	}

	wsprintf(szT, "%d", m_flags);
	if (!::WritePrivateProfileString(szDiagSection, "TraceFlags", szT, 
		szIniFile))
	{
		MessageBox("Unable to Write to Initialization File",
			"Tracer", MB_ICONEXCLAMATION);
	}    
}

BOOL CPromptDlg::OnInitDialog()
{
	if (m_bEnabled)
		((CButton*)GetDlgItem(512))->SetCheck(TRUE);

	for (int i = 0; i < 5; i++)
	{
		CButton* pCheck = (CButton*)GetDlgItem(256 + i);
		if (pCheck == NULL)
			break;      // no more checkboxes
		if (m_flags & (1 << i))
			pCheck->SetCheck(TRUE);
	}
	return TRUE;
}

void CPromptDlg::OnOK()
{
	m_bEnabled = ((CButton*)GetDlgItem(512))->GetCheck();
	m_flags = 0;

	for (int i = 0; i < 5; i++)
	{
		CButton* pCheck = (CButton*)GetDlgItem(256 + i);
		if (pCheck == NULL)
			break;      // no more checkboxes
		if (pCheck->GetCheck())
			m_flags |= (1 << i);
	}
	EndDialog(IDOK);
}

/////////////////////////////////////////////////////////////////////////////
// Application class

class CTracerApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
};

CTracerApp theTracerApp;

BOOL CTracerApp::InitInstance()
{
	CPromptDlg  dlg;

	if (dlg.DoModal() == IDOK)
		dlg.Save();

	::PostQuitMessage(0);       // Exit application
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
