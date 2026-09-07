#include "stdafx.h"
#include "heap.h"

IMPLEMENT_FIXED_ALLOC(CString, 100);

/////////////////////////////////////////////////////////////////////////////
// static class data, special inlines

const WCHAR epChNil = L'\0';
static int _epInitData[] = { -1, 0, 0, 0 };
static CStringData* _epDataNil = (CStringData*)&_epInitData;
LPCWSTR _epPchNil = (LPCWSTR)(((BYTE*)&_epInitData)+sizeof(CStringData));

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction

CString::CString(const CString& stringSrc)
{
	if (stringSrc.GetData()->nRefs >= 0)
	{
		m_pchData = stringSrc.m_pchData;
		InterlockedIncrement(&GetData()->nRefs);
	}
	else
	{
		Init();
		*this = stringSrc.m_pchData;
	}
}

CString::CString(WCHAR ch, int nLength)
{
	Init();
	if (nLength >= 1)
	{
		AllocBuffer(nLength);
		for (int i = 0; i < nLength; i++)
			m_pchData[i] = ch;
	}
}

CString::CString(LPCWSTR lpch, int nLength)
{
	Init();
	if (nLength != 0)
	{
		AllocBuffer(nLength);
		memcpy(m_pchData, lpch, nLength*sizeof(WCHAR));
	}
}

//////////////////////////////////////////////////////////////////////////////
// Assignment operators

const CString& CString::operator=(WCHAR ch)
{
	AssignCopy(1, &ch);
	return *this;
}

#pragma warning(disable: 4074)
#pragma init_seg(compiler)

#define ROUND(x,y) (((x)+(y-1))&~(y-1))
#define ROUND4(x) ROUND(x, 4)
static CFixedAlloc _epAlloc6(ROUND4(7*sizeof(WCHAR)+sizeof(CStringData)));
static CFixedAlloc _epAlloc7(ROUND4(8*sizeof(WCHAR)+sizeof(CStringData)));
static CFixedAlloc _epAlloc8(ROUND4(9*sizeof(WCHAR)+sizeof(CStringData)));
static CFixedAlloc _epAlloc9(ROUND4(10*sizeof(WCHAR)+sizeof(CStringData)));
static CFixedAlloc _epAlloc10(ROUND4(11*sizeof(WCHAR)+sizeof(CStringData)));
static CFixedAlloc _epAlloc11(ROUND4(12*sizeof(WCHAR)+sizeof(CStringData)));
static CFixedAlloc _epAlloc12(ROUND4(13*sizeof(WCHAR)+sizeof(CStringData)));
static CFixedAlloc _epAlloc16(ROUND4(17*sizeof(WCHAR)+sizeof(CStringData)));
static CFixedAlloc _epAlloc20(ROUND4(21*sizeof(WCHAR)+sizeof(CStringData)));
static CFixedAlloc _epAlloc24(ROUND4(25*sizeof(WCHAR)+sizeof(CStringData)));
static CFixedAlloc _epAlloc32(ROUND4(33*sizeof(WCHAR)+sizeof(CStringData)));
static CFixedAlloc _epAlloc64(ROUND4(65*sizeof(WCHAR)+sizeof(CStringData)));
static CFixedAlloc _epAlloc128(ROUND4(129*sizeof(WCHAR)+sizeof(CStringData)));
static CFixedAlloc _epAlloc256(ROUND4(257*sizeof(WCHAR)+sizeof(CStringData)));
static CFixedAlloc _epAlloc512(ROUND4(513*sizeof(WCHAR)+sizeof(CStringData)));

