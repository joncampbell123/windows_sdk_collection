/*==========================================================================
 *
 *  Copyright (C) 1995-1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       duel.h
 *  Content:    main include file
 *
 *
 ***************************************************************************/

#ifndef DUEL_INCLUDED
#define DUEL_INCLUDED

#define INITGUID
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <ddraw.h>
#include <dplay.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include "resource.h"
#include "ddutil.h"

#define DEF_SHOW_DELAY     (2000)
#define MAX_BUFFER_SIZE    256

/*
 * keyboard commands
 */

#define KEY_STOP   0x00000001l
#define KEY_DOWN   0x00000002l
#define KEY_LEFT   0x00000004l
#define KEY_RIGHT  0x00000008l
#define KEY_UP     0x00000010l
#define KEY_FIRE   0x00000020l
#define KEY_THROW  0x00000040l
#define KEY_SHIELD 0x00000080l

// program states
enum
{
    PS_SPLASH,
    PS_ACTIVE,
    PS_BEGINREST,
    PS_REST
};

#define     MAX_SCREEN_X    639
#define     MAX_SCREEN_Y    479
#define     MAX_SHIP_X     (MAX_SCREEN_X - 32)
#define     MAX_SHIP_Y     (MAX_SCREEN_Y - 32)
#define     MAX_SHIP_FRAME 40
#define     MAX_BULLET_X    (MAX_SCREEN_X - 3)
#define     MAX_BULLET_Y    (MAX_SCREEN_Y - 3)
#define     MAX_BULLET_FRAME 400


// Offsets for the bullet bitmap
#define     BULLET_X    304
#define     BULLET_Y    0


/*
 * structures
 */

typedef struct _GLOBALSHIP
{
    BOOL                enable;     // is this ship active?
    double              posx, posy; // x and y position
    double              velx, vely; // x and y velocity (pixels/millisecond)
    double              frame;      // current frame
    BOOL                benable;    // Is there an active bullet?
    double              bposx, bposy;  // bullet x and y position
    double              bvelx, bvely; // bullet x and y velocity (pixels/millisecond)
    double              bframe;     // bullet frame
    int                 score;      // current score
    DWORD               lastTick;   // time since last frame
    DWORD               timeStamp;  // time received
} GLOBALSHIP, *LPGLOBALSHIP;

typedef struct _BLOCKS
{
    BYTE        bits[30][5];
} BLOCKS, *LPBLOCKS;

//----------------------------------------------------------
// communication packet structures
//----------------------------------------------------------
#define MSG_UPDATE      0x11
#define MSG_HEREIAM     0x22
#define MSG_INIT        0x33
#define MSG_BLOCKHIT    0x44
#define MSG_SHIPHIT     0x55
#define MSG_ADDBLOCK    0x66

typedef struct _UPDATEMSG
{
    BYTE        MsgCode;
    BYTE        WhoIAm;
    GLOBALSHIP  Ship;
} UPDATEMSG, *LPUPDATEMSG;

typedef struct _HEREIAMMSG
{
    BYTE        MsgCode;
    DWORD       ID;
} HEREIAMMSG, *LPHEREIAMMSG;

typedef struct _INITMSG
{
    BYTE        MsgCode;
    BYTE        YouAre;
    BLOCKS      Blocks;
} INITMSG, *LPINITMSG;

typedef struct _BLOCKHITMSG
{
    BYTE        MsgCode;
    BYTE        row;
    BYTE        col;
    BYTE        mask;
} BLOCKHITMSG, *LPBLOCKHITMSG;

typedef struct _SHIPHITMSG
{
    BYTE        MsgCode;
    BYTE        You;
} SHIPHITMSG, *LPSHIPHITMSG;

typedef struct _frag
{
    double      posx;
    double      posy;
    LPDIRECTDRAWSURFACE surf;
    RECT        src;
    double      velx;
    double      vely;
    BOOL        valid;
} FRAG, *LPFRAG;

typedef struct _ADDBLOCKMSG
{
    BYTE        MsgCode;
    BYTE        x;
    BYTE        y;
} ADDBLOCKMSG, *LPADDBLOCKMSG;

