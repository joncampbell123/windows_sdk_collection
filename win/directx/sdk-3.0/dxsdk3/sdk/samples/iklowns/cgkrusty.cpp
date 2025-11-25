/*===========================================================================*\
|
|  File:        cgkrusty.cpp
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __WATCOMC__
#include <mem.h>
#else
#include <memory.h>
#endif
#include "cgchdll.h"
#include "cgchar.h"
#include "cgtimer.h"
#include "cginput.h"

char inifile [260];
int gFastKlown;
int gFastVel;
int gDebugOut;
int gPieSpeed;
int gPieRange;
int gKlownHits;
int gAgression;
int gMobility;
int SENSITIVITY  = 33;
int RUN_THRESHOLD  = 66;

static DWORD gTimeout=30000;
static DWORD gUpdateInterval=1000;

#define MIN_MOUSE_MOVE  3
#define BIG_MOUSE_MOVE  30
#define CenterX 320
#define CenterY 240

#define BREAK_IF(x) if (x) _asm int 3;

#define WALK_VELOCITY 60
#define RUN_VELOCITY 180

#define rand timeGetTime
// klown states:
typedef enum eKLOWN_STATES 
{
  STAND_RIGHT, STAND_LEFT,      //0,1
  WALK_RIGHT, WALK_LEFT,        //2
  RUN_RIGHT, RUN_LEFT,          //4
  R2L_TURN, L2R_TURN,           //6
  THROW_RIGHT, THROW_LEFT,         //8
  THROW_RIGHT_WALK, THROW_LEFT_WALK,  //10  
  THROW_RIGHT_RUN, THROW_LEFT_RUN,    //12
  IN_RIGHT, IN_LEFT,                  //14
  OUT_RIGHT, OUT_LEFT,                //16
  DUCK_RIGHT, DUCK_LEFT,              //18
  BLOCK_RIGHT, BLOCK_LEFT,            //20
  POKE_RIGHT, POKE_LEFT,              //22
  HIT_FACE_L, HIT_FACE_R,             //24
  HIT_BACK_L, HIT_BACK_R,             //26
  POKED_L, POKED_R,                   //28
  IS_DEAD,                            //30
  GAME_OVER,                          //31
  // insert new states here, before next NUM_STATES:

  NUM_STATES,

// INTERNAL USE ONLY: don't use these for things which will have sequences, 
// and DONT!!! return them  to the level object:
  INT_THROW_RIGHT,
  INT_THROW_LEFT,
  INT_THROW_RIGHT_WALK,
  INT_THROW_LEFT_WALK,
  INT_THROW_RIGHT_RUN,
  INT_THROW_LEFT_RUN,
  INT_COLLIDED_ON_LEFT,
  INT_COLLIDED_ON_RIGHT,
  
  INT_NUM_STATES
}
#ifdef __WATCOMC__
xx1
#endif
;

typedef enum eKLOWN_SEQ
{
    EOS, NOTHING, DUCK, THROW, BLOCK, POKE, GO_IN, GO_OUT, 
    WALK_L, WALK_R, RUN_L, RUN_R, GOT_HIT_L, GOT_HIT_R,
    GOT_POKED_L, GOT_POKED_R
}
#ifdef __WATCOMC__
xx2
#endif
;

typedef struct
{
    int start;
    int end;
    int transition;
} STATE;

// 
// klown info - flag if sequence needs to run till finished or not...
#define MUST_FINISH 1
int klown_state_flags[NUM_STATES] =
{
  0,    //STAND_RIGHT, 
  0,    // STAND_LEFT,
  0,    // WALK_RIGHT, 
  0,    //WALK_LEFT,
  0,    //RUN_RIGHT, 
  0,    //RUN_LEFT,
  MUST_FINISH,  //R2L_TURN, 
  MUST_FINISH,  //L2R_TURN,
  MUST_FINISH,  //THROW_RIGHT, 
  MUST_FINISH,  //THROW_LEFT, 
  MUST_FINISH,  //THROW_RIGHT_WALK, 
  MUST_FINISH,  //THROW_LEFT_WALK,  
  MUST_FINISH,  //THROW_RIGHT_RUN, 
  MUST_FINISH,  //THROW_LEFT_RUN,   
  0,    //IN_RIGHT, 
  0,    //IN_LEFT,
  0,    //OUT_RIGHT, 
  0,    //OUT_LEFT,
  MUST_FINISH,  //DUCK_RIGHT, 
  MUST_FINISH,  //DUCK_LEFT,
  MUST_FINISH,  //BLOCK_RIGHT, 
  MUST_FINISH,  //BLOCK_LEFT,
  MUST_FINISH,  //POKE_RIGHT, 
  MUST_FINISH,  //POKE_LEFT,
  MUST_FINISH,  //HIT_FACE_L, 
  MUST_FINISH,  //HIT_FACE_R,
  MUST_FINISH,  //HIT_BACK_L, 
  MUST_FINISH,  //HIT_BACK_R,
  MUST_FINISH,  //POKED_L, 
  MUST_FINISH,  //POKED_R,
  MUST_FINISH,  //IS_DEAD   
  MUST_FINISH               // GAME_OVER
};

int klown_states_index [INT_NUM_STATES];
// klown state machine - triplets describing all possible transitions:
STATE klown_states[] =
{
    { STAND_RIGHT, WALK_RIGHT, WALK_R },
    { STAND_RIGHT, RUN_RIGHT,  RUN_R },
    { STAND_RIGHT, BLOCK_RIGHT,  BLOCK },
    { STAND_RIGHT, POKE_RIGHT,  POKE },
    { STAND_RIGHT, DUCK_RIGHT,  DUCK },
    { STAND_RIGHT, IN_RIGHT,  GO_IN },
    { STAND_RIGHT, OUT_RIGHT,  GO_OUT },
    { STAND_RIGHT, THROW_RIGHT,  THROW },
    { STAND_RIGHT, R2L_TURN,  WALK_L },
    { STAND_RIGHT, R2L_TURN,  RUN_L },
    { STAND_RIGHT, HIT_FACE_R,  GOT_HIT_R },
    { STAND_RIGHT, HIT_BACK_R,  GOT_HIT_L },
    { STAND_RIGHT, POKED_R,  GOT_POKED_R },

    { STAND_LEFT, WALK_LEFT, WALK_L },
    { STAND_LEFT, RUN_LEFT,  RUN_L },
    { STAND_LEFT, BLOCK_LEFT,  BLOCK },
    { STAND_LEFT, POKE_LEFT,  POKE },
    { STAND_LEFT, DUCK_LEFT,  DUCK },
    { STAND_LEFT, IN_LEFT,  GO_IN },
    { STAND_LEFT, OUT_LEFT,  GO_OUT },
    { STAND_LEFT, THROW_LEFT,  THROW },
    { STAND_LEFT, L2R_TURN,  WALK_R },
    { STAND_LEFT, L2R_TURN,  RUN_R },
    { STAND_LEFT, HIT_FACE_L,  GOT_HIT_L },
    { STAND_LEFT, HIT_BACK_L,  GOT_HIT_R },
    { STAND_LEFT, POKED_L,  GOT_POKED_L },

    { WALK_RIGHT, WALK_RIGHT, WALK_R },
    { WALK_RIGHT, RUN_RIGHT, RUN_R },
    { WALK_RIGHT, THROW_RIGHT_WALK, THROW },
    { WALK_RIGHT, R2L_TURN, RUN_L },
    { WALK_RIGHT, R2L_TURN, WALK_L },
    { WALK_RIGHT, IN_RIGHT,  GO_IN },
    { WALK_RIGHT, OUT_RIGHT,  GO_OUT },
    { WALK_RIGHT, STAND_RIGHT, NOTHING },

    { WALK_LEFT, WALK_LEFT, WALK_L },
    { WALK_LEFT, RUN_LEFT, RUN_L },
    { WALK_LEFT, THROW_LEFT_WALK, THROW },
    { WALK_LEFT, L2R_TURN, RUN_R },
    { WALK_LEFT, L2R_TURN, WALK_R },
    { WALK_LEFT, IN_LEFT,  GO_IN },
    { WALK_LEFT, OUT_LEFT,  GO_OUT },
    { WALK_LEFT, STAND_LEFT, NOTHING },

    { RUN_RIGHT, RUN_RIGHT, RUN_R },
    { RUN_RIGHT, WALK_RIGHT, WALK_R },
    { RUN_RIGHT, THROW_RIGHT_RUN, THROW },
    { RUN_RIGHT, R2L_TURN, RUN_L },
    { RUN_RIGHT, R2L_TURN, WALK_L },
    { RUN_RIGHT, IN_RIGHT,  GO_IN },
    { RUN_RIGHT, OUT_RIGHT,  GO_OUT },
    { RUN_RIGHT, STAND_RIGHT, NOTHING },

    { RUN_LEFT, RUN_LEFT, RUN_L },
    { RUN_LEFT, WALK_LEFT, WALK_L },
    { RUN_LEFT, THROW_LEFT_RUN, THROW },
    { RUN_LEFT, L2R_TURN, RUN_R },
    { RUN_LEFT, L2R_TURN, WALK_R },
    { RUN_LEFT, IN_LEFT,  GO_IN },
    { RUN_LEFT, OUT_LEFT,  GO_OUT },
    { RUN_LEFT, STAND_LEFT, NOTHING },

    { R2L_TURN, STAND_LEFT, EOS },

    { L2R_TURN, STAND_RIGHT, EOS },

    { DUCK_RIGHT, STAND_RIGHT, EOS },

    { DUCK_LEFT, STAND_LEFT, EOS },

    { BLOCK_RIGHT, STAND_RIGHT, EOS },

    { BLOCK_LEFT, STAND_LEFT, EOS },

    { POKE_RIGHT, STAND_RIGHT, EOS },

    { POKE_LEFT, STAND_LEFT, EOS },

    { IN_RIGHT, WALK_RIGHT, WALK_R },
    { IN_RIGHT, R2L_TURN, WALK_L },
    { IN_RIGHT, OUT_RIGHT, GO_OUT },
    { IN_RIGHT, STAND_RIGHT, EOS },
    { IN_RIGHT, STAND_RIGHT, NOTHING },

    { IN_LEFT, WALK_LEFT, WALK_L },
    { IN_LEFT, L2R_TURN, WALK_R },
    { IN_LEFT, OUT_LEFT, GO_OUT },
    { IN_LEFT, STAND_LEFT, EOS },
    { IN_LEFT, STAND_LEFT, NOTHING },

    { OUT_RIGHT, WALK_RIGHT, WALK_R },
    { OUT_RIGHT, R2L_TURN, WALK_L },
    { OUT_RIGHT, IN_RIGHT, GO_IN },
    { OUT_RIGHT, STAND_RIGHT, EOS },
    { OUT_RIGHT, STAND_RIGHT, NOTHING},

    { OUT_LEFT, WALK_LEFT, WALK_L },
    { OUT_LEFT, L2R_TURN, WALK_R },
    { OUT_LEFT, IN_LEFT, GO_IN },
    { OUT_LEFT, STAND_LEFT, EOS },
    { OUT_LEFT, STAND_LEFT, NOTHING },

    { HIT_FACE_L, STAND_LEFT, EOS },

    { HIT_FACE_R, STAND_RIGHT, EOS },

    { HIT_BACK_L, STAND_LEFT, EOS },

    { HIT_BACK_R, STAND_RIGHT, EOS },

    { POKED_L, STAND_LEFT, EOS },

    { POKED_R, STAND_RIGHT, EOS },

    { THROW_RIGHT, INT_THROW_RIGHT, EOS },
    { THROW_LEFT, INT_THROW_LEFT, EOS },
    { THROW_RIGHT_WALK, INT_THROW_RIGHT_WALK, EOS },
    { THROW_LEFT_WALK, INT_THROW_LEFT_WALK, EOS },
    { THROW_RIGHT_RUN, INT_THROW_RIGHT_RUN, EOS },
    { THROW_LEFT_RUN, INT_THROW_LEFT_RUN, EOS },
    
    { IS_DEAD, GAME_OVER, EOS },

// HEY!  PAY ATTENTION!  States listed here on out MUST NOT be returned
// to the level object... they are for internal use only, and since they 
// don't have sprite sequences defined for them ,the game will die horribly
// if they are returned!  I warned you...
    { INT_THROW_RIGHT, STAND_RIGHT, EOS },
    { INT_THROW_LEFT, STAND_LEFT, EOS },
    { INT_THROW_RIGHT_WALK, WALK_RIGHT, EOS },
    { INT_THROW_LEFT_WALK, WALK_LEFT, EOS },
    { INT_THROW_RIGHT_RUN, RUN_RIGHT, EOS },
    { INT_THROW_LEFT_RUN, RUN_LEFT, EOS },

    { -1,-1,-1 }    // end of states.  don't change!
};

typedef signed char SCHAR;

typedef struct 
{
    LONG    posx;
    LONG    posy;
    SCHAR   state;
    SCHAR   velx;
    SCHAR   vely;
    SCHAR   curZ;
} GENERIC_CHAR_INFO;

typedef struct
{
    int curState;
    int LastMouseX;
    int LastMouseY;
    DWORD   timeLastUpdate;
    int HitsLeft;
    int pushedState;
    CGameCharacter * myPie;
    int type;           // 0 = main; 1=computer; 2= second;
    int IGotKilled;
    DWORD   Timeout;
    DWORD   UpdateInterval;
} KLOWN_DATA;


#define KLOWN_MOVE  1
#define KLOWN_PIE   2

HINSTANCE hInst = NULL;            // our library instance
// This is returned by the 'Ident' function, but isn't used internally at all
CGameVersionIdent version =
{
    RELEASE1_0,
    GAMEID
};

// prototypes so we can fill in the action arrays
int PieCreate( CGameCharacter *, CGameLevel * );
int PieAction( CGameCharacter *, CGameLevel * );
int PieDestroy( CGameCharacter *, CGameLevel * );
int PieCollide( CGameCharacter *, CGameCharacter *, CGameLevel * );
int KlownCreate( CGameCharacter *, CGameLevel * );
int KlownAction( CGameCharacter *, CGameLevel * );
int KlownDestroy( CGameCharacter *, CGameLevel * );
int KlownRemoteAction( CGameCharacter *, CGameLevel * );
int KlownCollide( CGameCharacter *, CGameCharacter *, CGameLevel * );

CGameCharSequenceInfo char1seq[2] =
{
    // sequence 0: ThrownLeft
    {"piefly.spr", "FlyLeft", 30, {"throw.wav", NULL, 100, 0}},
    // sequence 1: ThrownRight
    {"piefly.spr", "FlyRight", 30, {"throw.wav", NULL, 100, 0}}
};

// NOTE: the klown sequences below are all the same.  They are included
// separately to permit possible future changes in behaviour for different klowns.
CGameCharSequenceInfo klown1seq[32] =
{
    {"c1stand.spr", "StandRight", 30, {NULL, NULL, 0, 0}},          // STAND_RIGHT
    {"c1stand.spr", "StandLeft", 30, {NULL, NULL, 0, 0}},           // STAND_LEFT
    {"c1walk.spr", "WalkRight", 30, {"walk.wav", NULL, 40, 1}},     // WALK_RIGHT
    {"c1walk.spr", "WalkLeft", 30, {"walk.wav", NULL, 40, 1}},      // WALK_LEFT
    {"c1run.spr", "RunRight", 30, {"run.wav", NULL, 60, 1}},        // RUN_RIGHT
    {"c1run.spr", "RunLeft", 30, {"run.wav", NULL, 60, 1}},         // RUN_LEFT
    {"c1turn.spr", "Right2Left", 30, {NULL, NULL, 80, 1}},          // R2L_TURN
    {"c1turn.spr", "Left2Right", 30, {NULL, NULL, 80, 1}},          // L2R_TURN
    {"c1throw.spr", "ThrowRight", 30, {NULL, NULL, 80, 1}},         // THROW_RIGHT
    {"c1throw.spr", "ThrowLeft", 30, {NULL, NULL, 80, 1}},          // THROW_LEFT
    {"c1stand.spr", "StandRight", 30, {NULL, NULL, 80, 1}},         // THROW_RIGHT_WALK
    {"c1stand.spr", "StandLeft", 30, {NULL, NULL, 80, 1}},          // THROW_LEFT_WALK
    {"c1stand.spr", "StandRight", 30, {NULL, NULL, 80, 1}},         // THROW_RIGHT_RUN
    {"c1stand.spr", "StandLeft", 30, {NULL, NULL, 80, 1}},          // THROW_LEFT_RUN
    {"c1walk45.spr", "InRight", 30, {"walk.wav", NULL, 40, 1}},     // IN_RIGHT
    {"c1walk45.spr", "InLeft", 30, {"walk.wav", NULL, 40, 1}},      // IN_LEFT
    {"c1walk45.spr", "OutRight", 30, {"walk.wav", NULL, 40, 1}},    // OUT_RIGHT
    {"c1walk45.spr", "OutLeft", 30, {"walk.wav", NULL, 40, 1}},     // OUT_LEFT
    {"c1duck.spr", "DuckRight", 30, {NULL, NULL, 40, 1}},           // DUCK_RIGHT
    {"c1duck.spr", "DuckLeft", 30, {NULL, NULL, 40, 1}},            // DUCK_LEFT
    {"c1block.spr", "BlockRight", 30, {NULL, NULL, 40, 1}},         // BLOCK_RIGHT
    {"c1block.spr", "BlockLeft", 30, {NULL, NULL, 40, 1}},          // BLOCK_LEFT
    {"c1poke.spr", "PokeRight", 30, {"nyuk.wav", NULL, 63, 0}},     // POKE_RIGHT
    {"c1poke.spr", "PokeLeft", 30, {"nyuk.wav", NULL, 63, 0}},      // POKE_LEFT
    {"c1piefac.spr", "PieFaceLeft", 30, {NULL, "piehit2.wav", 100, 0}},     // HIT_FACE_L
    {"c1piefac.spr", "PieFaceRight", 30, {NULL, "piehit2.wav", 100, 0}},    // HIT_FACE_R
    {"c1piehed.spr", "PieHeadLeft", 30, {NULL, "piehit2.wav", 100, 0}},     // HIT_BACK_L
    {"c1piehed.spr", "PieHeadRight", 30, {NULL, "piehit2.wav", 100, 0}},    // HIT_BACK_R
    {"c1stand.spr", "StandLeft", 30, {"woob.wav", NULL, 63, 0}},    // POKED_L
    {"c1stand.spr", "StandRight", 30, {"woob.wav", NULL, 63, 0}},   // POKED_R
    {"c1sad.spr", "C1SadLeft", 10, {"sad.wav", NULL, 100, 0}},      // IS_DEAD
    {"c1sad.spr", "C1SadDone", 10, {NULL, NULL, 100, 0}}            // GAME_OVER
};

CGameCharSequenceInfo klown2seq[32] =
{
    {"c2stand.spr", "C2StandRight", 30, {NULL, NULL, 0, 0}},            // STAND_RIGHT
    {"c2stand.spr", "C2StandLeft", 30, {NULL, NULL, 0, 0}},         // STAND_LEFT
    {"c2walk.spr", "C2WalkRight", 30, {"walk.wav", NULL, 40, 1}},       // WALK_RIGHT
    {"c2walk.spr", "C2WalkLeft", 30, {"walk.wav", NULL, 40, 1}},        // WALK_LEFT
    {"c2run.spr", "C2RunRight", 30, {"run.wav", NULL, 60, 1}},      // RUN_RIGHT
    {"c2run.spr", "C2RunLeft", 30, {"run.wav", NULL, 60, 1}},           // RUN_LEFT
    {"c2turn.spr", "C2Right2Left", 30, {NULL, NULL, 80, 1}},            // R2L_TURN
    {"c2turn.spr", "C2Left2Right", 30, {NULL, NULL, 80, 1}},            // L2R_TURN
    {"c2throw.spr", "C2ThrowRight", 30, {NULL, NULL, 80, 1}},           // THROW_RIGHT
    {"c2throw.spr", "C2ThrowLeft", 30, {NULL, NULL, 80, 1}},            // THROW_LEFT
    {"c2stand.spr", "C2StandRight", 30, {NULL, NULL, 80, 1}},           // THROW_RIGHT_WALK
    {"c2stand.spr", "C2StandLeft", 30, {NULL, NULL, 80, 1}},            // THROW_LEFT_WALK
    {"c2stand.spr", "C2StandRight", 30, {NULL, NULL, 80, 1}},           // THROW_RIGHT_RUN
    {"c2stand.spr", "C2StandLeft", 30, {NULL, NULL, 80, 1}},            // THROW_LEFT_RUN
    {"c2walk45.spr", "C2InRight", 30, {"walk.wav", NULL, 40, 1}},       // IN_RIGHT
    {"c2walk45.spr", "C2InLeft", 30, {"walk.wav", NULL, 40, 1}},        // IN_LEFT
    {"c2walk45.spr", "C2OutRight", 30, {"walk.wav", NULL, 40, 1}},  // OUT_RIGHT
    {"c2walk45.spr", "C2OutLeft", 30, {"walk.wav", NULL, 40, 1}},       // OUT_LEFT
    {"c2duck.spr", "C2DuckRight", 30, {NULL, NULL, 40, 1}},         // DUCK_RIGHT
    {"c2duck.spr", "C2DuckLeft", 30, {NULL, NULL, 40, 1}},          // DUCK_LEFT
    {"c2block.spr", "C2BlockRight", 30, {NULL, NULL, 40, 1}},           // BLOCK_RIGHT
    {"c2block.spr", "C2BlockLeft", 30, {NULL, NULL, 40, 1}},            // BLOCK_LEFT
    {"c2poke.spr", "C2PokeRight", 30, {"nyuk.wav", NULL, 63, 0}},       // POKE_RIGHT
    {"c2poke.spr", "C2PokeLeft", 30, {"nyuk.wav", NULL, 63, 0}},        // POKE_LEFT
    {"c2piefac.spr", "C2PieFaceLeft", 30, {NULL, "piehit2.wav", 100, 0}},       // HIT_FACE_L
    {"c2piefac.spr", "C2PieFaceRight", 30, {NULL, "piehit2.wav", 100, 0}},  // HIT_FACE_R
    {"c2piehed.spr", "C2PieHeadLeft", 30, {NULL, "piehit2.wav", 100, 0}},       // HIT_BACK_L
    {"c2piehed.spr", "C2PieHeadRight", 30, {NULL, "piehit2.wav", 100, 0}},  // HIT_BACK_R
    {"c2stand.spr", "C2StandLeft", 30, {"woob.wav", NULL, 63, 0}},  // POKED_L
    {"c2stand.spr", "C2StandRight", 30, {"woob.wav", NULL, 63, 0}}, // POKED_R
    {"c2sad.spr", "c2SadLeft", 10, {"sad.wav", NULL, 100, 0}},      // IS_DEAD
    {"c2sad.spr", "c2SadDone", 10, {NULL, NULL, 100, 0}}            // GAME_OVER
};

#if 0   // !!! don't have all the bitmaps yet

CGameCharSequenceInfo klown3seq[32] =
{
    {"C3stand.spr", "C3StandRight", 30, {NULL, NULL, 0, 0}},            // STAND_RIGHT
    {"C3stand.spr", "C3StandLeft", 30, {NULL, NULL, 0, 0}},         // STAND_LEFT
    {"C3walk.spr", "C3WalkRight", 30, {"walk.wav", NULL, 40, 1}},       // WALK_RIGHT
    {"C3walk.spr", "C3WalkLeft", 30, {"walk.wav", NULL, 40, 1}},        // WALK_LEFT
    {"C3run.spr", "C3RunRight", 30, {"run.wav", NULL, 60, 1}},      // RUN_RIGHT
    {"C3run.spr", "C3RunLeft", 30, {"run.wav", NULL, 60, 1}},           // RUN_LEFT
    {"C3turn.spr", "C3Right2Left", 30, {NULL, NULL, 80, 1}},            // R2L_TURN
    {"C3turn.spr", "C3Left2Right", 30, {NULL, NULL, 80, 1}},            // L2R_TURN
    {"C3throw.spr", "C3ThrowRight", 30, {NULL, NULL, 80, 1}},           // THROW_RIGHT
    {"C3throw.spr", "C3ThrowLeft", 30, {NULL, NULL, 80, 1}},            // THROW_LEFT
    {"C3stand.spr", "C3StandRight", 30, {NULL, NULL, 80, 1}},           // THROW_RIGHT_WALK
    {"C3stand.spr", "C3StandLeft", 30, {NULL, NULL, 80, 1}},            // THROW_LEFT_WALK
    {"C3stand.spr", "C3StandRight", 30, {NULL, NULL, 80, 1}},           // THROW_RIGHT_RUN
    {"C3stand.spr", "C3StandLeft", 30, {NULL, NULL, 80, 1}},            // THROW_LEFT_RUN
    {"C3walk45.spr", "C3InRight", 30, {"walk.wav", NULL, 40, 1}},       // IN_RIGHT
    {"C3walk45.spr", "C3InLeft", 30, {"walk.wav", NULL, 40, 1}},        // IN_LEFT
    {"C3walk45.spr", "C3OutRight", 30, {"walk.wav", NULL, 40, 1}},  // OUT_RIGHT
    {"C3walk45.spr", "C3OutLeft", 30, {"walk.wav", NULL, 40, 1}},       // OUT_LEFT
    {"C3duck.spr", "C3DuckRight", 30, {NULL, NULL, 40, 1}},         // DUCK_RIGHT
    {"C3duck.spr", "C3DuckLeft", 30, {NULL, NULL, 40, 1}},          // DUCK_LEFT
    {"C3block.spr", "C3BlockRight", 30, {NULL, NULL, 40, 1}},           // BLOCK_RIGHT
    {"C3block.spr", "C3BlockLeft", 30, {NULL, NULL, 40, 1}},            // BLOCK_LEFT
    {"C3poke.spr", "C3PokeRight", 30, {"nyuk.wav", NULL, 63, 0}},       // POKE_RIGHT
    {"C3poke.spr", "C3PokeLeft", 30, {"nyuk.wav", NULL, 63, 0}},        // POKE_LEFT
    {"C3piefac.spr", "C3PieFaceLeft", 30, {NULL, "piehit2.wav", 100, 0}},       // HIT_FACE_L
    {"C3piefac.spr", "C3PieFaceRight", 30, {NULL, "piehit2.wav", 100, 0}},  // HIT_FACE_R
    {"C3piehed.spr", "C3PieHeadLeft", 30, {NULL, "piehit2.wav", 100, 0}},       // HIT_BACK_L
    {"C3piehed.spr", "C3PieHeadRight", 30, {NULL, "piehit2.wav", 100, 0}},  // HIT_BACK_R
    {"C3stand.spr", "C3StandLeft", 30, {"woob.wav", NULL, 63, 0}},  // POKED_L
    {"C3stand.spr", "C3StandRight", 30, {"woob.wav", NULL, 63, 0}}, // POKED_R
    {"C3sad.spr", "C3SadLeft", 10, {"sad.wav", NULL, 100, 0}},      // IS_DEAD
    {"C3sad.spr", "C3SadDone", 10, {NULL, NULL, 100, 0}}            // GAME_OVER
};
#endif

// number of klown sequence sets above -1 for main klown
//#define NUM_AUTO_KLOWNS 2
#define NUM_AUTO_KLOWNS 1

// table of sequence tables for allocating non-main klowns
CGameCharSequenceInfo* klownSeqTable[NUM_AUTO_KLOWNS] =
{
    klown2seq //,
//  klown3seq
};

// returned by 'Info' function, and isn't used either (internally)
CGameCharInfo character1 =
{
    "Pie",
    2,
    &char1seq[0],

    PieCreate,
    PieAction,
    PieDestroy,
    NULL,
    PieCollide
};

// returned by 'Info' function, and isn't used either (internally)
static CGameCharInfo character2 =
{
    "Klown",
    32,
    &klown1seq[0],

    KlownCreate,
    KlownAction,
    KlownDestroy,
    KlownRemoteAction,
    KlownCollide
};

// returned by 'Info' function, and isn't used either (internally)
// Klown2 uses the klown2 - klownN sequences above, allocated 1 at a time
static CGameCharInfo character3 =
{
    "Klown2",
    32,
    &klown2seq[0],  // modified at create time

    KlownCreate,
    KlownAction,
    KlownDestroy,
    KlownRemoteAction,
    KlownCollide
};

// This array allows the caller to get our information directly
CGameCharInfo *characters[] =
{
    &character1,
    &character2,
    &character3
};

CGameInfo dllinfo =
{
    3,                 // number of characters implemented in
    // this DLL
    characters             // array of CGameCharInfo pointers
};

void InitStateIndex(STATE *states, int *index, int indexsize);
// EXPORTED as ordinal #1:
#if defined(__BORLANDC__) || defined(__WATCOMC__)
extern "C" void CALLBACK Ident( CGameVersionIdent * id )
#else
void CALLBACK Ident( CGameVersionIdent * id )
#endif
{    
    GetModuleFileName(NULL, inifile, 259);
    char *p = strrchr(inifile, '.');
    if (p)
        lstrcpy(p+1, "GAM");

    gFastKlown = GetPrivateProfileInt("KRUSTY.DLL", "fastklown", 0, inifile);
    gFastVel = GetPrivateProfileInt("KRUSTY.DLL", "fastvel", 20, inifile);
    gDebugOut = GetPrivateProfileInt("KRUSTY.DLL", "debugout", 0, inifile);
    gPieSpeed = GetPrivateProfileInt("KRUSTY.DLL", "piespeed", 5, inifile);
    gPieRange = GetPrivateProfileInt("KRUSTY.DLL", "pierange", 500, inifile);
    gKlownHits = GetPrivateProfileInt("KRUSTY.DLL", "hits", 10, inifile);
    SENSITIVITY = GetPrivateProfileInt("KRUSTY.DLL", "JoystickSensitivity", 33, inifile);
    RUN_THRESHOLD = GetPrivateProfileInt("KRUSTY.DLL", "RunThreshold", 66, inifile);
    gAgression = GetPrivateProfileInt("KRUSTY.DLL", "aggression", 10, inifile);
    gMobility = GetPrivateProfileInt("KRUSTY.DLL", "mobility", 3, inifile);
    gTimeout = (DWORD)GetPrivateProfileInt("KRUSTY.DLL", "RemoteTimeout", 30, inifile)
        * 1000;
    gUpdateInterval = (DWORD)GetPrivateProfileInt("KRUSTY.DLL", "RemoteUpdateInterval", 100, inifile);
    
    memcpy( id, &version, sizeof( CGameVersionIdent ) );

    InitStateIndex(klown_states, &klown_states_index[0], INT_NUM_STATES);
}

// EXPORTED as ordinal #2:
#if defined(__BORLANDC__) || defined(__WATCOMC__)
extern "C" void  CALLBACK Info( CGameInfo * info )
#else
void CALLBACK Info( CGameInfo * info )
#endif
{    
    memcpy( info, &dllinfo, sizeof( dllinfo ) );
}

void InitStateIndex(STATE *states, int *index, int indexsize)
{
    int ix;

    // set index to unused
    memset(index, -1, indexsize * sizeof(int));

    // iterate through states, putting correct index into 'index'
    ix = 0;
    int laststate = -2;
    while (states[ix].start != -1)
    {
        if (states[ix].start != laststate)
        {
            if (states[ix].start < indexsize)
                index[states[ix].start] = ix;

            laststate = states[ix].start;
        }
        ++ix;
    }
}

int ChangeState ( STATE * states, 
                int *index,
                int curstate, 
                int transition, 
                CGameCharacter * me, 
                int time)
{
    /*  
        The STATEs are grouped by state; they are "clumped" together, so
        that once we find the 'curstate' in the array, we can look at all
        the possible transitions out of 'curstate'.

        The last state in the array of allowable states is "-1".  
    */


    // first off: does this state care about any transitions?
    if (klown_state_flags[curstate] & MUST_FINISH)
    {
        // must finish this sequence; ignore transition!
        if (me->NextSprite(time, FALSE) == 0)
        {
            // end of sequence;
            transition = EOS;
        }
        else
        {
            // not end; just return
            return(curstate);                       
        }
    }
    else
    {
        // do the next sprite
        me->NextSprite(time, TRUE);
    }

    int ix = index[curstate];
    while (states[ix].start == curstate)
    {
        if (states[ix].transition == transition)
        {
            return(states[ix].end);
        }                   
        ++ix;
    }
    // didn't find our transition, so don't change state
    return(curstate);           
}

