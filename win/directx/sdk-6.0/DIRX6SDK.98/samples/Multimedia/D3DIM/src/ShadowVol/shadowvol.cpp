//-----------------------------------------------------------------------------
// File: ShadowVol.cpp
//
// Desc: Example code showing how to use stencil buffers to implement shadow
//       voulmes.
//
//       Note: This code uses the D3D Framework helper library.
//
//
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define STRICT
#define D3D_OVERLOADS
#include <math.h>
#include "D3DTextr.h"
#include "D3DUtil.h"
#include "D3DEnum.h"


//-----------------------------------------------------------------------------
// Declare the application globals for use in WinMain.cpp
//-----------------------------------------------------------------------------
TCHAR* g_strAppTitle       = TEXT( "Shadow: Stencil Shadows Example" );
BOOL   g_bAppUseZBuffer    = FALSE;
BOOL   g_bAppUseBackBuffer = TRUE;




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define LERP(m,x0,x1)       ((x0) + (m)*((x1)-(x0)))
#define PI                  3.14159265358979323846f
#define ELLIPSE_RADIUS      1.5f
#define ELLIPSE_NUMRINGS    20
#define ELLIPSE_NUMSECTIONS 20
#define ELLIPSE_X_LENGTH    1.0f
#define ELLIPSE_Y_LENGTH    3.0f
#define ELLIPSE_Z_LENGTH    1.0f

LPDIRECTDRAWSURFACE4 g_pddsDepthBuffer = NULL;
LPDIRECT3DMATERIAL3  g_pmtrlObjectMtrl = NULL;
LPDIRECT3DLIGHT         g_pLight          = NULL;

D3DVERTEX* g_pvModelVertices1 = NULL;             //object's vertices
D3DVERTEX* g_pvModelVertices2 = NULL;             //object's vertices
D3DVERTEX* g_pvRenderVertices = NULL;             //object's vertices
WORD*      g_pwRenderIndices  = NULL;             //object's indices
DWORD      g_dwNumVertices;
DWORD      g_dwNumIndices;

typedef struct
{
    D3DVECTOR p;
    D3DCOLOR  c;
} COLORVERTEX;

COLORVERTEX g_pvPolyVertices[100];         // verts of square
WORD        g_pwPolyIndices[100];         // indices of square's tris
WORD        g_pwShadVolIndices[100];    




//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
VOID    AppPause( BOOL );
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    RotateVertexInX( FLOAT, DWORD, D3DVERTEX*, D3DVERTEX* );
BOOL    GenerateSphere( FLOAT, DWORD, DWORD, FLOAT, FLOAT, FLOAT, D3DVERTEX**,
                        DWORD*, WORD**, DWORD* );
VOID    BlendObjects( DWORD, D3DVERTEX*, D3DVERTEX*, D3DVERTEX* );




//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{

#define PWIDTH (ELLIPSE_Y_LENGTH+2.5)
#define DEPTHBELOW (-1.5)

    g_pvPolyVertices[0].p = D3DVECTOR( -PWIDTH, DEPTHBELOW, -PWIDTH);
    g_pvPolyVertices[1].p = D3DVECTOR(  PWIDTH, DEPTHBELOW, -PWIDTH);
    g_pvPolyVertices[2].p = D3DVECTOR(  PWIDTH, DEPTHBELOW, PWIDTH);
    g_pvPolyVertices[3].p = D3DVECTOR( -PWIDTH, DEPTHBELOW, PWIDTH);

    g_pvPolyVertices[3].c =     g_pvPolyVertices[2].c =
    g_pvPolyVertices[1].c =     g_pvPolyVertices[0].c = RGBA_MAKE(0xff,0,0,0xff);

    // note these square's are one-sided
    g_pwPolyIndices[0]=0;
    g_pwPolyIndices[1]=2;
    g_pwPolyIndices[2]=1;

    g_pwPolyIndices[3]=0;
    g_pwPolyIndices[4]=3;
    g_pwPolyIndices[5]=2;

    // small square shadow caster

