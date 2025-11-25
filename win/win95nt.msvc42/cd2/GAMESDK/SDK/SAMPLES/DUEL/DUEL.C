/*==========================================================================
 *
 *  Copyright (C) 1995-1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       duel.c
 *  Content:    Multi-player duel
 *
 *
 ***************************************************************************/
#include "duel.h"

// Duel 09438c20-e06a-11ce-8681-00aa006c5d58
DEFINE_GUID(DUEL_GUID,0x09438c20,0xe06a,0x11CE,0x86,0x81,0x00,0xaa,0x00,0x6c,0x5d,0x58);

LPDIRECTPLAY            lpIDC=NULL;     // DirectPlay Object
DPID                    dcoID=0;        // our DirectPlay ID
LPGUID                  g_lpGuid;
HANDLE                  dphEvent = NULL;

LPDIRECTDRAWSURFACE     lpFrontBuffer;
LPDIRECTDRAWSURFACE     lpBackBuffer;
LPDIRECTDRAWSURFACE     lpShip0;
LPDIRECTDRAWSURFACE     lpShip1;
LPDIRECTDRAWSURFACE     lpShip2;
LPDIRECTDRAWSURFACE     lpShip3;
LPDIRECTDRAWSURFACE     lpNum;
LPDIRECTDRAW            lpDD;
LPDIRECTDRAWPALETTE     lpArtPalette;
LPDIRECTDRAWPALETTE     lpSplashPalette;
int                     showDelay = 0;
HWND                    hWndMain;
BOOL                    bShowFrameCount=TRUE;
BOOL                    bIsActive;
DWORD                   dwFrameCount;
DWORD                   dwFrameTime;
DWORD                   dwFrames;
DWORD                   dwFramesLast;
BOOL                    bUseEmulation;
RGBQUAD                 SPalette[256];
int                     ProgramState;
int                     level;
int                     restCount;
DWORD                   dwFillColor;
BOOL                    IsHost;
BOOL                    HaveHostInit;
BYTE                    WhoIAm;
#ifdef DEBUG
char                    DebugBuf[256];
BOOL                    bHELBlt = FALSE;
#endif

BLOCKS                  Blocks;
GLOBALSHIP              Ships[16];
DWORD                   Keys;
BYTE                    CommBuff[256];
int                     CountDown=0;
FRAG                    Frags[64];

/*
 * MainWndproc
 *
 * Callback for all Windows messages
 */
long FAR PASCAL MainWndproc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC         hdc;

    switch( message )
    {
    case WM_ACTIVATEAPP:
        bIsActive = (BOOL) wParam;
        break;

    case WM_CREATE:
        break;

    case WM_SETCURSOR:
        SetCursor(NULL);
        return TRUE;

    case WM_KEYDOWN:
        switch( wParam )
        {
        case VK_NUMPAD5:
            Keys |= KEY_STOP;
            break;
        case VK_DOWN:
        case VK_NUMPAD2:
            Keys |= KEY_DOWN;
            break;
        case VK_LEFT:
        case VK_NUMPAD4:
            Keys |= KEY_LEFT;
            break;
        case VK_RIGHT:
        case VK_NUMPAD6:
            Keys |= KEY_RIGHT;
            break;
        case VK_UP:
        case VK_NUMPAD8:
            Keys |= KEY_UP;
            break;
        case VK_SPACE:
            Keys |= KEY_FIRE;
            break;
        case VK_F5:
            bShowFrameCount = !bShowFrameCount;
            if( bShowFrameCount )
            {
                dwFrameCount = 0;
                dwFrameTime = timeGetTime();
            }
            break;
        case VK_RETURN:
            if( ProgramState == PS_SPLASH )
            {
                lpDD->lpVtbl->FlipToGDISurface( lpDD );
                if ( RemoteCreate(DUEL_GUID, "Duel", "Ship") )
                {
                    ProgramState = PS_REST;
                    // set the palette
                    if( !IsHost )
                    {
                        SendGameMessage(MSG_HEREIAM, 0, 0, 0, 0);
                        HaveHostInit = FALSE;
                    }
                    else
                    {
                        HaveHostInit = TRUE;
                        WhoIAm = 0;
                        InitGame();
                    }
                }
            }
            break;
        case VK_ESCAPE:
        case VK_F12:
            PostMessage( hWnd, WM_CLOSE, 0, 0 );
            return 0;
        }
        break;

    case WM_KEYUP:
        switch( wParam )
        {
        case VK_NUMPAD5:
            Keys &= ~KEY_STOP;
            break;
        case VK_DOWN:
        case VK_NUMPAD2:
            Keys &= ~KEY_DOWN;
            break;
        case VK_LEFT:
        case VK_NUMPAD4:
            Keys &= ~KEY_LEFT;
            break;
        case VK_RIGHT:
        case VK_NUMPAD6:
            Keys &= ~KEY_RIGHT;
            break;
        case VK_UP:
        case VK_NUMPAD8:
            Keys &= ~KEY_UP;
            break;
        case VK_SPACE:
            Keys &= ~KEY_FIRE;
            break;
        }
        break;

    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT:
        hdc = BeginPaint( hWnd, &ps );
        EndPaint( hWnd, &ps );
        return 1;

    case WM_DESTROY:
        DestroyGame();
        PostQuitMessage( 0 );
        break;

    default:
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);

} /* MainWndproc */

/*
 * initApplication
 *
 * Do that Windows initialization stuff...
 */
static BOOL initApplication( HINSTANCE hInstance, int nCmdShow )
{
    WNDCLASS    wc;
    BOOL        rc;

    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = MainWndproc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( hInstance, "DUEL_ICON");
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockObject( BLACK_BRUSH );
    wc.lpszMenuName =  NULL;
    wc.lpszClassName = "DuelClass";
    rc = RegisterClass( &wc );
    if( !rc )
    {
        return FALSE;
    }

    hWndMain = CreateWindowEx(0,  // WS_EX_TOPMOST,
        "DuelClass",
        "Duel",
        WS_VISIBLE | // so we don't have to call ShowWindow
        WS_POPUP |   // non-app window POPUP
        WS_SYSMENU,  // so we get an icon in the tray
        0,
        0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        NULL,
        NULL,
        hInstance,
        NULL );

    if( !hWndMain )
    {
        return FALSE;
    }

    UpdateWindow( hWndMain );

    return TRUE;

} /* initApplication */

