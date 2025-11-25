/*===========================================================================*\
|
|  File:        cgload.h
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

#ifndef _CGLOAD_H
#define _CGLOAD_H

#include "cgsound.h"
#include "cgtext.h"

class CGameScreen;
class CGameDSBitBuffer;

typedef struct _TXTCOLOR {
    COLORREF    main;
    COLORREF    shadow;
} TXTCOLOR;

class CLoadingScreen {
private:
    POINT       Origin;
    int     curTotal;
//  HDC     hdcScreen;
    HDC     hdcLoading;
    CGameText   *pText;
    CSoundEffect    *pSoundStart;
    CSoundEffect    *pSoundUpdate;
    CSoundEffect    *pSoundEnd;
    CGameScreen *mpScreen;
    CGameDSBitBuffer    *mpLoadBuffer;
    int     mWidth;
    int     mHeight;

public:

    CLoadingScreen(
        CGameScreen* pScreen,
        LPSTR pBitmapName, 
        int     StringId, 
        POINT   pt,
        TXTCOLOR color,
        RECT    rect,
        CSoundEffect    *pSoundStart=NULL,
        CSoundEffect    *pSoundUpdate=NULL,
        CSoundEffect    *pSoundEnd=NULL,
        LPSTR       MidiFile=NULL
        ); 
    ~CLoadingScreen();
    void Update(int Increment=5);
    void Paint();
};

#endif
