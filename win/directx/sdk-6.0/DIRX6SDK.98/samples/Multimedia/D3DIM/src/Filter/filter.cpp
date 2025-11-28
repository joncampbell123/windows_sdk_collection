//-----------------------------------------------------------------------------
// File: Filter.cpp
//
// Desc: Example code showing how to enable various filtering modes in
//       Direct3D.
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
#include "resource.h"


//-----------------------------------------------------------------------------
// Declare the application globals for use in WinMain.cpp
//-----------------------------------------------------------------------------
TCHAR* g_strAppTitle       = TEXT( "Filter: DX6 Filtering Sample" );
BOOL   g_bAppUseZBuffer    = FALSE;
BOOL   g_bAppUseBackBuffer = TRUE;




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
LPDIRECT3DMATERIAL3 g_pmtrlObjectMtrl     = NULL;
D3DVERTEX           g_pvVertices[4];
HMENU               g_hMenu;

// Filter modes
D3DTEXTUREMAGFILTER g_dwLeftTexMagState,     g_dwRightTexMagState;
D3DTEXTUREMINFILTER g_dwLeftTexMinState,     g_dwRightTexMinState;
DWORD               g_dwLeftAnisotropyLevel, g_dwRightAnisotropyLevel;
D3DANTIALIASMODE    g_dwLeftAntialiasMode,   g_dwRightAntialiasMode;

// Device capabilities
BOOL                g_bDeviceDoesFlatCubic;
BOOL                g_bDeviceDoesGaussianCubic;
DWORD               g_dwDeviceMaxAnisotropy;
BOOL                g_bDeviceDoesSortDependantAA;
BOOL                g_bDeviceDoesSortIndependantAA;




//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
VOID    AppPause( BOOL );
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
HRESULT UpdateDeviceCapabilities( LPDIRECT3DDEVICE3 );

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK App_OverridenWndProc( HWND, UINT, WPARAM, LPARAM );
VOID             SetMenuStates( HMENU hMenu );




//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{
    // Add a menu and a message handler to the program
    g_hMenu = GetMenu( hWnd );
    SetWindowLong( hWnd, GWL_WNDPROC, (LONG)App_OverridenWndProc );

    D3DVECTOR vNorm( 0.0f, 0.0f, 1.0f );
    g_pvVertices[0] = D3DVERTEX( D3DVECTOR( 1, 1, 0), vNorm, 1.0f, 0.0f );
    g_pvVertices[1] = D3DVERTEX( D3DVECTOR( 1,-1, 0), vNorm, 1.0f, 1.0f );
    g_pvVertices[2] = D3DVERTEX( D3DVECTOR(-1, 1, 0), vNorm, 0.0f, 0.0f );
    g_pvVertices[3] = D3DVERTEX( D3DVECTOR(-1,-1, 0), vNorm, 0.0f, 1.0f );

    // Create some textures
    D3DTextr_CreateTexture( "Filter.bmp" );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT App_FrameMove( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{
    D3DMATRIX matWorldSpin;
    D3DUtil_SetRotateXMatrix( matWorldSpin, (FLOAT)sin( fTimeKey ) );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorldSpin );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT App_Render( LPDIRECT3DDEVICE3 pd3dDevice, 
                    LPDIRECT3DVIEWPORT3 pvViewport,
                    D3DRECT* prcViewportRect )
{
    //Clear the viewport
    pvViewport->Clear2( 1UL, prcViewportRect, D3DCLEAR_TARGET, 0x003399ff, 0L, 0L );

    // Begin the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        D3DMATRIX matWorld;
        pd3dDevice->GetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );

        // Set the filter states for the left image
        pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER,     g_dwLeftTexMagState );
        pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER,     g_dwLeftTexMinState );
        pd3dDevice->SetTextureStageState( 0, D3DTSS_MAXANISOTROPY, g_dwLeftAnisotropyLevel );
        pd3dDevice->SetRenderState( D3DRENDERSTATE_ANTIALIAS,      g_dwLeftAntialiasMode );

        // Draw the left image
        matWorld._41 = -1.1f;
        pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
        pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX,
                                   g_pvVertices, 4, NULL );

        // Set the filter states for the left image
        pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER,     g_dwRightTexMagState );
        pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER,     g_dwRightTexMinState );
        pd3dDevice->SetTextureStageState( 0, D3DTSS_MAXANISOTROPY, g_dwRightAnisotropyLevel );
        pd3dDevice->SetRenderState( D3DRENDERSTATE_ANTIALIAS,      g_dwRightAntialiasMode );

        // Draw the right image
        matWorld._41 = 1.1f;
        pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
        pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX,
                                   g_pvVertices, 4, NULL );
        
        // End the scene.
        pd3dDevice->EndScene();
    }

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

    // Get a ptr to the ID3D object to create materials and/or lights. Note:
    // the Release() call just serves to decrease the ref count.
    LPDIRECT3D3 pD3D;
    if( FAILED( pd3dDevice->GetDirect3D( &pD3D ) ) )
        return E_FAIL;
    pD3D->Release();

    HRESULT           hr;
    D3DMATERIAL       mtrl;
    D3DMATERIALHANDLE hmtrl;
    
    // Create and set up the shine materials w/ textures
    if( FAILED( hr = pD3D->CreateMaterial( &g_pmtrlObjectMtrl, NULL ) ) )
        return E_FAIL;

    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    mtrl.power = 40.0f;
    g_pmtrlObjectMtrl->SetMaterial( &mtrl );
    g_pmtrlObjectMtrl->GetHandle( pd3dDevice, &hmtrl );
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_MATERIAL, hmtrl );

    pd3dDevice->SetLightState(  D3DLIGHTSTATE_AMBIENT,  0xffffffff );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, 0 );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_CLAMP );

    D3DTextr_RestoreAllTextures( pd3dDevice );
    pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("Filter.bmp") );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );

    // Set the transform matrices
    D3DVECTOR vEyePt    = D3DVECTOR( 0, 0, -2.5 );
    D3DVECTOR vLookatPt = D3DVECTOR( 0, 0,   0  );
    D3DVECTOR vUpVec    = D3DVECTOR( 0, 1,   0  );
    D3DMATRIX matView, matProj;

    D3DUtil_SetViewMatrix( matView, vEyePt, vLookatPt, vUpVec );
    D3DUtil_SetProjectionMatrix( matProj, 1.57f, 1.0f, 1.0f, 100.0f );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW,       &matView );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

    UpdateDeviceCapabilities( pd3dDevice );

    return hr;
}




