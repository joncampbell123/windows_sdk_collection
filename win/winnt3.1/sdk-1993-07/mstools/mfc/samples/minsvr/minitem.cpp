// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "minsvr.h"

/////////////////////////////////////////////////////////////////////////////

CMinItem::CMinItem()
{
	m_data = "example string";
}

void CMinItem::Serialize(CArchive& ar)
{
	// Customize this to store real data
	if (ar.IsStoring())
	{
		ar << m_data;
	}
	else
	{
		ar >> m_data;
	}
}


OLESTATUS CMinItem::OnShow(BOOL /* bTakeFocus */)
{
	// make sure server is the topmost window
	AfxGetApp()->m_pMainWnd->BringWindowToTop();
	return OLE_OK;
}


BOOL CMinItem::OnGetTextData(CString& rStringReturn)
{
	rStringReturn = m_data;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Simple dialog for changing the string

class CChangeDlg : public CModalDialog
{
protected:
	CString&    m_rString;

public:
	CChangeDlg(CString& rString)
		: CModalDialog("ChangeDlg"), m_rString(rString)
			{ }

	BOOL OnInitDialog();
	void OnOK();
};

BOOL CChangeDlg::OnInitDialog()
{
	GetDlgItem(IDC_EDIT1)->SetWindowText(m_rString);
	return TRUE;
};

void CChangeDlg::OnOK()
{
	GetDlgItem(IDC_EDIT1)->GetWindowText(m_rString);
	EndDialog(IDOK);
}


BOOL CMinItem::PromptChangeString()
{
	CChangeDlg dlg(m_data);
	return (dlg.DoModal() == IDOK);
}

/////////////////////////////////////////////////////////////////////////////
// Drawing items into bitmap or metafile

BOOL CMinItem::OnDraw(CMetaFileDC* pDC)
{
	ASSERT_VALID(pDC);
	CSize textSize;

	// first calculate the text size in MM_TEXT units
	{
		CWindowDC screenDC(NULL);
		textSize = screenDC.GetTextExtent(m_data, m_data.GetLength());
	}

	// if you want the item to always be drawn in a specific mapping
	//  mode set it here.

	// Otherwise the OLE DLLs will scale the metafile to fit the
	//  client specified size.  Setting the viewport size/extent
	//  determines the relative scale of everything.

	int cx = textSize.cx + 100;
	int cy = (cx * 4) / 3;      // nice aspect ratio
	TRACE("Item drawing size is %d x %d\n", cx, cy);
	pDC->SetWindowExt(cx, cy);

	// Draw a shaded circle
	pDC->SelectStockObject(LTGRAY_BRUSH);
	pDC->Ellipse(0, 0, cx, cy);

	// draw the text in the middle (as best we can)
	pDC->SetBkMode(TRANSPARENT);
	pDC->TextOut((cx - textSize.cx) / 2, (cy - textSize.cy) / 2, m_data);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
