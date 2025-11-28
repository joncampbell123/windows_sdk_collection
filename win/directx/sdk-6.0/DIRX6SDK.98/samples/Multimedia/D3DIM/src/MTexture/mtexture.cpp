//-----------------------------------------------------------------------------
// File: MTexture.cpp
//
// Desc: Example code showing how to enable multiple-texturing. This
//       sample shows the interior of a room "lit" with a light map
//       using mulitple textures.
//
//       Note: This code uses the D3D Framework helper library.
//
//
// Copyright (c) 1997-1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define STRICT
#define D3D_OVERLOADS
#include <math.h>
#include "D3DTextr.h"
#include "D3DUtil.h"
#include "D3DMath.h"
#include "resource.h"




//-----------------------------------------------------------------------------
// Declare the application globals
//-----------------------------------------------------------------------------
TCHAR* g_strAppTitle       = TEXT( "MTexture: DX6 Multi-Texturing Demo" );
BOOL   g_bAppUseZBuffer    = FALSE;
BOOL   g_bAppUseBackBuffer = TRUE;




//-----------------------------------------------------------------------------
// Define a custom vertex that uses XYZ, a normal, and two sets of tex coords
//-----------------------------------------------------------------------------
struct MTVERTEX
{
    D3DVALUE x, y, z;
    D3DVALUE tuBase, tvBase;
    D3DVALUE tuLightMap, tvLightMap;
};

#define FILL_MTVERTEX( v, ax, ay, az, atu1, atv1, atu2, atv2 )  \
{   v.x = ax; v.y = ay; v.z = az; \
    v.tuBase     = atu1; v.tvBase     = atv1; \
    v.tuLightMap = atu2; v.tvLightMap = atv2; \
}




//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
VOID    AppOutputText( LPDIRECT3DDEVICE3, DWORD, DWORD, CHAR* );
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
HRESULT App_CreateInvAlphaTexFromTex( LPDIRECT3DTEXTURE2*,
                                      LPDIRECT3DTEXTURE2, DDPIXELFORMAT* );
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK App_OverridenWndProc( HWND, UINT, WPARAM, LPARAM );


D3DVERTEX          g_avFloorVertices[4];  // Vertex data
D3DVERTEX          g_avCeilVertices[4];
MTVERTEX           g_avWallVertices[10];

LPDIRECT3DTEXTURE2 g_ptexWallTexture   = NULL; // Textures for the app
LPDIRECT3DTEXTURE2 g_ptexFloorTexture  = NULL; 
LPDIRECT3DTEXTURE2 g_ptexLightMap      = NULL;
LPDIRECT3DTEXTURE2 g_ptexAlphaLightMap = NULL;

HMENU g_hMenu;
BOOL  g_bCanDoMultiTexture;      // Whether device does mtexture
BOOL  g_bCanDoColorBlend;        // Device can do multi pass w/color blend
BOOL  g_bCanDoAlphaBlend;        // Device can do multi pass w/alpha blend
BOOL  g_bCanDoTextureAlpha;      // Device can do use textures with alpha

enum TEXTUREMODE
{
    TEXTURE_NONE,
    TEXTURE_SINGLEPASS,
    TEXTURE_MULTIPASSCOLOR,
    TEXTURE_MULTIPASSALPHA
} g_wTextureMode;  // Which texturing mode we are using.




