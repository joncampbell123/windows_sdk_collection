//-----------------------------------------------------------------------------
// File: Compress.cpp
//
// Desc: Example code shows how to display a compressed DDS texture.
//
//       Note: This code uses the D3D Framework helper library.
//
//
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define STRICT
#define D3D_OVERLOADS
#include <math.h>
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include "D3DUtil.h"
#include "D3DMath.h"
#include "resource.h"


//-----------------------------------------------------------------------------
// Declare the application globals for use in WinMain.cpp
//-----------------------------------------------------------------------------
TCHAR* g_strAppTitle       = TEXT( "Compress: Texture Compression Example" );
BOOL   g_bAppUseZBuffer    = FALSE;   // Don't use a z-buffering
BOOL   g_bAppUseBackBuffer = TRUE;    // Create/use a back buffer




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
LPDIRECT3DDEVICE3   g_pd3dDevice       = NULL;
LPDIRECT3DMATERIAL3 g_pmtrlObjectMtrl  = NULL;
LPDIRECT3DLIGHT     g_pLight           = NULL;
LPDIRECT3DTEXTURE2  g_ptexTexture      = NULL; // The compressed DDS texture 

#define NUM_CUBE_VERTICES (6*4)
#define NUM_CUBE_INDICES  (6*2*3)
D3DVERTEX g_pCubeVertices[NUM_CUBE_VERTICES];  // Vertices for a cube
WORD      g_pCubeIndices[NUM_CUBE_INDICES];    // Indices for a cube

DWORD     g_dwDXT       = 0L;
BOOL      g_bMipTexture = TRUE;
BOOL      g_bSupportsMipmaps;

CHAR      g_strDiskPixelFormat[20];
CHAR      g_strRendererPixelFormat[20];
CHAR      g_strError[200];

struct PixelFormatNode
{
    DDPIXELFORMAT       ddpf;
    PixelFormatNode*    pNext;
    int                 nAlphaBits;
    int                 nRedBits;
    int                 nGreenBits;
    int                 nBlueBits;
    BOOL                bPremultipliedAlpha;
};




//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
VOID    AppOutputText( LPDIRECT3DDEVICE3, DWORD, DWORD, CHAR* );
VOID    AppPause( BOOL );
BOOL    GenerateBox( FLOAT, FLOAT, FLOAT, D3DVERTEX**, DWORD*, WORD**, DWORD* );
HRESULT LoadTexture( CHAR*, LPDIRECT3DDEVICE3, LPDIRECT3DTEXTURE2* );
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK App_OverridenWndProc( HWND, UINT, WPARAM, LPARAM );




