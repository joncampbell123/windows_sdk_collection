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
#include "bibitem.h"
#include "bibdoc.h"

/////////////////////////////////////////////////////////////////////////////

CBibItem::CBibItem(const char* pszKey)
{
	m_key = pszKey;     // used for the name of the item
}

OLESTATUS
CBibItem::OnRelease()
{
	OLESTATUS status;
	if ((status = COleServerItem::OnRelease()) != OLE_OK)
		return status;
	delete this;
	return OLE_OK;
}

void
CBibItem::Serialize(CArchive& ar)
{
	// the raw native format for a bib-item is just it's key

	if (ar.IsStoring())
	{
		ar << m_key;
	}
	else
	{
		ar >> m_key;
	}
}


OLESTATUS
CBibItem::OnShow(BOOL /* bTakeFocus */)
{
	// Scroll into view
	if (!GetDocument()->ShowItem(m_key))
	{
		TRACE("no item specified\n");
	}

	// make sure Bibref viewer is the topmost window
	AfxGetApp()->m_pMainWnd->BringWindowToTop();

	return OLE_OK;
}


BOOL
CBibItem::OnGetTextData(CString& rStringReturn)
{
	return GetDocument()->GetItemValue(m_key, rStringReturn) != -1;
}

/////////////////////////////////////////////////////////////////////////////
// Drawing items into bitmap or metafile

BOOL CBibItem::OnDraw(CMetaFileDC* pDC)
{
	CString value;

	if (GetDocument()->GetItemValue(m_key, value) == -1)
		return TRUE;    // leave it blank
	
	pDC->SetMapMode(MM_TEXT);
	pDC->TextOut(0, 0, value);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void CBibItem::AssertValid() const
{
	COleServerItem::AssertValid();
}


void CBibItem::Dump(CDumpContext& dc) const
{
	COleServerItem::Dump(dc);
}

#endif

/////////////////////////////////////////////////////////////////////////////