/*
 * WinMain
 */
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
                        int nCmdShow )
{
    MSG     msg;

    if( lpCmdLine[0] == '-' )
    {
        bUseEmulation = TRUE;
    }

    if( !initApplication(hInstance, nCmdShow) )
    {
        return FALSE;
    }

    if( !InitializeGame() )
    {
        DestroyWindow( hWndMain );
        return FALSE;
    }
 
    dwFrameTime = timeGetTime();

    while( 1 )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
        {
            if( !GetMessage( &msg, NULL, 0, 0 ) )
            {
                return msg.wParam;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            ReceiveGameMessages();
            if( bIsActive )
            {
                UpdateFrame();
            }
        }
    }
} /* WinMain */


void DestroyGame( void )
{
    if( dcoID != 0 )
    {
        lpIDC->lpVtbl->DestroyPlayer(lpIDC, dcoID);
    }
}

BOOL InitializeGame( void )
{
    DDCAPS          ddcaps;
    HRESULT         ddrval;
    DDSURFACEDESC   ddsd;
    DDSCAPS         ddscaps;

    if( bUseEmulation )
        ddrval = DirectDrawCreate( (LPVOID) DDCREATE_EMULATIONONLY, &lpDD, NULL );
    else
        ddrval = DirectDrawCreate( NULL, &lpDD, NULL );

    if( ddrval != DD_OK )
        return CleanupAndExit(1);

    ddrval = lpDD->lpVtbl->SetCooperativeLevel( lpDD, hWndMain,
                            DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
    if( ddrval != DD_OK )
        return CleanupAndExit(2);

    // set the mode to 640 by 480 by 8
#ifdef DEBUG
    if( GetProfileInt( "Duel", "force_16_bit", 0) )
    {
        ddrval = lpDD->lpVtbl->SetDisplayMode( lpDD, 640, 480, 16 );
    }
    else
#endif
    {
        ddrval = lpDD->lpVtbl->SetDisplayMode( lpDD, 640, 480, 8 );
    }
    if( ddrval != DD_OK )
        return CleanupAndExit(3);

    // check the color key hardware capabilities
    ddcaps.dwSize = sizeof( ddcaps );

#ifdef DEBUG
    bHELBlt = GetProfileInt( "Duel", "force_HEL_blt", bHELBlt );
#endif

    // Create surfaces
    memset( &ddsd, 0, sizeof( ddsd ) );
    ddsd.dwSize = sizeof( ddsd );
    ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
                          DDSCAPS_FLIP |
                          DDSCAPS_COMPLEX;
    ddsd.dwBackBufferCount = 1;
    ddrval = lpDD->lpVtbl->CreateSurface( lpDD, &ddsd, &lpFrontBuffer, NULL );
    if( ddrval != DD_OK )
        return CleanupAndExit(4);

    // get a pointer to the back buffer
    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
    ddrval = lpFrontBuffer->lpVtbl->GetAttachedSurface(
                lpFrontBuffer,
                &ddscaps,
                &lpBackBuffer );
    if( ddrval != DD_OK )
        return CleanupAndExit(5);

    ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;        
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
#ifdef DEBUG
    if( bHELBlt )
        ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
#endif

    ddsd.dwWidth = 320;
    ddsd.dwHeight = 128;
    ddrval = lpDD->lpVtbl->CreateSurface( lpDD, &ddsd, &lpShip0, NULL );
    if( ddrval != DD_OK )
        return CleanupAndExit(6);

    ddsd.dwWidth = 320;
    ddsd.dwHeight = 128;
    ddrval = lpDD->lpVtbl->CreateSurface( lpDD, &ddsd, &lpShip1, NULL );
    if( ddrval != DD_OK )
        return CleanupAndExit(7);

    ddsd.dwWidth = 320;
    ddsd.dwHeight = 128;
    ddrval = lpDD->lpVtbl->CreateSurface( lpDD, &ddsd, &lpShip2, NULL );
    if( ddrval != DD_OK )
        return CleanupAndExit(8);

    ddsd.dwWidth = 320;
    ddsd.dwHeight = 128;
    ddrval = lpDD->lpVtbl->CreateSurface( lpDD, &ddsd, &lpShip3, NULL );
    if( ddrval != DD_OK )
        return CleanupAndExit(9);

    ddsd.dwHeight = 16;
    ddrval = lpDD->lpVtbl->CreateSurface( lpDD, &ddsd, &lpNum, NULL );
    if( ddrval != DD_OK )
        return CleanupAndExit(10);

    if( !RestoreSurfaces() )
        return CleanupAndExit(11);
        
    Keys = 0;
    ProgramState = PS_SPLASH;
    return TRUE;
}

BOOL CleanupAndExit( int err)
{
    char buf[256];
    
#ifdef DEBUG
    wsprintf(DebugBuf, "___CleanupAndExit  err = %d\n", err );
    OutputDebugString( DebugBuf );
#endif

    if( lpShip0 != NULL )
        lpShip0->lpVtbl->Release( lpShip0 );
        
    if( lpShip1 != NULL )
        lpShip1->lpVtbl->Release( lpShip1 );
        
    if( lpShip2 != NULL )
        lpShip2->lpVtbl->Release( lpShip2 );
        
    if( lpShip3 != NULL )
        lpShip3->lpVtbl->Release( lpShip3 );
        
    if( lpNum != NULL )
        lpNum->lpVtbl->Release( lpNum );
        
    if( lpFrontBuffer != NULL )
        lpFrontBuffer->lpVtbl->Release( lpFrontBuffer );
        
    if( lpArtPalette != NULL )
        lpArtPalette->lpVtbl->Release( lpArtPalette );
        
    if( lpSplashPalette != NULL )
        lpSplashPalette->lpVtbl->Release( lpSplashPalette );
        
    if( lpDD != NULL )
        lpDD->lpVtbl->Release( lpDD );

    wsprintf(buf, "Game could not start (%d)", err);
    MessageBox( hWndMain, buf, "ERROR", MB_OK );
    return FALSE;
}

void bltSplash( LPRECT rc)
{
    HRESULT     ddrval;
    HBITMAP     hbm;

    if( ( lpFrontBuffer == NULL ) ||
        ( lpSplashPalette == NULL ) ||
        ( lpBackBuffer == NULL ) )
    {
        return;
    }

    // set the palette before loading the splash screen
    lpFrontBuffer->lpVtbl->SetPalette( lpFrontBuffer, lpSplashPalette );

    hbm = (HBITMAP)LoadImage( GetModuleHandle( NULL ), "SPLASH", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION );
    if ( NULL == hbm )
        return;
        
    // if the surface is lost, DDCopyBitmap will fail and the surface will
    // be restored below.
    ddrval = DDCopyBitmap( lpBackBuffer, hbm, 0, 0, 0, 0 );

    DeleteObject( hbm );

    while( 1 )
    {
        if( rc != NULL )
        {
            ddrval = lpFrontBuffer->lpVtbl->BltFast( lpFrontBuffer, 
                rc->left, rc->top, lpBackBuffer, rc, 0);
        }
        else
        {
            ddrval = lpFrontBuffer->lpVtbl->BltFast( lpFrontBuffer, 
                0, 0, lpBackBuffer, NULL, 0);
        }
        if( ddrval == DD_OK )
            return;
        if( ddrval == DDERR_SURFACELOST )
            if( !RestoreSurfaces() )
                return;
        if( ddrval != DDERR_WASSTILLDRAWING )
            return;
    }
}

void UpdateFrame( void )
{
    int i;
    
    switch( ProgramState )
    {
        case PS_SPLASH:
            // display the splash screen
            bltSplash(NULL);
            return;
        case PS_ACTIVE:
            ReadKey();
            for(i=0; i<16; i++)
                UpdatePosition(&Ships[i]);
            for(i=0; i<64; i++)
                UpdateFragment(i);
            SendGameMessage(MSG_UPDATE, 0, 0, 0, 0);
            DrawScreen();
            return;
        case PS_REST:
            if( HaveHostInit )
            {
                lpFrontBuffer->lpVtbl->SetPalette( lpFrontBuffer, lpArtPalette );
                ProgramState = PS_ACTIVE;
            }
            break;
    }
}

void ReadKey( void )
{
    if( Keys & KEY_LEFT )
    {
        Ships[WhoIAm].frame -= 0.5;
        if( Ships[WhoIAm].frame < 0.0 )
            Ships[WhoIAm].frame += MAX_SHIP_FRAME;
    }
    if( Keys & KEY_RIGHT )
    {
        Ships[WhoIAm].frame += 0.5;
        if( Ships[WhoIAm].frame >= MAX_SHIP_FRAME )
            Ships[WhoIAm].frame -= MAX_SHIP_FRAME;
    }
    if( Keys & KEY_UP )
    {
        Ships[WhoIAm].velx += Dirx[(int)Ships[WhoIAm].frame] * 10.0 / 1000.0;
        Ships[WhoIAm].vely += Diry[(int)Ships[WhoIAm].frame] * 10.0 / 1000.0;
    }
    if( Keys & KEY_DOWN )
    {
        Ships[WhoIAm].velx -= Dirx[(int)Ships[WhoIAm].frame] * 10.0 / 1000.0;
        Ships[WhoIAm].vely -= Diry[(int)Ships[WhoIAm].frame] * 10.0 / 1000.0;
    }
    if( Keys & KEY_STOP )
    {
        Ships[WhoIAm].velx = 0.0;
        Ships[WhoIAm].vely = 0.0;
    }
    if( !Ships[WhoIAm].benable && Ships[WhoIAm].enable )
    {
        if( Keys & KEY_FIRE )
        {
            // launch a new bullet
            Ships[WhoIAm].bposx = Dirx[(int)Ships[WhoIAm].frame]*6.0 + 16.0 + Ships[WhoIAm].posx;
            Ships[WhoIAm].bposy = Diry[(int)Ships[WhoIAm].frame]*6.0 + 16.0 + Ships[WhoIAm].posy;
            Ships[WhoIAm].bvelx = Dirx[(int)Ships[WhoIAm].frame]*500.0/1000.0;
            Ships[WhoIAm].bvely = Diry[(int)Ships[WhoIAm].frame]*500.0/1000.0;
            Ships[WhoIAm].benable = TRUE;
            Ships[WhoIAm].bframe = 0;
        }
    }
}

void UpdatePosition( LPGLOBALSHIP ship )
{
    int     x,y,j;
    BYTE    mask, col, row, oldxCell, oldyCell, newxCell, newyCell;
    double  thisTick, totalTick, xtick, ytick;
    DWORD   TickCnt;
    DWORD   tickDiff;
    LPGLOBALSHIP  target;
    
    TickCnt = GetTickCount();
    tickDiff = TickCnt - ship->lastTick;
    ship->lastTick = TickCnt;
        
    if( ship == &(Ships[WhoIAm]) && !ship->enable)
    {
        CountDown -= tickDiff;
        if( CountDown < 0 )
        {
            Ships[WhoIAm].enable = TRUE;
        }
    }

    if( !ship->enable )
        return;
        
    if( ( (TickCnt - ship->timeStamp ) > 2000 ) &&
        ( ship != &(Ships[WhoIAm]) ) )
    {
        ship->enable = FALSE;
        return;
    }
        
    
    // incrementally update the position of the ship
    // find out the number of ticks needed to move 8 pixels
    if( ship->velx != 0.0 )
        xtick = 8.0/ship->velx;
    else
        xtick = 999999.0;

    if( ship->vely != 0.0 )
        ytick = 8.0/ship->vely;
    else
        ytick = 999999.0;

    if( xtick < 0.0 )
        xtick = -xtick;
    if( ytick < 0.0 )
        ytick = -ytick;

    if( xtick < ytick )
        thisTick = xtick;
    else
        thisTick = ytick;
        
    if( thisTick > tickDiff )
        thisTick = tickDiff;
                
    for( totalTick = 0.0; totalTick < tickDiff; )
    {
        totalTick += tickDiff;
        oldxCell = (int)(ship->posx+16.0) >> 4;
        oldyCell = (int)(ship->posy+16.0) >> 4;
        ship->posx += ship->velx * thisTick;
        ship->posy += ship->vely * thisTick;
        newxCell = (int)(ship->posx+16.0) >> 4;
        newyCell = (int)(ship->posy+16.0) >> 4;
        if(oldxCell != newxCell)
        {
            if( IsHit( newxCell, newyCell ) )
            {
                if( ship->velx > 0.0 )
                    ship->posx = (oldxCell << 4) + 15 - 16;
                else
                    ship->posx = (oldxCell << 4) - 16;
                ship->velx = -ship->velx*0.9;
                newxCell = oldxCell;
            }
        }
        if(oldyCell != newyCell)
        {
            if( IsHit( newxCell, newyCell ) )
            {
                if( ship->vely > 0.0 )
                    ship->posy = (oldyCell << 4) + 15 - 16;
                else
                    ship->posy = (oldyCell << 4) - 16;
                ship->vely = -ship->vely*0.9;
            }
        }
        if( ship->posx > MAX_SHIP_X )
        {
            ship->posx = MAX_SHIP_X;
            ship->velx = -ship->velx*0.9;
        }
        else if ( ship->posx < 0 )
        {
            ship->posx =0;
            ship->velx = -ship->velx*0.9;
        }
        if( ship->posy > MAX_SHIP_Y )
        {
            ship->posy = MAX_SHIP_Y;
            ship->vely = -ship->vely*0.9;
        }
        else if ( ship->posy < 0 )
        {
            ship->posy =0;
            ship->vely = -ship->vely*0.9;
        }
    }

    if( !ship->benable )
        return;

    // update the active bullet
    ship->bframe += tickDiff;
    if( ship->bframe >= MAX_BULLET_FRAME )
    {
        ship->benable = FALSE;
        return;
    }

    if( ship->bvelx != 0.0 )
        xtick = 8.0/ship->bvelx;
    else
        xtick = 999999.0;

    if( ship->bvely != 0.0 )
        ytick = 8.0/ship->bvely;
    else
        ytick = 999999.0;

    if( xtick < 0.0 )
        xtick = -xtick;
    if( ytick < 0.0 )
        ytick = -ytick;

    if( xtick < ytick )
        thisTick = xtick;
    else
        thisTick = ytick;
        
    if( thisTick > tickDiff )
        thisTick = tickDiff;
                
    for( totalTick = 0.0; totalTick < tickDiff; )
    {
        totalTick += thisTick;

        ship->bposx += ship->bvelx * thisTick;
        ship->bposy += ship->bvely * thisTick;
        
        // see if it hit a ship
        for(j=0; j<16; j++)
        {
            target = &(Ships[j]);
            if( ( target == ship ) || !target->enable )
                continue;
            if( (ship->bposx > target->posx) &&
                (ship->bposx < (target->posx + 32.0) ) &&
                (ship->bposy > target->posy) &&
                (ship->bposy < (target->posy + 32.0) ) )
            {
                SendGameMessage(MSG_SHIPHIT, 0, (BYTE)j, 0, 0);
                DestroyShip( j );
                target->enable = FALSE;
                ship->benable = FALSE;
                ship->score += 1000;
                return;
            }
        }
    
        if( ship->bposx > MAX_BULLET_X )
        {
            ship->bposx = MAX_BULLET_X;
            ship->bvelx = -ship->bvelx*0.9;
        }
        else if ( ship->bposx < 0 )
        {
            ship->bposx =0;
            ship->bvelx = -ship->bvelx*0.9;
        }
        if( ship->bposy > MAX_BULLET_Y )
        {
            ship->bposy = MAX_BULLET_Y;
            ship->bvely = -ship->bvely*0.9;
        }
        else if ( ship->bposy < 0 )
        {
            ship->bposy =0;
            ship->bvely = -ship->bvely*0.9;
        }
    
        // check to see if it hit anything
        x = (int)(ship->bposx + 0.5) + 1;
        y = (int)(ship->bposy + 0.5) + 1;
        
        row = y >> 4;
        col = x >> 4;
        mask = 1 << (col & 0x7);
        col = col >> 3;
        if( Blocks.bits[row][col] & mask )
        {
            // scored a block hit
            SendGameMessage(MSG_BLOCKHIT, 0, row, col, mask);
            Blocks.bits[row][col] &= ~mask;
            ship->score += 10;
            ship->benable = FALSE;
            return;
        }
    }
}

BOOL IsHit( int x, int y )
{
    int col, mask;
    
    // outside screen boundaries?
    if( (x < 0) || (y < 0) || (x >= 40) || (y >= 30) )
        return TRUE;
        
    // look at the block bits
    mask = 1 << (x & 0x7);
    col = x >> 3;
    if( Blocks.bits[y][col] & mask )
        return TRUE;
    else
        return FALSE;
}

void DrawScreen( void )
{
    int     i;
    BYTE    mask, col;
    int     x, y;

    EraseScreen();
    
    for(i=0; i<16; i++)
    {
        if( Ships[i].benable )
        {
            DrawBullet( i );
        }
    }
    
    for(i=0; i<16; i++)
    {
        if( Ships[i].enable )
        {
            DrawShip( i );
        }
    }

    for( y=0; y<30; y++)
    {
        for( x=0; x<40; x++)
        {
            mask = 1 << (x & 0x7);
            col = x >> 3;
            if( Blocks.bits[y][col] & mask )
                DrawBlock( x, y );
        }
    }
    
    DrawScore();

    DrawFragments();    
    if( bShowFrameCount )
        DisplayFrameRate();

    FlipScreen();
}

void DrawScore( void )
{
    char        scorebuf[11];
    int         rem;

    // blt everything in reverse order if we are doing destination transparency
    // calculate score string
    scorebuf[0] = Ships[WhoIAm].score/100000 + '0';
    rem = Ships[WhoIAm].score % 100000;
    scorebuf[1] = rem/10000 + '0';
    rem = Ships[WhoIAm].score % 10000;
    scorebuf[2] = rem/1000 + '0';
    rem = Ships[WhoIAm].score % 1000;
    scorebuf[3] = rem/100 + '0';
    rem = Ships[WhoIAm].score % 100;
    scorebuf[4] = rem/10 + '0';
    rem = Ships[WhoIAm].score % 10;
    scorebuf[5] = rem + '0';
    scorebuf[6] = '\0';

    bltScore(scorebuf, 8, 8);
}

void DrawBlock( int x, int y )
{
    RECT    src;
    
    src.top = 0;
    src.left = 224;
    src.right = src.left + 16;
    src.bottom = src.top + 16;
    bltObject( x << 4, y << 4, lpNum, &src, DDBLTFAST_SRCCOLORKEY );
}

void DrawShip( int i )
{
    RECT    src;
    LPDIRECTDRAWSURFACE surf;
    
    src.top = 32 * ( (int)Ships[i].frame / 10 );
    src.left = 32 * ( (int)Ships[i].frame % 10 );
    src.right = src.left + 32;
    src.bottom = src.top + 32;
    switch( i % 4 )
    {
    case 0: surf = lpShip0; break;
    case 1: surf = lpShip1; break;
    case 2: surf = lpShip2; break;
    case 3: surf = lpShip3; break;
    }
    bltObject( (int)Ships[i].posx, (int)Ships[i].posy, surf, &src, DDBLTFAST_SRCCOLORKEY );
}

void DrawBullet( int i )
{
    RECT    src;
    
    src.top = BULLET_Y;
    src.left = BULLET_X + (i%4)*4;
    src.right = src.left + 3;
    src.bottom = src.top + 3;
    bltObject( (int)Ships[i].bposx, (int)Ships[i].bposy, lpNum, &src, DDBLTFAST_SRCCOLORKEY );
}

void bltScore( char *num, int x, int y )
{
    char *c;
    RECT    src;
    int     i;

    for(c=num; *c != '\0'; c++)
    {
        i = *c - '0';
        src.left = i*16;
        src.top = 0;
        src.right = src.left + 16;
        src.bottom = src.top + 16;
        bltObject( x, y, lpNum, &src, DDBLTFAST_SRCCOLORKEY );
        x += 16;
    }
}

void bltObject( int x, int y, LPDIRECTDRAWSURFACE surf, LPRECT src, DWORD flags )
{
    HRESULT ddrval;

    while( 1 )
    {
        ddrval = lpBackBuffer->lpVtbl->BltFast( lpBackBuffer, x, y, surf, src, flags );
        if( ddrval == DD_OK )
            return;
        if( ddrval == DDERR_SURFACELOST )
            if( !RestoreSurfaces() )
                return;
        if( ddrval != DDERR_WASSTILLDRAWING )
            return;
    }
}

void EraseScreen( void )
{
    DDBLTFX     ddbltfx;
    HRESULT     ddrval;

    // Erase the background
    ddbltfx.dwSize = sizeof( ddbltfx );
    ddbltfx.dwFillColor = dwFillColor;
    while( 1 )
    {
        ddrval = lpBackBuffer->lpVtbl->Blt( lpBackBuffer, NULL, NULL,
                 NULL, DDBLT_COLORFILL, &ddbltfx );

        if( ddrval == DD_OK )
        {
            break;
        }
        if( ddrval == DDERR_SURFACELOST )
        {
            if( !RestoreSurfaces() )
                return;
        }
        if( ddrval != DDERR_WASSTILLDRAWING )
        {
            return;
        }
    }
}

void FlipScreen( void )
{
    HRESULT     ddrval;

    // Flip the surfaces
    while( 1 )
    {
        ddrval = lpFrontBuffer->lpVtbl->Flip( lpFrontBuffer, NULL, 0 );
        if( ddrval == DD_OK )
        {
            break;
        }
        if( ddrval == DDERR_SURFACELOST )
        {
            if( !RestoreSurfaces() )
            {
                return;
            }
        }
        if( ddrval != DDERR_WASSTILLDRAWING )
        {
            break;
        }
    }
}

void DisplayFrameRate( void )
{
    DWORD               time2;
    char                buff[256];

    dwFrameCount++;
    time2 = timeGetTime() - dwFrameTime;
    if( time2 > 1000 )
    {
        dwFrames = (dwFrameCount*1000)/time2;
        dwFrameTime = timeGetTime();
        dwFrameCount = 0;
    }
    if( dwFrames == 0 )
    {
        return;
    }

    if (dwFrames != dwFramesLast)
    {
        dwFramesLast = dwFrames;
    }

    if( dwFrames > 99 )
    {
        dwFrames = 99;
    }
    buff[0] = (char)((dwFrames / 10) + '0');
    buff[1] = (char)((dwFrames % 10) + '0');
    buff[2] = '\0';
    bltScore(buff, 295, 10);
}

void InitGame(void)
{
    int i, x, y;
    
    // clear all blocks
    for(x=0; x<5; x++)
        for(y=0; y<30; y++)
            Blocks.bits[y][x] = 0;

    // set random blocks
    for(i=0; i<400; i++)
    {
        x = randInt(0, 40);
        y = randInt(0, 30);
        if( !setBlock(x, y) )
            i--;
    }

    // initialize the host ship
    initShip(&(Ships[0]));
}
        
BOOL setBlock( int x, int y )
{
    BYTE  mask, col;

    mask = 1 << (x & 0x7);
    col = x >> 3;
    
    // is Block already set?
    if( Blocks.bits[y][col] & mask )
        return FALSE;
        
    // set the block and return success
    Blocks.bits[y][col] |= mask;
    return TRUE;
}
        
void initShip( LPGLOBALSHIP ship )
{
    int i;
    
    ship->velx = ship->vely = 0.0;

    ship->enable = TRUE;
    ship->posx = HomeX[WhoIAm];
    ship->posy = HomeY[WhoIAm];
    ship->frame = HomeFrame[WhoIAm];
    ship->benable = FALSE;
    ship->timeStamp = GetTickCount();
    ship->lastTick = ship->timeStamp;
    
    // no ship fragments
    for(i=0; i<64; i++)
        Frags[i].valid = FALSE;
}

BOOL RestoreSurfaces( void )
{
    HRESULT     ddrval;
    HBITMAP     hbm;

    ddrval = lpFrontBuffer->lpVtbl->Restore(lpFrontBuffer);
    if( ddrval != DD_OK )
        return FALSE;
    ddrval = lpShip0->lpVtbl->Restore(lpShip0);
    if( ddrval != DD_OK )
        return FALSE;
    ddrval = lpShip1->lpVtbl->Restore(lpShip1);
    if( ddrval != DD_OK )
        return FALSE;
    ddrval = lpShip2->lpVtbl->Restore(lpShip2);
    if( ddrval != DD_OK )
        return FALSE;
    ddrval = lpShip3->lpVtbl->Restore(lpShip3);
    if( ddrval != DD_OK )
        return FALSE;
    ddrval = lpNum->lpVtbl->Restore(lpNum);
    if( ddrval != DD_OK )
        return FALSE;

    // Create and set the palette for the splash bitmap
    lpSplashPalette = DDLoadPalette( lpDD, "SPLASH" );
    if( NULL == lpSplashPalette )
        return FALSE;

    // Create and set the palette for the art bitmap
    lpArtPalette = DDLoadPalette( lpDD, "Duel8" );
    if( NULL == lpArtPalette )
        return FALSE;

    // set the palette before loading the art
    lpFrontBuffer->lpVtbl->SetPalette( lpFrontBuffer, lpArtPalette );

    hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), "Duel8", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION );

    if( NULL == hbm )
        return FALSE;

    ddrval = DDCopyBitmap( lpShip0, hbm, 0, 0, 320, 128 );
    if( ddrval != DD_OK )
    {
        DeleteObject( hbm );
        return FALSE;
    }

    ddrval = DDCopyBitmap( lpShip1, hbm, 0, 128, 320, 128 );
    if( ddrval != DD_OK )
    {
        DeleteObject( hbm );
        return FALSE;
    }

    ddrval = DDCopyBitmap( lpShip2, hbm, 0, 256, 320, 128 );
    if( ddrval != DD_OK )
    {
        DeleteObject( hbm );
        return FALSE;
    }

    ddrval = DDCopyBitmap( lpShip3, hbm, 0, 384, 320, 128 );
    if( ddrval != DD_OK )
    {
        DeleteObject( hbm );
        return FALSE;
    }

    ddrval = DDCopyBitmap( lpNum, hbm, 0, 512, 320, 16 );
    if( ddrval != DD_OK )
    {
        DeleteObject( hbm );
        return FALSE;
    }

    DeleteObject( hbm );

    // set colorfill colors and color keys according to bitmap contents
    dwFillColor = DDColorMatch( lpShip0, CLR_INVALID );
    
    DDSetColorKey( lpShip0, CLR_INVALID );
    DDSetColorKey( lpShip1, CLR_INVALID );
    DDSetColorKey( lpShip2, CLR_INVALID );
    DDSetColorKey( lpShip3, CLR_INVALID );
    DDSetColorKey( lpNum, CLR_INVALID );

    return TRUE;
}


