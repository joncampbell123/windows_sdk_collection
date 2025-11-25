/*===========================================================================*\
|
|  File:        cglevel.cpp
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
#include <linklist.h>
#ifdef __WATCOMC__
extern "C" cdecl char   *itoa( int __value, char *__buf, int __radix );
#endif
#include "cggraph.h"
#include "cglevel.h"
#include "cgtimer.h"
#include "cgchdll.h"
#include "cgchrint.h"
#include "strrec.h"
#include "cginput.h"
#include "cgchar.h"
#include "cgimage.h"
#include "cgmidi.h"
#include "cgremote.h"
#include "cgsound.h"
#include "cgglobl.h"
#include "cgtext.h"
#include "cgrsrce.h"

void dbgprintf(char *fmt,...);

extern LPSTR NewStringResource(
    HINSTANCE   hInst,
    int     idString
);
extern void GetRectFromProfile(RECT &, LPSTR, LPSTR, LPSTR);

extern CGameTimer * Timer;
static RECT WholeScreen = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
HDC LoadBitmapFile (
    LPSTR   pBitmapFile
);
#define BASE_HWND   ghMainWnd

/*---------------------------------------------------------------------------*\
|
|       Class CGameLevel
|
|  DESCRIPTION:
|
|
|
\*---------------------------------------------------------------------------*/

// this is ONLY for the Klown; it must match the corresponding entry in cgkrusty.cpp
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
} KLOWN_DATA;
extern int gGameMode;
CGameLevel::CGameLevel( 
    char *pFileName,
    TCHAR * pLevelName,
    CGameTimer* pTimer,
    CGameInput* pInput,
    CGameScreen* pScreen
 ) : mpGraphics( NULL ),
    mpTimer( pTimer ),
    mpInput( pInput ),
    mpScreen( pScreen ),
    mOffsetX( 0 ),
    mOffsetY( 0 ),
    mFrameTime( 0 ),
    mpGraphicsKey( NULL ),
    mFastKlown( FALSE ),
    mpProfile( NULL )
{
    char DllList[256];

    mMainKlown = NULL;
    // first scan internal table of characters:
    LoadCharInfo( NULL );
    // Grab info from the GAM file; look for character DLLs and load them...
    lstrcpy(DllList, "");
    GetPrivateProfileString("General", "DLLS", "", DllList, 255, pFileName);
    if (strlen(DllList) > 0)
    {
        CStringRecord crec(DllList, ",");
        int x;
        char    dlldir[260];
        char   *p;

        lstrcpy( dlldir, pFileName );
        p = strrchr( dlldir, '/' );
        if ( p == NULL )
            p = strrchr( dlldir, '\\' );
        if ( p != NULL )
            lstrcpy( p + 1, "*.dll" );


        for (x=0; x<crec.GetNumFields(); x++)
        {
            LoadMyDLL(dlldir, crec[x]);
        }
    }

    MatchProfile(pFileName);

    char prof[256];

    GetPrivateProfileString(
            pLevelName,
            mpProfile,
            pLevelName,     // default to level name
            prof,
            sizeof(prof),
            pFileName
            );

    pLevName = new char [lstrlen(prof)+1];
    lstrcpy(pLevName, prof);

    pFilName = new char [lstrlen(pFileName)+1];
    lstrcpy(pFilName, pFileName);

    // Load up any sound effects that are designated as preload.
    char    SoundList[255];
    GetPrivateProfileString(pLevName, "PreloadSounds", "", SoundList, sizeof(SoundList)
    , pFileName);
    if (strlen(SoundList) > 0)
    {
        CStringRecord crec(SoundList, ",");
        for (int x=0; x<crec.GetNumFields(); x++)
        {
            new CSoundEffect(crec[x], 0, FALSE, gSoundMode);
        }
    }

    // init
    mMaxWorldX = GetPrivateProfileInt(pLevName, "WorldX", SCREEN_HEIGHT, pFileName) / 2;
    mMaxWorldY = GetPrivateProfileInt(pLevName, "WorldY", SCREEN_WIDTH, pFileName) / 2;

    mOffsetX = GetPrivateProfileInt(pLevName, "StartX", 0, pFileName);
    mOffsetY = GetPrivateProfileInt(pLevName, "StartY", 0, pFileName);

    // now create the characters as needed
    SetCurrentLevel(this);

    {
        
        // set the screen's palette
        char paletteFile[256];
        GetPrivateProfileString(
            pLevName,
            "Palette",
            "",
            paletteFile,
            sizeof( paletteFile ),
            pFileName
            );
        pScreen->SetPalette( paletteFile );
    }   

    char graphicsName[256];
    GetPrivateProfileString(
        pLevName,
        "Graphics",
        "",
        graphicsName,
        sizeof( graphicsName ),
        pFileName
        );

    // keep a copy of the section name
    mpGraphicsKey = new char[lstrlen(graphicsName)+1];
    lstrcpy( mpGraphicsKey, graphicsName );

    mpGraphics = new CGameDisplayList(pFileName, graphicsName, this);

    // Add computer opponent(s), second klown, if needed:
    mGameType = gGameMode;
    mNumComputerKlowns =
        mGameType == 0 ?
            1 :
            0 ;
    memset(&mComputerKlowns[0], 0, sizeof(mComputerKlowns));
    GetPrivateProfileString("General", "RoboKlown", "", DllList, sizeof(DllList), pFileName);
    int posx, posy;
    mMainKlown->GetXY(&posx, &posy);
    for (int x=0; x<mNumComputerKlowns; x++)
    {
        // create new klown, computer generated
        mComputerKlowns[x] = new CGameCharacter(pFileName, DllList, graphicsName, this, 
            mMainKlown->GetMinZ(), 
            mMainKlown->GetMaxZ(), posx + mMainKlown->GetCurWidth() * 2, 
            posy, NULL);

        if (mComputerKlowns[x])
        {
            mpGraphics->Insert(mComputerKlowns[x]);
            KLOWN_DATA *data = (KLOWN_DATA *) mComputerKlowns[x]->mpPrivateData;
            if (data)
                data->type = 1; // computer opponent;
        }
    }

    // if playing other person on same machine, create opponent;
    if (mGameType == 1)
    {
        // create second klown
        GetPrivateProfileString("General", "SecondKlown", "", DllList, sizeof(DllList), pFileName);
        mSecondKlown = new CGameCharacter(pFileName, DllList, graphicsName, this, 
            mMainKlown->GetMinZ(), 
            mMainKlown->GetMaxZ(), posx + mMainKlown->GetCurWidth() * 2 , posy, NULL);

        if (mSecondKlown)
        {
            mpGraphics->Insert(mSecondKlown);
            KLOWN_DATA *data = (KLOWN_DATA *) mSecondKlown->mpPrivateData;
            if (data)
                data->type = 2; // second (human) opponent;         
        }

    }
    else
        mSecondKlown = NULL;

    mpUpdateList = new CGameUpdateList;
    mpUpdateList->AddRect(WholeScreen);
}

