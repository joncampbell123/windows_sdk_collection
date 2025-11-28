//-----------------------------------------------------------------------------
// File: Lightmap.cpp
//
// Desc: Example code showing how to enable multiple-texturing. This
//       samples shows the interior of a room "lit" with a light map
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
// Declare the application globals for use in WinMain.cpp
//-----------------------------------------------------------------------------
TCHAR* g_strAppTitle       = TEXT( "Lightmap: DX6 Lightmap Demo" );
BOOL   g_bAppUseZBuffer    = FALSE;
BOOL   g_bAppUseBackBuffer = TRUE;




//-----------------------------------------------------------------------------
// Define a custom vertex that uses XYZ, a color, and two sets of tex coords
//-----------------------------------------------------------------------------
struct MTVERTEX
{
    FLOAT x, y, z;
    DWORD dwColor;
    FLOAT tuBase, tvBase;
    FLOAT tuLightMap, tvLightMap;
};

#define FILL_MTVERTEX( v, ax, ay, az, atu1, atv1, atu2, atv2 )  \
{   v.x = ax; v.y = ay; v.z = az; \
    v.dwColor = 0xffffffff; \
    v.tuBase     = atu1; v.tvBase     = atv1; \
    v.tuLightMap = atu2; v.tvLightMap = atv2; \
}




//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
VOID    AppPause( BOOL );
VOID    AppOutputText( LPDIRECT3DDEVICE3, DWORD, DWORD, CHAR* );
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK App_OverridenWndProc( HWND, UINT, WPARAM, LPARAM );

HMENU     g_hMenu;
BOOL      g_bCanDoMultiTexture;     // Whether device does mtexture
BOOL      g_bUseMultiTexture;