int randInt( int low, int high )
{
    int range = high - low;
    int num = rand() % range;
    return( num + low );
}

double randDouble( double low, double high )
{
    double range = high - low;
    double num = range * (double)rand()/(double)RAND_MAX;
    return( num + low );
}


BOOL RemoteCreate(GUID pGuid, LPSTR FullName, LPSTR NickName)
{
    HRESULT hr;
    DPSESSIONDESC dpDesc;

    // Be sure we aren't already initialized.
    if (lpIDC != NULL)
    {
        return( FALSE );
    }

    GetProvider();

    if (lpIDC == NULL)
        return(FALSE);

    switch( CreateGame())
    {
    case 1:             // Create
        IsHost = TRUE;
        memset(&dpDesc, 0x00, sizeof(DPSESSIONDESC));
        dpDesc.dwSize = sizeof(dpDesc);
        dpDesc.dwMaxPlayers = 16;
        dpDesc.dwFlags = DPOPEN_CREATESESSION;
        dpDesc.guidSession = pGuid;
        strcpy( dpDesc.szSessionName, FullName);
        
        if ((hr = lpIDC->lpVtbl->Open(lpIDC, &dpDesc)) != DP_OK)
        {
            lpIDC->lpVtbl->Release(lpIDC);
            lpIDC = NULL;
            return(FALSE);
        }
        
        break;

    case 2:             // Connect
        IsHost = FALSE;
        g_lpGuid = (LPGUID) &pGuid;

        GetGame();

        if (lpIDC == NULL)
            return(FALSE);

        break;

    default:
        return(FALSE);
    }

    if ((hr = lpIDC->lpVtbl->CreatePlayer(lpIDC, &dcoID, NickName,
                                  "Duel Player", &dphEvent)) != DP_OK)
    {
        lpIDC->lpVtbl->Close(lpIDC);
        lpIDC->lpVtbl->Release(lpIDC);
        lpIDC = NULL;
        return(FALSE);
    }
    
    return(TRUE);

}


