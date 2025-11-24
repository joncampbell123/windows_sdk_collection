// chartwnd.h : Declares the class interfaces for the Chart frame window.
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
//

#ifndef __CHARTWND_H__
#define __CHARTWND_H__

/////////////////////////////////////////////////////////////////////////////

class CPrintDlgBox;

/////////////////////////////////////////////////////////////////////////////
// class CChartWnd

class CChartWnd : public CFrameWnd
{
protected:
	short GetHighValue();
	void SetNewColors();
	void RenderChart(CDC*);
	void DrawBarChart(CDC*);
	void DrawLineChart(CDC*);
	BOOL DoPrint();
	void GetFileName(char*);    
	BOOL ChangeFile();
	void PrepareDC(CDC* pDC);

public:
	BOOL m_bUntitled;
	BOOL m_bChartSerializedOK;
	short m_cxClient, m_cyClient;
	float m_fTallest;
	CString m_szFileName;
	HCURSOR m_hCross, m_hArrow;
	CPrintDlgBox* m_pPrintDlg;
	CChartObject* m_pChartObject;
	static CRect rectPage;
	static CRect rectData;

	CChartWnd();
	~CChartWnd();
	BOOL Create(LPCSTR szTitle, LONG style = WS_OVERLAPPEDWINDOW,
				const RECT& rect = rectDefault,
				CWnd* pParent = NULL);

	// See file.cpp for the file-handling (serialization) code.

	BOOL FileDlg(BOOL bOpen, int nMaxFile, LPSTR szFile);
	int  LoadFile(const char*);
	void ReadFile();
	void SaveFile(BOOL bNewFileName);

	void UpdateMenu();

	// message handlers

	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChange();
	afx_msg void OnPrint();
	afx_msg void OnBar();
	afx_msg void OnLine();
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nButtons, CPoint pos);
	afx_msg void CmdFileOpen();
	afx_msg void CmdFileSave();
	afx_msg void CmdFileSaveAs();
	afx_msg void OnNew();
	afx_msg void OnClose();
	afx_msg void OnAbout();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // __CHARTWND_H__

