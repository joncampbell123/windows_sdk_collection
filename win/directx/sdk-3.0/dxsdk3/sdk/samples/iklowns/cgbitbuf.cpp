/*===========================================================================*\
|
|  File:        cgbitbuf.cpp
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


//#ifndef __WATCOMC__
#define WIN32_LEAN_AND_MEAN
//#endif
#include <windows.h>
#include "cgdebug.h"
#include "cgdib.h"
#include "cgbitbuf.h"
#include "cgimage.h"
#include "cgglobl.h"

/*---------------------------------------------------------------------------*\
|
|       Class CGameBitBuffer
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/

CGameBitBuffer::CGameBitBuffer(
    CGameDIB* pDIB,
    int width,
    int height,
    COLORREF transColor
    ) : mhPalette( NULL ),
        mTransColor( transColor )
{
}   

CGameBitBuffer::CGameBitBuffer(
    int width,
    int height,
    HPALETTE hPal,
    COLORREF transColor
    ) : mhPalette( NULL ),
        mTransColor( transColor )
{
}   

CGameBitBuffer::~CGameBitBuffer()
{
    if (mhPalette)
    {
        DeleteObject( mhPalette );
    }
}

/*---------------------------------------------------------------------------*\
|
|       Class CGameDDrawBitBuffer
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/
// static members
LPDIRECTDRAW CGameDDrawBitBuffer::mpDDrawDriver = NULL;
int CGameDDrawBitBuffer::mInstanceCount = 0;    // for allocating/freeing mpDDrawDriver

// construct a buffer from existing DIB, optionally stretching to fit new size
CGameDDrawBitBuffer::CGameDDrawBitBuffer(
        CGameDIB* pDIB,
        int width,      // 0 (default) means use DIB's width
        int height,     // 0 (default) means use DIB's height
        COLORREF transColor
        ) : CGameBitBuffer(pDIB, width, height, transColor),
            mpSurface( NULL ),
            mIsAttached( FALSE ),
            mIsValid( FALSE ),
            mpFileName( NULL )
{
    if (width == 0)
        width = pDIB->GetWidth();

    if (height == 0)
        height = pDIB->GetHeight();

    // we need to copy the dib's filename so we can reload later if necessary
    char* pFileName = (char*) pDIB->GetNamePtr();

    if (pFileName)
    {
        mpFileName = new char[lstrlen(pFileName)+1];
        lstrcpy( mpFileName, pFileName );
    }

    InitDDraw();
    CreateSurface( width, height );

    if (mIsValid)
    {
        SetBits( pDIB );    // copy DIB bits onto surface

        mhPalette = pDIB->CreatePalette();
    }
 }  

// construct an "empty" buffer with given size
CGameDDrawBitBuffer::CGameDDrawBitBuffer(
        int width,
        int height,
        HPALETTE hPal,
        COLORREF transColor
        ) : CGameBitBuffer(width, height, hPal, transColor),
            mpSurface( NULL ),
            mIsAttached( FALSE ),
            mIsValid( FALSE ),
            mpFileName( NULL )
{
    InitDDraw();
    CreateSurface( width, height );
}

void
CGameDDrawBitBuffer::InitDDraw(
    )
{
    HRESULT result;

    // do we need to get the Direct Draw driver?
    if (mpDDrawDriver == NULL)
    {
        result = DirectDrawCreate( NULL, &mpDDrawDriver, NULL );
    }

    ++mInstanceCount;   // keep count so we know when to release driver
}

void
CGameDDrawBitBuffer::CreateSurface(
    int width,
    int height
    )
{
    if (mpDDrawDriver)
    {
        DDSURFACEDESC surfDesc;

        memset( &surfDesc, 0, sizeof( surfDesc ) );

        surfDesc.dwSize = sizeof( surfDesc );
        surfDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
        surfDesc.dwFlags |= DDSD_HEIGHT | DDSD_WIDTH;
        surfDesc.dwHeight = height;
        surfDesc.dwWidth = width;

        surfDesc.dwFlags |= DDSD_CAPS;

        surfDesc.ddckCKSrcBlt.dwColorSpaceLowValue = 1;
        surfDesc.ddckCKSrcBlt.dwColorSpaceHighValue = 1;

        // ask driver for the surface
        HRESULT result;
        if ((result = mpDDrawDriver->CreateSurface(&surfDesc,
                &mpSurface, NULL )) == DD_OK)
        {
            // have to call SetColorKey
            result = mpSurface->SetColorKey( DDCKEY_SRCBLT, &surfDesc.ddckCKSrcBlt );

            mIsValid = TRUE;
        }
    }
}

void
CGameDDrawBitBuffer::SetBits(
    CGameDIB* pSource
    )
{
    HRESULT     ddrawrval;
    DDSURFACEDESC   dsd;
    LPBYTE      lpdest;
    DWORD       y;
    DWORD       bytes_scanline;
    LPBYTE      lpdib_bits;

    /*
     * get surface description
     */
    mSurfD.dwSize = sizeof( mSurfD );
    ddrawrval = mpSurface->GetSurfaceDesc( &mSurfD );
    if( ddrawrval != DD_OK )
    {
        DB_LOG( DB_PROBLEM, "Could not get surface description!" );
        return;
    }

    if( !(mSurfD.ddpfPixelFormat.dwFlags & DDPF_RGB) )
    {
        DB_LOG( DB_PROBLEM, "Can only copy to RGB surfaces for now" );
        return;
    }

    bytes_scanline = pSource->BytesPerScanline();

    /*
     * access the surface
     */
    dsd.dwSize = sizeof( dsd );
    do {
        ddrawrval = mpSurface->Lock( NULL, &dsd, DDLOCK_SURFACEMEMORYPTR, NULL );
        if( ddrawrval != DD_OK && ddrawrval != DDERR_WASSTILLDRAWING )
        {
            DB_LOG( DB_PROBLEM, "Lock failed in creating DDBitBuffer" );
            return;
        }
    } 
    while( ddrawrval == DDERR_WASSTILLDRAWING );

    lpdest = (LPBYTE) dsd.lpSurface;
    mPitch = mSurfD.lPitch;

    // NOTE: we only work with 8bpp; need different code for different bitdepths
    // this code just assumes 8 bits
    for( y=0;y<mSurfD.dwHeight;y++ )
    {
        lpdib_bits = pSource->GetPixelAddress(0,y);
        memcpy( lpdest, lpdib_bits, bytes_scanline );
        lpdest += (DWORD) mSurfD.lPitch;
    }

    /*
    * release the surface and go home...
    */
    mpSurface->Unlock( NULL );
}   

