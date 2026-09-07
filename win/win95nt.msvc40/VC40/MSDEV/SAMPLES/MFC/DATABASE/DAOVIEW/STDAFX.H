// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#include <afxcview.h>		// MFC Common controls
#include "ctrlext.h"		// Tree/List control extension classes
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxpriv.h>		// For CSharedFile
#include <afxole.h>			// For Drag-Drop etc
#include <afxdisp.h>        // MFC OLE automation classes
#include <afxdao.h>			// MFC DAO Database support

// variant handling
#ifndef _UNICODE
	#define V_BSTRT(b)	(LPSTR)V_BSTR(b)
#else
	#define V_BSTRT(b)	V_BSTR(b)
#endif
