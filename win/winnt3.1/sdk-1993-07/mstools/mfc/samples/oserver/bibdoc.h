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
// CBibDoc:

class CBibItem;

class CBibDoc : public COleServerDoc
{
protected:
	// visual data is stored in listbox
	CListBox*   m_pList;
	const char* m_pszFileName;
	const char* m_pszSectionName;
	CBibItem*   m_pSelectedItem;


// Construction
public:
	CBibDoc(CListBox* pList, const char* pszFileName,
			const char* pszSectionName);

// Attributes
	// cached host names
	CString     m_strHost, m_strHostObj;

// Operations
	BOOL    Load();
	void    AddItem(const CString& key, const CString& value);
	void    DeleteItem(const CString& key, int nIndex);
	void    GetItemKeyValue(int nIndex, CString& key, CString& value);
	BOOL    GetItemValue(const CString& key, CString& value);

	BOOL    UpdateClient(const CString& string);

	// UI specific
	BOOL    ShowItem(const CString& key);   // make visible

// Overridables for OLE server document
protected:
	virtual COleServerItem* OnGetDocument();
	virtual COleServerItem* OnGetItem(LPCSTR lpszObjname);
	virtual COleServerItem* GetNextItem(POSITION& rPosition);
	virtual OLESTATUS OnSetHostNames(LPCSTR lpszHost, LPCSTR lpszHostObj);
	virtual OLESTATUS OnRelease();

#ifdef _DEBUG
public:
	virtual void Dump(CDumpContext& dc) const;
	virtual void AssertValid() const;
#endif
};

/////////////////////////////////////////////////////////////////////////////
// Defines for max doc data size

#define MAX_INI_FILE    20000
	// limit since we are using the Windows Profile API to store data
	// (not a typical or efficient way to manage a database, but it works)

/////////////////////////////////////////////////////////////////////////////