CGameDDrawBitBuffer::~CGameDDrawBitBuffer()
{
    if (mpSurface && !mIsAttached)
    {
        mpSurface->Release(  );
    }

    if (--mInstanceCount == 0)
    {
        if (mpDDrawDriver)
        {
            mpDDrawDriver->Release( );
            mpDDrawDriver = NULL;
        }
    }

    delete mpFileName;
}

// call this to re-create the surface if it was lost
// this can happen if we've switched modes & switch back
void
CGameDDrawBitBuffer::ReCreate(
    )
{
    int width = mSurfD.dwWidth;
    int height = mSurfD.dwHeight;

    HRESULT result = mpSurface->Restore();

    if (result == DD_OK)
        mIsValid = TRUE;

    if (mIsValid && mpFileName)
    {
        // reload the file
        CGameDIB dib( mpFileName );
        SetBits( &dib );    // copy DIB bits onto surface
    }
}


// generic blt call to this buffer from another DDraw surface
void
CGameDDrawBitBuffer::Blt(
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    CGameBitBuffer* pSrcBuffer,
    int xSrc,
    int ySrc,
    DWORD rop
    )
{
    HRESULT result;
    DDBLTFX dbf;
    BOOL useFX = FALSE;

    // negative width or height means mirror the blt
    RECT destRect = 
    {
        xDest,
        yDest,
        xDest+((wDest<0) ? -wDest : wDest),
        yDest+((hDest<0) ? -hDest : hDest)
    };

    RECT srcRect = 
    {
        xSrc,
        ySrc,
        xSrc+((wDest<0) ? -wDest : wDest),
        ySrc+((hDest<0) ? -hDest : hDest)
    };


    if ((wDest < 0) || (hDest < 0))
    {
        useFX = TRUE;
        memset( &dbf, 0, sizeof( dbf ) );

        dbf.dwSize = sizeof(dbf);
        dbf.dwDDFX = (wDest<0) ? DDBLTFX_MIRRORLEFTRIGHT : 0;
        dbf.dwDDFX |= (hDest<0) ? DDBLTFX_MIRRORUPDOWN : 0;
    }

    result = mpSurface->Blt( 
            &destRect,  // dest rect
            ((CGameDDrawBitBuffer*)pSrcBuffer)->mpSurface,      // src surf
            &srcRect,   // src rect
            useFX ? DDBLT_DDFX : 0,// flags
            useFX ? &dbf : NULL );  // bltfx

}   