D3DMATRIX g_matLightBillboardingMatrix;
D3DVERTEX g_avLightVertices[4];
D3DVERTEX g_avStringVertices[2];
MTVERTEX  g_avWallVertices[36];
D3DDRAWPRIMITIVESTRIDEDDATA g_WallData;      // Vertex data
D3DDRAWPRIMITIVESTRIDEDDATA g_FloorCielData;




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
    FILL_MTVERTEX( g_avWallVertices[ 0], -5,-5, 5, 0, 1, 0, 1 );
    FILL_MTVERTEX( g_avWallVertices[ 1], -5, 5, 5, 0, 0, 0, 0 );
    FILL_MTVERTEX( g_avWallVertices[ 2],  5,-5, 5, 1, 1, 1, 1 );
    FILL_MTVERTEX( g_avWallVertices[ 3],  5, 5, 5, 1, 0, 1, 0 );
    FILL_MTVERTEX( g_avWallVertices[ 4],  5,-5, 5, 1, 1, 1, 1 );
    FILL_MTVERTEX( g_avWallVertices[ 5], -5, 5, 5, 0, 0, 0, 0 );

    FILL_MTVERTEX( g_avWallVertices[ 6],  5,-5, 5, 0, 1, 0, 1 );
    FILL_MTVERTEX( g_avWallVertices[ 7],  5, 5, 5, 0, 0, 0, 0 );
    FILL_MTVERTEX( g_avWallVertices[ 8],  5,-5,-5, 1, 1, 1, 1 );
    FILL_MTVERTEX( g_avWallVertices[ 9],  5, 5,-5, 1, 0, 1, 0 );
    FILL_MTVERTEX( g_avWallVertices[10],  5,-5,-5, 1, 1, 1, 1 );
    FILL_MTVERTEX( g_avWallVertices[11],  5, 5, 5, 0, 0, 0, 0 );

    FILL_MTVERTEX( g_avWallVertices[12],  5,-5,-5, 0, 1, 0, 1 );
    FILL_MTVERTEX( g_avWallVertices[13],  5, 5,-5, 0, 0, 0, 0 );
    FILL_MTVERTEX( g_avWallVertices[14], -5,-5,-5, 1, 1, 1, 1 );
    FILL_MTVERTEX( g_avWallVertices[15], -5, 5,-5, 1, 0, 1, 0 );
    FILL_MTVERTEX( g_avWallVertices[16], -5,-5,-5, 1, 1, 1, 1 );
    FILL_MTVERTEX( g_avWallVertices[17],  5, 5,-5, 0, 0, 0, 0 );

    FILL_MTVERTEX( g_avWallVertices[18], -5,-5,-5, 0, 1, 0, 1 );
    FILL_MTVERTEX( g_avWallVertices[19], -5, 5,-5, 0, 0, 0, 0 );
    FILL_MTVERTEX( g_avWallVertices[20], -5,-5, 5, 1, 1, 1, 1 );
    FILL_MTVERTEX( g_avWallVertices[21], -5, 5, 5, 1, 0, 1, 0 );
    FILL_MTVERTEX( g_avWallVertices[22], -5,-5, 5, 1, 1, 1, 1 );
    FILL_MTVERTEX( g_avWallVertices[23], -5, 5,-5, 0, 0, 0, 0 );

    FILL_MTVERTEX( g_avWallVertices[24],  5,-5, 5, 0, 1, 0, 1 );
    FILL_MTVERTEX( g_avWallVertices[25],  5,-5,-5, 1, 1, 1, 1 );
    FILL_MTVERTEX( g_avWallVertices[26], -5,-5, 5, 0, 0, 0, 0 );
    FILL_MTVERTEX( g_avWallVertices[27],  5,-5,-5, 1, 1, 1, 1 );
    FILL_MTVERTEX( g_avWallVertices[28], -5,-5,-5, 1, 0, 1, 0 );
    FILL_MTVERTEX( g_avWallVertices[29], -5,-5, 5, 0, 0, 0, 0 );

    FILL_MTVERTEX( g_avWallVertices[30],  5, 5, 5, 0, 1, 0, 1 );
    FILL_MTVERTEX( g_avWallVertices[31], -5, 5, 5, 0, 0, 0, 0 );
    FILL_MTVERTEX( g_avWallVertices[32],  5, 5,-5, 1, 1, 1, 1 );
    FILL_MTVERTEX( g_avWallVertices[33], -5, 5,-5, 1, 0, 1, 0 );
    FILL_MTVERTEX( g_avWallVertices[34],  5, 5,-5, 1, 1, 1, 1 );
    FILL_MTVERTEX( g_avWallVertices[35], -5, 5, 5, 0, 0, 0, 0 );

    // Setup the vertices for the hanging light and its string
    D3DVECTOR vNorm(0,1,0);
    g_avLightVertices[0]  = D3DVERTEX( D3DVECTOR(-0.5, 0.5, 0), vNorm, 0, 0 );
    g_avLightVertices[1]  = D3DVERTEX( D3DVECTOR(-0.5,-0.5, 0), vNorm, 1, 0 );
    g_avLightVertices[2]  = D3DVERTEX( D3DVECTOR( 0.5, 0.5, 0), vNorm, 0, 1 );
    g_avLightVertices[3]  = D3DVERTEX( D3DVECTOR( 0.5,-0.5, 0), vNorm, 1, 1 );
    g_avStringVertices[0] = D3DVERTEX( D3DVECTOR(0,5,0), vNorm, 0.0f, 0.0f );
    g_avStringVertices[1] = D3DVERTEX( D3DVECTOR(0,5,0), vNorm, 0.5f, 0.5f );

    // Set up data structures for using strided vertices.
    g_WallData.position.lpvData          = &g_avWallVertices[0].x;
    g_WallData.diffuse.lpvData           = &g_avWallVertices[0].dwColor;
    g_WallData.textureCoords[0].lpvData  = &g_avWallVertices[0].tuBase;
    g_WallData.textureCoords[1].lpvData  = &g_avWallVertices[0].tuLightMap;
    g_WallData.position.dwStride         = sizeof(MTVERTEX);
    g_WallData.diffuse.dwStride          = sizeof(MTVERTEX);
    g_WallData.textureCoords[0].dwStride = sizeof(MTVERTEX);
    g_WallData.textureCoords[1].dwStride = sizeof(MTVERTEX);

    g_FloorCielData.position.lpvData          = &g_avWallVertices[24].x;
    g_FloorCielData.diffuse.lpvData           = &g_avWallVertices[24].dwColor;
    g_FloorCielData.textureCoords[0].lpvData  = &g_avWallVertices[24].tuBase;
    g_FloorCielData.textureCoords[1].lpvData  = &g_avWallVertices[24].tuLightMap;
    g_FloorCielData.position.dwStride         = sizeof(MTVERTEX);
    g_FloorCielData.diffuse.dwStride          = sizeof(MTVERTEX);
    g_FloorCielData.textureCoords[0].dwStride = sizeof(MTVERTEX);
    g_FloorCielData.textureCoords[1].dwStride = sizeof(MTVERTEX);

    // Create some textures. The number passed in is a hint (for some hardware)
    // as to which texture stage state the texture will be used.
    D3DTextr_CreateTexture( "wall.bmp",   0 );
    D3DTextr_CreateTexture( "floor.bmp",  0 );
    D3DTextr_CreateTexture( "lightmap.bmp", 1 );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT App_FrameMove( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{
    // Rotate the camera around the room
    FLOAT fViewAngle = fTimeKey/5;
    D3DMATRIX matView;
    D3DVECTOR up(0.0f, 1.0f, 0.0f);
    D3DVECTOR from( (FLOAT)sin(fViewAngle), 0, (FLOAT)cos(fViewAngle) );
    D3DVECTOR at( 0.0f, 0.0f, 0.0f );
    D3DUtil_SetViewMatrix( matView, 5.0f * from, at, up );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &matView );

    // Move light, as a swinging pendulum
    FLOAT     fPendulumTheta = 1.0f*(FLOAT)sin( fTimeKey );
    D3DVECTOR vLightPos;
    vLightPos.x = 0.0f + 3.0f*(FLOAT)sin( fPendulumTheta );
    vLightPos.y = 3.0f - 3.0f*(FLOAT)cos( fPendulumTheta );
    vLightPos.z = 0.0f;

    // Position the end of the string
    g_avStringVertices[1].x = vLightPos.x;
    g_avStringVertices[1].y = vLightPos.y;

    // Build the billboarding matrix for the light (so the light always 
    // renders face-on to the camera)
    D3DUtil_SetRotateYMatrix( g_matLightBillboardingMatrix, fViewAngle );
    g_matLightBillboardingMatrix._41 += vLightPos.x;
    g_matLightBillboardingMatrix._42 += vLightPos.y;
    g_matLightBillboardingMatrix._43 += vLightPos.z;

    // Apply the lightmap to the six sides of the room
    for( int w=0; w<6; w++ )
    {
        FLOAT fDistance, tuLightPos, tvLightPos;

        // Calc the lightmaps tex coords based on the light pos
        switch( w )
        {
            case 0: fDistance = 5.0f;
                    tuLightPos = -vLightPos.x/5;
                    tvLightPos = vLightPos.y/5;
                    break;
            case 1: fDistance = 5.0f - vLightPos.x;
                    tuLightPos = 0.0f;
                    tvLightPos = vLightPos.y/5;
                    break;
            case 2: fDistance = 5.0f;
                    tuLightPos = vLightPos.x/5;
                    tvLightPos = vLightPos.y/5;
                    break;
            case 3: fDistance = 5.0f + vLightPos.x;
                    tuLightPos = 0.0f;
                    tvLightPos = vLightPos.y/5;
                    break;
            case 4: fDistance = 5.0f + vLightPos.y;
                    tuLightPos = 0.0f;
                    tvLightPos = -vLightPos.x/5;
                    break;
            case 5: fDistance = 5.0f - vLightPos.y;
                    tuLightPos = 0.0f;
                    tvLightPos = -vLightPos.x/5;
                    break;
        }

        // Calculate the size and color for the lightmaps' vertices
        FLOAT fBrightnessScale = 1.0f - (fDistance-2)/10.0f;
        DWORD dwDiffuse = 0x01010101 * (DWORD)(255*fBrightnessScale);
        FLOAT fSizeScale = 1.0f + fDistance/5.0f;

        // Apply the lightmap to the vertices
        for( int v=0; v<6; v++ )
        {
            // Translate and scale the lightmap
            FLOAT tu = g_avWallVertices[6*w+v].tuBase - 0.5f + fSizeScale/2;
            FLOAT tv = g_avWallVertices[6*w+v].tvBase - 0.5f + fSizeScale/2;
            g_avWallVertices[6*w+v].tuLightMap = ( tu + tuLightPos ) / fSizeScale;
            g_avWallVertices[6*w+v].tvLightMap = ( tv + tvLightPos ) / fSizeScale;
            
            // Modify the brightness of the lightmap
            g_avWallVertices[6*w+v].dwColor = dwDiffuse;
        }
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
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        pd3dDevice->SetRenderState( D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_CLAMP );
    
        if( g_bUseMultiTexture ) // Use mtexture features of device
        {
            // Set up the texture stages (Note: we don't really need to do this
            // every frame)
            pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
            pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
            pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
            pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
            pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
            pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT ); 
            pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );

            g_WallData.textureCoords[0].lpvData  = &g_avWallVertices[0].tuBase;
            g_WallData.textureCoords[1].lpvData  = &g_avWallVertices[0].tuLightMap;

            // Draw the walls in multi-texture mode
            pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("wall.bmp") );
            pd3dDevice->SetTexture( 1, D3DTextr_GetTexture("lightmap.bmp") );
            pd3dDevice->DrawPrimitiveStrided( D3DPT_TRIANGLELIST,
                                        D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2,
                                        &g_WallData, 24, NULL );

            // Draw the floor in single-texture mode
            pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("floor.bmp") );
            pd3dDevice->SetTexture( 1, NULL );
            pd3dDevice->DrawPrimitiveStrided( D3DPT_TRIANGLELIST,
                                        D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2,
                                        &g_FloorCielData, 12, NULL );

            // Restore state
            pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
        }
        else // Else, resort to multipass rendering
        {
            // Draw the first textures normally. Use the 1st set of tex coords.
            pd3dDevice->SetRenderState( D3DRENDERSTATE_TEXTUREMAPBLEND,  D3DTBLEND_COPY );
            g_WallData.textureCoords[0].lpvData = &g_avWallVertices[0].tuBase;
            
            pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("wall.bmp") );
            pd3dDevice->DrawPrimitiveStrided( D3DPT_TRIANGLELIST,
                                        D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2,
                                        &g_WallData, 24, NULL );
            pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("floor.bmp") );
            pd3dDevice->DrawPrimitiveStrided( D3DPT_TRIANGLELIST,
                                        D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2,
                                        &g_FloorCielData, 12, NULL );

            // Draw the lightmap using blending, with the 2nd set of tex coords
            pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
            pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,  D3DBLEND_ZERO );
            pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCCOLOR );

            g_WallData.textureCoords[0].lpvData = &g_avWallVertices[0].tuLightMap;
            pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("lightmap.bmp") );
            pd3dDevice->DrawPrimitiveStrided( D3DPT_TRIANGLELIST,
                                        D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2,
                                        &g_WallData, 36, NULL );
            // Restore state
            pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
        }

        // Draw the string
        pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("lightmap.bmp") );
        pd3dDevice->DrawPrimitive( D3DPT_LINELIST, D3DFVF_VERTEX, 
                                   g_avStringVertices, 2, NULL );
        // Draw the light
        D3DMATRIX matSave;
        pd3dDevice->GetTransform( D3DTRANSFORMSTATE_WORLD, &matSave );
        pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,
                                  &g_matLightBillboardingMatrix );
        pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
        pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,  D3DBLEND_ONE );
        pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE );
        pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, 
                                   g_avLightVertices, 4, NULL );
        // Restore state
        pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
        pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matSave );

        // End the scene.
        pd3dDevice->EndScene();

        // Output which rendering technique we're using
        if( g_bUseMultiTexture )
            AppOutputText( pd3dDevice, 0, 20, "Using Multi-texture" );
        else
            AppOutputText( pd3dDevice, 0, 20, "Using Multi-pass" );
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

    // Load the textures to the device
    D3DTextr_RestoreAllTextures( pd3dDevice );

    // Check whether the device supports real multitexturing (if not, we're
    // going to emulate it using multipass rendering)
    D3DDEVICEDESC ddHwDesc, ddSwDesc;
    ddHwDesc.dwSize = sizeof(D3DDEVICEDESC);
    ddSwDesc.dwSize = sizeof(D3DDEVICEDESC);
    pd3dDevice->GetCaps( &ddHwDesc, &ddSwDesc );
    g_bCanDoMultiTexture = ( ddHwDesc.wMaxSimultaneousTextures >=2 || 
                             ddSwDesc.wMaxSimultaneousTextures >=2 );

    // Set the menu states for multitexture devices and devices that emulate
    // multitexture using multipass rendering.
    if( g_bCanDoMultiTexture )
    {
        EnableMenuItem( g_hMenu, IDM_MULTITEXTURE, MF_ENABLED );
        CheckMenuItem(  g_hMenu, IDM_MULTIPASS,    MF_UNCHECKED );
        CheckMenuItem(  g_hMenu, IDM_MULTITEXTURE, MF_CHECKED );
        g_bUseMultiTexture = TRUE;
    }
    else
    {
        EnableMenuItem( g_hMenu, IDM_MULTITEXTURE, MF_GRAYED );
        CheckMenuItem(  g_hMenu, IDM_MULTIPASS,    MF_CHECKED );
        CheckMenuItem(  g_hMenu, IDM_MULTITEXTURE, MF_UNCHECKED );
        g_bUseMultiTexture = FALSE;
    }

    // Set the transform matrices
    D3DMATRIX matWorld, matView, matProj;
    D3DUtil_SetIdentityMatrix( matWorld );
    D3DUtil_SetProjectionMatrix( matProj, g_PI/2, 1.0f, 1.0f, 100.0f );

    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW,       &matView );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

    // Set any appropiate state
    pd3dDevice->SetRenderState( D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTFN_LINEAR );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTFG_LINEAR );

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
    // Accept devices that really support multiple textures. If not, accept
    // device that support alpha blending to emulate multi-texturing with
    // mulit-pass rendering.
    if( pd3dDeviceDesc->wMaxSimultaneousTextures > 1 )
        return S_OK;
    if( pd3dDeviceDesc->dpcTriCaps.dwDestBlendCaps & D3DPBLENDCAPS_SRCCOLOR )
        if( pd3dDeviceDesc->dpcTriCaps.dwSrcBlendCaps & D3DPBLENDCAPS_ZERO )
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
            case IDM_MULTIPASS:
                g_bUseMultiTexture = FALSE;
                CheckMenuItem( g_hMenu, IDM_MULTIPASS,    MF_CHECKED );
                CheckMenuItem( g_hMenu, IDM_MULTITEXTURE, MF_UNCHECKED );
                break;
            case IDM_MULTITEXTURE:
                g_bUseMultiTexture = TRUE;
                CheckMenuItem( g_hMenu, IDM_MULTIPASS,    MF_UNCHECKED );
                CheckMenuItem( g_hMenu, IDM_MULTITEXTURE, MF_CHECKED );
                break;
        }
    }
    return WndProc( hWnd, uMsg, wParam, lParam );
}




