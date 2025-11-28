//-----------------------------------------------------------------------------
// File: VideoTex.cpp
//
// Desc: Example code showing how to do use an AVI file as a texture map. This
//       sample draws a cube with an AVI texture mapped to each of its faces.
//       Ideally, the texture would be created with the DDSCAPS2_HINTDYNAMIC
//       flag set.
//
//       Note: This code uses the D3D Framework helper library.
//
//@@BEGIN_MSINTERNAL
//
// Hist: 06.11.98 - t-aperez - Created
//       06.12.98 - mwetzel  - Cleaned up
//       06.22.98 - mwetzel  - More clean up
//       07.27.98 - mwetzel  - Removed MIRROR mode which crashes on Virge
//       07.27.98 - mwetzel  - Removed bogus SetSurfaceDesc() call.
//
//@@END_MSINTERNAL
//
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define STRICT
#define D3D_OVERLOADS
#include <time.h>
#include <stdio.h>
#include <windows.h>
#include <vfw.h>
#include "D3DTextr.h"
#include "D3DUtil.h"
#include "D3DMath.h"




//-----------------------------------------------------------------------------
// Declare the application globals for use in WinMain.cpp
//-----------------------------------------------------------------------------
TCHAR* g_strAppTitle       = TEXT("VideoTex sample app");
BOOL   g_bAppUseZBuffer    = TRUE;
BOOL   g_bAppUseBackBuffer = TRUE;




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define CUBE_SCALE        5.0f
#define NUM_CUBE_VERTICES (4*6)
#define NUM_CUBE_INDICES  (6*6)
D3DVERTEX     g_pCubeVertices[NUM_CUBE_VERTICES]; // Vertices for the cube
WORD          g_pCubeIndices[NUM_CUBE_INDICES];   // Indices for the cube

PAVISTREAM    g_paviStream;    // The AVI stream
PGETFRAME     g_pgfFrame;      // Where in the stream to get the next frame
AVISTREAMINFO g_psiStreamInfo; // Info about the AVI stream




//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
HRESULT UpdateTexture( LPDIRECT3DDEVICE3 pd3dDevice );