// generic blt call -- determine type of destination buffer
void
CGameDDrawBitBuffer::Blt(
    CGameBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    DWORD rop
    )
{
    switch (pDestBuffer->TypeID())
    {
        case BB_DDraw:
            Blt(
                (CGameDDrawBitBuffer*)pDestBuffer,
                xDest,
                yDest,
                wDest,
                hDest,
                xSrc,
                ySrc,
                rop
                );
            break;
        case BB_DS:
            Blt(
                (CGameDSBitBuffer*)pDestBuffer,
                xDest,
                yDest,
                wDest,
                hDest,
                xSrc,
                ySrc,
                rop
                );
            break;
        default:
            DB_BREAK();
            break;
    }
}

// generic transblt call -- determine type of source buffer
void
CGameDDrawBitBuffer::TransBlt(
    CGameBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    int transColor
    )
{
    switch (pDestBuffer->TypeID())
    {
        case BB_DDraw:
            TransBlt(
                (CGameDDrawBitBuffer*)pDestBuffer,
                xDest,
                yDest,
                wDest,
                hDest,
                xSrc,
                ySrc,
                transColor
                );
            break;
        case BB_DS:
            TransBlt(
                (CGameDSBitBuffer*)pDestBuffer,
                xDest,
                yDest,
                wDest,
                hDest,
                xSrc,
                ySrc,
                transColor
                );
            break;
        default:
            DB_BREAK();
            break;
    }
}

// specific blt call for another DDraw surface
void
CGameDDrawBitBuffer::BltDDraw(
    CGameDDrawBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    DWORD rop
    )
{
    HRESULT result;
    DDBLTFX dbf;
    BOOL useFX = FALSE;

    // negative width or height means mirror the blt
    RECT destRect =
    {
        xDest,
        yDest,
        xDest+((wDest<0) ? -wDest : wDest),
        yDest+((hDest<0) ? -hDest : hDest)
    };

    RECT srcRect =
    {
        xSrc,
        ySrc,
        xSrc+((wDest<0) ? -wDest : wDest),
        ySrc+((hDest<0) ? -hDest : hDest)
    };

    if ((wDest < 0) || (hDest < 0))
    {
        useFX = TRUE;
        memset( &dbf, 0, sizeof( dbf ) );

        dbf.dwSize = sizeof(dbf);
        dbf.dwDDFX = (wDest<0) ? DDBLTFX_MIRRORLEFTRIGHT : 0;
        dbf.dwDDFX |= (hDest<0) ? DDBLTFX_MIRRORUPDOWN : 0;
    }

    // force a stop if we never succeed
    int stopCount = DDRAW_RETRY;

    do
    {
        result = pDestBuffer->mpSurface->BltFast( 
                destRect.left,
                destRect.top,
                mpSurface,      // src surf
                &srcRect,   // src rect
                FALSE           // transparent
                );

        // surface may have been lost due to mode switch
        if (result == DDERR_SURFACELOST)
        {
            mIsValid = FALSE;
            // re-create our surface
            ReCreate();
        }
    }
    while( (result != DD_OK) && (--stopCount > 0));
}   

// specific transblt call for another DDraw buffer
void
CGameDDrawBitBuffer::TransBltDDraw(
    CGameDDrawBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    int transColor
    )
{
    HRESULT result;

    RECT destRect = 
    {
        xDest, yDest, xDest+wDest, yDest+hDest
    };

    RECT srcRect =
    {
        xSrc, ySrc, xSrc+wDest, ySrc+hDest
    };

    // force a stop if we never succeed
    int stopCount = DDRAW_RETRY;

    do
    {
        result = pDestBuffer->mpSurface->BltFast( 
                destRect.left,
                destRect.top,
                mpSurface,      // src surf
                &srcRect,   // src rect
                TRUE            // transparent
                );

        // surface may have been lost due to mode switch
        if (result == DDERR_SURFACELOST)
        {
            mIsValid = FALSE;
            // re-create our surface
            ReCreate();
        }
    }
    while( (result != DD_OK) && (--stopCount > 0));
}

