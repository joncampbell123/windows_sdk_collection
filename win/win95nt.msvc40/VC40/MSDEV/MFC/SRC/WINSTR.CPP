// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1995 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"

#ifdef AFX_CORE1_SEG
#pragma code_seg(AFX_CORE1_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Windows extensions to strings

BOOL CString::LoadString(UINT nID)
{
	// try buffer size of 256, then larger size until entire string is retrieved
	int nSize = -1;
	int nLen;
	do
	{
		nSize += 256;
		nLen = AfxLoadString(nID, GetBuffer(nSize), nSize+1);
	} while (nLen == nSize);
	ReleaseBuffer();

	return nLen > 0;
}

#ifndef _AFXDLL
int AFXAPI AfxLoadString(UINT nID, LPTSTR lpszBuf, UINT nMaxBuf)
{
	ASSERT(AfxIsValidAddress(lpszBuf, nMaxBuf*sizeof(TCHAR)));
#ifdef _DEBUG
	// LoadString without annoying warning from the Debug kernel if the
	//  segment containing the string is not present
	if (::FindResource(AfxGetResourceHandle(),
	   MAKEINTRESOURCE((nID>>4)+1), RT_STRING) == NULL)
	{
		lpszBuf[0] = '\0';
		return 0; // not found
	}
#endif //_DEBUG
	int nLen = ::LoadString(AfxGetResourceHandle(), nID, lpszBuf, nMaxBuf);
	if (nLen == 0)
		lpszBuf[0] = '\0';
	return nLen;
}
#endif

/////////////////////////////////////////////////////////////////////////////

BOOL AFXAPI AfxExtractSubString(CString& rString, LPCTSTR lpszFullString,
	int iSubString, TCHAR chSep)
{
	if (lpszFullString == NULL)
		return FALSE;

	while (iSubString--)
	{
		lpszFullString = _tcschr(lpszFullString, chSep);
		if (lpszFullString == NULL)
		{
			rString.Empty();        // return empty string as well
			return FALSE;
		}
		lpszFullString++;       // point past the separator
	}
	LPCTSTR lpchEnd = _tcschr(lpszFullString, chSep);
	int nLen = (lpchEnd == NULL) ?
		lstrlen(lpszFullString) : (int)(lpchEnd - lpszFullString);
	ASSERT(nLen >= 0);
	memcpy(rString.GetBufferSetLength(nLen), lpszFullString, nLen*sizeof(TCHAR));
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
