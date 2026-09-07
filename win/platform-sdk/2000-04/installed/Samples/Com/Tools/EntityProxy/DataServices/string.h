#ifndef __CSTRING_H
#define __CSTRING_H

struct CStringData
{
	long nRefs;             // reference count
	int nDataLength;        // length of data (including terminator)
	int nAllocLength;       // length of allocation
	// WCHAR data[nAllocLength]

	WCHAR* data()           // WCHAR* to managed data
		{ return (WCHAR*)(this+1); }
};

class CString
{
	DECLARE_FIXED_ALLOC(CString);

public:

// Constructors

	// constructs empty CString
	CString();
	// copy constructor
	CString(const CString& stringSrc);
	// from a single character
	CString(WCHAR ch, int nRepeat = 1);
	// from an ANSI string (converts to WCHAR)
	CString(LPCSTR lpsz);
	// from a UNICODE string (converts to WCHAR)
	CString(LPCWSTR lpsz);
	// subset of characters from an ANSI string (converts to WCHAR)
	CString(LPCSTR lpch, int nLength);
	// subset of characters from a UNICODE string (converts to WCHAR)
	CString(LPCWSTR lpch, int nLength);
	// from unsigned characters
	CString(const unsigned char* psz);

// Attributes & Operations

	// get data length
	int GetLength() const;
	// TRUE if zero length
	BOOL IsEmpty() const;
	// clear contents to empty
	void Empty();

	// return single character at zero-based index
	WCHAR GetAt(int nIndex) const;
	// return single character at zero-based index
	WCHAR operator[](int nIndex) const;
	// set a single character at zero-based index
	void SetAt(int nIndex, WCHAR ch);
	// return pointer to const string
	operator LPCWSTR() const;

	// overloaded assignment

	// ref-counted copy from another CString
	const CString& operator=(const CString& stringSrc);
	// set string content to single character
	const CString& operator=(WCHAR ch);
#ifdef _UNICODE
	const CString& operator=(char ch);
#endif
	// copy string content from ANSI string (converts to WCHAR)
	const CString& operator=(LPCSTR lpsz);
	// copy string content from UNICODE string (converts to WCHAR)
	inline const CString& operator=(LPCWSTR lpsz)
	{
		AssignCopy(SafeStrlen(lpsz), lpsz);
		return *this;
	}

	// copy string content from unsigned chars
	const CString& operator=(const unsigned char* psz);

	// string concatenation

	// concatenate from another CString
	const CString& operator+=(const CString& string);

	// concatenate a single character
	const CString& operator+=(WCHAR ch);
#ifdef _UNICODE
	// concatenate an ANSI character after converting it to WCHAR
	const CString& operator+=(char ch);
#endif
	// concatenate a UNICODE character after converting it to WCHAR
	const CString& operator+=(LPCWSTR lpsz);

	friend CString operator+(const CString& string1,
			const CString& string2);
	friend CString operator+(const CString& string, WCHAR ch);
	friend CString operator+(WCHAR ch, const CString& string);
#ifdef _UNICODE
	friend CString operator+(const CString& string, char ch);
	friend CString operator+(char ch, const CString& string);
#endif
	friend CString operator+(const CString& string, LPCWSTR lpsz);
	friend CString operator+(LPCWSTR lpsz, const CString& string);

	// string comparison

	// straight character comparison
	int Compare(LPCWSTR lpsz) const;

	// Access to string implementation buffer as "C" character array
	// get pointer to modifiable buffer at least as long as nMinBufLength
	LPWSTR GetBuffer(int nMinBufLength);
	// release buffer, setting length to nNewLength (or to first nul if -1)
	void ReleaseBuffer(int nNewLength = -1);
	// get pointer to modifiable buffer exactly as long as nNewLength
	LPWSTR GetBufferSetLength(int nNewLength);
	// release memory allocated to but unused by string
	void FreeExtra();

	// Use LockBuffer/UnlockBuffer to turn refcounting off

	// turn refcounting back on
	LPWSTR LockBuffer();
	// turn refcounting off
	void UnlockBuffer();

// Implementation
public:
	inline ~CString();
	int GetAllocLength() const;
	inline void AssignCopy(int nSrcLen, LPCWSTR lpszSrcData)
	{
		AllocBeforeWrite(nSrcLen);
	//	memcpy(m_pchData, lpszSrcData, nSrcLen*sizeof(WCHAR));
	//	m_pchData[nSrcLen] = L'\0';
		WCSNCPY(m_pchData, lpszSrcData, nSrcLen);
		GetData()->nDataLength = nSrcLen;
	}

	LPWSTR m_pchData;   // pointer to ref counted string data

protected:

