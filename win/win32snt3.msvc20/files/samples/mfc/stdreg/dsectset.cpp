// dsectset.cpp : implementation file
//

#include "stdafx.h"
#include "stdreg.h"
#include "dsectset.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDynabindSectionSet

IMPLEMENT_DYNAMIC(CDynabindSectionSet, CRecordset)

CDynabindSectionSet::CDynabindSectionSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CDynabindSectionSet)
	m_CourseID = "";
	m_SectionNo = "";
	m_InstructorID = "";
	m_RoomNo = "";
	m_Schedule = "";
	m_Capacity = 0;
	m_LabRoomNo = "";
	m_LabSchedule = "";
	m_nFields = 8;
	//}}AFX_FIELD_INIT
}


CString CDynabindSectionSet::GetDefaultConnect()
{
	return "ODBC;DSN=Student Registration;";
}

CString CDynabindSectionSet::GetDefaultSQL()
{
	return "DYNABIND_SECTION";
}

void CDynabindSectionSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CDynabindSectionSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Text(pFX, "CourseID", m_CourseID);
	RFX_Text(pFX, "SectionNo", m_SectionNo);
	RFX_Text(pFX, "InstructorID", m_InstructorID);
	RFX_Text(pFX, "RoomNo", m_RoomNo);
	RFX_Text(pFX, "Schedule", m_Schedule);
	RFX_Int(pFX, "Capacity", m_Capacity);
	RFX_Text(pFX, "LabRoomNo", m_LabRoomNo);
	RFX_Text(pFX, "LabSchedule", m_LabSchedule);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CDynabindSectionSet diagnostics

#ifdef _DEBUG
void CDynabindSectionSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CDynabindSectionSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
