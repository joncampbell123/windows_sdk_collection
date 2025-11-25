/*===========================================================================*\
|
|  File:        cgscreen.h
|
|  Description: 
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

#ifndef CGSCREEN_H
#define CGSCREEN_H

#include <windows.h>
#include <ddraw.h>
#include "cgbitbuf.h"

enum ST_TYPE
{
    ST_ROOT,    // CGameScreen -- abstract base class
    ST_DDraw,   // CGameDDrawScreen -- Direct Draw screen
    ST_DS       // CGameDSScreen    -- DIBSections screen
};

class CGameScreen
{
public:
    CGameScreen(HWND hwnd, int logWidth, int logHeight, int orgX=0, int orgY=0);
    virtual ~CGameScreen();

    virtual void SetPalette(HPALETTE);
    virtual void SetPalette( char* ) = 0;
    
    virtual void Render(
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            CGameBitBuffer* pSrcBuffer,
            int xSrc,
            int ySrc,
            DWORD rop
            ){};

    // render with transparency mask
    virtual void TransRender(
        int xDest,
        int yDest,
        int wDest,
        int hDest,
        CGameBitBuffer* pSrcBuffer,
        int xSrc,
        int ySrc
        ) = 0;

    virtual void PageFlip()=0;
    virtual void SetMode( int width, int height, int bits )=0;
    virtual void RestoreMode()=0;

    virtual void ColorFill( LPRECT pRect, int palIndex )=0;

    // report this object's type
    virtual ST_TYPE TypeID()
    {
        return ST_ROOT;
    }

    virtual void Refresh()
    {
    }

protected:
    HWND mhwnd;             // parent window
    int mOutWidth;          // width of output screen
    int mOutHeight;         // height of output screen
    RECT mScreenRect;       // describes entire screen

    int mxCurrent;          // current x scroll position
    int myCurrent;          // current y scroll position

    HPALETTE mOldPalette;   // saved to restore when object goes away
};

class CGameDDrawScreen : public CGameScreen
{
public:
    CGameDDrawScreen(HWND hwnd, int logWidth, int logHeight, int orgX=0, int orgY=0);
    virtual ~CGameDDrawScreen();

    virtual void Render(
        int xDest,
        int yDest,
        int wDest,
        int hDest,
        CGameBitBuffer* pSrcBuffer,
        int xSrc,
        int ySrc,
        DWORD rop
        );

    // render with transparency mask
    virtual void TransRender(
        int xDest,
        int yDest,
        int wDest,
        int hDest,
        CGameBitBuffer* pSrcBuffer,
        int xSrc,
        int ySrc
        );

    virtual void PageFlip();
    virtual void SetMode( int width, int height, int bits );
    virtual void ShowGDIPage();
    virtual void RestoreMode();
    virtual void ColorFill( LPRECT pRect, int palIndex );

    virtual void SetPalette( char* );

    // use direct draw to determine total video memory
    int GetVideoMemory();

    // report this object's type
    virtual ST_TYPE TypeID()
    {
        return ST_DDraw;
    }

    virtual void Refresh();

protected:
    CGameDDrawScreenBuffer* mpSurfaces[2];  // ptrs to front & back buffer
    int mBackSurface;           // index to current back buffer
};

// use CreateDIBSection for accessing bits
class CGameDSScreen : public CGameScreen
{
public:
    CGameDSScreen(HWND hwnd, int logWidth, int logHeight, int orgX=0, int orgY=0);
    virtual ~CGameDSScreen();

    virtual void Render(
        int xDest,
        int yDest,
        int wDest,
        int hDest,
        CGameBitBuffer* pSrcBuffer,
        int xSrc,
        int ySrc,
        DWORD rop
        );

    // render with transparency mask
    virtual void TransRender(
        int xDest,
        int yDest,
        int wDest,
        int hDest,
        CGameBitBuffer* pSrcBuffer,
        int xSrc,
        int ySrc
        );

    virtual void PageFlip();
    virtual void SetMode( int width, int height, int bits ){};
    virtual void ShowGDIPage(){};
    virtual void RestoreMode(){};
    virtual void SetPalette(HPALETTE);
    virtual void SetPalette( char* );
    virtual void ColorFill( LPRECT pRect, int palIndex );

    // report this object's type
    virtual ST_TYPE TypeID()
    {
        return ST_DS;
    }

protected:
    HDC mhdcOut;            // keep output hdc around (assumes window is CS_OWNDC!)
    HDC mhdcBackBuffer;     // for holding backbuffer

    CGameDIB* mpBufferDIB;      // ptr to back buffer
    CGameDIB* mpScreenDIB;      // ptr to screen

    HBITMAP mhbmOld;        // for restoring screen's bitmap
};

#endif // CGSCREEN_H