int GetProvider()
{
  return(DialogBox (NULL, (LPCTSTR)IDD_CHOOSEPROVIDER, hWndMain,
                    (DLGPROC)DlgProcChooseProvider));
}

int CreateGame()
{
  return(DialogBox (NULL, (LPCTSTR) IDD_Q_CREATE, hWndMain, 
                    (DLGPROC) DlgProcQCreate));
}

int GetGame()
{

  return(DialogBox (NULL, (LPCTSTR) IDD_SELSESSION, hWndMain, 
                    (DLGPROC) DlgProcSelSession));
}


BOOL CALLBACK DlgProcChooseProvider(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LPGUID  lpGuid;
    static  LONG    iIndex;
    static  HWND hWndCtl;
    
    switch (msg)
    {
    case WM_INITDIALOG:

        hWndCtl = GetDlgItem(hDlg, IDC_LIST1);
        if (hWndCtl == NULL)
        {
            EndDialog(hDlg, TRUE);
            return(TRUE);
        }
        DirectPlayEnumerate(EnumSP, (LPVOID) hWndCtl);
        SetFocus(hWndCtl);
        SendMessage(hWndCtl, LB_SETCURSEL, 0, 0);
        return(FALSE);

    case WM_COMMAND:
        switch( HIWORD(wParam))
        {
        case LBN_SELCHANGE:
            iIndex = SendMessage((HWND) lParam, LB_GETCURSEL, 0, 0);
            hWndCtl = (HWND) lParam;
            return(FALSE);

        case LBN_DBLCLK:
            iIndex = SendMessage((HWND) lParam, LB_GETCURSEL, 0, 0);
            if (iIndex != LB_ERR)
            {
                lpGuid = (LPGUID) SendMessage((HWND) lParam, LB_GETITEMDATA, iIndex, 0);
                DirectPlayCreate(lpGuid, &lpIDC, NULL);
                EndDialog(hDlg, TRUE);
                return(TRUE);
            }
            break;

        case 0:
            if (LOWORD(wParam) == IDOK)
            {
                if (iIndex != LB_ERR)
                {
                    lpGuid = (LPGUID) SendMessage(hWndCtl, LB_GETITEMDATA, iIndex, 0);
                    if (lpGuid)
                    {
                        DirectPlayCreate(lpGuid, &lpIDC, NULL);
                        EndDialog(hDlg, TRUE);
                    }
                    else
                        EndDialog(hDlg, FALSE);
                    return(TRUE);
                }
            }
            else if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, FALSE);
                return(TRUE);
            }
            break;

        }
    }
    return (FALSE);
}