//-----------------------------------------------------------------------------
// Name: CreateCube()
// Desc: Sets up the vertices for a cube.
//-----------------------------------------------------------------------------
HRESULT CreateCube( D3DVERTEX* pVertices, WORD* pIndices )
{
    // Define the normals for the cube
    D3DVECTOR n0( 0.0f, 0.0f,-1.0f ); // Front face
    D3DVECTOR n1( 0.0f, 0.0f, 1.0f ); // Back face
    D3DVECTOR n2( 0.0f, 1.0f, 0.0f ); // Bottom face
    D3DVECTOR n3( 0.0f,-1.0f, 0.0f ); // Top face
    D3DVECTOR n4( 1.0f, 0.0f, 0.0f ); // Left face
    D3DVECTOR n5(-1.0f, 0.0f, 0.0f ); // Right face

    // Front face
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f, 1.0f,-1.0f), n0, 0.01f, 0.01f ); 
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f, 1.0f,-1.0f), n0, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f,-1.0f,-1.0f), n0, 0.99f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f,-1.0f,-1.0f), n0, 0.01f, 0.99f ); 

    // Back face
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f, 1.0f, 1.0f), n1, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f,-1.0f, 1.0f), n1, 0.99f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f,-1.0f, 1.0f), n1, 0.01f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f, 1.0f, 1.0f), n1, 0.01f, 0.01f );

    // Top face
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f, 1.0f, 1.0f), n2, 0.01f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f, 1.0f, 1.0f), n2, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f, 1.0f,-1.0f), n2, 0.99f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f, 1.0f,-1.0f), n2, 0.01f, 0.99f );

    // Bottom face
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f,-1.0f, 1.0f), n3, 0.01f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f,-1.0f,-1.0f), n3, 0.01f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f,-1.0f,-1.0f), n3, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f,-1.0f, 1.0f), n3, 0.99f, 0.99f );

    // Right face
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f, 1.0f,-1.0f), n4, 0.01f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f, 1.0f, 1.0f), n4, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f,-1.0f, 1.0f), n4, 0.99f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f,-1.0f,-1.0f), n4, 0.01f, 0.99f );

    // Left face
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f, 1.0f,-1.0f), n5, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f,-1.0f,-1.0f), n5, 0.99f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f,-1.0f, 1.0f), n5, 0.01f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f, 1.0f, 1.0f), n5, 0.01f, 0.01f );

    // Set up the indices for the cube
    *pIndices++ =  0+0;   *pIndices++ =  0+1;   *pIndices++ =  0+2;
    *pIndices++ =  0+2;   *pIndices++ =  0+3;   *pIndices++ =  0+0;
    *pIndices++ =  4+0;   *pIndices++ =  4+1;   *pIndices++ =  4+2;
    *pIndices++ =  4+2;   *pIndices++ =  4+3;   *pIndices++ =  4+0;
    *pIndices++ =  8+0;   *pIndices++ =  8+1;   *pIndices++ =  8+2;
    *pIndices++ =  8+2;   *pIndices++ =  8+3;   *pIndices++ =  8+0;
    *pIndices++ = 12+0;   *pIndices++ = 12+1;   *pIndices++ = 12+2;
    *pIndices++ = 12+2;   *pIndices++ = 12+3;   *pIndices++ = 12+0;
    *pIndices++ = 16+0;   *pIndices++ = 16+1;   *pIndices++ = 16+2;
    *pIndices++ = 16+2;   *pIndices++ = 16+3;   *pIndices++ = 16+0;
    *pIndices++ = 20+0;   *pIndices++ = 20+1;   *pIndices++ = 20+2;
    *pIndices++ = 20+2;   *pIndices++ = 20+3;   *pIndices++ = 20+0;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{

    // Add a message handler to the program
    SetWindowLong( hWnd, GWL_WNDPROC, (LONG)App_OverridenWndProc );

    // Generate the cube data
    CreateCube( g_pCubeVertices, g_pCubeIndices );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT App_FrameMove( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{
    // Compute the x and y rotation
    FLOAT fRotateY = fTimeKey / 0.5f;
    FLOAT fRotateX = fTimeKey / 0.3f;
    FLOAT fTransZ  = (FLOAT) fabs( sin( fTimeKey ) * 50 );

    // Setup the world spin matrix
    D3DMATRIX matSpinY, matSpinX, matWorld;
    D3DUtil_SetRotateYMatrix( matSpinY, fRotateY );
    D3DUtil_SetRotateXMatrix( matSpinX, fRotateX );

    if( g_bMipTexture )
        D3DUtil_SetTranslateMatrix( matWorld, 0.0f, 0.0f, fTransZ );
    else
        D3DUtil_SetTranslateMatrix( matWorld, 0.0f, 0.0f, 0.0f );

    D3DMath_MatrixMultiply( matWorld, matWorld, matSpinY );
    D3DMath_MatrixMultiply( matWorld, matWorld, matSpinX );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );

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
    // Clear the viewport
    pvViewport->Clear2( 1UL, prcViewRect, D3DCLEAR_TARGET, 0x000000ff, 1.0f, 0L );

    // Begin the scene 
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        // Display the object
        pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                   g_pCubeVertices, NUM_CUBE_VERTICES, 
                                   g_pCubeIndices,  NUM_CUBE_INDICES, NULL );

        // End the scene.
        pd3dDevice->EndScene();
    }

    // Output the texture's pixel formats to the frame buffer
    CHAR buffer[80];
    sprintf( buffer, "TEXTURE%d.DDS ( %s as %s )", g_dwDXT,
                     g_strDiskPixelFormat, g_strRendererPixelFormat );
    AppOutputText( pd3dDevice, 0, 20, buffer );

    if (strlen(g_strError) > 0)
        AppOutputText( pd3dDevice, 0, 40, g_strError );

    return S_OK;
}





