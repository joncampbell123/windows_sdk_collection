//*******************************************************************************************
//
// Filename : Dlg.cpp
//	
//				Implementation file for CDlg, CFileDlg and CPropPage
//
// Copyright (c) 1994 - 1996 Microsoft Corporation. All rights reserved
//
//*******************************************************************************************

#include "Pch.H"

#include "ThisDll.H"

#include "Dlg.H"

BOOL CDlg::RealDlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return(FALSE);
}


BOOL CALLBACK CDlg::DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CDlg *pThis = (CDlg *)GetWindowLong(hDlg, DWL_USER);

	if (uMsg == WM_INITDIALOG)
	{
		pThis = (CDlg *)lParam;
		pThis->m_hDlg = hDlg;
		SetWindowLong(hDlg, DWL_USER, lParam);
	}
	else if (!pThis)
	{
		return(FALSE);
	}

	return(pThis->RealDlgProc(uMsg, wParam, lParam));
}


int CDlg::DoModal(UINT idRes, HWND hParent)
{
	return(DialogBoxParam(g_ThisDll.GetInstance(), MAKEINTRESOURCE(idRes), hParent,
		DlgProc, (LPARAM)this));
}


HWND CDlg::DoModeless(UINT idRes, HWND hParent)
{
	return(CreateDialogParam(g_ThisDll.GetInstance(), MAKEINTRESOURCE(idRes), hParent,
		DlgProc, (LPARAM)this));
}


CFileDlg::CFileDlg(HWND hwndParent, LPCSTR szFilter, LPSTR szFile, UINT uFileLen,
	LPCSTR szTitle)
	: m_dwError(0)
{
	memset((LPOPENFILENAME)this, 0, sizeof(OPENFILENAME));

	lStructSize = sizeof(OPENFILENAME);
	hwndOwner = hwndParent;
	hInstance = g_ThisDll.GetInstance();
	lpstrFilter = szFilter;
	lpstrFile = szFile;
	nMaxFile = uFileLen;
	lpstrTitle = szTitle;
}


BOOL CFileDlg::RealHookProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return(FALSE);
}


BOOL CFileDlg::HookProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CFileDlg *pThis = (CFileDlg *)GetWindowLong(hDlg, DWL_USER);

	if (uMsg == WM_INITDIALOG)
	{
		LPOPENFILENAME pofn = (LPOPENFILENAME)lParam;

		pThis = (CFileDlg *)pofn->lCustData;
		pThis->m_hDlg = hDlg;
		SetWindowLong(hDlg, DWL_USER, pofn->lCustData);
	}
	else if (!pThis)
	{
		return(FALSE);
	}

	return(pThis->RealHookProc(uMsg, wParam, lParam));
}


BOOL CFileOpenDlg::DoModal()
{
	// Caller needs to set OFN_ENABLEHOOK to get a hook
	lpfnHook = (LPOFNHOOKPROC)HookProc;
	lCustData = (LONG)this;

	BOOL bRet = GetOpenFileName(this);
	m_dwError = bRet ? 0 : CommDlgExtendedError();

	return(bRet);
}


CPropPage::~CPropPage()
{
}


CPropPage::CPropPage(LPCSTR szTmplt)
{
	dwSize = sizeof(CPropPage);
	dwFlags = PSP_DEFAULT;
	hInstance = g_ThisDll.GetInstance();
	pszTemplate = szTmplt;
	pfnDlgProc = PageProc;
	m_pThis = this;
}


HRESULT CPropPage::DoModeless(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam)
{
	dwFlags |= PSP_USECALLBACK;
	pfnCallback = PageRelease;

    // add the page; note this whole object has been copied to the page structure
    HPROPSHEETPAGE hPage = CreatePropertySheetPage(this);
	if (!hPage)
	{
		return(E_OUTOFMEMORY);
	}

	if (!lpfnAddPage(hPage, lParam))
	{
		return(E_UNEXPECTED);
	}

    return(NOERROR);

}


BOOL CALLBACK CPropPage::PageProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CPropPage *pThis = (CPropPage *)GetWindowLong(hDlg, DWL_USER);

	if (uMsg == WM_INITDIALOG)
	{
		CPropPage *pThat = IToClass(CPropPage, dwSize, lParam);
		pThis = pThat->m_pThis;

		pThis->m_hPage = hDlg;
		SetWindowLong(hDlg, DWL_USER, (LONG)pThis);
	}
	else if (!pThis)
	{
		return(FALSE);
	}

	return(pThis->RealPageProc(uMsg, wParam, lParam));
}


UINT CALLBACK CPropPage::PageRelease(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp)
{
	if (uMsg == PSPCB_RELEASE)
	{
		CPropPage *pThat = IToClass(CPropPage, dwSize, ppsp);
		CPropPage *pThis = pThat->m_pThis;

		delete pThis;
	}

	return(TRUE);
}


BOOL CPropPage::RealPageProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return(FALSE);
}