BOOL FAR PASCAL EnumSession(LPDPSESSIONDESC lpDPGameDesc, LPVOID lpContext, 
                            LPDWORD lpdwTimeOut, DWORD dwFlags)
{
    LONG iIndex;
    HWND hWnd = (HWND) lpContext;

    if( dwFlags & DPESC_TIMEDOUT )
    {
        return FALSE;       // don't try again
    }

    iIndex = SendMessage(hWnd, LB_ADDSTRING, 0, (LPARAM) lpDPGameDesc->szSessionName);
    if (iIndex != LB_ERR)
        SendMessage(hWnd, LB_SETITEMDATA, iIndex, (LPARAM) lpDPGameDesc->dwSession);

    SetFocus(hWnd);
    SendMessage(hWnd, LB_SETCURSEL, 0, 0);
    return(TRUE);

}

BOOL FAR PASCAL EnumSP(LPGUID lpGuid, LPSTR lpDesc, DWORD dwMajorVersion,
                       DWORD dwMinorVersion, LPVOID lpv)
{
    LONG iIndex;
    HWND hWnd = (HWND) lpv;

    iIndex = SendMessage(hWnd, LB_ADDSTRING, 0, (LPARAM) lpDesc);
    if (iIndex != LB_ERR)
        SendMessage(hWnd, LB_SETITEMDATA, iIndex, (LPARAM) lpGuid);

    SetFocus(hWnd);
    SendMessage(hWnd, LB_SETCURSEL, 0, 0);
    return(TRUE);
}


