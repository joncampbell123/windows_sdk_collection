/*===========================================================================*\
|
|  File:        cgtext.h
|
|  Description: 
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/
/**************************************************************************

    (C) Copyright 1995 Microsoft Corp.  All rights reserved.

    You have a royalty-free right to use, modify, reproduce and 
    distribute the Sample Files (and/or any modified version) in 
    any way you find useful, provided that you agree that 
    Microsoft has no warranty obligations or liability for any 
    Sample Application Files which are modified. 

    we do not recomend you base your game on IKlowns, start with one of
    the other simpler sample apps in the GDK

 **************************************************************************/

#ifndef _CGTEXT_H
#define _CGTEXT_H
#include "linklist.h"

//#define   COLOR_RED   (PALETTERGB(255,0,0))
//#define   COLOR_RED   (RGB(255,0,0))
#define COLOR_RED   (PALETTEINDEX(5))
#define COLOR_GREEN (PALETTERGB(0,255,0))
#define COLOR_BLUE  (PALETTERGB(0,0,255))
#define COLOR_BLACK (PALETTERGB(0,0,0))
#define COLOR_WHITE (PALETTERGB(255,255,255))
//#define COLOR_YELLOW  (PALETTERGB(255, 255, 0))
#define COLOR_YELLOW    (PALETTEINDEX(4))
#define COLOR_GREY  (PALETTERGB(149, 149, 149))

#define NO_SHADOW   ((COLORREF)-1)

class CGameText {
private:
    int     maxWidth;
    int     maxHeight;
    RECT        rect;
    int     nLines;
    int     maxLines;
    int     spacing;
    CLinkedList *pLines;
    HDC     hdcText;
    HFONT       hFont;
    HFONT       hOldFont;

public:
    CGameText(HDC, LPRECT, int maxLines=2, int lineSpacing=2);
    ~CGameText();

    void SetSpacing(int lineSpacing) { spacing = lineSpacing; };
    void SetMaxLines(int lines) { maxLines = lines; };

    int AddLine(LPSTR, COLORREF Color = COLOR_RED
    , COLORREF DropColor = COLOR_GREY);
    void ChangeColor(int, COLORREF, COLORREF DropColor = COLOR_GREY);
    LPSTR GetText(int);
    int TextBlt(int ScrollPos=0);
};
#endif