int     PieCreate( CGameCharacter *me, CGameLevel *level )
{
    // use private data as plain old integer...
    me->mpPrivateData = (void *) level->GetFrameTime();

    return ( ACTION_COMPLETED );
}

int     PieAction( CGameCharacter *me, CGameLevel *level )
{
    int posx, posy, velx, vely;
    int time = level->GetFrameTime();
    int slices = (me->mLastTime == -1) ? 1 : time - me->mLastTime;
    me->mLastTime = time;

    me->GetVelocity(&velx, &vely);

    // remember to use sub-pixels!
    me->GetSubXY(&posx, &posy);
    posx += SUBPIXEL_DELTA(velx, slices);
    posy += SUBPIXEL_DELTA(vely, slices);

    if (time > ((int) me->mpPrivateData + gPieRange))
    {
        posy = -100000;
        me->mpPrivateData = (void *)-1;
        me->SetAndMove(posx, posy);
        me->NextSprite(level->GetTimer()->Time, FALSE);
        return(-2);     
    }
    else
    {
        me->SetAndMove(posx, posy);
        me->NextSprite(level->GetTimer()->Time, FALSE);     

        if (velx < 0)
            return(0);
        else
            return(1);
    }

}

int     PieDestroy( CGameCharacter *me, CGameLevel *level )
{
    me->mpPrivateData = (void *)-1;
    return ( ACTION_COMPLETED );
}

