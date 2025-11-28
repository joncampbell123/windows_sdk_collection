// autocpnt.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CClikPoint command target

class CClikPoint : public CCmdTarget
{
	DECLARE_DYNCREATE(CClikPoint)
public:
	CClikPoint();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClikPoint)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void OnFinalRelease();

protected:
	virtual ~CClikPoint();
	// Generated message map functions
	//{{AFX_MSG(CClikPoint)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CClikPoint)
	short m_x;
	short m_y;
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
};

/////////////////////////////////////////////////////////////////////////////