BOOL CALLBACK DlgProcQCreate (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        switch(wParam)
        {
        case IDC_CREATE:
            EndDialog(hDlg, 1);
            return(TRUE);

        case IDC_CONNECT:
            EndDialog(hDlg, 2);
            return(TRUE);

        case IDCANCEL:
            EndDialog(hDlg, -1);
            return(TRUE);
        }
        break;

    }
    return(FALSE);
}

BOOL CALLBACK DlgProcSelSession (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static  LONG    iIndex;
    static  HWND hWndCtl;
    DPSESSIONDESC dpDesc;
    HRESULT hr = DP_OK + 10;

    switch (msg)
    {
    case WM_INITDIALOG:

        hWndCtl = GetDlgItem(hDlg, IDC_LB_SESSION);
        if (hWndCtl == NULL)
        {
            EndDialog(hDlg, TRUE);
            return(TRUE);
        }
        memset(&dpDesc, 0x00, sizeof(DPSESSIONDESC));
        dpDesc.dwSize = sizeof(dpDesc);
        dpDesc.guidSession = *g_lpGuid;
        // enum sessions with 5 second timeout
        lpIDC->lpVtbl->EnumSessions(lpIDC, &dpDesc, (DWORD)5000, EnumSession, (LPVOID) hWndCtl, (DWORD)NULL);

        SetFocus(hWndCtl);
        return(FALSE);

    case WM_COMMAND:

        switch( HIWORD(wParam))
        {
        case LBN_SELCHANGE:
            iIndex = SendMessage((HWND) lParam, LB_GETCURSEL, 0, 0);
            hWndCtl = (HWND) lParam;
            return(FALSE);

        case 0:
            if (LOWORD(wParam) == IDCANCEL)
            {
                lpIDC->lpVtbl->Close(lpIDC);
                lpIDC->lpVtbl->Release(lpIDC);
                lpIDC = NULL;
                EndDialog(hDlg, FALSE);
                return(TRUE);
            }
            //
            // Fall Through.
            //
        case LBN_DBLCLK:
            if (HIWORD(wParam) == LBN_DBLCLK)
            {
                hWndCtl = (HWND) lParam;
                iIndex = SendMessage(hWndCtl, LB_GETCURSEL, 0, 0);
            }

            if (iIndex != LB_ERR)
            {
                memset(&dpDesc, 0x00, sizeof(DPSESSIONDESC));
                dpDesc.dwSize       = sizeof(dpDesc);
                dpDesc.guidSession  = *g_lpGuid;
                dpDesc.dwFlags      = DPOPEN_OPENSESSION;
                dpDesc.dwSession    = SendMessage((HWND) hWndCtl, LB_GETITEMDATA, iIndex, 0);
                hr = lpIDC->lpVtbl->Open(lpIDC, &dpDesc);

                if (hr != DP_OK)
                {
                    lpIDC->lpVtbl->Close(lpIDC);
                    lpIDC->lpVtbl->Release(lpIDC);
                    lpIDC = NULL;
                    EndDialog(hDlg, FALSE);
                }

                EndDialog(hDlg, TRUE);
                return(TRUE);

            }
        }
    }
    return (FALSE);
}