int     PieCollide( CGameCharacter *me, CGameCharacter *other, CGameLevel *level )
{   
    int posx, posy;
    me->GetSubXY(&posx, &posy);

    //move off the visible world, so we "disappear"
    posy = -100000;
    me->SetAndMove(posx, posy);
    me->mpPrivateData = (void *)-1;
    return(0);
}

static int curKlown = 0;

static int     KlownCreate( CGameCharacter *me, CGameLevel *level )
{
    KLOWN_DATA  *pKlown = new KLOWN_DATA;
    int x,y,buttons;
//  srand(0);
    memset(pKlown, 0, sizeof(KLOWN_DATA));
    me->mpPrivateData = (void *) pKlown;
    if (level->GetInput()->GetMouse(x, y, buttons))
    {
        pKlown->LastMouseX =  x;
        pKlown->LastMouseY =  y;        
    }

    pKlown->curState = STAND_RIGHT;
    pKlown->pushedState = -1;
    pKlown->HitsLeft = gKlownHits;
    pKlown->timeLastUpdate = level->GetFrameTime();
    pKlown->IGotKilled = 0;

    pKlown->Timeout = gTimeout;
    pKlown->UpdateInterval = gUpdateInterval;

    BOOL isMain = (lstrcmpi(me->GetName(), "klown") == 0);

    if (isMain && gFastKlown)
    {       
        pKlown->curState = RUN_RIGHT;
        me->SetVelocity(gFastVel,0);
    }
    else
    {
        pKlown->myPie = level->Add("Pie",me->GetCurrentZ(),0,-100000);      
    }

    if (!isMain)
    {
        // set sequence set for non-main klowns using counter to allocate
        // NOTE: this will change our global copy, so be careful!
        me->mpCharInfo->Sequences = klownSeqTable[curKlown++ % NUM_AUTO_KLOWNS];
    }

    return ( ACTION_COMPLETED );
}

