// DaoVwDoc.h : interface of the CDaoViewDoc class
//
/////////////////////////////////////////////////////////////////////////////

class CDaoTreeView;
class CDaoListView;

class CDaoViewDoc : public CDocument
{
protected: // create from serialization only
	CDaoViewDoc();
	DECLARE_DYNCREATE(CDaoViewDoc)

// Attributes
public:
	CDaoDatabase* m_pDB;
	CDaoTreeView* m_pTreeView;
	CDaoListView* m_pListView;

// Operations
public:
	void RefreshViews();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDaoViewDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDaoViewDoc();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDaoViewDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
