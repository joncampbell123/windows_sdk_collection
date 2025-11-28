// enrollset.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEnrollmentSet recordset

class CEnrollmentSet : public CRecordset
{
public:
	CEnrollmentSet(CDatabase* pDatabase);
	DECLARE_DYNAMIC(CEnrollmentSet)

// Field/Param Data
	//{{AFX_FIELD(CEnrollmentSet, CRecordset)
	long    m_StudentID;
	CString m_CourseID;
	CString m_SectionNo;
	CString m_Grade;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEnrollmentSet)
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