static void FirePie (
    int posx,
    int posy,
    int velx,
    int vely,
    CGameCharacter *me, 
    CGameLevel *level,
    BOOL    fRemote
)
{
    KLOWN_DATA *pKlown = (KLOWN_DATA *) me->mpPrivateData;
        char chBuffer[128];

    if (pKlown->myPie == NULL)
    {
        pKlown->myPie = level->Add("Pie",me->GetCurrentZ(),0,0);                
    }

    PieCreate(pKlown->myPie, level);

    pKlown->myPie->SetCurrentZ(me->GetCurrentZ());
    if (velx > 0)   
    {
        pKlown->myPie->MoveTo(posx + me->GetCurWidth(), posy);
    }
    else    
    {
        pKlown->myPie->MoveTo(posx - 32, posy);
    }

    pKlown->myPie->SetVelocity(velx, vely);
    if (!fRemote)
    {
        GENERIC_CHAR_INFO   ci;

        ci.state = 0;
        ci.posx = posx;
        ci.posy = posy;
        ci.velx = (velx > 0) ? 1 : -1;
        ci.vely = (vely > 0) ? 1 : -1;
                wsprintf( chBuffer, "Pie %d %d %d %d\r\n",
                    velx, vely, ci.velx, ci.vely);
                OutputDebugString(chBuffer);
        ci.curZ = me->GetCurrentZ();
        me->TransmitRemoteAction(KLOWN_PIE, &ci, sizeof(ci));
    }
}

