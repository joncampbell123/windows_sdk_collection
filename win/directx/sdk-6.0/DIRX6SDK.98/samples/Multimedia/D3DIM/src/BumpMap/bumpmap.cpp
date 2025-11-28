//-----------------------------------------------------------------------------
// File: BumpMap.cpp
//
// Desc: Direct3D environment mapping / bump mapping sample. The technique
//       used perturbs the environment map to simulate bump mapping.
//
//
// Copyright (c) 1997-1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define D3D_OVERLOADS
#define STRICT
#include <d3d.h>
#include <math.h>
#include <stdio.h>
#include "D3DUtil.h"
#include "D3DTextr.h"
#include "D3DMath.h"
#include "resource.h"


//-----------------------------------------------------------------------------
// Declare the application globals for use in WinMain.cpp
//-----------------------------------------------------------------------------
TCHAR* g_strAppTitle       = TEXT("Bumpmap: Direct3D EnvMap/BumpMap Demo");
BOOL   g_bAppUseZBuffer    = FALSE;
BOOL   g_bAppUseBackBuffer = TRUE;




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
LPDIRECT3DDEVICE3    g_pd3dDevice;
LPDIRECT3DMATERIAL3  g_pD3DMaterial = NULL;
LPDIRECTDRAWSURFACE4 g_pddsBumpMap  = NULL;
LPDIRECT3DTEXTURE2   g_ptexBumpMap  = NULL;
HMENU                g_hMenu;

#define SPHERE_NUM_RINGS    14
#define SPHERE_NUM_VERTICES (SPHERE_NUM_RINGS*12)*(SPHERE_NUM_RINGS-1)

struct BUMPVERTEX
{
    D3DVERTEX v;
    FLOAT     tu2, tv2;
};

BUMPVERTEX g_SphereVertices[SPHERE_NUM_VERTICES];




//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    InitSphereVertices();
VOID    AppOutputText( LPDIRECT3DDEVICE3, DWORD, DWORD, CHAR* );
LRESULT CALLBACK App_OverridenWndProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

enum BUMPMAPFORMAT { BUMPMAP_U5V5L6, BUMPMAP_U8V8L8, BUMPMAP_U8V8 };

BOOL          g_bTextureOn    = TRUE;
BOOL          g_bBumpMapOn    = TRUE;
BOOL          g_bEnvMapOn     = TRUE;
BUMPMAPFORMAT g_BumpMapFormat = BUMPMAP_U5V5L6;




//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{
    // Add a menu and a WndProc function for handling keyboard input
    g_hMenu = GetMenu( hWnd );
    SetWindowLong( hWnd, GWL_WNDPROC, (LONG)App_OverridenWndProc );

    // Initialize geometry
    InitSphereVertices();

    // Load texture maps
    D3DTextr_CreateTexture( "block.bmp" );
    D3DTextr_CreateTexture( "earth.bmp" );
    D3DTextr_CreateTexture( "earthbump.bmp", 0, D3DTEXTR_32BITSPERPIXEL );
    D3DTextr_CreateTexture( "EarthEnvMap.bmp" );

    return S_OK;
}




DWORD FLOATtoDWORD( FLOAT f )
{
    union FLOATDWORD
    {
        FLOAT f;
        DWORD dw;
    };

    FLOATDWORD val;
    val.f = f;
    return val.dw;
}

    


//-----------------------------------------------------------------------------
// Name: ApplyEnivronmentMap()
// Desc: Performs a calculation on each of the vertices' normals to determine
//       what the texture coordinates should be for the environment map.
//-----------------------------------------------------------------------------
VOID ApplyEnivronmentMap( BUMPVERTEX* pv, DWORD dwNumVertices,
                          LPDIRECT3DDEVICE3 pd3dDevice )
{
    // Get the World-View(WV) and Projection(P) matrices
    D3DMATRIX W, V, WV;
    pd3dDevice->GetTransform( D3DTRANSFORMSTATE_VIEW,      &V );
    pd3dDevice->GetTransform( D3DTRANSFORMSTATE_WORLD,     &W );
    pd3dDevice->MultiplyTransform( D3DTRANSFORMSTATE_VIEW, &W );
    pd3dDevice->GetTransform( D3DTRANSFORMSTATE_VIEW,      &WV );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW,      &V );

    // Loop through the vertices, transforming each one and calculating
    // the correct texture coordinates.
    for( WORD i = 0; i < dwNumVertices; i++ )
    {
        FLOAT nx = pv[i].v.nx;
        FLOAT ny = pv[i].v.ny;
        FLOAT nz = pv[i].v.nz;
    
        FLOAT nxv = nx*WV._11 + ny*WV._21 + nz*WV._31 + WV._41;
        FLOAT nyv = nx*WV._12 + ny*WV._22 + nz*WV._32 + WV._42;
        FLOAT nzv = nx*WV._13 + ny*WV._23 + nz*WV._33 + WV._43;
        FLOAT nwv = nx*WV._14 + ny*WV._24 + nz*WV._34 + WV._44;

        FLOAT nlen = (FLOAT)sqrt( nxv*nxv + nyv*nyv + nzv*nzv );

        pv[i].v.tu = 0.5f + 1.2f*nxv/nlen;
        pv[i].v.tv = 0.5f - 1.2f*nyv/nlen;
    }
}




