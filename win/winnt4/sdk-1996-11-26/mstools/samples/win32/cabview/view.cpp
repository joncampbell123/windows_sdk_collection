//*******************************************************************************************
//
// Filename : View.cpp
//	
//				Implementation file for CCabView
//
// Copyright (c) 1994 - 1996 Microsoft Corporation. All rights reserved
//
//*******************************************************************************************


#include "pch.h"

#include "thisdll.h"

#include "resource.h"

#include "folder.h"
#include "view.h"
#include "os.h"

#include "unknown.h"

class CCabView : public CUnknown, public IShellFolderViewCallback
{
public:
	CCabView() {}
	virtual ~CCabView();

	// *** IUnknown methods ***
	STDMETHODIMP QueryInterface(REFIID riid, LPVOID * ppvObj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// *** IShellFolderViewCallback methods ***
	STDMETHODIMP Message(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	HRESULT GetDetailsOf(UINT iColumn, SFVCB_GETDETAILSOF_DATA* lpDetails);
} ;


CCabView::~CCabView()
{
}


STDMETHODIMP CCabView::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
	static const IID *apiid[] = { &IID_IShellFolderViewCallback, NULL };
	LPUNKNOWN aobj[] = { (IShellFolderViewCallback *)this };

	return(QIHelper(riid, ppvObj, apiid, aobj));
}


STDMETHODIMP_(ULONG) CCabView::AddRef()
{
	return(AddRefHelper());
}


STDMETHODIMP_(ULONG) CCabView::Release()
{
	return(ReleaseHelper());
}


STDMETHODIMP CCabView::Message(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	HANDLE_SFVCB_MSG(SFVCB_GETDETAILSOF, GetDetailsOf);

	default:
		return(E_NOTIMPL);
	}

	return(S_OK);
}


struct _CVCOLINFO
{
	UINT iColumn;
	UINT iTitle;
	UINT cchCol;
	UINT iFmt;
} s_aCVColInfo[] = {
    {CV_COL_NAME,     IDS_CV_COL_NAME,     20, LVCFMT_LEFT},
    {CV_COL_SIZE,     IDS_CV_COL_SIZE,     10, LVCFMT_RIGHT},
    {CV_COL_TYPE,     IDS_CV_COL_TYPE,     20, LVCFMT_LEFT},
    {CV_COL_MODIFIED, IDS_CV_COL_MODIFIED, 20, LVCFMT_LEFT},
};

HRESULT CCabView::GetDetailsOf(UINT iColumn, SFVCB_GETDETAILSOF_DATA* lpDetails)
{
	LPCITEMIDLIST pidl = lpDetails->pidl;
	LPCABITEM pit = (LPCABITEM)pidl;

	if (iColumn >= CV_COL_MAX)
	{
		return(E_NOTIMPL);
	}

	lpDetails->str.uType = STRRET_CSTR;
	lpDetails->str.cStr[0] = '\0';

	if (!pit)
	{
		LoadString(g_ThisDll.GetInstance(), s_aCVColInfo[iColumn].iTitle,
			lpDetails->str.cStr, sizeof(lpDetails->str.cStr));
		lpDetails->fmt = s_aCVColInfo[iColumn].iFmt;
		lpDetails->cChar = s_aCVColInfo[iColumn].cchCol;
		lpDetails->lParamSort = iColumn;
		return S_OK;
	}

	switch (iColumn)
	{
	case CV_COL_NAME:
		CCabFolder::GetNameOf(pit, &lpDetails->str);
		break;

	case CV_COL_SIZE:
	{
	    char szOrder[10];

	    LoadString(g_ThisDll.GetInstance(), IDS_ORDERKB, szOrder, sizeof(szOrder));
	    wsprintf(lpDetails->str.cStr, szOrder, (pit->dwFileSize + 1023) / 1024);
		break;
	}

	case CV_COL_TYPE:
		CCabFolder::GetTypeOf(pit, &lpDetails->str);
		break;

	case CV_COL_MODIFIED:
		CFileTime::DateTimeToString(pit->uFileDate, pit->uFileTime,
			lpDetails->str.cStr);
		break;
	}

	return(S_OK);
}


HRESULT CabView_CreateCallback(IShellFolderViewCallback **ppcb)
{
	*ppcb = NULL;

	CCabView *pcb = new CCabView;

	if (!pcb)
	{
		return(E_OUTOFMEMORY);
	}

//  #pragma message("Make 1 the initial refcount in CUnknown")
	pcb->AddRef();

	*ppcb = (IShellFolderViewCallback *)pcb;
	return(NOERROR);
}
