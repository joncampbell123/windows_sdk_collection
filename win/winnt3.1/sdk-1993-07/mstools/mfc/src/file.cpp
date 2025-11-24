// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifdef _WINDOWS
#include "afxwin.h"
#else
#include "afx.h"
#endif
#pragma hdrstop

#include <errno.h>
#include <io.h>
#include <limits.h>
#include <malloc.h>

#ifdef _DOSWIN
#include <sys\types.h>
#include <sys\stat.h>
#endif

#include "dosio_.h"

#ifdef AFX_CORE_SEG
#pragma code_seg(AFX_CORE_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

////////////////////////////////////////////////////////////////////////////

#ifdef _DOSWIN

// DOS INT 21h functions not provided by C Runtimes
// additional Dos calls
#pragma optimize("qgel", off) // assembler cannot be globally optimized

UINT _Afx_seek(UINT hFile, LONG lOff, UINT nMode, DWORD FAR* lpdwNew)
{ 
	UINT res;
	*lpdwNew = -1;
	_asm { 
			mov     bx, hFile
			mov     ax, nMode
			mov     ah, 42h
			mov     dx, word ptr lOff
			mov     cx, word ptr lOff+2
			DOSCALL
			jc      __seek_err
			les     bx, lpdwNew
			mov     word ptr es:[bx], ax
			mov     word ptr es:[bx+2], dx
			xor     ax, ax
	__seek_err:
			mov     res, ax
	}
	return res;
}
		
UINT _Afx_rename(LPCSTR lpszOld, LPCSTR lpszNew)
{ 
	UINT res;
	_asm { 
			mov     bx, ds
			lds     dx, lpszOld
			les     di, lpszNew
			mov     ax, 5600h
			DOSCALL
			jc      __rename_err
			xor     ax, ax
	__rename_err:
			mov     res, ax
			mov     ds, bx
	}
	return res;
}

UINT _Afx_remove(LPCSTR lpsz)  
{ 
	UINT res;
	_asm { 
			mov     bx, ds
			lds     dx, lpsz
			mov     ax, 4100h
			DOSCALL
			jc      __remove_err
			xor     ax, ax
	__remove_err:
			mov     res, ax
			mov     ds, bx
	}
	return res;
}

UINT _Afx_lock(UINT hFile, DWORD dwStart, DWORD dwLen, BOOL bUnlock)
{ 
	UINT res;
	_asm { 
			mov     ax, bUnlock
			mov     ah, 5ch
			mov     bx, hFile
			mov     dx, word ptr dwStart
			mov     cx, word ptr dwStart+2
			mov     di, word ptr dwLen
			mov     si, word ptr dwLen+2
			DOSCALL
			jc      __lock_err1
			xor     ax, ax
	__lock_err1:
			mov     res, ax
			
	}
	return res;
}

#pragma optimize("", on)    // return to default optimizations

#endif // _DOSWIN

/////////////////////////////////////////////////////////////////////////////
// CFileStatus implementation
#ifdef _DEBUG
void
CFileStatus::Dump(CDumpContext& dc) const
{           
	dc << "file status information:";
	dc << "\nm_ctime = " << m_ctime;
	dc << "\nm_mtime = " << m_mtime;
	dc << "\nm_atime = " << m_atime;
	dc << "\nm_size = " << m_size;
	dc << "\nm_attribute = " << m_attribute;
	dc << "\nm_szFullName = " << m_szFullName;
}
#endif


////////////////////////////////////////////////////////////////////////////
// CFile implementation
IMPLEMENT_DYNAMIC(CFile, CObject)


CFile::CFile()
{
	m_hFile = hFileNull;
	m_bCloseOnDelete = FALSE;
}

CFile::CFile(int hFile)
{
	m_hFile = hFile;
	m_bCloseOnDelete = FALSE;
}