//-----------------------------------------------------------------------------
// Name: App_ChangeTexture()
// Desc: Frees the old texture and loads a new one.
//-----------------------------------------------------------------------------
HRESULT App_ChangeTexture()
{
    CHAR  strPath[256];
    CHAR  strTextureName[256];
    HKEY  key;
    DWORD type, size = 512;

    // Release the old texture
    SAFE_RELEASE( g_ptexTexture );

    // Get the media path from the registry
    if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, 
                                       TEXT("Software\\Microsoft\\DirectX"),
                                       0, KEY_READ, &key ) )
    {
        if( ERROR_SUCCESS == RegQueryValueEx( key, TEXT("DX6SDK Samples Path"),
                                         NULL, &type, (BYTE*)strPath, &size ) )
        {
            strcat( strPath, TEXT("\\D3DIM\\Media\\") );
        }
        RegCloseKey( key );
    }

    // Build the mipmap device surfaces and textures.
    sprintf( strTextureName, "%sTEXTURE%d.DDS", strPath, g_dwDXT );
    if( FAILED( LoadTexture( strTextureName, g_pd3dDevice, &g_ptexTexture ) ) )
    {
        strcpy( g_strError, "Couldn't load and/or blit compressed texture file!");
    }
    else
    {
        strcpy(g_strError, "");
    }

    g_pd3dDevice->SetTexture( 0, g_ptexTexture );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3 pd3dDevice,
                               LPDIRECT3DVIEWPORT3 pvViewport )
{
    HRESULT hr;

    // Check parameters
    if( NULL==pd3dDevice || NULL==pvViewport )
        return E_INVALIDARG;

    // Store away the D3DDevice ptr so the texture functions can use it
    g_pd3dDevice = pd3dDevice;

	// Get a ptr to the ID3D object to create materials and/or lights. Note:
	// the Release() call just serves to decrease the ref count.
    LPDIRECT3D3 pD3D;
    if( FAILED( pd3dDevice->GetDirect3D( &pD3D ) ) )
        return E_FAIL;
    pD3D->Release();

    // Create and set up the object material
    if( FAILED( hr = pD3D->CreateMaterial( &g_pmtrlObjectMtrl, NULL ) ) )
        return E_FAIL;
    D3DMATERIAL       mtrl;
    D3DMATERIALHANDLE hmtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    g_pmtrlObjectMtrl->SetMaterial( &mtrl );
    g_pmtrlObjectMtrl->GetHandle( pd3dDevice, &hmtrl );
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_MATERIAL, hmtrl );

    // Set the transform matrices
    D3DVECTOR vEyePt    = D3DVECTOR( 0, 0, -6.5 );
    D3DVECTOR vLookatPt = D3DVECTOR( 0, 0,   0  );
    D3DVECTOR vUpVec    = D3DVECTOR( 0, 1,   0  );
    D3DMATRIX matWorld, matView, matProj;

    D3DUtil_SetIdentityMatrix( matWorld );
    D3DUtil_SetViewMatrix( matView, vEyePt, vLookatPt, vUpVec );
    D3DUtil_SetProjectionMatrix( matProj, g_PI/4, 1.0f, 1.0f, 100.0f );

    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW,       &matView );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

    // Set up the light
    if( FAILED( hr = pD3D->CreateLight( &g_pLight, NULL ) ) )
        return E_FAIL;
    D3DLIGHT light;
    D3DUtil_InitLight( light, D3DLIGHT_POINT, 0.0, 0.0, -12.0 );
    light.dcvColor.r = D3DVAL(0.9);
    light.dcvColor.g = D3DVAL(0.9);
    light.dcvColor.b = D3DVAL(0.9);
    light.dvAttenuation0 = 1.0f;
    g_pLight->SetLight( &light );
    pvViewport->AddLight( g_pLight );

    D3DDEVICEDESC d3dHALDesc;
    D3DDEVICEDESC d3dHELDesc;
    LPD3DPRIMCAPS pdpcTriCaps;

    ZeroMemory( &d3dHALDesc, sizeof(D3DDEVICEDESC) );
    ZeroMemory( &d3dHELDesc, sizeof(D3DDEVICEDESC) );

    d3dHALDesc.dwSize  = sizeof( D3DDEVICEDESC );
    d3dHELDesc.dwSize  = sizeof( D3DDEVICEDESC );

    pd3dDevice->GetCaps( &d3dHALDesc, &d3dHELDesc );

    // get triangle caps from hardware or software description 
    if( 0 != d3dHALDesc.dwFlags )
	    pdpcTriCaps = &d3dHALDesc.dpcTriCaps;
    else
	    pdpcTriCaps = &d3dHELDesc.dpcTriCaps;

	// check if device supports mipmaping 
	if( pdpcTriCaps->dwTextureFilterCaps & D3DPTFILTERCAPS_MIPNEAREST | 
         pdpcTriCaps->dwTextureFilterCaps & D3DPTFILTERCAPS_MIPLINEAR )
        g_bSupportsMipmaps = TRUE;
    else
        g_bSupportsMipmaps = FALSE;

    // Set texture states
    App_ChangeTexture();

    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
	
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
    SAFE_RELEASE( g_ptexTexture );
    SAFE_RELEASE( g_pLight );
    SAFE_RELEASE( g_pmtrlObjectMtrl );
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




