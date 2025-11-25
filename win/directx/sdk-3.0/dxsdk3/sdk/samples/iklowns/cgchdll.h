/*===========================================================================*\
|
|  File:        cgchdll.h
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

#ifndef CGCHDLL_H
#define CGCHDLL_H
// version number in CGameVersionIdent structure returned by Ident()
#define RELEASE1_0  0x00010000
#define GAMEID      0x6b455749  // 'Game'

struct CGameVersionIdent {
    long    version;
    long    id; // must be == GAMEID
};

// ordinal numbers in DLL:
#define EXPORTED_IDENT  1   
#define EXPORTED_INFO   2

#define ACTION_COMPLETED    0
#define ACTION_KILLME       -1

class CGameCharacter;
class CGameLevel;
typedef int (*pActionFunction)(CGameCharacter *mythis, CGameLevel *mylevel);
typedef int (*pCollideFunction)(CGameCharacter *mythis, CGameCharacter * otherthis, CGameLevel *mylevel);

struct CGameCharSoundInfo {
    char *  Sound;
    char *  SoundEnd;
    int Rate;
    int Loop;
};

struct CGameCharSequenceInfo {
    char    * SequenceFile;
    char    * SequenceName;
    int     Rate;
    CGameCharSoundInfo Sound;
};

struct CGameCharInfo {
    char    *name ;
    int     NumSequences;
    CGameCharSequenceInfo * Sequences;

    pActionFunction Create;
    pActionFunction Action;
    pActionFunction Destroy;
    pActionFunction RemoteAction;
    pCollideFunction Collide;
};

struct CGameInfo {
    int numcharacters;
    CGameCharInfo **characters;
};

// functions exported by each character DLL
// exported as EXPORTED_IDENT:
#if defined(__BORLANDC__) || defined(__WATCOMC__)
extern "C" void CALLBACK Ident (CGameVersionIdent *);

// exported as EXPORTED_INFO:
extern "C" void CALLBACK Info (CGameInfo *);
#else
void CALLBACK Ident (CGameVersionIdent *);

// exported as EXPORTED_INFO:
void CALLBACK Info (CGameInfo *);
#endif
#endif
