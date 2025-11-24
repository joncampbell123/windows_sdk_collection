// dialogs.cpp : Several simple dialogs for the ShowFont main window.
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

#include "showfont.h"

static int CurrentFont = 0;
static int CurrentSize = 0;
static char FontList[MAXFONT][32];
static BYTE CharSet[MAXFONT];
static BYTE PitchAndFamily[MAXFONT];
static int FontIndex = 0;
static int SizeList[MAXSIZE];
static int SizeIndex = 0;

static CStringList fontList;        // list of added fonts

/////////////////////////////////////////////////////////////////////////////
// forward declarations

static void GetSizes(CWnd* wnd, int iCurrentFont);

static CString SeparateFile(char* pszDestPath, const char* pszSrcFileName);

/////////////////////////////////////////////////////////////////////////////
// About dialog

void CMainWindow::OnAbout()
{
	CModalDialog about("AboutBox", this);
	about.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// Select Font dialog

class CSelFtDlg : public CModalDialog
{
public:
	CSelFtDlg() : CModalDialog("SelectFont")
		{ }

	CListBox&   TypefaceList()
					{ return *((CListBox*) GetDlgItem(ID_TYPEFACE)); } 
	CListBox&   TypesizeList()
					{ return *((CListBox*) GetDlgItem(ID_SIZE)); } 

	BOOL OnInitDialog()
	{
		CListBox& facesList = TypefaceList();
		CListBox& sizesList = TypesizeList();

		for (int i = 0; i < FontIndex; i++)
		{
			// Display available fonts.
			//
			facesList.AddString(FontList[i]);
			facesList.SetCurSel(0);
		}
		GetSizes(this, 0);
		for (i = 0; i < SizeIndex; i++)
		{
			// Display font sizes.
			//
			char buf[LF_FACESIZE];
			sprintf(buf, "%d", SizeList[i]);
			sizesList.AddString(buf);
			sizesList.SetCurSel(0);
		}
		return TRUE;
	}

	afx_msg void OnOK()
	{
		int index;
		if ((index = TypefaceList().GetCurSel()) == LB_ERR)
		{
			MessageBox("No font selected",
				"Select Font", MB_OK | MB_ICONEXCLAMATION);
			return;
		}
		CurrentFont = index;

		if ((index = TypesizeList().GetCurSel()) == LB_ERR)
		{
			MessageBox("No size selected",
				"Select Font", MB_OK | MB_ICONEXCLAMATION);
			return;
		}
		CurrentSize = index;
		EndDialog(IDOK);
	}

	afx_msg void OnTypeFaceChange()
	{
		int index = TypefaceList().GetCurSel();
		if (index == LB_ERR)
			return;
		TypesizeList().ResetContent();
		GetSizes(this, index);

		CListBox& sizesList = TypesizeList();
		for (int i = 0; i < SizeIndex; i++)
		{
			char buf[LF_FACESIZE];
			sprintf(buf, "%d", SizeList[i]);
			sizesList.AddString(buf);
			sizesList.SetCurSel(0);
		}
	}

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CSelFtDlg, CModalDialog)
	ON_LBN_SELCHANGE(ID_TYPEFACE, OnTypeFaceChange)

	// Double-click on the listboxes act like clicking OK.  No extra code!
	ON_LBN_DBLCLK(ID_TYPEFACE, OnOK)
	ON_LBN_DBLCLK(ID_SIZE, OnOK)
END_MESSAGE_MAP()

void CMainWindow::OnSelectFont()
{
	CSelFtDlg dlg;
	if (dlg.DoModal() != IDOK)
		return;     // cancelled

	// change the font
	myFont->DeleteObject();
	myFont->CreateFont(SizeList[CurrentSize],
		0, 0, 0, FW_NORMAL,
		FALSE, FALSE, FALSE,
		CharSet[CurrentFont],
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		PitchAndFamily[CurrentFont],
		FontList[CurrentFont]);
	pTheFont = myFont;
	SetFaceName();
}

/////////////////////////////////////////////////////////////////////////////
// TextMetrics modeless dialog

class CMetricDlg : public CDialog   // modeless
{
private:
	const TEXTMETRIC& textMetric;

public:
	CMetricDlg(TEXTMETRIC& rtextMetric) : textMetric(rtextMetric)
	{
		VERIFY(Create("MetricBox"));
	}

	afx_msg void OnClose()
	{
		delete this;    // Note this does the DestroyWindow automatically.
	}

	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CMetricDlg, CDialog)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CMetricDlg::OnInitDialog()
{
	// fill in the dialog with statistics
	SetDlgItemInt(IDMB_HEIGHT, textMetric.tmHeight, FALSE);
	SetDlgItemInt(IDMB_ASCENT, textMetric.tmAscent, FALSE);
	SetDlgItemInt(IDMB_DESCENT, textMetric.tmDescent, FALSE);
	SetDlgItemInt(IDMB_INTERNALLEADING, textMetric.tmInternalLeading, FALSE);
	SetDlgItemInt(IDMB_EXTERNALLEADING, textMetric.tmExternalLeading, FALSE);
	SetDlgItemInt(IDMB_AVECHARWIDTH, textMetric.tmAveCharWidth, FALSE);
	SetDlgItemInt(IDMB_MAXCHARWIDTH, textMetric.tmMaxCharWidth, FALSE);
	SetDlgItemInt(IDMB_WEIGHT, textMetric.tmWeight, FALSE);
	SetDlgItemInt(IDMB_ITALIC, textMetric.tmItalic, FALSE);
	SetDlgItemInt(IDMB_UNDERLINED, textMetric.tmUnderlined, FALSE);
	SetDlgItemInt(IDMB_STRUCKOUT, textMetric.tmStruckOut, FALSE);
	SetDlgItemInt(IDMB_FIRSTCHAR, textMetric.tmFirstChar, FALSE);
	SetDlgItemInt(IDMB_LASTCHAR, textMetric.tmLastChar, FALSE);
	SetDlgItemInt(IDMB_DEFAULTCHAR, textMetric.tmDefaultChar, FALSE);
	SetDlgItemInt(IDMB_BREAKCHAR, textMetric.tmBreakChar, FALSE);
	SetDlgItemInt(IDMB_PITCHANDFAMILY, textMetric.tmPitchAndFamily, FALSE);
	SetDlgItemInt(IDMB_CHARSET, textMetric.tmCharSet, FALSE);
	SetDlgItemInt(IDMB_OVERHANG, textMetric.tmOverhang, FALSE);
	SetDlgItemInt(IDMB_DIGITIZEDASPECTX, textMetric.tmDigitizedAspectX, FALSE);
	SetDlgItemInt(IDMB_DIGITIZEDASPECTY, textMetric.tmDigitizedAspectY, FALSE);
	return TRUE;
}


void CMainWindow::OnShowTextMetric()
{
	CClientDC dc(this);
	CFont* oldFont = dc.SelectObject(pTheFont);
	if (oldFont == NULL)
		return;

	TEXTMETRIC textMetric;
	dc.GetTextMetrics(&textMetric);

	CMetricDlg* pDlg;
	pDlg = new CMetricDlg(textMetric);

	char szDialogTitle[100];
	char buf[80];
	strcpy(szDialogTitle, "Metric Font: ");
	dc.GetTextFace(80, buf);
	strcat(szDialogTitle, buf);
	pDlg->SetWindowText(szDialogTitle);

	dc.SelectObject(oldFont);
}

/////////////////////////////////////////////////////////////////////////////
// Show LogicalFont modeless dialog

class CLogFontDlg : public CDialog  // modeless
{
private:
	const LOGFONT& logFont;  // just needed for init

public:
	CLogFontDlg(const LOGFONT& rLogFont) : logFont(rLogFont)
		{
			VERIFY(Create("LogBox"));
		}

	afx_msg void OnClose()
		{
			delete this;  // Does DestroyWindow for us.
		}

	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CLogFontDlg, CDialog)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CLogFontDlg::OnInitDialog()
{
	// Fill in the dialog with statistics.
	SetDlgItemInt(IDMI_HEIGHT, logFont.lfHeight, FALSE);
	SetDlgItemInt(IDMI_WIDTH, logFont.lfWidth, FALSE);
	SetDlgItemInt(IDMI_ESCAPEMENT, logFont.lfEscapement, FALSE);
	SetDlgItemInt(IDMI_ORIENTATION, logFont.lfOrientation, FALSE);
	SetDlgItemInt(IDMI_WEIGHT, logFont.lfWeight, FALSE);
	SetDlgItemInt(IDMI_ITALIC, logFont.lfItalic, FALSE);
	SetDlgItemInt(IDMI_UNDERLINED, logFont.lfUnderline, FALSE);
	SetDlgItemInt(IDMI_STRIKEOUT, logFont.lfStrikeOut, FALSE);
	SetDlgItemInt(IDMI_CHARSET, logFont.lfCharSet, FALSE);
	SetDlgItemInt(IDMI_OUTPRECISION, logFont.lfOutPrecision, FALSE);
	SetDlgItemInt(IDMI_CLIPPRECISION, logFont.lfClipPrecision, FALSE);
	SetDlgItemInt(IDMI_QUALITY, logFont.lfQuality, FALSE);
	SetDlgItemInt(IDMI_PITCHANDFAMILY, logFont.lfPitchAndFamily, FALSE);
	return TRUE;
}

void CMainWindow::OnShowLogFont()
{
	CClientDC dc(this);
	CFont* oldFont = dc.SelectObject(&systemFont);
	if (oldFont == NULL)
		return;

	TEXTMETRIC TextMetric;

	dc.GetTextMetrics(&TextMetric);
	nLineSpace = TextMetric.tmHeight + TextMetric.tmExternalLeading;
	LOGFONT logFont;
	pTheFont->GetObject(sizeof(LOGFONT), &logFont);

	CLogFontDlg* pDlg;
	pDlg = new CLogFontDlg(logFont);    // logFont just needed for init
	char szDialogTitle[100];
	strcpy(szDialogTitle, "Log Font: ");
	strcat(szDialogTitle, (const char*)logFont.lfFaceName);
	pDlg->SetWindowText(szDialogTitle);

	dc.SelectObject(oldFont);
}

/////////////////////////////////////////////////////////////////////////////
// Add Font dialog

class CAddFontDlg : public CModalDialog
{
public:
	CAddFontDlg() : CModalDialog("Add")
		{ }

// Attributes
	char m_szPath[256]; // not a CString since we use it
						//  as parameter to DlgDirSelect

// Implementation
	BOOL OnInitDialog();
	afx_msg void OnOK();
	afx_msg void OnFileChange();
	DECLARE_MESSAGE_MAP()
};

BOOL CAddFontDlg::OnInitDialog()
{
	SetWindowText("Add Font Resource");
	DlgDirList(m_szPath, ID_LISTBOX, ID_PATH, 0x4010);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CAddFontDlg, CModalDialog)
	ON_LBN_SELCHANGE(ID_LISTBOX, OnFileChange)
	ON_LBN_DBLCLK(ID_LISTBOX, OnOK)
END_MESSAGE_MAP()

void CAddFontDlg::OnFileChange()
{
	// If item is a directory name, append "*.fon".
	//
	if (DlgDirSelect(m_szPath, ID_LISTBOX))
		strcat(m_szPath, "*.fon");
	DlgDirList(m_szPath, ID_LISTBOX, ID_PATH, 0x4010);
}

void CAddFontDlg::OnOK()
{
	// Get the filename from the edit control.
	//
	m_szPath[0] = '\0';
	if (DlgDirSelect(m_szPath, ID_LISTBOX) ||
		m_szPath[0] == '\0')
	{
		// It's still a directory, or something is wrong.
		//
		MessageBox("Not a file", "Add Font", MB_OK | MB_ICONQUESTION);
		return;
	}

	// Assume the file is worth trying.
	//
	EndDialog(IDOK);
}

void CMainWindow::OnAddFont()
{
	// Spawn dialog to get the filename.
	//
	CAddFontDlg dlg;

	strcpy(dlg.m_szPath, "*.fon");
	if (dlg.DoModal() != IDOK)
		return;     // cancelled

	// Check to see if it is a new font name.
	//
	for (POSITION pos = fontList.GetHeadPosition(); pos != NULL; )
		if (fontList.GetNext(pos) == dlg.m_szPath)
		{
			MessageBox("Font already exists", "Add Font",
					   MB_OK | MB_ICONQUESTION);
			return;
		}

	// Tell Windows to add the font resource.
	//
	if (AddFontResource(dlg.m_szPath) == 0)
	{
		MessageBox("No font loaded", "Add Font", MB_OK | MB_ICONQUESTION);
		return;
	}

	// Let all applications know there is a new font resource.
	//
	::SendMessage((HWND) 0xFFFF, WM_FONTCHANGE, NULL, (LONG) NULL);

	// Add it to our font list.
	//
	fontList.AddTail(dlg.m_szPath);         // save copy of string
}

/////////////////////////////////////////////////////////////////////////////
// Remove Font dialog

class CRmvFtDlg : public CModalDialog
{
public:
	int m_iFont;  // font index (from global fontList)

	CRmvFtDlg() : CModalDialog("Remove")
		{ }

	CListBox&   FileList()
		{
			return *((CListBox*) GetDlgItem(ID_LISTBOX));
		}

	BOOL OnInitDialog();
	afx_msg void OnOK();

	DECLARE_MESSAGE_MAP()
};

BOOL CRmvFtDlg::OnInitDialog()
{
	SetWindowText("Remove Font Resource");

	for (POSITION pos = fontList.GetHeadPosition(); pos != NULL; )
		FileList().AddString(fontList.GetNext(pos));
	return TRUE;
}

BEGIN_MESSAGE_MAP(CRmvFtDlg, CModalDialog)
	// A double-click on the listbox aliases for clicking the OK.
	ON_LBN_DBLCLK(ID_LISTBOX, OnOK)
END_MESSAGE_MAP()

void CRmvFtDlg::OnOK()
{
	// Get the filename from the edit control.
	//
	m_iFont = FileList().GetCurSel();

	EndDialog(IDOK);
}

void CMainWindow::OnDeleteFont()
{
	if (fontList.IsEmpty())
	{
		MessageBox("No fonts to delete",
			"Remove Font", MB_OK | MB_ICONQUESTION);
		return;
	}

	// Invoke dialog to let user select one font from list.
	//
	CRmvFtDlg   dlg;
	if (dlg.DoModal() != IDOK || dlg.m_iFont == -1)
		return;     // cancelled or no selection

	POSITION posFont = fontList.FindIndex(dlg.m_iFont);
	ASSERT(posFont != NULL);

	// Remove it.  Tell all the apps.
	//
	RemoveFontResource((LPSTR)(LPCSTR)fontList.GetAt(posFont));
	::SendMessage((HWND) 0xFFFF, WM_FONTCHANGE, NULL, (LONG) NULL);

	// Remove that element in the list.
	//
	fontList.RemoveAt(posFont);
}

/////////////////////////////////////////////////////////////////////////////
// Cleanup on exit

void CMainWindow::OnDestroy()
{
	// Remove any fonts that were added.
	//
	while (!fontList.IsEmpty())
		RemoveFontResource((LPSTR)(LPCSTR)fontList.RemoveHead());

	// Notify any other applications that the fonts have been deleted.
	//
	::SendMessage((HWND) 0xFFFF, WM_FONTCHANGE, NULL, (LONG) NULL);

	// Terminate ourselves.
	//
	PostQuitMessage(0);
}

/////////////////////////////////////////////////////////////////////////////
// Routines that must enumerate all fonts.
// (These keep the font names and sizes in global variables.)

int FAR PASCAL _export EnumFunc(LPLOGFONT lpLogFont, LPTEXTMETRIC, short, 
		LPSTR lpData)
{
	switch (LOWORD((DWORD)lpData))
	{
		case 0:
			if (FontIndex >= MAXFONT)
				return (0);
			_fstrcpy(FontList[FontIndex], (PSTR)lpLogFont->lfFaceName);
			CharSet[FontIndex] = lpLogFont->lfCharSet;
			PitchAndFamily[FontIndex] = lpLogFont->lfPitchAndFamily;
			return (++FontIndex);

		case 1:
			if (SizeIndex >= MAXSIZE)
				return (0);
			SizeList[SizeIndex] = lpLogFont->lfHeight;
			return (++SizeIndex);
	}
	ASSERT(FALSE);
	return 0;
}

void CMainWindow::OnFontChange()
{
	FontIndex = 0;
	SizeIndex = 0;
	CClientDC dc(this);
#ifdef _NTWIN
	::EnumFonts(dc.m_hDC, NULL, (FONTENUMPROC)EnumFunc, NULL);
#else
	::EnumFonts(dc.m_hDC, NULL, (OLDFONTENUMPROC)EnumFunc, NULL);
#endif
}

static void GetSizes(CWnd* wnd, int iCurrentFont)
{
	SizeIndex = 0;
	CClientDC dc(wnd);

#ifdef _NTWIN
#ifdef STRICT
	::EnumFonts(dc.m_hDC, FontList[iCurrentFont], (FONTENUMPROC)EnumFunc, (LPARAM)1L);
#else
	::EnumFonts(dc.m_hDC, FontList[iCurrentFont], (FONTENUMPROC)EnumFunc, (LPARAM)1L);
#endif // STRICT
#else
#ifdef STRICT
	::EnumFonts(dc.m_hDC, FontList[iCurrentFont], (OLDFONTENUMPROC)EnumFunc, 1L);
#else
	::EnumFonts(dc.m_hDC, FontList[iCurrentFont], (OLDFONTENUMPROC)EnumFunc, (LPSTR)1L);
#endif // STRICT
#endif // NTWIN
}
