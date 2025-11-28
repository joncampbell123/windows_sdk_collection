//-----------------------------------------------------------------------------
// File: Lights.cpp
//
// Desc: Example code showing how to do lights in D3D.
//
//       Note: This code uses the D3D Framework helper library.
//
//
// Copyright (c) 1997-1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define STRICT
#define D3D_OVERLOADS
#include <math.h>
#include <time.h>
#include <stdio.h>
#include "D3DTextr.h"
#include "D3DUtil.h"
#include "D3DMath.h"


//-----------------------------------------------------------------------------
// Declare the application globals for use in WinMain.cpp
//-----------------------------------------------------------------------------
TCHAR* g_strAppTitle       = TEXT( "Lights: D3D Lighting Sample" );
BOOL   g_bAppUseZBuffer    = FALSE;
BOOL   g_bAppUseBackBuffer = TRUE;




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
LPDIRECT3DMATERIAL3 g_pmtrlObjectMtrl     = NULL;
LPDIRECT3DLIGHT     g_pLight              = NULL;

// mesh size must be even for linelists to work
#define MESH_SIZE       12
#define NUM_VERTICES    (MESH_SIZE*MESH_SIZE)
#define NUM_INDICES     ((MESH_SIZE-1)*(MESH_SIZE-1)*6)

D3DVERTEX   g_WallVertices[NUM_VERTICES];
WORD        g_WallIndices[NUM_INDICES];

// Determines how fine mesh is used for spheres and cylinders.
#define SPHERE_MESH_SIZE    4

#define SPHERE_VERTICES (2+SPHERE_MESH_SIZE*SPHERE_MESH_SIZE*2)
#define SPHERE_INDICES  ((SPHERE_MESH_SIZE*4 + SPHERE_MESH_SIZE*4*(SPHERE_MESH_SIZE-1))*3)
D3DVERTEX   sphere[SPHERE_VERTICES];
WORD        sphere_indices[SPHERE_INDICES];


#define rnd() ( ( ((FLOAT)rand())-((FLOAT)rand()) ) / RAND_MAX )




//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
VOID    AppOutputText( LPDIRECT3DDEVICE3, DWORD, DWORD, CHAR* );
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );




