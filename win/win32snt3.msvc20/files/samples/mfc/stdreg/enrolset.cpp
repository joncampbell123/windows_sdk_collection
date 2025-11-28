// enrollset.cpp : implementation file
//

#include "stdafx.h"
#include "stdreg.h"
#include "enrolset.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnrollmentSet

IMPLEMENT_DYNAMIC(CEnrollmentSet, CRecordset)

CEnrollmentSet::CEnrollmentSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CEnrollmentSet)
	m_StudentID = 0;
	m_CourseID = "";
	m_SectionNo = "";
	m_Grade = "";
	m_nFields = 4;
	//}}AFX_FIELD_INIT
}


CString CEnrollmentSet::GetDefaultConnect()
{
	return "ODBC;DSN=Student Registration;";
}

CString CEnrollmentSet::GetDefaultSQL()
{
	return "ENROLLMENT";
}

void CEnrollmentSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CEnrollmentSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, "StudentID", m_StudentID);
	RFX_Text(pFX, "CourseID", m_CourseID);
	RFX_Text(pFX, "SectionNo", m_SectionNo);
	RFX_Text(pFX, "Grade", m_Grade);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CEnrollmentSet diagnostics

#ifdef _DEBUG
void CEnrollmentSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CEnrollmentSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
