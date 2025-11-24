// tracer.cpp : Contains TRACER.DLL implementation and initialization
//              code.
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
#include "traceapi.h"

#ifndef _DEBUG
#error This source file must be compiled with _DEBUG defined
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialog class

class CPromptDlg : public CModalDialog
{
	TracerData FAR* m_lpData;
public:
	CPromptDlg(TracerData FAR* lpData)
		: CModalDialog("PROMPT")
		{ m_lpData = lpData; }

	BOOL OnInitDialog();
	void OnOK();
};

BOOL CPromptDlg::OnInitDialog()
{
	if (m_lpData->bEnabled)
		((CButton*)GetDlgItem(512))->SetCheck(TRUE);

	for (int i = 0; i < 16; i++)
	{
		CButton* pCheck = (CButton*)GetDlgItem(256 + i);
		if (pCheck == NULL)
			break;      // no more checkboxes
		if (m_lpData->flags & (1 << i))
			pCheck->SetCheck(TRUE);
	}
	return TRUE;
}

void CPromptDlg::OnOK()
{
	m_lpData->bEnabled = ((CButton*)GetDlgItem(512))->GetCheck();
	m_lpData->flags = 0;

	for (int i = 0; i < 16; i++)
	{
		CButton* pCheck = (CButton*)GetDlgItem(256 + i);
		if (pCheck == NULL)
			break;      // no more checkboxes
		if (pCheck->GetCheck())
			m_lpData->flags |= (1 << i);
	}
	EndDialog(IDOK);
}

/////////////////////////////////////////////////////////////////////////////
// Public C interface

extern "C"
BOOL FAR PASCAL _export PromptTraceFlags(TracerData FAR* lpData)
{
	TRACE("Inside Tracer DLL\n");
	CPromptDlg  dlg(lpData);

	return (dlg.DoModal() == IDOK);
}

/////////////////////////////////////////////////////////////////////////////
// Library init

class CTracerDLL : public CWinApp
{
public:
	virtual BOOL InitInstance();

	// nothing special for the constructor
	CTracerDLL(const char* pszAppName)
		: CWinApp(pszAppName)
		{ }
};

BOOL CTracerDLL::InitInstance()
{
	// any DLL initialization goes here
	TRACE("TRACER.DLL initializing\n");
	return TRUE;
}

extern "C"
BOOL FAR PASCAL _export FilterDllMsg(LPMSG lpMsg)
{
	return AfxGetApp()->PreTranslateMessage(lpMsg);
}

CTracerDLL  tracerDLL("tracer.dll");

/////////////////////////////////////////////////////////////////////////////
