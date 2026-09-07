#ifndef __MODCOMMON_HPP__
#define __MODCOMMON_HPP__

/////////////////////////////////////////////////////////////////////////////
// Includes
//
/////////////////////////////////////////////////////////////////////////////
#include "MODMacros.hpp"
#include "CSuperLog.hpp"
#include "MODClasses.hpp"
#include "MODuleCore.h"
#include <crtdbg.h>

/////////////////////////////////////////////////////////////////////////////
// Defines
//
/////////////////////////////////////////////////////////////////////////////
//These inlines are to output the filename and line number before every allocation...
inline	void* CoTaskMemAlloc_Trace(ULONG cbSize, CHAR* pszFileName, ULONG ulLine)
{		
#ifdef _DEBUG
	_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "CoTaskMemAlloc(%d) - File: %s, Line: %d\n", cbSize, pszFileName, ulLine);
#endif	//_DEBUG
	return CoTaskMemAlloc(cbSize);
}

inline	void* CoTaskMemRealloc_Trace(void* pv, ULONG cbSize, CHAR* pszFileName, ULONG ulLine)
{
#ifdef _DEBUG
	_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "CoTaskMemRealloc(0x%08x, %d) - File: %s, Line: %d\n", pv, cbSize, pszFileName, ulLine);
#endif	//_DEBUG
	return CoTaskMemRealloc(pv, cbSize);
}

inline	void CoTaskMemFree_Trace(void* pv, CHAR* pszFileName, ULONG ulLine)
{
#ifdef _DEBUG
	_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "CoTaskMemFree(0x%08x) - File: %s, Line: %d\n", pv, pszFileName, ulLine);
#endif	//_DEBUG
	CoTaskMemFree(pv);
}

//Macros
#define LTMALLOCSPY(cbSize)				CoTaskMemAlloc_Trace(cbSize, __FILE__, __LINE__)
#define LTMREALLOCSPY(pv, cbSize)		CoTaskMemRealloc_Trace(pv, cbSize, __FILE__, __LINE__)
#define LTMFREESPY(pv)					CoTaskMemFree_Trace(pv, __FILE__, __LINE__)


#ifndef DLL_IMPORT
#define DLL_IMPORT	__declspec(dllimport)
#endif


#endif //__MODCOMMON_HPP__
