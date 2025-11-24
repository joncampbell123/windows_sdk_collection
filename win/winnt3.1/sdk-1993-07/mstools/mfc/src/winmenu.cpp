// This is a part of the Microsoft Foundation Classes C++ library. 
// Copyright (C) 1992 Microsoft Corporation 
// All rights reserved. 
//  
// This source code is only intended as a supplement to the 
// Microsoft Foundation Classes Reference and Microsoft 
// QuickHelp documentation provided with the library. 
// See these sources for detailed information regarding the 
// Microsoft Foundation Classes product. 


#include "afxwin.h"
#pragma hdrstop

#include "winhand_.h"

#ifdef AFX_CORE_SEG
#pragma code_seg(AFX_CORE_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// Map from HMENU to CMenu *

class NEAR CMenuHandleMap : public CHandleMap
{
public:
	CObject* NewTempObject(HANDLE h)
				{
					// don't add in permanent
					CMenu* p = new CMenu();
					p->m_hMenu = (HMENU)h;     // set after constructed
					return p;
				}
	void DeleteTempObject(CObject* ob)
				{
					ASSERT(ob->IsKindOf(RUNTIME_CLASS(CMenu)));
					((CMenu*)ob)->m_hMenu = NULL;   // clear before destructed
					delete ob;
				}
};
static CMenuHandleMap NEAR hMenuMap;

// Map from HMENU to CMenu*
CMenu*
CMenu::FromHandle(HMENU hMenu)
{
	return (CMenu*)hMenuMap.FromHandle(hMenu);
}

void
CMenu::DeleteTempMap()
{
	hMenuMap.DeleteTemp();
}

/////////////////////////////////////////////////////////////////////////////
// CMenu

IMPLEMENT_DYNAMIC(CMenu, CObject)

#ifdef _DEBUG
void CMenu::AssertValid() const
{
	CObject::AssertValid();
}

void CMenu::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
	dc << "m_hMenu = " << (void NEAR*)m_hMenu;
}
#endif

BOOL
CMenu::Attach(HMENU hMenu)
{
	ASSERT(m_hMenu == NULL);        // only attach once, detach on destroy
	if (hMenu == NULL)
		return FALSE;
	hMenuMap.SetPermanent(m_hMenu = hMenu, this);
	return TRUE;
}

HMENU
CMenu::Detach()
{
	HMENU hMenu;
	if ((hMenu = m_hMenu) != NULL)
		hMenuMap.RemovePermanent(m_hMenu);
	m_hMenu = NULL;
	return hMenu;
}

CMenu::~CMenu()
{
	DestroyMenu();
}

BOOL
CMenu::DestroyMenu()
{
	if (m_hMenu == NULL)
		return FALSE;
	return ::DestroyMenu(Detach());
}

/////////////////////////////////////////////////////////////////////////////
// Self-drawing menu items

void CMenu::DrawItem(LPDRAWITEMSTRUCT /* lpDrawItemStruct */)
{
	// default drawing does nothing
}

void CMenu::MeasureItem(LPMEASUREITEMSTRUCT /* lpMeasureItemStruct */)
{
	// default drawing does nothing
}


/////////////////////////////////////////////////////////////////////////////
