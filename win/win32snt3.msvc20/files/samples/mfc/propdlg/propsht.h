// propsht.h : interface of the CModalShapePropSheet class
//
/////////////////////////////////////////////////////////////////////////////

class CModalShapePropSheet : public CPropertySheet
{
public:
	DECLARE_DYNAMIC(CModalShapePropSheet)
	CModalShapePropSheet(CWnd* pWndParent);

// Attributes
	CStylePage m_stylePage;
	CColorPage m_colorPage;
	CShapePreviewWnd wndPreview;

// Operations
	void SetSheetPropsFromShape(CShape* pShape);
	void SetShapePropsFromSheet(CShape* pShape);
	void UpdateShapePreview();

protected:
	//{{AFX_MSG(CModalShapePropSheet)
	afx_msg void OnApplyNow();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
