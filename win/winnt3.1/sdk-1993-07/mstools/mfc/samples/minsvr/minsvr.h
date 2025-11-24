// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


// Minsvr.cpp : minimal OLE Server

/////////////////////////////////////////////////////////////////////////////

#include <afxole.h>
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CMinItem : mini server item

class CMinDoc;

class CMinItem : public COleServerItem
{
// Constructors
public:
	CMinItem();

// Attributes
	CMinDoc*    GetDocument() const // type cast helper
					{ return (CMinDoc*) COleServerItem::GetDocument(); }

	CString m_data;     // example of item data

// Operations
	BOOL    PromptChangeString();       // return TRUE if changed

// Overridables
protected:
	virtual OLESTATUS   OnShow(BOOL bTakeFocus);
	virtual BOOL        OnDraw(CMetaFileDC* pDC);

	virtual void        Serialize(CArchive& ar);        // for native data
	virtual BOOL        OnGetTextData(CString& rStringReturn);
};

/////////////////////////////////////////////////////////////////////////////
// CMinDoc : mini server document

class CMinDoc : public COleServerDoc
{
// Attributes
public:
	CMinItem m_item; // document contains one item

// Overridables for OLE server
protected:
	virtual COleServerItem* OnGetDocument();
	virtual COleServerItem* OnGetItem(LPCSTR lpszObjname);
	virtual COleServerItem* GetNextItem(POSITION& rPosition);
};

/////////////////////////////////////////////////////////////////////////////
// CMinServer : mini server

class CMainWnd;

#define SERVER_NAME         "MINSVR"
#define SERVER_LOCAL_NAME   "Mini OLE Server"

class CMinServer : public COleServer
{
public:
	CMinServer(CMainWnd* pMainWnd);

// Attributes
public:
	CMinDoc m_doc;      // single document server (multi-instance app)

protected:
// Overridables for OLE Server requests
	virtual COleServerDoc* OnOpenDoc(LPCSTR lpszDoc);
	virtual COleServerDoc* OnCreateDoc(LPCSTR lpszClass, LPCSTR lpszDoc);
	virtual COleServerDoc* OnEditDoc(LPCSTR lpszClass, LPCSTR lpszDoc);

// Implementation
protected:
	CMainWnd* m_pMainWnd;
};

/////////////////////////////////////////////////////////////////////////////
// CMainWnd : the main window

class CMainWnd : public CFrameWnd
{
// Construction
public:
	CMainWnd();

// Attributes
public:
	CMinServer m_server; // (contains server (contains doc (contains item)))

// Implementation
protected:
	afx_msg void OnClose();

	// menu commands
	afx_msg void OnUpdateClient();
	afx_msg void OnChangeString();
	afx_msg void OnAbout();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CMinApp : the application

class CMinApp : public CWinApp
{
public:
	// single server application
	CMinApp() : CWinApp(SERVER_NAME)
		{ }

	// implementation
	BOOL InitInstance();
	int ExitInstance();
};

/////////////////////////////////////////////////////////////////////////////