	// implementation helpers
	inline CStringData* GetData() const;
	void Init();
	void AllocCopy(CString& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const;
	void AllocBuffer(int nLen);
	void ConcatCopy(int nSrc1Len, LPCWSTR lpszSrc1Data, int nSrc2Len, LPCWSTR lpszSrc2Data);
	void ConcatInPlace(int nSrcLen, LPCWSTR lpszSrcData);
	void CopyBeforeWrite();
	inline void AllocBeforeWrite(int nLen)
	{
		if (GetData()->nRefs > 1 || nLen > GetData()->nAllocLength)
		{
			Release();
			AllocBuffer(nLen);
		}
	}

	void Release();
	static void Release(CStringData* pData);
	static int SafeStrlen(LPCWSTR lpsz);
	static void FreeData(CStringData* pData);
};

// Compare helpers
bool operator==(const CString& s1, const CString& s2);
bool operator==(const CString& s1, LPCWSTR s2);
bool operator==(LPCWSTR s1, const CString& s2);
bool operator!=(const CString& s1, const CString& s2);
bool operator!=(const CString& s1, LPCWSTR s2);
bool operator!=(LPCWSTR s1, const CString& s2);
bool operator<(const CString& s1, const CString& s2);
bool operator<(const CString& s1, LPCWSTR s2);
bool operator<(LPCWSTR s1, const CString& s2);
bool operator>(const CString& s1, const CString& s2);
bool operator>(const CString& s1, LPCWSTR s2);
bool operator>(LPCWSTR s1, const CString& s2);
bool operator<=(const CString& s1, const CString& s2);
bool operator<=(const CString& s1, LPCWSTR s2);
bool operator<=(LPCWSTR s1, const CString& s2);
bool operator>=(const CString& s1, const CString& s2);
bool operator>=(const CString& s1, LPCWSTR s2);
bool operator>=(LPCWSTR s1, const CString& s2);

// Globals
extern const WCHAR epChNil;
extern LPCWSTR _epPchNil;
inline const CString& EpGetEmptyString()
{
	return *(CString*)&_epPchNil;
}
#define epEmptyString EpGetEmptyString()

inline CStringData* CString::GetData() const
{
	return ((CStringData*)m_pchData)-1;
}

inline void CString::Init()
{
	m_pchData = epEmptyString.m_pchData;
}

inline CString::CString()
{
	m_pchData = epEmptyString.m_pchData;
}

inline int CString::GetLength() const
	{ return GetData()->nDataLength; }
inline int CString::GetAllocLength() const
	{ return GetData()->nAllocLength; }
inline BOOL CString::IsEmpty() const
	{ return GetData()->nDataLength == 0; }
inline CString::operator LPCWSTR() const
	{ return m_pchData; }
inline int CString::SafeStrlen(LPCWSTR lpsz)
	{ return (lpsz == NULL) ? 0 : wcslen(lpsz); }

// CString support (windows specific)
inline int CString::Compare(LPCWSTR lpsz) const
	{ return WCSCMP(m_pchData, lpsz); }    // MBCS/Unicode aware

inline WCHAR CString::GetAt(int nIndex) const
{
	return m_pchData[nIndex];
}
inline WCHAR CString::operator[](int nIndex) const
{
	// same as GetAt
	return m_pchData[nIndex];
}
inline bool operator==(const CString& s1, const CString& s2)
	{ return s1.Compare(s2) == 0; }
inline bool operator==(const CString& s1, LPCWSTR s2)
	{ return s1.Compare(s2) == 0; }
inline bool operator==(LPCWSTR s1, const CString& s2)
	{ return s2.Compare(s1) == 0; }
inline bool operator!=(const CString& s1, const CString& s2)
	{ return s1.Compare(s2) != 0; }
inline bool operator!=(const CString& s1, LPCWSTR s2)
	{ return s1.Compare(s2) != 0; }
inline bool operator!=(LPCWSTR s1, const CString& s2)
	{ return s2.Compare(s1) != 0; }
inline bool operator<(const CString& s1, const CString& s2)
	{ return s1.Compare(s2) < 0; }
inline bool operator<(const CString& s1, LPCWSTR s2)
	{ return s1.Compare(s2) < 0; }
inline bool operator<(LPCWSTR s1, const CString& s2)
	{ return s2.Compare(s1) > 0; }
inline bool operator>(const CString& s1, const CString& s2)
	{ return s1.Compare(s2) > 0; }
inline bool operator>(const CString& s1, LPCWSTR s2)
	{ return s1.Compare(s2) > 0; }
inline bool operator>(LPCWSTR s1, const CString& s2)
	{ return s2.Compare(s1) < 0; }
inline bool operator<=(const CString& s1, const CString& s2)
	{ return s1.Compare(s2) <= 0; }
inline bool operator<=(const CString& s1, LPCWSTR s2)
	{ return s1.Compare(s2) <= 0; }
inline bool operator<=(LPCWSTR s1, const CString& s2)
	{ return s2.Compare(s1) >= 0; }
inline bool operator>=(const CString& s1, const CString& s2)
	{ return s1.Compare(s2) >= 0; }
inline bool operator>=(const CString& s1, LPCWSTR s2)
	{ return s1.Compare(s2) >= 0; }
inline bool operator>=(LPCWSTR s1, const CString& s2)
	{ return s2.Compare(s1) <= 0; }


#endif