CGameCharacter * CGameLevel::Add (
                char *name, 
                int curz, 
                int curx, 
                int  cury,
                void *pNewObjID)
{
    CGameCharacter * newchar;

    if (mpGraphics == NULL)
        return(NULL);

    newchar = new CGameCharacter(pFilName, name, mpGraphicsKey, this, curz, curz, curx, cury, pNewObjID);
    if (newchar)
    {
        mpGraphics->Insert(newchar);
    }
    return(newchar);    
}

CGameLevel::~CGameLevel(  )
{
    delete[] mpProfile;
    delete mpGraphics;
    delete pLevName;
    delete pFilName;
    delete mpGraphicsKey;
    delete mpUpdateList;
}

BOOL gameover = FALSE;
BOOL quit = FALSE;
BOOL showing = FALSE;
BOOL showFrameRate = FALSE;
void CGameLevel::GameOver()
{
    gameover = TRUE;
}

void
CGameLevel::StopAnimating()
{
    showing = FALSE;
}

void    CGameLevel::Animate( 
            HWND hwndParent,
            CGameScreen * pScreen
 )
{
    SetCapture( hwndParent );   // so we get mouse clicks

    // turn off the cursor
    HCURSOR hOldCursor = SetCursor( NULL );

//  pScreen->SetMode( SCREEN_WIDTH, SCREEN_HEIGHT, 8 );

    showing = TRUE;
    MSG     msg;

    Timer->Time = timeGetTime(); // * 60 / 1000;

    UINT    lastTime = Timer->Time;
    mFrameTime = lastTime;
    UINT elapsed = 0;

#define DEBOUNCE_FRAMES 12
    static int debounceF2 = 0;
    static int debounceF3 = 0;
    static int debounceF5 = 0;
    static int debounceF9 = 0;

#define FRAMERATE

#define SCORE_WIDTH 64
#define SCORE_HEIGHT 64

    CGameDSBitBuffer* pScoreFrameBufferLeft;
    CGameDSBitBuffer* pScoreFrameBufferRight;

    HDC hdcWindow = GetDC(hwndParent);
    HDC hdcScoreLeft = CreateCompatibleDC(hdcWindow);
    HDC hdcScoreRight = CreateCompatibleDC(hdcWindow);
    ReleaseDC(hwndParent, hdcWindow);

    pScoreFrameBufferLeft = new CGameDSBitBuffer( SCORE_WIDTH, SCORE_HEIGHT);
    pScoreFrameBufferRight = new CGameDSBitBuffer( SCORE_WIDTH, SCORE_HEIGHT);
    SelectObject(hdcScoreLeft, pScoreFrameBufferLeft->GetHBitmap());
    SelectObject(hdcScoreRight, pScoreFrameBufferRight->GetHBitmap());
    SetBkMode(hdcScoreLeft, TRANSPARENT);
    SetBkMode(hdcScoreRight, TRANSPARENT);

    //    set up  our cool font
    LOGFONT logFont;
    HANDLE hFont;
    memset(&logFont, 0, sizeof(LOGFONT));
    logFont.lfHeight = SCORE_HEIGHT; //maxHeight;
    logFont.lfPitchAndFamily = FF_ROMAN;
    hFont = CreateFontIndirect(&logFont);
    SelectObject(hdcScoreLeft, hFont);
    SetTextColor(hdcScoreLeft, PALETTEINDEX(4));

    SelectObject(hdcScoreRight, hFont);
    SetTextColor(hdcScoreRight, PALETTEINDEX(4));

    int lastScoreLeft = -1;
    int lastScoreRight = -1;

#ifdef FRAMERATE
#define FR_WIDTH 32
#define FR_HEIGHT 32

    UINT frames = 0;
    UINT frameTime = timeGetTime();

    // create a memory bitmap for our frame text
    CGameDSBitBuffer* pFrameBuffer;

    HDC hdc = GetDC( hwndParent );
    HDC hdcFrame = CreateCompatibleDC( hdc );

    pFrameBuffer = new CGameDSBitBuffer( FR_WIDTH, FR_HEIGHT);
    SelectObject( hdcFrame, pFrameBuffer->GetHBitmap() );
    //    set up  our cool font
    LOGFONT logFont2;
    HANDLE hFont2;
    memset(&logFont2, 0, sizeof(LOGFONT));
    logFont2.lfHeight = FR_HEIGHT; //maxHeight;
    logFont2.lfPitchAndFamily = FF_ROMAN;
    hFont2 = CreateFontIndirect(&logFont2);
    SelectObject(hdcFrame, hFont2);
    SetTextColor(hdcFrame, COLOR_RED);
    SetBkMode(hdcFrame, TRANSPARENT);

    memset(pFrameBuffer->GetBits(), 1, FR_WIDTH * FR_HEIGHT);
    TextOut( hdcFrame, 0,0, "30", 2 );

    frames = 0;
#endif // FRAMERATE

    Timer->Resume();
    if (gMusicOn)
    {
        resumeMusic();
    }
#ifndef DEBUG
    HANDLE hprocess;
    hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
    SetPriorityClass(hprocess, HIGH_PRIORITY_CLASS );
    CloseHandle(hprocess);
#endif

    while ( showing ) {
        Timer->Time = timeGetTime();
        UINT    time = Timer->Time;

        if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE | PM_NOYIELD ) ) {
            TranslateMessage(&msg);

            if ((msg.message == WM_ACTIVATEAPP) &&
                (LOWORD(msg.wParam) == WA_INACTIVE))
            {
                showing = FALSE;
                DispatchMessage(&msg);
                continue;
            }
            else
                DispatchMessage(&msg);
        }

        if (mpInput->GetKeyboard(VK_ESCAPE) ||
            mpInput->GetKeyboard(VK_F12))
        {
            showing = FALSE;
            if (!mpInput->GetKeyboard(VK_CONTROL))
            {
                quit = TRUE;
            }
            continue;               
        }

        // toggles need to be debounced
        if (mpInput->GetKeyboard(VK_F2) && (debounceF2 == 0))
        {
            // mute!
            gMusicOn = !gMusicOn;
            if (gMusicOn)
            {
                resumeMusic();
            } else {
                pauseMusic();
            }
            debounceF2 = DEBOUNCE_FRAMES;
        }

        if (debounceF2)
            --debounceF2;

        if (mpInput->GetKeyboard(VK_F3) && (debounceF3 == 0))
        {
            extern BOOL gbQuiet;
            SetSilence(!gbQuiet);
            debounceF3 = DEBOUNCE_FRAMES;
        }
        if (debounceF3)
            --debounceF3;

        if (mpInput->GetKeyboard(VK_F9) && (debounceF9 == 0)) 
        {
            mFastKlown = !mFastKlown;
            debounceF9 = DEBOUNCE_FRAMES;
        }

        if (debounceF9)
            --debounceF9;

