//-----------------------------------------------------------------------------
// File: Billboard.cpp
//
// Desc: Example code showing how to do billboarding. The sample uses
//       billboarding to draw some trees.
//
//       Note: This code uses the D3D Framework helper library.
//
//
// Copyright (c) 1995-1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define STRICT
#define D3D_OVERLOADS
#include <math.h>
#include <stdio.h>
#include "D3DTextr.h"
#include "D3DUtil.h"
#include "D3DMath.h"


//-----------------------------------------------------------------------------
// Declare the application globals for use in WinMain.cpp
//-----------------------------------------------------------------------------
TCHAR* g_strAppTitle       = TEXT( "Billboard: DX6 Billboarding Example" );
BOOL   g_bAppUseZBuffer    = TRUE;
BOOL   g_bAppUseBackBuffer = TRUE;




//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
VOID    AppPause( BOOL );
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define NUM_DEBRIS  512     // Tree, shadow, and point data
#define NUM_TREES   128
#define MAX_WIDTH    50.0f

BOOL        g_bDeviceDoesAlphaTest;
D3DLVERTEX  g_TreeMesh[4];
D3DTLVERTEX g_BackgroundMesh[4];
WORD        g_Indices[12] = { 0,1,3, 1,2,3, 3,1,0, 3,2,1 };
D3DLVERTEX  g_Debris[NUM_DEBRIS];
D3DVECTOR   g_TreePositions[NUM_TREES];
WORD        g_TreeLOD[NUM_TREES];
FLOAT       g_fViewAngle; // Angle to rotate trees by, to align with the camera

inline FLOAT RandomPos()   { return (MAX_WIDTH*(FLOAT)(rand()-rand()))/RAND_MAX; }
inline FLOAT RandomColor() { return 0.1f+0.5f*rand()/RAND_MAX; }