//-----------------------------------------------------------------------------
// Name: GetNumberOfBits()
// Desc: Returns the number of bits set in a DWORD mask
//-----------------------------------------------------------------------------
WORD GetNumberOfBits( DWORD dwMask )
{
    WORD wBits = 0;

    while( dwMask )
    {
        dwMask = dwMask & ( dwMask - 1 );  
        wBits++;
    }
    return wBits;
}




//-----------------------------------------------------------------------------
// Name: PixelFormatToString()
// Desc: Creates a string describing a pixel format.
//-----------------------------------------------------------------------------
VOID PixelFormatToString( CHAR* strPixelFormat, DDPIXELFORMAT* pddpf )
{
    char szTemp[50] = "";

    switch( pddpf->dwFourCC )
    {
        case 0:
            if( pddpf->dwBBitMask & DDPF_ALPHAPREMULT )
                strcpy( szTemp, "-premul" );

            // This dds texture isn't compressed so write out ARGB format
            sprintf( strPixelFormat, "ARGB-%d%d%d%d%s", 
                     GetNumberOfBits( pddpf->dwRGBAlphaBitMask ), 
                     GetNumberOfBits( pddpf->dwRBitMask ),
                     GetNumberOfBits( pddpf->dwGBitMask ),
                     GetNumberOfBits( pddpf->dwBBitMask ),
                     szTemp );
            break;

        case MAKEFOURCC('D','X','T','1'):
            strcpy( strPixelFormat, "DXT1" );
            break;

        case MAKEFOURCC('D','X','T','2'):
            strcpy( strPixelFormat, "DXT2" );
            break;

        case MAKEFOURCC('D','X','T','3'):
            strcpy( strPixelFormat, "DXT3" );
            break;

        case MAKEFOURCC('D','X','T','4'):
            strcpy( strPixelFormat, "DXT4" );
            break;

        case MAKEFOURCC('D','X','T','5'):
            strcpy( strPixelFormat, "DXT5" );
            break;
    }
}




