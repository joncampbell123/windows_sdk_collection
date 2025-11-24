#include        <afxwin.h>
#include        <afxdlgs.h>
#include        <afxcoll.h>
#include        <string.h>

#include        "types.h"
#include        "resource.h"
#include        "ranges.h"
#include        "list.h"
#include        "listwin.h"


#define PAGESIZE 8

/*********************** GLOBAL DATA *****************************/

extern  LOGFONT DefaultFont;
extern  HCURSOR HCursorSizer;
extern  HCURSOR HCursorNormal;
extern  HICON   HIconList;


/*********************** CListWnd implementation **********************/

BEGIN_MESSAGE_MAP(CListWnd, CMDIChildWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_MDIACTIVATE()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
        ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_VSCROLL()

	ON_COMMAND( VK_UP, OnUp )
	ON_COMMAND( VK_DOWN, OnDown )
	ON_COMMAND( IDM_LIST_FONT, OnFont )
	ON_COMMAND( IDM_LIST_RANGES, OnRanges )
	ON_COMMAND( IDM_LIST_ALPHA, OnSortAlpha )
	ON_COMMAND( IDM_LIST_CALLS, OnSortCalls )
	ON_COMMAND( IDM_LIST_TIME, OnSortTime )
	ON_COMMAND( IDM_LIST_PER_CALL, OnSortTimePer)
	ON_COMMAND( IDM_LIST_CHILDREN, OnChildren)
END_MESSAGE_MAP()

BOOL CListWnd::Create(LPCSTR szTitle, LONG style, const RECT& rect,
	CMDIFrameWnd* parent)
{
    const char * pszStockClass = AfxRegisterWndClass(
	CS_HREDRAW | CS_VREDRAW, NULL,
	(HBRUSH) (COLOR_WINDOW+1), HIconList);

    rgl[0] = 0;
    rgl[1] = 150;
    rgl[2] = 200;
    rgl[3] = 250;
    rgl[4] = 300;
    rgl[5] = 350;
    rgl[6] = 400;
    rgl[7] = 450;
    rgl[8] = 500;
    rgl[9] = 550;
    rgl[10] = 600;
    m_cxClient = 600;
    fCapture = FALSE;

    return CMDIChildWnd::Create(pszStockClass, szTitle, style, rect, parent);
}


CListWnd::CListWnd()
{
    int i;
    
    m_pMenuCurrent = NULL;
    m_bWindowActive = FALSE;
    m_SortOrder = LIST_ALPHA;
    logFont = DefaultFont;
    fChildren = FALSE;
    
    for (i=0; i<MAX_NUM_RANGES; i++) {
        rs.TextColor[i] = 0;
        rs.BackColor[i] = RGB( 0, 0, 0);
        rs.Above[i] = 101;
    }
    
    rs.Above[0] = 0;  rs.BackColor[0] = RGB(   0,   0, 255);  
    rs.TextColor[0] = 1;
    rs.Above[1] = 1;  rs.BackColor[1] = RGB(   0, 255,   0);
    rs.Above[2] = 2;  rs.BackColor[2] = RGB(   0, 255, 255);
    rs.Above[3] = 5;  rs.BackColor[3] = RGB( 255, 255,   0);
    rs.Above[4] = 20; rs.BackColor[4] = RGB( 255,   0,   0);
    rs.Above[5] = 101;
    rs.cRanges = 5;

    ratio = 0.33;

    return;
}                               /* CListWnd::CListWnd() */

CListWnd::~CListWnd()
{
    if (m_bWindowActive) {
	m_pMenuCurrent->Detach();
    }
}

void CListWnd::EndTiming()
{
    m_ListHead.SortByTotalTime();
    m_ListHead.SortByPerTime();
    m_ListHead.SortByCallCount();
    m_ListHead.SortByTPerTime();
    m_ListHead.SortByTimeAll();
    pRight->ComputeWidths();
    pLeft->ComputeWidths();
    return;
}

void CListWnd::InvalidateLine()
{
    CRect area(0, (m_nSelectLine - m_nVscrollPos ) * m_cyChar,
		m_cxClient, (m_nSelectLine + 1 - m_nVscrollPos) * m_cyChar);
    InvalidateRect( area );
}

int CListWnd::OnCreate(LPCREATESTRUCT p)
{
    TEXTMETRIC  tm;

    CDC *       dc = GetDC();
    dc->GetTextMetrics( &tm );
    ReleaseDC( dc );

    m_cxChar = tm.tmAveCharWidth;
    m_cxCaps = ( (tm.tmPitchAndFamily & 1) ? 3 : 2) * m_cxChar / 2;
    m_cyChar = tm.tmHeight + tm.tmExternalLeading;
    m_nMaxWidth = ( 20 ) * m_cxCaps;
    m_nVscrollPos = 0;

    /*
     */

    pLeft = new CListWndLeft;
    pLeft->Create(WS_HSCROLL|WS_VISIBLE, rectDefault, this);

    pRight = new CListWndRight;
    pRight->Create(WS_HSCROLL|WS_VISIBLE, rectDefault, this);

    return 0;
}


void CListWnd::OnDestroy()
{
  //  m_pMDIFrameWnd->SendMessage(WM_CHILDDESTROY, (UINT)m_hWnd, 0);
    return;
}


void CListWnd::OnSize(UINT nType, int cx, int cy)
{
    int  cxOld = m_cxClient;
    int  i;

    i = (int) (cx*ratio);
    pLeft->MoveWindow(0, 0, i-1, cy);
    pRight->MoveWindow(i+2, 0, cx-(i+2), cy);

    /* 
     * Now set the scroll ranges
     */

    m_cxClient = cx;
    m_cyClient = max(cy, 1);

    m_nVscrollMax = max( 0, (int)( m_ListHead.GetCount() ) - m_cyClient / m_cyChar );
    m_nVscrollPos = min( m_nVscrollPos, m_nVscrollMax );
    
    SetScrollRange( SB_VERT, 0, m_nVscrollMax, FALSE );
    SetScrollPos( SB_VERT, m_nVscrollPos, TRUE );

    CMDIChildWnd::OnSize(nType, cx, cy);

    return;
}

void CListWnd::OnMDIActivate(BOOL bActivate, CWnd * pActive,
	CWnd* pDeactivate)
{
    CMDIFrameWnd * pFrame = m_pMDIFrameWnd;
    CMenu *             pWinPopupMenu = NULL;
    CMenu *             pMenu = NULL;

    m_bWindowActive = bActivate;

    if (bActivate) {
	pMenu = new CMenu;
	pMenu->LoadMenu("MdiMenuList");
	pWinPopupMenu = pMenu->GetSubMenu(LISTWIN_MENU_POSITION);

	CMenu * pLastMenu = pFrame->MDISetMenu(pMenu,pWinPopupMenu);
	pLastMenu->DestroyMenu();

	delete m_pMenuCurrent;
	m_pMenuCurrent = pMenu;
    } else {
	pMenu = new CMenu;
	pMenu->LoadMenu("MdiMenuInit");
	pWinPopupMenu = pMenu->GetSubMenu(INIT_MENU_POS);

	CMenu * pLastMenu = pFrame->MDISetMenu(pMenu, pWinPopupMenu);
	pLastMenu->DestroyMenu();

	delete m_pMenuCurrent;
	m_pMenuCurrent = pMenu;
    }

    pFrame->DrawMenuBar();
}

void CListWnd::OnPaint()
{
    CPaintDC dc (this);
    CRect       rt;
    CBrush      b(RGB(0, 0, 0));
    
    dc.FillRect(&dc.m_ps.rcPaint, &b);

    return;
}

void CListWnd::OnVScroll(UINT wParam, UINT pos, CScrollBar * p)
{
    pLeft->OnVScroll(wParam, pos, p);
    m_nVscrollPos = pRight->OnVScroll(wParam, pos, p);
    SetScrollPos( SB_VERT, m_nVscrollPos );

    return;
}

void CListWnd::OnUp()
{
    InvalidateLine();
    
    if ( m_nSelectLine <= 0 ) {
	m_nSelectLine = m_ListHead.GetCount() - 1;
	m_nVscrollPos = max( 0, m_nSelectLine + 1 - ( m_cyClient / m_cyChar ) );
	Invalidate( TRUE );
    } else {
	m_nSelectLine -= 1;
	if (m_nSelectLine - m_nVscrollPos < 0) {
	    OnVScroll( SB_LINEUP, 0, NULL );
	}

	if (m_nSelectLine - m_nVscrollPos > (m_cyClient / m_cyChar ) ) {
	    m_nVscrollPos = m_nSelectLine + 1 - (m_cyClient / m_cyChar);
	    SetScrollPos( SB_VERT, m_nVscrollPos, TRUE );
	    Invalidate( TRUE );
	}
	if (m_nSelectLine - m_nVscrollPos < 0) {
	    m_nVscrollPos = m_nSelectLine;
	    SetScrollPos( SB_VERT, m_nVscrollPos, TRUE );
	    Invalidate( TRUE );
	}
    }

    InvalidateLine();
}

void CListWnd::OnDown()
{

    InvalidateLine();
    
    if (( m_nSelectLine == (int)(m_ListHead.GetCount() - 1 )) ||
	(m_nSelectLine == -1)) {
	m_nSelectLine = 0;
	m_nVscrollPos = 0;
	Invalidate( TRUE );
    } else {
	m_nSelectLine += 1;
	if ((m_nSelectLine - m_nVscrollPos + 1) > (m_cyClient/m_cyChar)) {
	    OnVScroll( SB_LINEDOWN, 0, NULL );
	}

	if (m_nSelectLine - m_nVscrollPos > (m_cyClient / m_cyChar ) ) {
	    m_nVscrollPos = m_nSelectLine + 1 - (m_cyClient / m_cyChar);
	    SetScrollPos( SB_VERT, m_nVscrollPos, TRUE );
	    Invalidate( TRUE );
	}
	if (m_nSelectLine - m_nVscrollPos < 0) {
	    m_nVscrollPos = m_nSelectLine;
	    SetScrollPos( SB_VERT, m_nVscrollPos, TRUE );
	    Invalidate( TRUE );
	}
    }

    InvalidateLine();
}

void CListWnd::OnKeyDown( UINT wParam, UINT, UINT )
{
    switch( wParam ) {
    case VK_HOME:
	OnVScroll( SB_TOP, 0, NULL );
	break;
    case VK_END:
	OnVScroll( SB_BOTTOM, 0, NULL );
	break;

    case VK_PRIOR:
	OnVScroll( SB_PAGEUP, 0, NULL );
	break;

    case VK_NEXT:
	OnVScroll( SB_PAGEDOWN, 0, NULL );
	break;

    case VK_LEFT:
	OnHScroll( SB_PAGEUP, 0, NULL );
	break;

    case VK_RIGHT:
	OnHScroll( SB_PAGEDOWN, 0, NULL );
	break;
    }
}

void CListWnd::OnLButtonDblClk( UINT wParam, CPoint location ) 
{
#if 0
    if (m_nSelectLine == -1) {
	OnLButtonDown( wParam, location );
    }
    OnEdit();
#endif
}



afx_msg void CListWnd::OnSortAlpha()
{
    m_pMenuCurrent->CheckMenuItem(IDM_LIST + (m_SortOrder & 7), MF_BYCOMMAND | MF_UNCHECKED);
    m_SortOrder = LIST_ALPHA;
    m_pMenuCurrent->CheckMenuItem(IDM_LIST + (m_SortOrder & 7), MF_BYCOMMAND | MF_CHECKED);
    Invalidate(TRUE);
    return;
}                               /* CListWnd::OnSortAlpha() */

afx_msg void CListWnd::OnSortCalls()
{
    m_pMenuCurrent->CheckMenuItem(IDM_LIST + (m_SortOrder & 7), MF_BYCOMMAND | MF_UNCHECKED);
    m_SortOrder = LIST_CALLS;
    m_pMenuCurrent->CheckMenuItem(IDM_LIST + (m_SortOrder & 7), MF_BYCOMMAND | MF_CHECKED);
    Invalidate(TRUE);
    return;
}                               /* CListWnd::OnSortCalls() */

afx_msg void CListWnd::OnSortTime()
{
    m_pMenuCurrent->CheckMenuItem(IDM_LIST + (m_SortOrder & 7), MF_BYCOMMAND | MF_UNCHECKED);
    m_SortOrder = fChildren ? LIST_TIME_TOTAL : LIST_TIME;
    m_pMenuCurrent->CheckMenuItem(IDM_LIST + (m_SortOrder & 7), MF_BYCOMMAND | MF_CHECKED);
    Invalidate(TRUE);
    return;
}                               /* CListWnd::OnSortTime() */

afx_msg void CListWnd::OnSortTimePer()
{
    m_pMenuCurrent->CheckMenuItem(IDM_LIST + (m_SortOrder & 7), MF_BYCOMMAND | MF_UNCHECKED);
    m_SortOrder = fChildren ? LIST_PER_TIME_TOTAL : LIST_PER_TIME;
    m_pMenuCurrent->CheckMenuItem(IDM_LIST + (m_SortOrder & 7), MF_BYCOMMAND | MF_CHECKED);
    Invalidate(TRUE);
    return;
}                               /* CListWnd::OnSortTimePer() */

void CListWnd::OnRanges()
{
    CFont       cFont;
    CFont *     lpcFont;        

    DoRangesDlg(this, &rs);

    CClientDC cdc( this );

    cFont.CreateFontIndirect(&logFont);
    lpcFont = cdc.SelectObject(&cFont);
//    pTimerObj->AssignSizes( &cdc, fZoom, 0, &rs);
//   SetRect( pTimerObj->AssignLocations()) ;
   cdc.SelectObject(lpcFont);

    Invalidate(TRUE);

    return;
}                               /* CListWnd::OnRanges() */

void CListWnd::OnFont()
{
   CFontDialog  fontDlg(&logFont);
   
   if (fontDlg.DoModal()) {
   
       pRight->ComputeWidths();
       pLeft->ComputeWidths();
   }
   return;
}                               /* CListWnd::OnFont() */

afx_msg void CListWnd::OnLButtonDown(UINT nFlags, CPoint pt)
{
    CClientDC       dc(this);
    
    fCapture = TRUE;
    SetCapture();
    rtCapture.SetRect(pt.x-1, 0, pt.x+1, m_cyClient);
    dc.InvertRect(&rtCapture);
    return;
}                               /* CListWnd::OnLButtonDown() */

afx_msg void CListWnd::OnLButtonUp(UINT nFlags, CPoint pt)
{
    int  x;

    if (fCapture) {
        ReleaseCapture();
        fCapture = FALSE;
        x = min(max(pt.x, 10), m_cxClient-10);
        ratio = ((float) x) / (float)m_cxClient;

        pLeft->MoveWindow(0, 0, x-1, m_cyClient);
        pRight->MoveWindow(x+2, 0, m_cxClient-(x+2), m_cyClient);
    } else {
        CWnd::OnLButtonUp(nFlags, pt);
    }
}                               /* CListWnd::OnLButtonDown() */

afx_msg void CListWnd::OnMouseMove(UINT nFlags, CPoint pt)
{
    if (!fCapture) {
        
        SetCursor(HCursorSizer);
        
    } else {
        CClientDC       dc(this);
        int             x = min(max(pt.x, 10), m_cxClient-10);
        
        dc.InvertRect(&rtCapture);
        rtCapture.SetRect(x-1, 0, x+1, m_cyClient);
        dc.InvertRect(&rtCapture);
    }
}                               /* CListWnd::OnMouseMove() */

afx_msg void CListWnd::OnChildren()
{
    fChildren = !fChildren;
    if (m_SortOrder == LIST_TIME) {
	m_SortOrder = LIST_TIME_TOTAL;
    } else if (m_SortOrder == LIST_TIME_TOTAL) {
	m_SortOrder = LIST_TIME;
    } else if (m_SortOrder == LIST_PER_TIME) {
	m_SortOrder = LIST_PER_TIME_TOTAL;
    } else if (m_SortOrder == LIST_PER_TIME_TOTAL) {
	m_SortOrder = LIST_PER_TIME;
    }
    m_pMenuCurrent->CheckMenuItem(IDM_LIST_CHILDREN, MF_BYCOMMAND | (fChildren ? MF_CHECKED : MF_UNCHECKED));
    Invalidate(TRUE);
    return;
}                               /* CListWnd::OnChildren() */




/**********************************************************************/

BOOL CListWndChild::Create(DWORD style, const RECT& rect, CListWnd * parent)
{
    const char * pszStockClass;

    pszStockClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, NULL,
                                        (HBRUSH) (COLOR_WINDOW+1), NULL);

    pListWnd = parent;

    return CWnd::Create(pszStockClass, "A", style, rect, parent, 999);

    
}                               /* CListWndChild::Create() */