// specific blt call for a dibsection destination buffer
void
CGameDDrawBitBuffer::BltDS(
    CGameDSBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    DWORD rop
    )
{
    LPBYTE pSrc = GetLockedAddress( xSrc, ySrc );

    if (pSrc)
    {
        int scanDest = -(pDestBuffer->mpDIB->BytesPerScanline());
        int scanSrc = mPitch;

        CopyDIBBits(
            pDestBuffer->mpDIB->GetPixelAddress( xDest, yDest ),
            pSrc,
            wDest,  // width pixels
            hDest,
            (DWORD) scanDest,
            (DWORD) scanSrc
            );

    }
    Unlock();
}

// specific transblt call for a ds buffer
void
CGameDDrawBitBuffer::TransBltDS(
    CGameDSBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    int transColor
    )
{
    LPBYTE pSrc = GetLockedAddress( xSrc, ySrc );

    if (pSrc)
    {
        int scanDest = -(pDestBuffer->mpDIB->BytesPerScanline());
        int scanSrc = mPitch;

        TransCopyDIBBits(
            pDestBuffer->mpDIB->GetPixelAddress( xDest, yDest ),
            pSrc,
            wDest,  // width pixels
            hDest,
            (DWORD) scanDest,
            (DWORD) scanSrc,
            transColor
            );

    }
    Unlock();
}

void
CGameDDrawBitBuffer::SetPalette( HPALETTE hPal )
{
}

LPBYTE  // return address to given pixel in locked surface
CGameDDrawBitBuffer::GetLockedAddress(
    int x,
    int y
    )
{
    HRESULT     ddrawrval;
    DDSURFACEDESC   dsd;
    /*
    * access the surface
    */
    dsd.dwSize = sizeof( dsd );

    int stopCount = DDRAW_RETRY;

    do
    {
        ddrawrval = mpSurface->Lock( NULL, &dsd, DDLOCK_SURFACEMEMORYPTR, NULL );

        // surface may have been lost due to mode switch
        if (ddrawrval == DDERR_SURFACELOST)
        {
            mIsValid = FALSE;
            // re-create our surface
            ReCreate();
        }
    }
    while( (ddrawrval != DD_OK) && (--stopCount > 0) );

    if (ddrawrval == DD_OK)
    {
        mPitch = dsd.lPitch;

        return (LPBYTE) dsd.lpSurface + x + (y * mPitch);
    }
    else
    {
        DB_BREAK();
        return NULL;
    }
}

void
CGameDDrawBitBuffer::Unlock()
{
    mpSurface->Unlock( NULL );
}

