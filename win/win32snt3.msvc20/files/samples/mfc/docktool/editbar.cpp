// editbar.cpp : implementation file
//

#include "stdafx.h"
#include "docktool.h"
#include "editbar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define COMBOBOX_INDEX 5
#define COMBOBOX_WIDTH 150
#define COMBOBOX_HEIGHT 150

static UINT BASED_CODE VertEditButtons[] =
{
	// same order as in the bitmap 'toolbar.bmp'
	ID_EDIT_BM_TOGGLE,
	ID_EDIT_BM_NEXT,
	ID_EDIT_BM_PREV,
	ID_EDIT_BM_CLEARALL,
		ID_SEPARATOR,
	ID_EDIT_FIND,
		ID_SEPARATOR,
	ID_EDIT_FINDINFILES,
		ID_SEPARATOR,
	ID_EDIT_INDENT,
	ID_EDIT_UNINDENT,
		ID_SEPARATOR,
	ID_WINDOW_NEW,
	ID_WINDOW_SPLIT,
	ID_WINDOW_CASCADE,
	ID_WINDOW_TILE_HORZ,
	ID_WINDOW_TILE_VERT,
};
static UINT BASED_CODE HorzEditButtons[] =
{
	// same order as in the bitmap 'toolbar.bmp'
	ID_EDIT_BM_TOGGLE,
	ID_EDIT_BM_NEXT,
	ID_EDIT_BM_PREV,
	ID_EDIT_BM_CLEARALL,
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR,
	ID_EDIT_FINDINFILES,
		ID_SEPARATOR,
	ID_EDIT_INDENT,
	ID_EDIT_UNINDENT,
		ID_SEPARATOR,
	ID_WINDOW_NEW,
	ID_WINDOW_SPLIT,
	ID_WINDOW_CASCADE,
	ID_WINDOW_TILE_HORZ,
	ID_WINDOW_TILE_VERT,
};

/////////////////////////////////////////////////////////////////////////////
// CEditBar

CEditBar::CEditBar()
{
	m_bColor = TRUE;
}

CEditBar::~CEditBar()
{
}

