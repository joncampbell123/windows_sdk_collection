/*===========================================================================*\
|
|  File:        cgtext.cpp
|
|  Description: 
|   Object class to manage lines of text to be displayed over a bitmap.
|   The text may have an optional drop shadow and may be scrolled
|   vertically.
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995-1996 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/

/**************************************************************************

    (C) Copyright 1995-1996 Microsoft Corp.  All rights reserved.

    You have a royalty-free right to use, modify, reproduce and 
    distribute the Sample Files (and/or any modified version) in 
    any way you find useful, provided that you agree that 
    Microsoft has no warranty obligations or liability for any 
    Sample Application Files which are modified. 

    we do not recomend you base your game on IKlowns, start with one of
    the other simpler sample apps in the GDK

 **************************************************************************/

//** include files **
#include <windows.h>
#include <windowsx.h>
#include "cgtext.h"

//** local definitions **
typedef struct {
    LPSTR       text;       // actual text string
    int     nChars;     // number of characters in string
    int     X;      // location of text in rectangle
    int     Y;
    COLORREF    color;      // color of text
    COLORREF    dropColor;  // color of shadow
    int     shadowX;    // horizontal offset of shaadow
    int     shadowY;    // vertical offset of shadow
} LINE_INFO;

#define DEFAULT_SHADOWX 2
#define DEFAULT_SHADOWY 1

#define MAX(x,y)    (((x) > (y)) ? (x) : (y))

//** external functions **
//** external data **
//** public data **
//** private data **
//** public functions **
//** private functions **

// ----------------------------------------------------------
// CGameText contructor - 
// ----------------------------------------------------------
CGameText::CGameText(
    HDC     hdc,        // hdc to blit to
    LPRECT      pRect,      // rectangle to center text
    int     screenLines,    // max lines on screen at once
    int     linespacing // 1=single, 2=double 
) : hOldFont( NULL )
{
    nLines = 0;
    maxLines = screenLines;
    maxWidth = 0;
    maxHeight = 0;
    hdcText = hdc;
    spacing = linespacing;
    hFont = NULL;
    memcpy(&rect, pRect, sizeof(RECT));
    pLines = new CLinkedList;

    SetBkMode(hdc, TRANSPARENT);
}

// ----------------------------------------------------------
// CGameText destructor - 
// ----------------------------------------------------------
CGameText::~CGameText()
{
    LINE_INFO   *pCur;

    // remove all the text and delete entry in list
    while ((pCur = (LINE_INFO *)pLines->RemoveFirst()) != NULL)
    {
        delete pCur->text;  // hope caller alloced these with new!
        delete pCur;
    }

    delete pLines;

    if (hOldFont)
        SelectObject( hdcText, hOldFont );

    if (hFont)
        DeleteObject(hFont);
}

// ----------------------------------------------------------
// AddLine - add a new line of text to the list
// ----------------------------------------------------------
int CGameText::AddLine(LPSTR NewLine, COLORREF Color, COLORREF DropShadow)
{
    LINE_INFO   *pCur = new LINE_INFO;

    pCur->text = NewLine;
    pCur->nChars = lstrlen(NewLine);
    pCur->X = -1;
    pCur->Y = -1;
    pCur->color = Color;
    pCur->shadowX = 0;
    pCur->shadowY = 0;
    pCur->dropColor = DropShadow;
    if (DropShadow != NO_SHADOW)
    {
        pCur->shadowX = DEFAULT_SHADOWX;
        pCur->shadowY = DEFAULT_SHADOWY;
    } else {
        pCur->shadowX = 0;
        pCur->shadowY = 0;
    }

    pLines->Append(pCur);
    return (++nLines);
}

