// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

/////////////////////////////////////////////////////////////////////////////

#ifndef __AFXOLE_H__
#include <afxole.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// CBibItem : Bibliographic reference

class CBibDoc;

class CBibItem : public COleServerItem
{
protected:
	CString     m_key;

public:
	CBibItem(const char* pszKey);

	CBibDoc*    GetDocument() const // type cast helper
					{ return (CBibDoc*) COleServerItem::GetDocument(); }

// Operations
	void    ChangeKey(const CString& newKey)
					{ m_key = newKey; }

// Overridables
protected:
	virtual OLESTATUS   OnShow(BOOL bTakeFocus);
	virtual BOOL        OnDraw(CMetaFileDC* pDC);

	virtual void        Serialize(CArchive& ar);        // for native data
	virtual BOOL        OnGetTextData(CString& rStringReturn);

	// cleanup (call COleServerItem::Release then delete your object)
	virtual OLESTATUS   OnRelease();

#ifdef _DEBUG
public:
	virtual void Dump(CDumpContext& dc) const;
	virtual void AssertValid() const;
#endif
};

/////////////////////////////////////////////////////////////////////////////

