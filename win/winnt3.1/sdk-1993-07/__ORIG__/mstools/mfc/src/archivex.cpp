// This is a part of the Microsoft Foundation Classes C++ library. 
// Copyright (C) 1992 Microsoft Corporation 
// All rights reserved. 
//  
// This source code is only intended as a supplement to the 
// Microsoft Foundation Classes Reference and Microsoft 
// QuickHelp documentation provided with the library. 
// See these sources for detailed information regarding the 
// Microsoft Foundation Classes product. 

#include "afx.h"
#pragma hdrstop

#ifdef AFX_AUX_SEG
#pragma code_seg(AFX_AUX_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG

// character strings to use for dumping CArchiveException
static char BASED_CODE szNone[] = "none";
static char BASED_CODE szGeneric[] = "generic";
static char BASED_CODE szReadOnly[] = "readOnly";
static char BASED_CODE szEndOfFile[] = "endOfFile";
static char BASED_CODE szWriteOnly[] = "writeOnly";
static char BASED_CODE szBadIndex[] = "badIndex";
static char BASED_CODE szBadClass[] = "badClass";
static char BASED_CODE szBadSchema[] = "badSchema";

static char FAR* BASED_CODE rgszCArchiveExceptionCause[] =
{
	szNone,
	szGeneric,
	szReadOnly,
	szEndOfFile,
	szWriteOnly,
	szBadIndex,
	szBadClass,
	szBadSchema,
};

static char BASED_CODE szUnknown[] = "unknown";
#endif 

/////////////////////////////////////////////////////////////////////////////
// CArchiveException

IMPLEMENT_DYNAMIC(CArchiveException, CException)
#define new DEBUG_NEW

#ifdef _DEBUG
void
CArchiveException::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
	dc << " m_cause = ";
	if (m_cause >= 0 &&
		m_cause < sizeof(rgszCArchiveExceptionCause) / sizeof(char FAR*))
	{
		dc << rgszCArchiveExceptionCause[m_cause];
	}
	else
	{
		dc << szUnknown;
	}
}
#endif //_DEBUG

void PASCAL AfxThrowArchiveException(int cause)
{
#ifdef _DEBUG
	TRACE("CArchive exception: ");
	if (cause >= 0 &&
		cause < sizeof(rgszCArchiveExceptionCause) / sizeof(char FAR*))
	{
		afxDump << (char FAR*)rgszCArchiveExceptionCause[cause];
	}
	else
	{
		afxDump << (char FAR*)szUnknown;
	}
	afxDump << "\n";
#endif //_DEBUG

	THROW(new CArchiveException(cause));
}