//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{
    // Add a message handler to the program
    SetWindowLong( hWnd, GWL_WNDPROC, (LONG)App_OverridenWndProc );
    g_hMenu = GetMenu( hWnd );

    // Declare the vertices which define the room's walls, floor, and ceiling
    D3DVECTOR vNorm(0,1,0);

    FILL_MTVERTEX( g_avWallVertices[ 0], -5,-5, 5, 0.0f, 1.0f, 0.0, 1.0f );
    FILL_MTVERTEX( g_avWallVertices[ 1], -5, 5, 5, 0.0f, 0.0f, 0.0, 0.0f );
    FILL_MTVERTEX( g_avWallVertices[ 2],  5,-5, 5, 1.0f, 1.0f, 1.0, 1.0f );
    FILL_MTVERTEX( g_avWallVertices[ 3],  5, 5, 5, 1.0f, 0.0f, 1.0, 0.0f );
    FILL_MTVERTEX( g_avWallVertices[ 4],  5,-5,-5, 2.0f, 1.0f, 2.0, 1.0f );
    FILL_MTVERTEX( g_avWallVertices[ 5],  5, 5,-5, 2.0f, 0.0f, 2.0, 0.0f );
    FILL_MTVERTEX( g_avWallVertices[ 6], -5,-5,-5, 3.0f, 1.0f, 3.0, 1.0f );
    FILL_MTVERTEX( g_avWallVertices[ 7], -5, 5,-5, 3.0f, 0.0f, 3.0, 0.0f );
    FILL_MTVERTEX( g_avWallVertices[ 8], -5,-5, 5, 4.0f, 1.0f, 4.0, 1.0f );
    FILL_MTVERTEX( g_avWallVertices[ 9], -5, 5, 5, 4.0f, 0.0f, 4.0, 0.0f );

    g_avFloorVertices[0] = D3DVERTEX( D3DVECTOR(-5,-5, 5), vNorm, 0.0f, 0.0f );
    g_avFloorVertices[1] = D3DVERTEX( D3DVECTOR( 5,-5, 5), vNorm, 0.0f, 1.0f );
    g_avFloorVertices[2] = D3DVERTEX( D3DVECTOR(-5,-5,-5), vNorm, 1.0f, 0.0f );
    g_avFloorVertices[3] = D3DVERTEX( D3DVECTOR( 5,-5,-5), vNorm, 1.0f, 1.0f );

    g_avCeilVertices[0]  = D3DVERTEX( D3DVECTOR(-5, 5, 5),-vNorm, 0.0f, 0.0f );
    g_avCeilVertices[1]  = D3DVERTEX( D3DVECTOR(-5, 5,-5),-vNorm, 1.0f, 0.0f );
    g_avCeilVertices[2]  = D3DVERTEX( D3DVECTOR( 5, 5, 5),-vNorm, 0.0f, 1.0f );
    g_avCeilVertices[3]  = D3DVERTEX( D3DVECTOR( 5, 5,-5),-vNorm, 1.0f, 1.0f );

    // Create some textures
    D3DTextr_CreateTexture( "wall.bmp", 0 );
    D3DTextr_CreateTexture( "floor.bmp", 0 );
    D3DTextr_CreateTexture( "spotlite.bmp", 1 );
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT App_FrameMove( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{
    // Setup the world spin matrix
    D3DMATRIX matWorldSpin;
    D3DUtil_SetRotateYMatrix( matWorldSpin, -fTimeKey/9 );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorldSpin );

    // Rotate the light map around the walls each frame
    FLOAT tuNew = (FLOAT)fmod( fTimeKey/5, 1.0f );

    for( int i=0; i<5; i++ )
    {
        g_avWallVertices[2*i+0].tuLightMap = tuNew + i;
        g_avWallVertices[2*i+1].tuLightMap = tuNew + i;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT App_Render( LPDIRECT3DDEVICE3 pd3dDevice, 
                    LPDIRECT3DVIEWPORT3 pvViewport, D3DRECT* prcRect )
{
    // Begin the scene
    if( FAILED( pd3dDevice->BeginScene() ) )
    {
        // Don't return an error, unless we want the app to exit
        return S_OK;
    }

    // Set the texture state's for single-texturing mode
    pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

    // Render the floor and cieling in single-texture mode
    pd3dDevice->SetTexture( 0, g_ptexFloorTexture );
    pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, 
                               g_avFloorVertices, 4, NULL );
    pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, 
                               g_avCeilVertices, 4, NULL );

    
    // Now, render the walls using either single-pass multitexturing, or one
    // of the multipass techniques.
    if( g_wTextureMode == TEXTURE_SINGLEPASS )
    {
        pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
        pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT ); 
        pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
        pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );

        // Draw the walls in multi-texture mode
        pd3dDevice->SetTexture( 0, g_ptexWallTexture );
        pd3dDevice->SetTexture( 1, g_ptexLightMap );
        pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_XYZ|D3DFVF_TEX2,
                                   g_avWallVertices, 10, NULL );
    }
    else 
    {
        // We're not drawing walls with multi-texturing, so we need to emulate
        // the effect with multi-pass rendering. Draw the wall, using the
        // single-texturing for the 1st pass
        pd3dDevice->SetTexture( 0, g_ptexWallTexture );
        pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_XYZ|D3DFVF_TEX2,
                                   g_avWallVertices, 10, NULL );

        // Render the 2nd pass based on which multi-pass technique we're using.
        if( g_wTextureMode == TEXTURE_MULTIPASSCOLOR ) 
        {
            // Multi-pass using color blending
            pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,  D3DBLEND_ZERO );
            pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCCOLOR );
            pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );

            // Use the lightmap texture for the 2nd pass
            pd3dDevice->SetTexture( 0, g_ptexLightMap );
            pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 1 );
            pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_XYZ|D3DFVF_TEX2,
                                       g_avWallVertices, 10, NULL );
        }
        else if( g_wTextureMode == TEXTURE_MULTIPASSALPHA ) 
        {
            // Multi-pass using alpha blending
            pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,  D3DBLEND_SRCALPHA );
            pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA );
            pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );

            // Use the inverse-alpha lightmap texture for the 2nd pass
            pd3dDevice->SetTexture( 0, g_ptexAlphaLightMap );
            pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 1 );
            pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_XYZ|D3DFVF_TEX2,
                                       g_avWallVertices, 10, NULL );
        }

        // Restore state
        pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
    }

    // End the scene.
    pd3dDevice->EndScene();

    // Output which rendering technique we're using
    if( g_wTextureMode == TEXTURE_SINGLEPASS )
        AppOutputText( pd3dDevice, 5, 20, "Single-pass multi-texture" );

    if( g_wTextureMode == TEXTURE_MULTIPASSCOLOR )
        AppOutputText( pd3dDevice, 5, 20, "Multi-pass with color blend" );

    if( g_wTextureMode == TEXTURE_MULTIPASSALPHA )
        AppOutputText( pd3dDevice, 5, 20, "Multi-pass with alpha blend" );

    if( g_wTextureMode == TEXTURE_NONE )
        AppOutputText( pd3dDevice, 5, 20, "Device cannot do m-texturing" );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: TextureSearchCallback()