int CListWndChild::OnCreate(LPCREATESTRUCT p)
{
//    ComputeWidths();

    return 0;
}                               /* CListWndChild::OnCreate() */

void CListWndChild::OnDestroy()
{
    return;
}                               /* CListWndCHild::OnDestroy() */


void  CListWndChild::OnSize(UINT nType, int cx, int cy)
{
    m_cxClient = cx;
    m_cyClient = cy;

    SetScrollRange(SB_HORZ, 0, max(1, m_nHscrollMax - cx), TRUE);

    m_nVscrollMax = max(1, (int)(ListHead()->GetCount())-m_cyClient/m_cyChar);

    CWnd::OnSize(nType, cx, cy);

    return;
}                               /* CListWndChild::OnSize() */

void CListWndChild::OnHScroll(UINT wParam, UINT pos, CScrollBar *)
{
    int         nScrollInc;
    
    switch( wParam ) {
    case SB_LEFT:
        nScrollInc = -m_nVscrollPos;
        break;
        
    case SB_RIGHT:
        nScrollInc = m_cxClient;
        break;
        
    case SB_LINELEFT:
        nScrollInc = -1;
        break;
        
    case SB_LINERIGHT:
        nScrollInc = 1;
        break;
        
    case SB_PAGELEFT:
        nScrollInc = -m_cxClient;
        break;

    case SB_PAGERIGHT:
        nScrollInc = m_cxClient;
        break;
        
    case SB_THUMBTRACK:
        nScrollInc = pos - m_nHscrollPos;
        break;
        
    default:
        nScrollInc = 0;
    }

    nScrollInc = max( -m_nHscrollPos,
                     min(nScrollInc,
                         (m_nHscrollMax - m_cxClient) - m_nHscrollPos ));
    if (nScrollInc) {
        m_nHscrollPos += nScrollInc;
	SetScrollPos( SB_HORZ, m_nHscrollPos );
        ScrollWindow( -nScrollInc, 0);
        UpdateWindow();
    }
    return;
}                               /* CListWndChild::OnHScroll() */

