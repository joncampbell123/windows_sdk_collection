// chartwnd.cpp : Defines the class behaviors for the Chart frame window.
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

#include <math.h>

#include <commdlg.h>

// hPrintDlg and bUserAbort are used to handle the print abort
// modeless dialog
//
static HWND hPrintDlg;
BOOL bUserAbort;

// CChartWnd static member variables that define the bounds of
// CChartWnd's custom anisotropic coordinate system
//
CRect CChartWnd::rectPage(0, 1000, 1000, 0);
CRect CChartWnd::rectData(150, 100, 850, 800);

static short nBlue, nGreen, nRed, nCurrentColor;

/////////////////////////////////////////////////////////////////////////////
// CChartWnd

BEGIN_MESSAGE_MAP(CChartWnd, CFrameWnd)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_CLOSE()

	ON_COMMAND(IDM_NEW, OnNew)
	ON_COMMAND(IDM_OPEN, CmdFileOpen)
	ON_COMMAND(IDM_SAVE, CmdFileSave)
	ON_COMMAND(IDM_SAVEAS, CmdFileSaveAs)
	ON_COMMAND(IDM_CHANGE, OnChange)
	ON_COMMAND(IDM_PRINT, OnPrint)
	ON_COMMAND(IDM_EXIT, OnClose)

	ON_COMMAND(IDM_BAR, OnBar)
	ON_COMMAND(IDM_LINE, OnLine)

	ON_COMMAND(IDM_ABOUT, OnAbout)
END_MESSAGE_MAP()

// Constructor:
//
CChartWnd::CChartWnd()
{
	m_bUntitled = TRUE;
	m_szFileName = "";
	m_bChartSerializedOK = FALSE;
	Create("Chart");
}

// Destructor:
//
CChartWnd::~CChartWnd()
{
	if (m_pChartObject != NULL)
	{
		delete m_pChartObject;  
		m_pChartObject = NULL;
	}
}

// Create:
// Load accelerator keys for this window and create a frame window.
// The frame window uses a custom window class because it has a
// non-standard icon.
// The accelerator table, main menu, and icon are defined in chart.rc.
//
BOOL CChartWnd::Create(LPCSTR szTitle,
	LONG style /* = WS_OVERLAPPEDWINDOW */,
	const RECT& rect /* = rectDefault */,
	CWnd* parent /* = NULL */)
{
	LoadAccelTable("MainAccelTable");

	const char* pszWndClass =
			AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW,
								NULL,
								(HBRUSH)(COLOR_WINDOW+1),
								LoadIcon(AfxGetInstanceHandle(), "chart"));

	return CFrameWnd::Create(pszWndClass, szTitle, style,
							 rect, parent, "MainMenu");
}

// OnCreate:
// Get cursors and initialize the main menu
//
int CChartWnd::OnCreate(LPCREATESTRUCT)
{
	m_pChartObject = NULL;
	m_hCross = LoadCursor(NULL, IDC_CROSS);
	m_hArrow = LoadCursor(NULL, IDC_ARROW);

	UpdateMenu();

	return 0;
}

// UpdateMenu:
// Update main menu item state based on chart state
//
void CChartWnd::UpdateMenu()
{
	CMenu* menu = GetMenu();

	UINT nData = (m_pChartObject? MF_ENABLED : MF_GRAYED);

	menu->EnableMenuItem(IDM_SAVEAS, nData);
	menu->EnableMenuItem(IDM_CHANGE, nData);
	menu->EnableMenuItem(IDM_PRINT,  nData);

	menu->EnableMenuItem(IDM_BAR,    nData);
	menu->EnableMenuItem(IDM_LINE,   nData);

	if (m_pChartObject != NULL)
	{
		nData = (m_pChartObject->m_nType == IDM_BAR);
		menu->CheckMenuItem(IDM_BAR,  nData? MF_CHECKED : MF_UNCHECKED);
		menu->CheckMenuItem(IDM_LINE, nData? MF_UNCHECKED : MF_CHECKED);
		menu->EnableMenuItem(IDM_SAVE,
							 m_bUntitled ? MF_GRAYED : MF_ENABLED);

					
	}
	else
	{
		menu->CheckMenuItem(IDM_BAR,  MF_UNCHECKED);
		menu->CheckMenuItem(IDM_LINE, MF_UNCHECKED);
		menu->EnableMenuItem(IDM_SAVE, MF_GRAYED);
	}

	DrawMenuBar();
}

