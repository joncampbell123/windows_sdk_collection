//-----------------------------------------------------------------------------
// File: Fog.cpp
//
// Desc: Example code showing how to do fog in D3D
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
TCHAR* g_strAppTitle       = TEXT( "Fog: D3D Fog Sample" );
BOOL   g_bAppUseZBuffer    = TRUE;
BOOL   g_bAppUseBackBuffer = TRUE;




//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
VOID    AppPause( BOOL );
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
FLOAT   PerlinTurbulence1D( D3DVECTOR & point, int terms );
VOID    InitPerlinNoise( int seed );

LPDIRECT3DMATERIAL3 g_pmtrlObjectMtrl     = NULL;
LPDIRECT3DLIGHT     g_pLight              = NULL;

#define NUM_GRID			32
#define NUM_GRID_VERTICES	(NUM_GRID*NUM_GRID)
#define NUM_GRID_INDICES	((NUM_GRID-1)*(NUM_GRID-1)*2*3)
#define GRID_WIDTH			20.0f

D3DVERTEX g_Grid[NUM_GRID][NUM_GRID];
WORD      g_GridIndices[NUM_GRID-1][NUM_GRID-1][2][3];




//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{
	WORD i, j;

	for( i=0; i<NUM_GRID; i++ )
	{
		for( j=0; j<NUM_GRID; j++ )
		{
			FLOAT x = i*GRID_WIDTH/(NUM_GRID-1) - GRID_WIDTH/2;
			FLOAT z = j*GRID_WIDTH/(NUM_GRID-1) - GRID_WIDTH/2;
			g_Grid[i][j] = D3DVERTEX( D3DVECTOR(x,0,z), D3DVECTOR(0,1,0),
				                     j/(FLOAT)NUM_GRID, i/(FLOAT)NUM_GRID );
		}
	}

	for( i=0; i<NUM_GRID-1; i++ )
	{
		for( j=0; j<NUM_GRID-1; j++ )
		{
			g_GridIndices[i][j][0][0] = (i+0) + (j+0)*NUM_GRID;
			g_GridIndices[i][j][0][1] = (i+1) + (j+0)*NUM_GRID;
			g_GridIndices[i][j][0][2] = (i+1) + (j+1)*NUM_GRID;
			g_GridIndices[i][j][1][0] = (i+0) + (j+0)*NUM_GRID;
			g_GridIndices[i][j][1][1] = (i+1) + (j+1)*NUM_GRID;
			g_GridIndices[i][j][1][2] = (i+0) + (j+1)*NUM_GRID;
		}
	}

	// Force the Perlin noise to be randomly initilized
	D3DVECTOR pnt( 0, 0, 0 );

	PerlinTurbulence1D( pnt, 1 );  // call it once to force automatic init
	InitPerlinNoise( time(NULL) ); // reseed with a random number

    // Create some textures
    D3DTextr_CreateTexture( "Terrain.bmp" );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateTerrain()
