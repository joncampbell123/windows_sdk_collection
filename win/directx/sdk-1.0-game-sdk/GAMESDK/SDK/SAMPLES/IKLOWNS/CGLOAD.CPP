/*===========================================================================*\
|
|  File:        cgoption.cpp
|
|  Description: 
|   Routines to display lines of text (from resource strings) overlayed on 
|   a bitmap.  The text will be sized appropriately to fit within the 
|   rectangle specified in the profile.  The user may hilight a particular 
|   line and the index of the line will be returned to the caller.
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

#include <windows.h>
#include "cgglobl.h"
#include "strrec.h"
#include "cgdib.h"
#include "cgscreen.h"
#include "cgmidi.h"
#include "cgsound.h"
#include "cgload.h"
#include "cgimage.h"

#define BASE_HWND   ghMainWnd

extern HDC LoadBitmapFile (LPSTR pBitmapFile);
extern LPSTR NewStringResource( HINSTANCE hInst, int idString);

// ----------------------------------------------------------
// CLoadingScreen - 
// ----------------------------------------------------------
CLoadingScreen::CLoadingScreen(
    CGameScreen* pScreen,
    LPSTR       pBitmapName,    // bkgrnd bitmap
    int     iTextId,    // resource text to overlay
    POINT       pt,     // location of circle
    TXTCOLOR    color,      // color of overlay texxt
    RECT        rect,
    CSoundEffect    *pSoundStart,   // sound effect to play
    CSoundEffect    *pSoundUpdate,  // sound effect to play
    CSoundEffect    *pSoundEnd, // sound effect to play
    LPSTR       MidiFile
) : pText( NULL ),
    mpLoadBuffer( NULL ),
    mpScreen( pScreen )
{
    HBRUSH      hBrush;

    HDC hdcScreen = GetDC(BASE_HWND);
    ShowCursor(FALSE);
    PatBlt(hdcScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACKNESS);

    hdcLoading = CreateCompatibleDC(hdcScreen);

    CLoadingScreen::pSoundStart = pSoundStart;
    CLoadingScreen::pSoundUpdate = pSoundUpdate;
    CLoadingScreen::pSoundEnd = pSoundEnd;

    if ((MidiFile != NULL) && (gMusicOn))
    {
        playMusic(MidiFile, TRUE);
    }

    CGameDIB loadDIB( pBitmapName );
    mWidth = loadDIB.GetWidth();
    mHeight = loadDIB.GetHeight();
    mpLoadBuffer = new CGameDSBitBuffer( &loadDIB );

    // Load the bitmap
    if (mpLoadBuffer)
    {
        SelectObject(hdcLoading, mpLoadBuffer->GetHBitmap());

        hBrush = CreateSolidBrush(COLOR_YELLOW);
        SelectObject(hdcLoading, hBrush);
        SetBkMode(hdcLoading, TRANSPARENT);

        if (pSoundStart != NULL) 
        {
            pSoundStart->Play();
        }

        Origin = pt;

        // Create text to be overlayed 
        pText = new CGameText(hdcLoading, &rect, 1, 1);
        pText->AddLine(NewStringResource(ghInst, iTextId)
        , color.main, color.shadow);

// Don't display initially, 'cause palette ain't right yet.
#if 0
        pText->TextBlt();
        mpScreen->Render(0,0, mWidth, mHeight, mpLoadBuffer, 0, 0, SRCCOPY);
        mpScreen->PageFlip();
#endif
    }

    ReleaseDC(BASE_HWND, hdcScreen);
    curTotal = 0;
}

#define MIN_RADIUS  5
// ----------------------------------------------------------
// Update - 
// ----------------------------------------------------------
void CLoadingScreen::Update(
    int Increment
)
{
    int radius;
    RECT    rect;

    curTotal += Increment;
    radius = curTotal;

    rect.left = Origin.x - radius;
    rect.top = Origin.y - radius;
    rect.right = Origin.x + radius;
    rect.bottom = Origin.y + radius;

    Ellipse(hdcLoading, rect.left, rect.top, rect.right, rect.bottom);
    pText->TextBlt();
    mpScreen->Render(0,0, mWidth, mHeight, mpLoadBuffer, 0, 0, SRCCOPY);
    mpScreen->PageFlip();

}

// ----------------------------------------------------------
// Paint
// ----------------------------------------------------------
void CLoadingScreen::Paint()
{
    mpScreen->Render(0,0, mWidth, mHeight, mpLoadBuffer, 0, 0, SRCCOPY);
    mpScreen->PageFlip();
}

// ----------------------------------------------------------
// ~CLoadingScreen - 
// ----------------------------------------------------------
CLoadingScreen::~CLoadingScreen()
{
    closeMusic();

    delete pText;

    if (pSoundStart != NULL)
    {
        delete pSoundStart;
    }
    if (pSoundUpdate != NULL)
    {
        delete pSoundUpdate;
    }

    RECT    rect = {0,0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1};
    mpScreen->ColorFill(&rect, 0);
    mpScreen->PageFlip();

    if (pSoundEnd != NULL)
    {
        pSoundEnd->Play();
    }

    DeleteDC( hdcLoading );
    delete mpLoadBuffer;
    ShowCursor(TRUE);
}