//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{
    // Initialize the tree and debris data
    for( WORD i=0; i<NUM_TREES; i++ )
        g_TreePositions[i] = D3DVECTOR( RandomPos(), 0.0f, RandomPos() );

    for( i=0; i<NUM_DEBRIS; i++ )
        g_Debris[i] = D3DLVERTEX( D3DVECTOR( RandomPos(), 0.0f, RandomPos() ),
                          D3DRGBA( RandomColor(), RandomColor(), 0.0f, 1.0f ),
                          0, 0.0f, 0.0f );

    // Initialize the tree and background meshes
    g_TreeMesh[0] = D3DLVERTEX( D3DVECTOR(-1, 0, 0), 0xffffffff, 0, 0.0f, 1.0f );
    g_TreeMesh[1] = D3DLVERTEX( D3DVECTOR(-1, 2, 0), 0xffffffff, 0, 0.0f, 0.0f );
    g_TreeMesh[2] = D3DLVERTEX( D3DVECTOR( 1, 2, 0), 0xffffffff, 0, 1.0f, 0.0f );
    g_TreeMesh[3] = D3DLVERTEX( D3DVECTOR( 1, 0, 0), 0xffffffff, 0, 1.0f, 1.0f );

    g_BackgroundMesh[0] = D3DTLVERTEX( D3DVECTOR(0,0,0.99f), 0.5f, -1, 0, 0, 1 );
    g_BackgroundMesh[1] = D3DTLVERTEX( D3DVECTOR(0,0,0.99f), 0.5f, -1, 0, 0, 0 );
    g_BackgroundMesh[2] = D3DTLVERTEX( D3DVECTOR(0,0,0.99f), 0.5f, -1, 0, 1, 1 );
    g_BackgroundMesh[3] = D3DTLVERTEX( D3DVECTOR(0,0,0.99f), 0.5f, -1, 0, 1, 0 );

    // Create some textures
    D3DTextr_CreateTexture( "Cloud3.bmp" );
    D3DTextr_CreateTexture( "Shadow1.bmp", 0, D3DTEXTR_TRANSPARENTWHITE );
    D3DTextr_CreateTexture( "Tree0.bmp",   0, D3DTEXTR_TRANSPARENTBLACK );
    D3DTextr_CreateTexture( "Tree1.bmp",   0, D3DTEXTR_TRANSPARENTBLACK );
    D3DTextr_CreateTexture( "Tree2.bmp",   0, D3DTEXTR_TRANSPARENTBLACK );
    D3DTextr_CreateTexture( "Tree3.bmp",   0, D3DTEXTR_TRANSPARENTBLACK );
    D3DTextr_CreateTexture( "Tree4.bmp",   0, D3DTEXTR_TRANSPARENTBLACK );

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

    // Check the alpha test caps of the device. (The alpha test offers a 
    // performance boost to not render pixels less than some alpha threshold.)
    D3DDEVICEDESC ddHELDesc, ddHALDesc;
    ddHELDesc.dwSize = sizeof(D3DDEVICEDESC);
    ddHALDesc.dwSize = sizeof(D3DDEVICEDESC);
    pd3dDevice->GetCaps( &ddHALDesc, &ddHELDesc );
    D3DDEVICEDESC* pDesc = (ddHALDesc.dwFlags) ? &ddHALDesc : &ddHELDesc;
    if( pDesc->dpcTriCaps.dwAlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
        g_bDeviceDoesAlphaTest = TRUE;
    else
        g_bDeviceDoesAlphaTest = FALSE;

        
    // Set up the dimensions for the background image
    D3DVIEWPORT2 vp;
    vp.dwSize = sizeof(vp);
    pvViewport->GetViewport2(&vp);
    g_BackgroundMesh[0].sy = (FLOAT)vp.dwHeight;
    g_BackgroundMesh[2].sy = (FLOAT)vp.dwHeight;
    g_BackgroundMesh[2].sx = (FLOAT)vp.dwWidth;
    g_BackgroundMesh[3].sx = (FLOAT)vp.dwWidth;

    // Set the transform matrices
    D3DMATRIX matProj;
    D3DUtil_SetProjectionMatrix( matProj, 1.57f, 1.0f, 1.0f, 100.0f );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

    // Set up the default texture states
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );

    D3DTextr_RestoreAllTextures( pd3dDevice );

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
// Name: App_FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT App_FrameMove( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{
    // Move the camera about a large circle through the trees
    g_fViewAngle = ( (fTimeKey/12)-(FLOAT)floor(fTimeKey/12) ) * 2 * g_PI;

    FLOAT x1 = 20.0f*(FLOAT)cos(g_fViewAngle);
    FLOAT y1 =  3.0f;
    FLOAT z1 = 20.0f*(FLOAT)sin(g_fViewAngle);

    FLOAT x2 = 20.0f*(FLOAT)cos(g_fViewAngle+0.1f);
    FLOAT y2 =  3.0f;
    FLOAT z2 = 20.0f*(FLOAT)sin(g_fViewAngle+0.1f);

    D3DVECTOR up(0.0f, 1.0f, 0.0f);
    D3DVECTOR from( x1, y1, z1 );
    D3DVECTOR to( x2, y2, z2 );
    D3DVECTOR at = from + 10*Normalize(to-from);

    D3DMATRIX matView;
    D3DUtil_SetViewMatrix( matView, from, at, up);
    pd3dDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &matView);

    // Scroll the background texture
    FLOAT tu = (fTimeKey/9)-(FLOAT)floor(fTimeKey/9);
    g_BackgroundMesh[0].tu = g_BackgroundMesh[1].tu = tu;
    g_BackgroundMesh[2].tu = g_BackgroundMesh[3].tu = tu - 1.0f;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DrawTrees()
