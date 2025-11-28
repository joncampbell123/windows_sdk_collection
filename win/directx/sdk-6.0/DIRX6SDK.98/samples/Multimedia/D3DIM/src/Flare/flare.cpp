//-----------------------------------------------------------------------------
// File: Flare.cpp
//
// Desc: Example code showing how to simulate lens flares using the
//       blending features of the 3D rasterizer. The lens flare images
//       are loaded from .BMP files into D3D textures.
//
//       Note: This code uses the D3D Framework helper library.
//
//
// Copyright (c) 1996-1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define D3D_OVERLOADS
#define STRICT
#include <math.h>
#include <stdio.h>
#include "D3DTextr.h"
#include "D3DUtil.h"


//-----------------------------------------------------------------------------
// Declare the application globals for use in WinMain.cpp
//-----------------------------------------------------------------------------
TCHAR* g_strAppTitle       = TEXT( "Flare: Lens Flare Demo" );
BOOL   g_bAppUseZBuffer    = FALSE;
BOOL   g_bAppUseBackBuffer = TRUE;




//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
VOID    AppPause( BOOL );
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
HRESULT RenderFlares( LPDIRECT3DDEVICE3 );




//-----------------------------------------------------------------------------
// Name: Flare
// Desc: Data structure for the lens flares
//-----------------------------------------------------------------------------
struct Flare 
{
	INT                wType;       // 0..5, matches flare material indices
	FLOAT              fLoc;        // postion on axis
	FLOAT              fScale;      // relative size
	FLOAT              r,g,b;       // color
	D3DVECTOR          vPositionPt; // 3D position for rendering
	FLOAT              fRenderSize; // size for rendering the flare
};

#define     NUM_FLARES 12        // Number of lens flares
Flare       g_Flare[NUM_FLARES]; // The actual flares
D3DVERTEX   g_Mesh[4];           // Vertices used to render flares
D3DTLVERTEX g_Background[4];     // Vertices used to render the backdrop

LPDIRECT3DTEXTURE2  g_ptexShineTextures[10];
LPDIRECT3DTEXTURE2  g_ptexFlareTextures[5];
LPDIRECT3DMATERIAL3	g_pmtrlRenderMtrl = NULL;




//-----------------------------------------------------------------------------
// Name: SetFlare()
// Desc: Helper function to initialize a flare
//-----------------------------------------------------------------------------
Flare SetFlare( INT wType, FLOAT fLocation, FLOAT fScale, FLOAT fRed,
			    FLOAT fGreen, FLOAT fBlue )
{
	Flare ret;
	ret.wType  = wType;
	ret.fLoc   = fLocation;
	ret.fScale = fScale;
	ret.r      = fRed;
	ret.g      = fGreen;
	ret.b      = fBlue;
	return ret;
}




