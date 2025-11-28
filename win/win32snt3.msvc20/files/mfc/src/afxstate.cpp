// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp and/or WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include <stddef.h>

#ifdef AFX_INIT_SEG
#pragma code_seg(AFX_INIT_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AFX_APP_STATE implementation (only for DLL versions)

#ifdef _WINDLL

#ifndef _USRDLL
AFX_MODULE_STATE::AFX_MODULE_STATE()
{
	// Note: it is only necessary to intialize non-zero data.

#ifdef _AFXCTL
	m_pID = AfxGetBaseModuleContext();
#endif
}

AFX_MODULE_STATE::~AFX_MODULE_STATE()
{
}
#endif

BOOL _afxSharedData = (BOOL)-1; // set to TRUE if running Win32s
DWORD _afxAppTlsIndex = NULL_TLS;
static BYTE _afxAppStateSafety[sizeof(AFX_APP_STATE)];
static BOOL _afxAppSafetyInUse;

AFX_APP_STATE::AFX_APP_STATE()
{
	// Note: it is only necessary to intialize non-zero data.

#ifdef _AFXCTL
	m_mapExtraData.InitHashTable(67, FALSE);
#endif
}

void* AFX_CDECL AFX_APP_STATE::operator new(size_t nSize)
{
	ASSERT(nSize == sizeof(_afxAppStateSafety));
	ASSERT(_afxAppTlsIndex == NULL_TLS ||
		TlsGetValue(_afxAppTlsIndex) == NULL);

	void* pTemp;
	if (!_afxAppSafetyInUse)
	{
		// fail-safe case, use static buffer of memory
		_afxAppSafetyInUse = TRUE;
		pTemp = _afxAppStateSafety;
	}
	else
	{
		// LocalAlloc is used because the CRT is not initialized yet.
		pTemp = LocalAlloc(NONZEROLPTR, nSize);
	}
	if (pTemp != NULL)
		memset(pTemp, 0, nSize);

	if (_afxAppTlsIndex != NULL_TLS)
	{
		TlsSetValue(_afxAppTlsIndex, pTemp);
		ASSERT(TlsGetValue(_afxAppTlsIndex) == pTemp);
	}
	return pTemp;
}

void AFX_CDECL AFX_APP_STATE::operator delete(void* p)
{
	if (p == NULL)
		return;

	if (p == _afxAppStateSafety)
	{
		_afxAppSafetyInUse = FALSE;
		return;
	}
	LocalFree(p);
}

AFX_APP_STATE::~AFX_APP_STATE()
{
	// reset thread local storage (for Win32s only)
	ASSERT(!_afxSharedData || _afxAppTlsIndex != NULL_TLS);
	if (_afxAppTlsIndex != NULL_TLS)
	{
		ASSERT(TlsGetValue(_afxAppTlsIndex) == this);
		TlsSetValue(_afxAppTlsIndex, NULL);
	}
}

#endif //_WINDLL

/////////////////////////////////////////////////////////////////////////////
// AfxGetAppState implementation

#ifdef _WINDLL
// DLL versions need process local thread state under Win32s

AFX_APP_STATE* AFXAPI AfxGetAppState()
{
	ASSERT(_afxSharedData != (BOOL)-1);

	AFX_APP_STATE* pAppState;
	if (_afxSharedData)
	{
		// For Win32s, it is necessary to use thread local storage
		ASSERT(_afxAppTlsIndex != NULL_TLS);
		pAppState = (AFX_APP_STATE*)TlsGetValue(_afxAppTlsIndex);
	}
	else
	{
		// For Win32, simply return a globally allocated AFX_APP_STATE
		ASSERT(_afxAppSafetyInUse);
		pAppState = (AFX_APP_STATE*)_afxAppStateSafety;
	}
	ASSERT(pAppState != NULL);
	return pAppState;
}

/////////////////////////////////////////////////////////////////////////////
// OCX specific APIs

#ifdef _AFXCTL
static BYTE _afxExModuleData[sizeof(AFX_MODULE_STATE)];

AFX_MODULE_STATE* AFXAPI AfxGetBaseModuleContext()
{
	return (AFX_MODULE_STATE*)&_afxExModuleData;
}

AFX_MODULE_STATE* AFXAPI AfxGetCurrentModuleContext()
{
	return AfxGetAppState()->m_pID;
}

AFX_MODULE_STATE* AFXAPI AfxPushModuleContext(AFX_MODULE_STATE* psIn)
{
	ASSERT(psIn != NULL);

	AFX_MODULE_STATE* psCurrentSource = AfxGetCurrentModuleContext();
	if (psCurrentSource == psIn->m_pID)
		return NULL;

	// Push and pop are mostly the same.
	AfxPopModuleContext(psIn, FALSE);
	return psCurrentSource;
}