int CListWndChild::OnVScroll(UINT wParam, UINT pos, CScrollBar *)
{
    int     nScrollInc;

    switch( wParam ) {
    case SB_TOP:
	nScrollInc = -m_nVscrollPos;
	break;

    case SB_BOTTOM:
	nScrollInc = m_nVscrollMax;
	break;

    case SB_LINEUP:
	nScrollInc = -1;
	break;

    case SB_LINEDOWN:
	nScrollInc = 1;
	break;

    case SB_PAGEUP:
	nScrollInc = min( -1, -(m_cyClient/m_cyChar - 2));
	break;

    case SB_PAGEDOWN:
	nScrollInc = max( 1, m_cyClient/m_cyChar  - 2);
	break;

    case SB_THUMBTRACK:
	nScrollInc = pos - m_nVscrollPos;
	break;

    default:
	nScrollInc = 0;
	break;
    }

    nScrollInc = max( -m_nVscrollPos,
                     min( nScrollInc, m_nVscrollMax - m_nVscrollPos ) );
    
    if (nScrollInc) {
        CRect    rt(0, 2*m_cyChar+1, m_cxClient, m_cyClient);
	m_nVscrollPos += nScrollInc;
	ScrollWindow( 0, -m_cyChar * nScrollInc, NULL, &rt );
	UpdateWindow();
    }

    return m_nVscrollPos;
}                               /* CListWndChild::OnVScroll() */