//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{
	// Initialize the array of lens flares
	//                   type   loc   scale  red  green  blue
	g_Flare[0]  = SetFlare( -1,  1.00f, 0.45f, 0.0f, 0.0f, 1.0f );
	g_Flare[1]  = SetFlare( -1,  1.00f, 0.30f, 0.0f, 1.0f, 0.0f );
	g_Flare[2]  = SetFlare( -1,  1.00f, 0.47f, 1.0f, 0.0f, 0.0f );
	g_Flare[3]  = SetFlare(  2,  1.30f, 0.06f, 0.6f, 0.0f, 0.0f );
	g_Flare[4]  = SetFlare(  3,  1.00f, 0.15f, 0.4f, 0.0f, 0.0f );
	g_Flare[5]  = SetFlare(  1,  0.50f, 0.30f, 0.3f, 0.0f, 0.0f );
	g_Flare[6]  = SetFlare(  3,  0.20f, 0.07f, 0.3f, 0.0f, 0.0f );
	g_Flare[7]  = SetFlare(  0,  0.00f, 0.06f, 0.3f, 0.0f, 0.0f );
	g_Flare[8]  = SetFlare(  4, -0.25f, 0.11f, 0.5f, 0.0f, 0.0f );
	g_Flare[9]  = SetFlare(  4, -0.40f, 0.03f, 0.6f, 0.0f, 0.0f );
	g_Flare[10] = SetFlare(  4, -0.60f, 0.06f, 0.4f, 0.0f, 0.0f );
	g_Flare[11] = SetFlare(  4, -1.00f, 0.04f, 0.2f, 0.0f, 0.0f );

	// Initializes vertices used to render the flares
	D3DVECTOR vNorm = D3DVECTOR( 0.0f, 0.0f, 1.0f );
	g_Mesh[0] = D3DVERTEX( D3DVECTOR(-1.0f,-1.0f, 0.0f ), vNorm, 0.0f, 1.0f );
	g_Mesh[1] = D3DVERTEX( D3DVECTOR(-1.0f, 1.0f, 0.0f ), vNorm, 0.0f, 0.0f );
	g_Mesh[2] = D3DVERTEX( D3DVECTOR( 1.0f,-1.0f, 0.0f ), vNorm, 1.0f, 1.0f );
	g_Mesh[3] = D3DVERTEX( D3DVECTOR( 1.0f, 1.0f, 0.0f ), vNorm, 1.0f, 0.0f );

	// Initializes vertices used to render the background
    g_Background[0] = D3DTLVERTEX( D3DVECTOR( 0, 0, 0.5 ), 0.5, -1, 0, 0, 1 );
    g_Background[1] = D3DTLVERTEX( D3DVECTOR( 0, 0, 0.5 ), 0.5, -1, 0, 0, 0 );
    g_Background[2] = D3DTLVERTEX( D3DVECTOR( 0, 0, 0.5 ), 0.5, -1, 0, 1, 1 );
    g_Background[3] = D3DTLVERTEX( D3DVECTOR( 0, 0, 0.5 ), 0.5, -1, 0, 1, 0 );

	// Load in lens flares' images as textures
	D3DTextr_CreateTexture( "dx5_logo.bmp" );
	D3DTextr_CreateTexture( "shine0.bmp" );
	D3DTextr_CreateTexture( "shine1.bmp" );
	D3DTextr_CreateTexture( "shine2.bmp" );
	D3DTextr_CreateTexture( "shine3.bmp" );
	D3DTextr_CreateTexture( "shine4.bmp" );
	D3DTextr_CreateTexture( "shine5.bmp" );
	D3DTextr_CreateTexture( "shine6.bmp" );
	D3DTextr_CreateTexture( "shine7.bmp" );
	D3DTextr_CreateTexture( "shine8.bmp" );
	D3DTextr_CreateTexture( "shine9.bmp" );
	D3DTextr_CreateTexture( "flare0.bmp" );
	D3DTextr_CreateTexture( "flare1.bmp" );
	D3DTextr_CreateTexture( "flare2.bmp" );
	D3DTextr_CreateTexture( "flare3.bmp" );
	D3DTextr_CreateTexture( "flare4.bmp" );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT App_FrameMove( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{
	// Mess with the light position
	D3DVECTOR vLightPt;
	vLightPt.x = (FLOAT)( sin(fTimeKey*0.73)*12 );
	vLightPt.y = (FLOAT)( 5.0f+10.0f*sin(fTimeKey*0.678) );
	vLightPt.z = (FLOAT)( sin(fTimeKey*0.895)*12 );

	// Compute the vectors between the from, lookat, and light positions.	
	D3DVECTOR vLookatPt = D3DVECTOR( 0, 0, 0 );
	D3DVECTOR vFromPt   = D3DVECTOR( 0, 0, -20 );
	D3DVECTOR vViewVec  = Normalize( vLookatPt - vFromPt );
	D3DVECTOR vLightVec = Normalize( vLightPt - vFromPt );

	// Compute the vector and center point for the lens flare axis
	FLOAT     fDotProduct = DotProduct( vLightVec, vViewVec );
	D3DVECTOR vNewLightPt = vFromPt + 1.0f/fDotProduct * vLightVec * 1.01f;
	D3DVECTOR vCenterPt   = vFromPt + vViewVec*1.01f;
	D3DVECTOR vAxisVec    = vNewLightPt - vCenterPt;

	// Compute the offset of the lens flare along the flare axis
	D3DVECTOR dx         = Normalize( vAxisVec );
	FLOAT     fDeltaX    = ( dx.x - dx.y );
	FLOAT     fDeltaY    = ( dx.y + dx.x );
	FLOAT     fViewScale = (FLOAT)sqrt( fDeltaX*fDeltaX + fDeltaY*fDeltaY );

	// Store the lens flares positions for each flare
	for( DWORD i=0; i<NUM_FLARES; i++ )
	{
		// Store the position of the flare along the axis
		g_Flare[i].vPositionPt = vCenterPt + vAxisVec * g_Flare[i].fLoc;

		// Store the render size of the flare. This is the lens flare size
		// corrected for the orientation of the flaring axis.
		g_Flare[i].fRenderSize = fViewScale * g_Flare[i].fScale;
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
				    LPDIRECT3DVIEWPORT3 pvViewport, D3DRECT* prcViewportRect )
{
	// Begin the scene
	if( SUCCEEDED( pd3dDevice->BeginScene() ) )
	{
        // Draw the background
        pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("dx5_logo.bmp") );
        pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX,
                                   g_Background, 4, 0 );

		// Render the flares
		RenderFlares( pd3dDevice );

		// End the scene.
		pd3dDevice->EndScene();
	}

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderFlares()
// Desc: Draws the set of flares
//-----------------------------------------------------------------------------
HRESULT RenderFlares( LPDIRECT3DDEVICE3 pd3dDevice )
{
	D3DMATERIAL mtrl;
	D3DUtil_InitMaterial( mtrl );

	// Turn on alpha blending for the flares
	pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );

	// Do the flares
	for( DWORD i=0; i<NUM_FLARES; i++ )
	{
		// Set up the emissive color of the flare
		mtrl.emissive.r = g_Flare[i].r;
		mtrl.emissive.g = g_Flare[i].g;
		mtrl.emissive.b = g_Flare[i].b;
		g_pmtrlRenderMtrl->SetMaterial( &mtrl );

		if( g_Flare[i].wType < 0 )
		{
			static dwShineTic = 0;
			if( ++dwShineTic > 9 )
				dwShineTic = 0;

			pd3dDevice->SetTexture( 0, g_ptexShineTextures[dwShineTic] );
		} 
		else 
		{
			pd3dDevice->SetTexture( 0, g_ptexFlareTextures[g_Flare[i].wType] );
		}

		// Translate the world matrix to the flare position
		D3DMATRIX matWorld;
		D3DUtil_SetTranslateMatrix( matWorld, g_Flare[i].vPositionPt );

		// Scale the world matrix to the flare size.
		matWorld._11 = g_Flare[i].fRenderSize;
		matWorld._22 = g_Flare[i].fRenderSize;
		matWorld._33 = 1;
		
		// Set the new world transform and render the flare
		pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX,
			                       g_Mesh, 4, 0 );
	}

	// Restore the material and states
	mtrl.emissive.r = mtrl.emissive.g = mtrl.emissive.b = 1.0f;
    g_pmtrlRenderMtrl->SetMaterial( &mtrl );

    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_InitDeviceObjects()