int AddWithLimit(int base, int howmuch, int lowerlimit, int upperlimit)
{
    base += howmuch;
    if (base > upperlimit)
        base = upperlimit;
    if (base < lowerlimit)
        base = lowerlimit;
    return(base);   
}

int AdjustedY(int base, int curz, int newz, int minz, int maxz)
{
    int y_shift = 0;
    int old_shift = 0;

    y_shift = (newz - (maxz - minz)) << 3;
    old_shift = (curz - (maxz - minz)) << 3;
    base += (old_shift << 8);
    base -= (y_shift  << 8);
    return(base);   
}

// returns the new state ("action")
static int     KlownAction( CGameCharacter *me, CGameLevel *level )
{
    int time = level->GetFrameTime();
    int slices = (me->mLastTime == -1) ? 1 : time - me->mLastTime;
    me->mLastTime = time;

    int velx, vely, zchange;
    int posx, posy;
    KLOWN_DATA *pKlown = (KLOWN_DATA *) me->mpPrivateData;

    int oldstate = pKlown->curState;
    int newstate = oldstate;
    int transition = NOTHING;

    JOYINFO joy;
    int x, y, buttons;

    zchange = 0;

    me->GetVelocity(&velx, &vely);

    // remember to use sub-pixels!
    me->GetSubXY(&posx, &posy);

    CGameInput * Input = level->GetInput();
    if ((pKlown->type == 0) && (gFastKlown != (int) level->mFastKlown)) 
    {
        gFastKlown = (int) level->mFastKlown;
        if (gFastKlown)
        {
            pKlown->curState = RUN_RIGHT;
            velx = gFastVel;
            vely = 0;
            me->SetVelocity(velx,vely);
            newstate = oldstate = RUN_RIGHT;        
        }
        else
        {
            pKlown->curState = STAND_RIGHT;
            me->SetVelocity( 0,0 );
            newstate = oldstate = STAND_RIGHT;      
        }
    }

    if (gFastKlown && (pKlown->type == 0))  
    {
        me->NextSprite(time);
        newstate = oldstate;        

        if (SUB2WORLD(posx) <= (-level->GetMaxX()))
        {
            velx = -velx;
            newstate = RUN_RIGHT;
        }
        else if (SUB2WORLD(posx) >= (level->GetMaxX() - me->GetCurWidth() ))
        {
            newstate = RUN_LEFT;
            velx = -velx;
        }
    }
    else
    {
    // step 1: Grab input and figure out what the command is:
        velx=vely=zchange=0;

        int alt, ctrl, shift;
        int right, left, up, down;

        alt = ctrl = shift = right = left = up = down = 0;

        switch (pKlown->type)
        {
            case 0:         // "normal"
                alt = Input->GetKeyboard(VK_MENU);
                ctrl = Input->GetKeyboard(VK_CONTROL);
                shift=Input->GetKeyboard(VK_SHIFT);
                right = (Input->GetKeyboard(VK_RIGHT) != 0) ||
                       (Input->GetKeyboard(VK_NUMPAD6) != 0);
                left = (Input->GetKeyboard(VK_LEFT) != 0) ||
                       (Input->GetKeyboard(VK_NUMPAD4) != 0);
                up = (Input->GetKeyboard(VK_UP) != 0) ||
                       (Input->GetKeyboard(VK_NUMPAD8) != 0);
                down = (Input->GetKeyboard(VK_DOWN) != 0) ||
                       (Input->GetKeyboard(VK_NUMPAD2) != 0);

                if (! (up || down || left || right || shift))
                {

                    if (Input->GetMouse(x, y, buttons))
                    {
                        int deltaX = x - pKlown->LastMouseX;
                        int deltaY = y - pKlown->LastMouseY;

                        right = (deltaX > MIN_MOUSE_MOVE);
                        left = (deltaX < -MIN_MOUSE_MOVE);
                        if ((deltaX > BIG_MOUSE_MOVE) ||  (deltaX < -BIG_MOUSE_MOVE))
                        {
                            ctrl = TRUE;
                        }
                        down = (deltaY > 5*MIN_MOUSE_MOVE);
                        up = (deltaY < -5*MIN_MOUSE_MOVE);

                        shift = shift || (buttons & 1);
                        alt = buttons & 2;

                        x = CenterX;
                        y = CenterY;

                        pKlown->LastMouseX = x;
                        pKlown->LastMouseY = y;
                        SetCursorPos(x, y);
                    }
                }

                if (! (up || down || left || right || shift))
                {

                    // grab joystick input...
                    if (Input->GetJoystick(1, &joy))
                    {
                        // motion left ,right, up or down; button 1=fire pie. button2=flip
                        int vel;
                        vel = joy.wXpos;
                        if (vel >= SENSITIVITY)
                        {
                            // moved right:
                            right = 1;
                            if (vel >= RUN_THRESHOLD)
                                ctrl = 1;
                        }
                        else if (vel <= -SENSITIVITY)
                        {
                            // moved left:
                            left = 1;
                            if (vel <= -RUN_THRESHOLD)
                                ctrl = 1;
                        }

                        vel = joy.wYpos;
                        if (vel >= SENSITIVITY)
                            down = 1;
                        else if (vel <= -SENSITIVITY)
                            up = 1;

                        if (joy.wButtons & JOY_BUTTON1)
                            shift = 1;
                        if (joy.wButtons & JOY_BUTTON2)
                            alt = 1;
                    }
                }
                break;

            case 1:         // "robo"
                // where is the main klown, relative to us?
                if (level->mMainKlown && (level->mMainKlown != me))
                {
                    // get its rectangle
                    PRECT pMainRect = level->mMainKlown->GetRect();
                    PRECT pMyRect  = me->GetRect();

                    // first: if we were walking, we should keep on walking!
                    if (oldstate == WALK_LEFT || oldstate == WALK_RIGHT)
                    {
                        if (rand() % 10)
                        {
                            if (oldstate == WALK_LEFT)
                                ++left;
                            else
                                ++right;
                            goto no_change;
                            
                        }

                    }

                    if ((int)(rand() % 100) <= gMobility)
                    {
                        if (rand() % 5)
                        { 
                            if (pMainRect->left < pMyRect->left) 
                               ++left;
                            else
                                ++right;
                        }
                        else
                        {
                            int z = level->mMainKlown->GetCurrentZ();
                            if ((z > me->GetCurrentZ()) || gFastKlown)
                            {
                                // increase our z
                                ++up;
                            }
                            else if (z < me->GetCurrentZ())
                            {
                                // decrease our z
                                ++down;
                            }
                        }

no_change:          
                    // nothing
                        ;
                    }
                    else if ((int)(rand() % 100) <= gAgression)
                        ++shift;
                }

                break;

            case 2:         // "second" always uses second joystick
                if (Input->GetJoystick(2, &joy))
                {
                    // motion left ,right, up or down; button 1=fire pie. button2=flip
                    int vel;
                    vel = joy.wXpos;
                    if (vel >= SENSITIVITY)
                    {
                        // moved right:
                        right = 1;
                        if (vel >= RUN_THRESHOLD)
                            ctrl = 1;
                    }
                    else if (vel <= -SENSITIVITY)
                    {
                        // moved left:
                        left = 1;
                        if (vel <= -RUN_THRESHOLD)
                            ctrl = 1;
                    }

                    vel = joy.wYpos;
                    if (vel >= SENSITIVITY)
                        down = 1;
                    else if (vel <= -SENSITIVITY)
                        up = 1;

                    if (joy.wButtons & JOY_BUTTON1)
                        shift = 1;
                    if (joy.wButtons & JOY_BUTTON2)
                        alt = 1;
                }
                break;
        }

        if (up)
            transition = GO_IN;
        if (down)
            transition = GO_OUT;
        if (left)
            transition = ctrl ? RUN_L : WALK_L;
        if (right)
            transition = ctrl ? RUN_R : WALK_R;
        if (shift)
            transition = THROW;
        if (alt && down)
            transition = DUCK;
        if (shift && down)
            transition = POKE;
        if (alt && shift)
            transition = BLOCK;

        // got a transition; figure out the new state
        if (pKlown->pushedState >= 0)
        {
            newstate = pKlown->pushedState;
            pKlown->pushedState = -1;
        }
        else
        {
            newstate = ChangeState( klown_states, 
                                    klown_states_index,
                                    oldstate, 
                                    transition, 
                                    me, 
                                    time);
        }

        switch (newstate)
        {
            case INT_COLLIDED_ON_RIGHT:
                if (rand() % 2)
                {
                    posx -= WORLD2SUB(me->GetCurWidth()/4);
                    ++zchange;                  
                }
                newstate = STAND_RIGHT;
                break;

            case INT_COLLIDED_ON_LEFT:
                if (rand() % 2)
                {
                    posx += WORLD2SUB(me->GetCurWidth()/4);
                    ++zchange;
                }
                newstate = STAND_LEFT;
                break;
                
            case HIT_FACE_L:
            case HIT_FACE_R:
            case HIT_BACK_L:
            case HIT_BACK_R:
                if (newstate != oldstate)
                {
                    --pKlown->HitsLeft;
                    if (pKlown->HitsLeft <= 0)
                    {
                        newstate = IS_DEAD;
                    }
                }
                break;

            case IS_DEAD:
                velx = 0;
                vely = 0;
                ++zchange;
                break;

            case GAME_OVER:
                level->GameOver();
                pKlown->IGotKilled = 1;

                // reset scores of all Klowns...
                {
                    KLOWN_DATA * pK = (KLOWN_DATA *) level->mMainKlown->mpPrivateData;
                    if (pK)
                        pK->HitsLeft = gKlownHits;
                    if (level->mSecondKlown)
                    {
                        pK = (KLOWN_DATA *) level->mSecondKlown->mpPrivateData;
                        if (pK)
                            pK->HitsLeft = gKlownHits;
                    }
                        
                    // need to change this for more than one computer klown...
                    if (level->mComputerKlowns[0])
                    {
                        pK = (KLOWN_DATA *) level->mComputerKlowns[0]->mpPrivateData;
                        if (pK)
                            pK->HitsLeft = gKlownHits;
                    }
                }

                break;

            case WALK_RIGHT:
                velx = WALK_VELOCITY;
                break;

            case RUN_RIGHT:
                velx = RUN_VELOCITY;
                break;

            case WALK_LEFT:
                velx = -WALK_VELOCITY;
                break;

            case RUN_LEFT:
                velx = -RUN_VELOCITY;
                break;

            case THROW_LEFT:
            case THROW_RIGHT:
            case THROW_LEFT_WALK:
            case THROW_RIGHT_WALK:
            case THROW_LEFT_RUN:
            case THROW_RIGHT_RUN:
                if (newstate != oldstate)
                    if (pKlown->myPie && (pKlown->myPie->mpPrivateData!=(void*)-1))
                        newstate = oldstate;
                break;

            case INT_THROW_RIGHT:
                    newstate = STAND_RIGHT;
                    FirePie(SUB2WORLD(posx), SUB2WORLD(posy), gPieSpeed, 0, me, level, FALSE);
                    break;

            case INT_THROW_RIGHT_WALK:
                    newstate = WALK_RIGHT;
                    FirePie(SUB2WORLD(posx), SUB2WORLD(posy), gPieSpeed, 0, me, level, FALSE);
                    break;

            case INT_THROW_RIGHT_RUN:
                    newstate = RUN_RIGHT;
                    FirePie(SUB2WORLD(posx), SUB2WORLD(posy), gPieSpeed, 0, me, level, FALSE);
                    break;

            case INT_THROW_LEFT:
                    newstate = STAND_LEFT;
                    FirePie(SUB2WORLD(posx), SUB2WORLD(posy), -gPieSpeed, 0, me, level, FALSE);
                    break;

            case INT_THROW_LEFT_WALK:
                    newstate = WALK_LEFT;
                    FirePie(SUB2WORLD(posx), SUB2WORLD(posy), -gPieSpeed, 0, me, level, FALSE);
                    break;

            case INT_THROW_LEFT_RUN:
                    newstate = RUN_LEFT;
                    FirePie(SUB2WORLD(posx), SUB2WORLD(posy), -gPieSpeed, 0, me, level, FALSE);
                    break;

            case IN_LEFT:
                {
                int curz = me->GetCurrentZ();
                velx = -32;
                curz = AddWithLimit(curz, 1, me->GetMinZ(), me->GetMaxZ());
                posy = AdjustedY(posy, me->GetCurrentZ(), curz, me->GetMinZ(), me->GetMaxZ());
                me->SetCurrentZ(curz);
                ++zchange;
                }
                break;

            case IN_RIGHT:
                {
                int curz = me->GetCurrentZ();
                velx = 32;
                curz = AddWithLimit(curz, 1, me->GetMinZ(), me->GetMaxZ());
                posy = AdjustedY(posy, me->GetCurrentZ(), curz, me->GetMinZ(), me->GetMaxZ());
                me->SetCurrentZ(curz);
                ++zchange;
                }
                break;

            case OUT_LEFT:
                {
                int curz = me->GetCurrentZ();
                velx=-32;
                curz = AddWithLimit(curz, -1, me->GetMinZ(), me->GetMaxZ());
                posy = AdjustedY(posy, me->GetCurrentZ(), curz, me->GetMinZ(), me->GetMaxZ());
                me->SetCurrentZ(curz);
                ++zchange;
                }
                break;
            case OUT_RIGHT:
                {
                int curz = me->GetCurrentZ();
                velx=32;
                curz = AddWithLimit(curz, -1, me->GetMinZ(), me->GetMaxZ());
                posy = AdjustedY(posy, me->GetCurrentZ(), curz, me->GetMinZ(), me->GetMaxZ());
                me->SetCurrentZ(curz);
                ++zchange;
                }
                break;
        }
    }

    if (velx || vely || zchange)
    {
        posx += SUBPIXEL_DELTA(slices, velx);
        posy += SUBPIXEL_DELTA(slices, vely);
        int newX = SUB2WORLD(posx);
        int newY = SUB2WORLD(posy);


        if (pKlown->type == 0)
            level->ForceOnScreen(&newX, &newY, me->GetCurWidth(), me->GetCurHeight());
        else
            level->ForceOnScreen(&newX, &newY, me->GetCurWidth(), me->GetCurHeight(), FALSE);
        me->MoveTo(newX, newY);
    }

    me->SetVelocity(velx, vely);
    pKlown->curState = newstate;


    if (pKlown->type == 0)
    {
        if ((newstate != oldstate) || (velx || vely || zchange) 
        || (time - pKlown->timeLastUpdate > pKlown->UpdateInterval))
        {
            GENERIC_CHAR_INFO   ci;
                        int velx;
                        int vely;


            ci.state = newstate;
            me->GetSubXY((int *)&ci.posx, (int *)&ci.posy);
            me->GetVelocity(&velx, &vely);
                        ci.velx = velx;
                        ci.vely = vely;
            ci.curZ = me->GetCurrentZ();

            me->TransmitRemoteAction(KLOWN_MOVE, &ci, sizeof(ci));
            pKlown->timeLastUpdate = time;
        }       
    }
    return ( newstate );
}

