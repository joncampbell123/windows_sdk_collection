// instrset.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInstructorSet recordset

class CInstructorSet : public CRecordset
{
public:
	CInstructorSet(CDatabase* pDatabase);
	DECLARE_DYNAMIC(CInstructorSet)

// Field/Param Data
	//{{AFX_FIELD(CInstructorSet, CRecordset)
	CString m_InstructorID;
	CString m_Name;
	CString m_RoomNo;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInstructorSet)
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