// Desc: Initialize scene objects
//-----------------------------------------------------------------------------
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3 pd3dDevice,
							   LPDIRECT3DVIEWPORT3 pvViewport )
{
	// Check parameters
	if( NULL==pd3dDevice || NULL==pvViewport )
        return E_INVALIDARG;

	D3DTextr_RestoreAllTextures( pd3dDevice );

	// Set up the dimensions for the background image
	D3DVIEWPORT2 vp;
	vp.dwSize = sizeof(vp);
	pvViewport->GetViewport2(&vp);
    g_Background[0].sy = (FLOAT)vp.dwHeight;
    g_Background[2].sy = (FLOAT)vp.dwHeight;
    g_Background[2].sx = (FLOAT)vp.dwWidth;
    g_Background[3].sx = (FLOAT)vp.dwWidth;
    
	// Get a ptr to the ID3D object to create materials and/or lights. Note:
	// the Release() call just serves to decrease the ref count.
	LPDIRECT3D3 pD3D;
	if( FAILED( pd3dDevice->GetDirect3D( &pD3D ) ) )
		return E_FAIL;
	pD3D->Release();

	//Create the render material
    if( FAILED( pD3D->CreateMaterial( &g_pmtrlRenderMtrl, NULL) ) )
		return E_FAIL;

	D3DMATERIAL       mtrl;
	D3DMATERIALHANDLE hmtrl;
	D3DUtil_InitMaterial( mtrl );
    g_pmtrlRenderMtrl->SetMaterial( &mtrl );
	g_pmtrlRenderMtrl->GetHandle( pd3dDevice, &hmtrl );
	pd3dDevice->SetLightState( D3DLIGHTSTATE_MATERIAL, hmtrl );

	CHAR strTexName[40];

	for( WORD i=0; i<10; i++ )
	{
		sprintf( strTexName, "shine%d.bmp", i );
		g_ptexShineTextures[i] = D3DTextr_GetTexture( strTexName );
	}
	for( i=0; i<5; i++ )
	{
		sprintf( strTexName, "flare%d.bmp", i );
		g_ptexFlareTextures[i] = D3DTextr_GetTexture( strTexName );
	}
		
	// Set the transform matrices
	D3DVECTOR vEyePt    = D3DVECTOR( 0.0f, 0.0f, -20.0f );
	D3DVECTOR vLookatPt = D3DVECTOR( 0.0f, 0.0f,   0.0f );
	D3DVECTOR vUpVec    = D3DVECTOR( 0.0f, 0.0f,   1.0f );
	D3DMATRIX matView, matProj;

	D3DUtil_SetViewMatrix( matView, vEyePt, vLookatPt, vUpVec );
	D3DUtil_SetProjectionMatrix( matProj, 1.57f, 1.0f, 1.0f, 100.0f );
	pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW,       &matView );
	pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

	// Set any appropiate state
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_AMBIENT,     0xffffffff );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,   D3DBLEND_ONE );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,  D3DBLEND_ONE );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );

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

	SAFE_RELEASE( g_pmtrlRenderMtrl );
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
	// Get triangle caps (Hardware or software) and check for alpha blending
	LPD3DPRIMCAPS pdpc = &pd3dDeviceDesc->dpcTriCaps;

	if( 0 == ( pdpc->dwSrcBlendCaps & pdpc->dwDestBlendCaps & D3DBLEND_ONE ) )
		return E_FAIL;

	return S_OK;
}






