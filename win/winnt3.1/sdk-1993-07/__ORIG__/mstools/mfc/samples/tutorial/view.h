// view.h : Declares the interfaces to the application and frame window.
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __VIEW_H__
#define __VIEW_H__

/////////////////////////////////////////////////////////////////////////////
//  CtheApp
//  Derived from CWinApp in order to allow us to override
//  the InitInstance member function to create our own window.
//
class CTheApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
};

/////////////////////////////////////////////////////////////////////////////
//  CFindDialog
//  This dialog is a one line entry field for getting a search
//  string to use as a find filter for the database
//
class CFindDialog : public CModalDialog
{
private:
	CString m_szFindName;
	
public:
	CFindDialog( CWnd* pParentWnd = NULL )
		: CModalDialog( "Find", pParentWnd )
		{ }

	virtual void OnOK();
	
	CString& GetFindString() { return m_szFindName; }
};


/////////////////////////////////////////////////////////////////////////////
//  CEditDialog
//  Used to add or edit a Person object.
//
class CEditDialog : public CModalDialog
{
private:
	CPerson* m_pData;

public:
	CEditDialog( CPerson* person, CWnd* pParentWnd = NULL )
		: CModalDialog( "EditPerson", pParentWnd )
		{ m_pData = person; }
	
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	
	CPerson* GetData()
		{ return m_pData; }
};

/////////////////////////////////////////////////////////////////////////////
//  CMainWindow
//  The window object that WinApp creates.  In this program we
//  only use one window class.  In that sense this object does
//  all the work that makes our window a CPersonList viewer.
//
class CMainWindow : public CFrameWnd
{
private:
	// Variables that contain the window size, font size and scroll
	// position.
	int m_cxChar;
	int m_cyChar;
	int m_nHscrollPos;
	int m_nVscrollPos;
	int m_cxCaps;
	int m_nMaxWidth;
	int m_cxClient;
	int m_cyClient;
	int m_nVscrollMax;
	int m_nHscrollMax;
	int m_nSelectLine;
	CDataBase m_people;

	// Private helpers for the other routines.
	void SetMenu();
	BOOL Save( BOOL bNamed=FALSE );
	BOOL FileDlg( BOOL bOpen, int nMaxFile, LPSTR szFile,
			int nMaxFileTitle, LPSTR szFileTitle );
	BOOL CheckForSave( const char* pszTitle, const char* pszMessage );
	void InvalidateLine();

public:
	// The CMainWindow constructor.
	CMainWindow();

	// These routines are all overrides of CWnd.  Windows messages
	// cause these to be called.
	afx_msg int  OnCreate( LPCREATESTRUCT cs );
	afx_msg void OnClose();
	afx_msg void OnSize( UINT type, int x, int y );
	afx_msg void OnHScroll( UINT nSBCode, UINT pos, CScrollBar* control );
	afx_msg void OnVScroll( UINT nSBCode, UINT pos, CScrollBar* control );
	afx_msg void OnLButtonDown( UINT wParam, CPoint location );
	afx_msg void OnLButtonDblClk( UINT wParam, CPoint location );
	afx_msg void OnKeyDown( UINT wParam, UINT, UINT );
	afx_msg void OnPaint();

	// These routines are all menu items.  User action causes
	// these to be called.
	afx_msg void OnNew();
	afx_msg void OnOpen();
	afx_msg void OnSave();
	afx_msg void OnSaveAs();
	afx_msg void OnDBClose();
	afx_msg void OnPrint();
	afx_msg void OnExit();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnFind();
	afx_msg void OnFindAll();
	afx_msg void OnEdit();
	afx_msg void OnHelp();
	afx_msg void OnAbout();
	afx_msg void OnUp();
	afx_msg void OnDown();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // __VIEW_H__
