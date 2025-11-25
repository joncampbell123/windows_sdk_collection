/*===========================================================================*\
|
|  File:        cgdib.h
|
|  Description: 
|       contains class definitions for handling DIBs
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995-1996 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/

#ifndef CGDIB_H
#define CGDIB_H

#include <windows.h>

// CGameDIB is class for handling DIBs in Manhattan games
class CGameDIB
{
public:
    CGameDIB();         // create empty, invalid object (used by derived classes)
    CGameDIB( char* pFileName );    // constructor - create image from file

    // construct a new DIB section in memory of given size, palette
    CGameDIB( int width, int height, HPALETTE hPal = NULL );

    virtual ~CGameDIB();                      // destructor

    virtual LPBITMAPINFO GetBitmapInfo(){return mpBitmapInfo;}
    virtual LPBYTE GetBits(){return mpBits;}

    virtual HPALETTE CreatePalette();
    virtual HBITMAP GetHBitmap(){return mhBitmap;}

    virtual LPBYTE GetPixelAddress(int x, int y);
    virtual COLORREF GetPixelColor(int x, int y);

    virtual UINT GetColorTable(UINT, UINT, RGBQUAD*);

    virtual int GetWidth();
    virtual int GetHeight();

    virtual int BytesPerScanline()
    {
        return (GetWidth() + 3) & ~3;
    }

    virtual const char* GetNamePtr()
    {
        return (const char*) mpFileName;
    }

protected:

    BOOL mImageValid;       // flag whether image is valid

    LPBITMAPINFO mpBitmapInfo;
    LPBYTE mpBits;
    HBITMAP mhBitmap;       // handle to our DIB section
    char* mpFileName;

    void OpenDIB(char* pFileName);
};

#endif // CGDIB_H
