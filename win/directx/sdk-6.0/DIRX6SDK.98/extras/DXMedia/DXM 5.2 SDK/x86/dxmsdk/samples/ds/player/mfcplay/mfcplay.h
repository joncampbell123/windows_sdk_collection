//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;
//
// player.h : main header file for the PLAYER application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

// Message to wake up our application
#define WM_WAKEUP WM_USER

/////////////////////////////////////////////////////////////////////////////
// CPlayerApp:
// See player.cpp for the implementation of this class
//

class CPlayerDoc;

class CPlayerApp : public CWinApp
{
public:
	CPlayerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlayerApp)
	public:
	virtual BOOL InitInstance();
        virtual int ExitInstance();
    virtual int Run();
	//}}AFX_VIRTUAL

// Notify when document created / destroyed
public:
    void OnDocumentCreated( CPlayerDoc *pPlayerDoc );
    void OnDocumentDestroyed( CPlayerDoc *pPlayerDoc );

// Point to our single document
protected:
    CPlayerDoc *m_pPlayerDoc;

public:
// Implementation

	//{{AFX_MSG(CPlayerApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