// OnNew:
// Create a new empty chart
//
void CChartWnd::OnNew()
{
	if (m_pChartObject != NULL)
	{
		if (m_pChartObject->m_bDirty)
		{

			if (MessageBox("Save existing data?", "Chart",
							MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				SaveFile(m_bUntitled);
			}
		}

		delete m_pChartObject;
		m_pChartObject = NULL;
		m_bUntitled = TRUE;
		m_szFileName = "";
		m_pChartObject = NULL;
	}
	OnChange();
}

// OnChange:
// User wants to enter (or change) chart data.
//
void CChartWnd::OnChange()
{
	// Create data structure if there is none.
	//
	if (m_pChartObject == NULL)
	{
		m_pChartObject = new CChartObject();
	}

	// Get the data.
	//
	CEntryDialog entryDlg(this);
	entryDlg.DoModal(m_pChartObject);

	ASSERT(m_pChartObject->m_pChartData != NULL);

	if (m_pChartObject->m_pChartData->IsEmpty())
	{
		delete m_pChartObject;
		m_pChartObject = NULL;
	}

	// Update menu state based on the data
	//
	UpdateMenu();
	Invalidate(TRUE);
}

// OnPrint:
// User wants to print the chart
//
void CChartWnd::OnPrint()
{
	if (!DoPrint())
	{
		MessageBox("Not able to print chart.", "Chart",
		   MB_OK | MB_ICONEXCLAMATION);
	}
}

// OnBar:
// Make the chart a bar chart
//
void CChartWnd::OnBar()
{
	ASSERT(m_pChartObject != NULL);

	m_pChartObject->m_nType = IDM_BAR;

	UpdateMenu();
	Invalidate(TRUE);
}

// OnLine:
// Make the chart a line chart
//
void CChartWnd::OnLine()
{
	ASSERT(m_pChartObject != NULL);

	m_pChartObject->m_nType = IDM_LINE;

	UpdateMenu();
	Invalidate(TRUE);
}

// PrepareDC:
// Prepare a DC for drawing the chart
void CChartWnd::PrepareDC(CDC* pDC)
{
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(rectPage.right, rectPage.top);
	pDC->SetViewportExt(m_cxClient, -m_cyClient);
	pDC->SetViewportOrg(0, m_cyClient);
	pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
	pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
}

// OnMouseMove:
// We do some hit-testing here to make the cursor a crosshairs while inside
// the data graphic, and an arrow otherwise.
//
void CChartWnd::OnMouseMove(UINT, CPoint mousePos)
{
	if (m_pChartObject == NULL)
	{
		SetCursor(m_hArrow);
		return;
	}

	// We use a custom logical coordinate system.  0,0 is the bottom-left
	// corner of the client area, and 1000,1000 is the top-right.
	// Convert the mouse coordinates to this scheme.
	//
	CDC* pDC;
	pDC = GetDC();
	PrepareDC(pDC);
	pDC->DPtoLP(&mousePos, 1);

	if (rectData.PtInRect(mousePos))
	{
		SetCursor(m_hCross);
	}
	else
	{
		SetCursor(m_hArrow);
	}

	ReleaseDC(pDC);
}
	
// OnPaint:
// Paint the chart...
//
void CChartWnd::OnPaint()
{
	CPaintDC dc(this);
	CRect screenPos;

	GetClientRect(screenPos);

	m_cxClient = screenPos.Width();
	m_cyClient = screenPos.Height();

	if (m_pChartObject != NULL)
	{
		RenderChart(&dc);
	}
}

// OnClose:
// User picked 'Close' on system menu
//
void CChartWnd::OnClose()
{
	if (m_pChartObject != NULL)
	{
		if (m_pChartObject->m_bDirty)
		{
			int fResponse;

			fResponse = MessageBox("Save file before exit?", "Chart",
							MB_YESNOCANCEL | MB_ICONQUESTION);

			if (fResponse == IDCANCEL)
			{
				return;
			}
			else if (fResponse == IDYES)
			{
				SaveFile(m_bUntitled);
			}
		}
		delete m_pChartObject;
	}

	m_pChartObject = NULL;

	DestroyWindow();
}

// GetHighValue:
// Returns largest value in chart data.
//
short CChartWnd::GetHighValue()
{
	short i, count, nLargest, nCurrent;
	POSITION pos;
	CChartData* ptr;

	ASSERT(m_pChartObject != NULL);
	CObList* pChartData = m_pChartObject->m_pChartData;

	ASSERT(pChartData != NULL);

	ptr = (CChartData*)(pChartData->GetHead());

	nLargest = ptr->height;

	pos = pChartData->GetHeadPosition();
	count = pChartData->GetCount();
	for (i = 0; i < count; i++)
	{
		ptr = (CChartData*)pChartData->GetNext(pos);

		if ((nCurrent = ptr->height) > nLargest)
		{
			nLargest = nCurrent;
		}
	}

	return nLargest;
}

// SetNewColors:
// Progresses nCurrentColor through the possible colors.
//
void CChartWnd::SetNewColors()
{
	nCurrentColor++;

	ASSERT(m_pChartObject != NULL);
	ASSERT(m_pChartObject->m_pChartData != NULL);

	nCurrentColor %= (m_pChartObject->m_pChartData->GetCount()+1);

	nBlue = (nCurrentColor & 1) ? 255 : 0;
	nGreen = (nCurrentColor & 2) ? 255 : 0;
	nRed = (nCurrentColor & 4) ? 255 : 0;

}

// RenderChart:
// Draw the chart in a device context.  This routine handles both
// screen DCs and printer DCs.
//
void CChartWnd::RenderChart(CDC* pDC)
{
	int i, nSize, nTextHeight;
	float yTickMag, yTickGuess;
	char szBuffer[80];

	ASSERT(m_pChartObject != NULL);

	if (m_pChartObject->m_pChartData->IsEmpty())
	{
		return;
		// return if there are no entries in the list
	}

	// We use a custom logical coordinate system.  0,0 is the bottom-left
	// corner of the client area, and 1000,1000 is the top-right.
	//
	PrepareDC(pDC);

	// A rectangle around the chart.
	//
	pDC->Rectangle(rectData);

	// A blue dotted pen to draw the grid on the chart.
	//
	CPen newPen(PS_DOT, 2, RGB(0, 0, 255));
	CPen* penOrig = pDC->SelectObject(&newPen);

	// Figure out a reasonable step size for the grid.
	//
	m_fTallest = GetHighValue();
	yTickGuess = m_fTallest / 10;
	yTickMag = (float)(((log10(yTickGuess) / log10(2.7182818))) /
						(log10(10.0) / log10(2.7182818)));
	yTickGuess = (float)(((yTickGuess / pow(10.0, (yTickMag - 1.0)) + 0.5)
		/ 10.0) * pow(10.0, yTickMag));

	// Draw grid.
	//
	m_fTallest = yTickGuess * 10;
	int iTickDelta = rectData.Height()/10;

	for (i = 1; i <= 10; i++)
	{
		if (i != 10)
		{       
			pDC->MoveTo(rectData.left, rectData.top + (iTickDelta*i));
			pDC->LineTo(rectData.right, rectData.top + (iTickDelta*i));
		}

		sprintf(szBuffer,"%3.2f", (yTickGuess*i));

		nSize = pDC->GetTextExtent(szBuffer,strlen(szBuffer)).cx;
		nTextHeight = pDC->GetTextExtent(szBuffer, strlen(szBuffer)).cy;
		if (((nSize+25) < rectData.left) && (nTextHeight < iTickDelta))
		{
			pDC->TextOut((125-nSize),
						 rectData.top+(iTickDelta*i)+(nTextHeight/2),
						szBuffer, strlen(szBuffer));
		}
	}

	// Done with the grid; re-select the original pen

	pDC->SelectObject(penOrig);

	// Create and select the brush to draw the chart data itself
	//
	CBrush* pOldBrush;
	CBrush newBrush(RGB(nRed, nGreen, nBlue));
	pOldBrush = pDC->SelectObject(&newBrush);

	switch (m_pChartObject->m_nType)
	{
	case IDM_LINE:
		DrawLineChart(pDC);
		break;

	case IDM_BAR:
		DrawBarChart(pDC);
		break;
	}

	// Delete the brush, after selecting the old one back in the DC.
	//
	pDC->SelectObject(pOldBrush);

	// Draw the title, if there's room.
	//
	nSize = pDC->GetTextExtent(m_pChartObject->m_Title,
		m_pChartObject->m_Title.GetLength()).cx;

	if ((nSize/2) < 500)
	{
		pDC->TextOut((500 - (nSize/2)), 900, m_pChartObject->m_Title,
			m_pChartObject->m_Title.GetLength());
	}
}

// DrawBarChart:
// Render the chart as a bar chart
//
void CChartWnd::DrawBarChart(CDC* pDC)
{
	float flFraction;
	int nWidth, i, nTextHeight, nCurrentHeight, nSize;
	CRect rectBar;
	POSITION pos;
	CChartData* ptr;
	char szBuffer[80];

	ASSERT(m_pChartObject != NULL);
	ASSERT(m_pChartObject->m_pChartData != NULL);

	CObList* pChartData = m_pChartObject->m_pChartData;

	flFraction = (float)(rectData.Width() / (pChartData->GetCount()));
	nWidth = (short) flFraction;
	pos = pChartData->GetHeadPosition();

	nCurrentColor = 0;
	int cChart = pChartData->GetCount();

	for (i = 0; i < cChart; i++)
	{
		char szLabel[40];
		ptr = (CChartData*)pChartData->GetNext(pos);
		strcpy(szLabel, ptr->szName);
		int nNewHeight = ptr->height;

		// Calculate the size of the bar.
		//
		flFraction = nNewHeight / m_fTallest;
		nCurrentHeight = (short) (flFraction * rectData.Height());

		rectBar.left = rectData.left + (nWidth * i);
		rectBar.top = nCurrentHeight + rectData.top;
		rectBar.right = rectData.left + nWidth * (i+1);
		rectBar.bottom = rectBar.top - nCurrentHeight;

		SetNewColors();
		CBrush chartBrush(RGB(nRed, nGreen, nBlue));
		CBrush* oldBrush = pDC->SelectObject(&chartBrush);

		pDC->Rectangle(rectBar);

		sprintf(szBuffer, "%s", szLabel);
		nSize = pDC->GetTextExtent(szBuffer,strlen(szBuffer)).cx;
		nTextHeight = pDC->GetTextExtent(szBuffer, strlen(szBuffer)).cy;
		if (((nSize+20) < nWidth) && (nTextHeight < 95))
			pDC->TextOut(rectBar.left+(nWidth/2)-(nSize/2),
						 rectBar.bottom-5,
						 szBuffer, strlen(szBuffer));

		// Delete the brush, after selecting the old one back into the DC.
		//
		pDC->SelectObject(oldBrush);
	}
}

// DrawLineChart:
// Render the chart data as a line chart
//
void CChartWnd::DrawLineChart(CDC* pDC)
{
	int nWidth, nSize, nTextHeight, i;
	int nCurrentHeight, nBottom;

	CPoint ptStart;
	CPoint ptEnd;

	LONG nTotalHeight;
	float flFraction, flOffset;
	POSITION pos;
	CChartData* ptr;
	char szBuffer[80];

	ASSERT(m_pChartObject != NULL);

	CObList* pChartData = m_pChartObject->m_pChartData;

	ASSERT(pChartData != NULL);
	flFraction = (float)(rectData.Width() / (pChartData->GetCount()));
	nWidth = (short) flFraction;

	ptr = (CChartData*)pChartData->GetHead();
	ptStart.x = rectData.left + (nWidth/2);
	nTotalHeight = (LONG)(((LONG)ptr->height) * rectData.Height());
	flOffset = (nTotalHeight / m_fTallest);
	ptStart.y = rectData.top + (int)flOffset;
	pos = pChartData->GetHeadPosition();

	int cChart = pChartData->GetCount();
	for (i = 0; i < cChart; i++)
	{
		char szLabel[40];
		ptr = (CChartData*)pChartData->GetNext(pos);
		strcpy(szLabel, ptr->szName);
		int nNewHeight = ptr->height;

		flFraction = nNewHeight / m_fTallest;
		nCurrentHeight = (short) (flFraction * rectData.Height());

		ptEnd.x = nWidth/2 + rectData.left + nWidth*i;
		ptEnd.y = rectData.top + nCurrentHeight;
		nBottom = ptEnd.y - nCurrentHeight;

		pDC->MoveTo(ptStart);
		pDC->LineTo(ptEnd);

		ptStart = ptEnd;

		sprintf(szBuffer, "%s", szLabel);
		nSize = pDC->GetTextExtent(szBuffer, strlen(szBuffer)).cx;
		nTextHeight = pDC->GetTextExtent(szBuffer, strlen(szBuffer)).cy;
		if (((nSize+20) < nWidth) && (nTextHeight < 95))
		{
			pDC->TextOut((ptEnd.x-(nSize/2)), (nBottom-5),
				szBuffer, strlen(szBuffer));
		}
	}
}

// AbortProc:
// While printing, the Printing... dialog (PrintDlgBox in chart.rc) is
// displayed, which has a Cancel button on it.  This routine replaces the
// normal message-handling mechanism, until the printing is done or the
// Cancel button is pressed.
//
BOOL FAR PASCAL _export AbortProc(HDC, int)
{
	MSG msg;

	while(!bUserAbort && PeekMessage(&msg,NULL,0,0,PM_REMOVE))
	{
		if (!hPrintDlg || !IsDialogMessage(hPrintDlg, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return !bUserAbort;
}


// DoPrint: 
// Get a printer DC and render the chart on it.  Uses COMMDLG
// printer dialog.
//
BOOL CChartWnd::DoPrint()
{
	BOOL (FAR PASCAL _export * lpfnAbortProc)(HDC hPrinterDC, int nCode);
	static char szMessage[] = "Printing chart...";
	short xPage, yPage, oldX, oldY;
	BOOL bError = FALSE;
	CDC* pDC = NULL;

	lpfnAbortProc = AbortProc;

	CPrintDialog printDialog(FALSE);
	if (printDialog.DoModal() == IDCANCEL)
		return TRUE;

	pDC = new CDC;
	ASSERT(printDialog.GetPrinterDC() != NULL);
	pDC->Attach(printDialog.GetPrinterDC());

	xPage = pDC->GetDeviceCaps(HORZRES);
	yPage = pDC->GetDeviceCaps(VERTRES);

	if (pDC->SetAbortProc(lpfnAbortProc) < 0)
	{
		delete pDC;
		return FALSE;
	}

	// The chart main window has to be disabled while printing so that
	// the user can't change data.
	//
	EnableWindow(FALSE);

	// Set up the printer abort box and its window procedure control
	// variables
	//
	bUserAbort = FALSE;
	m_pPrintDlg = new CPrintDlgBox;
	hPrintDlg = m_pPrintDlg->m_hWnd;

	if (pDC->StartDoc(szMessage) >= 0 && pDC->StartPage() >= 0)
	{
		oldX = m_cxClient;
		m_cxClient = xPage;
		oldY = m_cyClient;
		m_cyClient = yPage;

		RenderChart(pDC);

		if (pDC->EndPage() >= 0)
		{
			pDC->EndDoc();
		}
		else
		{
			bError = TRUE;
		}
	}
	else
	{
		bError = TRUE;
	}

	if (bError)
		pDC->AbortDoc();

	m_cxClient = oldX;
	m_cyClient = oldY;

	// Now that we're done, we can now allow the user access to the frame
	// window again.  bUserAbort is set in the AbortProc just before this
	// function.
	//
	if (!bUserAbort)
	{
		EnableWindow(TRUE);
	}

	delete pDC;
	delete m_pPrintDlg;

	return !bError && !bUserAbort;
}

// CmdFileSave:
// User wants to save current chart data in the current file
//
void CChartWnd::CmdFileSave()
{
	if (!m_bUntitled)
	{
		SaveFile(FALSE);
		UpdateMenu();
	}
}

// CmdFileSaveAs:
// User wants to save current chart data in a named file
//
void CChartWnd::CmdFileSaveAs()
{
	SaveFile(TRUE);
	UpdateMenu();
}

// CmdFileOpen
// User wants to open an existing file and read in chart data
//
void CChartWnd::CmdFileOpen()
{
	// Create data structure if there is none.
	//
	if (m_pChartObject != NULL)
	{
		if (m_pChartObject->m_bDirty)
		{
			if (MessageBox("Save existing data?", "Chart",
						MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				SaveFile(m_bUntitled);
			}
		}
	}

	// Get the data.
	//
	ReadFile();

	// If read failed, or there's no data to read, clean up.
	//

	if (!m_bChartSerializedOK)
	{
		// chart deserialization failed and chart is in an
		// inconsistent state -- don't delete.  Just null
		// the pointer and suffer a memory leak.

		m_pChartObject = NULL;
	}
	else
	{
		if ((m_pChartObject->m_pChartData == NULL) ||
			 m_pChartObject->m_pChartData->IsEmpty())
		{
			delete m_pChartObject;
			m_pChartObject = NULL;
		}
	}

	// Update the frame menu and client area
	//
	UpdateMenu();
	Invalidate(TRUE);
}

// OnAbout:
//
void CChartWnd::OnAbout()
{
	CModalDialog aboutBox("AboutBox");
	aboutBox.DoModal();
}