#ifdef FRAMERATE
        if (mpInput->GetKeyboard(VK_F5) && (debounceF5 == 0)) 
        {
            showFrameRate = !showFrameRate;
            debounceF5 = DEBOUNCE_FRAMES;
        }

        if (debounceF5)
            --debounceF5;
#endif

        lastTime = time;

        // If we are playing a network game, we
        // poll here synchronously for the remote network activity
        {
            extern void PollForRemoteReceive( void );

            if ( mGameType > 1 ) {
                PollForRemoteReceive();
            }
        }

        // !!! if we want to limit our framerate, put a check here for elapsed time
        {
            mFrameTime = time;

            // let each object update position, get input, etc
            mpGraphics->Update(this, mpUpdateList);

            // let each object render its graphical image
            mpGraphics->Render(this, pScreen, mpUpdateList);
            mpUpdateList->Clear();

            // get the correct score to show... there is always at least *one*
            KLOWN_DATA * pKlown;
            RECT score_rect;
            char    scorebuf[5];
            pKlown = (KLOWN_DATA *) mMainKlown->mpPrivateData;

            score_rect.left = 10;
            score_rect.right = 10+SCORE_WIDTH;
            score_rect.top = 10;
            score_rect.bottom = 10+SCORE_HEIGHT;
            mpUpdateList->AddRect(score_rect);

            if (pKlown->HitsLeft != lastScoreLeft)
            {
                lastScoreLeft = pKlown->HitsLeft;

                itoa(pKlown->HitsLeft, scorebuf, 10);

                memset(pScoreFrameBufferLeft->GetBits(), 1, SCORE_WIDTH * SCORE_HEIGHT);

                TextOut(hdcScoreLeft, 0, 0, scorebuf, lstrlen(scorebuf));
            }
            pScreen->TransRender(10,10,SCORE_WIDTH, SCORE_HEIGHT,
                pScoreFrameBufferLeft,0,0);     

            switch (mGameType)
            {
                case 0: // against computer
                    pKlown = (KLOWN_DATA *) mComputerKlowns[0]->mpPrivateData;
                    break;

                case 1:
                    pKlown = (KLOWN_DATA *) mSecondKlown->mpPrivateData;
                    break;

                default:
                    pKlown = NULL;
                    break;
            }
            if (pKlown != NULL) 
            {
                score_rect.left = SCREEN_WIDTH -10 - SCORE_WIDTH;
                score_rect.right = SCREEN_WIDTH -10;
                mpUpdateList->AddRect(score_rect);

                if (pKlown->HitsLeft != lastScoreRight)
                {
                    lastScoreRight = pKlown->HitsLeft;
                    itoa(pKlown->HitsLeft, scorebuf, 10);
                    memset(pScoreFrameBufferRight->GetBits(), 1,
                                        SCORE_WIDTH * SCORE_HEIGHT);
                    TextOut(hdcScoreRight, 0, 0, scorebuf, lstrlen(scorebuf));
                }
                pScreen->TransRender(SCREEN_WIDTH -10-SCORE_WIDTH,10,
                        SCORE_WIDTH, SCORE_HEIGHT,
                pScoreFrameBufferRight,0,0);
            }

#ifdef FRAMERATE
            if (showFrameRate)
            {
                ++frames;
                UINT newTime = timeGetTime();
                UINT dTime = newTime - frameTime;
                if (dTime >= 1000 )
                {
                    char buf[4];

                    memset(pFrameBuffer->GetBits(), 1, FR_WIDTH * FR_HEIGHT);
                    itoa( frames / (dTime / 1000) , buf, 10 );
                    TextOut( hdcFrame, 0,0, buf, lstrlen(buf) );

                    frames = 0;
                    frameTime = newTime;
                }
                pScreen->TransRender(
                        320 - (FR_WIDTH >> 1),
                        10,
                        FR_WIDTH,
                        FR_HEIGHT,
                        pFrameBuffer,
                        0,
                        0
                        );
            }
            else
            {
                // keep frametime updated
                frameTime = time;
            }
#endif // FRAMERATE

            // update the screen
            pScreen->PageFlip();
            mpInput->UpdateJoystick();

            if (gameover)
            {
                RECT rect;
                char tempstring[20];
                pKlown = (KLOWN_DATA *) mMainKlown->mpPrivateData;

                Sleep(2000);

                GetPrivateProfileString( pLevName, "WinLose", "end.bmp", tempstring, 19, pFilName);

                CGameDIB * myDib = new CGameDIB(tempstring);
                CGameDSBitBuffer * myBuf = new CGameDSBitBuffer(myDib);

                if (myBuf)
                {
                    RECT rectdtop;

                    HDC hdcWindow = GetDC(hwndParent);
                    HDC hdc = CreateCompatibleDC(hdcWindow);
                    ReleaseDC(hwndParent, hdcWindow);

                    rectdtop.left = 0;
                    rectdtop.right = SCREEN_WIDTH;
                    rectdtop.top = 0;
                    rectdtop.bottom = SCREEN_HEIGHT;
                    mpUpdateList->AddRect(rectdtop);

                    SelectObject(hdc, myBuf->GetHBitmap());
                    // make the gameover bitmap fill current screen
                    rect = rectdtop;

                    CGameText * pCtext = new CGameText (hdc, &rect, 12,1);

                    if (pCtext)
                    {
                        int ix, ixend;
                        pKlown = (KLOWN_DATA *) mMainKlown->mpPrivateData;
                        pKlown->curState = 0;

                        ix = pKlown->IGotKilled ? IDS_LOSE_START : IDS_WIN_START;
                        ixend = pKlown->IGotKilled ? IDS_LOSE_END : IDS_WIN_END;
                        // load strings...
                        while (ix <= ixend)
                        {
                            char *pChoice = NewStringResource(ghInst, ix);
                            pCtext->AddLine(pChoice, COLOR_YELLOW);
                            ++ix;
                        }

                        // Overlay text on bitmap
                        pCtext->TextBlt();
                        delete pCtext;
                    }

                    //
                    // Display option screen!
                    pScreen->Render(
                        0,
                        0,
                        SCREEN_WIDTH,
                        SCREEN_HEIGHT,
                        myBuf,
                        0,
                        0,
                        SRCCOPY
                        );

                    pScreen->PageFlip();
                    Sleep(5000);
                    pScreen->PageFlip();
                    DeleteDC(hdc);
                    delete myBuf;
                }
                delete myDib;

                // reset igotkilled flag...
                pKlown->IGotKilled = 0;
                if (mComputerKlowns[0])
                {
                    pKlown = (KLOWN_DATA *) mComputerKlowns[0]->mpPrivateData;
                    if (pKlown) 
                    {
                        pKlown->curState = 0;                           
                        pKlown->IGotKilled = 0;
                    }

                }
                if (mSecondKlown)
                {
                    pKlown = (KLOWN_DATA *) mSecondKlown->mpPrivateData;
                    if (pKlown) 
                    {
                        pKlown->curState = 0;                           
                        pKlown->IGotKilled = 0;
                    }
                }

                gameover = FALSE;
                lastScoreLeft = lastScoreRight = -1;
            } // if (gameover)
        } // limit block
    } // while

    Timer->Pause();
    SetCursor( hOldCursor );
    ReleaseCapture(  );

    if (gMusicOn)
    {
        pauseMusic();
    }
