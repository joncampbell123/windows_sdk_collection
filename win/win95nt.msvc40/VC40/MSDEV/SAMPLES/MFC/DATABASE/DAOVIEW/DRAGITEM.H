// DragItem.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDragItem window

class CDragItem : public CObject
{
public:
// Flag values
	enum Mode { itemNull = 0,itemTable = 1,itemQuery = 2,itemRelation = 3 };
	 
// Construction
	CDragItem();
	CDragItem(WORD nItemType, LPCTSTR lpszDBName, LPCTSTR lpszDBConnect, 
		LPCTSTR lpszItemName );

// Attributes
public:
	WORD m_nItemType;
	CString m_strDBName;
	CString m_strDBConnect;
	CString m_strItemName;

	virtual void Serialize( CArchive& ar );

// Operations
public:
	BOOL Transfer(CDaoTreeView* pTreeView,CDaoDatabase* pDBTarget);
	BOOL CopyTable(CDaoDatabase* pDBSource,CDaoDatabase* pDBTarget);
	BOOL CopyQueryDef(CDaoDatabase* pDBSource,CDaoDatabase* pDBTarget);

// Overrides

// Implementation
public:
	virtual ~CDragItem();

	// Generated message map functions
protected:
};

/////////////////////////////////////////////////////////////////////////////