//-----------------------------------------------------------------------------
// Name: ReadDDSTexture()
// Desc: Reads a DDS texture format from disk given a filename.
//       ppCompTop contains the DDS surface, and 
//       pddsdComp contains the DDS surface description
//-----------------------------------------------------------------------------
HRESULT ReadDDSTexture( CHAR* strTextureName, LPDIRECTDRAW4 pDD, 
                        DDSURFACEDESC2* pddsdComp, 
                        LPDIRECTDRAWSURFACE4* ppddsCompTop )
{
    HRESULT              hr;
    LPDIRECTDRAWSURFACE4 pddsTop      = NULL;
    LPDIRECTDRAWSURFACE4 pdds         = NULL;
    LPDIRECTDRAWSURFACE4 pddsAttached = NULL;
    DDSURFACEDESC2       ddsd;
    DWORD                dwMagic;

    hr = E_FAIL;

    // open the compressed texture file
    FILE* file = fopen( strTextureName, "rb" );
    if( file == NULL )
        return E_FAIL;

    // read magic number
    fread( &dwMagic, sizeof(DWORD), 1, file );
    if( dwMagic != MAKEFOURCC('D','D','S',' ') )
        goto LFail;

    // read the surface description
    fread( pddsdComp, sizeof(DDSURFACEDESC2), 1, file );

    // mask/set surface caps appropriately for app
    pddsdComp->ddsCaps.dwCaps2 |= DDSCAPS2_TEXTUREMANAGE;

    // handle special case for hardware that doesn't support mipmaping
    if( !g_bSupportsMipmaps )
    {
        pddsdComp->dwMipMapCount = 0;
        pddsdComp->dwFlags &= ~DDSD_MIPMAPCOUNT;
        pddsdComp->ddsCaps.dwCaps &= ~( DDSCAPS_MIPMAP | DDSCAPS_COMPLEX );
    }

    // does texture have mipmaps?
    if( pddsdComp->dwMipMapCount == 0 )
        g_bMipTexture = FALSE;
    else
        g_bMipTexture = TRUE;

    // create a new surface based on the surface description
    if( FAILED( hr = pDD->CreateSurface( pddsdComp, ppddsCompTop, NULL ) ) )
        goto LFail;

    pddsTop = *ppddsCompTop;

    pdds = pddsTop;
    pdds->AddRef();

    while( TRUE )
    {
        ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
        ddsd.dwSize = sizeof(DDSURFACEDESC2);

        if( FAILED( hr = pdds->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL )))
            goto LFail;

        if( ddsd.dwFlags & DDSD_LINEARSIZE )
        {
            fread( ddsd.lpSurface, ddsd.dwLinearSize, 1, file );
        }
        else
        {
            DWORD yp;
            BYTE* pbDest = (BYTE*)ddsd.lpSurface;
            LONG dataBytesPerRow = ddsd.dwWidth * ddsd.ddpfPixelFormat.dwRGBBitCount / 8;
            for( yp = 0; yp < ddsd.dwHeight; yp++ )
            {
                fread( pbDest, dataBytesPerRow, 1, file );
                pbDest += ddsd.lPitch;
            }
        }

        pdds->Unlock( NULL );

        if( !g_bSupportsMipmaps )
        {
            // for mipless hardware, don't copy mipmaps
            pdds->Release();
            break;
        }

        ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;

        if( FAILED( hr = pdds->GetAttachedSurface( &ddsd.ddsCaps, &pddsAttached ) ) )
        {
            pdds->Release();
            break;
        }

        pdds->Release();
        pdds = pddsAttached;
    }

    // create string descriptor of pixel format of
    // the texture on disk.  This is used in AppShowStats()
    PixelFormatToString( g_strDiskPixelFormat, &pddsdComp->ddpfPixelFormat );

    hr = S_OK;  // everything worked

LFail:
    fclose( file );

    return hr;
}




//-----------------------------------------------------------------------------
// Name: EnumTextureFormats()
// Desc: Callback fn for enumerating the pixel formats the current renderer 
//       supports.  Enumerated pixel formats are collected in a linked list
//       The head for this linked list is in (PixelFormatNode*)pUserArg->pNext. 
//-----------------------------------------------------------------------------
HRESULT WINAPI EnumTextureFormats( DDPIXELFORMAT* pPixelFormat,
                                   VOID* pUserArg )
{
    PixelFormatNode** ppHead = (PixelFormatNode**)pUserArg; 
    PixelFormatNode* pNode = NULL;

    // create a new node 
    pNode = new PixelFormatNode;
    if( NULL == pNode )
        return DDENUMRET_CANCEL;
    
    // insert new node at beginning of list
    pNode->pNext = *ppHead;
    *ppHead = pNode;

    // fill up node info
    pNode->ddpf = *pPixelFormat;
    pNode->nAlphaBits = GetNumberOfBits( pPixelFormat->dwRGBAlphaBitMask );
    pNode->nRedBits   = GetNumberOfBits( pPixelFormat->dwRBitMask );
    pNode->nGreenBits = GetNumberOfBits( pPixelFormat->dwGBitMask );
    pNode->nBlueBits  = GetNumberOfBits( pPixelFormat->dwBBitMask );
    pNode->bPremultipliedAlpha = pPixelFormat->dwFlags & DDPF_ALPHAPREMULT;

    // continue enumerating all supported pixel formats
    return DDENUMRET_OK;
}




