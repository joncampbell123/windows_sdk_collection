// preview.h : interface of the CShapePreviewWnd class
//
/////////////////////////////////////////////////////////////////////////////

class CModalShapePropSheet;

class CShapePreviewWnd : public CWnd
{
	friend class CModalShapePropSheet;

	CShapePreviewWnd();

	//{{AFX_MSG(CShapePreviewWnd)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