double      Dirx[40] =
{
    0.000000,
    0.156434,
    0.309017,
    0.453991,
    0.587785,
    0.707107,
    0.809017,
    0.891007,
    0.951057,
    0.987688,
    1.000000,
    0.987688,
    0.951057,
    0.891007,
    0.809017,
    0.707107,
    0.587785,
    0.453990,
    0.309017,
    0.156434,
    0.000000,
    -0.156435,
    -0.309017,
    -0.453991,
    -0.587785,
    -0.707107,
    -0.809017,
    -0.891007,
    -0.951057,
    -0.987688,
    -1.000000,
    -0.987688,
    -0.951056,
    -0.891006,
    -0.809017,
    -0.707107,
    -0.587785,
    -0.453990,
    -0.309017,
    -0.156434
};

double      Diry[40] =
{
    -1.000000,
    -0.987688,
    -0.951057,
    -0.891007,
    -0.809017,
    -0.707107,
    -0.587785,
    -0.453990,
    -0.309017,
    -0.156434,
    0.000000,
    0.156434,
    0.309017,
    0.453991,
    0.587785,
    0.707107,
    0.809017,
    0.891007,
    0.951057,
    0.987688,
    1.000000,
    0.987688,
    0.951057,
    0.891006,
    0.809017,
    0.707107,
    0.587785,
    0.453990,
    0.309017,
    0.156434,
    0.000000,
    -0.156435,
    -0.309017,
    -0.453991,
    -0.587785,
    -0.707107,
    -0.809017,
    -0.891007,
    -0.951057,
    -0.987688
};

double HomeX[16] =
{
    16.0,
    624.0,
    16.0,
    624.0,
    320.0,
    16.0,
    624.0,
    320.0,
    160.0,
    480.0,
    16.0,
    624.0,
    16.0,
    160.0,
    480.0,
    624.0,
};

double HomeY[16] =
{
    16.0,
    16.0,
    464.0,
    464.0,
    16.0,
    240.0,
    240.0,
    464.0,
    16.0,
    464.0,
    360.0,
    120.0,
    120.0,
    464.0,
    16.0,
    360.0
};

double HomeFrame[16] =
{
    15.0,
    25.0,
    5.0,
    35.0,
    20.0,
    10.0,
    30.0,
    0.0,
    20.0,
    0.0,
    10.0,
    30.0,
    10.0,
    0.0,
    20.0,
    30.0
};

/*
 * fn prototypes
 */
void    DestroyGame( void );
BOOL    InitializeGame( void );
void    UpdateFrame( void );
BOOL    CleanupAndExit( int err );
BOOL    RestoreSurfaces( void );
void    initShip( LPGLOBALSHIP ship);
void    initLevel( int level );
int     randInt( int low, int high );
double  randDouble( double low, double high );
void    bltScore( char *num, int x, int y );
void    DisplayFrameRate( void );
void    bltSplash( LPRECT );
void    EraseScreen( void );
void    FlipScreen( void );
void    DisplayLevel( void );
void    DrawScreen( void );
void    DrawShip( int i );
void    DrawBlock( int x, int y );
void    bltObject( int x, int y, LPDIRECTDRAWSURFACE surf, LPRECT src, DWORD flags );
BOOL    setBlock( int x, int y );
void    ReadKey(void);
void    UpdatePosition( LPGLOBALSHIP ship );
void    DrawBullet( int i );
BOOL    IsHit( int x, int y );
BOOL    RemoteCreate(GUID pGuid, LPSTR FullName, LPSTR NickName);
int     GetProvider(void);
int     CreateGame(void);
int     GetGame(void);
BOOL CALLBACK DlgProcChooseProvider(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcQCreate (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcSelSession (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL FAR PASCAL EnumSP(LPGUID lpGuid, LPSTR lpDesc, DWORD dwJamorVersion,
                       DWORD dwMinorVersion, LPVOID lpv);
BOOL FAR PASCAL EnumSession(LPDPSESSIONDESC lpDPGameDesc, LPVOID pContext, 
                            LPDWORD lpdwTimeOut, DWORD dwFlags);
void    EvaluateMessage( DWORD len );
void    ReceiveGameMessages( void );
void    InitGame(void);
void    SendGameMessage( BYTE msg, DWORD to, BYTE row, BYTE col, BYTE mask );
void    DrawScore( void );
void    AddFrag(int which, int offX, int offY);
void    DrawFragments( void );
void    UpdateFragment(int i);
void    DestroyShip( int which );
#endif