#define SM_WIDTH ((float)(0.4*PWIDTH))
#define SM_HEIGHT ((float)(DEPTHBELOW+5.5))

    g_pvPolyVertices[4].p = D3DVECTOR( -SM_WIDTH, SM_HEIGHT, -SM_WIDTH);
    g_pvPolyVertices[5].p = D3DVECTOR(  SM_WIDTH, SM_HEIGHT, -SM_WIDTH);
    g_pvPolyVertices[6].p = D3DVECTOR(  SM_WIDTH, SM_HEIGHT, SM_WIDTH);
    g_pvPolyVertices[7].p = D3DVECTOR( -SM_WIDTH, SM_HEIGHT, SM_WIDTH);

    g_pvPolyVertices[4].c =     g_pvPolyVertices[5].c =
    g_pvPolyVertices[6].c =     g_pvPolyVertices[7].c = RGBA_MAKE(0xff,0x0,0xff,0xff);

    g_pwPolyIndices[6]=4;
    g_pwPolyIndices[7]=5;
    g_pwPolyIndices[8]=6;

    g_pwPolyIndices[9]=4;
    g_pwPolyIndices[10]=6;
    g_pwPolyIndices[11]=7;

// simple, hard-coded shadow volume with only outward-facing tris
// this assumes a directional light at infinity above
// the shadow caster, so that shadow volume walls are parallel

// re-use verts of shadow caster