//-----------------------------------------------------------------------------
// Name: FindBestPixelFormatMatch()
// Desc: Given a pixel format from a compressed surface, it finds the best
//       pixel format match format that is supported by the current
//       renderer.  pddsdBestMatch contains the best match found.
//-----------------------------------------------------------------------------
HRESULT FindBestPixelFormatMatch( LPDIRECT3DDEVICE3 pd3dDevice, 
                                  DDPIXELFORMAT     ddsdDDSTexture, 
                                  DDPIXELFORMAT*    pddsdBestMatch )
{
    HRESULT          hr;
    PixelFormatNode* pHead          = NULL;
    PixelFormatNode* pNode          = NULL;
    PixelFormatNode* pGoodMatchNode = NULL;
    int              nCompAlphaBits;
    int              nHighestFound = 0;
    BOOL             bCompressedTexture; 
    BOOL             bPremultipliedAlpha;

    bCompressedTexture = TRUE; // assume true

    // set how many alpha bits are in the compressed texture 
    switch ( ddsdDDSTexture.dwFourCC )
    {
        case 0:
            // this dds texture isn't compressed so we need an
            // exact pixel format match to render this surface
            // (or do a manual pixel conversion)
            bCompressedTexture = FALSE;  
            break;

        case MAKEFOURCC('D','X','T','1'):
            nCompAlphaBits = 1;
            bPremultipliedAlpha = FALSE;
            break;

        case MAKEFOURCC('D','X','T','2'):
            nCompAlphaBits = 4;
            bPremultipliedAlpha = TRUE;
            break;

        case MAKEFOURCC('D','X','T','3'):
            nCompAlphaBits = 4;
            bPremultipliedAlpha = FALSE;
            break;

        case MAKEFOURCC('D','X','T','4'):
            nCompAlphaBits = 8;
            bPremultipliedAlpha = TRUE;
            break;

        case MAKEFOURCC('D','X','T','5'):
            nCompAlphaBits = 8;
            bPremultipliedAlpha = FALSE;
            break;
    }

    // pixelFormatEnum is just a placeholder.
    // pixelFormatEnum.pNext will start the list
    if( FAILED( hr = pd3dDevice->EnumTextureFormats( EnumTextureFormats, 
                                                     (VOID*)&pHead ) ) )
        return hr;

    if( !bCompressedTexture )
    {
        // if the texture isn't compressed then look for an exact
        // pixel format match, otherwise fail since this sample
        // doesn't implement any manual pixel format conversion 
        // algorithms.
        int nTextureABits = GetNumberOfBits( ddsdDDSTexture.dwRGBAlphaBitMask );
        int nTextureRBits = GetNumberOfBits( ddsdDDSTexture.dwRBitMask );
        int nTextureGBits = GetNumberOfBits( ddsdDDSTexture.dwGBitMask );
        int nTextureBBits = GetNumberOfBits( ddsdDDSTexture.dwBBitMask );

        pGoodMatchNode = NULL;

        // run through list looking for an exact match
        pNode = pHead;

        while( NULL != pNode )
        {
            if( pNode->ddpf.dwFourCC == 0             &&
                pNode->nAlphaBits    == nTextureABits &&
                pNode->nRedBits      == nTextureRBits &&
                pNode->nGreenBits    == nTextureGBits &&
                pNode->nBlueBits     == nTextureBBits ) 
            {
                // this is an exact pixel format match, so it works
                pGoodMatchNode = pNode;
                break;
            }

            pNode = pNode->pNext; // advance along list
        }

        // pGoodMatchNode will be NULL if no exact match found, 
        // and since this is an uncompressed DDS texture format 
        // the blt can not convert between pixel formats.  
        // A manual conversion of the pixels could be done, but 
        // this is not implemeneted in this sample
    }
    else
    {
        // search for an exact pixel format match 
        // if renderer supports compressed textures 
        pNode = pHead;
        while( NULL != pNode )
        {
            if( pNode->ddpf.dwFourCC == ddsdDDSTexture.dwFourCC )
            {
                // look no further, since this is the best possible match
                pGoodMatchNode = pNode;
                break;
            }

            pNode = pNode->pNext; // advance along list
        }

        // if a good match was not found then keep looking
        if( NULL == pGoodMatchNode ) 
        {
            // search for exact or highest alpha bit rate match 
            // and also make sure the texture isn't blitted from
            // premultipled alpha to non-premultipled alpha or visa-versa
            pNode = pHead;
            nHighestFound = -1;
            pGoodMatchNode = NULL;

            while( NULL != pNode )
            {
                if( pNode->nAlphaBits          == nCompAlphaBits &&
                    pNode->bPremultipliedAlpha == bPremultipliedAlpha &&
                    pNode->nRedBits            >= 4 &&
                    pNode->nGreenBits          >= 4 &&
                    pNode->nBlueBits           >= 4 ) 
                {
                    // look no further, since this is the next best possible match
                    pGoodMatchNode = pNode;
                    break;
                }

                if( pNode->nAlphaBits          >  nHighestFound &&
                    pNode->bPremultipliedAlpha == bPremultipliedAlpha &&
                    pNode->nRedBits            >= 4 &&
                    pNode->nGreenBits          >= 4 &&
                    pNode->nBlueBits           >= 4 ) 
                {
                    nHighestFound = pNode->nAlphaBits;
                    pGoodMatchNode = pNode;
                    // keep looking for a better match
                }

                pNode = pNode->pNext; // advance along list
            }
        }
    }

    // if no match was found then either no texture pixel formats 
    // are supported by the renderer or this in an uncompressed
    // pixel format and an exact pixel format match was not found
    if( NULL == pGoodMatchNode ) 
    {
        hr = E_FAIL;
        goto LFail; // delete linked list
    }

    // choose the highest alpha rate possible as the best match
    *pddsdBestMatch = pGoodMatchNode->ddpf;  

    // create string descriptor of the best match found 
    // this is used in AppShowStats().
    PixelFormatToString( g_strRendererPixelFormat, &pGoodMatchNode->ddpf );

LFail:
    // delete the nodes of the linked list
    while( NULL != pHead )  // while more in list, keep deleting
    {
        pNode = pHead; 
        pHead = pHead->pNext;
        delete pNode;
    }

    return hr;
}