/*---------------------------------------------------------------------------*\
|
|       Class CGameDDrawScreenBuffer
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/
// construct the screen's primary surface
CGameDDrawScreenBuffer::CGameDDrawScreenBuffer(
        ) : CGameDDrawBitBuffer(),
        mpPalette( NULL ),
        mpOldPalette( NULL )
{
    InitDDraw();

    // call our create surface to create the primary surface
    CreateSurface( 0,0 );
}   

// create an attached-buffer object from given primary surface
CGameDDrawScreenBuffer::CGameDDrawScreenBuffer(
    CGameDDrawScreenBuffer* pFront
    ) : CGameDDrawBitBuffer(),
        mpPalette( NULL ),
        mpOldPalette( NULL )
{
    HRESULT result;
    DDSCAPS caps;

    memset(&caps, 0, sizeof(caps));
    caps.dwCaps = DDSCAPS_BACKBUFFER;

    result = pFront->mpSurface->GetAttachedSurface(
                &caps,
                &mpSurface);

    mSurfD.dwSize = sizeof( mSurfD );
    pFront->mpSurface->GetSurfaceDesc(
                &mSurfD
                );

    mPitch = mSurfD.lPitch;

    if (mpSurface)
        mIsValid = TRUE;

    // flag this as an attached surface so we won't release it
    mIsAttached = TRUE;
    ++mInstanceCount;   // keep count so we know when to release driver
}

void
CGameDDrawScreenBuffer::CreateSurface(
    int width,
    int height
    )
{
    HRESULT result;

        result = mpDDrawDriver->SetCooperativeLevel( ghMainWnd,
                                      DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
    if( result != DD_OK )
    {
        return;
    }

    // first, force display into our 640x480, 8bpp mode
    result = mpDDrawDriver->SetDisplayMode(
                        SCREEN_WIDTH,
                        SCREEN_HEIGHT,
                        8
                        );
    if( result != DD_OK )
    {
        return;
    }

    memset( &mSurfD, 0, sizeof( mSurfD ) );

    mSurfD.dwSize = sizeof( mSurfD );
    mSurfD.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;

    mSurfD.dwFlags |= DDSD_BACKBUFFERCOUNT | DDSD_CAPS;
    if( gDoubleBuffer )
    {
        mSurfD.dwBackBufferCount = 1;
    }
    else
    {
        mSurfD.dwBackBufferCount = 2;
    }

    // ask DirectDraw for the surface
    if ((result = mpDDrawDriver->CreateSurface(&mSurfD, &mpSurface,
                NULL)) == DD_OK)
    {
        mPitch = mSurfD.lPitch;
        mIsValid = TRUE;
    }
}

CGameDDrawScreenBuffer::~CGameDDrawScreenBuffer()
{
    // free up palette
    DeleteDDPalette();
}   

void
CGameDDrawScreenBuffer::SetMode(
    int width,
    int height,
    int bits
    )
{
    mpDDrawDriver->SetDisplayMode(
                        width,
                        height,
                        bits
                        );
}

void
CGameDDrawScreenBuffer::RestoreMode()
{
    mpDDrawDriver->FlipToGDISurface();
    mpDDrawDriver->RestoreDisplayMode();
}

void
CGameDDrawScreenBuffer::ShowGDIPage()
{
    mpDDrawDriver->FlipToGDISurface();
}

void
CGameDDrawScreenBuffer::SetPalette(
    LPPALETTEENTRY pPal
    )
{
    // grab current palette if we don't already have it
    if (mpOldPalette == NULL)
    {
        mpSurface->GetPalette( &mpOldPalette );
    }

    // free up current palette
    if (mpPalette)
    {
        DeleteDDPalette();
    }

    if ((mpDDrawDriver->CreatePalette(
                DDPCAPS_8BIT,
                pPal,
                &mpPalette,
                NULL
                ) == DD_OK) && mpSurface)
    {
        mpSurface->SetPalette( mpPalette );
    }
    
}

void
CGameDDrawScreenBuffer::DeleteDDPalette()
{
    if (mpPalette)
    {
        // reset to old palette if we have it
        if (mpOldPalette && mpSurface)
        {
            mpSurface->SetPalette( mpOldPalette );
        }

        if (mpPalette->Release() == DD_OK)
            mpPalette = NULL;
    }
}

int
CGameDDrawScreenBuffer::GetVideoMemory()
{
    DDCAPS caps;

    caps.dwSize = sizeof(DDCAPS);
    if ((mpDDrawDriver != NULL) && (mpDDrawDriver->GetCaps(&caps,NULL) == DD_OK))
    {
        return caps.dwVidMemTotal;
    }

    return 0;
}

/*---------------------------------------------------------------------------*\
|
|       Class CGameDSBitBuffer
|
|  DESCRIPTION:
|       Use Win32's CreateDIBSection for a non-Direct Draw bitbuffer
|
|
\*---------------------------------------------------------------------------*/
// construct a buffer from existing DIB, optionally stretching to fit new size
CGameDSBitBuffer::CGameDSBitBuffer(
        CGameDIB* pDIB,
        int width,      // 0 (default) means use DIB's width
        int height,     // 0 (default) means use DIB's height
        COLORREF transColor
        ) : CGameBitBuffer(pDIB, width, height, transColor),
            mpBitmapInfo( NULL ),
            mpDIB( NULL ),
            mpBits( NULL ),
            mIsValid( FALSE )
{
    if (width == 0)
        width = pDIB->GetWidth();

    if (height == 0)
        height = pDIB->GetHeight();

    mhPalette = pDIB->CreatePalette();

    // create our empty DIBSection
    mpDIB = new CGameDIB( width, height, mhPalette );
    SetBits( pDIB );    // copy DIB bits onto surface

    // cache important ptrs for blting
    mpBits = mpDIB->GetBits();
    mpBitmapInfo = mpDIB->GetBitmapInfo();
    mIsValid = TRUE;
}   

