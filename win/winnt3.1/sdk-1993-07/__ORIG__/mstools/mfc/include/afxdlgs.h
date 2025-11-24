// Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation,
// All rights reserved.

// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFXDLGS_H__
#define __AFXDLGS_H__

/////////////////////////////////////////////////////////////////////////////
// Classes declared in this file

		// CDialog
			class CFindReplaceDialog; // Find/FindReplace dialogs
			// class CModalDialog
				class CFileDialog;    // FileOpen/FileSaveAs dialogs
				class CColorDialog;   // Color picker dialog
				class CFontDialog;    // Font chooser dialog
				class CPrintDialog;   // Print/PrintSetup dialogs


/////////////////////////////////////////////////////////////////////////////
// Make sure 'afxwin.h' is included first

#ifndef __AFXWIN_H__
#include "afxwin.h"
#endif

extern "C" {
#include "commdlg.h"    // common dialog APIs

#ifndef _NTWIN
#include "print.h"      // printer specific APIs (DEVMODE)
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Standard dialogs using Windows 3.x dialog (COMMDLG.DLL must be on path)

// CFileDialog - used for FileOpen... or FileSaveAs...
class CFileDialog : public CModalDialog
{
	DECLARE_DYNAMIC(CFileDialog)

public:
// Attributes
	// open file parameter block
	OPENFILENAME m_ofn;

// Constructors
	CFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCSTR lpszDefExt = NULL,
		LPCSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCSTR lpszFilter = NULL, 
		CWnd* pParentWnd = NULL);

// Operations
	virtual int DoModal();

	// Helpers for parsing file name after successful return
	CString GetPathName() const;  // return full path name
	CString GetFileName() const;  // return only filename
	CString GetFileExt() const;   // return only ext
	CString GetFileTitle() const; // return file title
	BOOL GetReadOnlyPref() const; // return TRUE if readonly checked
	
// Overridable callbacks
protected:
	friend UINT FAR PASCAL AFX_EXPORT _AfxCommDlgProc(HWND, UINT, UINT, LONG);
	virtual UINT OnShareViolation(LPCSTR lpszPathName);
	virtual BOOL OnFileNameOK();
	virtual void OnLBSelChangedNotify(UINT nIDBox, UINT iCurSel, UINT nCode);

// Implementation
#ifdef _DEBUG
public:
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void OnOK();
	virtual void OnCancel();

	BOOL m_bOpenFileDialog;       // TRUE for file open, FALSE for file save
	CString m_strFilter;          // filter string
						// separate fields with '|', terminate with '||\0'
	char m_szFileTitle[64];       // contains file title after return
	char m_szFileName[_MAX_PATH]; // contains full path name after return
};

// CFontDialog - used to select a font
class CFontDialog : public CModalDialog
{
	DECLARE_DYNAMIC(CFontDialog)

public:
// Attributes
	// font choosing parameter block
	CHOOSEFONT m_cf;
	LOGFONT    m_lf;

// Constructors
	CFontDialog(LPLOGFONT lplfInitial = NULL,
		DWORD dwFlags = CF_EFFECTS | CF_SCREENFONTS, 
		CDC* pdcPrinter = NULL,
		CWnd* pParentWnd = NULL);

// Operations
	virtual int DoModal();

	// Retrieve the currently selected font while dialog is displayed
	void GetCurrentFont(LPLOGFONT lplf);

	// Helpers for parsing information after successful return
	CString GetFaceName() const;  // return the face name of the font
	CString GetStyleName() const; // return the style name of the font
	int GetSize() const;          // return the pt size of the font
	COLORREF GetColor() const;    // return the color of the font
	int GetWeight() const;        // return the chosen font weight 
	BOOL IsStrikeOut() const;     // return TRUE if strikeout
	BOOL IsUnderline() const;     // return TRUE if underline
	BOOL IsBold() const;          // return TRUE if bold font
	BOOL IsItalic() const;        // return TRUE if italic font

// Implementation

#ifdef _DEBUG
public:
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void OnOK();
	virtual void OnCancel();

	char m_szStyleName[64]; // contains style name after return
};



// CColorDialog - used to select a color
class CColorDialog : public CModalDialog
{
	DECLARE_DYNAMIC(CColorDialog)

public:
// Attributes
	// color picker parameter block
	CHOOSECOLOR m_cc;

// Constructors
	CColorDialog(COLORREF clrInit = 0, DWORD dwFlags = 0, 
			CWnd* pParentWnd = NULL);

// Operations
	virtual int DoModal();

	// Set the current color while dialog is displayed
	void SetCurrentColor(COLORREF clr); 

	// Helpers for parsing information after successful return
	COLORREF GetColor() const;

