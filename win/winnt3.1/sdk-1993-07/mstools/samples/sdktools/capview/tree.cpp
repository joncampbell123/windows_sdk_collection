#include        <afxwin.h>
#include        <afxdlgs.h>
#include        <afxcoll.h>
#include        <string.h>

#include        "types.h"
//#include        "dialogs.h"
#include        "resource.h"
#include        "ranges.h"
#include        "dispobj.h"
#include        "timer.h"
#include        "tree.h"


#define PAGESIZE 8

/*********************** GLOBAL DATA *****************************/

CAaaArray       rgLoc;
BOOL  fXYZ = FALSE;
BOOL  fTotal = TRUE;
extern  HICON   HIconTree;

/*****************************************************************/

BEGIN_MESSAGE_MAP(CTREEWND, CMDIChildWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_MDIACTIVATE()
	ON_WM_HSCROLL()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_VSCROLL()

	ON_COMMAND( VK_UP, OnUp )
	ON_COMMAND( VK_DOWN, OnDown )
	ON_COMMAND( IDM_TREE_EXPANDALL, OnExpandAll )
	ON_COMMAND( IDM_TREE_FONT, OnFont )
	ON_COMMAND( IDM_TREE_TOTAL, OnTotal )
	ON_COMMAND( IDM_TREE_XYZ, OnXYZ )
	ON_COMMAND( IDM_TREE_ZOOM, OnZoom )
	ON_COMMAND( IDM_TREE_RANGES, OnRanges )

END_MESSAGE_MAP()

BOOL CTREEWND::Create(LPCSTR szTitle, LONG style, const RECT& rect,
	CMDIFrameWnd* parent)
{
    const char * pszStockClass = AfxRegisterWndClass(
	CS_HREDRAW | CS_VREDRAW, LoadCursor(NULL, IDC_UPARROW),
	(HBRUSH) (COLOR_WINDOW+1), HIconTree);

    return CMDIChildWnd::Create(pszStockClass, szTitle, style, rect, parent);
}


CTREEWND::CTREEWND()  : ptOrigin(0, 0)
{
   int  i;
   
   m_pMenuCurrent = NULL;
   m_bWindowActive = FALSE;
   logFont = DefaultFont;
   fZoom = FALSE;
   
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
   rs.Above[4] = 20;  rs.BackColor[4] = RGB( 255,   0,   0);
   rs.Above[5] = 101;
   rs.cRanges = 5;
}

CTREEWND::~CTREEWND()
{
    if (m_bWindowActive) {
	m_pMenuCurrent->Detach();
    }
}


int CTREEWND::OnCreate(LPCREATESTRUCT p)
{
    TEXTMETRIC  tm;

    CDC *       dc = GetDC();
    dc->GetTextMetrics( &tm );
    ReleaseDC( dc );

    m_cxChar = tm.tmAveCharWidth;
    m_cxCaps = ( (tm.tmPitchAndFamily & 1) ? 3 : 2) * m_cxChar / 2;
    m_cyChar = tm.tmHeight + tm.tmExternalLeading;
    m_nMaxWidth = ( 20 ) * m_cxCaps;

    return 0;
}


void CTREEWND::OnDestroy()
{
  //  m_pMDIFrameWnd->SendMessage(WM_CHILDDESTROY, (UINT)m_hWnd, 0);
    return;
}


void CTREEWND::OnSize(UINT nType, int cx, int cy)
{
    m_cxClient = cx;
    m_cyClient = cy;

    ptOrigin.y = min( ptOrigin.y, rtArea.Height());
    
    SetScrollRange( SB_VERT, 0, rtArea.Height(), FALSE );
    SetScrollPos( SB_VERT, ptOrigin.y, TRUE );


    ptOrigin.x = min( ptOrigin.x, rtArea.Width() );

    SetScrollRange( SB_HORZ, 0, rtArea.Width(), FALSE );
    SetScrollPos( SB_HORZ, ptOrigin.x, TRUE );

    CMDIChildWnd::OnSize(nType, cx, cy);

    return;
}

void CTREEWND::OnMDIActivate(BOOL bActivate, CWnd * pActive,
	CWnd* pDeactivate)
{
    CMDIFrameWnd * pFrame = m_pMDIFrameWnd;
    CMenu *             pWinPopupMenu = NULL;
    CMenu *             pMenu = NULL;

    m_bWindowActive = bActivate;

    if (bActivate) {
	pMenu = new CMenu;
	pMenu->LoadMenu("MdiMenuTree");
	pWinPopupMenu = pMenu->GetSubMenu(TREE_MENU_POSITION);

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

void CTREEWND::OnPaint()
{
    CPaintDC dc (this);
    CFont       cFont;
    CFont *     lpcFontOld;
    
    if (!fZoom) {
	cFont.CreateFontIndirect(&logFont);
	lpcFontOld = dc.SelectObject(&cFont);
    }
    dc.SetWindowOrg(ptOrigin);
    
    dc.SetBkMode(TRANSPARENT);
    dc.m_ps.rcPaint.left += ptOrigin.x;
    dc.m_ps.rcPaint.right += ptOrigin.x;
    dc.m_ps.rcPaint.top += ptOrigin.y;
    dc.m_ps.rcPaint.bottom += ptOrigin.y;
    
    timerRoot.PaintTree(&dc, fZoom, &rs, dc.m_ps.rcPaint);

    if (!fZoom) {
	dc.SelectObject(lpcFontOld);    
    }
    return;
}

void CTREEWND::OnVScroll(UINT wParam, UINT pos, CScrollBar *)
{
    int     nScrollInc;

    switch( wParam ) {
    case SB_TOP:
	nScrollInc = -ptOrigin.y;
	break;

    case SB_BOTTOM:
	nScrollInc = rtArea.Height() - ptOrigin.y;
	break;

    case SB_LINEUP:
	nScrollInc = -m_cyClient/10;
	break;

    case SB_LINEDOWN:
	nScrollInc = m_cyClient/10;
	break;

    case SB_PAGEUP:
	nScrollInc = min( -1, -m_cyClient);
	break;

    case SB_PAGEDOWN:
	nScrollInc = max( 1, m_cyClient );
	break;

    case SB_THUMBTRACK:
	nScrollInc = pos - ptOrigin.y;
	break;

    default:
	nScrollInc = 0;
	break;
    }

    nScrollInc = max( -ptOrigin.y, min( nScrollInc, rtArea.Height() - ptOrigin.y ) );
    if (nScrollInc) {
	ptOrigin.y += nScrollInc;
	ScrollWindow( 0, -nScrollInc );
	SetScrollPos( SB_VERT, ptOrigin.y );
	UpdateWindow();
    }
}

void CTREEWND::OnHScroll(UINT wParam, UINT pos, CScrollBar *)
{
    int     nScrollInc;

    switch( wParam ) {
    case SB_LINEUP:
	nScrollInc = -m_cxClient/10;
	break;

    case SB_LINEDOWN:
	nScrollInc = m_cxClient/10;
	break;

    case SB_PAGEUP:
	nScrollInc = -m_cxClient;
	break;

    case SB_PAGEDOWN:
	nScrollInc = m_cxClient;
	break;

    case SB_THUMBTRACK:
	nScrollInc = pos - ptOrigin.x;
	break;

    default:
	nScrollInc = 0;
	break;
    }

    nScrollInc = max( -ptOrigin.x, min( nScrollInc, rtArea.Width() - ptOrigin.x ) );
    if (nScrollInc) {
	ptOrigin.x += nScrollInc;
	ScrollWindow( -nScrollInc, 0 );
	SetScrollPos( SB_HORZ, ptOrigin.x );
	UpdateWindow();
    }
}

void CTREEWND::OnUp()
{
}

void CTREEWND::OnDown()
{
}

void CTREEWND::OnKeyDown( UINT wParam, UINT, UINT )
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

void CTREEWND::OnLButtonDown( UINT, CPoint location )
{
    location.Offset(ptOrigin);
    timerRoot.DblClick( location );
    timerRoot.SetProperties(SET_LOCATION, fZoom);
    SetRect(timerRoot.RectTotal());
    Invalidate();
   
    return;
}

void CTREEWND::OnLButtonDblClk( UINT wParam, CPoint location ) 
{
   
   return;
}

void CTREEWND::OnFont()
{
   CFont        cFont;
   CFont *      lpcFont;        
   CFontDialog  fontDlg(&logFont);
   CRect        rt;
   
   fontDlg.DoModal();   
   
   CClientDC cdc( this );
   cFont.CreateFontIndirect(&logFont);
   lpcFont = cdc.SelectObject(&cFont);

   timerRoot.SetProperties(SET_SIZE|SET_LOCATION, fZoom, NULL, &cdc);
   SetRect( timerRoot.RectTotal() );

   cdc.SelectObject(lpcFont);
   Invalidate();
   return;
}


void CTREEWND::OnExpandAll()
{
    timerRoot.ExpandAll();
    timerRoot.SetProperties(SET_LOCATION, fZoom);
    SetRect( timerRoot.RectTotal() );
    Invalidate();
}

void CTREEWND::OnXYZ()
{
   fXYZ = !fXYZ;
   m_pMenuCurrent->CheckMenuItem(IDM_TREE_XYZ,
        MF_BYCOMMAND | (fXYZ ? MF_CHECKED : MF_UNCHECKED));
   Invalidate();
}

void CTREEWND::OnTotal()
{
   fTotal = !fTotal;
   m_pMenuCurrent->CheckMenuItem(IDM_TREE_TOTAL,
        MF_BYCOMMAND | (fTotal ? MF_CHECKED : MF_UNCHECKED));
   Invalidate();
}
   
void CTREEWND::OnZoom()
{
   fZoom = !fZoom;

   CClientDC cdc( this );

   timerRoot.SetProperties(SET_LOCATION, fZoom);
   SetRect( timerRoot.RectTotal() );

   Invalidate();
   
   return;    
}

void CTREEWND::OnRanges()
{
    BOOL f = rs.fPrune;

    DoRangesDlg(this, &rs);

    if (f != rs.fPrune) {
	timerRoot.SetProperties(SET_COLORS|SET_LOCATION, fZoom, &rs);
    } else {
	timerRoot.SetProperties(SET_COLORS, fZoom, &rs);
    }
    SetRect( timerRoot.RectTotal() );

    Invalidate();

    return;
}
