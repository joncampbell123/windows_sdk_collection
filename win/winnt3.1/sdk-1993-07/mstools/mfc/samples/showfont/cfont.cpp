// cfont.cpp : Defines the CFontDlg modal dialog for ShowFont.
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

/////////////////////////////////////////////////////////////////////////////

// CFontDlg:
// This class is completely declared and defined in this file.
// Unlike most classes, the declaration is not in a separate header file
// which is included in other sources.  Instead, a simple function
// creates an object and invokes the dialog.
//
// This technique may simplify the use of classes whose objects can be
// created and destroyed without much other interaction; modal dialogs like
// this are often candidates for this approach.
//
class CFontDlg : public CModalDialog
{
private:
	LOGFONT&  logFont;
public:
	CFontDlg(CWnd* pWnd, LOGFONT& rLogFont)
		: logFont(rLogFont), CModalDialog("CFont", pWnd)
			{ }


	// One way of getting access to members type-safely is to use
	// member functions like the below that do the appropriate call
	// to GetDlgItem() and cast the result to the correct C++ type.

	// simple edit item
	CEdit&      FaceEdit()
					{ return *((CEdit*) GetDlgItem(ID_FACE)); } 

	// styles (checkboxes)
	CButton&    ItalicCheck()
					{ return *((CButton*) GetDlgItem(ID_ITALIC)); } 
	CButton&    UnderlineCheck()
					{ return *((CButton*) GetDlgItem(ID_UNDERLINE)); } 
	CButton&    StrikeOutCheck()
					{ return *((CButton*) GetDlgItem(ID_STRIKEOUT)); } 


	// When dealing with edit controls used for parsed number values,
	// inline routines that call GetDlgItemInt/SetDlgItemInt are used.

	int         GetHeight()
					{ return GetDlgItemInt(ID_HEIGHT); }
	void        SetHeight(int n)
					{ SetDlgItemInt(ID_HEIGHT, n); }
	int         GetWidth()
					{ return GetDlgItemInt(ID_WIDTH); }
	void        SetWidth(int n)
					{ SetDlgItemInt(ID_WIDTH, n); }
	int         GetEscapement()
					{ return GetDlgItemInt(ID_ESCAPEMENT); }
	void        SetEscapement(int n)
					{ SetDlgItemInt(ID_ESCAPEMENT, n); }
	int         GetOrientation()
					{ return GetDlgItemInt(ID_ORIENTATION); }
	void        SetOrientation(int n)
					{ SetDlgItemInt(ID_ORIENTATION, n); }
	int         GetWeight()
					{ return GetDlgItemInt(ID_WEIGHT); }
	void        SetWeight(int n)
					{ SetDlgItemInt(ID_WEIGHT, n); }
	int         GetCharSetNum()
					{ return GetDlgItemInt(ID_CHARSET); }
	void        SetCharSetNum(int n)
					{ SetDlgItemInt(ID_CHARSET, n); }


	// When dealing with radio groups - it is easy to provide Get and Set
	// functions that treat the group as one value.
	//
	// In the following we convert the radio button IDs to zero based values.

	int     GetOutPrecision()
				{ return GetCheckedRadioButton(ID_OUT_STRING, ID_OUT_DEFAULT)
					- ID_OUT_STRING; }
	void    SetOutPrecision(int n)
				{ CheckRadioButton(ID_OUT_STRING, ID_OUT_DEFAULT,
					ID_OUT_STRING + n); }
	int     GetClipPrecision()
				{ return GetCheckedRadioButton(ID_CLIP_CHAR, ID_CLIP_DEFAULT)
					- ID_CLIP_CHAR; }
	void    SetClipPrecision(int n)
				{ CheckRadioButton(ID_CLIP_CHAR, ID_CLIP_DEFAULT,
					ID_CLIP_CHAR + n); }
	int     GetQuality()
				{ return GetCheckedRadioButton(ID_PROOF, ID_DEF_QUALITY)
					- ID_PROOF; }
	void    SetQuality(int n)
				{ CheckRadioButton(ID_PROOF, ID_DEF_QUALITY, ID_PROOF + n); }
	int     GetPitch()
				{ return GetCheckedRadioButton(ID_FIXED, ID_DEF_PITCH)
					- ID_FIXED; }
	void    SetPitch(int n)
				{ CheckRadioButton(ID_FIXED, ID_DEF_PITCH, ID_FIXED + n); }
	int     GetFamily()
				{ return GetCheckedRadioButton(ID_ROMAN, ID_DEF_FAMILY)
					- ID_ROMAN; }
	void    SetFamily(int n)
				{ CheckRadioButton(ID_ROMAN, ID_DEF_FAMILY, ID_ROMAN + n); }
	int     GetCharSetOption()
				{ return GetCheckedRadioButton(ID_ANSI, ID_OEM) - ID_ANSI; }
	void    SetCharSetOption(int n)
				{ CheckRadioButton(ID_ANSI, ID_OEM, ID_ANSI + n); }

