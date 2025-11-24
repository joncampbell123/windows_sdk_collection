// custmenu.cpp : custom menu
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

#include "ctrltest.h"

/////////////////////////////////////////////////////////////////////////////

// for owner draw menus, the CMenu object is embedded in the main frame window
//  the CMenu stays attached to the HMENU while it is running so that
//  owner draw messages are delegated to this class.
//  Since we attach the HMENU to a menu bar (with ModifyMenu below), we
//  don't want to delete the menu twice - so we detach on the destructor.

CColorMenu::CColorMenu()
{
	VERIFY(CreateMenu());
}

CColorMenu::~CColorMenu()
{
	Detach();
	ASSERT(m_hMenu == NULL);    // defaul CMenu::~CMenu will destroy
}

void CColorMenu::AppendColorMenuItem(UINT nID, COLORREF color)
{
	VERIFY(AppendMenu(MF_ENABLED | MF_OWNERDRAW, nID, (LPCSTR)color));
}

/////////////////////////////////////////////////////////////////////////////

#define COLOR_BOX_WIDTH     20
#define COLOR_BOX_HEIGHT    20


void CColorMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	// all items are of fixed size
	lpMIS->itemWidth = COLOR_BOX_WIDTH;
	lpMIS->itemHeight = COLOR_BOX_HEIGHT;
}

void CColorMenu::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	COLORREF cr = lpDIS->itemData; // RGB in item data

	if (lpDIS->itemAction & ODA_DRAWENTIRE)
	{
		// Paint the color item in the color requested
		CBrush br(cr);
		pDC->FillRect(&lpDIS->rcItem, &br);
	}

	if ((lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{
		// item has been selected - hilite frame
		COLORREF crHilite = RGB(255-GetRValue(cr),
						255-GetGValue(cr), 255-GetBValue(cr));
		CBrush br(crHilite);
		pDC->FrameRect(&lpDIS->rcItem, &br);
	}

	if (!(lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & ODA_SELECT))
	{
		// Item has been de-selected -- remove frame
		CBrush br(cr);
		pDC->FrameRect(&lpDIS->rcItem, &br);
	}
}

/////////////////////////////////////////////////////////////////////////////

// Call AttachCustomMenu once
//   it will replace the menu item with the ID  'IDM_TEST_CUSTOM_MENU'
//   with a color menu popup
// Replace the specified menu item with a color popup
void CTestWindow::AttachCustomMenu()
{
	// now add a few new menu items
	for (int iColor = 0; iColor <= (IDM_COLOR_LAST-IDM_COLOR_FIRST); iColor++)
	{
		// 3 bit encoded RGB values
		BYTE red = (iColor & 4) ? 0xff : 0;
		BYTE green = (iColor & 2) ? 0xff : 0;
		BYTE blue = (iColor & 1) ? 0xff : 0;

		m_colorMenu.AppendColorMenuItem(IDM_COLOR_FIRST + iColor,
			RGB(red, green, blue));
	}

	// Replace the specified menu item with a color popup
	//  (note: will only work once)
	CMenu* pMenuBar = GetMenu();
	ASSERT(pMenuBar != NULL);
	char szString[256];     // don't change the string

	pMenuBar->GetMenuString(IDM_TEST_CUSTOM_MENU, szString, sizeof(szString),
		MF_BYCOMMAND);
	VERIFY(GetMenu()->ModifyMenu(IDM_TEST_CUSTOM_MENU, MF_BYCOMMAND | MF_POPUP,
		(UINT)m_colorMenu.m_hMenu, szString));
}

/////////////////////////////////////////////////////////////////////////////

char* colorNames[] =
{
	"black", "blue", "green", "cyan",
	"red", "magenta", "yellow", "white"
};

BOOL CTestWindow::OnCommand(UINT wParam, LONG lParam)
{
	if (wParam < IDM_COLOR_FIRST || wParam > IDM_COLOR_LAST)
		return CFrameWnd::OnCommand(wParam, lParam);        // default

	// special color selected
	char szT[256];
	sprintf(szT, "You picked %s", colorNames[wParam - IDM_COLOR_FIRST]);
	MessageBox(szT, "MenuTest");
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