// ----------------------------------------------------------
// ChangeColor - change the color of a particular string in
//  the display list.  The text is repainted onto the DC.
// ----------------------------------------------------------
void CGameText::ChangeColor(
    int     LineIndex,  // index of string to recolor
    COLORREF    NewColor,   // color to make string
    COLORREF    DropColor   // color of drop shadow
)
{
    LINE_INFO   *pCur;

    if ((LineIndex <= 0) || (LineIndex > nLines))
    {
        return;
    }

    // Find the line in list
    pCur = (LINE_INFO *)pLines->GetFirst();
    for (int ii=1; ii < LineIndex; ii++)
    {
        pCur = (LINE_INFO *)pLines->GetNext();
    }

    // Change the color of the entry
    pCur->color = NewColor;
    pCur->dropColor = DropColor;

    // Display drop shadow if any
    if (pCur->shadowX || pCur->shadowY)
    {
        SetTextColor(hdcText, pCur->dropColor);
        TextOut(hdcText, pCur->X+pCur->shadowX, pCur->Y+pCur->shadowY
        , pCur->text, pCur->nChars);
    }

    // Display text
    SetTextColor(hdcText, pCur->color);
    TextOut(hdcText, pCur->X, pCur->Y, pCur->text, pCur->nChars);
}

// ----------------------------------------------------------
// GetText - retrieve text assoicated with a particular index
// ----------------------------------------------------------
LPSTR CGameText::GetText(
    int LineIndex
)
{
    LINE_INFO   *pCur;

    if ((LineIndex <= 0) || (LineIndex > nLines))
    {
        return(NULL);
    }

    pCur = (LINE_INFO *)pLines->GetFirst();
    for (int ii=1; ii < LineIndex; ii++)
    {
        pCur = (LINE_INFO *)pLines->GetNext();
    }
    
    return(pCur->text);
}

// ----------------------------------------------------------
// TextBlt - draw text to DC
// ----------------------------------------------------------
int CGameText::TextBlt(
    int ScrollPos       // offset using for scrolling the data
)
{
    LINE_INFO   *pCur;
    int     TextX, TextY;
    int         totalLines;
    int     nDisplayed=0;

    // Figure out how many lines are displayed and how tall each line is

    if (maxLines <= 2)
        maxLines = nLines;

//  totalLines = nLines < maxLines ? nLines : maxLines;
    totalLines = maxLines;
    if (totalLines < 2)
        totalLines = 2;

    maxHeight = (rect.bottom - rect.top) / ((totalLines * spacing) - 1);

    // figure out starting point (may be off screen if we are scrolling)
    TextY = rect.top+ScrollPos; 
    TextX = rect.left;

    // If we don't have a font yet -- figure out how big of a font to use
    // and create it.
    if (hFont == NULL)
    {
        LOGFONT logFont;
        memset(&logFont, 0, sizeof(LOGFONT));
        logFont.lfHeight = maxHeight;
        logFont.lfPitchAndFamily = FF_ROMAN;
        hFont = CreateFontIndirect(&logFont);
                hOldFont = (HFONT)SelectObject(hdcText, hFont);
    }


    // Go through each line of text and display it to the screen.
    pCur = (LINE_INFO *)pLines->GetFirst();
    while (pCur != NULL)
    {

        // Don't bother if text won't be visible
        if ((TextY+maxHeight > rect.top) && (TextY <rect.bottom))
        {

            // paint shadow of text
            if (pCur->shadowX || pCur->shadowY)
            {
                SetTextColor(hdcText, pCur->dropColor);
                ExtTextOut(hdcText, TextX+pCur->shadowX
                , TextY+pCur->shadowY
                , ETO_CLIPPED, &rect
                , pCur->text, pCur->nChars, NULL);
            }

            // paint text
            SetTextColor(hdcText, pCur->color);
            ExtTextOut(hdcText, TextX, TextY
            , ETO_CLIPPED, &rect
            , pCur->text, pCur->nChars, NULL);

            nDisplayed++;
        }

        // save position of text
        pCur->X = TextX;
        pCur->Y = TextY;
        TextY += maxHeight * spacing;

        pCur = (LINE_INFO *)pLines->GetNext();
    }

    return(nDisplayed);
}
