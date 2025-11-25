// This code and information is provided "as is" without warranty of
// any kind, either expressed or implied, including but not limited to
// the implied warranties of merchantability and/or fitness for a
// particular purpose.

// Copyright (C) 1996 - 1997 Intel corporation.  All rights reserved.

// IPlay.h : main header file for the IPLAY application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CIPlayApp:
// See IPlay.cpp for the implementation of this class
//

class CIPlayDoc;  // defined in IPlayDoc.h

class CIPlayApp : public CWinApp
{
public:
	CIPlayApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIPlayApp)
	public:
	virtual BOOL InitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Notify when document created / destroyed
public:
    void OnDocumentCreated( CIPlayDoc *pIPlayDoc );
    void OnDocumentDestroyed( CIPlayDoc *pIPlayDoc );

// Point to our single document
protected:
    CIPlayDoc *m_pIPlayDoc;

// Implementation

	//{{AFX_MSG(CIPlayApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