	afx_msg void OnLight()
	{
		SetWeight(FW_LIGHT);
	}

	afx_msg void OnNormal()
	{
		SetWeight(FW_NORMAL);
	}

	afx_msg void OnBold()
	{
		SetWeight(FW_BOLD);
	}

	afx_msg void OnChangeWeight()
	{
		// set a specific weight (uncheck all)
		CheckRadioButton(ID_LIGHT, ID_BOLD, 0);
	}

	afx_msg void OnAnsi()
	{
		SetCharSetNum(ANSI_CHARSET);
	}

	afx_msg void OnOEM()
	{
		SetCharSetNum(OEM_CHARSET);
	}

	afx_msg void OnCharSet()
	{
		// specific character set size - assume neither ANSI not OEM
		CheckRadioButton(ID_ANSI, ID_OEM, 0);
	}

	BOOL    OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
};

// CFontDlg message map:
// This map ties each child control's notification messages (clicks) to
// the appropriate member functions.
//
BEGIN_MESSAGE_MAP(CFontDlg, CModalDialog)
	ON_COMMAND(ID_LIGHT, OnLight)
	ON_COMMAND(ID_NORMAL, OnNormal)
	ON_COMMAND(ID_BOLD, OnBold)
	ON_COMMAND(ID_WEIGHT, OnChangeWeight)
	ON_COMMAND(ID_ANSI, OnAnsi)
	ON_COMMAND(ID_OEM, OnOEM)
	ON_COMMAND(ID_CHARSET, OnCharSet)
	// Note that OnOK is already inherited virtually.
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RadioGroup mapping tables

// In the dialog, radio groups are represented with 0 based numbers.
// Outside of the dialog, we want those numbers to represent specific
// values to set or get from a LOGFONT.  The 'MatchValue' function takes
// one of these values, and turns it into a zero based index.
// The 'GetValue' function takes the zero based index and returns
// the appropriate value.

static int MatchValue(int value, int * pValues)
	// return the index of the match, or -1
{
	for (int index = 0; *pValues != -1; index++)
	{
		if (value == *pValues++)
			return index;
	}
	return -1;
}

static int GetValue(int index, int * pValues)
{
	// index into array with special case for negative values
	if (index < 0)
		return 0;       // hopefully a sensible default
	else
		return pValues[index];
}

// The following tables must be in the same order as the radio buttons
// defined in the resource file.

static int rgOutPrecision[] =
{
	OUT_STRING_PRECIS,
	OUT_CHARACTER_PRECIS,
	OUT_STROKE_PRECIS,
	OUT_DEFAULT_PRECIS,
	-1 /* end */
};

static int rgClipPrecision[] =
{
	CLIP_CHARACTER_PRECIS,
	CLIP_STROKE_PRECIS,
	CLIP_DEFAULT_PRECIS,
	-1 /* end */
};

static int rgQuality[] =
{
	PROOF_QUALITY,
	DRAFT_QUALITY,
	DEFAULT_QUALITY,
	-1 /* end */
};

static int rgPitch[] =
{
	FIXED_PITCH,
	VARIABLE_PITCH,
	DEFAULT_PITCH,
	-1 /* end */
};