#ifndef DEBUG
    hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
    SetPriorityClass(hprocess, IDLE_PRIORITY_CLASS );
    CloseHandle(hprocess);
#endif

#ifdef FRAMERATE
    DeleteDC( hdcFrame );
    DeleteObject(hFont2);
    delete pFrameBuffer;
#endif

    DeleteDC( hdcScoreLeft );
    DeleteDC( hdcScoreRight );

    DeleteObject(hFont);
    delete pScoreFrameBufferLeft;
    delete pScoreFrameBufferRight;

    ReleaseDC( hwndParent, hdc );

    if (quit)
        PostQuitMessage(0);
}

void CGameLevel::ForceOnScreen(int *x, int *y, int wide, int high, BOOL primary)
{
    // if the x or y coords are off screen:
    // 1) force screen to be there - unless
    // 2) the coords are impossible, then adjust them!

#define ROUGH_WIDTH 96
    // keep main char in center of screen
    static RECT center = 
    {
        SCREEN_WIDTH / 4,
        0,
        SCREEN_WIDTH - (SCREEN_WIDTH / 4) - ROUGH_WIDTH,
        SCREEN_HEIGHT

    };

    if (*x < -mMaxWorldX)
        *x = -mMaxWorldX;
    else
    {
        if (*x > (mMaxWorldX-wide))
            *x = mMaxWorldX-wide;
    }

    if (*y < -mMaxWorldY)
        *y = -mMaxWorldY;
    else
    {
        if (*y > (mMaxWorldY-wide))
            *y = mMaxWorldY-wide;
    }

    if (primary)
    {
        if ((*x < Screen2WorldX(center.left)) && (mOffsetX > -mMaxWorldX))
        {
            mOffsetX = max(-mMaxWorldX, mOffsetX - (Screen2WorldX(center.left) - *x));
            mpUpdateList->AddRect(WholeScreen);
        }
        else
        {
            if ((*x > Screen2WorldX(center.right)) && (mOffsetX < (mMaxWorldX-SCREEN_WIDTH)))
            {
                mOffsetX = min((mMaxWorldX-SCREEN_WIDTH), mOffsetX + (*x - Screen2WorldX(center.right)));           
                mpUpdateList->AddRect(WholeScreen);
            }
        }

        if ((*y < Screen2WorldY(center.top)) && (mOffsetY > -mMaxWorldY))
        {       
            mOffsetY = max(-mMaxWorldY, mOffsetY - (Screen2WorldY(center.top) - *y));
            mpUpdateList->AddRect(WholeScreen);
        }
        else
        {
            if ((*y > Screen2WorldY(center.bottom)) && (mOffsetY < (mMaxWorldY-SCREEN_HEIGHT)))
            {           
                mOffsetY = min((mMaxWorldY-SCREEN_HEIGHT), mOffsetY + (*y - Screen2WorldY(center.bottom)));
                mpUpdateList->AddRect(WholeScreen);
            }
        }       
    }
}