#define UNDERHEIGHT (DEPTHBELOW-1.0)
// shad vol must go encompass all shadow receivers

    g_pvPolyVertices[8].p = D3DVECTOR( -SM_WIDTH, UNDERHEIGHT, -SM_WIDTH);
    g_pvPolyVertices[9].p = D3DVECTOR(  SM_WIDTH, UNDERHEIGHT, -SM_WIDTH);
    g_pvPolyVertices[10].p = D3DVECTOR(  SM_WIDTH, UNDERHEIGHT, SM_WIDTH);
    g_pvPolyVertices[11].p = D3DVECTOR( -SM_WIDTH, UNDERHEIGHT, SM_WIDTH);

    g_pvPolyVertices[8].c =     g_pvPolyVertices[10].c =
    g_pvPolyVertices[9].c =     g_pvPolyVertices[11].c = RGBA_MAKE(0x0,0x0,0xff,0xff);

    int i=0;

    g_pwShadVolIndices[i++]=5;
    g_pwShadVolIndices[i++]=8;
    g_pwShadVolIndices[i++]=4;

    g_pwShadVolIndices[i++]=5;
    g_pwShadVolIndices[i++]=9;
    g_pwShadVolIndices[i++]=8;

    g_pwShadVolIndices[i++]=6;
    g_pwShadVolIndices[i++]=9;
    g_pwShadVolIndices[i++]=5;

    g_pwShadVolIndices[i++]=6;
    g_pwShadVolIndices[i++]=10;
    g_pwShadVolIndices[i++]=9;

    g_pwShadVolIndices[i++]=7;
    g_pwShadVolIndices[i++]=10;
    g_pwShadVolIndices[i++]=6;

    g_pwShadVolIndices[i++]=7;
    g_pwShadVolIndices[i++]=11;
    g_pwShadVolIndices[i++]=10;

    g_pwShadVolIndices[i++]=4;
    g_pwShadVolIndices[i++]=11;
    g_pwShadVolIndices[i++]=7;

    g_pwShadVolIndices[i++]=4;
    g_pwShadVolIndices[i++]=8;
    g_pwShadVolIndices[i++]=11;

    // Generate the object data
    GenerateSphere( ELLIPSE_RADIUS, ELLIPSE_NUMRINGS, ELLIPSE_NUMSECTIONS, 
                    ELLIPSE_X_LENGTH, ELLIPSE_Y_LENGTH, ELLIPSE_Z_LENGTH, 
                    &g_pvRenderVertices, &g_dwNumVertices, 
                    &g_pwRenderIndices, &g_dwNumIndices );
    RotateVertexInX( (FLOAT)(PI/2), g_dwNumVertices, g_pvRenderVertices,
                     g_pvRenderVertices );

    // Make two copies of the object (for modification of the vertices)
    g_pvModelVertices1 = new D3DVERTEX[g_dwNumVertices];
    g_pvModelVertices2 = new D3DVERTEX[g_dwNumVertices];
    memcpy( g_pvModelVertices1, g_pvRenderVertices,
            g_dwNumVertices*sizeof(D3DVERTEX) );
    memcpy( g_pvModelVertices2, g_pvRenderVertices,
            g_dwNumVertices*sizeof(D3DVERTEX) );

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
    // Compute the Shadow and rotate angles for this frame
    static FLOAT fTime = 0.0f;
    FLOAT fRotateAngle = (FLOAT)( fTime / 7 );
    FLOAT fShadowAngle   = (FLOAT)( (sin(fTime)+1.0f)*0.6f );
    fTime += .50f;

    // Setup the world spin matrix
    D3DMATRIX matWorldSpin;
    D3DUtil_SetRotateYMatrix( matWorldSpin, fRotateAngle );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorldSpin );

    // Shadow two copies of the object in different directions and 
    // merge (blend) them into one set of vertex data.
    RotateVertexInX( fShadowAngle, g_dwNumVertices, g_pvModelVertices2,
                     g_pvModelVertices1 );
    BlendObjects( g_dwNumVertices, g_pvModelVertices1, g_pvModelVertices2,
                  g_pvRenderVertices );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderShadow()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT RenderShadow( LPDIRECT3DDEVICE3 pd3dDevice )
{
    // Turn depth buffer off, and stencil buffer on
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ZWRITEENABLE,  FALSE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILENABLE, TRUE );

    // Set up stencil compare fuction, reference value, and masks
    // Stencil test passes if ((ref & mask) cmpfn (stencil & mask)) is true
    pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILFUNC,     D3DCMP_ALWAYS );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILREF,      0x1 );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILMASK,     0xffffffff );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILWRITEMASK,0xffffffff );

    // If ztest passes, write 1 into stencil buffer
    pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILZFAIL, D3DSTENCILOP_KEEP );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILFAIL,  D3DSTENCILOP_KEEP );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILPASS,  D3DSTENCILOP_REPLACE );

    // Since destcolor=SRCBLEND * SRC_COLOR + DESTBLEND * DEST_COLOR,
    // this should result in the tri color being completely dropped
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,  D3DBLEND_ZERO );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE );

    // draw front-side of shadow volume in stencil/z only
    pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, (D3DFVF_XYZ | D3DFVF_DIFFUSE),
                                      g_pvPolyVertices, 12,
                                      g_pwShadVolIndices, 3*2*4, NULL );

    // Now reverse cull order so back sides of shadow volume are written.
    // write 0's into
    pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILREF, 0x0 );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_CULLMODE,   D3DCULL_CW );

    // Draw back-side of shadow volume in stencil/z only
    pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, (D3DFVF_XYZ | D3DFVF_DIFFUSE),
                                      g_pvPolyVertices, 12,
                                      g_pwShadVolIndices, 3*2*4, NULL );

    // Restore render states
    pd3dDevice->SetRenderState( D3DRENDERSTATE_CULLMODE, D3DCULL_CCW );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ZWRITEENABLE,     TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILENABLE,    FALSE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DrawShadow()
