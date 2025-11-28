// stdset.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStudentSet recordset

class CStudentSet : public CRecordset
{
public:
	CStudentSet(CDatabase* pDatabase);
	DECLARE_DYNAMIC(CStudentSet)

// Field/Param Data
	//{{AFX_FIELD(CStudentSet, CRecordset)
	long    m_StudentID;
	CString m_Name;
	int     m_GradYear;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CStudentSet)
	public:
	virtual CString GetDefaultConnect();    // Default connection string
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);  // RFX support
	//}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};