//-----------------------------------------------------------------------------
// Name: BltToUncompressedSurface()
// Desc: Creates an uncompressed surface and blits the compressed surface to 
//       it using the specified pixel format.
//-----------------------------------------------------------------------------
HRESULT BltToUncompressedSurface( LPDIRECTDRAW4 pDD, DDSURFACEDESC2 ddsd, 
                                  DDPIXELFORMAT ddpf, 
                                  LPDIRECTDRAWSURFACE4 pddsDXT, 
                                  LPDIRECTDRAWSURFACE4* ppddsNewSurface )
{
    LPDIRECTDRAWSURFACE4 pddsDXTAttached;
    LPDIRECTDRAWSURFACE4 pddsNew;
    LPDIRECTDRAWSURFACE4 pddsNewAttached;

    // Set surface caps for the new surface
    ddsd.ddpfPixelFormat = ddpf;

    // create an un-compressed surface based on the enumerated texture formats
    if( FAILED( pDD->CreateSurface( &ddsd, &pddsNew, NULL ) ) )
        return E_FAIL;
    *ppddsNewSurface = pddsNew;

    // Copy compress image to un-compressed surface, including mips (if any)
    while( TRUE )
    {
        if( FAILED( pddsNew->Blt( NULL, pddsDXT, NULL, DDBLT_WAIT, NULL ) ) )
            return E_FAIL;

        // Get next surface in DXT's mipmap chain
        ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
        if( FAILED( pddsDXT->GetAttachedSurface( &ddsd.ddsCaps, &pddsDXTAttached )))
        {
            // Failure here means were done with the mipmap chain
            return S_OK;
        }
        pddsDXT = pddsDXTAttached;  

        // Get next surface in the new surface's mipmap chain
        if( FAILED( pddsNew->GetAttachedSurface( &ddsd.ddsCaps, &pddsNewAttached )))
            return E_FAIL;
        pddsNew = pddsNewAttached;
    }
}




