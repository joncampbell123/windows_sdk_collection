// mainfrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "collect.h"

#include "mainfrm.h"

#include "colledoc.h"
#include "strlstvw.h"
#include "typlstvw.h"
#include "intlstvw.h"
#include "dwarryvw.h"
#include "typaryvw.h"
#include "ptarryvw.h"
#include "mapssvw.h"
#include "typtrmap.h"
#include "mapdwvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_COMMAND_RANGE(ID_STRINGLIST, ID_MAP, OnExample)
	ON_UPDATE_COMMAND_UI_RANGE(ID_STRINGLIST, ID_MAP, OnUpdateExampleUI)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars

// toolbar buttons - IDs are command buttons
static UINT BASED_CODE buttons[] =
{
	// same order as in the bitmap 'toolbar.bmp'
	ID_FILE_NEW,
	ID_FILE_OPEN,
	ID_FILE_SAVE,
		ID_SEPARATOR,
	ID_EDIT_CUT,
	ID_EDIT_COPY,
	ID_EDIT_PASTE,
		ID_SEPARATOR,
	ID_FILE_PRINT,
	ID_APP_ABOUT,
};

static UINT BASED_CODE indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_nCurrentExample = ID_STRINGLIST;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadBitmap(IDR_MAINFRAME) ||
		!m_wndToolBar.SetButtons(buttons,
		  sizeof(buttons)/sizeof(UINT)))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnExample(UINT nCmdID)
{
	if (nCmdID == m_nCurrentExample)
		return;  // already selected

	// Set the child window i.d. of the active view to AFX_IDW_PANE_FIRST.
	// This is necessary so that CFrameWnd::RecalcLayout will allocate
	// this "first pane" to that portion of the frame window's client
	// area not allocated to control bars.  Set the child i.d. of
	// the previously active view to some other i.d.; we will use the
	// command i.d. as the child i.d.
	CView* pOldActiveView = GetActiveView();
		::SetWindowLong(pOldActiveView->m_hWnd, GWL_ID, m_nCurrentExample);

	CView* pNewView;
	switch (nCmdID)
	{
		case ID_STRINGLIST:
			pNewView = new CStringListView;
			break;
		case ID_TYPEDLIST:
			pNewView = new CTypedPtrListView;
			break;
		case ID_INTLIST:
			pNewView = new CIntListView;
			break;
		case ID_DWORDARRAY:
			pNewView = new CDWordArrayView;
			break;
		case ID_TYPEDPTRARRAY:
			pNewView = new CTypedPtrArrayView;
			break;
		case ID_POINTARRAY:
			pNewView = new CPointArrayView;
			break;
		case ID_MAPSTRINGTOSTRING:
			pNewView = new CMapStringToStringView;
			break;
		case ID_TYPEDPTRMAP:
			pNewView = new CTypedPtrMapView;
			break;
		case ID_MAPDWORDTOMYSTRUCT:
			pNewView = new CMapDWordToMyStructView;
			break;
		default:
			ASSERT(0);
			return;
	}

	pNewView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		rectDefault, this, AFX_IDW_PANE_FIRST, NULL);
	pNewView->ShowWindow(SW_SHOW);
	pOldActiveView->GetDocument()->AddView(pNewView);
	pNewView->OnInitialUpdate();
	SetActiveView(pNewView);
	RecalcLayout();
	pOldActiveView->GetDocument()->RemoveView(pOldActiveView);
	pOldActiveView->DestroyWindow();

	m_nCurrentExample = nCmdID;
}

void CMainFrame::OnUpdateExampleUI(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrentExample);
}