// construct an "empty" buffer with given size
CGameDSBitBuffer::CGameDSBitBuffer(
        int width,
        int height,
        HPALETTE hPal,
        COLORREF transColor
        ) : CGameBitBuffer(width, height, hPal, transColor),
            mpBitmapInfo( NULL ),
            mpDIB( NULL ),
            mpBits( NULL ),
            mIsValid( FALSE )
{
    mpDIB = new CGameDIB( width, height, hPal );

    // cache important ptrs for blting
    mpBits = mpDIB->GetBits();
    mpBitmapInfo = mpDIB->GetBitmapInfo();
    mIsValid = TRUE;
}

void
CGameDSBitBuffer::SetBits(
    CGameDIB* pSource
    )
{
    LPBYTE      lpdest;
    DWORD       bytes_scanline;
    LPBYTE      lpdib_bits;

    bytes_scanline = pSource->BytesPerScanline();

    for( int y=0;y<pSource->GetHeight();y++ )
    {
        lpdib_bits = pSource->GetPixelAddress(0,y);
        lpdest = mpDIB->GetPixelAddress(0,y);
        CopyMemory( lpdest, lpdib_bits, bytes_scanline );
    }
}   

CGameDSBitBuffer::~CGameDSBitBuffer()
{
    delete mpDIB;
}   

// specific blt call to this buffer from another DS buffer
void CGameDSBitBuffer::Blt(
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    CGameBitBuffer* pSrcBuffer,
    int xSrc,
    int ySrc,
    DWORD rop
    )
{
    int scanDest = -(mpDIB->BytesPerScanline());
    int scanSrc = -(((CGameDSBitBuffer*)pSrcBuffer)->mpDIB->BytesPerScanline());

    CopyDIBBits(
        mpDIB->GetPixelAddress( xDest, yDest ),
        ((CGameDSBitBuffer*)pSrcBuffer)->mpDIB->GetPixelAddress( xSrc, ySrc ),
        wDest,  // width pixels
        hDest,
        (DWORD) scanDest,
        (DWORD) scanSrc
        );
}   

// generic dest blt call -- determine type of source buffer
void
CGameDSBitBuffer::Blt(
    CGameBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    DWORD rop
    )
{
    switch (pDestBuffer->TypeID())
    {
        case BB_DDraw:
            Blt(
                (CGameDDrawBitBuffer*)pDestBuffer,
                xDest,
                yDest,
                wDest,
                hDest,
                xSrc,
                ySrc,
                rop
                );
            break;
        case BB_DS:
            Blt(
                (CGameDSBitBuffer*)pDestBuffer,
                xDest,
                yDest,
                wDest,
                hDest,
                xSrc,
                ySrc,
                rop
                );
            break;
        default:
            DB_BREAK();
            break;
    }
}

// generic transblt call -- determine type of dest buffer
void
CGameDSBitBuffer::TransBlt(
    CGameBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    int transColor
    )
{
    switch (pDestBuffer->TypeID())
    {
        case BB_DDraw:
            TransBlt(
                (CGameDDrawBitBuffer*)pDestBuffer,
                xDest,
                yDest,
                wDest,
                hDest,
                xSrc,
                ySrc,
                transColor
                );
            break;
        case BB_DS:
            TransBlt(
                (CGameDSBitBuffer*)pDestBuffer,
                xDest,
                yDest,
                wDest,
                hDest,
                xSrc,
                ySrc,
                transColor
                );
            break;
        default:
            DB_BREAK();
            break;
    }
}

// specific blt call for a DS buffer
void CGameDSBitBuffer::BltDS(
    CGameDSBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    DWORD rop
    )
{
    int scanDest = -(pDestBuffer->mpDIB->BytesPerScanline());
    int scanSrc = -(mpDIB->BytesPerScanline());

    CopyDIBBits(
        pDestBuffer->mpDIB->GetPixelAddress( xDest, yDest ),
        mpDIB->GetPixelAddress( xSrc, ySrc ),
        wDest,  // width pixels
        hDest,
        (DWORD) scanDest,
        (DWORD) scanSrc
        );
}   

