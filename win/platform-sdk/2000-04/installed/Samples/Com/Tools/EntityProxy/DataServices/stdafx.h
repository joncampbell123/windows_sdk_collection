// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__B7418AD1_EBDA_4D87_A361_0CB1506C47A0__INCLUDED_)
#define AFX_STDAFX_H__B7418AD1_EBDA_4D87_A361_0CB1506C47A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <mtx.h>
#define THROW_ERR(exp,str) {if (FAILED (hr = (exp))) {lErrFlag = 0; pErrMsg = _T(str); throw hr;}}
#define THROW_STR(str) { hr = APP_ERROR; lErrFlag = 1; pErrMsg = str; throw hr;}
#define RETHROW_ERR(exp) { if (FAILED(hr = (exp))) {lErrFlag = 2; throw hr;}}
#define APP_ERROR 0x80004100
#define SafeRelease(x) { if (x){x->Release();x=NULL;}}

#pragma warning( disable : 4146 )
#pragma warning( disable : 4192 )
#import "d:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "adoEOF")
#pragma warning( default : 4192 )
#pragma warning( default : 4146 )

#include <atlcom.h>
#include <vector>
#include <map>

#define ARRAY_VERSION 4			// version of the array (used for packet checking)
#define COLLECTION_VERSION 4	// version of the collection (used for packet checking)

#include "limits.h"
#include "string_fast.h"

#define WCSCPY wcscpy_fast
//#define WCSCPY wcscpy
#define WCSNCPY wcsncpy_fast
//#define WCSNCMP wcsncmp
#define WCSNCMP wcsncmp_fast

#define WCSCMP wcscmp_fast

#define SETCOMPLETE if (m_spObjectContext.p != NULL) m_spObjectContext->SetComplete();
#define SETABORT if (m_spObjectContext.p != NULL) m_spObjectContext->SetAbort();

#include "heap.h"
#include "string.h"
#include "utils.h"

#include "MarshalHelpers.h"
#include "StaticMap.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B7418AD1_EBDA_4D87_A361_0CB1506C47A0__INCLUDED)