CFile::CFile(const char* pszFileName, UINT nOpenFlags)
{
	ASSERT(AfxIsValidAddress(pszFileName, strlen(pszFileName), FALSE));

	CFileException e;
	if (!this->Open(pszFileName, nOpenFlags, &e))
		AfxThrowFileException(e.m_cause, e.m_lOsError);
}

CFile::~CFile()
{   
	if (m_hFile != hFileNull && m_bCloseOnDelete)
		Close();
}

CFile*
CFile::Duplicate() const
{
	ASSERT_VALID(this);
	ASSERT(m_hFile != hFileNull);

	CFile* pFile = new CFile(hFileNull);
#ifdef _DOSWIN
	pFile->m_hFile = _dup(this->m_hFile);
#else
	if (!::DuplicateHandle(::GetCurrentProcess(),
		(HANDLE)m_hFile, ::GetCurrentProcess(), 
		(LPHANDLE)&pFile->m_hFile, 0L, TRUE, DUPLICATE_SAME_ACCESS))
	{
		delete pFile;
		CFileException::ThrowOsError(::GetLastError());
	}
#endif
	pFile->m_bCloseOnDelete = this->m_bCloseOnDelete;
	return pFile;
}

BOOL
CFile::Open(const char* pszFileName, UINT nOpenFlags, 
	CFileException* pException /* = NULL */)
{
	ASSERT(AfxIsValidAddress(pszFileName, strlen(pszFileName), FALSE));
	ASSERT(pException == NULL || AfxIsValidAddress(pException, sizeof(CFileException)));
	ASSERT((nOpenFlags & typeText) == 0);

	// CFile objects are always binary and _dos_open does not need flag
	nOpenFlags &= ~(UINT)CFile::typeBinary;

	m_bCloseOnDelete = FALSE;
	m_hFile = hFileNull;


#ifdef _DOSWIN
	ASSERT_VALID(this);

	char szOemPath[_MAX_PATH];
	UINT nErr;

	AnsiToOem(pszFileName, szOemPath);

	if (nOpenFlags & (UINT)CFile::modeCreate)
	{
		if ((nErr = _dos_creat(szOemPath, CFile::normal, (int*)&m_hFile)) != 0)
		{
			if (pException != NULL)
			{
				pException->m_lOsError = nErr;
				pException->m_cause = CFileException::OsErrorToException(nErr);
				return FALSE; // file was not created
			}
		}
		if ((nErr = _dos_close(m_hFile)) != 0)
		{
			// try to delete the file and throw modeCreate exception
			_Afx_remove(szOemPath);
			if (pException != NULL)
			{
				pException->m_lOsError = nErr;
				pException->m_cause = CFileException::OsErrorToException(nErr);
			}
			return FALSE;
		}
	}

	// the file has been modeCreated if needed, now open it
	if ((nErr = _dos_open(szOemPath, nOpenFlags & ~(UINT)CFile::modeCreate, (int*)&m_hFile)) != 0)
	{
		// try to delete the file and throw open exception
		if (pException != NULL)
		{
			pException->m_lOsError = nErr;
			pException->m_cause = CFileException::OsErrorToException(nErr);
		}
		return FALSE;
	}
#endif

#ifdef _NTWIN
	ASSERT_VALID(this);
	ASSERT(sizeof(HFILE) == sizeof(int));
	ASSERT(CFile::shareCompat == 0);

	OFSTRUCT of;

	if (((int)(m_hFile = (int)::OpenFile((LPSTR)pszFileName, &of, nOpenFlags))) < 0)
	{
		if (pException != NULL)
		{
			pException->m_lOsError = of.nErrCode;
			pException->m_cause = CFileException::OsErrorToException((LONG)of.nErrCode);
		}
		return FALSE;
	}
#endif

	m_bCloseOnDelete = TRUE;
	return TRUE;
}


