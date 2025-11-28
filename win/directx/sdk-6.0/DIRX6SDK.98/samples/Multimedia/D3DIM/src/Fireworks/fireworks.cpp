//-----------------------------------------------------------------------------
// File: Fireworks.cpp
//
// Desc: Example code showing how to use particles to simulate a fireworks
//       explosion.
//
//       Note: This code uses the D3D Framework helper library.
//
//
// Copyright (c) 1997-1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define D3D_OVERLOADS
#define STRICT
#include <time.h>
#include <math.h>
#include <stdio.h>
#include "D3DTextr.h"
#include "D3DUtil.h"
#include "D3DMath.h"
#include "resource.h"


//-----------------------------------------------------------------------------
// Declare the application globals for use in WinMain.cpp
//-----------------------------------------------------------------------------
TCHAR* g_strAppTitle       = TEXT( "Fireworks: Using Particles" );
BOOL   g_bAppUseZBuffer    = FALSE;
BOOL   g_bAppUseBackBuffer = TRUE;




//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
VOID    AppPause( BOOL );
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
HRESULT RenderParticles( LPDIRECT3DDEVICE3 );

#define rnd() (((FLOAT)rand()-(FLOAT)rand())/(2L*RAND_MAX))
#define RND() (((FLOAT)rand())/RAND_MAX)




//-----------------------------------------------------------------------------
// Name: Particle
// Desc: Data structure for each particle
//-----------------------------------------------------------------------------
struct Particle
{
    D3DVECTOR vPosition;
    D3DVECTOR vLaunchVelocity;
    D3DVECTOR vInitialPosition;
    D3DVECTOR vInitialVelocity;
    FLOAT     r, g, b;
    FLOAT     fLifetime;
    FLOAT     fMaturity;
    WORD      wType;
    FLOAT     fSize;
};




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
LPDIRECT3DMATERIAL3 g_pmtrlRenderMtrl = NULL;

#define     NUM_PARTICLES 100

Particle    g_Particle[NUM_PARTICLES]; // Array of particles
D3DVERTEX   g_Mesh[4];                 // Vertices used to render particles
D3DTLVERTEX g_Background[4];           // Vertices used to render the backdrop
FLOAT       g_fStartTimeKey = 0.0f;    // Time reference for calculations



//-----------------------------------------------------------------------------
// Name: SetParticle()
// Desc: Helper function to initialize a particle
//-----------------------------------------------------------------------------
Particle SetParticle( WORD wType, FLOAT fLifeTime, D3DVECTOR vBasePosition,
                      D3DVECTOR vBaseVelocity )
{
    Particle ret;
	ret.vInitialVelocity  = 15*Normalize( D3DVECTOR( rnd(),rnd(),rnd() ) );
	ret.vInitialVelocity += vBaseVelocity + D3DVECTOR( rnd(),rnd(),rnd() );
   	ret.vInitialPosition  = vBasePosition;
	ret.vLaunchVelocity   = vBaseVelocity;
    ret.r         = 1.0f;
    ret.g         = 1.0f;
    ret.b         = 1.0f;
    ret.fLifetime = fLifeTime + fLifeTime*rnd()/2;
    ret.fMaturity = 0.0f;
    ret.fSize     = 0.2f;
    ret.wType     = wType;
    return ret;
}




//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{
    // Initialize the array of particles
    for( WORD i=0; i<NUM_PARTICLES; i++ )
        g_Particle[i] = SetParticle( (i%3), 4.0f, D3DVECTOR(0,0,0),
                                     D3DVECTOR( 0, 30, 0 ) );

    // Initializes vertices used to render the particles
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

    // Load in textures
    D3DTextr_CreateTexture( "lake.bmp" );
    D3DTextr_CreateTexture( "firework.bmp" );

    return S_OK;
}