// Desc: Callback function used to enumerate texture formats.
//-----------------------------------------------------------------------------
HRESULT CALLBACK TextureSearchCallback( DDPIXELFORMAT* pddpf, VOID* param )
{
    // Skip unwanted formats. We are looking for a 4444 ARGB format.
    if( pddpf->dwRGBAlphaBitMask != 0x0000f000 )
        return DDENUMRET_OK;

    memcpy( (DDPIXELFORMAT*)param, pddpf, sizeof(DDPIXELFORMAT) );
    return DDENUMRET_CANCEL;
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
        return DDERR_INVALIDPARAMS;

    // Load the textures to the device
    D3DTextr_RestoreAllTextures( pd3dDevice );

    if( NULL == ( g_ptexWallTexture = D3DTextr_GetTexture( "wall.bmp" ) ) )
        return E_FAIL;
    if( NULL == ( g_ptexFloorTexture = D3DTextr_GetTexture( "floor.bmp" ) ) )
        return E_FAIL;
    if( NULL == ( g_ptexLightMap = D3DTextr_GetTexture( "spotlite.bmp" ) ) )
        return E_FAIL;

    // The lightmap is a greyscale 565 RGB texture which is used to build a 
    // a 4444 ARGB texture with an inverted alpha (1-alpha), which is used for
    // alternate multi-texture rendering methods.
    DDPIXELFORMAT ddpfAlphaFormat;
    ddpfAlphaFormat.dwRGBBitCount = 0L;
    pd3dDevice->EnumTextureFormats( TextureSearchCallback, &ddpfAlphaFormat );
    if( ddpfAlphaFormat.dwRGBBitCount > 0 )
    {
        if( FAILED( App_CreateInvAlphaTexFromTex( &g_ptexAlphaLightMap,
                                                  g_ptexLightMap, 
                                                  &ddpfAlphaFormat ) ) )
            return E_FAIL;
    }
    else
        g_ptexAlphaLightMap = NULL;

    // Set the transform matrices
    D3DMATRIX matWorld, matView, matProj;
    D3DVECTOR vEyePt    = D3DVECTOR( 0, 0, -2.5 );
    D3DVECTOR vLookatPt = D3DVECTOR( 0, 0,   0  );
    D3DVECTOR vUpVec    = D3DVECTOR( 0, 1,   0  );

    D3DUtil_SetIdentityMatrix( matWorld );
    D3DUtil_SetViewMatrix( matView, vEyePt, vLookatPt, vUpVec );
    D3DUtil_SetProjectionMatrix( matProj, g_PI/2, 1.0f, 1.0f, 100.0f );

    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW,       &matView );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

    // Set any appropiate state
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, FALSE );
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_AMBIENT,  0xffffffff );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTFN_LINEAR );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTFG_LINEAR );

    // Set all device capabilites initially to FALSE
    g_bCanDoMultiTexture = FALSE;
    g_bCanDoColorBlend   = FALSE;
    g_bCanDoAlphaBlend   = FALSE;
    
    // Check whether the device supports real multitexturing (if not, we're
    // going to emulate it using multipass rendering)
    D3DDEVICEDESC ddHwDesc, ddSwDesc;
    ddHwDesc.dwSize = sizeof(D3DDEVICEDESC);
    ddSwDesc.dwSize = sizeof(D3DDEVICEDESC);
    pd3dDevice->GetCaps( &ddHwDesc, &ddSwDesc );

    // Check if the device supports single pass multiple texture.
    if( ddHwDesc.wMaxSimultaneousTextures > 1 )
        if( ddHwDesc.dwTextureOpCaps & D3DTEXOPCAPS_MODULATE )
            g_bCanDoMultiTexture = TRUE;
    if( ddSwDesc.wMaxSimultaneousTextures > 1 )
        if( ddSwDesc.dwTextureOpCaps & D3DTEXOPCAPS_MODULATE )
            g_bCanDoMultiTexture = TRUE;

    // Check whether device can do mulit-pass color blending
    if( ddHwDesc.dpcTriCaps.dwDestBlendCaps & D3DPBLENDCAPS_SRCCOLOR )
        if( ddHwDesc.dpcTriCaps.dwSrcBlendCaps & D3DPBLENDCAPS_ZERO )
            g_bCanDoColorBlend = TRUE;
    if( ddSwDesc.dpcTriCaps.dwDestBlendCaps & D3DPBLENDCAPS_SRCCOLOR )
        if( ddSwDesc.dpcTriCaps.dwSrcBlendCaps & D3DPBLENDCAPS_ZERO )
            g_bCanDoColorBlend = TRUE;

    // Check whether device can do multi-pass blending w/alpha textures
    if( g_ptexAlphaLightMap )
    {
        if( ddHwDesc.dpcTriCaps.dwDestBlendCaps & D3DPBLENDCAPS_INVSRCALPHA )
            if( ddHwDesc.dpcTriCaps.dwSrcBlendCaps & D3DPBLENDCAPS_SRCALPHA )
                g_bCanDoAlphaBlend = TRUE;
        if( ddSwDesc.dpcTriCaps.dwDestBlendCaps & D3DPBLENDCAPS_INVSRCALPHA )
            if( ddSwDesc.dpcTriCaps.dwSrcBlendCaps & D3DPBLENDCAPS_SRCALPHA )
                g_bCanDoAlphaBlend = TRUE;
    }

    // Set the menu states for multitexture devices and devices that emulate
    // multitexture using multipass rendering.
    g_wTextureMode = TEXTURE_NONE;
    EnableMenuItem( g_hMenu, IDM_MULTITEXTURE,   MF_GRAYED );
    EnableMenuItem( g_hMenu, IDM_MULTIPASSCOLOR, MF_GRAYED );
    EnableMenuItem( g_hMenu, IDM_MULTIPASSALPHA, MF_GRAYED );

    if( g_bCanDoAlphaBlend )
    {
        g_wTextureMode = TEXTURE_MULTIPASSALPHA;
        EnableMenuItem( g_hMenu, IDM_MULTIPASSALPHA, MF_ENABLED );
        CheckMenuItem(  g_hMenu, IDM_MULTITEXTURE,   MF_UNCHECKED );
        CheckMenuItem(  g_hMenu, IDM_MULTIPASSCOLOR, MF_UNCHECKED );
        CheckMenuItem(  g_hMenu, IDM_MULTIPASSALPHA, MF_CHECKED );
    }
    if( g_bCanDoColorBlend )
    {
        g_wTextureMode = TEXTURE_MULTIPASSCOLOR;
        EnableMenuItem( g_hMenu, IDM_MULTIPASSCOLOR, MF_ENABLED );
        CheckMenuItem(  g_hMenu, IDM_MULTITEXTURE,   MF_UNCHECKED );
        CheckMenuItem(  g_hMenu, IDM_MULTIPASSCOLOR, MF_CHECKED );
        CheckMenuItem(  g_hMenu, IDM_MULTIPASSALPHA, MF_UNCHECKED );
    }
    if( g_bCanDoMultiTexture )
    {
        g_wTextureMode = TEXTURE_SINGLEPASS;
        EnableMenuItem( g_hMenu, IDM_MULTITEXTURE,   MF_ENABLED );
        CheckMenuItem(  g_hMenu, IDM_MULTITEXTURE,   MF_CHECKED );
        CheckMenuItem(  g_hMenu, IDM_MULTIPASSCOLOR, MF_UNCHECKED );
        CheckMenuItem(  g_hMenu, IDM_MULTIPASSALPHA, MF_UNCHECKED );
    }

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

    // Release the alpha lightmap which was explicity created by this app
    SAFE_RELEASE( g_ptexAlphaLightMap );
}




