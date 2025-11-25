/*===========================================================================*\
|
|  File:        cgimage.h
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

#ifndef CGIMAGE_H
#define CGIMAGE_H

#include "cggraph.h"

#define TRANSPARENT_TILE ((WORD)-1)
#define MARK_TRANSPARENT(tile) ((WORD) tile | 0x8000)

// we use palette index 1 for transparency
#define TRANSPARENCY_INDEX ((BYTE)1)

#define HAS_TRANSPARENCY( num ) ((WORD) num & 0x8000)
#define IS_TRANSPARENT( num ) (num == TRANSPARENT_TILE)

#define TILE_INDEX( num ) ((WORD)IS_TRANSPARENT(num)?0:num & ~0x8000)

// maximum width we can allow for our tile source bitmaps
#define MAX_TILE_SRC_WIDTH 640
#define MAX_TILE_COLUMNS 20

// convert a tile number into x or y from source bitmap
#define TILE_SRC_X( num ) ((WORD) (TILE_INDEX(num) % MAX_TILE_COLUMNS) * TILE_SIZE)
#define TILE_SRC_Y( num ) ((WORD) (TILE_INDEX(num) / MAX_TILE_COLUMNS) * TILE_SIZE)

//#define TILE_MAP_SIGNATURE 'TLMP'
#define TILE_MAP_SIGNATURE 0x544c4d50
#define TILE_MAP_VERSION 0x0012

#define TILE_SIZE 32

#define TILE_TO_TX(tile) (tile * TILE_SIZE)
#define TILE_TO_TY(tile) (0)

#define WX_TO_INDEX(wx) (wx % TILE_SIZE)
#define WY_TO_INDEX(wy) (wy / TILE_SIZE)

struct TileMapStamp
{
    DWORD signature;
    DWORD version;
    WORD columns;
    WORD rows;
    WORD tileSize;
    WORD nameLength;    // length of following filename including terminator

    // this is followed by the DIB name, then the WORD array[col][row] of tile numbers
};

#define CG_DEFAULT_TILE_SIZE 16

// maximum number of tiles in an image
#define CG_TILE_LIMIT ((WORD)0xffff)

typedef WORD CG_TILE_INDEX;

class CGameImage : public CGameGraphic
{
public:
    CGameImage(char* pFileName, int curz) : CGameGraphic(curz){};       // load existing image file
    virtual ~CGameImage(){};

    virtual HPALETTE GetHPalette() = 0;
protected:
};

class CGameSkyImage : public CGameImage
{
public:
    CGameSkyImage(char* pFileName);     // load existing image file
    virtual ~CGameSkyImage();

    virtual HPALETTE GetHPalette()
    {
        return NULL;
    }

    virtual void Update(CGameLevel* pLevel, CGameUpdateList* pUpdate);
    virtual void Render(CGameLevel* pLevel, CGameScreen* pScreen, CGameUpdateList* pUpdate);

protected:
};

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_COLS (SCREEN_WIDTH / TILE_SIZE)
#define SCREEN_ROWS (SCREEN_HEIGHT / TILE_SIZE)

#define BUFFER_COLS (SCREEN_COLS+2)
#define BUFFER_ROWS (SCREEN_ROWS+2)

#define BUFFER_WIDTH (BUFFER_COLS * TILE_SIZE)
#define BUFFER_HEIGHT (BUFFER_ROWS * TILE_SIZE)

#define EXTRA_WIDTH (TILE_SIZE * 2)
#define EXTRA_HEIGHT (TILE_SIZE * 2)

// the "read-only" tiled image (for use in games)
class CGameTiledImage : public CGameImage
{
public:
    CGameTiledImage(char* pFileName, int curz,
        int curx=0, int cury=0  );      // load existing image file

    virtual ~CGameTiledImage();

    virtual void Update(CGameLevel* pLevel, CGameUpdateList* pUpdate);
    virtual void Render(CGameLevel* pLevel, CGameScreen* pScreen, CGameUpdateList* pUpdate);

    virtual HPALETTE GetHPalette()
    {
        return mpDIBBuffer ? mpDIBBuffer->GetHPalette() : NULL;
    }

protected:
    int mNumCols;

    CG_TILE_INDEX** mTileArray;     // array[row][column] of tile #s describing image

    CGameBitBuffer* mpDIBBuffer;    // holds the image's DIB tiles
};

#endif // CGIMAGE_H
