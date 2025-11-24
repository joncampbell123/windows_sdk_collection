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
#include "afxcoll.h"
#pragma hdrstop

#include <malloc.h>

#ifdef AFX_CORE_SEG
#pragma code_seg(AFX_CORE_SEG)
#endif


#ifndef min
#define min(a,b)        (((a) < (b)) ? (a) : (b))
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

////////////////////////////////////////////////////////////////////////////
// Serialize member functions for low level classes put here
// for code swapping improvements

// CString serialization code
// String format: if < 255 chars: len:BYTE, characters in bytes
//              if >= 255 characters: 0xff, len:WORD, characters in bytes

CArchive&
operator <<(CArchive& ar, const CString& string)
{
	if (string.m_nDataLength < 255)
	{
		ar << (BYTE) string.m_nDataLength;
	}
	else
	{
		ar << (BYTE) 0xff;
		ar << (WORD) string.m_nDataLength;
	}
	ar.Write(string.m_pchData, string.m_nDataLength);
	return ar;
}

CArchive&
operator >>(CArchive& ar, CString& string)
{
	string.Empty();

	BYTE bLen;
	ar >> bLen;

	WORD nNewLen;
	if (bLen == 0xff)
		// read word of length
		ar >> nNewLen;
	else
		nNewLen = bLen;

	// read in as normal characters
	if (nNewLen != 0)
	{
		string.AllocBuffer(nNewLen);
		if (ar.Read(string.m_pchData, nNewLen) != nNewLen)
			AfxThrowArchiveException(CArchiveException::endOfFile);
	}
	return ar;
}

// Runtime class serialization code
CRuntimeClass*
CRuntimeClass::Load(CArchive& ar, UINT* pwSchemaNum)
{
	WORD nLen;
	char szClassName[64];
	CRuntimeClass* pClass;

	ar >> (WORD&)(*pwSchemaNum) >> nLen;

	if (nLen >= sizeof(szClassName) || ar.Read(szClassName, nLen) != nLen)
		return NULL;
	szClassName[nLen] = '\0';

	for (pClass = pFirstClass; pClass != NULL; pClass = pClass->m_pNextClass)
	{
		if (strcmp(szClassName, pClass->m_pszClassName) == 0)
			return pClass;
	}

	return NULL;
}

