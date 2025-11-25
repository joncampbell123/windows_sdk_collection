/*===========================================================================*\
|
|  File:        cgbitbuf.h
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

#ifndef CGBITBUF_H
#define CGBITBUF_H

#include <windows.h>
#include <ddraw.h>
#include "cgdib.h"

// We use this value to time out on waiting for Direct Draw so we don't infinite loop
// It should be very big to keep us from timing out too soon & tearing
#define DDRAW_RETRY 40000
#if defined(__BORLANDC__) || defined(__WATCOMC__)
extern "C" {
#endif
void
#if defined(__WATCOMC__)
_cdecl
#endif
TransCopyDIBBits(
        LPBYTE pDest,       // destination
        LPBYTE pSource,     //         ; source pointer
        DWORD dwWidth,      //         ; width pixels
        DWORD dwHeight,     //        ; height pixels
        DWORD dwScanD,      //         ; width bytes dest
        DWORD dwScanS,      //         ; width bytes source
        BYTE bTranClr       //        ; transparent color
    );

void
#if defined(__WATCOMC__)
_cdecl
#endif
CopyDIBBits(
        LPBYTE pDest,   //           ; dest pointer
        LPBYTE pSource, //         ; source pointer
        DWORD dwWidth,  //         ; width pixels
        DWORD dwHeight, //        ; height pixels
        DWORD dwScanD,  //         ; width bytes dest
        DWORD dwScanS   //         ; width bytes source
    );
#if defined(__BORLANDC__) || defined(__WATCOMC__)
}
#endif

class CGameDSBitBuffer;
class CGameDDrawBitBuffer;
enum BB_TYPE
{
    BB_ROOT,    // CGameBitBuffer -- abstract base class
    BB_DDraw,   // CGameDDrawBitBuffer -- Direct Draw surfaces
    BB_DS       // CGameDSBitBuffer -- DIBSections
};

class CGameBitBuffer
{
public:
    // construct a buffer from existing DIB, optionally stretching to fit new size
    CGameBitBuffer(CGameDIB* pDIB, int width=0, int height=0, COLORREF trans=PALETTEINDEX(1));

    // construct an "empty" buffer with given size
    CGameBitBuffer(int width, int height, HPALETTE hPal = NULL, COLORREF trans=PALETTEINDEX(1));

    // blt to this buffer from pSrcBuffer
    virtual void Blt(
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            CGameBitBuffer* pSrcBuffer,
            int xSrc,
            int ySrc,
            DWORD rop
            )=0;

    // blt to pDestBuffer from this buffer
    virtual void Blt(
            CGameBitBuffer* pDestBuffer,
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            int xSrc,
            int ySrc,
            DWORD rop
            )=0;

    // transparent blt to pSrcBuffer from this buffer
    virtual void TransBlt(
            CGameBitBuffer* pDestBuffer,
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            int xSrc,
            int ySrc,
            int transColor
            )=0;

    // specific blit for a DDraw buffer
    virtual void BltDDraw(
            CGameDDrawBitBuffer* pDestBuffer,
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            int xSrc,
            int ySrc,
            DWORD rop
            )=0;

    virtual void TransBltDDraw(
            CGameDDrawBitBuffer* pDestBuffer,
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            int xSrc,
            int ySrc,
            int transColor
            )=0;

    // specific blit for a dibsection buffer
    virtual void BltDS(
            CGameDSBitBuffer* pDestBuffer,
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            int xSrc,
            int ySrc,
            DWORD rop
            )=0;

    virtual void TransBltDS(
            CGameDSBitBuffer* pDestBuffer,
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            int xSrc,
            int ySrc,
            int transColor
            )=0;

    virtual ~CGameBitBuffer();

    // returns mhPalette -- the hpalette we created
    virtual HPALETTE GetHPalette()
    {
        return mhPalette;
    }

    // destroys mhPalette & sets current palette leaving mhPalette NULL
    virtual void SetPalette( HPALETTE )= 0;

    virtual COLORREF GetTransColor()
    {
        return mTransColor;
    }

    // report whether this object is a valid bitbuffer
    virtual BOOL IsValid()
    {
        return FALSE;   // default objects are not valid
    }

    // report this object's type
    virtual BB_TYPE TypeID()
    {
        return BB_ROOT;
    }

protected:
    HPALETTE mhPalette;
    COLORREF mTransColor;       // transparency color
    CGameBitBuffer() :
        mhPalette( NULL )
    {
    }
};

class CGameDDrawBitBuffer : public CGameBitBuffer
{
public:
    // construct a buffer from existing DIB, optionally stretching to fit new size
    CGameDDrawBitBuffer(CGameDIB* pDIB, int width=0, int height=0, COLORREF trans=PALETTEINDEX(1));

    // construct an "empty" buffer with given size
    CGameDDrawBitBuffer(int width, int height, HPALETTE hPal = NULL, COLORREF trans=PALETTEINDEX(1));

    virtual ~CGameDDrawBitBuffer();

    // blt to this buffer
    virtual void Blt(
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            CGameBitBuffer* pSrcBuffer,
            int xSrc,
            int ySrc,
            DWORD rop
            );

    // generic dest blitting calls can handle any type of bitbuffer
    virtual void Blt(
            CGameBitBuffer* pDestBuffer,
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            int xSrc,
            int ySrc,
            DWORD rop
            );

    virtual void TransBlt(
            CGameBitBuffer* pDestBuffer,
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            int xSrc,
            int ySrc,
            int transColor
            );

    // specific blit for another DDraw buffer
    virtual void BltDDraw(
            CGameDDrawBitBuffer* pDestBuffer,
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            int xSrc,
            int ySrc,
            DWORD rop
            );

    virtual void TransBltDDraw(
            CGameDDrawBitBuffer* pDestBuffer,
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            int xSrc,
            int ySrc,
            int transColor
            );

    // specific blit for a dibsection buffer
    virtual void BltDS(
            CGameDSBitBuffer* pDestBuffer,
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            int xSrc,
            int ySrc,
            DWORD rop
            );

    virtual void TransBltDS(
            CGameDSBitBuffer* pDestBuffer,
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            int xSrc,
            int ySrc,
            int transColor
            );

    virtual void SetPalette( HPALETTE );

    // report whether this object is a valid bitbuffer
    virtual BOOL IsValid()
    {
        return mIsValid;
    }

    // report this object's type
    virtual BB_TYPE TypeID()
    {
        return BB_DDraw;
    }

protected:
    // we keep a single ptr around to access DDraw
    static LPDIRECTDRAW mpDDrawDriver;
    static int mInstanceCount;  // for allocating/freeing mpDDrawDriver
    BOOL mIsAttached;   // flag whether this is backbuffer
    BOOL mIsValid;
    long mPitch;

    LPDIRECTDRAWSURFACE mpSurface;      // ptr to our DDraw surface
    DDSURFACEDESC mSurfD;           // cache the surface description

    char* mpFileName;       // we need to keep dib's filename around
                    // in case we need to reload

    friend class CGameDDrawScreen;
    friend class CGameDSBitBuffer;

    friend DWORD CALLBACK EnumCallback(LPVOID lpContext, LPDDSURFACEDESC lpDDrawSurfaceInfo);

    void InitDDraw();

    CGameDDrawBitBuffer() :
        mpSurface( NULL ),
        mIsAttached( FALSE ),
        mIsValid( FALSE )
    {
    };

    // create this DDraw surface
    virtual void CreateSurface(int width, int height);
    // if surface is lost (e.g. from mode switch), call this to recreate it
    virtual void ReCreate();

    void SetBits( CGameDIB* pSource );

    virtual LPBYTE GetLockedAddress( int x, int y );
    virtual void Unlock();
};

class CGameDDrawScreenBuffer : public CGameDDrawBitBuffer
{
public:
    CGameDDrawScreenBuffer();

    // create an attached-buffer object from given primary surface
    CGameDDrawScreenBuffer( CGameDDrawScreenBuffer* pFront );
    virtual void SetMode( int width, int height, int bits );
    virtual void ShowGDIPage();
    virtual void RestoreMode();
    virtual void SetPalette( LPPALETTEENTRY );
    virtual void DeleteDDPalette();

    int GetVideoMemory();

    virtual ~CGameDDrawScreenBuffer();

protected:
    // create the screen's primary surface
    virtual void CreateSurface(int width, int height);
    // if surface is lost (e.g. from mode switch), call this to recreate it
//  virtual void ReCreate();
    LPDIRECTDRAWPALETTE mpPalette;
    LPDIRECTDRAWPALETTE mpOldPalette;
};

class CGameDSBitBuffer : public CGameBitBuffer
{
public:
    // construct a buffer from existing DIB, optionally stretching to fit new size
    CGameDSBitBuffer(CGameDIB* pDIB, int width=0, int height=0, COLORREF trans=PALETTEINDEX(1));

    // construct an "empty" buffer with given size
    CGameDSBitBuffer(int width, int height, HPALETTE hPal = NULL, COLORREF trans=PALETTEINDEX(1));

    virtual ~CGameDSBitBuffer();

    // blt to this buffer
    virtual void Blt(
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            CGameBitBuffer* pSrcBuffer,
            int xSrc,
            int ySrc,
            DWORD rop
            );

    // generic dest blitting calls can handle any type of bitbuffer
    virtual void Blt(
            CGameBitBuffer* pDestBuffer,
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            int xSrc,
            int ySrc,
            DWORD rop
            );

    virtual void TransBlt(
            CGameBitBuffer* pDestBuffer,
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            int xSrc,
            int ySrc,
            int transColor
            );

    // specific blit for a DDraw buffer
    virtual void BltDDraw(
            CGameDDrawBitBuffer* pDestBuffer,
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            int xSrc,
            int ySrc,
            DWORD rop
            );

    virtual void TransBltDDraw(
            CGameDDrawBitBuffer* pDestBuffer,
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            int xSrc,
            int ySrc,
            int transColor
            );

    // specific blit for a dibsection buffer
    virtual void BltDS(
            CGameDSBitBuffer* pDestBuffer,
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            int xSrc,
            int ySrc,
            DWORD rop
            );

    virtual void TransBltDS(
            CGameDSBitBuffer* pDestBuffer,
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            int xSrc,
            int ySrc,
            int transColor
            );

    virtual void SetPalette( HPALETTE );

    virtual HBITMAP GetHBitmap()
    {
        return mpDIB ? mpDIB->GetHBitmap() : NULL;
    }

    virtual LPBYTE GetBits()
    {
        return mpBits;
    }

    // report whether this object is a valid bitbuffer
    virtual BOOL IsValid()
    {
        return mIsValid;
    }

    // report this object's type
    virtual BB_TYPE TypeID()
    {
        return BB_DS;
    }

protected:
    friend class CGameDSScreen;
    friend class CGameDDrawBitBuffer;
    LPBITMAPINFO mpBitmapInfo;
    LPBYTE mpBits;
    CGameDIB* mpDIB;
    BOOL mIsValid;

    void SetBits( CGameDIB* pSource );
};

#endif // CGBITBUF_H
