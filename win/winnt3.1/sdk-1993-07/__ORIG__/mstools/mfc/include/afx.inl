// Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation,
// All rights reserved.

// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFX_INL__
#define __AFX_INL__

#ifdef _DEBUG
extern char BASED_CODE _afxSzAfxInl[]; // defined in dumpcont.cpp
#undef THIS_FILE
#define THIS_FILE _afxSzAfxInl
#endif

// CObject inline functions
inline CObject::~CObject() 
	{ }

inline void* CObject::operator new(size_t, void* p) 
	{ return p; } 

#ifndef _DEBUG
// _DEBUG versions in memory.cpp
inline void CObject::operator delete(void* p) 
	{ ::operator delete(p); }
inline void* CObject::operator new(size_t nSize) 
	{ return ::operator new(nSize); }
#endif

inline CObject::CObject() 
	{ }
inline CObject::CObject(const CObject& /* objectSrc */) 
	{ }
inline void CObject::operator=(const CObject& /* objectSrc */) 
	{ }
inline void CObject::Serialize(CArchive&)
	{ /* CObject does not defaultly serialize anything */ }



// CException inline functions
inline CExceptionLink::CExceptionLink(CExceptionLink* NEAR& rLinkTop)
	{
		m_pLinkPrev = rLinkTop;
		rLinkTop = this;
	}

inline CMemoryException::CMemoryException() 
	{ }
inline CNotSupportedException::CNotSupportedException() 
	{ }
inline CArchiveException::CArchiveException(int cause /* = CArchiveException::none */)
	{ m_cause = cause; }
inline CFileException::CFileException(int cause /* = CFileException::none */, LONG lOsError /* = -1 */)
	{ m_cause = cause; m_lOsError = lOsError; }


// CFile inline functions
inline DWORD CFile::SeekToEnd()
	{ return this->Seek(0, CFile::end); }
inline void CFile::SeekToBegin()
	{ this->Seek(0, CFile::begin); }

// CString inline functions
inline int CString::GetLength() const
	{ return m_nDataLength; }
inline BOOL CString::IsEmpty() const
	{ return m_nDataLength == 0; }
inline CString::operator const char*() const
	{ return (const char*)m_pchData; }
inline int CString::Compare(const char* psz) const
	{ return strcmp(m_pchData, psz); }
inline int CString::CompareNoCase(const char* psz) const
	{ return _stricmp(m_pchData, psz); }
inline int CString::Collate(const char* psz) const
	{ return strcoll(m_pchData, psz); }
inline void CString::MakeUpper()
	{ _strupr(m_pchData); }
inline void CString::MakeLower()
	{ _strlwr(m_pchData); }
inline void CString::MakeReverse()
	{ _strrev(m_pchData); }
inline char CString::GetAt(int nIndex) const
	{
		ASSERT(nIndex >= 0);
		ASSERT(nIndex < m_nDataLength);

		return m_pchData[nIndex];
	}
inline char CString::operator[](int nIndex) const
	{
		// same as GetAt

		ASSERT(nIndex >= 0);
		ASSERT(nIndex < m_nDataLength);

		return m_pchData[nIndex];
	}
inline void CString::SetAt(int nIndex, char ch)
	{
		ASSERT(nIndex >= 0);
		ASSERT(nIndex < m_nDataLength);
		ASSERT(ch != 0);

		m_pchData[nIndex] = ch;
	}
inline BOOL operator==(const CString& s1, const CString& s2)
	{ return s1.Compare(s2) == 0; }
inline BOOL operator==(const CString& s1, const char* s2)
	{ return s1.Compare(s2) == 0; }
inline BOOL operator==(const char* s1, const CString& s2)
	{ return s2.Compare(s1) == 0; }
inline BOOL operator!=(const CString& s1, const CString& s2)
	{ return s1.Compare(s2) != 0; }
inline BOOL operator!=(const CString& s1, const char* s2)
	{ return s1.Compare(s2) != 0; }
inline BOOL operator!=(const char* s1, const CString& s2)
	{ return s2.Compare(s1) != 0; }
inline BOOL operator<(const CString& s1, const CString& s2)
	{ return s1.Compare(s2) < 0; }
inline BOOL operator<(const CString& s1, const char* s2)
	{ return s1.Compare(s2) < 0; }
inline BOOL operator<(const char* s1, const CString& s2)
	{ return s2.Compare(s1) > 0; }
inline BOOL operator>(const CString& s1, const CString& s2)
	{ return s1.Compare(s2) > 0; }
inline BOOL operator>(const CString& s1, const char* s2)
	{ return s1.Compare(s2) > 0; }
inline BOOL operator>(const char* s1, const CString& s2)
	{ return s2.Compare(s1) < 0; }
inline BOOL operator<=(const CString& s1, const CString& s2)
	{ return s1.Compare(s2) <= 0; }
inline BOOL operator<=(const CString& s1, const char* s2)
	{ return s1.Compare(s2) <= 0; }
inline BOOL operator<=(const char* s1, const CString& s2)
	{ return s2.Compare(s1) >= 0; }
