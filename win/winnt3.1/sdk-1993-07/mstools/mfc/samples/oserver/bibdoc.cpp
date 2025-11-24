// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


#include "bibref.h"
#include "bibdoc.h"
#include "bibitem.h"

#include <malloc.h>

/////////////////////////////////////////////////////////////////////////////


CBibDoc::CBibDoc(CListBox* pList,
	const char* pszFileName, const char* pszSectionName)
{
	m_pList = pList;
	m_pszFileName = pszFileName;        // Private INI file
	m_pszSectionName = pszSectionName;

	m_pSelectedItem = NULL;     // no item yet
}

BOOL CBibDoc::Load()
{
	int nDataSize;

	LPSTR lpBuffer = (LPSTR)_fmalloc(MAX_INI_FILE);
	if (lpBuffer == NULL)
		return FALSE;

	nDataSize = GetPrivateProfileString(m_pszSectionName,
		NULL, "", lpBuffer, MAX_INI_FILE, m_pszFileName);

	if (nDataSize == 0) 
	{
		// no initial data or data corrupt
		_ffree(lpBuffer);
		return TRUE;
	}

	if (nDataSize >= MAX_INI_FILE)
	{
		// no initial data or data corrupt
		_ffree(lpBuffer);
		return FALSE;
	}

	LPSTR lpKey = lpBuffer;
	// lpKey points to a list of string for the keys
	while (*lpKey != '\0')
	{
		char szKey[OLE_MAXNAMESIZE*2];  // more than large enough
		int cch = _fstrlen(lpKey);

		_fstrcpy(szKey, lpKey);
		char* pchNext = &szKey[cch];
		*pchNext++ = '\t';

		GetPrivateProfileString(m_pszSectionName,
			lpKey, "", pchNext, sizeof(szKey)-2-cch, m_pszFileName);
		lpKey += cch + 1;
		m_pList->AddString(szKey);
	}

	_ffree(lpBuffer);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

void CBibDoc::AddItem(const CString& key, const CString& value)
{
	CString both = key + "\t" + value;
	m_pList->SetCurSel(m_pList->AddString(both));

	WritePrivateProfileString(m_pszSectionName, key, value, m_pszFileName);
}

void CBibDoc::DeleteItem(const CString& key, int nIndex)
{
	ASSERT(nIndex != -1);
	m_pList->DeleteString(nIndex);
	WritePrivateProfileString(m_pszSectionName, key, NULL, m_pszFileName);
}

/////////////////////////////////////////////////////////////////////////////

void CBibDoc::GetItemKeyValue(int nIndex, CString& key, CString& value)
{
	ASSERT(nIndex != -1);
	CString both;
	m_pList->GetText(nIndex, both);

	// tab character ('\t') separates key and value
	int i = both.Find("\t");
	if (i == -1)
	{
		// key only "<key>"
		key = both;
		value = "???";
	}
	else
	{
		// format is "<key> \t <value>"
		key = both.Left(i);
		value = both.Mid(i+1);
	}
}

int CBibDoc::GetItemValue(const CString& key, CString& value)
	// return index or -1 if not found
{
	// we could either look in the listbox or the file
	CString findString = key + "\t";
	int nIndex = m_pList->FindString(-1, findString);
	if (nIndex == -1)
		return -1;

	CString key2;
	GetItemKeyValue(nIndex, key2, value);
	ASSERT(key2.CompareNoCase(key) == 0);   // same key (case insensitive)
	return nIndex;
}

/////////////////////////////////////////////////////////////////////////////
// UI Specific

BOOL CBibDoc::ShowItem(const CString& key)
{
	CString findString = key + "\t";
	int nIndex = m_pList->FindString(-1, findString);
	if (nIndex == -1)
	{
		m_pList->SetCurSel(-1);
		return FALSE;
	}
	m_pList->SetCurSel(nIndex);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// OLE Server functionality

BOOL CBibDoc::UpdateClient(const CString& string)
{
	if (m_pSelectedItem != NULL)
	{
		m_pSelectedItem->ChangeKey(string);
	}
	else
	{
		TRACE("Warning: UpdateClient with no item\n");
	}
	
	TRACE("Notifying of saved\n");
	NotifySaved();
	return TRUE;
}

COleServerItem* CBibDoc::OnGetDocument()
{
	TRACE("Creating an item to represent the entire (blank) document\n");
	CBibItem* pItem = new CBibItem(NULL);
	ASSERT(m_pSelectedItem == NULL);
	m_pSelectedItem = pItem;
	return pItem;
}

COleServerItem* CBibDoc::OnGetItem(LPCSTR lpszObjname)
{
	CString key = lpszObjname;
	TRACE("Doc::GetObject(%s)\n", (const char*)key);
	CString findString = key + "\t";
	int nIndex = m_pList->FindString(-1, findString);
	if (nIndex == -1)
	{
		TRACE("couldn't find (%Fs)\n", lpszObjname);
		return NULL;
	}
	
	CBibItem* pItem = new CBibItem(key);
	ASSERT(m_pSelectedItem == NULL);
	m_pSelectedItem = pItem;
	return pItem;
}

COleServerItem* CBibDoc::GetNextItem(POSITION& rPosition)
{
	if (rPosition == NULL)
	{
		rPosition = (POSITION) 1;
		return m_pSelectedItem;
	}
	else
	{
		return NULL;
	}
}

OLESTATUS CBibDoc::OnRelease()
{
	OLESTATUS status;
	if ((status = COleServerDoc::OnRelease()) != OLE_OK)
		return status;

	TRACE("Fake destroying CBibDoc\n");
	//   don't do this -> delete this;
	return OLE_OK;
}

OLESTATUS CBibDoc::OnSetHostNames(LPCSTR lpszHost, LPCSTR lpszHostObj)
{
	// save host names in case needed later
	m_strHost = lpszHost;
	m_strHostObj = lpszHostObj;
	return OLE_OK;
}

/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void CBibDoc::AssertValid() const
{
	COleServerDoc::AssertValid();
	ASSERT(m_pList != NULL);
}


void CBibDoc::Dump(CDumpContext& dc) const
{
	COleServerDoc::Dump(dc);
	dc << "m_pList = " << m_pList << "\n";
	dc << "m_pszFileName = " << m_pszFileName << "\n";
	dc << "m_pszSectionName = " << m_pszSectionName << "\n";
	dc << "m_pSelectedItem = " << m_pSelectedItem;
}

#endif

/////////////////////////////////////////////////////////////////////////////