UINT
CFile::Read(void FAR* lpBuf, UINT nCount)
{
	ASSERT_VALID(this);
	ASSERT(m_hFile != hFileNull);
	ASSERT(lpBuf != NULL);
	ASSERT(AfxIsValidAddress(lpBuf, nCount));

	UINT nRead = 0;
	UINT nErr;

	if ((nErr = _dos_read(m_hFile, lpBuf, nCount, &nRead)) != 0)
		CFileException::ThrowOsError(nErr);
		
	return nRead;
}

void
CFile::Write(const void FAR* lpBuf, UINT nCount)
{
	ASSERT_VALID(this);
	ASSERT(m_hFile != hFileNull);
	ASSERT(lpBuf != NULL);
	ASSERT(AfxIsValidAddress(lpBuf, nCount, FALSE));

	UINT nWritten = 0;
	UINT nErr;

	if ((nErr = _dos_write(m_hFile, lpBuf, nCount, &nWritten)) != 0)
		CFileException::ThrowOsError(nErr);
	
	if (nCount != nWritten)
		AfxThrowFileException(CFileException::diskFull);
}


LONG
CFile::Seek(LONG lOff, UINT nFrom)
{
	ASSERT_VALID(this);
	ASSERT(m_hFile != hFileNull);
	ASSERT(nFrom == CFile::begin || nFrom == CFile::end || nFrom == CFile::current);

	DWORD dwNew;
	UINT nErr;

	if ((nErr  = _Afx_seek(m_hFile, lOff, nFrom, &dwNew)) != 0)
		CFileException::ThrowOsError(nErr);
	
	return dwNew;
}


DWORD
CFile::GetPosition() const
{
	ASSERT_VALID(this);
	ASSERT(m_hFile != hFileNull);

	DWORD dwPos;
	UINT nErr;

	if ((nErr = _Afx_seek(m_hFile, 0, CFile::current, &dwPos)) != 0)
		CFileException::ThrowOsError(nErr);
	
	return dwPos;
}


#pragma optimize("qgel", off) // assembler cannot be globally optimized
void
CFile::Flush()
{
	ASSERT_VALID(this);
	ASSERT(m_hFile != hFileNull);
	
	UINT nErr;

#ifdef _DOSWIN
	// SmartDrive 4.0 workaround, carry flag incorrectly propogated
		_asm { CLC }
#endif

	if ((nErr = _dos_commit(m_hFile)) != 0)
		CFileException::ThrowOsError(nErr);
}
#pragma optimize("", on)    // return to default optimizations

void
CFile::Close()
{
	ASSERT_VALID(this);
	ASSERT(m_hFile != hFileNull);

	UINT nErr;

	if (m_hFile != hFileNull && (nErr = _dos_close(m_hFile)) != 0)
		CFileException::ThrowOsError(nErr);

	m_hFile = hFileNull;
	m_bCloseOnDelete = FALSE;
}


void
CFile::LockRange(DWORD dwPos, DWORD dwCount)
{
	ASSERT_VALID(this);
	ASSERT(m_hFile != hFileNull);

	UINT nErr;

	if ((nErr = _Afx_lock(m_hFile, dwPos, dwCount, FALSE)) != 0)
		CFileException::ThrowOsError(nErr);
}


void
CFile::UnlockRange(DWORD dwPos, DWORD dwCount)
{
	ASSERT_VALID(this);
	ASSERT(m_hFile != hFileNull);

	UINT nErr;

	if ((nErr = _Afx_lock(m_hFile, dwPos, dwCount, TRUE)) != 0)
		CFileException::ThrowOsError(nErr);
}


void
CFile::SetLength(DWORD dwNewLen)
{
	ASSERT_VALID(this);
	ASSERT(m_hFile != hFileNull);

#ifdef _DOSWIN
	UINT nErr;
	UINT nWritten = 0;

	this->Seek(dwNewLen, CFile::begin);

	if ((nErr = _dos_write(m_hFile, NULL, 0, &nWritten)) != 0)
		CFileException::ThrowOsError(nErr);
#endif
#ifdef _NTWIN
	this->Seek((LONG)dwNewLen, (UINT)CFile::begin);

	if (!::SetEndOfFile((HANDLE)m_hFile))
		CFileException::ThrowOsError((LONG)::GetLastError());
#endif
}

