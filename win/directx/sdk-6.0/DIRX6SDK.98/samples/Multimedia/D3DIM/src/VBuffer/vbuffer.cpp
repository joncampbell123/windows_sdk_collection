//-----------------------------------------------------------------------------
// File: VBuffer.cpp
//
// Desc: Example code showing how to use DirectX 6 vertex buffers.
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


//-----------------------------------------------------------------------------
// Declare the application globals for use in WinMain.cpp
//-----------------------------------------------------------------------------
TCHAR* g_strAppTitle       = TEXT( "VBuffer: Direct3D Vertex Buffer Demo" );
BOOL   g_bAppUseZBuffer    = TRUE;
BOOL   g_bAppUseBackBuffer = TRUE;




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
LPDIRECT3DMATERIAL3    g_pmtrlObjectMtrl     = NULL;
LPDIRECT3DLIGHT        g_pLight              = NULL;
LPDIRECT3DVERTEXBUFFER g_pvbVertexBuffer     = NULL;

#define FLAG_SIZE         10
#define NUM_FLAG_VERTICES ((FLAG_SIZE+1)*(FLAG_SIZE+1))
#define NUM_FLAG_INDICES  (FLAG_SIZE*FLAG_SIZE*6)
#define NUM_POLE_VERTICES 8*2

WORD        g_pwFlagIndices[NUM_FLAG_INDICES];
D3DVERTEX   g_pvPoleVertices[NUM_POLE_VERTICES];
D3DTLVERTEX g_Background[4];



//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );




//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{
	for( WORD i=0, ix=0; ix<FLAG_SIZE; ix++ )
	{
		for( WORD iy=0; iy<FLAG_SIZE; iy++ )
		{
			g_pwFlagIndices[i++] = (ix+0) + (iy+1)*(FLAG_SIZE+1);
			g_pwFlagIndices[i++] = (ix+1) + (iy+0)*(FLAG_SIZE+1);
			g_pwFlagIndices[i++] = (ix+0) + (iy+0)*(FLAG_SIZE+1);
			g_pwFlagIndices[i++] = (ix+0) + (iy+1)*(FLAG_SIZE+1);
			g_pwFlagIndices[i++] = (ix+1) + (iy+1)*(FLAG_SIZE+1);
			g_pwFlagIndices[i++] = (ix+1) + (iy+0)*(FLAG_SIZE+1);
		}
	}


	for( int r=0; r<8; r++ )
	{
		FLOAT theta = (r/8.0f)*2*3.1415926283f;
		FLOAT x     = (FLOAT)cos(theta)*0.1f;
		FLOAT z     = -(FLOAT)sin(theta)*0.1f;

		D3DVECTOR vNorm = Normalize( D3DVECTOR(x,0,z) );

		g_pvPoleVertices[2*r+0] = D3DVERTEX( D3DVECTOR(x,10,z),
			                                 vNorm, r/8.0f, 0.0f );
		g_pvPoleVertices[2*r+1] = D3DVERTEX( D3DVECTOR(x,0,z),
			                                 vNorm, r/8.0f, 1.0f );
	}

    g_Background[0] = D3DTLVERTEX( D3DVECTOR( 0, 0, 0.99f ), 0.5, -1, 0, 0.0f, 0.6f );
    g_Background[1] = D3DTLVERTEX( D3DVECTOR( 0, 0, 0.99f ), 0.5, -1, 0, 0.0f, 0.0f );
    g_Background[2] = D3DTLVERTEX( D3DVECTOR( 0, 0, 0.99f ), 0.5, -1, 0, 1.0f, 0.6f );
    g_Background[3] = D3DTLVERTEX( D3DVECTOR( 0, 0, 0.99f ), 0.5, -1, 0, 1.0f, 0.0f );


    // Create some textures
    D3DTextr_CreateTexture( "Flag.bmp" );
    D3DTextr_CreateTexture( "Cloud3.bmp" );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT App_FrameMove( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{
    D3DVERTEX* pvFlagVertices;
    g_pvbVertexBuffer->Lock( DDLOCK_WAIT, (VOID**)&pvFlagVertices, NULL );

	for( int ix=0; ix<FLAG_SIZE+1; ix++ )
	{
		for( int iy=0; iy<FLAG_SIZE+1; iy++ )
		{
			pvFlagVertices[ix+iy*(FLAG_SIZE+1)].z = ix*0.2f*(FLOAT)sin(ix-fTimeKey*6 )/(FLAG_SIZE+1);
		}
	}

    g_pvbVertexBuffer->Unlock();

	// Move the clouds
	FLOAT t = fTimeKey/40.0f;
	FLOAT u = (((DWORD)(t*10000))%10000)/10000.0f;
    g_Background[0].tu = 0 - u;
    g_Background[1].tu = 0 - u;
    g_Background[2].tu = 1 - u;
    g_Background[3].tu = 1 - u;

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
    pvViewport->Clear2( 1UL, prcViewRect, D3DCLEAR_ZBUFFER, 0L, 1.0f, 0L );

    // Begin the scene 
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        // Draw the background
        pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("Cloud3.bmp") );
        pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX,
                                   g_Background, 4, 0 );

        // Draw the pole
        pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX,
                                   g_pvPoleVertices, 16, 0 );

        //Display the object
	    pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("Flag.bmp") );
        pd3dDevice->DrawIndexedPrimitiveVB( D3DPT_TRIANGLELIST, 
			                                g_pvbVertexBuffer, g_pwFlagIndices,
						                    NUM_FLAG_INDICES, D3DDP_WAIT );

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

	// Get a ptr to the ID3D object to create VB's, materials and/or lights.
    // Note: the Release() call just serves to decrease the ref count.
    LPDIRECT3D3 pD3D;
    if( FAILED( pd3dDevice->GetDirect3D( &pD3D ) ) )
        return E_FAIL;
    pD3D->Release();

	// Create and fill the vertex buffer.
	D3DVERTEXBUFFERDESC vbdesc;
	ZeroMemory( &vbdesc, sizeof(D3DVERTEXBUFFERDESC) );
	vbdesc.dwSize        = sizeof(D3DVERTEXBUFFERDESC);
	vbdesc.dwCaps        = 0L;
    vbdesc.dwFVF         = D3DFVF_VERTEX;
    vbdesc.dwNumVertices = NUM_FLAG_VERTICES;

	// Get the device's caps bits
	D3DDEVICEDESC ddHwDesc, ddSwDesc;
	D3DUtil_InitDeviceDesc( ddHwDesc );
	D3DUtil_InitDeviceDesc( ddSwDesc );
	if( FAILED( pd3dDevice->GetCaps( &ddHwDesc, &ddSwDesc ) ) )
		return E_FAIL;

	// If the device is not hardware, make sure vertex buffers use system mem.
	if( 0L == ddHwDesc.dwFlags )
		vbdesc.dwCaps |= D3DVBCAPS_SYSTEMMEMORY;

	if( FAILED( pD3D->CreateVertexBuffer( &vbdesc, &g_pvbVertexBuffer, 0L,
		                                  NULL ) ) )
		return E_FAIL;
	
	// Lock the vertex buffer and fill it
    D3DVERTEX* pvVertices;
    g_pvbVertexBuffer->Lock( DDLOCK_WAIT, (VOID**)&pvVertices, NULL );

	for( WORD ix=0; ix<FLAG_SIZE+1; ix++ )
	{
		for( WORD iy=0; iy<FLAG_SIZE+1; iy++ )
		{
			FLOAT tu = ix/(FLOAT)FLAG_SIZE;
			FLOAT tv = iy/(FLOAT)FLAG_SIZE;
			FLOAT x  = 0.2f + tu * 3.31f;
			FLOAT y  = 8.0f + tv * 1.82f;

			pvVertices[ix+iy*(FLAG_SIZE+1)] = D3DVERTEX( D3DVECTOR(x,y,0),
			                                         D3DVECTOR(0,0,-1), tu, 1-tv );
		}
	}
	
    g_pvbVertexBuffer->Unlock();

    // Size the background
	D3DVIEWPORT2 vp;
	vp.dwSize = sizeof(vp);
	pvViewport->GetViewport2(&vp);
	FLOAT fAspect = ((FLOAT)vp.dwHeight) / vp.dwWidth;
    g_Background[0].sy = (FLOAT)vp.dwHeight;
    g_Background[2].sy = (FLOAT)vp.dwHeight;
    g_Background[2].sx = (FLOAT)vp.dwWidth;
    g_Background[3].sx = (FLOAT)vp.dwWidth;

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

    // Set the transform matrices
    D3DVECTOR vEyePt    = D3DVECTOR( -1,  7.5, -3.0 );
    D3DVECTOR vLookatPt = D3DVECTOR( 2,  7.5,   0  );
    D3DVECTOR vUpVec    = D3DVECTOR( 0,  1,  0  );
    D3DMATRIX matWorld, matView, matProj;

    D3DUtil_SetTranslateMatrix( matWorld, 0, 0, 0 );
    D3DUtil_SetViewMatrix( matView, vEyePt, vLookatPt, vUpVec );
    D3DUtil_SetProjectionMatrix( matProj, 1.57f, fAspect, 1.0f, 100.0f );
    
	pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW,       &matView );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

    // Set up the light
    if( FAILED( pD3D->CreateLight( &g_pLight, NULL ) ) )
        return E_FAIL;
    
    D3DLIGHT light;
    D3DUtil_InitLight( light, D3DLIGHT_POINT, -1.0, 8.0, -2.0 );
    light.dvAttenuation0 = 1.0f;
    g_pLight->SetLight( &light );
    pvViewport->AddLight( g_pLight );

	// Set texture and miscellaneous states
    D3DTextr_RestoreAllTextures( pd3dDevice );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_AMBIENT,  0x11111111 );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, 1 );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT App_FinalCleanup( LPDIRECT3DDEVICE3 pd3dDevice, 
                          LPDIRECT3DVIEWPORT3 pvViewport)
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
                              LPDIRECT3DVIEWPORT3 pvViewport)
{
    D3DTextr_InvalidateAllTextures();

	SAFE_RELEASE( g_pvbVertexBuffer );
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




