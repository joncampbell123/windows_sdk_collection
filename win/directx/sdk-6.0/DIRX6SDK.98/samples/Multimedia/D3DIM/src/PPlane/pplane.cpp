//-----------------------------------------------------------------------------
// File: PPlane.cpp
//
// Desc: Direct3D sample of paper planes flying around. This sample 
//       demonstrates DrawPrimitives() calls introducted with DX5.
//
//       Note: This code uses the D3D Sample Framework.
//
//
// Copyright (c) 1995-1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define  D3D_OVERLOADS
#define  STRICT
#include <windows.h>
#include <math.h>
#include <time.h>
#include "D3DUtil.h"
#include "D3DMath.h"


//-----------------------------------------------------------------------------
// Declare the application globals for use in WinMain.cpp
//-----------------------------------------------------------------------------
TCHAR* g_strAppTitle       = TEXT( "PPlane" );
BOOL   g_bAppUseZBuffer    = TRUE;
BOOL   g_bAppUseBackBuffer = TRUE;




//-----------------------------------------------------------------------------
// Function prototypes
//-----------------------------------------------------------------------------
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
#define rnd()  (((FLOAT)rand() ) / RAND_MAX)




//-----------------------------------------------------------------------------
// Name: Plane
// Desc: State structure for a paper plane
//-----------------------------------------------------------------------------
struct Plane
{
	D3DMATRIX matLocal;   // Matrix for rendering
	D3DVECTOR vLoc;        // Current location
	D3DVECTOR vGoal;       // Current goal
	D3DVECTOR vDelta;      // Current direction
	FLOAT     fYaw, fPitch, fRoll;
	FLOAT     fDYaw;
};




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define NUM_PLANES         42
#define NUM_PLANE_VERTICES 36
#define NUM_PLANE_INDICES  60
#define NUM_GRID           22
#define GRID_WIDTH         800.0f

Plane	            g_Planes[NUM_PLANES];
D3DVERTEX	        g_avVertices[NUM_PLANE_VERTICES];
WORD		        g_awIndices[NUM_PLANE_INDICES];
D3DVERTEX	        g_avGridVertices[NUM_GRID*NUM_GRID];
LPDIRECT3DMATERIAL3 g_pmtrlMaterial;
LPDIRECT3DLIGHT	    g_pLight;