void AFXAPI AfxPopModuleContext(AFX_MODULE_STATE* psIn, BOOL bCopy /*=FALSE*/)
{
	AFX_MODULE_STATE* psCurrent = AfxGetAppState();
	AFX_MODULE_STATE* psOut = psCurrent->m_pID;

	ASSERT(psIn != psCurrent->m_pID);   // Same as current
	ASSERT(psIn == NULL || psIn == psIn->m_pID);    // Reference to original.
	ASSERT(psOut != NULL);  // App state is corrupted

	// Save the current state back from where it came.
	if (bCopy)
		memcpy(psOut, psCurrent, sizeof(AFX_MODULE_STATE));
	else
	{
		// Copy only variables which may have changed (several in win state).
		memcpy(&psOut->m_winState, &psCurrent->m_winState,
			sizeof(AFX_WIN_STATE));
	}

	// If the in pointer is NULL, then we just copy out.  Used for
	// initialization of the runtime DLL.

	if (psIn != NULL)
	{
		memcpy(psCurrent, psIn, sizeof(AFX_MODULE_STATE));

		AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
		pThreadState->m_pCurrentWinThread = AfxGetApp();
	}
}
#endif //_AFXCTL

#endif //!_WINDLL

/////////////////////////////////////////////////////////////////////////////
// AFX_THREAD_STATE implementation

DWORD _afxThreadTlsIndex = NULL_TLS;
static BYTE _afxThreadStateSafety[sizeof(AFX_THREAD_STATE)];
static BOOL _afxThreadSafetyInUse;

AFX_THREAD_STATE::AFX_THREAD_STATE() :
	m_mapHWND(RUNTIME_CLASS(CWnd), offsetof(CWnd, m_hWnd)),
	m_mapHMENU(RUNTIME_CLASS(CMenu), offsetof(CMenu, m_hMenu)),
	m_mapHDC(RUNTIME_CLASS(CDC), offsetof(CDC, m_hDC), 2),
	m_mapHGDIOBJ(RUNTIME_CLASS(CGdiObject), offsetof(CGdiObject, m_hObject))
{
	// Note: all other member data is initialized to zero automatically
	//  since operator new uses memset(0) on the memory.

	ASSERT(_afxThreadTlsIndex != NULL_TLS);
	if (TlsGetValue(_afxThreadTlsIndex) == NULL)
	{
		// setup thread local storage
		TlsSetValue(_afxThreadTlsIndex, this);
	}
}

void* AFX_CDECL AFX_THREAD_STATE::operator new(size_t nSize)
{
	ASSERT(nSize == sizeof(_afxThreadStateSafety));

	void* pTemp;
	if (!_afxThreadSafetyInUse)
	{
		// fail-safe case, use static buffer of memory
		_afxThreadSafetyInUse = TRUE;
		pTemp = _afxThreadStateSafety;
	}
	else
	{
		// LocalAlloc is used because the CRT is not initialized yet.
		pTemp = LocalAlloc(NONZEROLPTR, nSize);
	}
	if (pTemp != NULL)
		memset(pTemp, 0, nSize);

	return pTemp;
}

#ifdef _DEBUG
void* AFX_CDECL AFX_THREAD_STATE::operator new(
	size_t nSize, LPCSTR lpszFileName, int nLine)
{
	UNUSED lpszFileName;
	UNUSED nLine;

	return AFX_THREAD_STATE::operator new(nSize);
}
#endif

void AFX_CDECL AFX_THREAD_STATE::operator delete(void* p)
{
	if (p == NULL)
		return;

	if (p == _afxThreadStateSafety)
	{
		_afxThreadSafetyInUse = FALSE;
		return;
	}
	LocalFree(p);
}

AFX_THREAD_STATE::~AFX_THREAD_STATE()
{
	if (m_pSafetyPoolBuffer != NULL)
	{
		free(m_pSafetyPoolBuffer);
		m_pSafetyPoolBuffer = NULL;
	}

	ASSERT(_afxThreadTlsIndex != NULL_TLS);
	if (TlsGetValue(_afxThreadTlsIndex) == this)
	{
		// reset thread local storage
		TlsSetValue(_afxThreadTlsIndex, NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////
// AfxGetThreadState implementation

#ifndef _MAC
DWORD WINAPI _AfxTlsAlloc();
#else
#define _AfxTlsAlloc TlsAlloc
#endif

AFX_THREAD_STATE* AFXAPI AfxGetThreadState()
{
	// The thread state is stored in thread local storage.  This works
	//  both for multiple threads under Win32 as well as multiple
	//  processes under Win32s.

	// allocate initial thread local storage index
	if (_afxThreadTlsIndex == NULL_TLS)
	{
		_afxThreadTlsIndex = _AfxTlsAlloc();
		if (_afxThreadTlsIndex == NULL_TLS)
			return NULL;    // failure
	}

	ASSERT(_afxThreadTlsIndex != NULL_TLS);
	AFX_THREAD_STATE* pThreadState =
		(AFX_THREAD_STATE*)TlsGetValue(_afxThreadTlsIndex);
	if (pThreadState == NULL)
	{
		BOOL bTracking = AfxEnableMemoryTracking(FALSE);
		pThreadState = new AFX_THREAD_STATE;
		ASSERT(TlsGetValue(_afxThreadTlsIndex) == pThreadState);
		AfxEnableMemoryTracking(bTracking);
	}
	return pThreadState;
}

/////////////////////////////////////////////////////////////////////////////