static int     KlownDestroy( CGameCharacter *me, CGameLevel *level )
{
    delete (void *) me->mpPrivateData;
    return ( ACTION_COMPLETED );
}

static int     KlownRemoteAction( CGameCharacter *me, CGameLevel *level )
{
    KLOWN_DATA *pKlown = (KLOWN_DATA *) me->mpPrivateData;
    int oldstate = pKlown->curState;

    int time = level->GetFrameTime();
    int slices = (me->mLastTime == -1) ? 1 : time - me->mLastTime;
    me->mLastTime = time;

    void    *Data;
    int newstate = oldstate;
    int action;
    DWORD   DataSize;

    me->NextSprite(time, FALSE);

    int nActionsProcessed = 0;
    while ((action = me->GetRemoteAction(Data, DataSize)) != -1)
    {

        nActionsProcessed++;

        GENERIC_CHAR_INFO *ci = (GENERIC_CHAR_INFO *)Data;

        switch (action) {
        case KLOWN_MOVE:
            newstate = ci->state;
            pKlown->curState = newstate;
            me->SetCurrentZ(ci->curZ);
            me->SetVelocity(ci->velx, ci->vely);
            me->SetAndMove(ci->posx, ci->posy);
            break;
            
        case KLOWN_PIE:
            FirePie(ci->posx, ci->posy, ci->velx * gPieSpeed, 0, me , level, TRUE);
            break;

        }
        me->ReleaseRemoteAction(Data);
    }

    if (nActionsProcessed > 0)
    {
        pKlown->timeLastUpdate = time;
    } 
    else if (time - pKlown->timeLastUpdate > pKlown->Timeout)
    {
        // need to kill 'em off
        return(ACTION_KILLME);                  
    } 
    else 
    {
        int velx, vely;
        int posx, posy;

        // Haven't heard from him, but keep him moving!
        me->GetVelocity(&velx, &vely);
        me->GetSubXY(&posx, &posy);
        if (velx || vely)
        {
            posx += SUBPIXEL_DELTA(slices, velx);
            posy += SUBPIXEL_DELTA(slices, vely);

            me->SetAndMove(posx, posy);
            me->SetVelocity(velx, vely);
        }
    }

    return(newstate);
}