void
CGameLevel::MatchProfile(
    char* pGamFile
    )
{
    static char* pDefault = "Default";
    char profBuf[256];
    char dataBuf[256];

    // first check to see if we need to force a given profile
    GetPrivateProfileString(
        "General",
        "ForceProfile",
        "",     // no default
        profBuf,
        sizeof( profBuf ),
        pGamFile
        );

    if (lstrlen(profBuf) >0)
    {
         mpProfile = new char[lstrlen(profBuf)+1];
         lstrcpy( mpProfile, profBuf );
    }
    else
    {
        GetPrivateProfileString(
            "Profiles",
            NULL,   // get all
            "",     // no default
            profBuf,
            sizeof( profBuf ),
            pGamFile
            );


        for (char *pChar = profBuf; *pChar; pChar++)
        {
            GetPrivateProfileString(
                "Profiles",
                pChar,
                "",
                dataBuf,
                sizeof( dataBuf ),
                pGamFile
                );

            // parse the data string into fields
            CStringRecord fields( dataBuf, "," );

            MC_PROCESSOR processor = (MC_PROCESSOR) atoi(fields[0]);
            MC_BUSTYPE bus = (MC_BUSTYPE) atoi(fields[1]);

            // memory fields are in megabytes
            DWORD sysMem = atoi(fields[2]) * 1024 * 1024;
            DWORD vidMem = atoi(fields[3]) * 1024 * 1024;

            MC_VIDSYS vidSys = (MC_VIDSYS) atoi(fields[4]);

            if ((gMachineCaps.processor >= processor) &&
                (gMachineCaps.bus >= bus) &&
                (gMachineCaps.sysMemory >= sysMem) &&
                (gMachineCaps.vidMemory >= vidMem) &&
                (gMachineCaps.vidSystem >= vidSys))
            {
                mpProfile = new char[lstrlen(pChar)+1];
                lstrcpy( mpProfile, pChar );
                break;
            }

            pChar += lstrlen( pChar );  // move beyond terminator
        }

        // if we didn't find it, use default
        if (mpProfile == NULL)
        {
            mpProfile = new char[lstrlen(pDefault)+1];
            lstrcpy( mpProfile, pDefault );
        }
    }
}
