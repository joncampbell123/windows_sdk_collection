// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


// bibref.cpp : OLE Server for Bibliographic reference

#define SERVER_NAME         "BibRef"
#define SERVER_LOCAL_NAME   "Bibliographical Reference"

/////////////////////////////////////////////////////////////////////////////

#include <afxwin.h>
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////

class CBibApp : public CWinApp
{
public:
	CBibApp() : CWinApp(SERVER_NAME)
		{ }

// Attributes
	CString strIniFile;

// Operations
	void ShutDown();

// Implementation
	virtual BOOL InitInstance();
	virtual int ExitInstance();
};

/////////////////////////////////////////////////////////////////////////////
// Globals

extern CBibApp app;

/////////////////////////////////////////////////////////////////////////////