static int boogy = 0;
int     KlownCollide( CGameCharacter *me, CGameCharacter *other, CGameLevel *level )
{   
    KLOWN_DATA *pKlown = (KLOWN_DATA *) me->mpPrivateData;
    RECT myRect, otherRect;
    LPRECT pmyRect, potherRect;

    if ((pKlown->curState == GAME_OVER) ||
       (pKlown->curState == IS_DEAD))
        return(ACTION_COMPLETED);
        

    pmyRect = me->GetRect();
    potherRect = other->GetRect();
    memcpy(&myRect, pmyRect, sizeof(RECT));
    memcpy(&otherRect, potherRect, sizeof(RECT));

        if (lstrcmpi(other->GetName(), "Pie") == 0)
    {
        // got hit by a pie, so decrement HitsLeft
        int velx, vely;
        other->GetVelocity(&velx, &vely);
        switch (pKlown->curState)
        {
        case STAND_LEFT:
        case WALK_LEFT:
        case RUN_LEFT:
            pKlown->pushedState = (velx < 0) ? HIT_BACK_L : HIT_FACE_L;
            break;
        case STAND_RIGHT:
        case WALK_RIGHT:
        case RUN_RIGHT:
            pKlown->pushedState = (velx < 0) ? HIT_FACE_R : HIT_BACK_R;
            break;
        }
    }
    else
    {
        // hit another solid body... need to move out of the way:
        int posx, posy;
        int posx2, posy2;

        if ( (myRect.right < otherRect.left) ||
             (myRect.left > otherRect.right)
            )
            return(ACTION_COMPLETED);

        me->GetXY(&posx, &posy);
        other->GetXY(&posx2, &posy2);

        // back up...
        if (posx2 < posx)
            pKlown->pushedState = INT_COLLIDED_ON_LEFT;
        else if (posx2 > posx)
            pKlown->pushedState = INT_COLLIDED_ON_RIGHT;
        else 
        {
            // coincident... how to handle this?
            if (++boogy % 2)
                pKlown->pushedState = IN_RIGHT;
            else
                pKlown->pushedState = OUT_RIGHT;
        }
    }
    return(ACTION_COMPLETED);   
}