afx_msg void CListWndChild::OnMouseMove(UINT nFlags, CPoint pt)
{
    SetCursor(HCursorNormal);

    return;
}                               /* CListWndChild::OnMouseMove() */


/*********************************************************************/

BEGIN_MESSAGE_MAP(CListWndLeft, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
        ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()


void  CListWndLeft::OnPaint()
{
    CPaintDC    dc(this);
    CBrush      b(RGB(255, 255, 255));

    CRect       area(0, 0, m_cxClient, m_cyClient);

    int nStart;
    int nEnd;

    nStart = max(dc.m_ps.rcPaint.top, 2*m_cyChar);
    nStart = m_nVscrollPos + (nStart / m_cyChar) - 2;

    nEnd = min( ListHead()->GetCount(),
               (dc.m_ps.rcPaint.bottom - dc.m_ps.rcPaint.top) / m_cyChar +
               nStart + 2);

    int         x = -m_nHscrollPos;
    int         y;
    int         perCent;
    int         j;
    CRect       rt;
    
    dc.FillRect( area, &b);

    dc.SetTextAlign( TA_CENTER | TA_TOP );
    rt.SetRect(0, 0, m_cxClient, m_cyChar);
    dc.ExtTextOut((m_cxClient-5)/2, 0, ETO_CLIPPED, &rt, "Name", 4, NULL);

    dc.SetTextAlign( TA_LEFT | TA_TOP );
    dc.SetBkMode(TRANSPARENT);

    /*
     *  Print out the data
     */

    CString     szDisplay;
    ListItem *  pCur;

    for (y= (nStart - m_nVscrollPos + 2) * m_cyChar;
         nStart < nEnd; nStart++, y += m_cyChar ) {
        pCur = ListHead()->GetPosition( nStart, SortOrder());
        perCent = (int) pCur->Time().PerCent(ListHead()->TotalTime());
        
        for (j=0; j<RangeStruct()->cRanges-1; j++) {
            if ((RangeStruct()->Above[j] <= perCent) &&
                (perCent < RangeStruct()->Above[j+1])) {
                break;
            }
        }
	
        CBrush bBack( RangeStruct()->BackColor[j] );
	
        dc.SetTextColor( RangeStruct()->TextColor[j] * RGB(255, 255, 255));
        
        area.top = y;
        area.bottom = y + m_cyChar;
        dc.FillRect( area, &bBack );
        
        /*
         * Print out the dll and function name
         */
	
        szDisplay = pCur->strModule() + " : " + pCur->strFunction();
        
        dc.TextOut(x, y, szDisplay);
        
    }

    dc.MoveTo(0, 2*m_cyChar-1); dc.LineTo(m_cxClient, 2*m_cyChar-1);
    dc.MoveTo(0, 2*m_cyChar); dc.LineTo(m_cxClient, 2*m_cyChar);

    return;
}                               /* CListWndLeft::OnPaint() */


void CListWndLeft::ComputeWidths()
{
    int         i;
    CRect       rt;
    CString     str;
    CClientDC   cdc( this );
    CFont       cFont;
    CFont *     lpcFont;
    TEXTMETRIC  tm;
    int         maxWidth = 0;

    cFont.CreateFontIndirect(LogFont());
    lpcFont = cdc.SelectObject(&cFont);
    cdc.GetTextMetrics( &tm );
    
    m_cxChar = tm.tmAveCharWidth;
    m_cyChar = tm.tmHeight + tm.tmExternalLeading;
    m_nVscrollPos = m_nHscrollPos = 0;

    ListItem * pCur;

    for (i=0; i<ListHead()->GetCount(); i++) {
        pCur = ListHead()->GetPosition(i, SortOrder());
        str = pCur->strModule() + " : " + pCur->strFunction();
        
        rt.SetRectEmpty();
        cdc.DrawText(str, -1, &rt, DT_CALCRECT);
        maxWidth = max(maxWidth, rt.Width()+10);
    }

    m_nHscrollMax = maxWidth;
    SetScrollRange(SB_HORZ, 0, max(1, m_nHscrollMax - m_cxClient), TRUE);

    m_nVscrollMax = max(1, (int)(ListHead()->GetCount())-m_cyClient/m_cyChar);

    cdc.SelectObject(lpcFont);
    Invalidate(TRUE);

    return;
}                               /* CListWndLeft::ComputeWidths() */

/*********************************************************************/

BEGIN_MESSAGE_MAP(CListWndRight, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
        ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()


void  CListWndRight::OnPaint()
{
    CPaintDC    dc(this);
    CBrush      b(RGB(255, 255, 255));
    CFont       cFont;
    CFont *     lpcFont;        

    CRect       area(0, 0, m_cxClient, m_cyClient);
    
    dc.FillRect( area, &b);

    CRect       rt;
    char        rgch[256];
    int         perCent;
    int         perCentPer;
    int         perCentAll;
    int         perCentPerAll;
    int         j;
    int         i;
    int         x = -m_nHscrollPos;
    int         y;
    
    int nStart;
    int nEnd;

    nStart = max(dc.m_ps.rcPaint.top, 2*m_cyChar);
    nStart = m_nVscrollPos + (nStart / m_cyChar) - 2;

    nEnd = min( ListHead()->GetCount(),
               (dc.m_ps.rcPaint.bottom - dc.m_ps.rcPaint.top) / m_cyChar +
               nStart + 2);

    dc.FillRect(&dc.m_ps.rcPaint, &b);

    area.SetRect(0, 0, m_cxClient, 0);
    dc.SetBkMode(TRANSPARENT);
    
    cFont.CreateFontIndirect(LogFont());
    lpcFont = dc.SelectObject(&cFont);

    area.bottom = 2*m_cyChar;
    dc.FillRect( area, &b );
    dc.SetTextColor(0);

    /*
     *  Print out the titles for the List Window
     */

    dc.SetTextAlign( TA_CENTER | TA_TOP );
	
    rt.SetRect(x, 0, x+rgl[0]-5, 2*m_cyChar);
    dc.ExtTextOut(x+(rgl[0]-5)/2, 0, ETO_CLIPPED, &rt, "Num", 3, NULL);
    dc.ExtTextOut(x+(rgl[0]-5)/2, m_cyChar, ETO_CLIPPED, &rt, "Calls", 5, NULL);

    rt.SetRect(x+rgl[0], 0, x+rgl[4]-5, 2*m_cyChar);
    dc.ExtTextOut(x+(rgl[0]+rgl[4]-5)/2, 0, ETO_CLIPPED, &rt, "Function Only", 13, NULL);
    
    rt.SetRect(x+rgl[0], 0, x+rgl[2]-5, 2*m_cyChar);
    dc.ExtTextOut(x+(rgl[0]+rgl[2]-5)/2, m_cyChar, ETO_CLIPPED, &rt, "Function", 8, NULL);

    rt.SetRect(x+rgl[2], 0, x+rgl[4]-5, 2*m_cyChar);
    dc.ExtTextOut(x+(rgl[2]+rgl[4]-5)/2, m_cyChar, ETO_CLIPPED, &rt, "Per Call", 8, NULL);
    
    rt.SetRect(x+rgl[4], 0, x+rgl[6]-5, 2*m_cyChar);
    dc.ExtTextOut(x+(rgl[4]+rgl[6]-5)/2, m_cyChar, ETO_CLIPPED, &rt, "Function", 8, NULL);
    
    rt.SetRect(x+rgl[4], 0, x+rgl[8], 2*m_cyChar);
    dc.ExtTextOut(x+(rgl[4]+rgl[8])/2, 0, ETO_CLIPPED, &rt, "Function + Children", 19, NULL);

    rt.SetRect(x+rgl[6], 0, x+rgl[8], 2*m_cyChar);
    dc.ExtTextOut(x+(rgl[6]+rgl[8])/2, m_cyChar, ETO_CLIPPED, &rt, "Per Call", 8, NULL);

    dc.SetTextAlign( TA_RIGHT | TA_TOP );


    /*
     *  Now print out the actual data items
     */

    CString szDisplay;
    ListItem *  pCur;

    for ( y= (nStart - m_nVscrollPos + 2)*m_cyChar;
	nStart < nEnd;
	nStart++, y += m_cyChar ) {
        pCur = ListHead()->GetPosition( nStart, SortOrder() );
	perCent = (int) pCur->Time().PerCent(ListHead()->TotalTime());
	perCentPer = (int) pCur->TimePerCall().PerCent(ListHead()->TotalTime());
	perCentAll = (int) pCur->TimeAll().PerCent(ListHead()->TotalTime());
	perCentPerAll = (int) pCur->TimeAllPerCall().PerCent(ListHead()->TotalTime());

	for (j=0; j<RangeStruct()->cRanges-1; j++) {
	    if ((RangeStruct()->Above[j] <= perCent) &&
		(perCent < RangeStruct()->Above[j+1])) {
		break;
	    }
	}
	
	CBrush bBack( RangeStruct()->BackColor[j] );
	
	dc.SetTextColor( RangeStruct()->TextColor[j] * RGB(255, 255, 255));

	area.top = y;
	area.bottom = y + m_cyChar;
	dc.FillRect( area, &bBack );

        /*
         * Now print the number of calls
         */
        
	sprintf(rgch, "%d", pCur->Calls());
	rt.SetRect(x, y, x+x+rgl[0]-5, y+m_cyChar);
	dc.ExtTextOut(x+rgl[0]-5, y, ETO_CLIPPED, &rt,
                      rgch, strlen(rgch), NULL);

        /*
         *
         */

        pCur->Time().format(rgch);
	rt.SetRect(x+rgl[0], y, x+rgl[1]-5, y+m_cyChar);
	dc.ExtTextOut(x+rgl[1]-5, y, ETO_CLIPPED, &rt,
                      rgch, strlen(rgch), NULL);

	sprintf(rgch, "%d%%", perCent);
	rt.SetRect(x+rgl[1], y, x+rgl[2]-5, y+m_cyChar);
	dc.ExtTextOut(x+rgl[2]-5, y, ETO_CLIPPED, &rt,
                      rgch, strlen(rgch), NULL);

	pCur->TimePerCall().format(rgch);
	rt.SetRect(x+rgl[2], y, x+rgl[3]-5, y+m_cyChar);
	dc.ExtTextOut(x+rgl[3]-5, y, ETO_CLIPPED, &rt,
                      rgch, strlen(rgch), NULL);

	sprintf(rgch, "%d%%", perCentPer);
	rt.SetRect(x+rgl[3], y, x+rgl[4]-5, y+m_cyChar);
	dc.ExtTextOut(x+rgl[4]-5, y, ETO_CLIPPED, &rt,
                      rgch, strlen(rgch), NULL);
    
	pCur->TimeAll().format(rgch);
	rt.SetRect(x+rgl[4], y, x+rgl[5]-5, y+m_cyChar);
	dc.ExtTextOut(x+rgl[5]-5, y, ETO_CLIPPED, &rt,
                      rgch, strlen(rgch), NULL);

	sprintf(rgch, "%d%%", perCentAll);
	rt.SetRect(x+rgl[5], y, x+rgl[6]-5, y+m_cyChar);
	dc.ExtTextOut(x+rgl[6]-5, y, ETO_CLIPPED, &rt,
                      rgch, strlen(rgch), NULL);

	pCur->TimeAllPerCall().format(rgch);
	rt.SetRect(x+rgl[6], y, x+rgl[7]-5, y+m_cyChar);
	dc.ExtTextOut(x+rgl[7]-5, y, ETO_CLIPPED, &rt,
                      rgch, strlen(rgch), NULL);

	sprintf(rgch, "%d%%", perCentPerAll);
	rt.SetRect(x+rgl[7], y, x+rgl[8]-5, y+m_cyChar);
	dc.ExtTextOut(x+rgl[8]-5, y, ETO_CLIPPED, &rt,
                      rgch, strlen(rgch), NULL);
    }

    /*
     * Now lets draw in the funny lines which make things look better
     */
    
    for (i=0; i<8; i++) {
	if ((i == 1) || (i == 3) || (i == 5) || (i == 7)) { 
	    y = 2*m_cyChar;
	} else if ((i == 2) || (i == 6)) {
	    y = m_cyChar;
	} else {
	    y = 0;
	}
	dc.MoveTo(x+rgl[i]-3, y); dc.LineTo(x+rgl[i]-3, m_cyClient);
	dc.MoveTo(x+rgl[i]-2, y); dc.LineTo(x+rgl[i]-2, m_cyClient);
	dc.MoveTo(x+rgl[i]-1, y); dc.LineTo(x+rgl[i]-1, m_cyClient);
    }

    dc.MoveTo(x+rgl[0], m_cyChar-1); dc.LineTo(m_cxClient, m_cyChar-1);
    dc.MoveTo(x+rgl[0], m_cyChar); dc.LineTo(m_cxClient, m_cyChar);
    dc.MoveTo(x+rgl[0], m_cyChar+1); dc.LineTo(m_cxClient, m_cyChar+1);

    dc.MoveTo(0, 2*m_cyChar-1); dc.LineTo(m_cxClient, 2*m_cyChar-1);
    dc.MoveTo(0, 2*m_cyChar); dc.LineTo(m_cxClient, 2*m_cyChar);

    dc.SelectObject(lpcFont);
    return;
}                               /* CListWndRight::OnPaint() */

void CListWndRight::ComputeWidths()
{
    int		i;
    char        rgch[100];
    CRect       rt;
    CClientDC   cdc( this );
    CFont       cFont;
    CFont *     lpcFont;
    TEXTMETRIC  tm;

    int         perCent;
    int         perCentPer;
    int         perCentAll;
    int         perCentPerAll;

    cFont.CreateFontIndirect(LogFont());
    lpcFont = cdc.SelectObject(&cFont);
    cdc.GetTextMetrics( &tm );
    
    m_cxChar = tm.tmAveCharWidth;
    m_cyChar = tm.tmHeight + tm.tmExternalLeading;
    m_nVscrollPos = m_nHscrollPos = 0;

    ListItem * pCur;
    CString     szDisplay;

    for (i=0; i<9; i++) {
        rgl[i] = 0;
    }


    for (i=0; i<ListHead()->GetCount(); i++) {
        pCur = ListHead()->GetPosition(i, SortOrder());
        
	perCent = (int) pCur->Time().PerCent(ListHead()->TotalTime());
	perCentPer = (int) pCur->TimePerCall().PerCent(ListHead()->TotalTime());
	perCentAll = (int) pCur->TimeAll().PerCent(ListHead()->TotalTime());
	perCentPerAll = (int) pCur->TimeAllPerCall().PerCent(ListHead()->TotalTime());


        /*
         *  Width of # calls
         */

        sprintf(rgch, "%d", pCur->Calls());
        rt.SetRectEmpty();
        cdc.DrawText(rgch, -1, &rt, DT_CALCRECT);
        rgl[0] = max(rgl[0], rt.Width()+10);

        /*
         *
         */

        pCur->Time().format(rgch);
	rt.SetRectEmpty();
        cdc.DrawText(rgch, -1, &rt, DT_CALCRECT);
        rgl[1] = max(rgl[1], rt.Width()+10);

	sprintf(rgch, "%d%%", perCent);
	rt.SetRectEmpty();
        cdc.DrawText(rgch, -1, &rt, DT_CALCRECT);
        rgl[2] = max(rgl[2], rt.Width()+10);

	pCur->TimePerCall().format(rgch);
	rt.SetRectEmpty();
        cdc.DrawText(rgch, -1, &rt, DT_CALCRECT);
        rgl[3] = max(rgl[3], rt.Width()+10);

	sprintf(rgch, "%d%%", perCentPer);
	rt.SetRectEmpty();
        cdc.DrawText(rgch, -1, &rt, DT_CALCRECT);
        rgl[4] = max(rgl[4], rt.Width()+10);
    
	pCur->TimeAll().format(rgch);
	rt.SetRectEmpty();
        cdc.DrawText(rgch, -1, &rt, DT_CALCRECT);
        rgl[5] = max(rgl[5], rt.Width()+10);

	sprintf(rgch, "%d%%", perCentAll);
	rt.SetRectEmpty();
        cdc.DrawText(rgch, -1, &rt, DT_CALCRECT);
        rgl[6] = max(rgl[6], rt.Width()+10);

	pCur->TimeAllPerCall().format(rgch);
	rt.SetRectEmpty();
        cdc.DrawText(rgch, -1, &rt, DT_CALCRECT);
        rgl[7] = max(rgl[7], rt.Width()+10);

	sprintf(rgch, "%d%%", perCentPerAll);
	rt.SetRectEmpty();
        cdc.DrawText(rgch, -1, &rt, DT_CALCRECT);
        rgl[8] = max(rgl[8], rt.Width()+10);
    }

    for (i=1; i<9; i++) {
        rgl[i] += rgl[i-1];
    }

    m_nHscrollMax = rgl[8];
    SetScrollRange(SB_HORZ, 0, max(1, m_nHscrollMax), TRUE);

    m_nVscrollMax = max(1, (int)(ListHead()->GetCount())-m_cyClient/m_cyChar);
    
    cdc.SelectObject(lpcFont);
    Invalidate(TRUE);

    return;
}                               /* CListWndRight::ComputeWidths() */


#if 0
afx_msg void CListWndRight::OnMouseMove(UINT nFlags, CPoint pt)
{
    int  i;
    BOOL f = FALSE;

//    if (!fCapture) {
        
        SetCursor(HCursorNormal);
        
//    } else {
//        CClientDC       dc(this);
//        int             x = min(max(pt.x, rgl[iCapture-1]+10),
//				rgl[iCapture+1]-10);
//        
//        dc.InvertRect(&rtCapture);
//        rtCapture.SetRect(x-1, 0, x+1, m_cyClient);
//        dc.InvertRect(&rtCapture);
//    }
}                               /* CListWndRight::OnMouseMove() */


afx_msg void CListWnd::OnLButtonDown(UINT nFlags, CPoint pt)
{
    BOOL        f = FALSE;
    int         i;
    
    for (i=1; i<10; i++) {
        if ((rgl[i]-5 < pt.x) && (pt.x < rgl[i])) {
            f = TRUE;
            break;
        }
    }

    if (f) {
        CClientDC       dc(this);
        fCapture = TRUE;
        iCapture = i;
        SetCapture();
        rtCapture.SetRect(pt.x-1, 0, pt.x+1, m_cyClient);
        dc.InvertRect(&rtCapture);
    } else {
        CWnd::OnLButtonDown(nFlags, pt);
    }
}                               /* CListWnd::OnLButtonDown() */

afx_msg void CListWnd::OnLButtonUp(UINT nFlags, CPoint pt)
{
    if (fCapture) {
        ReleaseCapture();
        fCapture = FALSE;
        rgl[iCapture] = min(max(pt.x, rgl[iCapture-1]+10), rgl[iCapture+1]-10);
        Invalidate(TRUE);
    } else {
        CWnd::OnLButtonUp(nFlags, pt);
    }
}                               /* CListWnd::OnLButtonDown() */
#endif