static int rgFamily[] =
{
	FF_ROMAN,
	FF_SWISS,
	FF_MODERN,
	FF_SCRIPT,
	FF_DECORATIVE,
	FF_DONTCARE,
	-1 /* end */
};

static int rgCharSet[] =
{
	ANSI_CHARSET,
	OEM_CHARSET,
	-1 /* end */
};

/////////////////////////////////////////////////////////////////////////////
// OnInitDialog:
// This is called when the dialog is invoked.  We set the state of all of
// the child controls to appropriate states from our logFont information.
//
BOOL CFontDlg::OnInitDialog()
{
	SetHeight(logFont.lfHeight);
	SetWidth(logFont.lfWidth);
	SetEscapement(logFont.lfEscapement);
	SetOrientation(logFont.lfOrientation);
	FaceEdit().SetWindowText((LPSTR)logFont.lfFaceName);
	SetCharSetNum(logFont.lfCharSet);

	ItalicCheck().SetCheck(logFont.lfItalic);
	StrikeOutCheck().SetCheck(logFont.lfStrikeOut);
	UnderlineCheck().SetCheck(logFont.lfUnderline);

	SetOutPrecision(MatchValue(logFont.lfOutPrecision, rgOutPrecision));
	SetClipPrecision(MatchValue(logFont.lfClipPrecision, rgClipPrecision));
	SetQuality(MatchValue(logFont.lfQuality, rgQuality));
	SetCharSetOption(MatchValue(logFont.lfCharSet, rgCharSet));

	BYTE pitch = logFont.lfPitchAndFamily & 3;
	BYTE family = logFont.lfPitchAndFamily & 0xf0;
	SetPitch(MatchValue(pitch, rgPitch));
	SetFamily(MatchValue(family, rgFamily));

	// If the weight matches one of the special values, check that radio
	// button.
	//
	SetWeight(logFont.lfWeight);
	switch (logFont.lfWeight)
	{
	case FW_LIGHT:
		CheckRadioButton(ID_LIGHT, ID_BOLD, ID_LIGHT);
		break;

	case FW_NORMAL:
		CheckRadioButton(ID_LIGHT, ID_BOLD, ID_NORMAL);
		break;

	case FW_BOLD:
		CheckRadioButton(ID_LIGHT, ID_BOLD, ID_BOLD);
		break;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// OnOK:
// This is automatically called when the OK button is pushed.  We update
// the logFont information and end the dialog with the "successful" return
// of IDOK.
//
void CFontDlg::OnOK()
{
	logFont.lfHeight = GetHeight();
	logFont.lfWidth = GetWidth();
	logFont.lfEscapement = GetEscapement();
	logFont.lfOrientation = GetOrientation();
	FaceEdit().GetWindowText((LPSTR)logFont.lfFaceName, 32);
	logFont.lfWeight = GetWeight();
	logFont.lfCharSet = GetCharSetNum();

	logFont.lfItalic = ItalicCheck().GetCheck();
	logFont.lfStrikeOut = StrikeOutCheck().GetCheck();
	logFont.lfUnderline = UnderlineCheck().GetCheck();

	logFont.lfOutPrecision = GetValue(GetOutPrecision(), rgOutPrecision);
	logFont.lfClipPrecision = GetValue(GetClipPrecision(), rgClipPrecision);
	logFont.lfQuality = GetValue(GetQuality(), rgQuality);
	// lfCharSet is not set by radio group (get from edit text)

	// pitch and family are combined
	BYTE pitch = GetValue(GetPitch(), rgPitch);
	BYTE family = GetValue(GetFamily(), rgFamily);
	logFont.lfPitchAndFamily = pitch | family;  // put back together

	EndDialog(IDOK);
}

/////////////////////////////////////////////////////////////////////////////
// DoCreateFontDlg:
// This function is the only interface required to use this class.  Calling
// it will create and invoke a CFontDlg object, and will not return until
// the modal dialog has been closed.  Include showfont.h for the prototype.
//
int DoCreateFontDlg(CWnd* pWnd, LOGFONT& rLogFont)
{
	CFontDlg dlg(pWnd, rLogFont);
	return dlg.DoModal();
}

