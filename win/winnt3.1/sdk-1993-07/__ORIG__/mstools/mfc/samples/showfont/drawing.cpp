// drawing.cpp : Drawing functions for the ShowFont main window.
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

#include "showfont.h"

/////////////////////////////////////////////////////////////////////////////

// InitDC:
// Prepare for drawing in this DC.
//
void CMainWindow::InitDC(CDC& dc)
{
	dc.SetBkMode(nBkMode);
	dc.SetBkColor(rgbBkColor);
	dc.SetTextColor(rgbTextColor);
	dc.SetTextAlign(nAlignLCR | nAlignTBB);
}

/////////////////////////////////////////////////////////////////////////////

// GetStringExtent:
// A helper function which calculates the extent of a string in a given font.
//
static int GetStringExtent(CDC& dc, PSTR pString, CFont* font)
{
	CFont* oldFont = dc.SelectObject(font);
	if (oldFont == NULL)
		return 0;

	CSize sizeExtent = dc.GetTextExtent(pString, strlen(pString));
	dc.SelectObject(oldFont);
	return (sizeExtent.cx);
}

/////////////////////////////////////////////////////////////////////////////

// StringOut:
// Helper routine for font-specific text out.
//
static int StringOut(CDC& dc, short x, short y, PSTR pString, CFont* newFont)
{
	CFont* oldFont = dc.SelectObject(newFont);
	if (oldFont == NULL)
		return 0;
	CSize sizeExtent = dc.GetTextExtent(pString, strlen(pString));
	dc.TextOut(x, y, pString, strlen(pString));
	dc.SelectObject(oldFont);
	return (sizeExtent.cx);
}

/////////////////////////////////////////////////////////////////////////////

// ShowString:
// Draw the string in the client area of this window at the current point.
//
void CMainWindow::ShowString()
{
	short nAlign;
	CFont italicFont, underlineFont, strikeOutFont, boldFont;

	LOGFONT logFont;
	pTheFont->GetObject(sizeof(LOGFONT), &logFont);
	logFont.lfItalic = TRUE;
	italicFont.CreateFontIndirect(&logFont);

	logFont.lfItalic = FALSE;
	logFont.lfUnderline = TRUE;
	underlineFont.CreateFontIndirect(&logFont);

	logFont.lfUnderline = FALSE;
	logFont.lfStrikeOut = TRUE;
	strikeOutFont.CreateFontIndirect(&logFont);

	logFont.lfStrikeOut = FALSE;
	logFont.lfWeight = FW_BOLD;
	boldFont.CreateFontIndirect(&logFont);

	// Draw text examples.
	//
	CClientDC dc(this);
	InitDC(dc);
	int x = ptCurrent.x;
	int y = ptCurrent.y;

	nAlign =  nAlignLCR | nAlignTBB;    /* GetTextAlign(hDC); */

	if ((nAlign & TA_CENTER) == TA_CENTER)
	{
		int tmpX = x;
		nAlignLCR = TA_LEFT;
		dc.SetTextAlign(nAlignLCR | nAlignTBB);
		x += StringOut(dc, x, y, ", and ", pTheFont);
		x += StringOut(dc, x, y, "strikeout", &strikeOutFont);
		x += StringOut(dc, x, y, " in a single line.", pTheFont);
		x = tmpX;

		nAlignLCR = TA_RIGHT;
		dc.SetTextAlign(nAlignLCR | nAlignTBB);
		x -= StringOut(dc, x, y, "underline", &underlineFont);
		x -= StringOut(dc, x, y, ", ", pTheFont);
		x -= StringOut(dc, x, y, "italic", &italicFont);
		x -= StringOut(dc, x, y, ", ", pTheFont);
		x -= StringOut(dc, x, y, "bold", &boldFont);
		x -= StringOut(dc, x, y, "You can use ", pTheFont);
		nAlignLCR = TA_CENTER;
	}
	else if ((nAlign & TA_CENTER) == TA_RIGHT)
	{
		x -= StringOut(dc, x, y, " in a single line.", pTheFont);
		x -= StringOut(dc, x, y, "strikeout", &strikeOutFont);
		x -= StringOut(dc, x, y, ", and ", pTheFont);
		x -= StringOut(dc, x, y, "underline", &underlineFont);
		x -= StringOut(dc, x, y, ", ", pTheFont);
		x -= StringOut(dc, x, y, "italic", &italicFont);
		x -= StringOut(dc, x, y, ", ", pTheFont);
		x -= StringOut(dc, x, y, "bold", &boldFont);
		x -= StringOut(dc, x, y, "You can use ", pTheFont);
	}
	else
	{
		x += StringOut(dc, x, y, "You can use ", pTheFont);
		x += StringOut(dc, x, y, "bold", &boldFont);
		x += StringOut(dc, x, y, ", ", pTheFont);
		x += StringOut(dc, x, y, "italic", &italicFont);
		x += StringOut(dc, x, y, ", ", pTheFont);
		x += StringOut(dc, x, y, "underline", &underlineFont);
		x += StringOut(dc, x, y, ", and ", pTheFont);
		x += StringOut(dc, x, y, "strikeout", &strikeOutFont);
		x += StringOut(dc, x, y, " in a single line.", pTheFont);
	}
}

/////////////////////////////////////////////////////////////////////////////

// ShowCharacterSet:
// Similar to ShowString, but show the entire character set in four rows.
//
void ShowCharacterSet(CDC& dc, CFont* newFont)
{
	CFont* oldFont = dc.SelectObject(newFont);
	if (oldFont == NULL)
		return;

	TEXTMETRIC TextMetric;

	dc.GetTextMetrics(&TextMetric);
	nLineSpace = (TextMetric.tmHeight + TextMetric.tmExternalLeading)*2;

	int y = ptCurrent.y;
	for (int iLine = 0; iLine < 4; iLine++)
	{
		dc.TextOut(ptCurrent.x, y, outputText[iLine], 64);
		y += nLineSpace;
	}
	dc.SelectObject(oldFont);
}