// specific transblt call for a DDraw buffer
void
CGameDSBitBuffer::TransBltDS(
    CGameDSBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    int transColor
    )
{
    int scanDest = -(pDestBuffer->mpDIB->BytesPerScanline());
    int scanSrc = -(mpDIB->BytesPerScanline());

    TransCopyDIBBits(
        pDestBuffer->mpDIB->GetPixelAddress( xDest, yDest ),
        mpDIB->GetPixelAddress( xSrc, ySrc ),
        wDest,  // width pixels
        hDest,
        (DWORD) scanDest,
        (DWORD) scanSrc,
        transColor
        );
}

// specific blt call for a DDraw buffer
void CGameDSBitBuffer::BltDDraw(
    CGameDDrawBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    DWORD rop
    )
{
    LPBYTE lpdest;

    lpdest = pDestBuffer->GetLockedAddress( xDest, yDest );

    if (lpdest)
    {
        int scanDest = pDestBuffer->mPitch;
        int scanSrc = -(mpDIB->BytesPerScanline());

        CopyDIBBits(
            lpdest,
            mpDIB->GetPixelAddress( xSrc, ySrc ),
            wDest,  // width pixels
            hDest,
            (DWORD) scanDest,
            (DWORD) scanSrc
            );

    }
    pDestBuffer->Unlock();
}

// specific transblt call for a DDraw buffer
void
CGameDSBitBuffer::TransBltDDraw(
    CGameDDrawBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    int transColor
    )
{
    LPBYTE lpdest = pDestBuffer->GetLockedAddress( xDest, yDest );

    if (lpdest)
    {
        int scanDest = pDestBuffer->mPitch;
        int scanSrc = -(mpDIB->BytesPerScanline());

        TransCopyDIBBits(
            lpdest,
            mpDIB->GetPixelAddress( xSrc, ySrc ),
            wDest,  // width pixels
            hDest,
            (DWORD) scanDest,
            (DWORD) scanSrc,
            transColor
            );

    }
    pDestBuffer->Unlock();
}

void
CGameDSBitBuffer::SetPalette( HPALETTE hPal )
{
}

#if !(defined(__BORLANDC__) || defined(__WATCOMC__))
void
TransCopyDIBBits(
        LPBYTE pDest,       // destination
        LPBYTE pSource,     //         ; source pointer
        DWORD dwWidth,      //         ; width pixels
        DWORD dwHeight,     //        ; height pixels
        DWORD dwScanD,      //         ; width bytes dest
        DWORD dwScanS,      //         ; width bytes source
        BYTE bTranClr       //        ; transparent color
    )
{
    _asm
    {
        push ds
        push esi
        push edi

        mov ecx, dwWidth
        or ecx,ecx
        jz tcdb_nomore     ; test for silly case

        mov edx, dwHeight       ; EDX is line counter
        mov bl, bTranClr        ; BL has transparency color

        mov esi, pSource         ; DS:[ESI] point to source

        mov edi, pDest

        or  edi, edi        ; check NULL ptrs
        jz  tcdb_nomore

        or  esi, esi        ; check NULL ptrs
        jz  tcdb_nomore

        sub dwScanD,ecx         ; bias these
        sub dwScanS,ecx

        push ecx

        align 4

tcdb_morelines:
        pop ecx
        push ecx

        shr ecx,2
        jz  short tcdb_nextscan

// ;
// ; The idea here is to not branch very often so we unroll the loop by four
// ; and try to not branch when a whole run of pixels is either transparent
// ; or not transparent.
// ;
// ; There are two loops. One loop is for a run of pixels equal to the
// ; transparent color, the other is for runs of pixels we need to store.
// ;
// ; When we detect a "bad" pixel we jump to the same position in the
// ; other loop.
// ;
// ; Here is the loop we will stay in as long as we encounter a "transparent"
// ; pixel in the source.
// ;

        align 4

tcdb_same:
        mov eax, ds:[esi]
        cmp al, bl
        jne short tcdb_diff0

tcdb_same0:
        cmp ah, bl
        jne short tcdb_diff1

tcdb_same1:
        shr eax, 16
        cmp al, bl
        jne short tcdb_diff2

tcdb_same2:
        cmp ah, bl
        jne short tcdb_diff3

tcdb_same3:
        add edi,4
        add esi,4
        dec ecx
        jnz short tcdb_same
        jz  short tcdb_nextscan

// ;
// ; Here is the loop we will stay in as long as
// ; we encounter a "non transparent" pixel in the source.
// ;

        align 4

tcdb_diff:
        mov eax, ds:[esi]
        cmp al, bl
        je short tcdb_same0

tcdb_diff0:
        mov es:[edi],al
        cmp ah, bl
        je short tcdb_same1

tcdb_diff1:
        mov es:[edi+1],ah

        shr eax, 16
        cmp al, bl
        je short tcdb_same2

tcdb_diff2:
        mov es:[edi+2],al
        cmp ah, bl
        je short tcdb_same3

tcdb_diff3:
        mov es:[edi+3],ah

        add edi,4
        add esi,4
        dec ecx
        jnz short tcdb_diff
        jz  short tcdb_nextscan

// ;
// ; We are at the end of a scan, check for odd leftover pixels to do
// ; and go to the next scan.
// ;

        align 4

tcdb_nextscan:
        pop ecx
        push ecx

        and ecx,11b
        jnz short tcdb_oddstuff
        // ; move on to the start of the next line

tcdb_nextscan1:
        add esi, dwScanS
        add edi, dwScanD

        dec edx                 // ; line counter
        jnz short tcdb_morelines
        jz  short tcdb_nomore

// ;
// ; If the width is not a multiple of 4 we will come here to clean up
// ; the last few pixels
// ;

tcdb_oddstuff:
        inc ecx
tcdb_oddloop:
        dec ecx
        jz  short tcdb_nextscan1
        mov al, ds:[esi]
        inc esi
        inc edi
        cmp al, bl
        je  short tcdb_oddloop
        mov es:[edi-1],al
        jmp short tcdb_oddloop

tcdb_nomore:
        pop ecx

        pop edi
        pop esi
        pop ds
    }
}