//-----------------------------------------------------------------------------
// Name: LoadTexture()
// Desc: Creates the device-dependant surface and loads a DDS texture 
//-----------------------------------------------------------------------------
HRESULT LoadTexture( CHAR* strTextureName, 
                     LPDIRECT3DDEVICE3 pd3dDevice, 
                     LPDIRECT3DTEXTURE2* pptexTexture )
{
    HRESULT              hr;
    LPDIRECTDRAW4        pDD        = NULL;
    LPDIRECTDRAWSURFACE4 pDDSNewTop = NULL;
    LPDIRECTDRAWSURFACE4 pDDSDXTTop = NULL;
    LPDIRECTDRAWSURFACE4 pddsRenderTarget = NULL;
    DDSURFACEDESC2       ddsdComp;
    DDPIXELFORMAT        ddpfBestMatch;

    // Get the render target surface
    if( FAILED( hr = pd3dDevice->GetRenderTarget( &pddsRenderTarget ) ) )
        goto LFail;

    // Get a DDraw ptr (from render target) for creating surfaces
    if( FAILED( hr = pddsRenderTarget->GetDDInterface( (VOID**)&pDD ) ) )
        goto LFail;

    // Create a DDS texture surface based on the dds file
    // this surface may or may not be compressed
    if( FAILED( hr = ReadDDSTexture( strTextureName, pDD, &ddsdComp,
                                     &pDDSDXTTop ) ) )
        goto LFail;

    // enumerate all pixel formats, then choose the best match
    // based on the read-in DDS texture format
    if( FAILED( hr = FindBestPixelFormatMatch( pd3dDevice, 
                                               ddsdComp.ddpfPixelFormat, 
                                               &ddpfBestMatch ) ) )
        goto LFail;

    if( ddpfBestMatch.dwFlags & DDPF_ALPHAPREMULT )
    {
        // Use D3DBLEND_ONE if DDPF_ALPHAPREMULT is on
        pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE );
    }
    else
    {
        // Use D3DBLEND_SRCALPHA if DDPF_ALPHAPREMULT is off
        pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA );
    }

    // Does the renderer support the compress texture format or 
    // is the dds texture already uncompressed? 
    if( ddsdComp.ddpfPixelFormat.dwFourCC != ddpfBestMatch.dwFourCC )
    {
        // blt the compressed surface to an uncompressed 
        // surface using the best pixel format match
        if( FAILED( hr = BltToUncompressedSurface( pDD, ddsdComp, ddpfBestMatch, 
                                                   pDDSDXTTop, &pDDSNewTop ) ) )
            goto LFail;

        // get the texture interface from the 
        // new uncompressed texture surface 
        if( FAILED( hr = pDDSNewTop->QueryInterface( IID_IDirect3DTexture2, 
                                                    (VOID**)pptexTexture ) ) )
            goto LFail;
    }
    else
    {
        // don't uncompress texture since renderer 
        // natively supports this pixel format
        if( FAILED( hr = pDDSDXTTop->QueryInterface( IID_IDirect3DTexture2, 
                                                    (VOID**)pptexTexture ) ) )
            goto LFail;
    }

LFail:

    if( pDDSNewTop )       pDDSNewTop->Release();
    if( pDDSDXTTop )       pDDSDXTTop->Release();
    if( pddsRenderTarget ) pddsRenderTarget->Release();
    if( pDD )              pDD->Release();

    return hr;
}




//-----------------------------------------------------------------------------
// Name: App_OverridenWndProc()
// Desc: App-specific message proc
//-----------------------------------------------------------------------------
LRESULT CALLBACK App_OverridenWndProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                       LPARAM lParam )
{
    if( WM_COMMAND == uMsg )
    {
        switch( LOWORD(wParam) )
        {
            case IDM_CHANGEDXT:
                // Recieved command to cycle the texture
                g_dwDXT = (g_dwDXT+1) % 4; // Increment and wrap counter 
                App_ChangeTexture();
                return 0;
        }
    }

    return WndProc( hWnd, uMsg, wParam, lParam );
}