	// Custom colors are held here and saved between calls
	static COLORREF clrSavedCustom[16];

// Overridable callbacks
protected:
	friend UINT FAR PASCAL AFX_EXPORT _AfxCommDlgProc(HWND, UINT, UINT, LONG);
	virtual BOOL OnColorOK();       // validate color
	
// Implementation

#ifdef _DEBUG
public:
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void OnOK();
	virtual void OnCancel();
};


// CPrintDialog - used for Print... and PrintSetup...
class CPrintDialog : public CModalDialog
{
	DECLARE_DYNAMIC(CPrintDialog)

public:
// Attributes
	// print dialog parameter block (note this is a reference)
	PRINTDLG FAR& m_pd;

// Constructors
	CPrintDialog(BOOL bPrintSetupOnly,  // FALSE for print dialog as well
		DWORD dwFlags = PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS
			| PD_HIDEPRINTTOFILE | PD_NOSELECTION,
		CWnd* pParentWnd = NULL); 

// Operations
	virtual int DoModal();

	// GetDefaults will not display a dialog but will get 
	// device defaults
	BOOL GetDefaults();

	// Helpers for parsing information after successful return
	int GetCopies() const;          // num. copies requested
	BOOL PrintCollate() const;      // TRUE if collate checked
	BOOL PrintSelection() const;    // TRUE if printing selection
	BOOL PrintAll() const;          // TRUE if printing all pages
	BOOL PrintRange() const;        // TRUE if printing page range
	int GetFromPage() const;        // starting page if valid
	int GetToPage() const;          // starting page if valid
	LPDEVMODE GetDevMode() const;   // return DEVMODE
	CString GetDriverName() const;  // return driver name
	CString GetDeviceName() const;  // return device name
	CString GetPortName() const;    // return output port name
	HDC GetPrinterDC() const;       // return HDC (caller must delete)

// Implementation

#ifdef _DEBUG
public:
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void OnOK();
	virtual void OnCancel();

	// The following handle the case of print setup... from the print dialog
	CPrintDialog(PRINTDLG FAR& pdInit);
	afx_msg void OnPrintSetup();
	virtual CPrintDialog* AttachOnSetup();

private:
	PRINTDLG m_pdActual; // the Print/Print Setup need to share this

	DECLARE_MESSAGE_MAP()
};

// Find/FindReplace modeless dialogs
class CFindReplaceDialog : public CDialog
{
	DECLARE_DYNAMIC(CFindReplaceDialog)

public:
// Attributes
	FINDREPLACE m_fr;

// Constructors
	CFindReplaceDialog();
	// NOTE: you must allocate these on the heap.  If you do not,
	// you must derive and override PostNcDestroy() and do not delete this.
	
	BOOL Create(BOOL bFindDialogOnly, // TRUE for Find, FALSE for FindReplace
			LPCSTR lpszFindWhat, 
			LPCSTR lpszReplaceWith  = NULL, 
			DWORD dwFlags = FR_DOWN,
			CWnd* pParentWnd = NULL);

	// find/replace parameter block
	static CFindReplaceDialog* GetNotifier(LONG lParam);

// Operations
	// Helpers for parsing information after successful return
	CString GetReplaceString() const;// get replacement string
	CString GetFindString() const;   // get find string
	BOOL SearchDown() const;         // TRUE if search down, FALSE is up
	BOOL FindNext() const;           // TRUE if command is find next
	BOOL MatchCase() const;          // TRUE if matching case
	BOOL MatchWholeWord() const;     // TRUE if matching whole words only
	BOOL ReplaceCurrent() const;     // TRUE if replacing current string
	BOOL ReplaceAll() const;         // TRUE if replacing all occurrences
	BOOL IsTerminating() const;      // TRUE if terminating dialog

// Implementation
protected:
	virtual void PostNcDestroy();

#ifdef _DEBUG
public:
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	char m_szFindWhat[128];
	char m_szReplaceWith[128];
};


// inline functions for classes in this file

inline CString CFileDialog::GetPathName() const
	{ return m_ofn.lpstrFile; }
inline CString CFileDialog::GetFileExt() const
	{ if (m_ofn.nFileExtension == 0) return (char)'\0';
		else return m_ofn.lpstrFile + m_ofn.nFileExtension; }
inline CString CFileDialog::GetFileTitle() const
	{ return m_ofn.lpstrFileTitle; }
inline BOOL CFileDialog::GetReadOnlyPref() const
	{ return m_ofn.Flags & OFN_READONLY ? TRUE : FALSE; }
inline void CFontDialog::GetCurrentFont(LPLOGFONT lplf)
	{ ASSERT(m_hWnd != NULL); this->SendMessage(WM_CHOOSEFONT_GETLOGFONT, 
		0, (DWORD)(LPSTR)lplf); }
