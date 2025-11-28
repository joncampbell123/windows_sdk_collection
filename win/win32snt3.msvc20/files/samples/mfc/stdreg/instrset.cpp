// instrset.cpp : implementation file
//

#include "stdafx.h"
#include "stdreg.h"
#include "instrset.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInstructorSet

IMPLEMENT_DYNAMIC(CInstructorSet, CRecordset)

CInstructorSet::CInstructorSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CInstructorSet)
	m_InstructorID = "";
	m_Name = "";
	m_RoomNo = "";
	m_nFields = 3;
	//}}AFX_FIELD_INIT
}


CString CInstructorSet::GetDefaultConnect()
{
	return "ODBC;DSN=Student Registration;";
}

CString CInstructorSet::GetDefaultSQL()
{
	return "INSTRUCTOR";
}

void CInstructorSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CInstructorSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Text(pFX, "InstructorID", m_InstructorID);
	RFX_Text(pFX, "Name", m_Name);
	RFX_Text(pFX, "RoomNo", m_RoomNo);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CInstructorSet diagnostics

#ifdef _DEBUG
void CInstructorSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CInstructorSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