//-----------------------------------------------------------------------------
// Name: App_FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT App_FrameMove( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{
    D3DVECTOR a0 = D3DVECTOR( 0.0, -9.8f, 0.0f );
    FLOAT     t  = fTimeKey - g_fStartTimeKey;
    FLOAT     k  = 1.8f;

    DWORD dwNumOldParticles = 0L;

    // Store the particles positions
	for( DWORD i=0; i<NUM_PARTICLES; i++ )
	{
		if( t < 0.0f ) // Particle is in "launch" mode
		{
			D3DVECTOR v0 = g_Particle[i].vLaunchVelocity;
			D3DVECTOR r0 = g_Particle[i].vInitialPosition;
			g_Particle[i].vPosition = r0 + v0*(t-RND()/10)/1.5f;
		} 
		else // Particle is in "explode" mode
		{
			D3DVECTOR v0 = g_Particle[i].vInitialVelocity;
			D3DVECTOR r0 = g_Particle[i].vInitialPosition;
			FLOAT fDamping = (1.0f-(FLOAT)exp(-k*t))/(k*k);
			g_Particle[i].vPosition = r0 + a0*t/k + (k*v0+a0)*fDamping;
			g_Particle[i].fMaturity = t / g_Particle[i].fLifetime;

			FLOAT st = g_Particle[i].fMaturity+0.5f;
			g_Particle[i].r     = (FLOAT)exp(-0.5f*st*st);
			g_Particle[i].g     = (FLOAT)exp(-1.8f*st*st);
			g_Particle[i].b     = (FLOAT)exp(-2.0f*st*st);
			g_Particle[i].fSize = (FLOAT)exp(-1.0f*st*st);

			if( g_Particle[i].fMaturity > 1.0f )
				dwNumOldParticles++;
		}
    }

    if( NUM_PARTICLES == dwNumOldParticles )
    {
		g_fStartTimeKey = fTimeKey + 1.0f;

		D3DVECTOR vLaunchVelocity = D3DVECTOR( 40.0f*rnd(), 30.0f, 0.0f );

        for( WORD i=0; i<NUM_PARTICLES; i++ )
            g_Particle[i] = SetParticle( (i%3), 4.0f, D3DVECTOR(0,0,0),
			                             vLaunchVelocity );
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
        pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("lake.bmp") );
        pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX,
                                   g_Background, 4, 0 );

        // Render the particles
        RenderParticles( pd3dDevice );

		// End the scene.
		pd3dDevice->EndScene();
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderParticles()
// Desc: Draws the system of particles
//-----------------------------------------------------------------------------
HRESULT RenderParticles( LPDIRECT3DDEVICE3 pd3dDevice )
{
    D3DMATERIAL mtrl;
	D3DUtil_InitMaterial( mtrl );

	// Turn on alpha blending for the particles
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("firework.bmp") );

    // Do the particles
    for( DWORD i=0; i<NUM_PARTICLES; i++ )
    {
        if( g_Particle[i].fMaturity > 1.0f )
			continue;

        // Set up the emissive color of the particle
        mtrl.emissive.r = g_Particle[i].r;
        mtrl.emissive.g = g_Particle[i].g;
        mtrl.emissive.b = g_Particle[i].b;
        g_pmtrlRenderMtrl->SetMaterial( &mtrl );

        // Translate and scale the world matrix for each particle
	    D3DMATRIX matWorld;
        D3DUtil_SetTranslateMatrix( matWorld, g_Particle[i].vPosition );
        matWorld._11 = g_Particle[i].fSize;
        matWorld._22 = g_Particle[i].fSize;
        
        // Set the new world transform and render the particle
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
    D3DTextr_DestroyTexture( "lake.bmp" );
    D3DTextr_DestroyTexture( "firework.bmp" );
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
    // Get triangle caps (Hardware or software) and check for alpha blending
    LPD3DPRIMCAPS pdpc = &pd3dDeviceDesc->dpcTriCaps;

    if( 0 == ( pdpc->dwSrcBlendCaps & pdpc->dwDestBlendCaps & D3DBLEND_ONE ) )
        return E_FAIL;

    return S_OK;
}