//-----------------------------------------------------------------------------
// Name: App_FrameMove()
// Desc: Animates the scene
//-----------------------------------------------------------------------------
HRESULT App_FrameMove( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{
    FLOAT fAngle = 30.0f;
    D3DMATRIX matWorld, matRotate;
    D3DUtil_SetRotateYMatrix( matRotate, -fAngle * (g_PI/180) );

    pd3dDevice->GetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
    D3DMath_MatrixMultiply( matWorld, matWorld, matRotate );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );

    ApplyEnivronmentMap( g_SphereVertices, SPHERE_NUM_VERTICES, pd3dDevice );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_Render()
// Desc: Renders the scene.
//-----------------------------------------------------------------------------
HRESULT App_Render( LPDIRECT3DDEVICE3 pd3dDevice, 
                    LPDIRECT3DVIEWPORT3 pvViewport, D3DRECT* prcViewportRect )
{
    pvViewport->Clear( 1, prcViewportRect, D3DCLEAR_TARGET );

    if( FAILED( pd3dDevice->BeginScene() ) )
        return S_OK; // Don't return a "fatal" error

    pd3dDevice->SetRenderState( D3DRENDERSTATE_WRAP0, D3DWRAP_U | D3DWRAP_V );
    pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE );
    pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
    pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE );

    if( g_bTextureOn )
        pd3dDevice->SetTexture(0, D3DTextr_GetTexture( "earth.bmp" ) );
    else
        pd3dDevice->SetTexture(0, D3DTextr_GetTexture( "block.bmp" ) );
    
    pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 1 );
    pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

    if( g_bBumpMapOn )
    {
        pd3dDevice->SetTexture(1, g_ptexBumpMap );
        pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1 );
        pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_BUMPENVMAPLUMINANCE );
        pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT );
        pd3dDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT00, FLOATtoDWORD(0.5f) );
        pd3dDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT01, FLOATtoDWORD(0.0f) );
        pd3dDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT10, FLOATtoDWORD(0.0f) );
        pd3dDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT11, FLOATtoDWORD(0.5f) );
        pd3dDevice->SetTextureStageState(1, D3DTSS_BUMPENVLSCALE, FLOATtoDWORD( 1.0f) );
        pd3dDevice->SetTextureStageState(1, D3DTSS_BUMPENVLOFFSET, FLOATtoDWORD(0.0f) );

        if( g_bEnvMapOn )
        {
            pd3dDevice->SetTexture(2, D3DTextr_GetTexture( "EarthEnvMap.bmp" ) );
            pd3dDevice->SetTextureStageState(2, D3DTSS_TEXCOORDINDEX, 0 );
            pd3dDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_ADD );
            pd3dDevice->SetTextureStageState(2, D3DTSS_COLORARG1, D3DTA_TEXTURE );
            pd3dDevice->SetTextureStageState(2, D3DTSS_COLORARG2, D3DTA_CURRENT );
        }
        else
            pd3dDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE );
    }
    else
    {
        if( g_bEnvMapOn )
        {
            pd3dDevice->SetTexture(1, D3DTextr_GetTexture( "EarthEnvMap.bmp" ) );
            pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0 );
            pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADD );
            pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
            pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT );
        }
        else
            pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE );
        pd3dDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE );
    }

    pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST,  D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2,
                   g_SphereVertices, SPHERE_NUM_VERTICES, 0x0 );

    pd3dDevice->EndScene();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitBumpMap()