void
CRuntimeClass::Store(CArchive& ar)
	// Stores a class ref
{
	WORD nLen = (WORD)strlen(m_pszClassName);

	ar << (WORD)m_wSchema << nLen;
	ar.Write(m_pszClassName, nLen);
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// Archive object input/output

	// amount to grow m_loadArray upon insert
	enum { nGrowSize = 10 };

	// minimum buffer size
	enum { nBufSizeMin = 128 };

////////////////////////////////////////////////////////////////////////////
// Pointer mapping constants
#define wNullTag      ((WORD)0)
#define wNewClassTag  ((WORD)-1)
#define wOldClassTag  ((WORD)-32768) /* 0x8000 or the class index with this */
#define nMaxMapCount  ((WORD)32766)  /* 0x7FFE last valid mapCount */


// TRY/CATCH cannot be used with /Ox
#pragma optimize("elg", off)

CArchive::CArchive(CFile* pFile, 
		UINT nMode,
		int nBufSize /* = 512 */,
		void FAR* lpBuf /* = NULL */)
{
	ASSERT_VALID(pFile);

	m_nMode = nMode;

	// initialize the buffer.  minimum size is 128
	m_lpBufStart = (BYTE FAR*)lpBuf;

	if (nBufSize < nBufSizeMin)
	{
		// force use of private buffer of minimum size
		m_nBufSize = nBufSizeMin;
		m_lpBufStart = NULL; 
	}
	else
		m_nBufSize = nBufSize;

	if (m_lpBufStart == NULL)
	{
		m_lpBufStart = (BYTE FAR*)_fmalloc(m_nBufSize);
		m_bUserBuf = FALSE;
	}
	else
		m_bUserBuf = TRUE;

	ASSERT(m_lpBufStart != NULL);
	ASSERT(AfxIsValidAddress(m_lpBufStart, m_nBufSize));

	m_lpBufMax = m_lpBufStart + m_nBufSize;
	m_lpBufCur = (IsLoading()) ? m_lpBufMax : m_lpBufStart;
	m_pFile = pFile;

	// allocate the load/store map/array fail gracefully if OOM
	TRY
	{
		if (nMode == CArchive::load) 
			m_pLoadArray = new CPtrArray;
		else
			m_pStoreMap = new CMapPtrToWord;
	}
	CATCH(CMemoryException, e)
	{
		if (!m_bUserBuf)
			_ffree(m_lpBufStart);
		THROW_LAST();
	}
	END_CATCH

	if (nMode == CArchive::load) 
	{
		ASSERT(IsLoading());
		ASSERT(nGrowSize > 0);
		m_pLoadArray->SetSize(nGrowSize, nGrowSize); 
		ASSERT(wNullTag == 0);
		m_pLoadArray->SetAt(wNullTag, NULL);
		m_nMapCount = 1;
	}
	else
	{
		ASSERT(IsStoring());

		m_pStoreMap->SetAt(NULL, wNullTag);
		m_nMapCount = 1;
	}

}
#pragma optimize("", on)


CArchive::~CArchive()
{
	ASSERT(AfxIsValidAddress(m_lpBufStart, (UINT)(m_lpBufMax - m_lpBufStart)));
	ASSERT(AfxIsValidAddress(m_lpBufCur, (UINT)(m_lpBufMax - m_lpBufCur)));
	ASSERT(m_lpBufStart != NULL);

	// Close makes m_pFile NULL.  If it is not NULL, we must Close the
	// CArchive.
	if (m_pFile)
		Close();

	if (!m_bUserBuf)
		_ffree(m_lpBufStart);

	if (m_nMode == CArchive::load)
		delete m_pLoadArray;
	else
		delete m_pStoreMap;
}

void
CArchive::Close()
{
	ASSERT_VALID(m_pFile);

	Flush();
	m_pFile = NULL;
}

void
CArchive::WriteObject(const CObject* cpOb)
{
	// object can be NULL
	ASSERT(IsStoring());    // proper direction
	ASSERT(m_lpBufStart != NULL);
	ASSERT(m_lpBufCur != NULL);

	CObject* pOb = (CObject*)cpOb;
	WORD nObIndex;

	ASSERT(sizeof(nObIndex) == 2);
	ASSERT(sizeof(wNullTag) == 2);
	ASSERT(sizeof(wNewClassTag) == 2);

	if (pOb == NULL)
		*this << wNullTag;
	else if (!(cpOb->IsSerializable()))
		AfxThrowNotSupportedException();
	else if ((nObIndex = (*m_pStoreMap)[pOb]) != 0) //ASSUME: initialized to 0 map
		*this << nObIndex;
	else
	{
		CRuntimeClass* pClassRef = pOb->GetRuntimeClass();
		WORD nClassIndex;

		// write out class id of pOb, with high bit set to indicate
		// new object follows

		// ASSUME: initialized to 0 map
		if ((nClassIndex = (*m_pStoreMap)[pClassRef]) != 0) 
		{
			// previously seen class, write out the index tagged by high bit
			*this << (WORD)(wOldClassTag | nClassIndex);
		}
		else
		{
			// new class
			*this << wNewClassTag;
			pClassRef->Store(*this);

			(*m_pStoreMap)[pClassRef] = (WORD) m_nMapCount++;
			if (m_nMapCount > nMaxMapCount)
				AfxThrowArchiveException(CArchiveException::badIndex);
		}
		// enter in stored object table and output
		(*m_pStoreMap)[pOb] = (WORD)m_nMapCount++;
		if (m_nMapCount > nMaxMapCount)
			AfxThrowArchiveException(CArchiveException::badIndex);

		pOb->Serialize(*this);
	}
}



CObject*
CArchive::ReadObject(const CRuntimeClass* pClassRefRequested)
{
	ASSERT(pClassRefRequested == NULL || AfxIsValidAddress(pClassRefRequested, sizeof(struct CRuntimeClass)));
	ASSERT(IsLoading());    // proper direction
	ASSERT(wNullTag == 0);
	ASSERT(m_lpBufStart != NULL);
	ASSERT(m_lpBufCur != NULL);

	CRuntimeClass* pClassRef;
	WORD obTag;
	WORD wSchema;

	if (pClassRefRequested && (pClassRefRequested->m_wSchema == 0xFFFF))
		AfxThrowNotSupportedException();
		
	*this >> obTag;

	//NOTE: this relies on signed testing of the tag values
	if ((short)obTag >= (short)wNullTag)
	{
		if (obTag > (WORD)m_pLoadArray->GetUpperBound())
			AfxThrowArchiveException(CArchiveException::badIndex);

		CObject* pOb = (CObject*)m_pLoadArray->GetAt(obTag);

		if (pOb != NULL && pClassRefRequested && !pOb->IsKindOf(pClassRefRequested))
			AfxThrowArchiveException(CArchiveException::badClass);
		return pOb;
	}


	if (obTag == wNewClassTag)
	{
		// new object follows a new class id
		if (m_nMapCount > nMaxMapCount)
			AfxThrowArchiveException(CArchiveException::badIndex);

		if ((pClassRef = CRuntimeClass::Load(*this, (UINT*)&wSchema)) == NULL)
		{
			AfxThrowArchiveException(CArchiveException::badClass);
			return NULL;
		}
		if (pClassRef->m_wSchema != wSchema)
		{
			AfxThrowArchiveException(CArchiveException::badSchema);
			return NULL;
		}
		m_pLoadArray->InsertAt(m_nMapCount++, pClassRef, 1);
		ASSERT(m_nMapCount < (UINT)0x7FFF);
	} 
	else
	{
		// existing class index in obTag followed by new object

		WORD nClassIndex = (WORD)(obTag & (WORD)~wOldClassTag);
		ASSERT(sizeof(nClassIndex) == 2);

		if (nClassIndex & 0x8000 || 
				nClassIndex > (WORD)m_pLoadArray->GetUpperBound())
			AfxThrowArchiveException(CArchiveException::badIndex);

		pClassRef = (CRuntimeClass*)m_pLoadArray->GetAt(nClassIndex);
	}

	// allocate a new object based on the class just acquired
	CObject* pOb = pClassRef->CreateObject();
	ASSERT(pOb != NULL);

	// Add to mapping array BEFORE de-serializing
	m_pLoadArray->InsertAt(m_nMapCount++, pOb, 1);

	pOb->Serialize(*this);

	ASSERT(pOb != NULL);
	if (pClassRefRequested && !pOb->IsKindOf(pClassRefRequested))
		AfxThrowArchiveException(CArchiveException::badClass);

	return pOb;
}


UINT
CArchive::Read(void FAR* lpBuf, UINT nMax)
{
	ASSERT_VALID(m_pFile);
	ASSERT(lpBuf != NULL);
	ASSERT(m_lpBufStart != NULL);
	ASSERT(m_lpBufCur != NULL);
	ASSERT(AfxIsValidAddress(lpBuf, nMax));
	ASSERT(AfxIsValidAddress(m_lpBufStart, (UINT)(m_lpBufMax - m_lpBufStart)));
	ASSERT(AfxIsValidAddress(m_lpBufCur, (UINT)(m_lpBufMax - m_lpBufCur)));
	ASSERT(IsLoading());

	register UINT nRead = 0;

	if (nMax == 0)
		return 0;

	while (nMax > 0)
	{
		UINT nCopy = min(nMax, (UINT)(m_lpBufMax - m_lpBufCur));
		_fmemcpy(lpBuf, m_lpBufCur, nCopy);
		m_lpBufCur += nCopy;
		lpBuf = ((BYTE FAR*)lpBuf) + nCopy;
		nMax -= nCopy;
		nRead += nCopy;
		if (nMax != 0)
			FillBuffer(min(nMax, (UINT)m_nBufSize));
	}
	return nRead;
}

void
CArchive::Write(const void FAR* lpBuf, UINT nMax)
{
	ASSERT_VALID(m_pFile);
	ASSERT(m_lpBufStart != NULL);
	ASSERT(m_lpBufCur != NULL);
	ASSERT(AfxIsValidAddress(lpBuf, nMax));
	ASSERT(AfxIsValidAddress(m_lpBufStart, (UINT)(m_lpBufMax - m_lpBufStart)));
	ASSERT(AfxIsValidAddress(m_lpBufCur, (UINT)(m_lpBufMax - m_lpBufCur)));
	ASSERT(IsStoring());

	register void FAR* lpBufT = (void FAR*)lpBuf;

	while (nMax > 0)
	{
		UINT nCopy = min(nMax, (UINT)(m_lpBufMax - m_lpBufCur));
		_fmemcpy(m_lpBufCur, lpBufT, nCopy);
		m_lpBufCur += nCopy;
		lpBufT = ((BYTE FAR*)lpBufT) + nCopy;
		nMax -= nCopy;
		if (nMax != 0)
		{
			// write out the current buffer to file
			if (m_lpBufCur != m_lpBufStart)
				m_pFile->Write(m_lpBufStart, m_lpBufCur - m_lpBufStart);

			// restore buffer to initial state
			m_lpBufCur = m_lpBufStart;
		}
	}
}


void
CArchive::Flush()
{
	ASSERT(m_lpBufStart != NULL);
	ASSERT(m_lpBufCur != NULL);
	ASSERT_VALID(m_pFile);
	ASSERT(m_lpBufStart != NULL);
	ASSERT(m_lpBufCur != NULL);
	ASSERT(AfxIsValidAddress(m_lpBufStart, (UINT)(m_lpBufMax - m_lpBufStart)));
	ASSERT(AfxIsValidAddress(m_lpBufCur, (UINT)(m_lpBufMax - m_lpBufCur)));

	if (IsLoading())
	{
		// unget the characters in the buffer, seek back unused amount
		m_pFile->Seek(-(m_lpBufMax - m_lpBufCur), CFile::current);
		m_lpBufCur = m_lpBufMax;    // empty
	}
	else
	{
		// write out the current buffer to file
		if (m_lpBufCur != m_lpBufStart)
		{
			m_pFile->Write(m_lpBufStart, m_lpBufCur - m_lpBufStart);
			m_pFile->Flush();
		}

		// restore buffer to initial state
		m_lpBufCur = m_lpBufStart;
	}
}

void 
CArchive::FillBuffer(UINT nBytesNeeded)
{
	ASSERT(IsLoading());
	ASSERT_VALID(m_pFile);
	ASSERT(m_lpBufStart != NULL);
	ASSERT(m_lpBufCur != NULL);
	ASSERT(nBytesNeeded > 0);
	ASSERT(AfxIsValidAddress(m_lpBufStart, (UINT)(m_lpBufMax - m_lpBufStart)));
	ASSERT(AfxIsValidAddress(m_lpBufCur, (UINT)(m_lpBufMax - m_lpBufCur)));


	// fill up the current buffer from file
	if (m_lpBufCur > m_lpBufStart)
	{
		// there is at least some room to fill
		UINT nUnused = 0; // bytes remaining in buffer
		UINT nActual = 0; // bytes read from file

		if ((nUnused = m_lpBufMax - m_lpBufCur) > 0)
		{
			_fmemcpy(m_lpBufStart, m_lpBufCur, m_lpBufMax - m_lpBufCur);    // copy unused
		}

		nActual = m_pFile->Read(m_lpBufStart+nUnused, m_nBufSize-nUnused);

		if (nActual < nBytesNeeded)
			// not enough data to fill request
			AfxThrowArchiveException(CArchiveException::endOfFile);

		m_lpBufCur = m_lpBufStart;
		m_lpBufMax = m_lpBufStart + nUnused + nActual;
	}
}
