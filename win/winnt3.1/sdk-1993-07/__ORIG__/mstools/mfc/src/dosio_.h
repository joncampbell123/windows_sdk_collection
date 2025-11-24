// This is a part of the Microsoft Foundation Classes C++ library. 
// Copyright (C) 1992 Microsoft Corporation 
// All rights reserved. 
//  
// This source code is only intended as a supplement to the 
// Microsoft Foundation Classes Reference and Microsoft 
// QuickHelp documentation provided with the library. 
// See these sources for detailed information regarding the 
// Microsoft Foundation Classes product. 

/////////////////////////////////////////////////////////////////////////////
// INTERNAL INTERNAL INTERNAL
// Do not explicitly include


#define CFile_shareMask ((UINT)(CFile::shareExclusive | CFile::shareDenyWrite | CFile::shareDenyRead | CFile::shareDenyNone | CFile::shareCompat))

#ifndef _WINDOWS
typedef  char FAR* LPSTR;
typedef  const char FAR* LPCSTR;
inline int AnsiToOem(LPCSTR src, LPSTR dst)
			{ _fstrcpy(dst, src); return -1; }
inline int OemToAnsi(LPCSTR src, LPSTR dst)
			{ _fstrcpy(dst, src); return -1; }
#endif

/////////////////////////////////////////////////////////////////////////////
#ifdef _NTWIN
// Microsoft C Runtime library calls for NT using DOS names

typedef int HFILE;

// Mimic C runtime _dos calls
inline UINT _dos_close(UINT hFile)
	{ return ::_lclose((HFILE)hFile) < 0 ? GetLastError() : 0; }

inline UINT _dos_read(UINT hFile, void FAR* lpv, UINT nBytes, UINT FAR* lpw)
{
	if (::ReadFile((HANDLE)hFile, (LPVOID)lpv, nBytes, (LPDWORD)lpw, NULL))
		return 0;
	else
		return (UINT)::GetLastError();
}

inline UINT _dos_write(UINT hFile, const void FAR* lpv, UINT nBytes, UINT FAR* lpw)
{
	if (::WriteFile((HANDLE)hFile, (LPVOID)lpv, nBytes, (LPDWORD)lpw, NULL))
		return 0;
	else
		return (UINT)::GetLastError();
}

inline UINT _dos_commit(UINT hFile)
	{ return ::FlushFileBuffers((HANDLE)hFile) == 0 ? ::GetLastError() : 0; }

// additional Dos calls not provided by C runtime
inline UINT _Afx_seek(UINT hFile, LONG lOff, UINT nMode, DWORD FAR* lpdwNew)
{
	if ((*lpdwNew = (DWORD)::SetFilePointer((HANDLE)hFile, lOff, 0, nMode)) == (DWORD)-1)
		return (UINT)::GetLastError();
	else
		return (UINT)0;
}

inline UINT _Afx_rename(const char FAR* lpszOld, const char FAR* lpszNew)
	{ return ::MoveFile((LPSTR)lpszOld, (LPSTR)lpszNew) ? 0U : (UINT)GetLastError(); }

inline UINT _Afx_remove(const char FAR* lpsz)
	{ return ::DeleteFile((LPSTR)lpsz) ? 0U : (UINT)GetLastError(); }

inline UINT _Afx_lock(UINT hFile, DWORD dwStart, DWORD dwLen, BOOL bUnlock)
{
	if (bUnlock)
		return ::UnlockFile((HANDLE)hFile, dwStart, 0L, dwLen, 0L) ? 0U : (UINT)GetLastError();
	else
		return ::LockFile((HANDLE)hFile, dwStart, 0L, dwLen, 0L) ? 0U : (UINT)GetLastError();
}
#endif // _NTWIN

#ifdef _DOSWIN
#include <dos.h>

#ifdef _WINDOWS
extern "C" void FAR PASCAL DOS3Call();
#define DOSCALL call DOS3Call
#else // _DOS
#define DOSCALL int 21h
#endif // _WINDOWS

#endif //_DOSWIN

/////////////////////////////////////////////////////////////////////////////