void ReceiveGameMessages( void )
{
    DPID                fromID, dcoReceiveID;
    DWORD               nBytes;
    int                 x,y;

    if ( lpIDC )
    {
        // maybe add a block?
        if( ( randInt( 0, 100 ) > 98 ) && IsHost && bIsActive)
        {
            x = randInt( 0, 40);
            y = randInt( 0, 30);
            if( setBlock( x, y) )
            {
                SendGameMessage(MSG_ADDBLOCK, 0, (BYTE)x, (BYTE)y, 0);
            }
        }
        // read all messages in queue
        while(1)
        {
            HRESULT status;
            nBytes = MAX_BUFFER_SIZE;
            status = lpIDC->lpVtbl->Receive( lpIDC,
                        &fromID,
                        &dcoReceiveID,
                        DPRECEIVE_ALL,
                        CommBuff,
                        &nBytes);
            switch( status )
            {
            case DP_OK:
                if ( fromID == 0 )
                {
                    // ignore system messages
                }
                else
                {
                    EvaluateMessage( nBytes );
                }
                break;

            default:

                // Error condition of some kind - we just stop
                // checking for now
                return;
            }
        }
    }
}

void EvaluateMessage( DWORD len )
{
    LPUPDATEMSG     lpUpdate;
    LPHEREIAMMSG    lpHereIAm;
    LPINITMSG       lpInit;
    LPBLOCKHITMSG   lpBlockHit;
    LPSHIPHITMSG    lpShipHit;
    LPADDBLOCKMSG   lpAddBlock;
    
    char dBuf[256];
    
    switch( CommBuff[0] )
    {
    case MSG_UPDATE:
        lpUpdate = (LPUPDATEMSG)CommBuff;
        Ships[lpUpdate->WhoIAm] = lpUpdate->Ship;
        Ships[lpUpdate->WhoIAm].timeStamp = GetTickCount();
        Ships[lpUpdate->WhoIAm].lastTick = Ships[lpUpdate->WhoIAm].timeStamp;
        break;
        
    case MSG_HEREIAM:
        if( IsHost )
        {
            lpHereIAm = (LPHEREIAMMSG)CommBuff;
            SendGameMessage(MSG_INIT, lpHereIAm->ID, 0, 0, 0);
        }
        break;
    case MSG_INIT:
        if( !IsHost )
        {
            lpInit = (LPINITMSG)CommBuff;
            WhoIAm = lpInit->YouAre;
            initShip(&Ships[lpInit->YouAre]);
            Blocks = lpInit->Blocks;
            HaveHostInit = TRUE;
        }
        break;

    case MSG_BLOCKHIT:
        lpBlockHit = (LPBLOCKHITMSG)CommBuff;
        Blocks.bits[lpBlockHit->row][lpBlockHit->col] &= ~lpBlockHit->mask;
        break;

    case MSG_ADDBLOCK:
        lpAddBlock = (LPADDBLOCKMSG)CommBuff;
        setBlock( lpAddBlock->x, lpAddBlock->y);
        break;

    case MSG_SHIPHIT:
        lpShipHit = (LPSHIPHITMSG)CommBuff;
        Ships[lpShipHit->You].enable = FALSE;
        DestroyShip( lpShipHit->You );
        if( lpShipHit->You == WhoIAm )
        {
            Ships[WhoIAm].posx = HomeX[WhoIAm];
            Ships[WhoIAm].posy = HomeY[WhoIAm];
            Ships[WhoIAm].frame = HomeFrame[WhoIAm];
            CountDown = 5000;
        }
        break;
                
    default:
        wsprintf(dBuf, "Unknown message: %d\n", CommBuff[0]);
        OutputDebugString( dBuf );
        break;
    }
}

