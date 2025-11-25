//*******************************************************************************************
//
// Filename : Dlg.h
//	
//				Definitions of CDlg, CFileDlg and CPropPage
//
// Copyright (c) 1994 - 1996 Microsoft Corporation. All rights reserved
//
//*******************************************************************************************

#ifndef _Dlg_H_
#define _Dlg_H_

class CDlg
{
public:
	CDlg() {}
	~CDlg() {}

	int DoModal(UINT idRes, HWND hParent);
	HWND DoModeless(UINT idRes, HWND hParent);

protected:
	static BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HWND m_hDlg;

private:
	virtual BOOL RealDlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
} ;

class CFileDlg : public tagOFNA
{
public:
	CFileDlg(HWND hwndParent, LPCSTR szFilter, LPSTR szFile, UINT uFileLen, LPCSTR szTitle);
	~CFileDlg() {}

	DWORD GetDlgError() {return(m_dwError);}

protected:
	DWORD m_dwError;
	HWND m_hDlg;

	static BOOL CALLBACK HookProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	virtual BOOL RealHookProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
} ;

class CFileOpenDlg : public CFileDlg
{
public:
	CFileOpenDlg(HWND hwndParent, LPCSTR szFilter, LPSTR szFile, UINT uFileLen, LPCSTR szTitle)
	: CFileDlg(hwndParent, szFilter, szFile, uFileLen, szTitle) {}
	~CFileOpenDlg() {}

	BOOL DoModal();
} ;


class CPropPage : public _PROPSHEETPAGEA
{
public:
	CPropPage(LPCSTR szTmplt);
	virtual ~CPropPage();

	HRESULT DoModeless(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam);

protected:
	static BOOL CALLBACK PageProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HWND m_hPage;

private:
	static UINT CALLBACK PageRelease(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp);

	virtual BOOL RealPageProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	CPropPage *m_pThis;
} ;

#endif // _Dlg_H_