DWORD
CFile::GetLength() const
{
	ASSERT_VALID(this);

	DWORD dwLen, dwCur;
	
	// Seek is a non const operation 
	dwCur = ((CFile*)this)->Seek(0, CFile::current);
	dwLen = ((CFile*)this)->SeekToEnd();
	VERIFY(dwCur == (DWORD)(((CFile*)this)->Seek(dwCur, CFile::begin)));

	return dwLen;
}

BOOL
CFile::GetStatus(CFileStatus& rStatus) const
{
	ASSERT_VALID(this);
	ASSERT(m_hFile != hFileNull);

	//NOTE: cannot return name of file from handle
	rStatus.m_szFullName[0] = '\0';


#ifdef _DOSWIN
	struct _stat s;

	if (_fstat(m_hFile, &s) == 0)
	{
		rStatus.m_ctime = CTime(s.st_atime);
		rStatus.m_atime = rStatus.m_ctime;
		rStatus.m_mtime = rStatus.m_ctime;
		rStatus.m_size = s.st_size;
		rStatus.m_attribute = 0;    // dos won't give us this from
									// just a fd, need the path name
		return TRUE;
	}   
	return FALSE;
#endif
#ifdef _NTWIN
	_FILETIME ftCreate, ftAccess, ftModify;
	if (!::GetFileTime((HANDLE)m_hFile, &ftCreate, &ftAccess, &ftModify))
		return FALSE;

	if ((rStatus.m_size = ::GetFileSize((HANDLE)m_hFile, NULL)) == (DWORD)-1L)
		return FALSE;

	rStatus.m_attribute = 0;		// nt won't give us this from
									// just a fd, need the path name

	rStatus.m_ctime = CTime(ftCreate);
	rStatus.m_atime = CTime(ftAccess);
	rStatus.m_mtime = CTime(ftModify);

	if (rStatus.m_ctime.GetTime() == 0)
		rStatus.m_ctime = rStatus.m_mtime;

	if (rStatus.m_atime.GetTime() == 0)
		rStatus.m_atime = rStatus.m_mtime;

	return TRUE;
#endif
}

void
CFile::Rename(const char* pszOldName, const char* pszNewName)
{
	char szOld[_MAX_PATH];
	char szNew[_MAX_PATH];
	UINT nErr;

	AnsiToOem(pszOldName, szOld);
	AnsiToOem(pszNewName, szNew);

	if ((nErr = _Afx_rename(szOld, szNew)) != 0)
		CFileException::ThrowOsError(nErr);

}

void
CFile::Remove(const char* pszFileName)
{
	UINT nErr;
	char sz[_MAX_PATH];

	AnsiToOem(pszFileName, sz);

	if ((nErr = _Afx_remove(sz)) != 0)
		CFileException::ThrowOsError(nErr);
}