BOOL CEditBar::Init(CWnd* pParentWnd, BOOL bColor, BOOL bToolTips)
{
	m_bColor=bColor;
	// start out with no borders
	if (!Create(pParentWnd, WS_CHILD | WS_VISIBLE | ((bToolTips)?(CBRS_TOOLTIPS |
		CBRS_FLYBY):0), IDW_EDIT_BAR))
	{
		return FALSE;
	}
	if (!SetVertical())
		return FALSE;

	// Cache vertically oriented toolbar measurements
	// when the toolbar has no border styles to make
	// responding to CalcFixedLayout easier
	m_rectInsideVert.SetRectEmpty();
	CalcInsideRect(m_rectInsideVert,FALSE);
	m_sizeVert = CToolBar::CalcFixedLayout(FALSE,FALSE);
	if (!SetHorizontal())
		return FALSE;

	CRect rect;

	// Cache horizontally oriented toolbar measurements
	// when the toolbar has no border styles to make
	// responding to CalcFixedLayout easier
	m_rectInsideHorz.SetRectEmpty();
	CalcInsideRect(m_rectInsideHorz,TRUE);
	m_sizeHorz = CToolBar::CalcFixedLayout(FALSE,TRUE);

	// Set toolbar border style to top or bottom
	// so that the ToolBar is Horizontally oriented
	// before we call GetItemRect which uses the orientation
	// style to determine the appropriate coordinates to return
	SetBarStyle(GetBarStyle() | CBRS_ALIGN_TOP);
	GetItemRect(COMBOBOX_INDEX, rect);

	// Adjust rectangle to be the size of the ComboBox
	rect.right=rect.left+COMBOBOX_WIDTH;
	rect.bottom=rect.top+COMBOBOX_HEIGHT;

	// The ID of the ComboBox is important for two reasons.  One, so you
	// can receive notifications from the control.  And also for ToolTips.
	// During HitTesting if the ToolBar sees that the mouse is one a child
	// control, the toolbar will lookup the controls ID and search for a
	// string in the string table with the same ID to use for ToolTips
	// and StatusBar info.
	if (!m_SearchBox.Create(WS_VISIBLE | WS_CHILD | CBS_DROPDOWN |
		CBS_AUTOHSCROLL | WS_VSCROLL | CBS_HASSTRINGS,rect,this,
		IDC_EDITBAR_SEARCHBOX))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CEditBar::SetHorizontal()
{
	if (m_bColor)
	{
		if (!LoadBitmap(IDR_COLOR_EDITBARHORZ))
			return FALSE;
	}
	else
	{
		if (!LoadBitmap(IDR_MONO_EDITBARHORZ))
			return FALSE;
	}

	if (!SetButtons(HorzEditButtons, sizeof(HorzEditButtons)/sizeof(UINT)))
		return FALSE;

	// CToolBar::CalcFixedLayout uses the value in the Image index as
	// the width or height depending on requested orientation when the button
	// style at that position is TBBS_SEPARATOR.  Since the Image index value
	// is set to zero for all ID_SEPARATOR entries in the button array during
	// a SetButtons call this has no effect normally to the calculated
	// dimensions.  The primary reason for this functionality is so that a
	// developer can insert a control on the toolbar(as we have done here)
	// without having to understand how to calculate the toolbar's dimensions.
	SetButtonInfo(COMBOBOX_INDEX,ID_SEPARATOR,TBBS_SEPARATOR,COMBOBOX_WIDTH);
	if (m_SearchBox.m_hWnd != NULL)
		m_SearchBox.ShowWindow(SW_SHOW);
	return TRUE;
}

BOOL CEditBar::SetVertical()
{
	if (m_bColor)
	{
		if (!LoadBitmap(IDR_COLOR_EDITBARVERT))
			return FALSE;
	}
	else
	{
		if (!LoadBitmap(IDR_MONO_EDITBARVERT))
			return FALSE;
	}

	if(!SetButtons(VertEditButtons, sizeof(VertEditButtons)/sizeof(UINT)))
		return FALSE;

	if (m_SearchBox.m_hWnd != NULL)
		m_SearchBox.ShowWindow(SW_HIDE);
	return TRUE;
}

void CEditBar::SetColor(BOOL bColor)
{
	m_bColor=bColor;
	BOOL bHorz = ((m_dwStyle & CBRS_ORIENT_HORZ)!=0);
	if (bHorz)
	{
		if (m_bColor)
			LoadBitmap(IDR_COLOR_EDITBARHORZ);
		else
			LoadBitmap(IDR_MONO_EDITBARHORZ);
	}
	else
	{
		if (m_bColor)
			LoadBitmap(IDR_COLOR_EDITBARVERT);
		else
			LoadBitmap(IDR_MONO_EDITBARVERT);
	}
}

CSize CEditBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	CSize size;

	// If bStretch is TRUE then the ToolBar is being permenantly docked
	// and is not floatable.  Consequently, the user shouldn't see a border
	// to the right/bottom of the last button to sugesst that the ToolBar can
	// be floated.  CControlBar::CalcFixedLayout will return the maximum
	// dimension in the orientation to be assured that the ToolBar will be to
	// big to see the border, and zero in the other dimension.  If bStretch
	// is FALSE CControlBar::CalFixedLayout returns (0,0)
	size = CControlBar::CalcFixedLayout(bStretch,bHorz);

	CRect rect;
	rect.SetRectEmpty();

	// CalcInsideRect is meant to take a rectangle which represents the whole
	// ToolBar and subtract borders to get to the area of the Toolbar that
	// contains just the buttons.   Since we are only interested in the
	// borders themselves, we give CalcInsideRect an empty rectangle, from
	// which it subtracts the current border styles.  What we end up with is
	// an inverted rectangle with negative dimensions.  Since we cached a
	// similar negative dimensioned rectangle with no border styles we can
	// take the difference to adjust our cached dimensions so that the
	// represent current border styles, without even know what these styles
	// are.  This is especially important if you are trying to write one
	// application that runs on different operating systems because border
	// styles will vary according to platform.
	CalcInsideRect(rect,bHorz);

	if (bHorz)
	{
		size.cx += m_sizeHorz.cx + (m_rectInsideHorz.Width() - rect.Width());
		size.cy += m_sizeHorz.cy + (m_rectInsideHorz.Height() - rect.Height());
	}
	else
	{
		size.cx = m_sizeVert.cx + (m_rectInsideVert.Width() - rect.Width());
		size.cy = m_sizeVert.cy + (m_rectInsideVert.Height() - rect.Height());
	}
	return size;
}

BEGIN_MESSAGE_MAP(CEditBar, CToolBar)
	//{{AFX_MSG_MAP(CEditBar)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEditBar message handlers

void CEditBar::OnSize(UINT nType, int cx, int cy)
{
	if (cx > cy)
		SetHorizontal();
	else
		SetVertical();

	CToolBar::OnSize(nType, cx, cy);
}