//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Initialize scene objects (vertices for the paper plane) and then
//       calls InitViewport to initialize the viewport (mtrls, lights, etc.)
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{
	// Define the coordinates and normals for the planes 
	D3DVECTOR  p1( 0.125, 0.03125, 3.5 ),   p2( -0.125, 0.03125, 3.5 );
	D3DVECTOR  p3( 0.75,  0.1875,  3.25 ),  p4( -0.75,  0.1875,  3.25 );
	D3DVECTOR  p5( 2.75,  0.6875, -2.0 ),   p6( -2.75,  0.6875, -2.0 );
	D3DVECTOR  p7( 2.0,   0.5,    -4.25 ),  p8( -2.0,   0.5,    -4.25 );
	D3DVECTOR  p9( 0.5,   0.125,  -3.5 ),  p10( -0.5,   0.125,  -3.5 );
	D3DVECTOR p11( 0.0,  -2.25,   -2.75 ), p12(  0.0,  -0.5,     3.625 );

	D3DVECTOR n1 = Normalize( D3DVECTOR( -0.25f,  1.0f, 0.0f ) );
	D3DVECTOR n2 = Normalize( D3DVECTOR(  0.25f,  1.0f, 0.0f ) );
	D3DVECTOR n3 = Normalize( D3DVECTOR( -1.00f,  0.1f, 0.0f ) );
    D3DVECTOR n4 = Normalize( D3DVECTOR(  1.00f,  0.1f, 0.0f ) );

	g_avVertices[ 0] = D3DVERTEX(  p1,  n1, 0.0f, 0.0f ); // Right wing top
	g_avVertices[ 1] = D3DVERTEX(  p3,  n1, 0.0f, 0.0f );
	g_avVertices[ 2] = D3DVERTEX(  p5,  n1, 0.0f, 0.0f );
	g_avVertices[ 3] = D3DVERTEX(  p7,  n1, 0.0f, 0.0f );
	g_avVertices[ 4] = D3DVERTEX(  p9,  n1, 0.0f, 0.0f );

	g_avVertices[ 5] = D3DVERTEX(  p1, -n1, 0.0f, 0.0f ); // Right wing bottom
	g_avVertices[ 6] = D3DVERTEX(  p3, -n1, 0.0f, 0.0f );
	g_avVertices[ 7] = D3DVERTEX(  p5, -n1, 0.0f, 0.0f );
	g_avVertices[ 8] = D3DVERTEX(  p7, -n1, 0.0f, 0.0f );
	g_avVertices[ 9] = D3DVERTEX(  p9, -n1, 0.0f, 0.0f );

	g_avVertices[10] = D3DVERTEX(  p2, -n2, 0.0f, 0.0f ); // Left wing bottom
	g_avVertices[11] = D3DVERTEX(  p4, -n2, 0.0f, 0.0f );
	g_avVertices[12] = D3DVERTEX(  p6, -n2, 0.0f, 0.0f );
	g_avVertices[13] = D3DVERTEX(  p8, -n2, 0.0f, 0.0f );
	g_avVertices[14] = D3DVERTEX( p10, -n2, 0.0f, 0.0f );

	g_avVertices[15] = D3DVERTEX(  p2,  n2, 0.0f, 0.0f ); // Left wing top
	g_avVertices[16] = D3DVERTEX(  p4,  n2, 0.0f, 0.0f );
	g_avVertices[17] = D3DVERTEX(  p6,  n2, 0.0f, 0.0f );
	g_avVertices[18] = D3DVERTEX(  p8,  n2, 0.0f, 0.0f );
	g_avVertices[19] = D3DVERTEX( p10,  n2, 0.0f, 0.0f );

	g_avVertices[20] = D3DVERTEX(  p1, -n3, 0.0f, 0.0f ); // Right body outside
	g_avVertices[21] = D3DVERTEX(  p9, -n3, 0.0f, 0.0f );
	g_avVertices[22] = D3DVERTEX( p11, -n3, 0.0f, 0.0f );
	g_avVertices[23] = D3DVERTEX( p12, -n3, 0.0f, 0.0f );

	g_avVertices[24] = D3DVERTEX(  p1,  n3, 0.0f, 0.0f ); // Right body inside
	g_avVertices[25] = D3DVERTEX(  p9,  n3, 0.0f, 0.0f );
	g_avVertices[26] = D3DVERTEX( p11,  n3, 0.0f, 0.0f );
	g_avVertices[27] = D3DVERTEX( p12,  n3, 0.0f, 0.0f );
	
	g_avVertices[28] = D3DVERTEX(  p2, -n4, 0.0f, 0.0f ); // Left body outside
	g_avVertices[29] = D3DVERTEX( p10, -n4, 0.0f, 0.0f );
	g_avVertices[30] = D3DVERTEX( p11, -n4, 0.0f, 0.0f );
	g_avVertices[31] = D3DVERTEX( p12, -n4, 0.0f, 0.0f );

	g_avVertices[32] = D3DVERTEX(  p2,  n4, 0.0f, 0.0f ); // Left body inside
	g_avVertices[33] = D3DVERTEX( p10,  n4, 0.0f, 0.0f );
	g_avVertices[34] = D3DVERTEX( p11,  n4, 0.0f, 0.0f );
	g_avVertices[35] = D3DVERTEX( p12,  n4, 0.0f, 0.0f );

	// Indices for the right wing top
	g_awIndices[ 0] = 0; g_awIndices[ 3] = 1; g_awIndices[ 6] = 4;
	g_awIndices[ 1] = 1; g_awIndices[ 4] = 2; g_awIndices[ 7] = 2;
	g_awIndices[ 2] = 4; g_awIndices[ 5] = 4; g_awIndices[ 8] = 3;

	// Indices for the right wing bottom
	g_awIndices[ 9] = 5; g_awIndices[12] = 6; g_awIndices[15] = 7;
	g_awIndices[10] = 9; g_awIndices[13] = 9; g_awIndices[16] = 9;
	g_awIndices[11] = 6; g_awIndices[14] = 7; g_awIndices[17] = 8;

	// Indices for the left wing top
	g_awIndices[18] = 10; g_awIndices[21] = 11; g_awIndices[24] = 14;
	g_awIndices[19] = 11; g_awIndices[22] = 12; g_awIndices[25] = 12;
	g_awIndices[20] = 14; g_awIndices[23] = 14; g_awIndices[26] = 13;

	// Indices for the left wing bottom
	g_awIndices[27] = 15; g_awIndices[30] = 16; g_awIndices[33] = 17;
	g_awIndices[28] = 19; g_awIndices[31] = 19; g_awIndices[34] = 19;
	g_awIndices[29] = 16; g_awIndices[32] = 17; g_awIndices[35] = 18;

	// Indices for the right body outside
	g_awIndices[36] = 20; g_awIndices[38] = 21; g_awIndices[40] = 23;
	g_awIndices[37] = 23; g_awIndices[39] = 21; g_awIndices[41] = 22;

	// Indices for the right body inside
	g_awIndices[42] = 24; g_awIndices[44] = 27; g_awIndices[46] = 26;
	g_awIndices[43] = 25; g_awIndices[45] = 25; g_awIndices[47] = 27;

	// Indices for the left body outside
	g_awIndices[48] = 28; g_awIndices[50] = 31; g_awIndices[52] = 30;
	g_awIndices[49] = 29; g_awIndices[51] = 29; g_awIndices[53] = 31;

	// Indices for the left body inside
	g_awIndices[54] = 32; g_awIndices[56] = 33; g_awIndices[58] = 35;
	g_awIndices[55] = 35; g_awIndices[57] = 33; g_awIndices[59] = 34;

	// Seed the random number generator
	srand( time(0) );

	// Set up initial directions (goals) for the planes
	for( int i=0; i<NUM_PLANES; i++ ) 
	{
		ZeroMemory( &g_Planes[i], sizeof(Plane) );

		g_Planes[i].vGoal.x  = 10.0f*(rnd()-rnd());
		g_Planes[i].vGoal.y  = 10.0f*(rnd()-rnd());
		g_Planes[i].vGoal.z  = 10.0f*(rnd()-rnd());
		g_Planes[i].vDelta.z = 1.0f;
	}

	// Create the grid vertices
	FLOAT fSize   = GRID_WIDTH/(NUM_GRID-1.0f);
	FLOAT fOffset = GRID_WIDTH/2.0f;
	for( i=0; i<NUM_GRID; i++ ) 
	{
		for( int j=0; j<NUM_GRID; j++ ) 
		{
			D3DVECTOR p( i*fSize-fOffset, 0.0f, j*fSize-fOffset );
			D3DVECTOR n( 0.0f, 1.0f, 0.0f );
			g_avGridVertices[j+i*NUM_GRID] = D3DVERTEX( p, n, 0.0f, 0.0f );
		}
	}

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_InitDeviceObjects()
// Desc: Initializes device dependant objects
//-----------------------------------------------------------------------------
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3 pd3dDevice, 
					           LPDIRECT3DVIEWPORT3 pvViewport )
{
	if( NULL==pd3dDevice || NULL==pvViewport )
        return E_INVALIDARG;
   
	HRESULT           hr;
	LPDIRECT3D3	      pD3D;
	D3DMATERIALHANDLE hmtrlMaterial;
	D3DMATERIAL       mtrl;
	D3DLIGHT          light;

	// Get a ptr to the ID3D object to create materials and/or lights. Note:
	// the Release() call just serves to decrease the ref count.
	if( FAILED( pd3dDevice->GetDirect3D( &pD3D ) ) )
        return E_FAIL;
	pD3D->Release();

	// Create and set up the plane material
    if( FAILED( hr = pD3D->CreateMaterial( &g_pmtrlMaterial, NULL ) ) )
		return E_FAIL;
	D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
	mtrl.dwRampSize = 16;
	g_pmtrlMaterial->SetMaterial( &mtrl );
	g_pmtrlMaterial->GetHandle( pd3dDevice, &hmtrlMaterial );

	// Set up the light
    if( FAILED( hr = pD3D->CreateLight( &g_pLight, NULL ) ) )
		return E_FAIL;
	D3DUtil_InitLight( light, D3DLIGHT_POINT, 0.0f, 1000.f, -100.0f );
	light.dvAttenuation0 = 1.0f;
	g_pLight->SetLight( &light );
	pvViewport->AddLight( g_pLight );
	
	// Set up the projection matrix
	D3DMATRIX matProj;
	D3DUtil_SetProjectionMatrix( matProj, g_PI/2, 1.0f, 10.0f, 500.0f );
	pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

	// Set the render state
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_MATERIAL, hmtrlMaterial );
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_AMBIENT,  0x50505050 );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, 1);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_FrameMove()
// Desc: Updates scene
//-----------------------------------------------------------------------------
HRESULT App_FrameMove( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{
	// Update position and orientation of the planes
	static FLOAT fSpeed      = 2.0f;
	static FLOAT fAngleTweak = 0.02f;

	for( WORD i=0; i<NUM_PLANES; i++ ) 
	{
		// Tweak orientation based on last position and goal
		D3DVECTOR vOffset = g_Planes[i].vGoal - g_Planes[i].vLoc;

		// First, tweak the pitch
		if( vOffset.y > 1.0)              // We're too low
		{			
			g_Planes[i].fPitch += fAngleTweak;
			if( g_Planes[i].fPitch > 0.8f )
				g_Planes[i].fPitch = 0.8f;
		}
		else if( vOffset.y < -1.0)       // We're too high
		{
			g_Planes[i].fPitch -= fAngleTweak;
			if( g_Planes[i].fPitch < -0.8f )
				g_Planes[i].fPitch = -0.8f;
		}
		else                             // Add damping
			g_Planes[i].fPitch *= 0.95f;

		// Now figure out yaw changes
		vOffset.y            = 0.0f;
		g_Planes[i].vDelta.y = 0.0f;
		
		g_Planes[i].vDelta = Normalize( g_Planes[i].vDelta );
		vOffset            = Normalize( vOffset );

		FLOAT fDot = DotProduct( g_Planes[i].vDelta, vOffset );
		fDot = (1.0f-fDot)/2.0f * fAngleTweak * 10.0f;

		vOffset = CrossProduct( vOffset, g_Planes[i].vDelta );

		if( vOffset.y > 0.01f )
			g_Planes[i].fDYaw = ( g_Planes[i].fDYaw * 9.0f + fDot ) * 0.1f;
		else if( vOffset.y < 0.01f )
			g_Planes[i].fDYaw = ( g_Planes[i].fDYaw * 9.0f - fDot ) * 0.1f;

		g_Planes[i].fYaw  +=  g_Planes[i].fDYaw;
		g_Planes[i].fRoll  = -g_Planes[i].fDYaw * 9.0f;

		// At random times, change the direction (goal) of the plane
		if( rnd() < 0.03 ) 
		{
			g_Planes[i].vGoal.x = 60.0f*(rnd()-rnd());
			g_Planes[i].vGoal.y = 60.0f*(rnd()-rnd());
			g_Planes[i].vGoal.z = 300.0f*rnd()-100.0f;
		}

		// Build the local matrix for the pplane
		D3DMATRIX matTemp, mx, my, mz;
		D3DMATRIX matOrientation, matTrans;
		D3DUtil_SetRotateXMatrix( mx, -g_Planes[i].fPitch );
		D3DUtil_SetRotateYMatrix( my, -g_Planes[i].fYaw );
		D3DUtil_SetRotateZMatrix( mz, -g_Planes[i].fRoll );
		D3DMath_MatrixMultiply( matTemp, my, mx);
		D3DMath_MatrixMultiply( matOrientation, matTemp, mz );
		
		// Get delta by grabbing the z axis out of the transform
		g_Planes[i].vDelta.x = matOrientation._31;
		g_Planes[i].vDelta.y = matOrientation._32;
		g_Planes[i].vDelta.z = matOrientation._33;

		// Update position
		g_Planes[i].vLoc += fSpeed * g_Planes[i].vDelta;

		// Before we draw the first plane, use it's position for the camera
		if( 0==i ) 
		{
			D3DVECTOR vUp   = D3DVECTOR(0.0f, 1.0f, 0.0f);
			D3DVECTOR vFrom = g_Planes[0].vLoc - 20.0f * g_Planes[0].vDelta;
			D3DVECTOR vAt   = g_Planes[0].vLoc + g_Planes[0].vDelta;
			vFrom.x += 6 * matOrientation._21;
			vFrom.y += 6 * matOrientation._22;
			vFrom.z += 6 * matOrientation._23;

			D3DMATRIX matView, matRoll;
			D3DUtil_SetViewMatrix( matView, vFrom, vAt, vUp );
			D3DUtil_SetRotateZMatrix( matRoll,  g_Planes[i].fRoll );
			D3DMath_MatrixMultiply( matView, matRoll, matView );

			pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &matView );
		}

		// First translate into place, then set orientation
		D3DUtil_SetTranslateMatrix( matTrans, g_Planes[i].vLoc );
		D3DMath_MatrixMultiply( g_Planes[i].matLocal, matTrans, matOrientation );
	}

	return S_OK;
}



  
//-----------------------------------------------------------------------------
// Name: App_Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
HRESULT App_Render( LPDIRECT3DDEVICE3 pd3dDevice,
				    LPDIRECT3DVIEWPORT3 pvViewport, D3DRECT* prcViewRect )
{
    // Clear both back and z-buffer.
    pvViewport->Clear2( 1UL, prcViewRect, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
		                0x000000ff, 1.0f, 0L );

	// Begin the scene
	if( SUCCEEDED( pd3dDevice->BeginScene() ) )
	{
		// Draw all the planes
		for( WORD i=0; i<NUM_PLANES; i++ ) 
		{
			// Apply the plane's local matrix and render the plane
			pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, 
									  &g_Planes[i].matLocal );

			// Render the plane
			pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
									(VOID*)g_avVertices, NUM_PLANE_VERTICES,
									g_awIndices, NUM_PLANE_INDICES, 0 );
		}

		// Draw the latitudal lines of the grid
		D3DMATRIX matWorld;
		D3DUtil_SetIdentityMatrix( matWorld );
		matWorld._42 = -60.0f;
		pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
		pd3dDevice->DrawPrimitive( D3DPT_LINELIST, D3DFVF_VERTEX, 
								   (VOID*)g_avGridVertices, NUM_GRID*NUM_GRID, 0 );

		// Draw the longitudal lines of the grid
		D3DMATRIX my;
		D3DUtil_SetRotateYMatrix( my, -(FLOAT)g_PI_DIV_2 );
		D3DMath_MatrixMultiply( matWorld, matWorld, my );
		pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
		pd3dDevice->DrawPrimitive( D3DPT_LINELIST, D3DFVF_VERTEX, 
								   (VOID*)g_avGridVertices, NUM_GRID*NUM_GRID, 0 );

		pd3dDevice->EndScene();
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
    SAFE_RELEASE( g_pLight );
    SAFE_RELEASE( g_pmtrlMaterial );
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





