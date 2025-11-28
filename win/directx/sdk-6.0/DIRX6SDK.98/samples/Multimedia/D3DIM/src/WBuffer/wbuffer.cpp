//-----------------------------------------------------------------------------
// File: WBuffer.cpp
//
// Desc: Example code showing how to use a w-buffer in Direct3D.
//
// Draws red/green sort-of-waffly object obscuring blue object.  Spins and
// moves away in Z, scaling as it moves away to keep same screen size.
//
// Should draw nice regular red/green grid fully obscuring blue object.
//
// Draws with two devices and displays side-by-side.  Right side is Z
// buffered.  Left side is W buffered.
//       Note: This code uses the D3D Framework helper library.
//
//
// Copyright (c) 1997-1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT
#define D3D_OVERLOADS
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "D3DTextr.h"
#include "D3DUtil.h"
#include "D3DMath.h"
#include "resource.h"


//-----------------------------------------------------------------------------
// Declare the application globals for use in WinMain.cpp
//-----------------------------------------------------------------------------
TCHAR* g_strAppTitle       = TEXT( "WBuffer: Direct3D W-buffering Demo" );
BOOL   g_bAppUseZBuffer    = TRUE;
BOOL   g_bAppUseBackBuffer = TRUE;




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define NUM_WAFFLE_SLICES 20
D3DLVERTEX     g_pvBluePlane[4];
D3DLVERTEX     g_pvGreenWaffle[NUM_WAFFLE_SLICES*2];

D3DZBUFFERTYPE g_dwDepthBufferType;
HMENU          g_hMenu;




//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
VOID    AppOutputText( LPDIRECT3DDEVICE3, DWORD, DWORD, CHAR* );
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK App_OverridenWndProc( HWND, UINT, WPARAM, LPARAM );