//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{
    WORD i, j, ind, v;

    // seed the random number generator
    srand( time(0) );

    // Generate a square mesh in XZ plane from 0,0 to 1,1 for the walls
    for( i=0; i<MESH_SIZE; i++ )
    {
        for( j=0; j<MESH_SIZE; j++ )
        {
            FLOAT      x = i / (FLOAT)(MESH_SIZE-1);
            FLOAT      y = j / (FLOAT)(MESH_SIZE-1);
            D3DVERTEX* v = &g_WallVertices[i*MESH_SIZE+j];
            (*v) = D3DVERTEX( D3DVECTOR(x,0,y), D3DVECTOR(0,1,0), x, y );
        }
    }

    // Generate the wall indices
    for( i=ind=0; i<MESH_SIZE-1; i++ )
    {
        for( j=0; j<MESH_SIZE-1; j++ )
        {
            g_WallIndices[ind++] = (i+0)*MESH_SIZE + (j+0);
            g_WallIndices[ind++] = (i+0)*MESH_SIZE + (j+1);
            g_WallIndices[ind++] = (i+1)*MESH_SIZE + (j+0);
            g_WallIndices[ind++] = (i+1)*MESH_SIZE + (j+0);
            g_WallIndices[ind++] = (i+0)*MESH_SIZE + (j+1);
            g_WallIndices[ind++] = (i+1)*MESH_SIZE + (j+1);
        }
    }

    FLOAT dj = g_PI/(SPHERE_MESH_SIZE+1.f);
    FLOAT di = g_PI/SPHERE_MESH_SIZE;

    // Generate the sphere data, note the random texture coords and the inward
	// facing normals

    // vertices 0 and 1 are the north and south poles
    sphere[0] = D3DVERTEX( D3DVECTOR(0.0f, 1.0f, 0.0f), 
		                   D3DVECTOR(0.0f,-1.0f, 0.0f), rnd(), rnd() );
    sphere[1] = D3DVERTEX( D3DVECTOR(0.0f, -1.0f, 0.0f), 
		                   D3DVECTOR(0.0f, 1.0f, 0.0f), rnd(), rnd() );

    for( j=0; j<SPHERE_MESH_SIZE; j++ )
	{
        for( i=0; i<SPHERE_MESH_SIZE*2; i++ ) 
		{
            D3DVECTOR   p;

            p.y = (FLOAT)( cos((j+1) * dj) );
            p.x = (FLOAT)( sin(i * di) * sin((j+1) * dj) );
            p.z = (FLOAT)( cos(i * di) * sin((j+1) * dj) );
            sphere[2+i+j*SPHERE_MESH_SIZE*2] = D3DVERTEX(p, -p, rnd(), rnd());
        }
    }

    // Now generate the traingle indices. Strip around north pole first
    for( i=0; i<SPHERE_MESH_SIZE*2; i++ )
	{
        sphere_indices[3*i+0] = 0;
        sphere_indices[3*i+1] = i+2;
        sphere_indices[3*i+2] = i+3;
        if( i==SPHERE_MESH_SIZE*2-1 )
            sphere_indices[3*i+2] = 2;
    }

    // Now all the middle strips
    for( j=0; j<SPHERE_MESH_SIZE-1; j++ )
	{
        v = 2+j*SPHERE_MESH_SIZE*2;
        ind = 3*SPHERE_MESH_SIZE*2 + j*6*SPHERE_MESH_SIZE*2;
        for( i=0; i<SPHERE_MESH_SIZE*2; i++ )
		{
            sphere_indices[6*i+0+ind] = v+i;
            sphere_indices[6*i+2+ind] = v+i+1;
            sphere_indices[6*i+1+ind] = v+i+SPHERE_MESH_SIZE*2;

            sphere_indices[6*i+0+ind+3] = v+i+SPHERE_MESH_SIZE*2;
            sphere_indices[6*i+2+ind+3] = v+i+1;
            sphere_indices[6*i+1+ind+3] = v+i+SPHERE_MESH_SIZE*2+1;
            if( i==SPHERE_MESH_SIZE*2-1 )
			{
                sphere_indices[6*i+2+ind+0] = v+i+1-2*SPHERE_MESH_SIZE;
                sphere_indices[6*i+2+ind+3] = v+i+1-2*SPHERE_MESH_SIZE;
                sphere_indices[6*i+1+ind+3] = v+i+SPHERE_MESH_SIZE*2+1-2*SPHERE_MESH_SIZE;
            }
        }
    }

    // Finally strip around south pole
    v = SPHERE_VERTICES-SPHERE_MESH_SIZE*2;
    ind = SPHERE_INDICES-3*SPHERE_MESH_SIZE*2;
    for( i=0; i<SPHERE_MESH_SIZE*2; i++ )
	{
        sphere_indices[3*i+0+ind] = 1;
        sphere_indices[3*i+1+ind] = v+i+1;
        sphere_indices[3*i+2+ind] = v+i;
        if( i==SPHERE_MESH_SIZE*2-1 )
            sphere_indices[3*i+1+ind] = v;
    }

    // Create some textures
    D3DTextr_CreateTexture( "Banana.bmp" );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT App_FrameMove( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{
    static D3DLIGHTTYPE dltType = D3DLIGHT_POINT;
    static BOOL bJustSwitched = FALSE;
    
    if( (0==((DWORD)fTimeKey)%10) )
    {
        if( FALSE == bJustSwitched )
        {
            if( dltType == D3DLIGHT_POINT )
                dltType = D3DLIGHT_SPOT;
            else if( dltType == D3DLIGHT_SPOT )
                dltType = D3DLIGHT_DIRECTIONAL;
            else if( dltType == D3DLIGHT_DIRECTIONAL )
                dltType = D3DLIGHT_PARALLELPOINT;
            else if( dltType == D3DLIGHT_PARALLELPOINT )
                dltType = D3DLIGHT_POINT;
        }
        bJustSwitched = TRUE;
    }
    else 
        bJustSwitched = FALSE;

    // Mess with the lights
    FLOAT fParam1 = (FLOAT)sin(fTimeKey*2.000f);
    FLOAT fParam2 = (FLOAT)sin(fTimeKey*2.246f);
    FLOAT fParam3 = (FLOAT)sin(fTimeKey*2.640f);

    D3DLIGHT light;
    light.dwSize         = sizeof(D3DLIGHT);
    light.dltType        = dltType;
    light.dcvColor.r     = 0.5f+0.5f*fParam1;
    light.dcvColor.g     = 0.5f+0.5f*fParam2;
    light.dcvColor.b     = 0.5f+0.5f*fParam3;
    light.dvPosition     = 4.9f * D3DVECTOR( fParam1, fParam2, fParam3 );
    light.dvDirection    = D3DVECTOR( fParam1, fParam2, fParam3 );
    light.dvAttenuation0 = 0.0f;
    light.dvAttenuation1 = 0.0f;
    light.dvAttenuation2 = 0.0f;
    
    switch( dltType )
    {
        case D3DLIGHT_POINT:
            light.dvAttenuation0 = 1.0f;
            break;
        case D3DLIGHT_PARALLELPOINT:
            light.dvAttenuation0 = 1.0f;
            light.dvAttenuation1 = 1.0f;
            light.dvAttenuation2 = 1.0f;
            break;
        case D3DLIGHT_DIRECTIONAL:
            light.dvPosition     = D3DVECTOR(0.0f, 0.0f, 0.0f);
            break;
        case D3DLIGHT_SPOT:
            light.dvPosition     = D3DVECTOR(0.0f, 0.0f, 0.0f);
            light.dvRange        =   1.0f;
            light.dvFalloff      = 100.0f;
            light.dvTheta        =   0.8f;
            light.dvPhi          =   1.0f;
            light.dvAttenuation2 =   1.0f;
    }

    g_pLight->SetLight( &light );

    FLOAT     toc = 0.3f*fParam1 - g_PI/4;
    D3DVECTOR vFrom( (FLOAT)(sin(toc)*4), 3.0f, (FLOAT)(-cos(toc)*4) );
    D3DVECTOR vAt(0.0f, 0.0f, 0.0f);
    D3DVECTOR vUp(0.0f, 1.0f, 0.0f);
    
	D3DMATRIX matView;
    D3DUtil_SetViewMatrix( matView, vFrom, vAt, vUp );
    pd3dDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &matView );

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
    pvViewport->Clear2( 1UL, prcViewRect, D3DCLEAR_TARGET, 0x00000000, 
		                1.0f, 0L );

    // Begin the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        D3DMATRIX world, trans, scale, rotate;

        // Draw the bottom wall
        D3DUtil_SetTranslateMatrix(trans,-5,-5,-5);
        D3DUtil_SetScaleMatrix(scale,10,10,10);
        D3DMath_MatrixMultiply(world,trans,scale);
        pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &world );
        pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                          (VOID*)g_WallVertices, NUM_VERTICES,
                                          g_WallIndices, NUM_INDICES, 0 );

        // Draw the back wall
        D3DUtil_SetTranslateMatrix(trans, 5,-5,-5);
        D3DUtil_SetRotateZMatrix(rotate,g_PI/2);
        D3DMath_MatrixMultiply(world,trans,rotate);
        D3DMath_MatrixMultiply(world,world,scale);
        pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &world);
        pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                          (VOID*)g_WallVertices, NUM_VERTICES,
                                          g_WallIndices, NUM_INDICES, 0 );

        // Draw the side wall
        D3DUtil_SetTranslateMatrix(trans, -5,-5,5);
        D3DUtil_SetRotateXMatrix(rotate,  -g_PI/2);
        D3DMath_MatrixMultiply(world,trans,rotate);
        D3DMath_MatrixMultiply(world,world,scale);
        pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &world);
        pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                          (VOID*)g_WallVertices, NUM_VERTICES,
                                          g_WallIndices, NUM_INDICES, 0 );

        // Draw sphere at light's position
        D3DLIGHT light;
        light.dwSize = sizeof(D3DLIGHT);
        g_pLight->GetLight(&light);
        D3DUtil_SetTranslateMatrix( world, light.dvPosition.x,
                                    light.dvPosition.y, light.dvPosition.z );
        pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &world);
        pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX, 
                                          (VOID*)sphere, SPHERE_VERTICES,
                                          sphere_indices, SPHERE_INDICES, 0 );

        // Output the name of the light type
        switch( light.dltType )
        {
            case D3DLIGHT_POINT:
                AppOutputText( pd3dDevice, 0, 20, TEXT("Point Light") );
                break;
            case D3DLIGHT_SPOT:
                AppOutputText( pd3dDevice, 0, 20, TEXT("Spot Light") );
                break;
            case D3DLIGHT_PARALLELPOINT:
                AppOutputText( pd3dDevice, 0, 20, TEXT("Parallel Point Light") );
                break;
            case D3DLIGHT_DIRECTIONAL:
                AppOutputText( pd3dDevice, 0, 20, TEXT("Directional Light") );
                break;
        }
    }

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

	// Get a ptr to the ID3D object to create materials and/or lights. Note:
	// the Release() call just serves to decrease the ref count.
    LPDIRECT3D3 pD3D;
    if( FAILED( pd3dDevice->GetDirect3D( &pD3D ) ) )
        return E_FAIL;
    pD3D->Release();

    D3DMATERIAL       mtrl;
    D3DMATERIALHANDLE hmtrl;
    
    // Create and set up the object material
    if( FAILED( pD3D->CreateMaterial( &g_pmtrlObjectMtrl, NULL ) ) )
        return E_FAIL;

    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    mtrl.power = 40.0f;
    g_pmtrlObjectMtrl->SetMaterial( &mtrl );
    g_pmtrlObjectMtrl->GetHandle( pd3dDevice, &hmtrl );
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_MATERIAL, hmtrl );
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_AMBIENT,  0x20202020 );

    // Set up a texture
    D3DTextr_RestoreAllTextures( pd3dDevice );
    pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("Banana.bmp") );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, TRUE );

    // Set the transform matrices
    D3DMATRIX matWorld, matProj;
    D3DUtil_SetIdentityMatrix( matWorld );
    D3DUtil_SetProjectionMatrix( matProj, 1.57f, 1.0f, 1.0f, 100.0f );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

    // Set up the light
    if( FAILED( pD3D->CreateLight( &g_pLight, NULL ) ) )
        return E_FAIL;

    pvViewport->AddLight( g_pLight );

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

    SAFE_RELEASE( g_pLight );
    SAFE_RELEASE( g_pmtrlObjectMtrl );
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





