// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


// MinSvrMI.cpp : minimal OLE Server - Multiple Inheritance Example

/////////////////////////////////////////////////////////////////////////////

#include <afxole.h>
#include "resource.h"

#define SERVER_NAME         "MINSVRMI"
#define SERVER_LOCAL_NAME   "Mini OLE Server - MI Implementation"

/////////////////////////////////////////////////////////////////////////////

// MiApp is-a Window - for the main window
// MiApp is-an application - since we need one
// MiApp is-an OLE Server - since we need one to do OLE
// MiApp is-an OLE document - since there is one document per main window
// MiApp is-an OLE item - since we need one for the OLE document

class CMiApp : public CFrameWnd,
		public CWinApp,
		public COleServer,
		public COleServerDoc,
		public COleServerItem
{
// Construction
public:
	CMiApp();

// Attributes
public:
	CString m_data;     // example of server item data

// Implementation
protected:
	// Window messages and commands
	afx_msg LONG OnCloseWindow(UINT, LONG);
	afx_msg void OnUpdateClient();
	afx_msg void OnChangeString();
	afx_msg void OnFileExit();
	afx_msg void OnAbout();
	DECLARE_MESSAGE_MAP()
protected:
	virtual void PostNcDestroy();       // for cleanup

	// App Overrides
	BOOL InitInstance();

	// OLE Server overrides
	virtual COleServerDoc* OnOpenDoc(LPCSTR lpszDoc);
	virtual COleServerDoc* OnCreateDoc(LPCSTR lpszClass, LPCSTR lpszDoc);
	virtual COleServerDoc* OnEditDoc(LPCSTR lpszClass, LPCSTR lpszDoc);

	// Document overrides
	virtual COleServerItem* OnGetDocument();
	virtual COleServerItem* OnGetItem(LPCSTR lpszItemName);
	virtual COleServerItem* GetNextItem(POSITION& rPosition);

	// Item overrides
	virtual OLESTATUS   OnShow(BOOL bTakeFocus);
	virtual BOOL        OnDraw(CMetaFileDC* pDC);
	virtual void        Serialize(CArchive& ar);        // for native data
	virtual BOOL        OnGetTextData(CString& rStringReturn);

public: // necessary for having two or more CObject derived base classes
	void* operator new(size_t nSize)
		{ return CFrameWnd::operator new(nSize); }
	void operator delete(void* p)
		{ CFrameWnd::operator delete(p); }
};

/////////////////////////////////////////////////////////////////////////////

