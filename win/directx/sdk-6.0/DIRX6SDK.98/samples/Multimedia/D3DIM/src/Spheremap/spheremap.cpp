//-----------------------------------------------------------------------------
// File: SphereMap.cpp
//
// Desc: Example code showing how to use sphere-mapping in D3DIM. The main part
//       of the code is how the sample must do it's own world-view
//       transformation of the object's normals. Based of the value of the
//       transformed normals, the corresponding vertex's texture coords are set
//       to the appopriate spot in the spheremap.
//
//       Note: This code uses the D3D Framework helper library.
//
//
// Copyright (c) 1997-1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define STRICT
#define D3D_OVERLOADS
#include <math.h>
#include <stdio.h>
#include "D3DTextr.h"
#include "D3DUtil.h"
#include "D3DMath.h"
#include "resource.h"


//-----------------------------------------------------------------------------
// Declare the application globals for use in WinMain.cpp
//-----------------------------------------------------------------------------
TCHAR* g_strAppTitle       = TEXT( "SphereMap: Using spheremaps in Direct3D" );
BOOL   g_bAppUseZBuffer    = TRUE;
BOOL   g_bAppUseBackBuffer = TRUE;




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
D3DVERTEX* g_pTeapotVertices     = NULL;
DWORD      g_dwTeapotNumVertices = 0L;
WORD*      g_pTeapotIndices      = NULL;
DWORD      g_dwTeapotNumIndices  = 0L;




//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
VOID    AppPause( BOOL );
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
HRESULT LoadMeshFromXFile( CHAR* strFilename, D3DVERTEX** ppVertices,
                  DWORD* pdwNumVertices, WORD** ppIndices,
                  DWORD* pdwNumIndices );




//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{
    // Load a DirectX .X file
    if( FAILED( LoadMeshFromXFile( "teapot.x", &g_pTeapotVertices,
                          &g_dwTeapotNumVertices, 
                          &g_pTeapotIndices, &g_dwTeapotNumIndices ) ) )
    {
        MessageBox( NULL, TEXT("Could not load TEAPOT.X file."),
                    TEXT("SphereMap Sample"), MB_OK|MB_ICONERROR );
        return E_FAIL;
    }

    // Load the spheremap texture
    if( FAILED( D3DTextr_CreateTexture( TEXT("SphereMap.bmp") ) ) )
    {
        MessageBox( NULL, TEXT("Could not load SPHEREMAP.BMP texture."),
                    TEXT("SphereMap Sample"), MB_OK|MB_ICONERROR );
        return E_FAIL;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ApplySphereMapToObject()
// Desc: Uses the current orientation of the vertices to calculate the object's
//       spheremapped texture coords.
//-----------------------------------------------------------------------------
HRESULT ApplySphereMapToObject( LPDIRECT3DDEVICE3 pd3dDevice,
                                D3DVERTEX* pvVertices, DWORD dwNumVertices )
{
    // Get the current world-view matrix
    D3DMATRIX matWorld, matView, matWV;
    pd3dDevice->GetTransform( D3DTRANSFORMSTATE_VIEW,  &matView );
    pd3dDevice->GetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
    D3DMath_MatrixMultiply( matWV, matView, matWorld );

    // Extract world-view matrix elements for speed
    FLOAT m11 = matWV._11,   m21 = matWV._21,   m31 = matWV._31;
    FLOAT m12 = matWV._12,   m22 = matWV._22,   m32 = matWV._32;
    FLOAT m13 = matWV._13,   m23 = matWV._23,   m33 = matWV._33;

    // Loop through the vertices, transforming each one and calculating
    // the correct texture coordinates.
    for( WORD i = 0; i < dwNumVertices; i++ )
    {
        FLOAT nx = pvVertices[i].nx;
        FLOAT ny = pvVertices[i].ny;
        FLOAT nz = pvVertices[i].nz;

        // Check the z-component, to skip any vertices that face backwards
        if( nx*m13 + ny*m23 + nz*m33 > 0.0f )
            continue;

        // Assign the spheremap's texture coordinates
        pvVertices[i].tu = 0.5f * ( 1.0f + ( nx*m11 + ny*m21 + nz*m31 ) );
        pvVertices[i].tv = 0.5f * ( 1.0f - ( nx*m12 + ny*m22 + nz*m32 ) );
    }

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
    D3DMATRIX matRotate;
    D3DUtil_SetRotationMatrix( matRotate, D3DVECTOR(1.0f,1.0f,0.0f), 
                               fTimeKey/2  );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matRotate );

    ApplySphereMapToObject( pd3dDevice, g_pTeapotVertices,
                            g_dwTeapotNumVertices );

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
    pvViewport->Clear2( 1UL, prcViewportRect, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                        0x000000ff, 1.0f, 0L );

    // Begin the scene 
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                              g_pTeapotVertices, g_dwTeapotNumVertices, 
                              g_pTeapotIndices, g_dwTeapotNumIndices, NULL );

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

    D3DTextr_RestoreAllTextures( pd3dDevice );
    pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("SphereMap.bmp") );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE,   TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE,        TRUE );
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_AMBIENT,         0xffffffff );

    // Set the transform matrices
    D3DVECTOR vEyePt    = D3DVECTOR( 0.0f, 0.0f, -4.5f );
    D3DVECTOR vLookatPt = D3DVECTOR( 0.0f, 0.0f,  0.0f );
    D3DVECTOR vUpVec    = D3DVECTOR( 0.0f, 1.0f,  0.0f );
    D3DMATRIX matWorld, matView, matProj;

    D3DUtil_SetIdentityMatrix( matWorld );
    D3DUtil_SetViewMatrix( matView, vEyePt, vLookatPt, vUpVec );
    D3DUtil_SetProjectionMatrix( matProj, g_PI/3, 1.0f, 0.5f, 100.0f );

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
    
    SAFE_DELETE( g_pTeapotVertices );
    SAFE_DELETE( g_pTeapotIndices );

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





