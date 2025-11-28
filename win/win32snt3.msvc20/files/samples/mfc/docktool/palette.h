// palette.h : interface of the CPaletteBar class
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFXEXT_H__
#include <afxext.h>         // for access to CToolBar
#endif

class CPaletteBar : public CToolBar
{
// Constructor
public:
	CPaletteBar();
	void SetColumns(UINT nColumns);
	UINT GetColumns() { return m_nColumns; };

// Attributes
public:

// Operations
public:

// Implementation
public:
	virtual ~CPaletteBar();
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	// overridden from CToolBar implementation
	virtual void GetItemRect(int nIndex, LPRECT lpRect) const;
	virtual int HitTest(CPoint point);
	virtual void DoPaint(CDC* pDC);

protected:
	UINT    m_nColumns;

// Generated message map functions
protected:
	//{{AFX_MSG(CPaletteBar)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
