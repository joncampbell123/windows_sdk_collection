// This is a part of the Microsoft Foundation Classes C++ library. 
// Copyright (C) 1992 Microsoft Corporation 
// All rights reserved. 
//  
// This source code is only intended as a supplement to the 
// Microsoft Foundation Classes Reference and Microsoft 
// QuickHelp documentation provided with the library. 
// See these sources for detailed information regarding the 
// Microsoft Foundation Classes product. 

#include "afxwin.h"
#pragma hdrstop

#ifdef AFX_CORE_SEG
#pragma code_seg(AFX_CORE_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Windows extensions to strings

BOOL CString::LoadString(UINT nID)
{
	// NOTE: will not work for strings > 255 characters
	char szBuffer[256];
	UINT nSize;
	if ((nSize = ::LoadString(AfxGetResourceHandle(),
			nID, szBuffer, sizeof(szBuffer)-1)) == 0)
		return FALSE;
	AssignCopy(nSize, szBuffer);
	return TRUE;
}

void CString::AnsiToOem()
{
	::AnsiToOem(m_pchData, m_pchData);
}

void CString::OemToAnsi()
{
	::OemToAnsi(m_pchData, m_pchData);
}

/////////////////////////////////////////////////////////////////////////////