void CString::AllocBuffer(int nLen)
// always allocate one extra character for '\0' termination
// assumes [optimistically] that data length will equal allocation length
{
	if (nLen == 0)
		Init();
	else
	{
		CStringData* pData;
		if (nLen <= 6)
		{
			pData = (CStringData*)_epAlloc6.Alloc();
			pData->nAllocLength = 6;
		}
		else if (nLen <= 7)
		{
			pData = (CStringData*)_epAlloc7.Alloc();
			pData->nAllocLength = 7;
		}
		else if (nLen <= 8)
		{
			pData = (CStringData*)_epAlloc8.Alloc();
			pData->nAllocLength = 8;
		}
		else if (nLen <= 9)
		{
			pData = (CStringData*)_epAlloc9.Alloc();
			pData->nAllocLength = 9;
		}
		else if (nLen <= 10)
		{
			pData = (CStringData*)_epAlloc10.Alloc();
			pData->nAllocLength = 10;
		}
		else if (nLen <= 11)
		{
			pData = (CStringData*)_epAlloc11.Alloc();
			pData->nAllocLength = 11;
		}
		else if (nLen <= 12)
		{
			pData = (CStringData*)_epAlloc12.Alloc();
			pData->nAllocLength = 12;
		}
		else if (nLen <= 16)
		{
			pData = (CStringData*)_epAlloc16.Alloc();
			pData->nAllocLength = 16;
		}
		else if (nLen <= 20)
		{
			pData = (CStringData*)_epAlloc20.Alloc();
			pData->nAllocLength = 20;
		}
		else if (nLen <= 24)
		{
			pData = (CStringData*)_epAlloc24.Alloc();
			pData->nAllocLength = 24;
		}
		else if (nLen <= 32)
		{
			pData = (CStringData*)_epAlloc32.Alloc();
			pData->nAllocLength = 32;
		}
		else if (nLen <= 64)
		{
			pData = (CStringData*)_epAlloc64.Alloc();
			pData->nAllocLength = 64;
		}
		else if (nLen <= 128)
		{
			pData = (CStringData*)_epAlloc128.Alloc();
			pData->nAllocLength = 128;
		}
		else if (nLen <= 256)
		{
			pData = (CStringData*)_epAlloc256.Alloc();
			pData->nAllocLength = 256;
		}
		else if (nLen <= 512)
		{
			pData = (CStringData*)_epAlloc512.Alloc();
			pData->nAllocLength = 512;
		}
		else
		{
			pData = (CStringData*)
				new BYTE[sizeof(CStringData) + (nLen+1)*sizeof(WCHAR)];
			pData->nAllocLength = nLen;
		}
		pData->nRefs = 1;
		pData->data()[nLen] = L'\0';
		pData->nDataLength = nLen;
		m_pchData = pData->data();
	}
}

void CString::FreeData(CStringData* pData)
{
	switch (pData->nAllocLength)
	{
		case 6:
		{
			_epAlloc6.Free(pData);
			break;
		}
		case 7:
		{
			_epAlloc7.Free(pData);
			break;
		}
		case 8:
		{
			_epAlloc8.Free(pData);
			break;
		}
		case 9:
		{
			_epAlloc9.Free(pData);
			break;
		}
		case 10:
		{
			_epAlloc10.Free(pData);
			break;
		}
		case 11:
		{
			_epAlloc11.Free(pData);
			break;
		}
		case 12:
		{
			_epAlloc12.Free(pData);
			break;
		}
		case 16:
		{
			_epAlloc16.Free(pData);
			break;
		}
		case 20:
		{
			_epAlloc20.Free(pData);
			break;
		}
		case 24:
		{
			_epAlloc24.Free(pData);
			break;
		}
		case 32:
		{
			_epAlloc32.Free(pData);
			break;
		}
		case 64:
		{
			_epAlloc64.Free(pData);
			break;
		}
		case 128:
		{
			_epAlloc128.Free(pData);
			break;
		}
		case 256:
		{
			_epAlloc256.Free(pData);
			break;
		}
		case 512:
		{
			_epAlloc512.Free(pData);
			break;
		}
		default:
		{
			delete[] (BYTE*)pData;
			break;
		}
	}
}

void CString::Release()
{
	if (GetData() != _epDataNil)
	{
		if (InterlockedDecrement(&GetData()->nRefs) <= 0)
			FreeData(GetData());
		Init();
	}
}