void
CopyDIBBits(
        LPBYTE pDest,   //           ; dest pointer
        LPBYTE pSource, //         ; source pointer
        DWORD dwWidth,  //         ; width pixels
        DWORD dwHeight, //        ; height pixels
        DWORD dwScanD,  //         ; width bytes dest
        DWORD dwScanS   //         ; width bytes source
    )
{
    _asm
    {
        push    esi
        push    edi

        ; load scanline width into EBX, image height into EDX
        mov ebx, [dwWidth]
                or      ebx, ebx
                jz      silly_case          ; width is 0
        mov edx, [dwHeight]
                or      edx, edx
        jz  silly_case      ; height is 0

        ; bias offsets
        sub [dwScanD], ebx
        sub [dwScanS], ebx

        ; load ESI
        mov esi, [pSource]

        ; load EDI and guarantee it is aligned on a DWORD boundary
        mov edi, [pDest]

        or  edi, edi        ; check NULL ptrs
        jz  silly_case

        or  esi, esi        ; check NULL ptrs
        jz  silly_case

    ;; NOTE: this assumes that the scanwidth will always be a multiple
    ;;  of DWORDs, so that the misalignment will be the same for all
    ;;  scanlines.  This allows us to calculate it once outside the loop

        mov ecx, edi
        and ecx, 11b                ; deal with non-aligned case

        mov eax, 100b
        sub eax, ecx            ; eax now contains aligment offset

        cmp eax, ebx            ; do we really have that many to do?
        jb next_scanline        ; if not, we're all set to blast

        mov eax, ebx            ; put actual width into offset


        ALIGN 4
next_scanline:

        ; adjust_alignment
        mov ecx, eax            ; eax == alignment offset
        rep movsb               ; move bytewise to make EDI aligned properly

        mov ecx, ebx                ; total count per scanline
        sub ecx, eax                ; residue left over: misaligned amount!

        ; at this point, ECX is number of BYTES to transfer, ESI and EDI point to source and dest
        push eax

        mov eax, ecx
        shr ecx, 2              ; div 4 to get number of DWORDS to transfer
        rep movsd               ; move by DWORDS as much as possible

        and eax, 11b                ; number of *bytes* to transfer (if any)
        mov ecx, eax
        rep movsb               ; move remaining bytes (may be zero)

        pop eax

        ; now adjust ESI and EDI for next scanline and jump to it!
        add     esi, [dwScanS]
        add     edi, [dwScanD]
        dec edx                 ; if more scanlines, process them
        jnz next_scanline

silly_case:
        pop edi
        pop esi

    }

}
#endif