void SendGameMessage( BYTE msg, DWORD to, BYTE row, BYTE col, BYTE mask )
{
    LPUPDATEMSG     lpUpdate;
    LPHEREIAMMSG    lpHereIAm;
    LPINITMSG       lpInit;
    LPBLOCKHITMSG   lpBlockHit;
    LPSHIPHITMSG    lpShipHit;
    LPADDBLOCKMSG   lpAddBlock;
    int             nBytes;
    int             i;
    DWORD           send_to = 0;
    
    switch( msg )
    {
    case MSG_HEREIAM:
        lpHereIAm = (LPHEREIAMMSG)CommBuff;
        lpHereIAm->MsgCode = msg;
        lpHereIAm->ID = (DWORD)dcoID;
        nBytes = sizeof( HEREIAMMSG );
        break;

    case MSG_INIT:
        lpInit = (LPINITMSG)CommBuff;
        lpInit->MsgCode = msg;
        send_to = to;
        for(i=0; i<16; i++)
        {
            if( !Ships[i].enable )
                break;
        }
        if( i == 16 )
        {
            // no room for more players
            return;
        }
        lpInit->YouAre = i;
        lpInit->Blocks = Blocks;
        nBytes = sizeof( INITMSG );
        break;
        
    case MSG_UPDATE:
        lpUpdate = (LPUPDATEMSG)CommBuff;
        lpUpdate->MsgCode = msg;
        lpUpdate->WhoIAm = WhoIAm;
        lpUpdate->Ship = Ships[WhoIAm];
        nBytes = sizeof( UPDATEMSG );
        break;
    case MSG_BLOCKHIT:
        lpBlockHit = (LPBLOCKHITMSG)CommBuff;
        lpBlockHit->MsgCode = msg;
        lpBlockHit->row = row;
        lpBlockHit->col = col;
        lpBlockHit->mask = mask;
        nBytes = sizeof( BLOCKHITMSG );
        break;
    case MSG_SHIPHIT:
        lpShipHit = (LPSHIPHITMSG)CommBuff;
        lpShipHit->MsgCode = msg;
        lpShipHit->You = row;
        nBytes = sizeof( SHIPHITMSG );
        break;
    case MSG_ADDBLOCK:
        lpAddBlock = (LPADDBLOCKMSG)CommBuff;
        lpAddBlock->MsgCode = msg;
        lpAddBlock->x = row;
        lpAddBlock->y = col;
        nBytes = sizeof( ADDBLOCKMSG );
        break;
    }
    // Broadcast it to everyone in the group.
    lpIDC->lpVtbl->Send( lpIDC,
                         dcoID,   // From
                         send_to, // send to everybody
                         0,
                         (LPSTR)CommBuff,
                         nBytes);       

}
        
        
void AddFrag(int which, int offX, int offY)
{
    int i;
    for(i=0; i<64; i++) // find available fragment
    {
        if( !Frags[i].valid )
            break;
    }
    if( i == 64 )
        return;
        
    
    Frags[i].posx = (double)offX + Ships[which].posx;
    Frags[i].posy = (double)offY + Ships[which].posy;
    switch( which % 4 )
    {
    case 0: Frags[i].surf = lpShip0;    break;
    case 1: Frags[i].surf = lpShip1;    break;
    case 2: Frags[i].surf = lpShip2;    break;
    case 3: Frags[i].surf = lpShip3;    break;
    }
    Frags[i].src.top = 32 * ( (int)Ships[which].frame / 10 ) + offX;
    Frags[i].src.left = 32 * ( (int)Ships[which].frame % 10 ) + offY;
    Frags[i].src.right = Frags[i].src.left + 8;
    Frags[i].src.bottom = Frags[i].src.top + 8;
    Frags[i].velx = ((double)offX - 12.0)/24.0;
    Frags[i].vely = ((double)offY - 12.0)/24.0;
    Frags[i].valid = TRUE;
}


void UpdateFragment(int i)
{
    DWORD   TickCnt;
    static DWORD   tickDiff;
    static DWORD lastTick;

    if( i == 0)
    {
        TickCnt = GetTickCount();
        tickDiff = TickCnt - lastTick;
        lastTick = TickCnt;
    }
    
    if( !Frags[i].valid )
        return;
        
    Frags[i].posx += Frags[i].velx * tickDiff;
    Frags[i].posy += Frags[i].vely * tickDiff;
    if( (Frags[i].posx < 0.0) || (Frags[i].posx >= 632.0) ||
        (Frags[i].posy < 0.0) || (Frags[i].posy >= 472.0) )
    {
        Frags[i].valid = FALSE;
    }
}

void DrawFragments( void )
{
    int     i;
    
    for(i=0; i<64; i++)
    {
        if( Frags[i].valid )
        {
            bltObject( (int)Frags[i].posx, (int)Frags[i].posy, Frags[i].surf,
                &(Frags[i].src), DDBLTFAST_SRCCOLORKEY );
        }
    }
}


void DestroyShip( int which )
{
    // add ship fragments
    AddFrag(which, 0, 0);
    AddFrag(which, 8, 0);
    AddFrag(which, 16, 0);
    AddFrag(which, 24, 0);
    AddFrag(which, 0, 8);
    AddFrag(which, 8, 8);
    AddFrag(which, 16, 8);
    AddFrag(which, 24, 8);
    AddFrag(which, 0, 16);
    AddFrag(which, 8, 16);
    AddFrag(which, 16, 16);
    AddFrag(which, 24, 16);
    AddFrag(which, 0, 24);
    AddFrag(which, 8, 24);
    AddFrag(which, 16, 24);
    AddFrag(which, 24, 24);
}