BOOL
CFile::GetStatus(const char* pszFileName, CFileStatus& rStatus)
{

#ifdef _DOSWIN
	char sz[_MAX_PATH];

	// first fill in the full name of the file, undefined if we return FALSE
	if (_fullpath(sz, pszFileName, _MAX_PATH) == NULL)
	{
		rStatus.m_szFullName[0] = '\0';
		return FALSE;
	}
	strncpy(rStatus.m_szFullName, sz, _MAX_PATH);

	// finish filling in the structure
	WORD wAttr = CFile::normal | CFile::readOnly |
				CFile::hidden | CFile::system |
				CFile::directory | CFile::archive;
	struct _find_t find;

	AnsiToOem(pszFileName, sz);

	if (_dos_findfirst(sz, wAttr, &find) == 0)
	{
		rStatus.m_mtime = CTime((WORD)find.wr_date, (WORD)find.wr_time);
		rStatus.m_ctime = rStatus.m_mtime;
		rStatus.m_atime = rStatus.m_mtime;

		rStatus.m_size = find.size;
		rStatus.m_attribute = find.attrib;
		return TRUE;
	}
	else
		return FALSE;
#endif // _DOSWIN
#ifdef _NTWIN
	WIN32_FIND_DATA findFileData;
	OFSTRUCT of;
	HANDLE hFound;

	if ((hFound = FindFirstFile((LPTSTR)pszFileName, &findFileData)) == (HANDLE)(-1))
		return FALSE;

	VERIFY(FindClose(hFound));

	OpenFile((LPSTR)pszFileName, &of, OF_PARSE);
#ifdef _WINDOWS
	VERIFY(OemToCharBuff((LPSTR)of.szPathName, (LPTSTR)rStatus.m_szFullName,
		_MAX_PATH));
#else
	strncpy(rStatus.m_szFullName, (LPTSTR)of.szPathName,
		sizeof(rStatus.m_szFullName)-1);
#endif
	rStatus.m_szFullName[_MAX_PATH-1] = '\0';
	rStatus.m_attribute = (BYTE)(findFileData.dwFileAttributes & ~0x80);

	ASSERT(findFileData.nFileSizeHigh == 0);
	rStatus.m_size = (LONG) findFileData.nFileSizeLow;
	ASSERT(rStatus.m_size >= 0);

	rStatus.m_ctime = CTime(findFileData.ftCreationTime);
	rStatus.m_atime = CTime(findFileData.ftLastAccessTime);
	rStatus.m_mtime = CTime(findFileData.ftLastWriteTime);

	if (rStatus.m_ctime.GetTime() == 0)
		rStatus.m_ctime = rStatus.m_mtime;

	if (rStatus.m_atime.GetTime() == 0)
		rStatus.m_atime = rStatus.m_mtime;

	return TRUE;
#endif
}

#ifdef _NTWIN
// NOTE: Implementation helper function.
static void NEAR TimeToFileTime(const CTime& time, LPFILETIME pFileTime)
{
	_SYSTEMTIME sysTime;
	sysTime.wYear  = time.GetYear();
	sysTime.wMonth = time.GetMonth();
	sysTime.wDay   = time.GetDay();
	sysTime.wHour = time.GetHour();
	sysTime.wMinute = time.GetMinute();
	sysTime.wSecond = time.GetSecond();
	sysTime.wMilliseconds = 0;

	if (!SystemTimeToFileTime((LPSYSTEMTIME)&sysTime, pFileTime));
		CFileException::ThrowOsError((LONG)::GetLastError());
}
#endif