//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{
    // Attach a message handler for the menu
    g_hMenu = GetMenu( hWnd );
    SetWindowLong( hWnd, GWL_WNDPROC, (LONG)App_OverridenWndProc );

    // Set up the vertices for the blue plane
    D3DVECTOR v1 = D3DVECTOR(-0.8f,-0.8f, 1.0f );
    D3DVECTOR v2 = D3DVECTOR(-0.8f, 0.8f, 1.0f );
    D3DVECTOR v3 = D3DVECTOR( 0.8f, 0.8f, 1.0f );
    D3DVECTOR v4 = D3DVECTOR( 0.8f,-0.8f, 1.0f );
    g_pvBluePlane[0] = D3DLVERTEX( v1, 0xff0000ff, 0x0, 0, 1 );
    g_pvBluePlane[1] = D3DLVERTEX( v2, 0xff0000ff, 0x0, 0, 0 );
    g_pvBluePlane[2] = D3DLVERTEX( v3, 0xff0000ff, 0x0, 1, 1 );
    g_pvBluePlane[3] = D3DLVERTEX( v4, 0xff0000ff, 0x0, 1, 0 );

    // Set up the vertices for the waffle object
    for( DWORD i=0; i<NUM_WAFFLE_SLICES; i++ )
    {
        FLOAT u = ((FLOAT)(i))/(NUM_WAFFLE_SLICES-1);
        FLOAT x = 2*u - 1.0f;
        FLOAT z = 0.0f;
        if( i%4 == 0 ) z = -0.5f;
        if( i%4 == 2 ) z = +0.5f;

        D3DVECTOR v1 = D3DVECTOR( x, -1.0f, z );
        D3DVECTOR v2 = D3DVECTOR( x, +1.0f, z );

        g_pvGreenWaffle[2*i+0] = D3DLVERTEX( v1, 0xff00ff00, 0x0, u, 1.0f );
        g_pvGreenWaffle[2*i+1] = D3DLVERTEX( v2, 0xff00ff00, 0x0, u, 0.0f );
    }

    // Create a texture
    D3DTextr_CreateTexture( "wbuffer.bmp" );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT App_FrameMove( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{
    // Rotate the object about the z-axis
    D3DMATRIX matWorld;
    D3DUtil_SetRotateZMatrix( matWorld, (g_PI/2) * ( 1.0f + (FLOAT)sin(fTimeKey) ) );

    // Move the object back in forth along the z-axis
    matWorld._43 = 50.0f + 40.0f*(FLOAT)sin( fTimeKey );

    // Put the new world matrix into effect
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT App_Render( LPDIRECT3DDEVICE3 pd3dDevice, 
                    LPDIRECT3DVIEWPORT3 pvViewport, D3DRECT* prcViewRect )
{
    // Clear the viewport
    pvViewport->Clear2( 1, prcViewRect, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                        0L, 1.0f, 0L );

    // Set the depth-buffering states
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ZWRITEENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ZFUNC,   D3DCMP_LESSEQUAL );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, g_dwDepthBufferType );

    // Begin the scene 
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        pd3dDevice->SetTexture( 0, D3DTextr_GetTexture( "wbuffer.bmp" ) );
        pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_LVERTEX,
                                   g_pvGreenWaffle, NUM_WAFFLE_SLICES*2, NULL );

        pd3dDevice->SetTexture( 0, NULL );
        pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, D3DFVF_LVERTEX,
                                   g_pvBluePlane, 4, NULL );
        
        // End the scene.
        pd3dDevice->EndScene();
    }

    // Output text to the backbuffer
    CHAR strBuffer[80];
    if( D3DZB_FALSE == g_dwDepthBufferType )
        sprintf( strBuffer, "Not using depth-buffer" );
    if( D3DZB_TRUE == g_dwDepthBufferType )
        sprintf( strBuffer, "Using Z-buffer" );
    if( D3DZB_USEW == g_dwDepthBufferType )
        sprintf( strBuffer, "Using W-buffer" );
    AppOutputText( pd3dDevice, 5, 15, strBuffer );

    D3DMATRIX matWorld;
    pd3dDevice->GetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
    sprintf( strBuffer, "Z-coordinate = %.2f", matWorld._43 );
    AppOutputText( pd3dDevice, 5, 30, strBuffer );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3 pd3dDevice,
                               LPDIRECT3DVIEWPORT3 pvViewport )
{
    // Check parameters
    if( NULL==pd3dDevice || NULL==pvViewport )
        return E_INVALIDARG;

    // Set menu items
    EnableMenuItem( g_hMenu, IDM_USEWBUFFER,    MF_ENABLED );
    EnableMenuItem( g_hMenu, IDM_USEZBUFFER,    MF_ENABLED );
    EnableMenuItem( g_hMenu, IDM_NODEPTHBUFFER, MF_ENABLED );
    CheckMenuItem( g_hMenu, IDM_USEWBUFFER,    MF_UNCHECKED );
    CheckMenuItem( g_hMenu, IDM_USEZBUFFER,    MF_CHECKED );
    CheckMenuItem( g_hMenu, IDM_NODEPTHBUFFER, MF_UNCHECKED );

    // Get device caps so we can check for w-buffering
    g_dwDepthBufferType = D3DZB_TRUE;
    D3DDEVICEDESC ddHwDesc, ddSwDesc;
    ddHwDesc.dwSize = sizeof(D3DDEVICEDESC);
    ddSwDesc.dwSize = sizeof(D3DDEVICEDESC);
    if( SUCCEEDED( pd3dDevice->GetCaps( &ddHwDesc, &ddSwDesc ) ) )
    {
        // Get triangle caps (Hardware or software)
        LPD3DPRIMCAPS pdpc = ( ddHwDesc.dwFlags ) ? &ddHwDesc.dpcTriCaps
                                                  : &ddSwDesc.dpcTriCaps;
        // Get triangle caps and check for w-buffering
        if( 0L == ( pdpc->dwRasterCaps & D3DPRASTERCAPS_WBUFFER ) )
            EnableMenuItem( g_hMenu, IDM_USEWBUFFER, MF_GRAYED );
    }
    
    D3DTextr_RestoreAllTextures( pd3dDevice );
    pd3dDevice->SetTexture( 0, D3DTextr_GetTexture( "wbuftex.bmp" ) );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );
    pd3dDevice->SetRenderState(  D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE );

    // Set the transform matrices
    D3DMATRIX matWorld, matView, matProj;
    D3DUtil_SetIdentityMatrix( matWorld );
    D3DUtil_SetIdentityMatrix( matView );
    D3DUtil_SetProjectionMatrix( matProj, 10.0f*g_PI/180.0f, 1.0f, 0.01f, 100.0f );

    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW,       &matView );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT App_FinalCleanup( LPDIRECT3DDEVICE3 pd3dDevice, 
                          LPDIRECT3DVIEWPORT3 pvViewport )
{
    App_DeleteDeviceObjects( pd3dDevice, pvViewport );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_DeleteDeviceObjects()
// Desc: Called when the app is exitting, or the device is being changed,
//       this function deletes any device dependant objects.
//-----------------------------------------------------------------------------
VOID App_DeleteDeviceObjects( LPDIRECT3DDEVICE3 pd3dDevice, 
                              LPDIRECT3DVIEWPORT3 pvViewport )
{
    D3DTextr_InvalidateAllTextures();
}




//----------------------------------------------------------------------------
// Name: App_RestoreSurfaces
// Desc: Restores any previously lost surfaces. Must do this for all surfaces
//       (including textures) that the app created.
//----------------------------------------------------------------------------
HRESULT App_RestoreSurfaces()
{
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_ConfirmDevice()
// Desc: Called during device intialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT App_ConfirmDevice( DDCAPS* pddDriverCaps,
                           D3DDEVICEDESC* pd3dDeviceDesc )
{
    // Accept all devices that do z-buffers
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_OverridenWndProc()
// Desc: Local WndProc() function to handle menu operations.
//-----------------------------------------------------------------------------
LRESULT CALLBACK App_OverridenWndProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                       LPARAM lParam )
{
    if( WM_COMMAND == uMsg )
    {
        switch( LOWORD(wParam) )
        {
            case IDM_USEWBUFFER:
                g_dwDepthBufferType = D3DZB_USEW;
                CheckMenuItem( g_hMenu, IDM_USEWBUFFER,    MF_CHECKED );
                CheckMenuItem( g_hMenu, IDM_USEZBUFFER,    MF_UNCHECKED );
                CheckMenuItem( g_hMenu, IDM_NODEPTHBUFFER, MF_UNCHECKED );
                break;

            case IDM_USEZBUFFER:
                g_dwDepthBufferType = D3DZB_TRUE;
                CheckMenuItem( g_hMenu, IDM_USEWBUFFER,    MF_UNCHECKED );
                CheckMenuItem( g_hMenu, IDM_USEZBUFFER,    MF_CHECKED );
                CheckMenuItem( g_hMenu, IDM_NODEPTHBUFFER, MF_UNCHECKED );
                break;

            case IDM_NODEPTHBUFFER:
                g_dwDepthBufferType = D3DZB_FALSE;
                CheckMenuItem( g_hMenu, IDM_USEWBUFFER,    MF_UNCHECKED );
                CheckMenuItem( g_hMenu, IDM_USEZBUFFER,    MF_UNCHECKED );
                CheckMenuItem( g_hMenu, IDM_NODEPTHBUFFER, MF_CHECKED );
                break;
        }
    }

    return WndProc( hWnd, uMsg, wParam, lParam );
}