// Desc:
//-----------------------------------------------------------------------------
BOOL DrawTrees(LPDIRECT3DDEVICE3 pd3dDevice )
{
    D3DMATRIX matIdentity;
    D3DUtil_SetIdentityMatrix( matIdentity );

    // Draw the background
    pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("Cloud3.bmp") );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,  D3DTOP_DISABLE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHATESTENABLE,  FALSE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
    pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX,
                               g_BackgroundMesh, 4, 0 );

    // Render the debris
    pd3dDevice->SetTexture( 0, NULL );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matIdentity );
    pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, D3DFVF_LVERTEX,
                               (VOID*)g_Debris, NUM_DEBRIS, 0 );

    // Set state for rendering shadows
    pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("Shadow1.bmp") );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE,   TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,  D3DBLEND_ZERO );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCCOLOR );

    // Enable alpha blending using the texture's alpha channel
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

    // Turn on alpha testing in case the device supports it. (This is a 
    // performance boost to avoid drawing pixels with less than a certain
    // alpha.)
    if( g_bDeviceDoesAlphaTest )
    {
        pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHATESTENABLE, TRUE );
        pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHAREF,        0x08 );
        pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATEREQUAL );
    }

    // Loop through the trees rendering the shadows
    for( WORD i=0; i<NUM_TREES; i++ )
    {
        // Rotate the world matrix, to lay the shadow on the ground
        D3DMATRIX matWorld, matTrans, matRotate;
        D3DUtil_SetTranslateMatrix( matTrans, g_TreePositions[i]);
        D3DUtil_SetRotateXMatrix( matRotate, g_PI/2.0f);
        D3DMath_MatrixMultiply(matWorld, matTrans, matRotate );
        pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );

        pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_LVERTEX,
                                          g_TreeMesh, 4, g_Indices, 6, 0 );
    }

    // Compute the level-of-distance (LOD) for each tree.
    D3DMATRIX mv;
    pd3dDevice->GetTransform( D3DTRANSFORMSTATE_VIEW,  &mv );

    for( i=0; i<NUM_TREES; i++ )
    {
        FLOAT x = g_TreePositions[i].x;
        FLOAT y = g_TreePositions[i].y;
        FLOAT z = g_TreePositions[i].z;

        FLOAT fDistance = x*mv._13 + y*mv._23 + z*mv._33 + mv._43;

        if( fDistance < 4.0f )          g_TreeLOD[i] = 0;
        else if( fDistance < 8.0f )     g_TreeLOD[i] = 1;
        else if( fDistance < 16.0f )    g_TreeLOD[i] = 2;
        else if( fDistance < 32.0f )    g_TreeLOD[i] = 3;
        else                            g_TreeLOD[i] = 4;
    }

    // Set state for drawing trees. Note that many states that were set for
    // rendering shadows are kept for rendering the trees.
    pd3dDevice->SetRenderState( D3DRENDERSTATE_TEXTUREPERSPECTIVE, FALSE );
    
    // Set diffuse blending for alpha set in vertices. 
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,  D3DBLEND_SRCALPHA );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA );

    // Finally, for each LOD, draw all the trees
    for( WORD j=0; j<5; j++ )
    {
        // Set the appropiate texture
        TCHAR strTreeTexture[20];
        sprintf( strTreeTexture, "Tree%d.bmp", j );
        pd3dDevice->SetTexture( 0, D3DTextr_GetTexture(strTreeTexture) );

        for( i=0; i<NUM_TREES; i++ )
        {
            if( j != g_TreeLOD[i] )
                continue;

            // Draw the tree
            D3DMATRIX matWorld;
            D3DUtil_SetRotateYMatrix( matWorld, -g_fViewAngle );
            matWorld._41 += g_TreePositions[i].x;
            matWorld._42 += g_TreePositions[i].y;
            matWorld._43 += g_TreePositions[i].z;

            pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
            pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
                                              D3DFVF_LVERTEX, g_TreeMesh, 4,
                                              g_Indices, 12, 0 );
        }
    }

    return TRUE;
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
    pvViewport->Clear2( 1UL, prcViewRect, D3DCLEAR_ZBUFFER, 0L, 1.0f, 0L );

    // Begin the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        // ok, now move through the trees
        DrawTrees(pd3dDevice);

        // End the scene.
        pd3dDevice->EndScene();
    }

    return S_OK;
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
    // This sample uses alpha textures and/or straight alpha. Make sure the 
    // device supports them
    DWORD dwDeviceCaps = pd3dDeviceDesc->dpcTriCaps.dwTextureCaps;
    if( dwDeviceCaps & D3DPTEXTURECAPS_ALPHAPALETTE )
        return S_OK;
    if( dwDeviceCaps & D3DPTEXTURECAPS_ALPHA )
        return S_OK;

    return E_FAIL;
}





