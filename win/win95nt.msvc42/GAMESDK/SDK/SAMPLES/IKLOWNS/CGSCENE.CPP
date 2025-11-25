/*===========================================================================*\
|
|  File:        cgscene.cpp
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

#include <windows.h>
#include "cgdebug.h"
#include "cgimage.h"
#include "cgscene.h"

#define MAX_IMAGES 4

/*---------------------------------------------------------------------------*\
|
|       Class CGameScene
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/
CGameScene::CGameScene(
    )
{
}

CGameScene::~CGameScene()
{
}


/*---------------------------------------------------------------------------*\
|
|       Class CGameGruberScene
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/
CGameGruberScene::CGameGruberScene(
    char* pFileName,
    char* pSceneName
    ) : CGameScene(),
        mpImages( NULL )
{
    // get the image info
    char nameBuf[256];
    CGameGruberImage* tempImages[MAX_IMAGES];

    // load all images for this scene
    GetPrivateProfileString(
        pSceneName,
        NULL,       // grab all the keys
        "",     // no default
        nameBuf,
        sizeof( nameBuf ),
        pFileName
        );

    for (char *pImage = nameBuf; *pImage && (mNumImages < MAX_IMAGES); pImage++, mNumImages++)
    {
        tempImages[mNumImages] = new CGameGruberImage( pImage );
        pImage += lstrlen( pImage );    // move beyond terminator
    }

    // now that we know how many there are, allocate our array & copy the temp
    mpImages = new CGameGruberImage*[ mNumImages ];
    CopyMemory( mpImages, tempImages, mNumImages * sizeof( CGameGruberImage* ) );

    // !!! for now use 1st image for size
    mMaxX = mpImages[0]->GetWidth();
    mMaxY = mpImages[0]->GetHeight();
}

CGameGruberScene::~CGameGruberScene()
{
    if (mpImages)
    {
        for (; mNumImages>0; mNumImages--)
        {
            delete mpImages[mNumImages-1];
        }

        delete[] mpImages;
    }
}

BOOL
CGameGruberScene::ScrollTo(
    int x,
    int y
    )
{
    // !!! for now, assume 1 image
    return mpImages[0]->ScrollTo(x, y);
}

void
CGameGruberScene::Render(
    CGameGDIScreen* pScreen
    )
{
    // !!! for now, assume 1 image
    mpImages[0]->Render( pScreen );
}
