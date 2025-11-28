//-----------------------------------------------------------------------------
// File: Mipmap.cpp
//
// Desc: Example code showing how to do mipmap textures.
//
//       Note: This code uses the D3D Framework helper library, but it does NOT
//       use functionality from the D3DTextr.cpp file. Rather, all texture
//       functions are performed locally to illustrate the complete
//       implementation of mipmaps.
//
//
// Copyright (c) 1997-1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define  STRICT
#define  D3D_OVERLOADS
#include <d3d.h>
#include <math.h>
#include <stdio.h>
#include "D3DUtil.h"


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
TCHAR* g_strAppTitle       = TEXT("Mipmap: Direct3D Mipmapping Sample");
BOOL   g_bAppUseBackBuffer = TRUE;
BOOL   g_bAppUseZBuffer    = FALSE;


// Structure to hold information for a mipmap texture
struct MipMapContainer
{
    HBITMAP              hbmBitmap[10];     // Bitmaps containing images
    DWORD                dwMipMapCount;     // Levels of mipmap
    LPDIRECTDRAWSURFACE4 pddsSurface;       // Surface of the mipmap
    LPDIRECT3DTEXTURE2   ptexTexture;       // Texture object for the mipmap
};


D3DVERTEX           g_Mesh[4];                    // Rendering vertices
MipMapContainer*    g_pMipMap = NULL;             // The main mipmap




//-----------------------------------------------------------------------------
// External and local function prototypes
//-----------------------------------------------------------------------------
VOID    AppPause( BOOL );
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );

HRESULT MipMap_Create( CHAR*, MipMapContainer** );
VOID    MipMap_Delete( MipMapContainer* );
HRESULT MipMap_Restore( MipMapContainer*, LPDIRECT3DDEVICE3 );
VOID    MipMap_Invalidate( MipMapContainer* );




//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{
    // Create the mipmaps (loads bitmaps from "brickN.bmp" files)
    if( FAILED( MipMap_Create( "BRICK", &g_pMipMap ) ) )
    {
        MessageBox( hWnd, TEXT( "Could not load bitmap images" ),
                    TEXT( "Mipmap" ), MB_ICONWARNING | MB_OK );
        DEBUG_MSG( "Could not load bitmap images" );
        return E_FAIL;
    }

    // Initialize mesh used to render the mipmaps
    g_Mesh[0] = D3DVERTEX( D3DVECTOR(-1,-1, 0), D3DVECTOR(0,0,1), 0.0f, 1.0f );
    g_Mesh[1] = D3DVERTEX( D3DVECTOR(-1, 1, 0), D3DVECTOR(0,0,1), 0.0f, 0.0f );
    g_Mesh[2] = D3DVERTEX( D3DVECTOR( 1,-1, 0), D3DVECTOR(0,0,1), 1.0f, 1.0f );
    g_Mesh[3] = D3DVERTEX( D3DVECTOR( 1, 1, 0), D3DVECTOR(0,0,1), 1.0f, 0.0f );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT App_FrameMove( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{
    D3DMATRIX matView;
    D3DVECTOR vEyePt    = D3DVECTOR( 0, 0, (FLOAT)(13*sin(fTimeKey)-15) );
    D3DVECTOR vLookatPt = D3DVECTOR( 0, 0, 0 );
    D3DVECTOR vUpVec    = D3DVECTOR( 0, 1, 0 );
    D3DUtil_SetViewMatrix( matView, vEyePt, vLookatPt, vUpVec );
    
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &matView );

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
    //Clear the viewport
    pvViewport->Clear2( 1UL, prcViewRect, D3DCLEAR_TARGET, 0x00004455, 0, 0 );

    // Begin the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        D3DMATRIX matWorld;

        // Draw the left quad. Set renderstates for bilinear filtering
        D3DUtil_SetTranslateMatrix( matWorld, -1.1f, 0.0f, 0.0f );
        pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
        pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTFP_NONE );
        pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX,
                                   g_Mesh, 4, 0 );

        // Draw the right quad. Set renderstates for mipmapping
        D3DUtil_SetTranslateMatrix( matWorld, +1.1f, 0.0f, 0.0f );
        pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
        pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTFP_LINEAR );
        pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX,
                                   g_Mesh, 4, 0 );

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

    // Build the mipmap device surfaces and textures.
    if( FAILED( MipMap_Restore( g_pMipMap, pd3dDevice ) ) )
        return E_FAIL;
    pd3dDevice->SetTexture( 0, g_pMipMap->ptexTexture );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );

    // Set the transform matrices.
    D3DMATRIX matProj;
    D3DUtil_SetProjectionMatrix( matProj, 1.57f, 1.0f, 1.0f, 100.0f );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

    // This simple sample uses only ambient light
    pd3dDevice->SetLightState( D3DLIGHTSTATE_AMBIENT, 0xffffffff );

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
    // Delete the device objects
    App_DeleteDeviceObjects( pd3dDevice, pvViewport );

    MipMap_Delete( g_pMipMap );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_DeleteDeviceObjects()
