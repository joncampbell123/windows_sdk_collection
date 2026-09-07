// combobar.h : interface of the CTBComboBox and CComboToolBar class
//
/////////////////////////////////////////////////////////////////////////////

class CNotepadView; // forward declaration

class CTBComboBox : public CComboBox
{
// Attributes
protected:
	CFont m_font;

// Implementation
protected:
	static int CALLBACK FontEnumProc(const LOGFONT *pLogFont, 
                                     const TEXTMETRIC *pTextMetric,
                                     DWORD type, LPARAM pDestLogFont);
	// Generated message map functions
	//{{AFX_MSG(CTBComboBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CComboToolBar : public CToolBar
{
// Attributes
public:
	CTBComboBox m_toolBarCombo;

// Implementation
public:
	CNotepadView* GetView();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	// Generated message map functions
	//{{AFX_MSG(CComboToolBar)
	//}}AFX_MSG
	afx_msg void OnSelectEndOk();
	afx_msg void OnSetFocus();
	DECLARE_MESSAGE_MAP()
};
