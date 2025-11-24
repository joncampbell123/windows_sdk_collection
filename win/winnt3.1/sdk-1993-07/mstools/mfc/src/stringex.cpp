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
#include <limits.h>

#ifdef AFX_AUX_SEG
#pragma code_seg(AFX_AUX_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

extern char _afxChNil;

//////////////////////////////////////////////////////////////////////////////
// More sophisticated construction

CString::CString(char ch, int nRepeat)
{
	if (nRepeat < 1)
		// return empty string if invalid repeat count
		Init();
	else
	{
		AllocBuffer(nRepeat);
		memset(m_pchData, ch, nRepeat);
	}
}


CString::CString(const char* pch, int nLen)
{
	if (nLen == 0)
		Init();
	else
	{
		AllocBuffer(nLen);
		memcpy(m_pchData, pch, nLen);
	}
}

//////////////////////////////////////////////////////////////////////////////
// Additional constructors for far string data

#ifdef _NEARDATA

CString::CString(const char FAR* lpsz)
{
	int nLen;
	if (lpsz == NULL || (nLen = _fstrlen(lpsz)) == 0)
		Init();
	else
	{
		AllocBuffer(nLen);
		_fmemcpy(m_pchData, lpsz, nLen);
	}
}

CString::CString(const char FAR* lpch, int nLen)
{
	if (nLen == 0)
		Init();
	else
	{
		AllocBuffer(nLen);
		_fmemcpy(m_pchData, lpch, nLen);
	}
}

#endif // need far overloads

//////////////////////////////////////////////////////////////////////////////
// Assignment operators
const CString&
CString::operator =(char ch)
{
	AssignCopy(1, &ch);
	return *this;
}



//////////////////////////////////////////////////////////////////////////////
// concatenation
const CString&
CString::operator +=(char ch)
{
	ConcatInPlace(1, &ch);
	return *this;
}


CString
operator +(const CString& string1, char ch)
{
	CString s;
	s.ConcatCopy(string1.m_nDataLength, string1.m_pchData, 1, &ch);
	return s;
}


CString
operator +(char ch, const CString& string)
{
	CString s;
	s.ConcatCopy(1, &ch, string.m_nDataLength, string.m_pchData);
	return s;
}


//////////////////////////////////////////////////////////////////////////////
// Very simple sub-string extraction

CString
CString::Mid(int nFirst) const
{
	return Mid(nFirst, m_nDataLength - nFirst);
}

CString
CString::Mid(int nFirst, int nCount) const
{
	ASSERT(nFirst >= 0);
	ASSERT(nCount >= 0);

	// out-of-bounds requests return sensible things
	if (nFirst + nCount > m_nDataLength)
		nCount = m_nDataLength - nFirst;
	if (nFirst > m_nDataLength)
		nCount = 0;

	CString dest;
	AllocCopy(dest, nCount, nFirst, 0);
	return dest;
}

CString
CString::Right(int nCount) const
{
	ASSERT(nCount >= 0);

	if (nCount > m_nDataLength)
		nCount = m_nDataLength;

	CString dest;
	AllocCopy(dest, nCount, m_nDataLength-nCount, 0);
	return dest;
}

CString
CString::Left(int nCount) const
{
	ASSERT(nCount >= 0);

	if (nCount > m_nDataLength)
		nCount = m_nDataLength;

	CString dest;
	AllocCopy(dest, nCount, 0, 0);
	return dest;
}

CString
CString::SpanIncluding(const char* pszCharSet) const
{
	// strspn equivalent
	return Left(strspn(m_pchData, pszCharSet));
}


CString
CString::SpanExcluding(const char* pszCharSet) const
{
	// strcspn equivalent
	return Left(strcspn(m_pchData, pszCharSet));
}

//////////////////////////////////////////////////////////////////////////////
// Finding

int CString::Find(char ch) const
{
	// find a single character (strchr)

	register char* psz;
	psz = (char*) strchr(m_pchData, ch);
	return (psz == NULL) ? -1 : psz - m_pchData;
}


int CString::ReverseFind(char ch) const
{
	// find a single character (start backwards, strrchr)

	register char* psz;
	psz = (char*) strrchr(m_pchData, ch);
	return (psz == NULL) ? -1 : psz - m_pchData;
}


int CString::FindOneOf(const char* pszCharSet) const
{
	// like single character find, but look for any of the characters
	// in the string "pszCharSet", like strpbrk

	char* psz = (char*) strpbrk(m_pchData, pszCharSet);
	return (psz == NULL) ? -1 : (psz-m_pchData);
}


int CString::Find(const char* pszSub) const
{
	// find a sub-string (like strstr)

	char* psz = (char*) strstr(m_pchData, pszSub);
	return (psz == NULL) ? -1 : (psz-m_pchData);
}

///////////////////////////////////////////////////////////////////////////////
// Advanced access

char* CString::GetBuffer(int nMinBufLength)
{
	if (nMinBufLength > m_nAllocLength)
	{
		// we have to grow the buffer
		char* pszOldData = m_pchData;
		int nOldLen = m_nDataLength;        // AllocBuffer will tromp it

		AllocBuffer(nMinBufLength);
		memcpy(m_pchData, pszOldData, nOldLen);
		m_nDataLength = nOldLen;
		m_pchData[m_nDataLength] = '\0';

		ASSERT(pszOldData != NULL);
		if (pszOldData != &_afxChNil)
			delete [] pszOldData;
	}

	// return a pointer to the character storage for this string
	return m_pchData;
}

void CString::ReleaseBuffer(int nNewLength)
{
	if (nNewLength == -1)
		nNewLength = strlen(m_pchData); // zero terminated

	ASSERT(nNewLength <= m_nAllocLength);
	m_nDataLength = nNewLength;
	m_pchData[m_nDataLength] = '\0';
}

char* CString::GetBufferSetLength(int nNewLength)
{
	GetBuffer(nNewLength);
	m_nDataLength = nNewLength;
	m_pchData[m_nDataLength] = '\0';
	return m_pchData;
}

///////////////////////////////////////////////////////////////////////////////

