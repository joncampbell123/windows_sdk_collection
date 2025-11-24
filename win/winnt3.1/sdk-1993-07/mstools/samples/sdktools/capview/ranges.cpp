#include        <afxwin.h>
#include        <afxdlgs.h>
#include        <afxcoll.h>

#include "resource.h"
#include "ranges.h"

/****************************** SPLIT Range Dialog ********************/

class SplitDlg : public CModalDialog
{
private:
    DWORD       c1;
    DWORD       c2;
    int         bottom;
    int         top;
    int         split;
public:
    SplitDlg(CWnd * pWnd, int bot, int top1, DWORD clr)
	: CModalDialog(IDD_SPLIT_DLG, pWnd)
    {
	split = bottom = bot;
	top = top1;
	c1 = c2 = clr;
	return;
    }

    int SplitValue() { return split; }
    DWORD Color1() { return c1; }
    DWORD Color2() { return c2; }
    
    BOOL OnInitDialog();
    void OnOK();
    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(SplitDlg, CModalDialog)
END_MESSAGE_MAP()

BOOL SplitDlg::OnInitDialog()
{
    SetDlgItemInt(IDD_SPLIT_BOTTOM, bottom);
    SetDlgItemInt(IDD_SPLIT_TOP, top);
    SetDlgItemInt(IDD_SPLIT_VALUE, bottom);
    return TRUE;
}

void SplitDlg::OnOK()
{
    split = GetDlgItemInt(IDD_SPLIT_VALUE);
    EndDialog(IDOK);
}

/******************************* RangesDlg ************************/

class RangeDlg : public CModalDialog
{
private:
    RANGESTRUCT * prs;
    char *        pch[MAX_NUM_RANGES];
    int           iCurr;
    DWORD         clr;
    
    void FormatLine(int iLine);
	
public:
    RangeDlg(CWnd * pWnd, RANGESTRUCT * rs)
	: CModalDialog("RangeDlg", pWnd)
    {
	for (int i=0; i<MAX_NUM_RANGES; i++) pch[i] = NULL;
	prs = rs;
    }
    
    ~RangeDlg()
    {
	for (int i=0; i<MAX_NUM_RANGES; i++)
	if (pch[i] != NULL) free(pch[i]);
    }
    
    int GetAbove() {
	return GetDlgItemInt(ID_RANGE_ABOVE);
    }
    
    void SetAbove(int i) {
	SetDlgItemInt(ID_RANGE_ABOVE, i);
	return;
    }
    int GetBelow() {
	return GetDlgItemInt(ID_RANGE_BELOW);
    }
    
    void SetBelow(int i) {
	SetDlgItemInt(ID_RANGE_BELOW, i);
	return;
    }
    
    int GetColor() {
	return GetCheckedRadioButton(ID_RANGE_BLACK, ID_RANGE_WHITE)
		- ID_RANGE_BLACK;
    }
    
    void SetColor(int i) {
	CheckRadioButton(ID_RANGE_BLACK, ID_RANGE_WHITE, 
	ID_RANGE_BLACK + i);
	return;
    }
    
    BOOL OnInitDialog();
    void OnOK();

    afx_msg void OnDrawItem(LPDRAWITEMSTRUCT lpdis);    
    afx_msg void OnMeasureItem(LPMEASUREITEMSTRUCT lpmis);
    afx_msg void OnColorButton();
    afx_msg void OnChangeButton();
    afx_msg void OnSplitButton();
    afx_msg void OnDeleteButton();
    afx_msg void OnHelp();
	    
    DECLARE_MESSAGE_MAP()
};
BEGIN_MESSAGE_MAP(RangeDlg, CModalDialog)
    ON_WM_DRAWITEM()
    ON_WM_MEASUREITEM()

