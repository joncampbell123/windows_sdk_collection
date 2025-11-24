// chartdlg.cpp : Defines the behaviors for the Entry and print abort
//                dialogs.  The Entry dialog layout is defined in 
//                entry.dlg; print abort is defined in chart.rc 
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
//

#include "chart.h"

#include <ctype.h>


static char szFormat[] = "%5d %s";
static int nFormatSplit = 5;
static BOOL bChange;

#define MAXSTRINGLEN    200

/////////////////////////////////////////////////////////////////////////////
// CEntryDialog

BEGIN_MESSAGE_MAP(CEntryDialog, CModalDialog)
	ON_BN_CLICKED(BTN_ADD, OnBtnAdd)
	ON_BN_CLICKED(BTN_DEL, OnBtnDel)
	ON_LBN_SELCHANGE(LIST_BOX, OnListSelChange)
END_MESSAGE_MAP()

// OnInitDialog:
// Initialize the data entry dialog.  If there's already chart data,
// stuff it into the dialog.
// Note: This member function should not be put into the dialog
// class message map -- it is called automatically by the CModalDialog
// class code.  This is a special case.
//
BOOL CEntryDialog::OnInitDialog()
{
	// Call base class' for proper initialization.
	//
	if (!CModalDialog::OnInitDialog())
	{
		return FALSE;
	}
	  
	bChange = FALSE;

	// Initial conditions -- add items but don't delete them
	//
	GetDlgItem(BTN_ADD)->SetWindowText("Add");
	GetDlgItem(BTN_DEL)->EnableWindow(FALSE);

	// If there's pre-existing data, stuff it into the dialog
	//
	if (m_pData != NULL)
	{
		ASSERT(m_pData->m_pChartData != NULL);
		CObList* pData = m_pData->m_pChartData;

		// Insert the title here.
		//
		CEdit* pTitle  = (CEdit*) GetDlgItem(EDIT_TITLE);
		pTitle->SetSel(0, -1);
		pTitle->ReplaceSel(m_pData->m_Title);

		int nItems = pData->GetCount();
		if (nItems > 0)
		{        
			// Now fill in the list box control.
			//
			CListBox* pListBox = (CListBox*) GetDlgItem(LIST_BOX);
			pListBox->ResetContent();

			POSITION pos = pData->GetHeadPosition();

			for (int i = 0; i < nItems; i++)
			{
				CChartData* ptr;
				char szValue[MAXSTRINGLEN];

				ptr = (CChartData*)pData->GetNext(pos);
				sprintf(szValue, szFormat, ptr->height, ptr->szName);

				pListBox->AddString(szValue);
			}
		}
	}

	return TRUE;
}

// DoModal:
// While we're running this dialog, the member variable m_pData should
// point to the user's data.
//
void CEntryDialog::DoModal(CChartObject* pData)
{
	m_pData = pData;
	CModalDialog::DoModal();  
	m_pData = NULL;
}

// OnListSelChange:
// The selection has changed; update edit fields with the new selection's
// height and label.
// 
void CEntryDialog::OnListSelChange()
{
	int nLength;
	char szLabel[MAXSTRINGLEN];

	CListBox* pListBox = (CListBox*) GetDlgItem(LIST_BOX);

	m_nIndex = pListBox->GetCurSel();

	if (m_nIndex == LB_ERR)
	{
		m_nIndex = -1;
		return;
	}

	nLength = pListBox->GetText(m_nIndex, szLabel);
	szLabel[nLength] = '\0';

	szLabel[nFormatSplit] = '\0';
	GetDlgItem(EDIT_VALUE)->SetWindowText(szLabel);
	GetDlgItem(EDIT_LABEL)->SetWindowText(&szLabel[nFormatSplit+1]);

	bChange = TRUE;
	GetDlgItem(BTN_ADD)->SetWindowText("Change");
	GetDlgItem(BTN_DEL)->EnableWindow();
}

