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

#ifdef AFX_CORE_SEG
#pragma code_seg(AFX_CORE_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CBitmapButton

IMPLEMENT_DYNAMIC(CBitmapButton, CButton)

// LoadBitmaps will load in the two bitmaps
BOOL CBitmapButton::LoadBitmaps(LPCSTR lpBitmapResource,
	LPCSTR lpBitmapResourceSel, LPCSTR lpBitmapResourceFocus /* = NULL */)
{
	m_bitmap.DeleteObject();
	if (!m_bitmap.LoadBitmap(lpBitmapResource))
	{
		TRACE("Failed to load bitmap for normal image\n");
		return FALSE;
	}
	if (lpBitmapResourceSel != NULL)
	{
		m_bitmapSel.DeleteObject();
		if (!m_bitmapSel.LoadBitmap(lpBitmapResourceSel))
		{
			TRACE("Failed to load bitmap for selected image\n");
			return FALSE;
		}
	}
	if (lpBitmapResourceFocus != NULL)
	{
		m_bitmapFocus.DeleteObject();
		if (!m_bitmapFocus.LoadBitmap(lpBitmapResourceFocus))
		{
			TRACE("Failed to load bitmap for focused image\n");
			return FALSE;
		}
	}
	return TRUE;
}

// SizeToContent will resize the button to the size of the bitmap
void CBitmapButton::SizeToContent()
{
	ASSERT(m_bitmap.m_hObject != NULL);
	CSize bitmapSize;
	BITMAP bmInfo;
	VERIFY(m_bitmap.GetObject(sizeof(bmInfo), &bmInfo) == sizeof(bmInfo));
	VERIFY(SetWindowPos(NULL, -1, -1, bmInfo.bmWidth, bmInfo.bmHeight,
		SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE));
}

// Autoload will load the bitmap resources based on the text of
//  the button
// Using suffices "U", "D" and "F" for up/down/focus (must have all three)
BOOL CBitmapButton::AutoLoad(UINT nID, CWnd* pParent)
{
	// first attach the CBitmapButton to the dialog control
	if (!SubclassDlgItem(nID, pParent))
		return FALSE;

	CString buttonName;
	GetWindowText(buttonName);
	ASSERT(!buttonName.IsEmpty());      // must provide a title

	if (!LoadBitmaps(buttonName + "U", buttonName + "D", buttonName + "F"))
		return FALSE;

	// size to content
	SizeToContent();
	return TRUE;
}

// Draw the appropriate bitmap
void CBitmapButton::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	// must have at least the first bitmap loaded before calling DrawItem
	ASSERT(m_bitmap.m_hObject != NULL);     // required

	// use the main bitmap for up, the selected bitmap for down
	CBitmap* pBitmap = &m_bitmap;
	if ((lpDIS->itemState & ODS_SELECTED) && m_bitmapSel.m_hObject != NULL)
		pBitmap = &m_bitmapSel;
	else if ((lpDIS->itemState & ODS_FOCUS) && m_bitmapFocus.m_hObject != NULL)
		pBitmap = &m_bitmapFocus;   // third image for focused

	if (lpDIS->itemAction & (ODA_DRAWENTIRE|ODA_SELECT|ODA_FOCUS))
	{
		// draw the whole button
		CDC* pDC = CDC::FromHandle(lpDIS->hDC);
		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		CBitmap* pOld = memDC.SelectObject(pBitmap);
		if (pOld == NULL)
			return;     // destructors will clean up

		CRect rect;
		rect.CopyRect(&lpDIS->rcItem);
		pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),
			&memDC, 0, 0, SRCCOPY);
		memDC.SelectObject(pOld);
		// will delete memDC
	}
}

/////////////////////////////////////////////////////////////////////////////