inline BOOL operator>=(const CString& s1, const CString& s2)
	{ return s1.Compare(s2) >= 0; }
inline BOOL operator>=(const CString& s1, const char* s2)
	{ return s1.Compare(s2) >= 0; }
inline BOOL operator>=(const char* s1, const CString& s2)
	{ return s2.Compare(s1) <= 0; }

// CTime and CTimeSpan inline functions
inline CTimeSpan::CTimeSpan() 
	{ }
inline CTimeSpan::CTimeSpan(time_t time) 
	{ m_timeSpan = time; }
inline CTimeSpan::CTimeSpan(LONG lDays, int nHours, int nMins, int nSecs)
	{ m_timeSpan = nSecs + 60* (nMins + 60* (nHours + 24* lDays)); }
inline CTimeSpan::CTimeSpan(const CTimeSpan& timeSpanSrc) 
	{ m_timeSpan = timeSpanSrc.m_timeSpan; }
inline const CTimeSpan& CTimeSpan::operator=(const CTimeSpan& timeSpanSrc)
	{ m_timeSpan = timeSpanSrc.m_timeSpan; return *this; }
inline LONG CTimeSpan::GetDays() const
	{ return m_timeSpan / (24*3600L); }
inline LONG CTimeSpan::GetTotalHours() const
	{ return m_timeSpan/3600; }
inline int CTimeSpan::GetHours() const
	{ return (int)(GetTotalHours() - GetDays()*24); }
inline LONG CTimeSpan::GetTotalMinutes() const
	{ return m_timeSpan/60; }
inline int CTimeSpan::GetMinutes() const
	{ return (int)(GetTotalMinutes() - GetTotalHours()*60); }
inline LONG CTimeSpan::GetTotalSeconds() const
	{ return m_timeSpan; }
inline int CTimeSpan::GetSeconds() const
	{ return (int)(GetTotalSeconds() - GetTotalMinutes()*60); }
inline CTimeSpan CTimeSpan::operator-(CTimeSpan timeSpan) const
	{ return CTimeSpan(m_timeSpan - timeSpan.m_timeSpan); }
inline CTimeSpan CTimeSpan::operator+(CTimeSpan timeSpan) const
	{ return CTimeSpan(m_timeSpan + timeSpan.m_timeSpan); }
inline const CTimeSpan& CTimeSpan::operator+=(CTimeSpan timeSpan)
	{ m_timeSpan += timeSpan.m_timeSpan; return *this; }
inline const CTimeSpan& CTimeSpan::operator-=(CTimeSpan timeSpan)
	{ m_timeSpan -= timeSpan.m_timeSpan; return *this; }
inline BOOL CTimeSpan::operator==(CTimeSpan timeSpan) const
	{ return m_timeSpan == timeSpan.m_timeSpan; }
inline BOOL CTimeSpan::operator!=(CTimeSpan timeSpan) const
	{ return m_timeSpan != timeSpan.m_timeSpan; }
inline BOOL CTimeSpan::operator<(CTimeSpan timeSpan) const
	{ return m_timeSpan < timeSpan.m_timeSpan; }
inline BOOL CTimeSpan::operator>(CTimeSpan timeSpan) const
	{ return m_timeSpan > timeSpan.m_timeSpan; }
inline BOOL CTimeSpan::operator<=(CTimeSpan timeSpan) const
	{ return m_timeSpan <= timeSpan.m_timeSpan; }
inline BOOL CTimeSpan::operator>=(CTimeSpan timeSpan) const
	{ return m_timeSpan >= timeSpan.m_timeSpan; }


inline CTime::CTime()
	{ }
inline CTime::CTime(time_t time) 
	{ m_time = time; }
inline CTime::CTime(const CTime& timeSrc) 
	{ m_time = timeSrc.m_time; }
inline const CTime& CTime::operator=(const CTime& timeSrc)
	{ m_time = timeSrc.m_time; return *this; }
inline const CTime& CTime::operator=(time_t t)
	{ m_time = t; return *this; }
inline time_t   CTime::GetTime() const
	{ return m_time; }
inline int CTime::GetYear() const
	{ return (GetLocalTm(NULL)->tm_year) + 1900; }
inline int CTime::GetMonth() const
	{ return GetLocalTm(NULL)->tm_mon + 1; }
inline int CTime::GetDay() const 
	{ return GetLocalTm(NULL)->tm_mday; }
inline int CTime::GetHour() const
	{ return GetLocalTm(NULL)->tm_hour; }
inline int CTime::GetMinute() const
	{ return GetLocalTm(NULL)->tm_min; }
inline int CTime::GetSecond() const
	{ return GetLocalTm(NULL)->tm_sec; }
inline int CTime::GetDayOfWeek() const  
	{ return GetLocalTm(NULL)->tm_wday + 1; }
inline CTimeSpan CTime::operator-(CTime time) const
	{ return CTimeSpan(m_time - time.m_time); }
inline CTime CTime::operator-(CTimeSpan timeSpan) const
	{ return CTime(m_time - timeSpan.m_timeSpan); }
inline CTime CTime::operator+(CTimeSpan timeSpan) const
	{ return CTime(m_time + timeSpan.m_timeSpan); }
