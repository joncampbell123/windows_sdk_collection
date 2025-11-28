// minifrm.h : interface of the CShapePropSheetFrame class
//
/////////////////////////////////////////////////////////////////////////////

class CShapePropSheetFrame : public CMiniFrameWnd
{
// Constructor
public:
	CShapePropSheetFrame();

// Attributes
public:
	CModelessShapePropSheet* m_pModelessShapePropSheet;

// Handlers
protected:
	//{{AFX_MSG(CShapePropSheetFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