//-----------------------------------------------------------------------------
// Name: CreateCube()
// Desc: Sets up the vertices for a cube.
//-----------------------------------------------------------------------------
HRESULT CreateCube( D3DVERTEX* pVertices, WORD* pIndices )
{
    // Define the normals for the cube
    D3DVECTOR n0( 0.0f, 0.0f,-1.0f ); // Front face
    D3DVECTOR n1( 0.0f, 0.0f, 1.0f ); // Back face
    D3DVECTOR n2( 0.0f,-1.0f, 0.0f ); // Top face
    D3DVECTOR n3( 0.0f, 1.0f, 0.0f ); // Bottom face
    D3DVECTOR n4(-1.0f, 0.0f, 0.0f ); // Right face
    D3DVECTOR n5( 1.0f, 0.0f, 0.0f ); // Left face

    // Set up the vertices for the cube. Note: to prevent tiling problems,
    // the u/v coords are knocked slightly inwards.

    // Front face
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f, 1.0f,-1.0f), n0, 0.01f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f, 1.0f,-1.0f), n0, 0.99f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f,-1.0f,-1.0f), n0, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f,-1.0f,-1.0f), n0, 0.01f, 0.01f );

    // Back face
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f, 1.0f, 1.0f), n1, 0.99f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f,-1.0f, 1.0f), n1, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f,-1.0f, 1.0f), n1, 0.01f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f, 1.0f, 1.0f), n1, 0.01f, 0.99f );

    // Top face
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f, 1.0f, 1.0f), n2, 0.01f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f, 1.0f, 1.0f), n2, 0.99f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f, 1.0f,-1.0f), n2, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f, 1.0f,-1.0f), n2, 0.01f, 0.01f );

    // Bottom face
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f,-1.0f, 1.0f), n3, 0.01f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f,-1.0f,-1.0f), n3, 0.01f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f,-1.0f,-1.0f), n3, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f,-1.0f, 1.0f), n3, 0.99f, 0.99f );

    // Right face
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f, 1.0f,-1.0f), n4, 0.01f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f, 1.0f, 1.0f), n4, 0.99f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f,-1.0f, 1.0f), n4, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f,-1.0f,-1.0f), n4, 0.01f, 0.01f );

    // Left face
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f, 1.0f,-1.0f), n5, 0.99f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f,-1.0f,-1.0f), n5, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f,-1.0f, 1.0f), n5, 0.01f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f, 1.0f, 1.0f), n5, 0.01f, 0.99f );

    // Set up the indices for the cube
    *pIndices++ =  0+0;   *pIndices++ =  0+1;   *pIndices++ =  0+2;
    *pIndices++ =  0+2;   *pIndices++ =  0+3;   *pIndices++ =  0+0;
    *pIndices++ =  4+0;   *pIndices++ =  4+1;   *pIndices++ =  4+2;
    *pIndices++ =  4+2;   *pIndices++ =  4+3;   *pIndices++ =  4+0;
    *pIndices++ =  8+0;   *pIndices++ =  8+1;   *pIndices++ =  8+2;
    *pIndices++ =  8+2;   *pIndices++ =  8+3;   *pIndices++ =  8+0;
    *pIndices++ = 12+0;   *pIndices++ = 12+1;   *pIndices++ = 12+2;
    *pIndices++ = 12+2;   *pIndices++ = 12+3;   *pIndices++ = 12+0;
    *pIndices++ = 16+0;   *pIndices++ = 16+1;   *pIndices++ = 16+2;
    *pIndices++ = 16+2;   *pIndices++ = 16+3;   *pIndices++ = 16+0;
    *pIndices++ = 20+0;   *pIndices++ = 20+1;   *pIndices++ = 20+2;
    *pIndices++ = 20+2;   *pIndices++ = 20+3;   *pIndices++ = 20+0;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{
    HRESULT hr;

    // Generate the cube data
    CreateCube( g_pCubeVertices, g_pCubeIndices );

    // Create a dummy texture. What we want is D3D to make a 16 bit surface
    // with dimensions 128x128. We're just loading a dummy texture that won't
    // be seen, tis better to let the framework deal with all the dirty work.
    // this one call saves us about 50 lines of drudgery.
    if( FAILED( hr = D3DTextr_CreateTexture( "videotex.bmp" ) ) )
    {
        MessageBox( hWnd, "Can't find VideoTex.bmp file.", g_strAppTitle,
                    MB_ICONERROR|MB_OK );
        return E_FAIL;
    }

    // Initialize the AVI library
    AVIFileInit();

    // Open our AVI file
    if( FAILED( hr = AVIStreamOpenFromFile( &g_paviStream, "skiing.avi",
                                            streamtypeVIDEO, 0, OF_READ,
                                            NULL ) ) )
    {
        // If the AVI was not found, let's check the system registry for an
        // alternate path.
        HKEY key;
        LONG result = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                                    TEXT("Software\\Microsoft\\DirectX"),
                                    0, KEY_READ, &key );
        if( ERROR_SUCCESS == result )
        {
            TCHAR strFullPath[512];
            TCHAR strPath[512];
            DWORD type, size = 512;
            result = RegQueryValueEx( key, TEXT("DX6SDK Samples Path"), NULL,
                                      &type, (BYTE*)strPath, &size );
            RegCloseKey( key );

            if( ERROR_SUCCESS == result )
            {
                sprintf( strFullPath, TEXT("%s\\D3DIM\\Media\\%s"), strPath,
                           "skiing.avi");
                hr = AVIStreamOpenFromFile( &g_paviStream, strFullPath,
                                            streamtypeVIDEO, 0, OF_READ, NULL );
            }
        }

        // If the AVI was still not found, return failure.
        if( FAILED( hr ) )
        {
            MessageBox( hWnd, "Can't find Skiing.avi file.", g_strAppTitle,
                        MB_ICONERROR|MB_OK );
            return E_FAIL;
        }
    }

    // Load the video stream
    if( NULL == ( g_pgfFrame = AVIStreamGetFrameOpen( g_paviStream, NULL ) ) )
        return E_FAIL;

    // Get the stream info
    if( FAILED( hr = AVIStreamInfo( g_paviStream, &g_psiStreamInfo,
                                    sizeof(AVISTREAMINFO) ) ) )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT App_FrameMove( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{
    // Rotate the cube's world matrix
    D3DMATRIX matScale, matRotate, matWorld;
    D3DUtil_SetScaleMatrix( matScale, CUBE_SCALE,CUBE_SCALE,CUBE_SCALE );
    D3DUtil_SetRotateYMatrix( matRotate, fTimeKey );
    D3DMath_MatrixMultiply( matWorld, matRotate, matScale );
    pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld );

    // Update the AVI frame in the texture
    UpdateTexture( pd3dDevice );

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
    pvViewport->Clear2( 1UL, prcViewRect, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                        0x000000ff, 1.0f, 0L );

    // Begin the scene
    if( FAILED( pd3dDevice->BeginScene() ) )
        return S_OK; // Don't return a "fatal" error

    // Draw the cube
    pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                      g_pCubeVertices, NUM_CUBE_VERTICES,
                                      g_pCubeIndices,  NUM_CUBE_INDICES, 0 );

    // End the scene.
    pd3dDevice->EndScene();

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

    pd3dDevice->SetLightState( D3DLIGHTSTATE_AMBIENT, 0xffffffff );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_TEXTUREPERSPECTIVE , TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
    
    // Set the projection matrix
    D3DMATRIX matProj;
    D3DVIEWPORT2 vp;
    vp.dwSize = sizeof(vp);
    pvViewport->GetViewport2(&vp);
    FLOAT fAspect = ((FLOAT)vp.dwHeight) / vp.dwWidth;
    D3DUtil_SetProjectionMatrix( matProj, g_PI/4, fAspect, 1.0f, 100.0f );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

    // Set the view matrix
    D3DMATRIX matView;
    D3DVECTOR vEyePt( 0.0f, 10.0f, -15.0f );
    D3DVECTOR vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DVECTOR vUpVec( 0.0f, 1.0f, 0.0f );
    D3DUtil_SetViewMatrix( matView, vEyePt, vLookatPt, vUpVec );
    pd3dDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &matView );

    // Set up the texture
    D3DTextr_RestoreAllTextures( pd3dDevice );
    pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("videotex.bmp") );

    // Get the texture surface, so we can change it's flags.
    LPDIRECTDRAWSURFACE4 pddsTexture = D3DTextr_GetSurface("videotex.bmp");
    if( NULL == pddsTexture )
        return E_FAIL;

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

    // Release the AVI file stream, and close the AVI library
    AVIStreamRelease( g_paviStream );
    AVIFileExit();

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
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateTexture()
// Desc: Called once per frame, updates the texture with the next video frame.
//-----------------------------------------------------------------------------
HRESULT UpdateTexture( LPDIRECT3DDEVICE3 pd3dDevice )
{
    // Get the texture surface so we can write to it.
    LPDIRECTDRAWSURFACE4 pddsTexture = D3DTextr_GetSurface("videotex.bmp");
    if( NULL == pddsTexture )
        return E_FAIL;

    // Lock the surface so we can access it's bits
    DDSURFACEDESC2 ddsd;
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    pddsTexture->Lock( NULL, &ddsd, DDLOCK_WRITEONLY, NULL );

    // Check the surface's pixel format. Depending on the device and hardware
    // we're using, the system could create either 565 or 555 format 16 bit
    // surfaces. This app fuddles with texture bits, so we need to know what
    // format we're writing the AVI images to.
    BOOL bTextureIs565 = (ddsd.ddpfPixelFormat.dwGBitMask==0x7E0)?TRUE:FALSE;

    // Save the start time of the AVI file
    static FLOAT fAVIStartTime = ((FLOAT)clock())/CLOCKS_PER_SEC;

    // Use the clock to find which frame we should be drawing
    FLOAT fCurrTime     = ((FLOAT)clock())/CLOCKS_PER_SEC;
    FLOAT fElapsedTime  = fCurrTime-fAVIStartTime;
    FLOAT fAVITimeScale = ((FLOAT)g_psiStreamInfo.dwRate) / g_psiStreamInfo.dwScale;
    DWORD dwCurrFrame   = (DWORD)( fElapsedTime * fAVITimeScale );

    // If we exceed the AVI length, wrap to the start of the AVI
    if( dwCurrFrame >= g_psiStreamInfo.dwLength )
    {
        fAVIStartTime = ((FLOAT)clock())/CLOCKS_PER_SEC;
        dwCurrFrame   = g_psiStreamInfo.dwStart + 1;
    }

    // Get the current frame of the video
    BITMAPINFO* pbmi;
    if( FAILED( pbmi = (LPBITMAPINFO)AVIStreamGetFrame( g_pgfFrame, dwCurrFrame ) ) )
        return E_FAIL;

    // Copy the current frame image to the surface. We recieve the video data
    // as a void pointer to a packed DIB. We have to skip past the header that
    // preceeds the raw video data.
    WORD* pSrc  = (WORD*)( sizeof(BITMAPINFO) + (BYTE*)pbmi );
    WORD* pDest = (WORD*)ddsd.lpSurface;

    // Copy the bits, pixel by pixel. Note that we need to handle the 565 and
    // 555 formats diffrently.
    if( bTextureIs565 )
    {
        for( DWORD i=0; i < 128*128; i++ )
        {
            WORD color = *pSrc++;
            *pDest++ = ((color & 0x1F)|((color & 0xFFE0)<<1));
        }
    }
    else
        memcpy( pDest, pSrc, sizeof(WORD)*128*128 );

    // We're done. Unlock the texture surface and return.
    pddsTexture->Unlock(NULL);

    return S_OK;
}