// Desc: Draws a big grey polygon over scene, and blend it with pixels with
//       stencil 1, which are in shadow
//-----------------------------------------------------------------------------
HRESULT DrawShadow( LPDIRECT3DDEVICE3 pd3dDevice, D3DRECT* prcBounds )
{
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, FALSE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILENABLE,    TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );

    // Since destcolor=SRCBLEND * SRC_COLOR + DESTBLEND * DEST_COLOR,
    // this results in destcolor= (AlphaSrc) * SRC_COLOR + (1-AlphaSrc)*DestColor
    pd3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
    pd3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);

    // Only write where stencil val == 1. 
    pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILREF,  0x1 );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILFUNC, D3DCMP_EQUAL );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILPASS, D3DSTENCILOP_KEEP );

    // Set the world matrix to identity to draw the big grey square
    D3DMATRIX matWorld, matIdentity;
    pd3dDevice->GetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
    D3DUtil_SetIdentityMatrix( matIdentity );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matIdentity );

    COLORVERTEX sqverts[4];     
    WORD sqindices[6];

    FLOAT x1 = (FLOAT)prcBounds->x1;
    FLOAT x2 = (FLOAT)prcBounds->x2;
    FLOAT y1 = (FLOAT)prcBounds->y1;
    FLOAT y2 = (FLOAT)prcBounds->y2;
    FLOAT dx,dy;

    // 0,0 is center of screen, so shift coords over

    dx = (x2-x1)/2.0f; dy = (y2-y1)/2.0f;

    x1 -= dx;  x2 -= dx;
    y1 -= dy;  y2 -= dy;

    sqverts[0].p = D3DVECTOR( x1, y1, 0.0f );
    sqverts[1].p = D3DVECTOR( x2, y1, 0.0f );
    sqverts[2].p = D3DVECTOR( x2, y2, 0.0f );
    sqverts[3].p = D3DVECTOR( x1, y2, 0.0f );
    sqverts[0].c = sqverts[1].c = sqverts[2].c = sqverts[3].c = RGBA_MAKE(0x0,0x0,0x0,0x7f);

    sqindices[0]=0; sqindices[1]=2; sqindices[2]=1;
    sqindices[3]=0; sqindices[4]=3; sqindices[5]=2;

    pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, (D3DFVF_XYZ | D3DFVF_DIFFUSE),
                                      sqverts, 4, sqindices, 6, NULL );

    // Restore render states
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE,          TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILENABLE,    FALSE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );

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
    // Clear the viewport, zbuffer, and stencil buffer
    DWORD dwClearFlags = D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL;
    DWORD dwClearColor = RGBA_MAKE(51, 153, 255, 0);
    pvViewport->Clear2( 1UL, prcViewportRect, dwClearFlags, dwClearColor,
                        1.0f, 0L );

    // Begin the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        //Display the object
        pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("Banana.bmp") );
        pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                          g_pvRenderVertices, g_dwNumVertices, 
                                          g_pwRenderIndices, g_dwNumIndices, NULL );
            
        // Draw squares
        pd3dDevice->SetTexture( 0, NULL);
        pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, (D3DFVF_XYZ | D3DFVF_DIFFUSE),
                                          g_pvPolyVertices, 8,
                                          g_pwPolyIndices, 12, NULL );

        // Render the shadow volume into the stenicl buffer, then add it into
        // the scene
        RenderShadow( pd3dDevice );
        DrawShadow( pd3dDevice, prcViewportRect );

        // End the scene.
        pd3dDevice->EndScene();
     }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: EnumZBufferFormatsCallback()
// Desc: Enumeration function to report valid pixel formats for z-buffers.
//-----------------------------------------------------------------------------
static HRESULT WINAPI EnumZBufferFormatsCallback( DDPIXELFORMAT* pddpf,
                                                  VOID* pddpfDesired )
{
    if( NULL==pddpf || NULL==pddpfDesired )
        return D3DENUMRET_CANCEL;

    // If the current pixel format's match the desired ones (DDPF_ZBUFFER and
    // possibly DDPF_STENCILBUFFER), lets copy it and return. This function is
    // not choosy...it accepts the first valid format that comes along.
    if( pddpf->dwFlags == ((DDPIXELFORMAT*)pddpfDesired)->dwFlags )
    {
        memcpy( pddpfDesired, pddpf, sizeof(DDPIXELFORMAT) );
        return D3DENUMRET_CANCEL;
    }

    return D3DENUMRET_OK;
}


