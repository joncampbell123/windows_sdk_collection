// propsht2.h : interface of the CModelessShapePropSheet class
//
/////////////////////////////////////////////////////////////////////////////

class CModelessShapePropSheet;

class CModelessShapePropSheet : public CPropertySheet
{
public:
	DECLARE_DYNAMIC(CModelessShapePropSheet)
	CModelessShapePropSheet(CWnd* pWndParent);

// Attributes
	CStylePage m_stylePage;
	CColorPage m_colorPage;

// Operations
	void SetSheetPropsFromShape(CShape* pShape);
	void SetShapePropsFromSheet(CShape* pShape);

// Overrides
	virtual void PostNcDestroy();

// Message handlers
protected:
	//{{AFX_MSG(CModelessShapePropSheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
