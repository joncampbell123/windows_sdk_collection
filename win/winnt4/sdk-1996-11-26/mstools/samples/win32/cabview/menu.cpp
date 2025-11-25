//*******************************************************************************************
//
// Filename : Menu.cpp
//	
//				Implementations for CCabItemMenu methods
//
// Copyright (c) 1994 - 1996 Microsoft Corporation. All rights reserved
//
//*******************************************************************************************

#include "pch.h"

#include "thisdll.h"

#include "resource.h"

#include "folder.h"
#include "menu.h"

#include "cabitms.h"

CCabItemMenu::CCabItemMenu(HWND hwndOwner, CCabFolder*pcf, LPCABITEM *apit, UINT cpit)
: m_lSel(8)
{
	m_hwndOwner = hwndOwner;
	m_pcfHere = pcf;
	pcf->AddRef();

	// No need to check return value here; check in QueryInterface
	m_lSel.AddItems(apit, cpit);
}

CCabItemMenu::~CCabItemMenu()
{
	m_pcfHere->Release();
}

// *** IUnknown methods ***
STDMETHODIMP CCabItemMenu::QueryInterface(
   REFIID riid, 
   LPVOID FAR* ppvObj)
{
	*ppvObj = NULL;

	if (m_lSel.GetState() == CCabItemList::State_OutOfMem)
	{
		return(E_OUTOFMEMORY);
	}

	LPUNKNOWN pObj;
 
	if (riid == IID_IUnknown)
	{
		pObj = (LPUNKNOWN)(IUnknown*)((IContextMenu*)this); 
		// The (IShellFolder*) ^^^ up there is to disambiguate :) the reference
	}
	else if (riid == IID_IContextMenu)
	{
		pObj = (LPUNKNOWN)(IContextMenu*)this;
	}
	else
	{
   		return(E_NOINTERFACE);
	}

	pObj->AddRef();
	*ppvObj = pObj;

	return(NOERROR);
}


STDMETHODIMP_(ULONG) CCabItemMenu::AddRef(void)
{
	return(m_cRef.AddRef());
}


STDMETHODIMP_(ULONG) CCabItemMenu::Release(void)
{
	if (!m_cRef.Release())
	{
   		delete this;
		return(0);
	}

	return(m_cRef.GetRef());
}


// *** IContextMenu methods ***
STDMETHODIMP CCabItemMenu::QueryContextMenu(
                                HMENU hmenu,
                                UINT indexMenu,
                                UINT idCmdFirst,
                                UINT idCmdLast,
                                UINT uFlags)
{
    HMENU hmMerge = LoadPopupMenu(MENU_ITEMCONTEXT, 0);

	if (!hmMerge)
	{
		return(E_OUTOFMEMORY);
	}

	UINT idMax = Cab_MergeMenus(hmenu, hmMerge, indexMenu, idCmdFirst, idCmdLast,
		MM_ADDSEPARATOR);

	DestroyMenu(hmMerge);

	SetMenuDefaultItem(hmenu, IDC_ITEM_EXTRACT+idCmdFirst, FALSE);

	return(ResultFromShort(idMax - idCmdFirst));
}

STDMETHODIMP CCabItemMenu::InvokeCommand(
                             LPCMINVOKECOMMANDINFO lpici)
{
	if (HIWORD(lpici->lpVerb))
	{
		// Deal with string commands
		return(E_INVALIDARG);
	}

	switch ((UINT)LOWORD((DWORD)lpici->lpVerb))
	{
	case IDC_ITEM_EXTRACT:
	{
		char szHere[MAX_PATH];
		if (!m_pcfHere->GetPath(szHere))
		{
			return(E_UNEXPECTED);
		}

		CCabExtract ceHere(szHere);

		ceHere.ExtractItems(m_hwndOwner, NULL, ShouldExtract, (LPARAM)this);

		break;
	}

	default:
		return(E_INVALIDARG);
	}

	return(NOERROR);
}

STDMETHODIMP CCabItemMenu::GetCommandString(
                                UINT        idCmd,
                                UINT        uType,
                                UINT      * pwReserved,
                                LPSTR       pszName,
                                UINT        cchMax)
{
	return(E_NOTIMPL);
}


HGLOBAL * CALLBACK CCabItemMenu::ShouldExtract(LPCSTR pszFile, DWORD dwSize, UINT date,
		UINT time, UINT attribs, LPARAM lParam)
{
	CCabItemMenu *pThis = (CCabItemMenu*)lParam;

	if (pThis->m_lSel.IsInList(pszFile, dwSize, date, time, attribs))
	{
		return(EXTRACT_TRUE);
	}

	// Copy nothing for now
	return(EXTRACT_FALSE);
}


HMENU CCabItemMenu::LoadPopupMenu(UINT id, UINT uSubMenu)
{
    HMENU hmParent = LoadMenu(g_ThisDll.GetInstance(), MAKEINTRESOURCE(id));
    if (!hmParent)
    {
		return(NULL);
	}

    HMENU hmPopup = GetSubMenu(hmParent, 0);
    RemoveMenu(hmParent, uSubMenu, MF_BYPOSITION);
    DestroyMenu(hmParent);

    return(hmPopup);
}