//-----------------------------------------------------------------------------
// Name: App_FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT App_FinalCleanup( LPDIRECT3DDEVICE3 pd3dDevice, 
                          LPDIRECT3DVIEWPORT3 pvViewport)
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
                              LPDIRECT3DVIEWPORT3 pvViewport)
{
    D3DTextr_InvalidateAllTextures();
    SAFE_RELEASE( g_pmtrlObjectMtrl );
}




//-----------------------------------------------------------------------------
// Name: App_RestoreSurfaces
// Desc: Restores any previously lost surfaces. Must do this for all surfaces
//       (including textures) that the app created.
//-----------------------------------------------------------------------------
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
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateDeviceCapabilities()
// Desc: Called for a new device, this functions checks the capabilities of
//       the device and stores them in global variables and updates the menu
//       accordingly.
//-----------------------------------------------------------------------------
HRESULT UpdateDeviceCapabilities( LPDIRECT3DDEVICE3 pd3dDevice )
{
    D3DDEVICEDESC ddHELDesc, ddHALDesc;
    ddHELDesc.dwSize = sizeof(D3DDEVICEDESC);
    ddHALDesc.dwSize = sizeof(D3DDEVICEDESC);
    pd3dDevice->GetCaps( &ddHALDesc, &ddHELDesc );

    // Initialize the default filtering states
    g_bDeviceDoesFlatCubic         = FALSE;
    g_bDeviceDoesGaussianCubic     = FALSE;
    g_dwDeviceMaxAnisotropy        = 1;
    g_bDeviceDoesSortDependantAA   = FALSE;
    g_bDeviceDoesSortIndependantAA = FALSE;
    
    g_dwLeftTexMagState      = D3DTFG_POINT;
    g_dwLeftTexMinState      = D3DTFN_POINT;
    g_dwLeftAnisotropyLevel  = 1;
    g_dwLeftAntialiasMode    = D3DANTIALIAS_NONE;
    g_dwRightTexMagState     = D3DTFG_POINT;
    g_dwRightTexMinState     = D3DTFN_POINT;
    g_dwRightAnisotropyLevel = 1;
    g_dwRightAntialiasMode   = D3DANTIALIAS_NONE;

    // Get the device caps
    D3DDEVICEDESC* pDesc = (ddHALDesc.dwFlags) ? &ddHALDesc : &ddHELDesc;
    DWORD dwRasterCaps = pDesc->dpcTriCaps.dwRasterCaps;
    DWORD dwFilterCaps = pDesc->dpcTriCaps.dwTextureFilterCaps;

    // Check the device for supported filtering methods
    if( dwRasterCaps & D3DPRASTERCAPS_ANISOTROPY )
        g_dwDeviceMaxAnisotropy = pDesc->dwMaxAnisotropy;
    if( dwRasterCaps & D3DPRASTERCAPS_ANTIALIASSORTDEPENDENT )
        g_bDeviceDoesSortDependantAA = TRUE;
    if( dwRasterCaps & D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT )
        g_bDeviceDoesSortIndependantAA = TRUE;
    if( dwFilterCaps & D3DPTFILTERCAPS_MAGFAFLATCUBIC )
        g_bDeviceDoesFlatCubic = TRUE;
    if( dwFilterCaps & D3DPTFILTERCAPS_MAGFGAUSSIANCUBIC )
        g_bDeviceDoesGaussianCubic = TRUE;

    SetMenuStates( g_hMenu );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetMenuStates()
// Desc: Overrrides the main WndProc, so the sample can do custom message 
//       handling (e.g. processing mouse, keyboard, or menu commands).
//-----------------------------------------------------------------------------
VOID SetMenuStates( HMENU hMenu )
{
    // Enable the appropiate menu items
    DWORD dwAnisotropy2 = ( g_dwDeviceMaxAnisotropy >= 2L ) ? MF_ENABLED : MF_GRAYED;
    DWORD dwAnisotropy4 = ( g_dwDeviceMaxAnisotropy >= 4L ) ? MF_ENABLED : MF_GRAYED;
    DWORD dwAnisotropy8 = ( g_dwDeviceMaxAnisotropy >= 8L ) ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem( hMenu, IDM_LEFT_MAGANISOTROPIC,  dwAnisotropy2 );
    EnableMenuItem( hMenu, IDM_LEFT_MINANISOTROPIC,  dwAnisotropy2 );
    EnableMenuItem( hMenu, IDM_LEFT_ANISOTROPY2,     dwAnisotropy2 );
    EnableMenuItem( hMenu, IDM_LEFT_ANISOTROPY4,     dwAnisotropy4 );
    EnableMenuItem( hMenu, IDM_LEFT_ANISOTROPY8,     dwAnisotropy8 );
    EnableMenuItem( hMenu, IDM_RIGHT_MAGANISOTROPIC, dwAnisotropy2 );
    EnableMenuItem( hMenu, IDM_RIGHT_MINANISOTROPIC, dwAnisotropy2 );
    EnableMenuItem( hMenu, IDM_RIGHT_ANISOTROPY2,    dwAnisotropy2 );
    EnableMenuItem( hMenu, IDM_RIGHT_ANISOTROPY4,    dwAnisotropy4 );
    EnableMenuItem( hMenu, IDM_RIGHT_ANISOTROPY8,    dwAnisotropy8 );

    DWORD dwSortDep   = ( g_bDeviceDoesSortDependantAA ) ? MF_ENABLED : MF_GRAYED;
    DWORD dwSortIndep = ( g_bDeviceDoesSortIndependantAA ) ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem( hMenu, IDM_LEFT_SORTDEPENDENT,    dwSortDep );
    EnableMenuItem( hMenu, IDM_LEFT_SORTINDEPENDENT,  dwSortIndep );
    EnableMenuItem( hMenu, IDM_RIGHT_SORTDEPENDENT,   dwSortDep );
    EnableMenuItem( hMenu, IDM_RIGHT_SORTINDEPENDENT, dwSortIndep );

    DWORD dwFlatCubic = ( TRUE == g_bDeviceDoesFlatCubic ) ? MF_ENABLED : MF_GRAYED;
    DWORD dwGaussian  = ( TRUE == g_bDeviceDoesGaussianCubic ) ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem( hMenu, IDM_LEFT_MAGFLATCUBIC,      dwFlatCubic );
    EnableMenuItem( hMenu, IDM_LEFT_MAGGAUSSIANCUBIC,  dwGaussian );
    EnableMenuItem( hMenu, IDM_RIGHT_MAGFLATCUBIC,     dwFlatCubic );
    EnableMenuItem( hMenu, IDM_RIGHT_MAGGAUSSIANCUBIC, dwGaussian );
    
    // Put checks by the appropiate menu items
    CheckMenuItem( hMenu, IDM_LEFT_MAGPOINT,
                   ( g_dwLeftTexMagState == D3DTFG_POINT ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_LEFT_MAGLINEAR,
                   ( g_dwLeftTexMagState == D3DTFG_LINEAR ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_LEFT_MAGFLATCUBIC,
                   ( g_dwLeftTexMagState == D3DTFG_FLATCUBIC ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_LEFT_MAGGAUSSIANCUBIC,
                   ( g_dwLeftTexMagState == D3DTFG_GAUSSIANCUBIC ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_LEFT_MAGANISOTROPIC,
                   ( g_dwLeftTexMagState == D3DTFG_ANISOTROPIC ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_LEFT_MINPOINT,
                   ( g_dwLeftTexMinState == D3DTFN_POINT ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_LEFT_MINLINEAR,
                   ( g_dwLeftTexMinState == D3DTFN_LINEAR ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_LEFT_MINANISOTROPIC,
                   ( g_dwLeftTexMinState == D3DTFN_ANISOTROPIC ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_LEFT_ANISOTROPY1,
                   ( g_dwLeftAnisotropyLevel == 1 ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_LEFT_ANISOTROPY2,
                   ( g_dwLeftAnisotropyLevel == 2 ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_LEFT_ANISOTROPY4,
                   ( g_dwLeftAnisotropyLevel == 4 ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_LEFT_ANISOTROPY8,
                   ( g_dwLeftAnisotropyLevel == 8 ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_LEFT_NOANTIALIAS,
                   ( g_dwLeftAntialiasMode == D3DANTIALIAS_NONE ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_LEFT_SORTDEPENDENT,
                   ( g_dwLeftAntialiasMode == D3DANTIALIAS_SORTDEPENDENT ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_LEFT_SORTINDEPENDENT,
                   ( g_dwLeftAntialiasMode == D3DANTIALIAS_SORTINDEPENDENT ) ? MF_CHECKED : MF_UNCHECKED );

    CheckMenuItem( hMenu, IDM_RIGHT_MAGPOINT,
                   ( g_dwRightTexMagState == D3DTFG_POINT ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_RIGHT_MAGLINEAR,
                   ( g_dwRightTexMagState == D3DTFG_LINEAR ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_RIGHT_MAGFLATCUBIC,
                   ( g_dwRightTexMagState == D3DTFG_FLATCUBIC ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_RIGHT_MAGGAUSSIANCUBIC,
                   ( g_dwRightTexMagState == D3DTFG_GAUSSIANCUBIC ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_RIGHT_MAGANISOTROPIC,
                   ( g_dwRightTexMagState == D3DTFG_ANISOTROPIC ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_RIGHT_MINPOINT,
                   ( g_dwRightTexMinState == D3DTFN_POINT ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_RIGHT_MINLINEAR,
                   ( g_dwRightTexMinState == D3DTFN_LINEAR ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_RIGHT_MINANISOTROPIC,
                   ( g_dwRightTexMinState == D3DTFN_ANISOTROPIC ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_RIGHT_ANISOTROPY1,
                   ( g_dwRightAnisotropyLevel == 1 ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_RIGHT_ANISOTROPY2,
                   ( g_dwRightAnisotropyLevel == 2 ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_RIGHT_ANISOTROPY4,
                   ( g_dwRightAnisotropyLevel == 4 ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_RIGHT_ANISOTROPY8,
                   ( g_dwRightAnisotropyLevel == 8 ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_RIGHT_NOANTIALIAS,
                   ( g_dwRightAntialiasMode == D3DANTIALIAS_NONE ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_RIGHT_SORTDEPENDENT,
                   ( g_dwRightAntialiasMode == D3DANTIALIAS_SORTDEPENDENT ) ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_RIGHT_SORTINDEPENDENT,
                   ( g_dwRightAntialiasMode == D3DANTIALIAS_SORTINDEPENDENT ) ? MF_CHECKED : MF_UNCHECKED );
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
            case IDM_LEFT_MAGPOINT:
                g_dwLeftTexMagState = D3DTFG_POINT;
                break;
            case IDM_LEFT_MAGLINEAR:
                g_dwLeftTexMagState = D3DTFG_LINEAR;
                break;
            case IDM_LEFT_MAGFLATCUBIC:
                g_dwLeftTexMagState = D3DTFG_FLATCUBIC;
                break;
            case IDM_LEFT_MAGGAUSSIANCUBIC:
                g_dwLeftTexMagState = D3DTFG_GAUSSIANCUBIC;
                break;
            case IDM_LEFT_MAGANISOTROPIC:
                g_dwLeftTexMagState = D3DTFG_ANISOTROPIC;
                break;
            case IDM_LEFT_MINPOINT:
                g_dwLeftTexMinState = D3DTFN_POINT;
                break;
            case IDM_LEFT_MINLINEAR:
                g_dwLeftTexMinState = D3DTFN_LINEAR;
                break;
            case IDM_LEFT_MINANISOTROPIC:
                g_dwLeftTexMinState = D3DTFN_ANISOTROPIC;
                break;
            case IDM_LEFT_ANISOTROPY1:
                g_dwLeftAnisotropyLevel = 1;
                break;
            case IDM_LEFT_ANISOTROPY2:
                g_dwLeftAnisotropyLevel = 2;
                break;
            case IDM_LEFT_ANISOTROPY4:
                g_dwLeftAnisotropyLevel = 4;
                break;
            case IDM_LEFT_ANISOTROPY8:
                g_dwLeftAnisotropyLevel = 8;
                break;
            case IDM_LEFT_NOANTIALIAS:
                g_dwLeftAntialiasMode = D3DANTIALIAS_NONE;
                break;
            case IDM_LEFT_SORTDEPENDENT:
                g_dwLeftAntialiasMode = D3DANTIALIAS_SORTDEPENDENT;
                break;
            case IDM_LEFT_SORTINDEPENDENT:
                g_dwLeftAntialiasMode = D3DANTIALIAS_SORTINDEPENDENT;
                break;

            case IDM_RIGHT_MAGPOINT:
                g_dwRightTexMagState = D3DTFG_POINT;
                break;
            case IDM_RIGHT_MAGLINEAR:
                g_dwRightTexMagState = D3DTFG_LINEAR;
                break;
            case IDM_RIGHT_MAGFLATCUBIC:
                g_dwRightTexMagState = D3DTFG_FLATCUBIC;
                break;
            case IDM_RIGHT_MAGGAUSSIANCUBIC:
                g_dwRightTexMagState = D3DTFG_GAUSSIANCUBIC;
                break;
            case IDM_RIGHT_MAGANISOTROPIC:
                g_dwRightTexMagState = D3DTFG_ANISOTROPIC;
                break;
            case IDM_RIGHT_MINPOINT:
                g_dwRightTexMinState = D3DTFN_POINT;
                break;
            case IDM_RIGHT_MINLINEAR:
                g_dwRightTexMinState = D3DTFN_LINEAR;
                break;
            case IDM_RIGHT_MINANISOTROPIC:
                g_dwRightTexMinState = D3DTFN_ANISOTROPIC;
                break;
            case IDM_RIGHT_ANISOTROPY1:
                g_dwRightAnisotropyLevel = 1;
                break;
            case IDM_RIGHT_ANISOTROPY2:
                g_dwRightAnisotropyLevel = 2;
                break;
            case IDM_RIGHT_ANISOTROPY4:
                g_dwRightAnisotropyLevel = 4;
                break;
            case IDM_RIGHT_ANISOTROPY8:
                g_dwRightAnisotropyLevel = 8;
                break;
            case IDM_RIGHT_NOANTIALIAS:
                g_dwRightAntialiasMode = D3DANTIALIAS_NONE;
                break;
            case IDM_RIGHT_SORTDEPENDENT:
                g_dwRightAntialiasMode = D3DANTIALIAS_SORTDEPENDENT;
                break;
            case IDM_RIGHT_SORTINDEPENDENT:
                g_dwRightAntialiasMode = D3DANTIALIAS_SORTINDEPENDENT;
                break;
        }

        // Update the menu to reflect any new changes
        SetMenuStates( g_hMenu );
    }
    return WndProc( hWnd, uMsg, wParam, lParam );
}