// Desc: Copies raw bits from _lpBumpSrc into the type of bump map requested
//       as 'format' into pBumpMap.
//-----------------------------------------------------------------------------
LPDIRECT3DTEXTURE2 InitBumpMap( BUMPMAPFORMAT format,
                                LPDIRECTDRAWSURFACE4 pddsBumpSrc )
{
    if( NULL == pddsBumpSrc )
        return NULL;

    LPDIRECTDRAW4 pDD;
    pddsBumpSrc->GetDDInterface( (VOID**)&pDD );
    pDD->Release();
    if( NULL == pDD )
        return NULL;

    // Create the bump map surface. Surface desc depends on bump format
    DDSURFACEDESC2 ddsd; 
    D3DUtil_InitSurfaceDesc( ddsd );
    pddsBumpSrc->GetSurfaceDesc( &ddsd );

    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps          = DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY;
    ddsd.ddsCaps.dwCaps2         = 0L;
    ddsd.ddpfPixelFormat.dwSize  = sizeof(DDPIXELFORMAT);
    ddsd.ddpfPixelFormat.dwFlags = DDPF_BUMPDUDV;

    switch( format )
    {
        case BUMPMAP_U8V8:
            ddsd.ddpfPixelFormat.dwBumpBitCount         = 16;
            ddsd.ddpfPixelFormat.dwBumpDuBitMask        = 0x000000ff;
            ddsd.ddpfPixelFormat.dwBumpDvBitMask        = 0x0000ff00;
            ddsd.ddpfPixelFormat.dwBumpLuminanceBitMask = 0x00000000;
            break;

        case BUMPMAP_U5V5L6:
            ddsd.ddpfPixelFormat.dwFlags |= DDPF_BUMPLUMINANCE;
            ddsd.ddpfPixelFormat.dwBumpBitCount         = 16;
            ddsd.ddpfPixelFormat.dwBumpDuBitMask        = 0x0000001f;
            ddsd.ddpfPixelFormat.dwBumpDvBitMask        = 0x000003e0;
            ddsd.ddpfPixelFormat.dwBumpLuminanceBitMask = 0x0000fc00;
            break;

        case BUMPMAP_U8V8L8:
            ddsd.ddpfPixelFormat.dwFlags |= DDPF_BUMPLUMINANCE;
            ddsd.ddpfPixelFormat.dwBumpBitCount         = 24;
            ddsd.ddpfPixelFormat.dwBumpDuBitMask        = 0x000000ff;
            ddsd.ddpfPixelFormat.dwBumpDvBitMask        = 0x0000ff00;
            ddsd.ddpfPixelFormat.dwBumpLuminanceBitMask = 0x00ff0000;
            break;
    }

    LPDIRECTDRAWSURFACE4 pddsBumpMap;
    LPDIRECT3DTEXTURE2   ptexBumpMap;

    // Create the bumpmap's surface and texture objects
    if( FAILED( pDD->CreateSurface( &ddsd, &pddsBumpMap, NULL ) ) )
        return NULL;

    // Fill the bits of the new texture surface with bits from
    // a private format.
    while( pddsBumpSrc->Lock( NULL, &ddsd, 0, 0 ) == DDERR_WASSTILLDRAWING );
    DWORD lSrcPitch = ddsd.lPitch;
    BYTE* pSrc      = (BYTE*)ddsd.lpSurface;

    while( pddsBumpMap->Lock( NULL, &ddsd, 0, 0 ) == DDERR_WASSTILLDRAWING );
    DWORD lDstPitch = ddsd.lPitch;
    BYTE* pDst      = (BYTE*)ddsd.lpSurface;

    for( DWORD y=0; y<ddsd.dwHeight; y++ )
    {
        BYTE* pDstT = pDst;
        BYTE* pSrcB0 = (BYTE*)pSrc;
        BYTE* pSrcB1 = ( pSrcB0 + lSrcPitch );

        if( y == ddsd.dwHeight-1 )   // Don't go past the last line
            pSrcB1 = pSrcB0;

        for( DWORD x=0; x<ddsd.dwWidth; x++ )
        {
            LONG  v00 = *(pSrcB0+0); // Get the current pixel
            LONG  v01 = *(pSrcB0+4); // and the pixel to the right
            LONG  v10 = *(pSrcB1+0); // and the pixel one line below.

            LONG iDu = v00-v01;      // The delta-u bump value
            LONG iDv = v00-v10;      // The delta-v bump value
            WORD uL  = 255;          // The luminance bump value
            
            if( v00 > 120 )          // This is specific to the earth bitmap,
                uL = 128;            // to make the land masses less shiny.

            switch( format )
            {
                case BUMPMAP_U8V8:
                    *pDstT++ = (BYTE)iDu;
                    *pDstT++ = (BYTE)iDv;
                    break;

                case BUMPMAP_U5V5L6:
                    *(WORD*)pDstT  =  (iDu >> 3) & 0x1f;
                    *(WORD*)pDstT |= ((iDv >> 3) & 0x1f)<<5;
                    *(WORD*)pDstT |= ( (uL >> 2) & 0x3f)<<10;
                    pDstT += 2;
                    break;

                case BUMPMAP_U8V8L8:
                    *pDstT++ = (BYTE)iDu;
                    *pDstT++ = (BYTE)iDv;
                    *pDstT++ = (BYTE)uL;
                    break;
            }

            pSrcB0+=4; // Move one pixel to the left (src is 32-bpp)
            pSrcB1+=4;
        }
        pSrc += lSrcPitch; // Move to the next line
        pDst += lDstPitch;
    }
    pddsBumpMap->Unlock(0);
    pddsBumpSrc->Unlock(0);

    // Get the texture interface for the bumpmap
    if( FAILED( pddsBumpMap->QueryInterface( IID_IDirect3DTexture2,
                                             (VOID**)&ptexBumpMap ) ) )
        ptexBumpMap = NULL;

    pddsBumpMap->Release(); // Note, surface still exists through texture ref

    return ptexBumpMap;
}