// Desc: Updates the terrain grid and returns a new altitude for the viewpoint.
//-----------------------------------------------------------------------------
FLOAT UpdateTerrain( FLOAT fTimeKey )
{
	D3DVECTOR pnt;
	WORD	  i, j;
	FLOAT	  alt;
	FLOAT	  size   = GRID_WIDTH/(NUM_GRID-1.0f);
	FLOAT	  offset = GRID_WIDTH/2.0f;
	FLOAT     tic = fTimeKey;

	for( i=0;i<NUM_GRID; i++ )
	{
		for( j=1; j<NUM_GRID; j++ )
		{
			g_Grid[i][j-1].y  = g_Grid[i][j].y;
			g_Grid[i][j-1].nx = g_Grid[i][j].nx;
			g_Grid[i][j-1].ny = g_Grid[i][j].ny;
			g_Grid[i][j-1].nz = g_Grid[i][j].nz;
			g_Grid[i][j-1].tv = g_Grid[i][j].tv;
		}
		// add new info to last row
		j = NUM_GRID-1;
		pnt = D3DVECTOR(i/(float)NUM_GRID, tic, j/(float)NUM_GRID);
		pnt *= g_PI/2;

		alt = PerlinTurbulence1D(pnt, 6);
		if( alt < 0.0f )
			alt = 0.0f;
		else
			alt = (FLOAT)pow(alt*4.f, 2.0f);
		
		g_Grid[i][j].y  = alt;
		g_Grid[i][j].tv = alt*0.2f + 0.18f;

		if( g_Grid[i][j].tv < 0.0 )
			g_Grid[i][j].tv = 0.0f;
		else if( g_Grid[i][j].tv > 0.95f )
			g_Grid[i][j].tv = 0.95f;
		g_Grid[i][j].tv = 1.0f - g_Grid[i][j].tv;
	}

	// Calculate normal for next to last row
	j = NUM_GRID-2;
	for( i=1; i<NUM_GRID-1; i++ )
	{
		D3DVECTOR pnt1 = D3DVECTOR( g_Grid[i-1][j].y, 1.0f, g_Grid[i][j-1].y );
		D3DVECTOR pnt2 = D3DVECTOR( g_Grid[i][j-1].y, 0.0f, g_Grid[i][j+1].y );
		pnt = Normalize(pnt1-pnt2);
		g_Grid[i][j].nx = pnt.x;
		g_Grid[i][j].ny = pnt.y;
		g_Grid[i][j].nz = pnt.z;
	}

	// Return the altitude for the camera
	return max( g_Grid[NUM_GRID/2][NUM_GRID/4].y, g_Grid[NUM_GRID/2][0].y);
}




//-----------------------------------------------------------------------------
// Name: App_FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT App_FrameMove( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{
	static FLOAT alt = 10.0f;	
	static FLOAT dy  = 0.0f;	

	// Update viewpoint altitude
	FLOAT new_alt = (9.0f * alt + UpdateTerrain(fTimeKey) )*0.1f;
	FLOAT new_dy  = new_alt - alt;
	dy  = (9.0f * dy + new_dy) *0.1f;
	alt = new_alt;

	// Update viewpoint 
	D3DVECTOR from = D3DVECTOR(0.0f, alt + 2.0f, -10.0f);
	D3DVECTOR at   = D3DVECTOR(0.0f, alt + 2.0f + dy, -9.0f);
	D3DVECTOR up   = D3DVECTOR(0.0f, 1.0f, 0.0f);
	D3DMATRIX matView;
	D3DUtil_SetViewMatrix( matView, from, at, up);
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
    pvViewport->Clear2( 1UL, prcViewRect, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
		                0x00b5b5ff, 1.0f, 0L );

    // Begin the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
		pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
			           (VOID*)g_Grid, NUM_GRID_VERTICES,
					   (WORD*)g_GridIndices, NUM_GRID_INDICES, 0 );

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
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_AMBIENT,  0x00000000 );

    // Set up a texture
    D3DTextr_RestoreAllTextures( pd3dDevice );
    pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("Terrain.bmp") );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );

    // Set the transform matrices
    D3DMATRIX matWorld, matProj;
    D3DUtil_SetIdentityMatrix( matWorld );
    D3DUtil_SetProjectionMatrix( matProj, g_PI/3.5f, 1.0f, 1.0f, 25.0f );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

    // Set up the light
    if( FAILED( pD3D->CreateLight( &g_pLight, NULL ) ) )
        return E_FAIL;
	D3DLIGHT light;
	D3DUtil_InitLight( light, D3DLIGHT_POINT, 0.0f, 10.0f, -10.0f );
	light.dvAttenuation0 =  1.0f;
	light.dvRange        = 30.0f;
	g_pLight->SetLight( &light );
    pvViewport->AddLight( g_pLight );

	FLOAT fFogEnd = 8.0f;

	pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE,      1 );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );
	// turn on fog
	pd3dDevice->SetRenderState( D3DRENDERSTATE_FOGENABLE,    TRUE );
	pd3dDevice->SetLightState(  D3DLIGHTSTATE_FOGMODE,       D3DFOG_LINEAR );
	pd3dDevice->SetLightState(  D3DLIGHTSTATE_FOGEND,        *((DWORD *)(&fFogEnd)) );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_FOGCOLOR,     0x00b5b5ff );

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
    
	if( pd3dDeviceDesc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_FOGVERTEX )
		return S_OK;

	return E_FAIL;
}





