// sectset.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSectionSet recordset

class CSectionSet : public CRecordset
{
public:
	CSectionSet(CDatabase* pDatabase);
	DECLARE_DYNAMIC(CSectionSet)

// Field/Param Data
	//{{AFX_FIELD(CSectionSet, CRecordset)
	CString m_CourseID;
	CString m_SectionNo;
	CString m_InstructorID;
	CString m_RoomNo;
	CString m_Schedule;
	int     m_Capacity;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSectionSet)
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
