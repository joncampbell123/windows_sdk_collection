// dsectset.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDynabindSectionSet recordset

class CDynabindSectionSet : public CRecordset
{
public:
	CDynabindSectionSet(CDatabase* pDatabase);
	DECLARE_DYNAMIC(CDynabindSectionSet)

// Field/Param Data
	//{{AFX_FIELD(CDynabindSectionSet, CRecordset)
	CString m_CourseID;
	CString m_SectionNo;
	CString m_InstructorID;
	CString m_RoomNo;
	CString m_Schedule;
	int     m_Capacity;
	CString m_LabRoomNo;
	CString m_LabSchedule;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDynabindSectionSet)
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