//-----------------------------------------------------------------------------
// Name: App_InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3 pd3dDevice,
                               LPDIRECT3DVIEWPORT3 pvViewport )
{
    // Yuck, a global ptr. Store this away for use during the message handling
    // function, so that the bumpmap format may be changed by the UI.
    g_pd3dDevice = pd3dDevice;

    for( DWORD t=0; t<3; t++ )
    {
        pd3dDevice->SetTextureStageState( t, D3DTSS_ADDRESS, D3DTADDRESS_WRAP );
        pd3dDevice->SetTextureStageState( t, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
        pd3dDevice->SetTextureStageState( t, D3DTSS_MINFILTER, D3DTFN_LINEAR );
    }

	pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );

    D3DTextr_RestoreAllTextures( pd3dDevice );
    g_pddsBumpMap = D3DTextr_GetSurface( "earthbump.bmp" );
    g_ptexBumpMap = InitBumpMap( g_BumpMapFormat, g_pddsBumpMap );

    //  Set matrices
    D3DVECTOR vEyePt( 0.0f, 2.0f, 10.0f );
    D3DVECTOR vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DVECTOR vUpVec( 0.0f, 1.0f, 0.0f );
    D3DMATRIX matScale, matWorld, matView, matProj;

    D3DUtil_SetViewMatrix( matView, vEyePt, vLookatPt, vUpVec );
    D3DUtil_SetProjectionMatrix( matProj, g_PI/4, 1.0f, 1.0f, 20.0f );
    D3DUtil_SetScaleMatrix( matScale, 3.0f, 3.0f, 3.0f );
    D3DUtil_SetRotateYMatrix( matWorld, 80.0f * (g_PI/180) );
    D3DMath_MatrixMultiply( matWorld, matScale, matWorld );

    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW,       &matView );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

	// Get a ptr to the ID3D object to create materials and/or lights. Note:
	// the Release() call just serves to decrease the ref count.
    LPDIRECT3D3 pD3D;
    pd3dDevice->GetDirect3D( &pD3D );
    pD3D->Release();

    // Create a material
    D3DMATERIALHANDLE hmtrl = NULL;
    if( SUCCEEDED( pD3D->CreateMaterial( &g_pD3DMaterial, NULL ) ) )
    {
        D3DMATERIAL mtrl;
        D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
        mtrl.ambient.r = 0.0f;
        mtrl.ambient.g = 0.0f;
        mtrl.ambient.b = 0.0f;
        mtrl.power     = 0.0f;
        g_pD3DMaterial->SetMaterial( &mtrl);
        g_pD3DMaterial->GetHandle( pd3dDevice, &hmtrl);
    }
    pd3dDevice->SetLightState( D3DLIGHTSTATE_MATERIAL, hmtrl );
    pd3dDevice->SetLightState( D3DLIGHTSTATE_AMBIENT, 0x000000ff );

    // Add a directional light
    LPDIRECT3DLIGHT pLight;

    if( SUCCEEDED( pD3D->CreateLight( &pLight, NULL ) ) )
    {
        D3DLIGHT light;
        D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, 0, 0, 1 );
        light.dcvColor.r     = 0.9f;
        light.dcvColor.g     = 0.9f;
        light.dcvColor.b     = 0.9f;
        light.dvAttenuation0 = 1.0f;

        pLight->SetLight( &light );
        pvViewport->AddLight(pLight );
    }

    // Set menu states
    CheckMenuItem( g_hMenu, IDM_TEXTURETOGGLE,
                   g_bTextureOn ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( g_hMenu, IDM_BUMPMAPTOGGLE,
                   g_bBumpMapOn ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( g_hMenu, IDM_ENVMAPTOGGLE,
                   g_bEnvMapOn ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( g_hMenu, IDM_U8V8L8,
		           g_BumpMapFormat==BUMPMAP_U8V8L8?MF_CHECKED:MF_UNCHECKED );
    CheckMenuItem( g_hMenu, IDM_U5V5L6,
		           g_BumpMapFormat==BUMPMAP_U5V5L6?MF_CHECKED:MF_UNCHECKED );
    CheckMenuItem( g_hMenu, IDM_U8V8,
		           g_BumpMapFormat==BUMPMAP_U8V8?MF_CHECKED:MF_UNCHECKED );
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

    SAFE_RELEASE( g_ptexBumpMap );
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
    if( pd3dDeviceDesc->dwTextureOpCaps & D3DTEXOPCAPS_BUMPENVMAP )
        return S_OK;
    if( pd3dDeviceDesc->dwTextureOpCaps & D3DTEXOPCAPS_BUMPENVMAPLUMINANCE )
        return S_OK;
    return E_FAIL;
}




//-----------------------------------------------------------------------------
// Name: InitSphereVertices()
// Desc: Sets up the vertices for a bump-mapped sphere.
//-----------------------------------------------------------------------------
VOID InitSphereVertices()
{
    // Counters
    WORD x, y, vtx = 0;

    // Angle deltas for constructing the sphere's vertices
    FLOAT fDAng   = g_PI / SPHERE_NUM_RINGS;
    FLOAT fDAngY0 = fDAng;
    FLOAT fDAngY1 = 2*fDAng;

    // Make the middle of the sphere
    for( y=0; y<(SPHERE_NUM_RINGS-2); y++ )
    {
        FLOAT y0 = (FLOAT)cos(fDAngY0);
        FLOAT y1 = (FLOAT)cos(fDAngY1);
        FLOAT r0 = (FLOAT)sin(fDAngY0);
        FLOAT r1 = (FLOAT)sin(fDAngY1);

        for( x=0; x<(SPHERE_NUM_RINGS*2); x++ )
        {
            FLOAT fDAngX0 = (x+0)*fDAng;
            FLOAT fDAngX1 = (x+1)*fDAng;
    
            if( x == (SPHERE_NUM_RINGS*2-1) )
                fDAngX1 = 0.0f;

            D3DVECTOR v00( r0*(FLOAT)sin(fDAngX0), y0, r0*(FLOAT)cos(fDAngX0) );
            D3DVECTOR v10( r1*(FLOAT)sin(fDAngX0), y1, r1*(FLOAT)cos(fDAngX0) );
            D3DVECTOR v11( r1*(FLOAT)sin(fDAngX1), y1, r1*(FLOAT)cos(fDAngX1) );
            D3DVECTOR v01( r0*(FLOAT)sin(fDAngX1), y0, r0*(FLOAT)cos(fDAngX1) );

            g_SphereVertices[ vtx ].v   = D3DVERTEX( v00, v00, 0, 0 );
            g_SphereVertices[ vtx ].tu2 = x/(SPHERE_NUM_RINGS*2.0f);
            g_SphereVertices[vtx++].tv2 = y0*.5f + .5f;

            g_SphereVertices[ vtx ].v   = D3DVERTEX( v10, v10, 0, 0 );
            g_SphereVertices[ vtx ].tu2 = x/(SPHERE_NUM_RINGS*2.0f);
            g_SphereVertices[vtx++].tv2 = y1*.5f + .5f;

            g_SphereVertices[ vtx ].v   = D3DVERTEX( v11, v11, 0, 0 );
            g_SphereVertices[ vtx ].tu2 = (x+1)/(SPHERE_NUM_RINGS*2.0f); 
            g_SphereVertices[vtx++].tv2 = y1*.5f + .5f;

            g_SphereVertices[ vtx ].v   = D3DVERTEX( v00, v00, 0, 0 );
            g_SphereVertices[ vtx ].tu2 = x/(SPHERE_NUM_RINGS*2.0f); 
            g_SphereVertices[vtx++].tv2 = y0*.5f + .5f;

            g_SphereVertices[ vtx ].v   = D3DVERTEX( v11, v11, 0, 0 );
            g_SphereVertices[ vtx ].tu2 = (x+1)/(SPHERE_NUM_RINGS*2.0f); 
            g_SphereVertices[vtx++].tv2 = y1*.5f + .5f;

            g_SphereVertices[ vtx ].v   = D3DVERTEX( v01, v01, 0, 0 );
            g_SphereVertices[ vtx ].tu2 = (x+1)/(SPHERE_NUM_RINGS*2.0f);
            g_SphereVertices[vtx++].tv2 = y0*.5f + .5f;
        }
        fDAngY0  = fDAngY1;
        fDAngY1 += fDAng;
    }

    // make top
    fDAngY1 = fDAng;
    FLOAT y1 = (FLOAT)cos(fDAngY1);
    FLOAT r1 = (FLOAT)sin(fDAngY1);
    
    for( x=0; x<(SPHERE_NUM_RINGS*2); x++ )
    {
        FLOAT fDAngX0 = (x+0)*fDAng;
        FLOAT fDAngX1 = (x+1)*fDAng;
    
        if( x == (SPHERE_NUM_RINGS*2-1) )
            fDAngX1 = 0.0f;

        D3DVECTOR vy( 0, 1, 0 );
        D3DVECTOR v10( r1*(FLOAT)sin(fDAngX0), y1, r1*(FLOAT)cos(fDAngX0) );
        D3DVECTOR v11( r1*(FLOAT)sin(fDAngX1), y1, r1*(FLOAT)cos(fDAngX1) );

        g_SphereVertices[ vtx ].v   = D3DVERTEX( vy, vy, 0, 0 );
        g_SphereVertices[ vtx ].tu2 = 0.0; 
        g_SphereVertices[vtx++].tv2 = 1.0;

        g_SphereVertices[ vtx ].v   = D3DVERTEX( v10, v10, 0, 0 );
        g_SphereVertices[ vtx ].tu2 = x/(SPHERE_NUM_RINGS*2.0f);
        g_SphereVertices[vtx++].tv2 = y1*.5f + .5f;

        g_SphereVertices[ vtx ].v   = D3DVERTEX( v11, v11, 0, 0 );
        g_SphereVertices[ vtx ].tu2 = (x+1)/(SPHERE_NUM_RINGS*2.0f); 
        g_SphereVertices[vtx++].tv2 = y1*.5f + .5f;
    }

    // make bottom
    fDAngY1 = fDAngY0;          // remember last value used, so there are no cracks
    y1 = (FLOAT)cos(fDAngY1);
    r1 = (FLOAT)sin(fDAngY1);

    for( x=0; x<(SPHERE_NUM_RINGS*2); x++ )
    {
        FLOAT fDAngX0 = (x+0)*fDAng;
        FLOAT fDAngX1 = (x+1)*fDAng;
    
        if( x == (SPHERE_NUM_RINGS*2-1) )
            fDAngX1 = 0.0f;
    
        D3DVECTOR vy( 0, 1, 0 );
        D3DVECTOR v10( r1*(FLOAT)sin(fDAngX1), y1, r1*(FLOAT)cos(fDAngX1) );
        D3DVECTOR v11( r1*(FLOAT)sin(fDAngX0), y1, r1*(FLOAT)cos(fDAngX0) );

        g_SphereVertices[ vtx ].v   = D3DVERTEX( -vy, vy, 0, 0 );
        g_SphereVertices[ vtx ].tu2 = 0.0f; 
        g_SphereVertices[vtx++].tv2 = 0.0f;

        g_SphereVertices[ vtx ].v   = D3DVERTEX( v10, v10, 0, 0 );
        g_SphereVertices[ vtx ].tu2 = x/(SPHERE_NUM_RINGS*2.0f); 
        g_SphereVertices[vtx++].tv2 = y1*.5f + .5f;

        g_SphereVertices[ vtx ].v   = D3DVERTEX( v11, v11, 0, 0 );
        g_SphereVertices[ vtx ].tu2 = (x+1)/(SPHERE_NUM_RINGS*2.0f); 
        g_SphereVertices[vtx++].tv2 = y1*.5f + .5f;
    }

    // Scale the bumpmap's texture coords
    for( vtx=0; vtx<SPHERE_NUM_VERTICES; vtx++ )
    {
        g_SphereVertices[vtx].tu2 *= -1; 
        g_SphereVertices[vtx].tv2 *= -1;

        g_SphereVertices[vtx].v.tu = g_SphereVertices[vtx].tu2;
        g_SphereVertices[vtx].v.tv = g_SphereVertices[vtx].tv2;
    }
}




//-----------------------------------------------------------------------------
// Name: App_OverridenWndProc()
// Desc: Message proc function to handle key and menu input
//-----------------------------------------------------------------------------
LRESULT CALLBACK App_OverridenWndProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                       LPARAM lParam )
{
    if( WM_COMMAND == uMsg )
    {
        switch( LOWORD(wParam) )
        {
            case IDM_TEXTURETOGGLE:
                g_bTextureOn = !g_bTextureOn;
                CheckMenuItem( g_hMenu, IDM_TEXTURETOGGLE,
                               g_bTextureOn ? MF_CHECKED : MF_UNCHECKED );
                break;

            case IDM_BUMPMAPTOGGLE:
                g_bBumpMapOn = !g_bBumpMapOn;
                CheckMenuItem( g_hMenu, IDM_BUMPMAPTOGGLE,
                               g_bBumpMapOn ? MF_CHECKED : MF_UNCHECKED );
                break;
        
            case IDM_ENVMAPTOGGLE:
                g_bEnvMapOn = !g_bEnvMapOn;
                CheckMenuItem( g_hMenu, IDM_ENVMAPTOGGLE,
                               g_bEnvMapOn ? MF_CHECKED : MF_UNCHECKED );
                break;
        
            case IDM_U8V8L8:
                g_BumpMapFormat = BUMPMAP_U8V8L8;
                SAFE_RELEASE( g_ptexBumpMap );
                D3DTextr_RestoreAllTextures( g_pd3dDevice );
                g_pddsBumpMap = D3DTextr_GetSurface( "earthbump.bmp" );
                g_ptexBumpMap = InitBumpMap( g_BumpMapFormat, g_pddsBumpMap );
                CheckMenuItem( g_hMenu, IDM_U8V8L8, MF_CHECKED );
                CheckMenuItem( g_hMenu, IDM_U5V5L6, MF_UNCHECKED );
                CheckMenuItem( g_hMenu, IDM_U8V8,   MF_UNCHECKED );
                break;

            case IDM_U5V5L6:
                g_BumpMapFormat = BUMPMAP_U5V5L6;
                SAFE_RELEASE( g_ptexBumpMap );
                D3DTextr_RestoreAllTextures( g_pd3dDevice );
                g_pddsBumpMap = D3DTextr_GetSurface( "earthbump.bmp" );
                g_ptexBumpMap = InitBumpMap( g_BumpMapFormat, g_pddsBumpMap );
                CheckMenuItem( g_hMenu, IDM_U8V8L8, MF_UNCHECKED );
                CheckMenuItem( g_hMenu, IDM_U5V5L6, MF_CHECKED );
                CheckMenuItem( g_hMenu, IDM_U8V8,   MF_UNCHECKED );
                break;
        
            case IDM_U8V8:
                g_BumpMapFormat = BUMPMAP_U8V8;
                SAFE_RELEASE( g_ptexBumpMap );
                D3DTextr_RestoreAllTextures( g_pd3dDevice );
                g_pddsBumpMap = D3DTextr_GetSurface( "earthbump.bmp" );
                g_ptexBumpMap = InitBumpMap( g_BumpMapFormat, g_pddsBumpMap );
                CheckMenuItem( g_hMenu, IDM_U8V8L8, MF_UNCHECKED );
                CheckMenuItem( g_hMenu, IDM_U5V5L6, MF_UNCHECKED );
                CheckMenuItem( g_hMenu, IDM_U8V8,   MF_CHECKED );
                break;
        }
    }

    return WndProc( hWnd, uMsg, wParam, lParam );
}