inline const CTime& CTime::operator+=(CTimeSpan timeSpan)
	{ m_time += timeSpan.m_timeSpan; return *this; }
inline const CTime& CTime::operator-=(CTimeSpan timeSpan)
	{ m_time -= timeSpan.m_timeSpan; return *this; }
inline BOOL CTime::operator==(CTime time) const
	{ return m_time == time.m_time; }
inline BOOL CTime::operator!=(CTime time) const
	{ return m_time != time.m_time; }
inline BOOL CTime::operator<(CTime time) const
	{ return m_time < time.m_time; }
inline BOOL CTime::operator>(CTime time) const
	{ return m_time > time.m_time; }
inline BOOL CTime::operator<=(CTime time) const
	{ return m_time <= time.m_time; }
inline BOOL CTime::operator>=(CTime time) const
	{ return m_time >= time.m_time; }


#ifdef _DEBUG
inline CMemoryState::CMemoryState()
	{ m_pBlockHeader = NULL; }
#endif

// CArchive inline functions
inline BOOL CArchive::IsLoading() const
	{ return (m_nMode == CArchive::load); }
inline BOOL CArchive::IsStoring() const
	{ return (m_nMode == CArchive::store); }
inline CFile* CArchive::GetFile() const
	{ return m_pFile; }
inline CArchive& CArchive::operator<<(BYTE by)
	{ if (m_lpBufCur + sizeof(BYTE) > m_lpBufMax) Flush();
		*(BYTE FAR*)m_lpBufCur = by; m_lpBufCur += sizeof(BYTE); return *this; }
inline CArchive& CArchive::operator<<(WORD w)
	{ if (m_lpBufCur + sizeof(WORD) > m_lpBufMax) Flush();
		*(UNALIGNED WORD FAR*)m_lpBufCur = w; m_lpBufCur += sizeof(WORD); return *this; }
inline CArchive& CArchive::operator<<(LONG l)
	{ if (m_lpBufCur + sizeof(LONG) > m_lpBufMax) Flush();
		*(UNALIGNED LONG FAR*)m_lpBufCur = l; m_lpBufCur += sizeof(LONG); return *this; }
inline CArchive& CArchive::operator<<(DWORD dw)
	{ if (m_lpBufCur + sizeof(DWORD) > m_lpBufMax) Flush();
		*(UNALIGNED DWORD FAR*)m_lpBufCur = dw; m_lpBufCur += sizeof(DWORD); return *this; }
inline CArchive& CArchive::operator>>(BYTE& by)
	{ if (m_lpBufCur + sizeof(BYTE) > m_lpBufMax)
			FillBuffer(sizeof(BYTE) - (m_lpBufMax - m_lpBufCur));
		by = *(BYTE FAR*)m_lpBufCur; m_lpBufCur += sizeof(BYTE); return *this; }
inline CArchive& CArchive::operator>>(WORD& w)
	{ if (m_lpBufCur + sizeof(WORD) > m_lpBufMax)
			FillBuffer(sizeof(WORD) - (m_lpBufMax - m_lpBufCur));
		w = *(UNALIGNED WORD FAR*)m_lpBufCur; m_lpBufCur += sizeof(WORD); return *this; }
inline CArchive& CArchive::operator>>(DWORD& dw)
	{ if (m_lpBufCur + sizeof(DWORD) > m_lpBufMax)
			FillBuffer(sizeof(DWORD) - (m_lpBufMax - m_lpBufCur));
		dw = *(UNALIGNED DWORD FAR*)m_lpBufCur; m_lpBufCur += sizeof(DWORD); return *this; }
inline CArchive& CArchive::operator>>(LONG& l)
	{ if (m_lpBufCur + sizeof(LONG) > m_lpBufMax)
			FillBuffer(sizeof(LONG) - (m_lpBufMax - m_lpBufCur));
		l = *(UNALIGNED LONG FAR*)m_lpBufCur; m_lpBufCur += sizeof(LONG); return *this; }
inline CArchive::CArchive(const CArchive& /* arSrc */) 
	{ }
inline void CArchive::operator=(const CArchive& /* arSrc */) 
	{ }
inline CArchive& operator<<(CArchive& ar, const CObject* pOb)
	{ ar.WriteObject(pOb); return ar; }
inline CArchive& operator>>(CArchive& ar, CObject*& pOb)
	{ pOb = ar.ReadObject(NULL); return ar; }
inline CArchive& operator>>(CArchive& ar, const CObject*& pOb)
	{ pOb = ar.ReadObject(NULL); return ar; }


// CDumpContext inline functions
inline int CDumpContext::GetDepth() const
	{ return m_nDepth; }
inline void CDumpContext::SetDepth(int nNewDepth)
	{ m_nDepth = nNewDepth; }
inline CDumpContext::CDumpContext(const CDumpContext& /* dcSrc */) 
	{ }
inline void CDumpContext::operator=(const CDumpContext& /* dcSrc */) 
	{ }

#undef THIS_FILE
#define THIS_FILE __FILE__
#endif //__AFX_INL__


