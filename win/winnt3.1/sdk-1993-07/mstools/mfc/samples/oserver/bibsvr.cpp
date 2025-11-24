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
#include "bibsvr.h"
#include "bibdoc.h"

#include "mainwnd.h"

/////////////////////////////////////////////////////////////////////////////

// For an MDI server, a CBibDoc (or CServerDoc) would be created by
//  the server in response to OnOpenDoc, OnCreateDoc or OnEditDoc.
// Since we are an SDI server (just one document at a time), we create
//  one document on program startup, and return it when a new server
//  document is requested.

static CBibDoc* GetOnlyDoc()
{
	// ok we'll fake it
	CMainWnd* pView = (CMainWnd*)AfxGetApp()->m_pMainWnd;
	if (pView == NULL)
		return NULL;
	CBibDoc* pDoc = pView->GetDocument();
	if (pDoc == NULL)
		return NULL;

	ASSERT(!pDoc->IsOpen());
	return pDoc;
}

COleServerDoc*
CBibServer::OnOpenDoc(LPCSTR lpszDoc)
{
	TRACE("BibServer: Open document (%Fs)\n", lpszDoc);
	(void)lpszDoc;  // file name ignored

	return GetOnlyDoc();
}

COleServerDoc*
CBibServer::OnCreateDoc(LPCSTR lpszClass, LPCSTR lpszDoc)
{
	TRACE("BibServer: Create (%Fs, %Fs)\n", lpszClass, lpszDoc);

	return GetOnlyDoc();
}

COleServerDoc*
CBibServer::OnEditDoc(LPCSTR lpszClass, LPCSTR lpszDoc)
{
	TRACE("BibServer: Edit (%Fs, %Fs)\n", lpszClass, lpszDoc);

	return GetOnlyDoc();
}


/////////////////////////////////////////////////////////////////////////////