//----------------------------------------------------------------------------
// Name: App_RestoreSurfaces
// Desc: Restores any previously lost surfaces. Must do this for all surfaces
//       (including textures) that the app created.
//----------------------------------------------------------------------------
HRESULT App_RestoreSurfaces( )
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
    // Accept devices that really support multiple textures. 
    if( pd3dDeviceDesc->wMaxSimultaneousTextures > 1 )
        if( pd3dDeviceDesc->dwTextureOpCaps & D3DTEXOPCAPS_MODULATE )
            return S_OK;

    // Accept devices that can do multipass, color blending
    if( pd3dDeviceDesc->dpcTriCaps.dwDestBlendCaps & D3DPBLENDCAPS_SRCCOLOR )
        if( pd3dDeviceDesc->dpcTriCaps.dwSrcBlendCaps & D3DPBLENDCAPS_ZERO )
            return S_OK;

    // Accept devices that can do multipass, alpha blending
    if( pd3dDeviceDesc->dpcTriCaps.dwDestBlendCaps & D3DPBLENDCAPS_INVSRCALPHA )
        if( pd3dDeviceDesc->dpcTriCaps.dwSrcBlendCaps & D3DPBLENDCAPS_SRCALPHA )
            return S_OK;

    return E_FAIL;
}




//-----------------------------------------------------------------------------
// Name: App_OverridenWndProc()
// Desc: Overrrides the main WndProc, so the sample can do custom message 
//       handling (e.g. processing mouse, keyboard, or menu commands).
//-----------------------------------------------------------------------------
LRESULT CALLBACK App_OverridenWndProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                       LPARAM lParam )
{
    if( WM_COMMAND == uMsg )
    {
        switch( LOWORD(wParam) )
        {
            case IDM_MULTITEXTURE:
                g_wTextureMode = TEXTURE_SINGLEPASS;
                CheckMenuItem( g_hMenu, IDM_MULTITEXTURE,      MF_CHECKED   );
                CheckMenuItem( g_hMenu, IDM_MULTIPASSCOLOR,    MF_UNCHECKED );
                CheckMenuItem( g_hMenu, IDM_MULTIPASSALPHA,    MF_UNCHECKED );
                break;
            case IDM_MULTIPASSCOLOR:
                g_wTextureMode = TEXTURE_MULTIPASSCOLOR;
                CheckMenuItem( g_hMenu, IDM_MULTITEXTURE,      MF_UNCHECKED   );
                CheckMenuItem( g_hMenu, IDM_MULTIPASSCOLOR,    MF_CHECKED );
                CheckMenuItem( g_hMenu, IDM_MULTIPASSALPHA,    MF_UNCHECKED );
                break;
            case IDM_MULTIPASSALPHA:
                g_wTextureMode = TEXTURE_MULTIPASSALPHA;
                CheckMenuItem( g_hMenu, IDM_MULTITEXTURE,      MF_UNCHECKED   );
                CheckMenuItem( g_hMenu, IDM_MULTIPASSCOLOR,    MF_UNCHECKED );
                CheckMenuItem( g_hMenu, IDM_MULTIPASSALPHA,    MF_CHECKED );
                break;
        }
    }
    return WndProc( hWnd, uMsg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: App_CreateInvAlphaTexFromTex()
// Desc: Copy (and convert) a 565 texture into a new 4444 ARGB texture.
//       The complement of the upper 4 of 5 source R bits are copied into the
//       four dest A bits. The complement is equivalent to 1-alpha when the
//       alpha scale is from 0 to 1. The dest RGB values are set to 0.
//
//       Note that the source texture is ASSUMED to be 565!
//-----------------------------------------------------------------------------
HRESULT App_CreateInvAlphaTexFromTex( LPDIRECT3DTEXTURE2* pptexDst,
                                      LPDIRECT3DTEXTURE2 ptexSrc, 
                                      DDPIXELFORMAT* pddpf )
{
    LPDIRECTDRAWSURFACE4 pddsSrc, pddsDst;
    LPDIRECTDRAW4        pDD;
    HRESULT              hr;

    // Initially set the destination texture ptr to NULL...in case of failure
    (*pptexDst) = NULL;

    // Get the surface for the source texture. Call Release() to keep the
    // ref count from increasing; the ptr will still be valid
    if( FAILED( hr = ptexSrc->QueryInterface( IID_IDirectDrawSurface4,
                                                 (VOID**)&pddsSrc ) ) )
        return hr;
    pddsSrc->Release();

    // Get the DDraw object from the src surface. The Release() call is
    // simply to keep the ref count from increasing.
    if( FAILED( pddsSrc->GetDDInterface( (VOID**)&pDD ) ) )
        return E_FAIL;
    pDD->Release();

    // Prepare a DDSURFACEDESC to create the destination texture surface
    DDSURFACEDESC2 ddsd;
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    if( FAILED( hr = pddsSrc->GetSurfaceDesc( &ddsd ) ) )
        return hr;

    // Modify the surface description obtained from the source texture to 
    // be 4444 ARGB, for creating the new texture.
    ddsd.dwFlags         = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE;
    ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
    memcpy( &ddsd.ddpfPixelFormat, pddpf, sizeof(DDPIXELFORMAT) );

    // Create the destination texture's surface
    if( FAILED( hr = pDD->CreateSurface( &ddsd, &pddsDst, NULL ) ) )
        return hr;

    // Lock the surfaces to get access to their bits
    pddsSrc->Lock( NULL, &ddsd, DDLOCK_WAIT, 0 );
    DWORD lSrcPitch = ddsd.lPitch;
    WORD* pSrc      = (WORD*)ddsd.lpSurface;

    pddsDst->Lock( NULL, &ddsd, DDLOCK_WAIT, 0 );
    DWORD lDstPitch = ddsd.lPitch;
    WORD* pDst      = (WORD*)ddsd.lpSurface;

    // Copy the source texture to the new texture, converting RGB to ARGB in
    // the process, with alpha inverted and all new RGB bits set to 0.
    // Bitwise-notting the alpha bits is equivalent to doing 1-alpha.
    for( DWORD y = 0; y < ddsd.dwHeight; y++ )
    {
        WORD* pCurrentSrcWord = pSrc;
        WORD* pCurrentDstWord = pDst;

        for( DWORD x = 0; x < ddsd.dwWidth; x++ )
        {
            // Mask the most significant 4 bits of the red component from the
            // source 565 RGB texture, and use it as the alpha component of the
            // destination 4444 ARGB texture.
            *pCurrentDstWord = 0xf000 & (~(*pCurrentSrcWord));
            
            pCurrentDstWord++;
            pCurrentSrcWord++;
        }
        pSrc += lSrcPitch/sizeof(WORD); // Move to the next line
        pDst += lDstPitch/sizeof(WORD);
    }

    pddsDst->Unlock(0);
    pddsSrc->Unlock(0);
    
    // Get the texture interface for the new 4444 ARGB texture
    hr = pddsDst->QueryInterface( IID_IDirect3DTexture2, (VOID**)pptexDst );
    pddsDst->Release();
    
    return hr;
}