HRESULT CreateStencilBuffer( LPDIRECT3DDEVICE3 pd3dDevice,
                             LPDIRECTDRAWSURFACE4* ppddsDepthBuffer )
{
    GUID*           pDriverGUID;
    GUID*           pDeviceGUID;
    DDSURFACEDESC2* pMode;
    D3DEnum_GetSelectedDriver( &pDriverGUID, &pDeviceGUID, &pMode );

	// Get a ptr to the ID3D object to create materials and/or lights. Note:
	// the Release() call just serves to decrease the ref count.
    LPDIRECT3D3 pD3D;
    pd3dDevice->GetDirect3D( &pD3D );
    pD3D->Release();

	// Get a ptr to the render target
    LPDIRECTDRAWSURFACE4 pdds;
    pd3dDevice->GetRenderTarget( &pdds );
    pdds->Release();

	// Get a ptr to the IDDraw object
    LPDIRECTDRAW4 pDD;
    pdds->GetDDInterface( (VOID**)&pDD );
    pDD->Release();

    pdds->DeleteAttachedSurface( 0,NULL );  // just in case

    // Get z-buffer dimensions from the render target
    // Setup the surface desc for the z-buffer.
    DDSURFACEDESC2 ddsd;
    D3DUtil_InitSurfaceDesc( ddsd );

    pdds->GetSurfaceDesc( &ddsd );

    DWORD dwMemType =  D3DUtil_GetDeviceMemoryType( pd3dDevice );

    ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | dwMemType;

    ddsd.ddpfPixelFormat.dwFlags = DDPF_ZBUFFER | DDPF_STENCILBUFFER;
    
    // Get an appropiate pixel format from enumeration of the formats.
    pD3D->EnumZBufferFormats( *pDeviceGUID, EnumZBufferFormatsCallback,
                              (VOID*)&ddsd.ddpfPixelFormat );

    // Create and attach a z-buffer
    if( FAILED( pDD->CreateSurface( &ddsd, ppddsDepthBuffer, NULL ) ) )
        return E_FAIL;


    if( FAILED( pdds->AddAttachedSurface( *ppddsDepthBuffer ) ) )
        return E_FAIL;

    // The SetRenderTarget() call is needed to rebuild internal structures for
    // the newly attached zbuffer.
    return pd3dDevice->SetRenderTarget( pdds, 0L );
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

    HRESULT           hr;
    D3DMATERIAL       mtrl;
    D3DMATERIALHANDLE hmtrl;

    // Create the stencil buffer, and reset the viewport which gets trashed
    // in the process.
    if( FAILED( CreateStencilBuffer( pd3dDevice, &g_pddsDepthBuffer ) ) )
        return E_FAIL;
    if( FAILED( pd3dDevice->SetCurrentViewport( pvViewport ) ) )
        return E_FAIL;

    // Create and set up the shine materials w/ textures
    if( FAILED( hr = pD3D->CreateMaterial( &g_pmtrlObjectMtrl, NULL ) ) )
        return E_FAIL;

    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    mtrl.power = 40.0f;
    g_pmtrlObjectMtrl->SetMaterial( &mtrl );
    g_pmtrlObjectMtrl->GetHandle( pd3dDevice, &hmtrl );
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_MATERIAL, hmtrl );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, 1 );
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_AMBIENT,  0x40404040 );

    D3DTextr_RestoreAllTextures( pd3dDevice );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );

    // Set the transform matrices
    D3DVECTOR vEyePt    = D3DVECTOR( 0, 0, -6.5 );
    D3DVECTOR vLookatPt = D3DVECTOR( 0, 0,   0  );
    D3DVECTOR vUpVec    = D3DVECTOR( 0, 1,   0  );
    D3DMATRIX matWorld, matView, matProj;

    D3DUtil_SetIdentityMatrix( matWorld );
    D3DUtil_SetViewMatrix( matView, vEyePt, vLookatPt, vUpVec );
    D3DUtil_SetProjectionMatrix( matProj, 2.0f, 1.0f, 0.2f, 50.0f );

    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW,       &matView );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

    // Set up the light
    if( FAILED( hr = pD3D->CreateLight( &g_pLight, NULL ) ) )
        return E_FAIL;

    D3DLIGHT light;
    D3DUtil_InitLight( light, D3DLIGHT_POINT, 0.0, 0.0, -12.0 );
    light.dvAttenuation0 = 1.0f;
    g_pLight->SetLight( &light );
    pvViewport->AddLight( g_pLight );

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
    
    SAFE_DELETE( g_pvModelVertices1 );
    SAFE_DELETE( g_pvModelVertices2 );
    SAFE_DELETE( g_pvRenderVertices );
    SAFE_DELETE( g_pwRenderIndices );

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
    SAFE_RELEASE( g_pddsDepthBuffer );
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
    // Get device's stencil caps
    DWORD dwStencilCaps = pd3dDeviceDesc->dwStencilCaps;

    if( 0 == dwStencilCaps )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RotateVertexInX()
