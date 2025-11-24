/*
 *   tree.h
 */

/*
 *   Window class for tree
 */

class CTREEWND : public CMDIChildWnd
{
private:
    int		m_cxChar;
    int		m_cyChar;
    int		m_cxCaps;
    int		m_nMaxWidth;
    int		m_cxClient;
    int		m_cyClient;
    int		m_nSelectLine;
    CMenu *	m_pMenuCurrent;
    BOOL	m_bWindowActive;
    CRect       rtArea;         /* Size of total display */
    CPoint      ptOrigin;
    LOGFONT	logFont;
    BOOL	fZoom;
    RANGESTRUCT	rs;
    TIMERROOT   timerRoot;

public:
    CTREEWND();
    ~CTREEWND();
    
    RANGESTRUCT * prs() { return &rs; };

    BOOL Create(LPCSTR szTitle, LONG style = 0, 
	const RECT& rect = rectDefault, CMDIFrameWnd* pParent = NULL);

    void	InvalidateLine();

    void InitTiming() {return; };
    void AddTiming(int iDepth, char * pszModule, char * pszFunction, int iCalls, TIMETYPE timeTotal, TIMETYPE timeFunction)
    {
        timerRoot.AddTiming(iDepth, pszModule, pszFunction, iCalls, timeTotal, timeFunction);
    }
    void EndTiming() {
        CClientDC cdc( this );
        CRect           rt;

        timerRoot.SetProperties(SET_COLORS|SET_PERCENT|SET_STRING|SET_SIZE|SET_LOCATION,
                                FALSE, &rs, &cdc);
        SetRect(timerRoot.RectTotal());
        return;
    };

    void SetRect(CRect rt) {
        rtArea = rt;
	
	ptOrigin.y = min( ptOrigin.y, rtArea.Height());
	SetScrollRange(SB_VERT, 0, rtArea.Height(), FALSE);
	SetScrollPos( SB_VERT, ptOrigin.y, TRUE );
	
	ptOrigin.x = min( ptOrigin.x, rtArea.Width() );
	
	SetScrollRange( SB_HORZ, 0, rtArea.Width(), FALSE );
	SetScrollPos( SB_HORZ, ptOrigin.x, TRUE );
        return;
    }

    // Message handlers
 
    afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void	OnDestroy();
    afx_msg void	OnDown();
    afx_msg void	OnMDIActivate(BOOL bActivate, CWnd * pActivateWnd, CWnd * pDeactiveWnd);
    afx_msg void	OnExpandAll();
    afx_msg void	OnFont();
    afx_msg void	OnHScroll( UINT wParam, UINT pos, CScrollBar * control);
    afx_msg void	OnKeyDown( UINT wParam, UINT, UINT );
    afx_msg void	OnLButtonDblClk( UINT wParam, CPoint location );
    afx_msg void	OnLButtonDown( UINT wParam, CPoint location );
    afx_msg void	OnPaint();
    afx_msg void	OnRanges();
    afx_msg void	OnSize(UINT nType, int cx, int cy);
    afx_msg void	OnTotal();
    afx_msg void	OnUp();
    afx_msg void	OnVScroll( UINT wParam, UINT pos, CScrollBar * control);
    afx_msg void	OnXYZ();
    afx_msg void	OnZoom();

    DECLARE_MESSAGE_MAP()
};
