/*
 *   listwin.h
 */

class CListWndLeft;
class CListWndRight;

class CListWnd : public CMDIChildWnd
{
private:
   short        m_iTop;                 /* index of car at Top of window */
   short        m_iCur;                 /* Current carat index          */
   int          m_cxChar;
   int          m_cyChar;
   int          m_nVscrollPos;
   int          m_cxCaps;
   int          m_nMaxWidth;
   int          m_cxClient;
   int          m_cyClient;
   int          m_nVscrollMax;
   int          m_nSelectLine;
   CMenu *      m_pMenuCurrent;
   BOOL         m_bWindowActive;
   ListHead     m_ListHead;
   int          m_SortOrder;
   LOGFONT      logFont;
   RANGESTRUCT  rs;
   CRect        rgrtMove[9];
   BOOL         fCursor;   
   BOOL         fChildren;
   BOOL         fCapture;
   CRect        rtCapture;
   int          iCapture;
   int          rgl[11];
   CListWndLeft * pLeft;
   CListWndRight * pRight;
   float        ratio;

public:
    CListWnd();
    ~CListWnd();

    ListHead *  ListHead() { return &m_ListHead; };
    int         SortOrder() { return m_SortOrder; };
    RANGESTRUCT * RangeStruct() { return &rs; };
    LOGFONT *   LogFont() { return &logFont; };

    BOOL Create(LPCSTR szTitle, LONG style = 0, 
	const RECT& rect = rectDefault, CMDIFrameWnd* pParent = NULL);

    void        InvalidateLine();

    RANGESTRUCT * prs() { return &rs; };

   void InitTiming() { return; }
   void AddTiming(int iDepth, char * szModule, char * szName, int cCalls, TIMETYPE totalTime, TIMETYPE functionTime)
   {
       m_ListHead.AddTiming(szModule, szName, cCalls, totalTime, functionTime);
   };

    void EndTiming();

    // Message handlers
 
    afx_msg void        OnChildren();
    afx_msg int         OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void        OnDestroy();
    afx_msg void        OnDown();
    afx_msg void        OnFont();
    afx_msg void        OnKeyDown( UINT wParam, UINT, UINT );
    afx_msg void        OnLButtonDblClk( UINT wParam, CPoint location );
    afx_msg void        OnLButtonDown( UINT wParam, CPoint location );
    afx_msg void        OnLButtonUp( UINT wParam, CPoint location );
    afx_msg void        OnMDIActivate(BOOL bActivate, CWnd * pActivateWnd, CWnd * pDeactiveWnd);
    afx_msg void        OnMouseMove(UINT nFlags, CPoint pt);
    afx_msg void        OnPaint();
    afx_msg void        OnRanges();
    afx_msg void        OnSize(UINT nType, int cx, int cy);
    afx_msg void        OnSortAlpha();
    afx_msg void        OnSortCalls();
    afx_msg void        OnSortTime();
    afx_msg void        OnSortTimePer();
    afx_msg void        OnUp();
    afx_msg void        OnVScroll(UINT wParam, UINT pos, CScrollBar * control);

    DECLARE_MESSAGE_MAP()
};



class CListWndChild : public CWnd {
 public:
    CListWnd *          pListWnd;
    int                 m_cxClient;
    int                 m_cyClient;

    int                 m_nHscrollPos;
    int                 m_nHscrollMax;
    int                 m_nVscrollPos;
    int                 m_nVscrollMax;
    int                 m_cxChar;
    int                 m_cyChar;

 public:
    ListHead *          ListHead() { return pListWnd->ListHead(); };
    int                 SortOrder() { return pListWnd->SortOrder(); };
    RANGESTRUCT *       RangeStruct() { return pListWnd->RangeStruct(); };
    LOGFONT *           LogFont() { return pListWnd->LogFont(); };

    BOOL                Create(DWORD, const RECT&, CListWnd *);
    int                 OnVScroll(UINT wParam, UINT pos, CScrollBar * control);
    
    afx_msg int         OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void        OnDestroy();
    afx_msg void        OnHScroll(UINT wParam, UINT pos, CScrollBar * control);
    afx_msg void        OnMouseMove(UINT nFlags, CPoint pt);
    afx_msg void        OnSize(UINT nType, int cx, int cy);

    virtual void        ComputeWidths() = 0;
};


class CListWndLeft : public CListWndChild {
    
 public:
    void                ComputeWidths();

    afx_msg void        OnPaint();

    DECLARE_MESSAGE_MAP()
};                              /* class CListWndLeft() */




class CListWndRight : public CListWndChild {
    int                 rgl[11];

 public:
    void 		ComputeWidths();
    
    afx_msg void        OnPaint();

    DECLARE_MESSAGE_MAP()
};                              /* class CListWndRight() */