void CString::Release(CStringData* pData)
{
	if (pData != _epDataNil)
	{
		if (InterlockedDecrement(&pData->nRefs) <= 0)
			FreeData(pData);
	}
}

void CString::Empty()
{
	if (GetData()->nDataLength == 0)
		return;
	if (GetData()->nRefs >= 0)
		Release();
	else
		*this = &epChNil;
}

void CString::CopyBeforeWrite()
{
	if (GetData()->nRefs > 1)
	{
		CStringData* pData = GetData();
		Release();
		AllocBuffer(pData->nDataLength);
		memcpy(m_pchData, pData->data(), (pData->nDataLength+1)*sizeof(WCHAR));
	}
}

inline CString::~CString()
//  free any attached data
{
	if (GetData() != _epDataNil)
	{
		if (InterlockedDecrement(&GetData()->nRefs) <= 0)
			FreeData(GetData());
	}
}

//////////////////////////////////////////////////////////////////////////////
// Helpers for the rest of the implementation

void CString::AllocCopy(CString& dest, int nCopyLen, int nCopyIndex,
	 int nExtraLen) const
{
	// will clone the data attached to this string
	// allocating 'nExtraLen' characters
	// Places results in uninitialized string 'dest'
	// Will copy the part or all of original data to start of new string

	int nNewLen = nCopyLen + nExtraLen;
	if (nNewLen == 0)
	{
		dest.Init();
	}
	else
	{
		dest.AllocBuffer(nNewLen);
		memcpy(dest.m_pchData, m_pchData+nCopyIndex, nCopyLen*sizeof(WCHAR));
	}
}

//////////////////////////////////////////////////////////////////////////////
// More sophisticated construction

CString::CString(LPCWSTR lpsz)
{
	Init();
	int nLen = SafeStrlen(lpsz);
	if (nLen != 0)
	{
		AllocBuffer(nLen);
		memcpy(m_pchData, lpsz, nLen*sizeof(WCHAR));
	}
}