// OnBtnAdd:
// Transfer a new (height, label) from the edit fields to the listbox.
// Validate height before transfer
//
void CEntryDialog::OnBtnAdd()
{
	int nLabLength, nLength;
	char szValue[MAXSTRINGLEN];
	char szLabel[MAXSTRINGLEN];
	char szTitle[MAXSTRINGLEN];

	// valid #'s are ints >= 0

	BOOL bValid = FALSE;

	int iValue = GetDlgItemInt(EDIT_VALUE, &bValid, TRUE);

	if (bValid && (iValue >= 0))
	{
		SetDlgItemInt(EDIT_VALUE, iValue, FALSE);
	}
	else
	{
		MessageBox("An invalid value has been entered.", "Chart",
			MB_ICONEXCLAMATION | MB_OK);
		return;
	}

	CWnd* pItem;
	pItem = GetDlgItem(EDIT_LABEL);

	nLabLength = pItem->GetWindowTextLength();

	if (nLabLength >= MAXSTRINGLEN)
	{
		nLabLength = MAXSTRINGLEN-1;
	}

	pItem->GetWindowText(szLabel, nLabLength+1);

	szLabel[nLabLength] = '\0';

	pItem = GetDlgItem(EDIT_TITLE);
	nLength = pItem->GetWindowTextLength();

	if (nLength >= MAXSTRINGLEN)
	{
		nLength = MAXSTRINGLEN-1;
	}

	pItem->GetWindowText(szTitle, nLength+1);

	szTitle[nLength] = '\0';

	m_pData->m_Title = szTitle;
	sprintf(szValue, szFormat, iValue, szLabel);

	CListBox* pListBox = (CListBox*) GetDlgItem(LIST_BOX);

	if (!bChange)
	{
		pListBox->AddString(szValue);
	}
	else
	{
		pListBox->DeleteString(m_nIndex);
		pListBox->InsertString(m_nIndex, szValue);

		bChange = FALSE;
		GetDlgItem(BTN_ADD)->SetWindowText("Add");
		GetDlgItem(BTN_DEL)->EnableWindow(FALSE);
		
		// Now no item is selected
		m_nIndex = -1;
		pListBox->SetCurSel(m_nIndex);
	}

	m_pData->m_bDirty = TRUE;

	ClearEditBoxes();
}

// OnBtnDel:
// Delete the currently selected entry
//
void CEntryDialog::OnBtnDel()
{
	if (bChange)
	{
		CListBox* pListBox = (CListBox*) GetDlgItem(LIST_BOX);
		pListBox->DeleteString(m_nIndex);

		// Now no item is selected
		m_nIndex = -1;
		pListBox->SetCurSel(m_nIndex);

		bChange = FALSE;
		GetDlgItem(BTN_ADD)->SetWindowText("Add");
		GetDlgItem(BTN_DEL)->EnableWindow(FALSE);
	}

	m_pData->m_bDirty = TRUE;

	ClearEditBoxes();
}

// OnOK:
// Transfer array data from the dialog to the app chart object
//
void CEntryDialog::OnOK()
{
	if (!SetupArrayStructure())
		 return;

	CModalDialog::OnOK();
}

// SetupArrayStructure:
//
BOOL CEntryDialog::SetupArrayStructure()
{
	short nCount, nLength, i;
	char szBuffer[MAXSTRINGLEN];
	CWnd* pItem;

	m_pData->RemoveAll();

	pItem = GetDlgItem(EDIT_TITLE);
	nLength = pItem->GetWindowTextLength();

	if (nLength >= MAXSTRINGLEN)
	{
		nLength = MAXSTRINGLEN-1;
	}

	pItem->GetWindowText(szBuffer, nLength+1);
	szBuffer[nLength] = '\0';

	if (m_pData->m_Title != szBuffer)
	{
		m_pData->m_Title = szBuffer;
		m_pData->m_bDirty = TRUE;
	}


	CListBox* pListBox = (CListBox*) GetDlgItem(LIST_BOX);
	nCount = pListBox->GetCount();

	if (nCount == 0)
	{
		MessageBox("Warning: no values in chart.","Chart");
	}
	else
	{
		for (i = 0; i < nCount; i++)
		{
			CChartData* ptr;

			ptr = new CChartData;
			pListBox->GetText(i, szBuffer);

			szBuffer[nFormatSplit] = '\0';

			ptr->height = atoi(szBuffer);
			strcpy(ptr->szName, &szBuffer[nFormatSplit+1]);

			m_pData->m_pChartData->AddTail(ptr);
		}
	}

	return TRUE;
}

// ClearEditBoxes:
//
void CEntryDialog::ClearEditBoxes()
{
	CEdit* pEditValue = (CEdit*) GetDlgItem(EDIT_VALUE);

	// Select entire field contents
	pEditValue->SetSel(0, -1);
	pEditValue->Clear();

	CEdit* pEditLabel = (CEdit*) GetDlgItem(EDIT_LABEL);
	pEditLabel->SetSel(0, -1);
	pEditLabel->Clear();

	pEditValue->SetFocus();
}

////////////////////////////////////////////////////////////////
// CPrintDlgBox
// Modeless print abort dialog box
//

BEGIN_MESSAGE_MAP(CPrintDlgBox, CDialog)
	ON_COMMAND(IDCANCEL, OnCancel)
END_MESSAGE_MAP()

CPrintDlgBox::CPrintDlgBox()
{
	// Dialog defined in chart.rc
	//
	Create("PrintDlgBox");
}

// OnInitDialog:
// Disable this dialog box's system menu 'Close' item so user can't 
// dismiss this dialog
//
BOOL CPrintDlgBox::OnInitDialog()
{
	GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_GRAYED);
	return TRUE;
}

// OnCancel:
// User hit the cancel button; re-enable the main frame window
// (disabled elsewhere)
//
void CPrintDlgBox::OnCancel()
{
	extern BOOL bUserAbort;

	bUserAbort = TRUE;
	GetParent()->EnableWindow(TRUE);
	EndDialog(0);
}

