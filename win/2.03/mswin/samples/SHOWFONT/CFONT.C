/****************************************************************************
    MODULE: cfont.c

    FUNCTION: CFontDlg(HWND, unsigned, WORD, LONG);

    PURPOSE: Processes dialog box messages for creating a font

****************************************************************************/

#include "windows.h"
#include "showfont.h"

BOOL FAR PASCAL CFontDlg(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned	message;
WORD wParam;
LONG lParam;
{
    switch (message) {
	case WM_INITDIALOG:
	    SetDlgItemInt(hDlg, ID_HEIGHT, CLogFont.lfHeight, TRUE);
	    SetDlgItemInt(hDlg, ID_WIDTH, CLogFont.lfWidth, TRUE);
	    SetDlgItemInt(hDlg, ID_ESCAPEMENT,
		CLogFont.lfEscapement, TRUE);
	    SetDlgItemInt(hDlg, ID_ORIENTATION,
		CLogFont.lfOrientation, TRUE);
	    SetDlgItemText(hDlg, ID_FACE, CLogFont.lfFaceName);
	    CheckDlgButton(hDlg, ID_ITALIC, CLogFont.lfItalic);
	    CheckDlgButton(hDlg, ID_UNDERLINE, CLogFont.lfUnderline);
	    CheckDlgButton(hDlg, ID_STRIKEOUT, CLogFont.lfStrikeOut);

	    SetDlgItemInt(hDlg, ID_WEIGHT, CLogFont.lfWeight, TRUE);
	    switch (CLogFont.lfWeight) {
		case FW_LIGHT:
		    CheckRadioButton(hDlg, ID_LIGHT, ID_BOLD, ID_LIGHT);
		    break;

		case FW_NORMAL:
		    CheckRadioButton(hDlg, ID_LIGHT, ID_BOLD, ID_NORMAL);
		    break;

		case FW_BOLD:
		    CheckRadioButton(hDlg, ID_LIGHT, ID_BOLD, ID_BOLD);
		    break;
	    }

	    SetDlgItemInt(hDlg, ID_CHARSET, CLogFont.lfCharSet, TRUE);
	    switch (CLogFont.lfCharSet) {
		case ANSI_CHARSET:
		    CheckRadioButton(hDlg, ID_ANSI, ID_OEM, ID_ANSI);
		    break;

		case OEM_CHARSET:
		    CheckRadioButton(hDlg, ID_ANSI, ID_OEM, ID_OEM);
		    break;
	    }

	    switch (CLogFont.lfOutPrecision) {
		case OUT_STRING_PRECIS:
		    CheckRadioButton(hDlg, ID_OUT_STRING, ID_OUT_DEFAULT,
			ID_OUT_STRING);
		    break;

		case OUT_CHARACTER_PRECIS:
		    CheckRadioButton(hDlg, ID_OUT_STRING, ID_OUT_DEFAULT,
			ID_OUT_CHAR);
		    break;

		case OUT_STROKE_PRECIS:
		    CheckRadioButton(hDlg, ID_OUT_STRING, ID_OUT_DEFAULT,
			ID_OUT_STROKE);
		    break;

		case OUT_DEFAULT_PRECIS:
		    CheckRadioButton(hDlg, ID_OUT_STRING, ID_OUT_DEFAULT,
			ID_OUT_DEFAULT);
		    break;
	    }

	    switch (CLogFont.lfClipPrecision) {
		case CLIP_CHARACTER_PRECIS:
		    CheckRadioButton(hDlg, ID_CLIP_CHAR, ID_CLIP_DEFAULT,
			ID_CLIP_CHAR);
		    break;

		case CLIP_STROKE_PRECIS:
		    CheckRadioButton(hDlg, ID_CLIP_CHAR, ID_CLIP_DEFAULT,
			ID_CLIP_STROKE);
		    break;

		case CLIP_DEFAULT_PRECIS:
		    CheckRadioButton(hDlg, ID_CLIP_CHAR, ID_CLIP_DEFAULT,
			ID_CLIP_DEFAULT);
		    break;
	    }

	    switch (CLogFont.lfQuality) {
		case PROOF_QUALITY:
		    CheckRadioButton(hDlg, ID_PROOF, ID_DEF_QUALITY, ID_PROOF);
		    break;

		case DRAFT_QUALITY:
		    CheckRadioButton(hDlg, ID_PROOF, ID_DEF_QUALITY, ID_DRAFT);
		    break;

		case DEFAULT_QUALITY:
		    CheckRadioButton(hDlg, ID_PROOF, ID_DEF_QUALITY,
			ID_DEF_QUALITY);
		    break;
	    }

	    switch ((CLogFont.lfPitchAndFamily) & 3) {
		case FIXED_PITCH:
		    CheckRadioButton(hDlg, ID_FIXED, ID_DEF_PITCH, ID_FIXED);
		    break;

		case VARIABLE_PITCH:
		    CheckRadioButton(hDlg, ID_FIXED, ID_DEF_PITCH, ID_VARIABLE);
		    break;

		case DEFAULT_PITCH:
		    CheckRadioButton(hDlg, ID_FIXED, ID_DEF_PITCH,
			ID_DEF_PITCH);
		    break;
	    }

	    switch ((CLogFont.lfPitchAndFamily) & 240) {
		case FF_ROMAN:
		    CheckRadioButton(hDlg, ID_ROMAN, ID_DEF_FAMILY, ID_ROMAN);
		    break;

		case FF_SWISS:
		    CheckRadioButton(hDlg, ID_ROMAN, ID_DEF_FAMILY, ID_SWISS);
		    break;

		case FF_MODERN:
		    CheckRadioButton(hDlg, ID_ROMAN, ID_DEF_FAMILY, ID_MODERN);
		    break;

		case FF_SCRIPT:
		    CheckRadioButton(hDlg, ID_ROMAN, ID_DEF_FAMILY, ID_SCRIPT);
		    break;

		case FF_DECORATIVE:
		    CheckRadioButton(hDlg, ID_ROMAN, ID_DEF_FAMILY, ID_DECO);
		    break;

		case FF_DONTCARE:
		    CheckRadioButton(hDlg, ID_ROMAN, ID_DEF_FAMILY,
			ID_DEF_FAMILY);
		    break;
	    }
	    break;

	case WM_COMMAND:
	    switch (wParam) {
		case IDOK:
		    CLogFont.lfHeight = GetDlgItemInt(hDlg,
			ID_HEIGHT, NULL, TRUE);
		    CLogFont.lfWidth = GetDlgItemInt(hDlg,
			ID_WIDTH, NULL, TRUE);
		    CLogFont.lfEscapement = GetDlgItemInt(hDlg,
			ID_ESCAPEMENT, NULL, FALSE);
		    CLogFont.lfOrientation = GetDlgItemInt(hDlg,
			ID_ORIENTATION, NULL, FALSE);
		    GetDlgItemText(hDlg, ID_FACE, CLogFont.lfFaceName, 32);
		    CLogFont.lfWeight = GetDlgItemInt(hDlg,
			ID_WEIGHT, NULL, FALSE);
		    CLogFont.lfCharSet = GetDlgItemInt(hDlg,
			ID_CHARSET, NULL, FALSE);
		    EndDialog(hDlg, 1);
		    break;

		case IDCANCEL:
		    EndDialog(hDlg, 0);
		    break;

		case ID_ITALIC:
		    CLogFont.lfItalic = IsDlgButtonChecked(hDlg, ID_ITALIC);
		    break;

		case ID_UNDERLINE:
		    CLogFont.lfUnderline = IsDlgButtonChecked(hDlg,
			ID_UNDERLINE);
		    break;

		case ID_STRIKEOUT:
		    CLogFont.lfStrikeOut = IsDlgButtonChecked(hDlg,
			ID_STRIKEOUT);
		    break;

		case ID_LIGHT:
		    SetDlgItemInt(hDlg, ID_WEIGHT, CLogFont.lfWeight, TRUE);
		    CheckRadioButton(hDlg, ID_LIGHT, ID_BOLD, ID_LIGHT);
		    CLogFont.lfWeight = FW_LIGHT;
		    break;

		case ID_NORMAL:
		    SetDlgItemInt(hDlg, ID_WEIGHT, CLogFont.lfWeight, TRUE);
		    CheckRadioButton(hDlg, ID_LIGHT, ID_BOLD, ID_NORMAL);
		    CLogFont.lfWeight = FW_NORMAL;
		    break;

		case ID_BOLD:
		    SetDlgItemInt(hDlg, ID_WEIGHT, CLogFont.lfWeight, TRUE);
		    CheckRadioButton(hDlg, ID_LIGHT, ID_BOLD, ID_BOLD);
		    CLogFont.lfWeight = FW_BOLD;
		    break;

		case ID_WEIGHT:
		    CheckDlgButton(hDlg, ID_LIGHT, FALSE);
		    CheckDlgButton(hDlg, ID_NORMAL, FALSE);
		    CheckDlgButton(hDlg, ID_BOLD, FALSE);
		    break;

		case ID_ANSI:
		    SetDlgItemInt(hDlg, ID_CHARSET, CLogFont.lfCharSet, TRUE);
		    CheckRadioButton(hDlg, ID_ANSI, ID_OEM, ID_ANSI);
		    break;

		case ID_OEM:
		    SetDlgItemInt(hDlg, ID_CHARSET, CLogFont.lfCharSet, TRUE);
		    CheckRadioButton(hDlg, ID_ANSI, ID_OEM, ID_OEM);
		    break;

		case ID_CHARSET:
		    CheckDlgButton(hDlg, ID_ANSI, FALSE);
		    CheckDlgButton(hDlg, ID_OEM, FALSE);
		    break;

		case ID_OUT_STRING:
		    CheckRadioButton(hDlg, ID_OUT_STRING, ID_OUT_DEFAULT,
			ID_OUT_STRING);
		    CLogFont.lfOutPrecision = OUT_STRING_PRECIS;
		    break;

		case ID_OUT_CHAR:
		    CheckRadioButton(hDlg, ID_OUT_STRING, ID_OUT_DEFAULT,
			ID_OUT_CHAR);
		    CLogFont.lfOutPrecision = OUT_CHARACTER_PRECIS;
		    break;

		case ID_OUT_STROKE:
		    CheckRadioButton(hDlg, ID_OUT_STRING, ID_OUT_DEFAULT,
			ID_OUT_STROKE);
		    CLogFont.lfOutPrecision = OUT_STROKE_PRECIS;
		    break;

		case ID_OUT_DEFAULT:
		    CheckRadioButton(hDlg, ID_OUT_STRING, ID_OUT_DEFAULT,
			ID_OUT_DEFAULT);
		    CLogFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
		    break;

		case ID_CLIP_CHAR:
		    CheckRadioButton(hDlg, ID_CLIP_CHAR, ID_CLIP_DEFAULT,
			ID_CLIP_CHAR);
		    CLogFont.lfClipPrecision = CLIP_CHARACTER_PRECIS;
		    break;

		case ID_CLIP_STROKE:
		    CheckRadioButton(hDlg, ID_CLIP_CHAR, ID_CLIP_DEFAULT,
			ID_CLIP_STROKE);
		    CLogFont.lfClipPrecision = CLIP_STROKE_PRECIS;
		    break;

		case ID_CLIP_DEFAULT:
		    CheckRadioButton(hDlg, ID_CLIP_CHAR, ID_CLIP_DEFAULT,
			ID_CLIP_DEFAULT);
		    CLogFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		    break;

		case ID_PROOF:
		    CheckRadioButton(hDlg, ID_PROOF, ID_DEF_QUALITY, ID_PROOF);
		    CLogFont.lfQuality = PROOF_QUALITY;
		    break;

		case ID_DRAFT:
		    CheckRadioButton(hDlg, ID_PROOF, ID_DEF_QUALITY, ID_DRAFT);
		    CLogFont.lfQuality = DRAFT_QUALITY;
		    break;

		case ID_DEF_QUALITY:
		    CheckRadioButton(hDlg, ID_PROOF, ID_DEF_QUALITY,
			ID_DEF_QUALITY);
		    CLogFont.lfQuality = DEFAULT_QUALITY;
		    break;

		case ID_FIXED:
		    CheckRadioButton(hDlg, ID_FIXED, ID_DEF_PITCH, ID_FIXED);
		    CLogFont.lfPitchAndFamily =
			(~3 & CLogFont.lfPitchAndFamily) | FIXED_PITCH;
		    break;

		case ID_VARIABLE:
		    CheckRadioButton(hDlg, ID_FIXED, ID_DEF_PITCH, ID_VARIABLE);
		    CLogFont.lfPitchAndFamily =
			(~3 & CLogFont.lfPitchAndFamily) | VARIABLE_PITCH;
		    break;

		case ID_DEF_PITCH:
		    CheckRadioButton(hDlg, ID_FIXED, ID_DEF_PITCH,
			ID_DEF_PITCH);
		    CLogFont.lfPitchAndFamily =
			(~3 & CLogFont.lfPitchAndFamily) | DEFAULT_PITCH;
		    break;

		case ID_ROMAN:
		    CheckRadioButton(hDlg, ID_ROMAN, ID_DEF_FAMILY, ID_ROMAN);
		    CLogFont.lfPitchAndFamily =
			(~240 & CLogFont.lfPitchAndFamily) | FF_ROMAN;
		    break;

		case ID_SWISS:
		    CheckRadioButton(hDlg, ID_ROMAN, ID_DEF_FAMILY, ID_SWISS);
		    CLogFont.lfPitchAndFamily =
			(~240 & CLogFont.lfPitchAndFamily) | FF_SWISS;
		    break;

		case ID_MODERN:
		    CheckRadioButton(hDlg, ID_ROMAN, ID_DEF_FAMILY, ID_MODERN);
		    CLogFont.lfPitchAndFamily =
			(~240 & CLogFont.lfPitchAndFamily) | FF_MODERN;
		    break;

		case ID_SCRIPT:
		    CheckRadioButton(hDlg, ID_ROMAN, ID_DEF_FAMILY, ID_SCRIPT);
		    CLogFont.lfPitchAndFamily =
			(~240 & CLogFont.lfPitchAndFamily) | FF_SCRIPT;
		    break;

		case ID_DECO:
		    CheckRadioButton(hDlg, ID_ROMAN, ID_DEF_FAMILY, ID_DECO);
		    CLogFont.lfPitchAndFamily =
			(~240 & CLogFont.lfPitchAndFamily) | FF_DECORATIVE;
		    break;

		case ID_DEF_FAMILY:
		    CheckRadioButton(hDlg, ID_ROMAN, ID_DEF_FAMILY,
			ID_DEF_FAMILY);
		    CLogFont.lfPitchAndFamily =
			(~240 & CLogFont.lfPitchAndFamily) | FF_DONTCARE;
		    break;

	    }
	    break;
       }
       return (FALSE);
}