// Desc: Called when the app is exitting, or the device is being changed,
//       this function deletes any device dependant objects.
//-----------------------------------------------------------------------------
VOID App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 )
{
    MipMap_Invalidate( g_pMipMap );
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
    // Get Triangle Caps
    LPD3DPRIMCAPS pdpcTriCaps = &pd3dDeviceDesc->dpcTriCaps;

    // Check Texture modes for MIPLINEAR MipMapping
    if( !( pdpcTriCaps->dwTextureFilterCaps & D3DPTFILTERCAPS_LINEARMIPLINEAR ) )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: MipMap_Create()
// Desc: Creates the mipmap structure and loads image data from bitmaps.
//-----------------------------------------------------------------------------
HRESULT MipMap_Create( CHAR* strMipMapName, MipMapContainer** ppMipMap )
{
    MipMapContainer* pMipMap;

    if( NULL == ( pMipMap = new MipMapContainer ) )
        return NULL;
    pMipMap->pddsSurface = NULL;
    pMipMap->ptexTexture = NULL;

    for( WORD wNum=0; wNum<10; wNum++ )
    {
        CHAR strFileName[80], strResourceName[80];
        sprintf( strFileName, "%s%d.bmp", strMipMapName, wNum );
        sprintf( strResourceName, "%s%d", strMipMapName, wNum );

        // Load the bitmaps from a file
        HBITMAP hbm = (HBITMAP)LoadImage( NULL, strFileName, IMAGE_BITMAP,
                                   0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION );

        // If that didn't work, trying loading bitmaps from the resource
        if( NULL == hbm ) 
            hbm = (HBITMAP)LoadImage( GetModuleHandle(NULL), strResourceName,
                               IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION );
        
        if( NULL == hbm ) 
        {
            if( 0 == wNum )
            {
                delete pMipMap;
                (*ppMipMap) = NULL;
                return E_FAIL;
            }
            pMipMap->dwMipMapCount = wNum;
            (*ppMipMap) = pMipMap;
            return S_OK;
        }

        pMipMap->hbmBitmap[wNum] = hbm;
    }

    (*ppMipMap) = NULL;
    return E_FAIL;
}




//-----------------------------------------------------------------------------
// Name: TextureSearchCallback()
// Desc: Callback function used to enumerate texture formats.
//-----------------------------------------------------------------------------
HRESULT CALLBACK TextureSearchCallback( DDPIXELFORMAT* pddpf, VOID* param )
{
    DDSURFACEDESC2* pddsd = (DDSURFACEDESC2*)param;

    // Skip unwanted formats
    if( pddpf->dwRGBBitCount != pddsd->dwFlags )
        return DDENUMRET_OK;
    if( pddpf->dwFlags & (DDPF_LUMINANCE|DDPF_ALPHAPIXELS) )
        return DDENUMRET_OK;
    if( pddpf->dwFlags & (DDPF_BUMPLUMINANCE|DDPF_BUMPDUDV) )
        return DDENUMRET_OK;
    if( 0 != pddpf->dwFourCC )
        return DDENUMRET_OK;

    memcpy( &pddsd->ddpfPixelFormat, pddpf, sizeof(DDPIXELFORMAT) );
    return DDENUMRET_CANCEL;
}




//-----------------------------------------------------------------------------
// Name: MipMap_Restore()
// Desc: Creates the device-dependant surface and texture for the mipmap
//-----------------------------------------------------------------------------
HRESULT MipMap_Restore( MipMapContainer* pMipMap, LPDIRECT3DDEVICE3 pd3dDevice )
{
    // Check params
    if( NULL==pMipMap || NULL==pd3dDevice )
        return E_INVALIDARG;

    // Release any previously created objects
    SAFE_RELEASE( pMipMap->ptexTexture );
    SAFE_RELEASE( pMipMap->pddsSurface );

    // Get a DDraw ptr (from the device's render target) for creating surfaces.
    // Note: the Release() calls just serve to decrement the ref count, but the
    // ptrs are still valid.
    LPDIRECTDRAWSURFACE4 pddsRender;
    LPDIRECTDRAW4        pDD = NULL;
    pd3dDevice->GetRenderTarget( &pddsRender );
    pddsRender->GetDDInterface( (VOID**)&pDD );
    pddsRender->Release();
    pDD->Release();

    // Get size info for top level bitmap
    BITMAP bm; 
    GetObject( pMipMap->hbmBitmap[0], sizeof(BITMAP), &bm ); 

    // Set up and create the mipmap surface
    DDSURFACEDESC2 ddsd;
    ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
    ddsd.dwSize          = sizeof(DDSURFACEDESC2);
    ddsd.dwFlags         = DDSD_CAPS|DDSD_MIPMAPCOUNT|DDSD_WIDTH|DDSD_HEIGHT|
                           DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE|DDSCAPS_MIPMAP|DDSCAPS_COMPLEX;
    ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
    ddsd.dwMipMapCount   = pMipMap->dwMipMapCount;
    ddsd.dwWidth         = bm.bmWidth;
    ddsd.dwHeight        = bm.bmHeight;

    // Enumerate a good texture format. Search for a 16-bit format first
    DDSURFACEDESC2 ddsdSearch;
    ddsdSearch.dwFlags = 16;
    pd3dDevice->EnumTextureFormats( TextureSearchCallback, &ddsdSearch );
    
    // If that wasn't found, check for a 32-bit format
    if( 16 != ddsdSearch.ddpfPixelFormat.dwRGBBitCount )
    {
        ddsdSearch.dwFlags = 32;
        pd3dDevice->EnumTextureFormats( TextureSearchCallback, &ddsdSearch );
        if( 32 != ddsdSearch.ddpfPixelFormat.dwRGBBitCount )
            return E_FAIL;
    }

    // If we got a good texture format, use it to create the surface
    memcpy( &ddsd.ddpfPixelFormat, &ddsdSearch.ddpfPixelFormat,
            sizeof(DDPIXELFORMAT) );

    // Create the mipmap surface and texture
    if( FAILED( pDD->CreateSurface( &ddsd, &pMipMap->pddsSurface, NULL ) ) )
        return E_FAIL;
    if( FAILED( pMipMap->pddsSurface->QueryInterface( IID_IDirect3DTexture2,
                                                     (VOID**)&pMipMap->ptexTexture ) ) )
        return E_FAIL;

    // Loop through each surface in the mipmap, copying the bitmap to the temp
    // surface, and then blitting the temp surface to the real one.
    LPDIRECTDRAWSURFACE4 pddsDest = pMipMap->pddsSurface;

    for( WORD wNum=0; wNum < pMipMap->dwMipMapCount; wNum++ )
    {
        // Copy the bitmap image to the surface
        BITMAP bm; 
        GetObject( pMipMap->hbmBitmap[wNum], sizeof(BITMAP), &bm ); 

        // Create a DC and setup the bitmap
        HDC hdcBitmap = CreateCompatibleDC( NULL );
        if( NULL == hdcBitmap )
            return E_FAIL;

        SelectObject( hdcBitmap, pMipMap->hbmBitmap[wNum] );

        HDC hdcSurface;
        if( SUCCEEDED( pddsDest->GetDC( &hdcSurface ) ) )
        {
            BitBlt( hdcSurface, 0, 0, bm.bmWidth, bm.bmHeight, hdcBitmap,
                    0, 0, SRCCOPY );
            pddsDest->ReleaseDC( hdcSurface );
        }

        DeleteDC( hdcBitmap );

        // Get the next surface in the chain. Do a Release() call, though, to
        // avoid increasing the ref counts on the surfaces.
        DDSCAPS2 ddsCaps;
        ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
        if( SUCCEEDED( pddsDest->GetAttachedSurface( &ddsCaps, &pddsDest ) ) )
            pddsDest->Release();
    }

    // For palettized textures, use the bitmap and GetDIBColorTable() to build
    // and attach a palette to the surface here.

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: MipMap_Invalidate()
// Desc: Frees device dependant objects for the mipmap
//-----------------------------------------------------------------------------
VOID MipMap_Invalidate( MipMapContainer* pMipMap )
{
    if( pMipMap )
    {
        SAFE_RELEASE( pMipMap->pddsSurface );
        SAFE_RELEASE( pMipMap->ptexTexture );
    }
}




//-----------------------------------------------------------------------------
// Name: Mipmap_Delete()
// Desc: Frees device dependant objects for the mipmap
//-----------------------------------------------------------------------------
VOID MipMap_Delete( MipMapContainer* pMipMap )
{
    if( pMipMap )
    {
        for( WORD wNum=0; wNum<pMipMap->dwMipMapCount; wNum++ )
            DeleteObject( pMipMap->hbmBitmap[wNum] );
    }
}