    ON_COMMAND(ID_RANGE_ADD, OnSplitButton)
    ON_COMMAND(ID_RANGE_CHANGE, OnChangeButton)
    ON_COMMAND(ID_RANGE_COLOR, OnColorButton)
    ON_COMMAND(ID_RANGE_DELETE, OnDeleteButton)
    ON_COMMAND(IDHELP, OnHelp)
END_MESSAGE_MAP()

void RangeDlg::FormatLine(int i)
{
    char rgch[256];
    
    if (pch[i] != NULL) free(pch[i]);
    if (i == 0) {
	sprintf(rgch, "Below %d", prs->Above[i+1]);
    } else if (i == prs->cRanges-1) {
	sprintf(rgch, "Above %d%%", prs->Above[i]);
    } else {
	sprintf(rgch, "%d%% - %d%%", prs->Above[i], prs->Above[i+1]);
    }
    pch[i] = strdup(rgch);
    return;
}

BOOL RangeDlg::OnInitDialog()
{
    int  i;
   
    for (i=0; i<prs->cRanges; i++) {
	FormatLine(i);
	SendDlgItemMessage(ID_RANGE_LISTBOX, LB_ADDSTRING, 0, (LPARAM) pch[i]);
    }
   
   iCurr = 0;
   SetAbove(prs->Above[1]);
   SetBelow(prs->Above[0]);
   SetColor(prs->TextColor[0]);
   clr = prs->BackColor[0];
   
   SendDlgItemMessage(ID_RANGE_PRUNE, BM_SETCHECK, prs->fPrune, 0);
   return TRUE;
}

void RangeDlg::OnOK()
{
    prs->fPrune = SendDlgItemMessage(ID_RANGE_PRUNE, BM_GETCHECK, 0, 0);
    EndDialog(IDOK);
}

int DoRangesDlg(CWnd * pWnd, RANGESTRUCT * rs)
{
    RangeDlg dlg(pWnd, rs);
    return dlg.DoModal();
}


afx_msg void RangeDlg::OnMeasureItem(LPMEASUREITEMSTRUCT lpmis)
{
    lpmis->itemWidth = 100;
    lpmis->itemHeight = 20;
    
    return;
}

afx_msg void RangeDlg::OnDrawItem(LPDRAWITEMSTRUCT lpdis)
{
    switch( lpdis->itemAction ) {
      case ODA_DRAWENTIRE:
	if (lpdis->itemID != -1) {
	    HBRUSH hBr;
	    hBr = CreateSolidBrush( prs->BackColor[lpdis->itemID] );
	    FillRect(lpdis->hDC, &lpdis->rcItem, hBr);
	    DeleteObject( hBr );
	    SetBkMode(lpdis->hDC, TRANSPARENT);
	    SetTextColor(lpdis->hDC, prs->TextColor[lpdis->itemID] * RGB(255, 255, 255));
	    DrawText(lpdis->hDC, (char *) lpdis->itemData, -1, &lpdis->rcItem, 
		DT_NOCLIP|DT_EXPANDTABS|DT_VCENTER|DT_SINGLELINE);
	}
	break;
	
      case ODA_SELECT:
	if (lpdis->itemState & ODS_SELECTED) {
	    iCurr = lpdis->itemID;
	    SetAbove(prs->Above[iCurr+1]);
	    SetBelow(prs->Above[iCurr]);
	    SetColor(prs->TextColor[iCurr]);
	    clr = prs->BackColor[iCurr];
	} else {
	}
	break;
	
      case ODA_FOCUS:
	if (lpdis->itemState & ODS_FOCUS) {
	} else {
	}
	break;
	
      default:
	return;
    }
    return;
}

afx_msg void RangeDlg::OnChangeButton()
{
    if (iCurr > 0) {
	prs->Above[iCurr] = GetBelow();
    }
    
    if (iCurr < prs->cRanges) {
	prs->Above[iCurr+1] = GetAbove();
    }
    
    prs->TextColor[iCurr] = GetColor();
    prs->BackColor[iCurr] = clr;
    
    SendDlgItemMessage(ID_RANGE_LISTBOX, LB_DELETESTRING, iCurr, 0);
    FormatLine(iCurr);
    SendDlgItemMessage(ID_RANGE_LISTBOX, LB_INSERTSTRING, iCurr, (LPARAM) pch[iCurr]);
    
    if (iCurr < prs->cRanges) {
	SendDlgItemMessage(ID_RANGE_LISTBOX, LB_DELETESTRING, iCurr+1, 0);
	FormatLine(iCurr+1);
	SendDlgItemMessage(ID_RANGE_LISTBOX, LB_INSERTSTRING, iCurr+1, (LPARAM) pch[iCurr+1]);
    }
    
    return;
}

afx_msg void RangeDlg::OnColorButton()
{
    CColorDialog dlg(prs->BackColor[iCurr]);
    dlg.DoModal();
    prs->BackColor[iCurr] = dlg.GetColor();
    return;
}

afx_msg void RangeDlg::OnSplitButton()
{
    int i;

    SplitDlg dlg(NULL, prs->Above[iCurr], prs->Above[iCurr+1], prs->BackColor[iCurr]);
    if (dlg.DoModal()) {
	for (i=prs->cRanges; i>iCurr; i--) {
	    prs->Above[i] = prs->Above[i-1];
	    prs->BackColor[i] = prs->BackColor[i-1];
	    pch[i] = pch[i-1];
	}
    }
    pch[iCurr] = NULL;
    prs->cRanges += 1;
    prs->Above[iCurr+1] = dlg.SplitValue();
    prs->BackColor[iCurr] = dlg.Color1();
    prs->BackColor[iCurr+1] = dlg.Color2();

    FormatLine(iCurr);   
    SendDlgItemMessage(ID_RANGE_LISTBOX, LB_DELETESTRING, iCurr, 0);
    SendDlgItemMessage(ID_RANGE_LISTBOX, LB_INSERTSTRING, iCurr, (LPARAM) pch[iCurr]);
    FormatLine(iCurr+1);
    SendDlgItemMessage(ID_RANGE_LISTBOX, LB_INSERTSTRING, iCurr+1, (LPARAM) pch[iCurr+1]);
    return;
}

afx_msg void RangeDlg::OnDeleteButton()
{
    return;
}


afx_msg void RangeDlg::OnHelp()
{
    WinHelp(m_hWnd, "capview.hlp", HELP_CONTEXT, (DWORD) IDD_RANGE_DLG);
    return;
}
