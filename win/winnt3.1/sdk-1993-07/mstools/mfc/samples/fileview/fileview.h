// fileview.h : Declares the class interfaces for the application.
//          Fileview is a program which can display the contents of
//          a text-only file regardless of its size.
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

#ifndef __FILEVIEW_H__
#define __FILEVIEW_H__

#include <afxwin.h>
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// Derived class to handle large line files
class CLineFile : public CStdioFile
{
	DECLARE_DYNAMIC(CLineFile)
public:
// Constructors
					CLineFile();
					CLineFile(const char* pszFileName, UINT nOpenFlags);

// Overridables

	virtual LONG    NextLine(char FAR* lpsz, UINT nMax);
	virtual LONG    BackLines(char FAR* lpsz, UINT nMax, UINT nLines);
	virtual LONG    LineNear(char FAR* lpsz, UINT nMax, LONG  lOffset);
	virtual LONG    SetBegin(LONG lnewBegin);
			LONG    GetBegin() { return m_lBeginLine; };


// Implementation
	virtual         ~CLineFile();

private:
	LONG    m_lBeginLine;   // 0 for beginning of file
							// -1 for don't know
							// offset of beginning of a line
};


// CMainWindow:
// See fileview.cpp for the code to the member functions and the
// message map.
//

class CMainWindow : public CFrameWnd
{
public:
	CMainWindow();
	~CMainWindow();

	afx_msg int  OnCreate(LPCREATESTRUCT lpcs);
	afx_msg void OnPaint();
	afx_msg void OnAbout();
	afx_msg void OnOpen();
	afx_msg void OnExit();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP()

private:
	CLineFile*  m_pMyFile;
	short       m_nVScrollPos;
	short       m_nHScrollPos;


	LONG        m_lTopLine;
	LONG        m_lFileSize;

	UINT        m_nCxChar;
	UINT        m_nCyChar;
	UINT        m_nLinesPainted;

protected:
	BOOL    FileDlg(BOOL bOpen, int nMaxFile, LPSTR szFile,
					int nMaxFileTitle, LPSTR szFileTitle);

};

/////////////////////////////////////////////////////////////////////////////

// CTheApp:
// See fileview.cpp for the code to the InitInstance member function.
//

class CTheApp : public CWinApp
{
public:
	BOOL InitInstance();
};

/////////////////////////////////////////////////////////////////////////////

#define SCROLLMAX 1000

#endif // __FILEVIEW_H__
