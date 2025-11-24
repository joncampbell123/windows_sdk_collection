// mpprint.cpp : Defines the printing logic.
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

#include "multipad.h"

#include <commdlg.h>

#ifndef _NTWIN
#pragma code_seg("_MPPRINT")
#endif

CPrinter* thePrinter;
char BASED_CODE szExtDeviceMode[] = "EXTDEVICEMODE";

/////////////////////////////////////////////////////////////////////////////

// AbortProc:
// While printing, this replaces the normal message receiver loop.
// This provides simplified message routing until the modeless "Abort"
// dialog can be closed.
//
BOOL FAR PASCAL EXPORT AbortProc(HDC, int)
{
	MSG msg;
	
	// Allow other apps to run, while polling the abort status.
	//
	while (!thePrinter->fAbort &&
		PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
	{
		if (!thePrinter->hwndPDlg ||
			!IsDialogMessage(thePrinter->hwndPDlg, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	
	return !thePrinter->fAbort;
}

class CPrintCanDlg : public CDialog
{
public:
	CPrintCanDlg();
	
	BOOL OnInitDialog();
	afx_msg void OnCancel();
	
	DECLARE_MESSAGE_MAP();
};

BEGIN_MESSAGE_MAP(CPrintCanDlg, CDialog)
	ON_COMMAND(IDOK, OnCancel)
END_MESSAGE_MAP()

CPrintCanDlg::CPrintCanDlg()
{
	VERIFY( Create(IDD_PRINT) );
}

BOOL CPrintCanDlg::OnInitDialog()
{
	SetDlgItemText(IDD_PRINTDEVICE, thePrinter->printDlg.GetDeviceName());
	SetDlgItemText(IDD_PRINTPORT, thePrinter->printDlg.GetPortName());
	SetDlgItemText(IDD_PRINTTITLE, thePrinter->szTitle);
	return TRUE;
}

void CPrintCanDlg::OnCancel()
{
	thePrinter->fAbort = TRUE;
}

// StartJob:
// Prepare the printer DC and open the Cancel dialog, ready for printing.
// The application's frame is disabled, mostly to protect the data while
// printing is in progress.
//
BOOL CPrinter::StartJob(char* szDocName)
{
	char sz [32];

	fAbort = FALSE;
	fError = TRUE; // Assume an error until done.
	pdc = NULL;
	pdlg = NULL;
	
	// Create the job title by loading the title string from STRINGTABLE.
	//
	int cch = LoadString(AfxGetInstanceHandle(), IDS_PRINTJOB,
		sz, sizeof(sz));
	strncpy(sz + cch, szDocName, sizeof (sz) - cch - 1);
	sz[sizeof(sz)-1] = '\0';
	strncpy(szTitle, szDocName, sizeof (szTitle) - 1);
	szTitle[sizeof(szTitle)-1] = 0;
	
	// Use standard PrintDlg to get printer DC
	//
	//If DoModalPrint returns 0 then user canceled or an error happened.
	if ( printDlg.DoModal() == IDCANCEL)
		return FALSE;

	pdc = new CDC;
	ASSERT(printDlg.GetPrinterDC() != NULL);
	pdc->Attach(printDlg.GetPrinterDC());

	// Allow the app to inform GDI of the escape function to call.
	//
	if (pdc->SetAbortProc(AbortProc) < 0)
		goto printFailed;

	pdlg = new CPrintCanDlg;
	
	hwndPDlg = pdlg->m_hWnd;
	
	// Disable the main application window and create the Cancel dialog.
	//
	AfxGetApp()->m_pMainWnd->EnableWindow(FALSE);

	pdlg->ShowWindow(SW_SHOW);
	pdlg->UpdateWindow();
	
	// Initialize the document.
	//
	if (pdc->StartDoc(sz) < 0)
		goto printFailed;
	
	return TRUE;

	printFailed: 
		delete pdc;
		return FALSE;
}

// EndJob:
// Do a final page-eject, shut down the printer context, and re-enable the
// application.
//
void CPrinter::EndJob()
{
	if (pdc != NULL)
	{
		if (fAbort || pdc->EndPage() < 0 || pdc->EndDoc() < 0)
		{
			pdc->AbortDoc();
		}
		else
		{
			fError = FALSE;
		}
		
		delete pdc;
		pdc = NULL;
	}

	if (pdlg != NULL)
	{
		AfxGetApp()->m_pMainWnd->EnableWindow(TRUE);
		
		delete pdlg;
		pdlg = NULL;
		hwndPDlg = NULL;
		
	}
	
	// Error? Make sure the user knows.
	//
	if (fError)
		MPError(MB_OK | MB_ICONEXCLAMATION, IDS_PRINTERROR, (LPCSTR)szTitle);
}

/////////////////////////////////////////////////////////////////////////////

// PrintFile:
// This does all the work of printing the text buffer.
//
void CMPChild::PrintFile()
{
	int     yExtPage;
	UINT    cch;
	UINT    ich;
	PSTR    pch;
	UINT    iLine;
	UINT    nLinesEc;
	HANDLE  hT;
	UINT    dy;
	int     yExtSoFar;
	
	char szDocName[256];
	GetWindowText(szDocName, sizeof (szDocName));
	
	if ( !thePrinter->StartJob(szDocName) )
		return;
	
	dy = thePrinter->pdc->GetTextExtent("CC", 2).cy;
	yExtPage = thePrinter->pdc->GetDeviceCaps(VERTRES);
	
	// Get the lines in document and and a handle to the text buffer.
	//
	iLine = 0;
	yExtSoFar = 0;
	nLinesEc = m_edit.GetLineCount();
	hT = m_edit.GetHandle();

	// Prepare the driver to accept the first page of data
	if (thePrinter->pdc->StartPage() < 0 || thePrinter->fAbort)
		return;

	// While more lines, print out the text.
	//
	while (iLine < nLinesEc)
	{
		if (yExtSoFar + (int)dy > yExtPage)
		{
			// Reached the end of a page. Tell the device driver to eject a
			// page.
			//
			if (thePrinter->pdc->EndPage() < 0 || thePrinter->fAbort)
				break;
			
			yExtSoFar = 0;

			// Prepare the driver for the next page
			if (thePrinter->pdc->StartPage() < 0 || thePrinter->fAbort)
				break;
		}
		
		// Get the length and position of the line in the buffer
		// and lock from that offset into the buffer.
		//
		ich = m_edit.LineIndex(iLine);
		cch = m_edit.LineLength(ich);
		pch = (PSTR)LocalLock(hT) + ich;
		
		// Print the line and unlock the text handle.
		//
		thePrinter->pdc->TabbedTextOut(0, yExtSoFar, (LPCSTR)pch, cch,
			0, NULL, 0);
		LocalUnlock(hT);
		
		// Test and see if the Abort flag has been set. If yes, exit.
		//
		if (thePrinter->fAbort)
			break;
		
		// Move down the page.
		//
		yExtSoFar += dy;
		iLine++;
	}
	
	thePrinter->EndJob();
}