void
CFile::SetStatus(const char* pszFileName, const CFileStatus& status)
{

#ifdef _DOSWIN
	UINT nErr;
	UINT wAttr;
	char sz[_MAX_PATH];

	AnsiToOem(pszFileName, sz);

	if ((nErr = _dos_getfileattr(sz, &wAttr)) != 0)
		CFileException::ThrowOsError(nErr);

	if (status.m_attribute != wAttr && wAttr & CFile::readOnly)
	{
		// Set file attribute, only if currently readonly.
		// This way we will be able to modify the time assuming the
		// caller changed the file from readonly.
		if ((nErr = _dos_setfileattr(sz, status.m_attribute)) != 0)
			CFileException::ThrowOsError(nErr);
	}

	if (status.m_mtime.GetTime() != 0)
	{
		WORD wDate, wTime;
		int handle;

		// set the file date/time
		if ((nErr = _dos_open(sz, CFile::modeReadWrite, &handle)) != 0)
			CFileException::ThrowOsError(nErr);

		wDate = (WORD)(((UINT)status.m_mtime.GetYear() - 1980) << 9);
		wDate += (WORD)(((UINT)status.m_mtime.GetMonth()) << 5);
		wDate += (WORD)((UINT)status.m_mtime.GetDay());

		wTime = (WORD)((UINT)(status.m_mtime.GetHour()) << 11);
		wTime += (WORD)((UINT)status.m_mtime.GetMinute() << 5);
		wTime += (WORD)((UINT)status.m_mtime.GetSecond() >> 1);

		if ((nErr = _dos_setftime(handle, wDate, wTime)) != 0)
			CFileException::ThrowOsError(nErr);

		if ((nErr = _dos_close(handle)) != 0)
			CFileException::ThrowOsError(nErr);
	}

	if (status.m_attribute != wAttr && !(wAttr & CFile::readOnly))
	{
		// Set file attribute, only if currently not readonly.
		if ((nErr = _dos_setfileattr(sz, status.m_attribute)) != 0)
			CFileException::ThrowOsError(nErr);
	}

#endif // _DOSWIN
#ifdef _NTWIN
	DWORD wAttr;
	_FILETIME creationTime;
	_FILETIME lastAccessTime;
	_FILETIME lastWriteTime;
	LPFILETIME lpCreationTime = NULL;
	LPFILETIME lpLastAccessTime = NULL;
	LPFILETIME lpLastWriteTime = NULL;

	if ((wAttr = GetFileAttributes((LPTSTR)pszFileName)) == (DWORD)-1L)
		CFileException::ThrowOsError((LONG)GetLastError());

	if ((DWORD)status.m_attribute != wAttr && (wAttr & CFile::readOnly))
	{
		// Set file attribute, only if currently readonly.
		// This way we will be able to modify the time assuming the
		// caller changed the file from readonly.

		if (!SetFileAttributes((LPTSTR)pszFileName, (DWORD)status.m_attribute))
			CFileException::ThrowOsError((LONG)GetLastError());
	}

	// last modification time
	if (status.m_mtime.GetTime() != 0)
	{
		TimeToFileTime(status.m_mtime, &lastWriteTime);
		lpLastWriteTime = &lastWriteTime;

		// last access time
		if (status.m_atime.GetTime() != 0)
		{
			TimeToFileTime(status.m_atime, &lastAccessTime);
			lpLastAccessTime = &lastAccessTime;
		}
	
		// create time
		if (status.m_ctime.GetTime() != 0)
		{
			TimeToFileTime(status.m_ctime, &creationTime);
			lpCreationTime = &creationTime;
		}
	
		OFSTRUCT of;
		HFILE hFile = OpenFile((LPSTR)pszFileName, &of, OF_READWRITE);

		if (hFile == (HFILE)-1L)
			CFileException::ThrowOsError((LONG)::GetLastError());
	
		if (!SetFileTime((HANDLE)hFile, lpCreationTime, lpLastAccessTime, lpLastWriteTime))
			CFileException::ThrowOsError((LONG)::GetLastError());
	
		if (_lclose(hFile) == -1)
			CFileException::ThrowOsError((LONG)::GetLastError());
	
	} // m_mtime != 0

	if ((DWORD)status.m_attribute != wAttr && !(wAttr & CFile::readOnly))
	{
		if (!SetFileAttributes((LPTSTR)pszFileName, (DWORD)status.m_attribute))
			CFileException::ThrowOsError((LONG)GetLastError());
	}
#endif // _NTWIN
}


#ifdef _DEBUG
void
CFile::AssertValid() const
{
	CObject::AssertValid();
	// we permit the descriptor m_hFile to be any value for derived classes
}

void
CFile::Dump(CDumpContext& dc) const
{
	ASSERT_VALID(this);

	CObject::Dump(dc);
	dc << "a " << GetRuntimeClass()->m_pszClassName << " with handle " << m_hFile;
}
#endif