inline CString CFontDialog::GetFaceName() const
	{ return m_cf.lpLogFont->lfFaceName; }
inline CString CFontDialog::GetStyleName() const
	{ return m_cf.lpszStyle; }
inline int CFontDialog::GetSize() const
	{ return m_cf.iPointSize; }
inline int CFontDialog::GetWeight() const
	{ return m_cf.lpLogFont->lfWeight; }
inline BOOL CFontDialog::IsItalic() const
	{ return m_cf.lpLogFont->lfItalic ? TRUE : FALSE; }
inline BOOL CFontDialog::IsStrikeOut() const
	{ return m_cf.lpLogFont->lfStrikeOut ? TRUE : FALSE; }
inline BOOL CFontDialog::IsBold() const
	{ return m_cf.lpLogFont->lfWeight == FW_BOLD ? TRUE : FALSE; }
inline BOOL CFontDialog::IsUnderline() const
	{ return m_cf.lpLogFont->lfUnderline ? TRUE : FALSE; }
inline COLORREF CFontDialog::GetColor() const
	{ return m_cf.rgbColors; }
inline COLORREF CColorDialog::GetColor() const
	{ return m_cc.rgbResult; }
inline BOOL CPrintDialog::GetDefaults()
	{ m_pd.Flags |= PD_RETURNDEFAULT;
	return ::PrintDlg(&m_pd); }
inline BOOL CPrintDialog::PrintSelection() const
	{ return m_pd.Flags & PD_SELECTION ? TRUE : FALSE; }
inline BOOL CPrintDialog::PrintRange() const
	{ return m_pd.Flags & PD_PAGENUMS ? TRUE : FALSE; }
inline BOOL CPrintDialog::PrintAll() const
	{ return !PrintRange() && !PrintSelection() ? TRUE : FALSE; }
inline BOOL CPrintDialog::PrintCollate() const
	{ return m_pd.Flags & PD_COLLATE ? TRUE : FALSE; }
inline int CPrintDialog::GetFromPage() const
	{ return (PrintRange() ? m_pd.nFromPage :-1); }
inline int CPrintDialog::GetToPage() const
	{ return (PrintRange() ? m_pd.nToPage :-1); }
inline LPDEVMODE CPrintDialog::GetDevMode() const
	{ return (LPDEVMODE)::GlobalLock(m_pd.hDevMode); }
inline CString CPrintDialog::GetDriverName() const
	{ LPDEVNAMES lpDev = (LPDEVNAMES)GlobalLock(m_pd.hDevNames);
	return (LPSTR)(lpDev) + (UINT)(lpDev->wDriverOffset); }
inline CString CPrintDialog::GetDeviceName() const
	{ LPDEVNAMES lpDev = (LPDEVNAMES)GlobalLock(m_pd.hDevNames);
	return (LPSTR)(lpDev) + (UINT)(lpDev->wDeviceOffset); }
inline CString CPrintDialog::GetPortName() const
	{ LPDEVNAMES lpDev = (LPDEVNAMES)GlobalLock(m_pd.hDevNames);
	return (LPSTR)(lpDev) + (UINT)(lpDev->wOutputOffset); }
inline BOOL CFindReplaceDialog::IsTerminating() const 
	{ return m_fr.Flags & FR_DIALOGTERM ? TRUE : FALSE ; }
inline CString CFindReplaceDialog::GetReplaceString() const 
	{ return m_fr.lpstrReplaceWith; }
inline CString CFindReplaceDialog::GetFindString() const 
	{ return m_fr.lpstrFindWhat; }
inline BOOL CFindReplaceDialog::SearchDown() const 
	{ return m_fr.Flags & FR_DOWN ? TRUE : FALSE; }
inline BOOL CFindReplaceDialog::FindNext() const 
	{ return m_fr.Flags & FR_FINDNEXT ? TRUE : FALSE; }
inline BOOL CFindReplaceDialog::MatchCase() const 
	{ return m_fr.Flags & FR_MATCHCASE ? TRUE : FALSE; }
inline BOOL CFindReplaceDialog::MatchWholeWord() const 
	{ return m_fr.Flags & FR_WHOLEWORD ? TRUE : FALSE; }
inline BOOL CFindReplaceDialog::ReplaceCurrent() const 
	{ return m_fr. Flags & FR_REPLACE ? TRUE : FALSE; }
inline BOOL CFindReplaceDialog::ReplaceAll() const 
	{ return m_fr.Flags & FR_REPLACEALL ? TRUE : FALSE; }

#endif //__AFXDLGS_H__