//////////////////////////////////////////////////////////////////////////////
// Assignment operators
const CString& CString::operator=(const CString& stringSrc)
{
	if (m_pchData != stringSrc.m_pchData)
	{
		if ((GetData()->nRefs < 0 && GetData() != _epDataNil) ||
			stringSrc.GetData()->nRefs < 0)
		{
			// actual copy necessary since one of the strings is locked
			AssignCopy(stringSrc.GetData()->nDataLength, stringSrc.m_pchData);
		}
		else
		{
			// can just copy references around
			Release();
			m_pchData = stringSrc.m_pchData;
			InterlockedIncrement(&GetData()->nRefs);
		}
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
// concatenation

// NOTE: "operator+" is done as friend functions for simplicity
//      There are three variants:
//          CString + CString
// and for ? = WCHAR, LPCWSTR
//          CString + ?
//          ? + CString

void CString::ConcatCopy(int nSrc1Len, LPCWSTR lpszSrc1Data,
	int nSrc2Len, LPCWSTR lpszSrc2Data)
{
  // -- master concatenation routine
  // Concatenate two sources
  // -- assume that 'this' is a new CString object

	int nNewLen = nSrc1Len + nSrc2Len;
	if (nNewLen != 0)
	{
		AllocBuffer(nNewLen);
		memcpy(m_pchData, lpszSrc1Data, nSrc1Len*sizeof(WCHAR));
		memcpy(m_pchData+nSrc1Len, lpszSrc2Data, nSrc2Len*sizeof(WCHAR));
	}
}

CString operator+(const CString& string1, const CString& string2)
{
	CString s;
	s.ConcatCopy(string1.GetData()->nDataLength, string1.m_pchData,
		string2.GetData()->nDataLength, string2.m_pchData);
	return s;
}

CString operator+(const CString& string, LPCWSTR lpsz)
{
	CString s;
	s.ConcatCopy(string.GetData()->nDataLength, string.m_pchData,
		CString::SafeStrlen(lpsz), lpsz);
	return s;
}

CString operator+(LPCWSTR lpsz, const CString& string)
{
	CString s;
	s.ConcatCopy(CString::SafeStrlen(lpsz), lpsz, string.GetData()->nDataLength,
		string.m_pchData);
	return s;
}

CString operator+(const CString& string1, WCHAR ch)
{
	CString s;
	s.ConcatCopy(string1.GetData()->nDataLength, string1.m_pchData, 1, &ch);
	return s;
}

CString operator+(WCHAR ch, const CString& string)
{
	CString s;
	s.ConcatCopy(1, &ch, string.GetData()->nDataLength, string.m_pchData);
	return s;
}

//////////////////////////////////////////////////////////////////////////////
// concatenate in place

void CString::ConcatInPlace(int nSrcLen, LPCWSTR lpszSrcData)
{
	//  -- the main routine for += operators

	// concatenating an empty string is a no-op!
	if (nSrcLen == 0)
		return;

	// if the buffer is too small, or we have a width mis-match, just
	//   allocate a new buffer (slow but sure)
	if (GetData()->nRefs > 1 || GetData()->nDataLength + nSrcLen > GetData()->nAllocLength)
	{
		// we have to grow the buffer, use the ConcatCopy routine
		CStringData* pOldData = GetData();
		ConcatCopy(GetData()->nDataLength, m_pchData, nSrcLen, lpszSrcData);
		CString::Release(pOldData);
	}
	else
	{
		// fast concatenation when buffer big enough
		memcpy(m_pchData+GetData()->nDataLength, lpszSrcData, nSrcLen*sizeof(WCHAR));
		GetData()->nDataLength += nSrcLen;
		m_pchData[GetData()->nDataLength] = L'\0';
	}
}

const CString& CString::operator+=(LPCWSTR lpsz)
{
	ConcatInPlace(SafeStrlen(lpsz), lpsz);
	return *this;
}

const CString& CString::operator+=(WCHAR ch)
{
	ConcatInPlace(1, &ch);
	return *this;
}

const CString& CString::operator+=(const CString& string)
{
	ConcatInPlace(string.GetData()->nDataLength, string.m_pchData);
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// Advanced direct buffer access

LPWSTR CString::GetBuffer(int nMinBufLength)
{
	if (GetData()->nRefs > 1 || nMinBufLength > GetData()->nAllocLength)
	{
		// we have to grow the buffer
		CStringData* pOldData = GetData();
		int nOldLen = GetData()->nDataLength;   // AllocBuffer will tromp it
		if (nMinBufLength < nOldLen)
			nMinBufLength = nOldLen;
		AllocBuffer(nMinBufLength);
		memcpy(m_pchData, pOldData->data(), (nOldLen+1)*sizeof(WCHAR));
		GetData()->nDataLength = nOldLen;
		CString::Release(pOldData);
	}
	// return a pointer to the character storage for this string
	return m_pchData;
}

void CString::ReleaseBuffer(int nNewLength)
{
	CopyBeforeWrite();  // just in case GetBuffer was not called

	if (nNewLength == -1)
		nNewLength = wcslen(m_pchData); // zero terminated

	GetData()->nDataLength = nNewLength;
	m_pchData[nNewLength] = L'\0';
}

LPWSTR CString::GetBufferSetLength(int nNewLength)
{
	GetBuffer(nNewLength);
	GetData()->nDataLength = nNewLength;
	m_pchData[nNewLength] = L'\0';
	return m_pchData;
}

void CString::FreeExtra()
{
	if (GetData()->nDataLength != GetData()->nAllocLength)
	{
		CStringData* pOldData = GetData();
		AllocBuffer(GetData()->nDataLength);
		memcpy(m_pchData, pOldData->data(), pOldData->nDataLength*sizeof(WCHAR));
		CString::Release(pOldData);
	}
}

LPWSTR CString::LockBuffer()
{
	LPWSTR lpsz = GetBuffer(0);
	GetData()->nRefs = -1;
	return lpsz;
}

void CString::UnlockBuffer()
{
	if (GetData() != _epDataNil)
		GetData()->nRefs = 1;
}
///////////////////////////////////////////////////////////////////////////////