// Desc: Rotates an array of vertices by an amount theta about the x-axis.
//-----------------------------------------------------------------------------
VOID RotateVertexInX( FLOAT fTheta, DWORD dwCount,
                      D3DVERTEX* pvInVertices, D3DVERTEX* pvOutVertices )
{
    FLOAT fSin = (FLOAT)sin(fTheta); 
    FLOAT fCos = (FLOAT)cos(fTheta);
    
    for( DWORD i=0; i<dwCount; i++ )
    {
        FLOAT y = pvInVertices[i].y;
        FLOAT z = pvInVertices[i].z;
        pvOutVertices[i].y = fCos*y + fSin*z;
        pvOutVertices[i].z = -fSin*y + fCos*z;

        FLOAT ny = pvInVertices[i].ny;
        FLOAT nz = pvInVertices[i].nz;
        pvOutVertices[i].ny = fCos*ny + fSin*nz;
        pvOutVertices[i].nz = -fSin*ny + fCos*nz;
    }
}




//-----------------------------------------------------------------------------
// Name: GenerateSphere()
// Desc: Makes vertex and index data for a sphere.
//-----------------------------------------------------------------------------
BOOL GenerateSphere( FLOAT fRadius, DWORD dwNumRings, DWORD dwNumSections, 
                     FLOAT sx, FLOAT sy, FLOAT sz,
                     D3DVERTEX** ppvVertices, DWORD* pdwNumVertices,
                     WORD** ppwIndices, DWORD* pdwNumIndices )
{
	FLOAT x, y, z, v, rsintheta; // Temporary variables
    DWORD i, j;            // counters
    DWORD n, m;            // counters

    //Generate space for the required triangles and vertices.
    DWORD      dwNumTriangles = (dwNumRings+1) * dwNumSections * 2;
    DWORD      dwNumVertices  = (dwNumRings+1) * dwNumSections + 2;
    D3DVERTEX* pvVertices     = new D3DVERTEX[dwNumVertices];
    DWORD      dwNumIndices   = dwNumTriangles*3;
    WORD*      pwIndices      = new WORD[dwNumIndices];

    // Generate vertices at the top and bottom points.
    D3DVECTOR vPoint  = D3DVECTOR( 0.0f, sy*fRadius, 0.0f );
    D3DVECTOR vNormal = D3DVECTOR( 0.0f, 0.0f, 1.0f );
    pvVertices[0]               = D3DVERTEX(  vPoint,  vNormal, 0.0f, 0.0f );
    pvVertices[dwNumVertices-1] = D3DVERTEX( -vPoint, -vNormal, 0.0f, 0.0f );

    // Generate vertex points for rings
    FLOAT dtheta = (FLOAT)(PI / (dwNumRings + 2));     //Angle between each ring
    FLOAT dphi   = (FLOAT)(2*PI / dwNumSections); //Angle between each section
    FLOAT theta  = dtheta;
    n = 1; //vertex being generated, begins at 1 to skip top point

	dwNumRings += 1;
	dwNumRings -= 1;

    for( i = 0; i < (dwNumRings+1); i++ )
    {
        y = fRadius * (FLOAT)cos(theta); // y is the same for each ring
        v = theta / PI;     // v is the same for each ring
        rsintheta = fRadius * (FLOAT)sin(theta);
        FLOAT phi = 0.0f;

        for( j = 0; j < dwNumSections; j++ )
        {
            x = rsintheta * (FLOAT)sin(phi);
            z = rsintheta * (FLOAT)cos(phi);
        
            FLOAT u = (FLOAT)(1.0 - phi / (2*PI) );
            
            vPoint        = D3DVECTOR( sx*x, sy*y, sz*z );
            vNormal       = D3DVECTOR( x/fRadius, y/fRadius, z/fRadius );
            pvVertices[n] = D3DVERTEX( vPoint, vNormal, u, v );

            phi += dphi;
            ++n;
        }
        theta += dtheta;
    }

    // Generate triangles for top and bottom caps.
	for( i = 0; i < dwNumSections; i++ )
	{
		DWORD t1 = 3*i;
		DWORD t2 = 3*(dwNumTriangles - dwNumSections + i);

		pwIndices[t1+0] = (WORD)(0);
		pwIndices[t1+1] = (WORD)(i + 1);
		pwIndices[t1+2] = (WORD)(1 + ((i + 1) % dwNumSections));

		pwIndices[t2+0] = (WORD)( dwNumVertices - 1 );
		pwIndices[t2+1] = (WORD)( dwNumVertices - 2 - i );
		pwIndices[t2+2] = (WORD)( dwNumVertices - 2 - ((1 + i) % dwNumSections) );
	}

	// Generate triangles for the rings
	m = 1;            // 1st vertex begins at 1 to skip top point
	n = dwNumSections; // triangle being generated, skip the top cap 
    
	for( i = 0; i < dwNumRings; i++ )
	{
		for( j = 0; j < dwNumSections; j++ )
		{
			pwIndices[3*n+0] = (WORD)(m + j);
			pwIndices[3*n+1] = (WORD)(m + dwNumSections + j);
			pwIndices[3*n+2] = (WORD)(m + dwNumSections + ((j + 1) % dwNumSections));
			n++;
			
			pwIndices[3*n+0] = (WORD)(m + j);
			pwIndices[3*n+1] = (WORD)(m + dwNumSections + ((j + 1) % dwNumSections));
			pwIndices[3*n+2] = (WORD)(m + ((j + 1) % dwNumSections));
			n++;
		}
		m += dwNumSections;
	}

    (*pdwNumIndices)  = dwNumIndices;
    (*ppwIndices)     = pwIndices;
    (*pdwNumVertices) = dwNumVertices;
    (*ppvVertices)    = pvVertices;

	return TRUE;
}




//-----------------------------------------------------------------------------
// Name: BlendObjects()
// Desc: Merges two sets of vertices together
//-----------------------------------------------------------------------------
VOID BlendObjects( DWORD dwCount, D3DVERTEX* pvInputVertices1, 
                   D3DVERTEX* pvInputVertices2,
                   D3DVERTEX* pvOutputVertices )
{
    D3DVERTEX* p1 = pvInputVertices1;
    D3DVERTEX* p2 = pvInputVertices2;
    D3DVERTEX* p3 = pvOutputVertices;

    FLOAT fMinZ = -ELLIPSE_Y_LENGTH * ELLIPSE_RADIUS;
    FLOAT fMaxZ = +ELLIPSE_Y_LENGTH * ELLIPSE_RADIUS;

    for( DWORD i=0; i<dwCount; i++ )
    {
        FLOAT m;
        FLOAT a = ( p2->z - fMinZ ) / ( fMaxZ - fMinZ );

        if( a >= 0.75f )
            m = 0.0f;
        else if( a >= 0.5f )
        {
            FLOAT x = 4*(0.75f-a);
            m = (x*x)*0.5f;
        }
        else if( a >= 0.25f )
        {
            FLOAT x = 4*(a-0.25f);
            m = 1.0f-(x*x)*0.5f;
        }
        else
            m = 1.0f;

        p3->x = LERP( m, p1->x, p2->x );
        p3->y = LERP( m, p1->y, p2->y );
        p3->z = LERP( m, p1->z, p2->z );

        p1++; p2++; p3++;
    }
}


