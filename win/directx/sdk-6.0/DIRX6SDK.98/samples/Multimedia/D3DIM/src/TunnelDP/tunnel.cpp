//-----------------------------------------------------------------------------
// File: Tunnel.cpp
//
// Desc: Sample code used to demonstrate the programming differences between
//       using D3D execute buffers and using D3D draw primitives. This version
//       uses draw primitives.
// 
//       This code shows the camera traversing through a texture-mapped tunnel.
//       The tunnel is composed of many segments centered along a spline. Only
//       a finite number of segments are kept in memory: as the camera moves
//       past a segment, it is reused to become the segment at the end of the
//       piece of the tunnel.
//
//       Note: This code uses the D3D Framework helper library.
//
//
// Copyright (c) 1997-1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define  STRICT
#define  D3D_OVERLOADS
#include <math.h>
#include <time.h>
#include <stdio.h>
#include "D3DUtil.h"
#include "D3DMath.h"
#include "resource.h"




//-----------------------------------------------------------------------------
// Declare the application globals for use in WinMain.cpp
//-----------------------------------------------------------------------------
TCHAR* g_strAppTitle       = TEXT( "Tunnel: A Direct3D Draw Primitives example" );
BOOL   g_bAppUseZBuffer    = FALSE;
BOOL   g_bAppUseBackBuffer = TRUE;




//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK App_OverridenWndProc( HWND, UINT, WPARAM, LPARAM );
VOID    SetMenuStates();

VOID    MakeSegment( WORD, WORD, WORD* );
VOID    MakeRing( const D3DVECTOR&, const D3DVECTOR&, const D3DVECTOR&,
                  FLOAT, D3DVERTEX* );
VOID    MoveToPosition( FLOAT, D3DVECTOR*, D3DVECTOR*, D3DVECTOR* );
VOID    UpdateTubeInMemory();
VOID    PositionCamera( D3DMATRIX*, D3DVECTOR*, D3DVECTOR*, D3DVECTOR* );




//-----------------------------------------------------------------------------
// Name: Tunnel
// Desc: Data structure for the tunnel
//-----------------------------------------------------------------------------
struct Tunnel
{
    D3DVERTEX*   pvVertices;  // Tunnel vertices
    WORD*        pwIndices;   // Tunnel triangles for its segments
    D3DVECTOR*   pvPoints;    // Points defining the spline curve
    D3DMATERIALHANDLE hMtrl;  // Handle for the material on the tube. 
    D3DTEXTUREHANDLE  hTex;   // Handle for the texture on the material.
    D3DLIGHT          light;             // Structure defining the light. 
    LPDIRECT3DLIGHT   pLight; // Object pointer for the light. 
    D3DVECTOR    vCameraP;    // Vectors defining the camera position
    D3DVECTOR    vCameraD;    // camera direction
    D3DVECTOR    vCameraN;    // camera up
    FLOAT        fCameraPos;  // Camera position along spline curve
    D3DVECTOR    vEndP, vEndD, vEndN; // Vectors defining the position, 
                                      // direction and up at the foremost end of
                                      // the section in memory.
    FLOAT        fEndPos;     // Position along the spline curve of the end.
    WORD wCurrentRing, wCurrentSegment; // Numbers of the ring and tunnel at
                                     // the back end of the section.
} g_Tunnel;




//-----------------------------------------------------------------------------
// Texture support
//-----------------------------------------------------------------------------
struct TextureContainer
{
    HBITMAP              hbmBitmap;   // Bitmaps containing images
    LPDIRECT3DDEVICE3    pd3dDevice;  // Device used to create texture
    LPDIRECTDRAWSURFACE4 pddsSurface; // Texture surface 
    LPDIRECT3DTEXTURE2   ptexTexture; // Texture object
};

TextureContainer*    g_pTextureContainer = NULL;  // The main texture

HRESULT Texture_Create( CHAR*, TextureContainer** );
HRESULT Texture_Restore( TextureContainer* );
HRESULT Texture_CopyBitmapToSurface( TextureContainer* );
VOID Texture_Invalidate( TextureContainer* );
VOID Texture_Delete( TextureContainer* );




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
LPDIRECT3DMATERIAL3 g_pmtrlTunnelMtrl     = NULL;
D3DMATRIX           g_matWorld, g_matView, g_matProj;
HMENU               g_hMenu;

// User selectable rendering options
BOOL                g_bSpecularEnabled    = FALSE;
BOOL                g_bPerspectiveCorrect = FALSE;
D3DTEXTUREFILTER    g_dwFilterType        = D3DFILTER_LINEAR;
D3DSHADEMODE        g_dwShading           = D3DSHADE_GOURAUD;
D3DANTIALIASMODE    g_dwAntialias         = D3DANTIALIAS_NONE;
BOOL                g_bFogEnabled         = FALSE;
BOOL                g_bDitherEnabled      = FALSE;


#define NUM_SEGMENTS      20 // # of segments in memory at one time.  Each
                             // segment is made up of triangles spanning
                             // between two rings.
#define NUM_SIDES         24 // # of sides on each ring.
#define NUM_TEX_RINGS     5  // # of rings to stretch the texture over.
#define NUM_VERTICES  (NUM_SIDES*(NUM_SEGMENTS+1)) // # of vertices in memory
#define NUM_TRIANGLES (NUM_SIDES*NUM_SEGMENTS*2)   // # of triangles in memory
#define TUNNEL_RADIUS   1.0f  // Radius of the tunnel.
#define NUM_SPLINE_POINTS 50  // Number of spline points to initially
                              // calculate.  The section in memory represents
                              // only a fraction of this.

// Movement and track scalars given in terms of position along the spline
// curve.
#define SEGMENT_LENGTH 0.05f  // Length of each segment along curve.
#define SPEED          0.02f  // Amount to increment camera position along
                              // curve for each frame.
#define DEPTH          0.8f   // How close the camera can get to the end of
                              // track before new segments are added.
#define PATH_LENGTH ((FLOAT)(NUM_SPLINE_POINTS - 1)) //Total length of the tunnel.



//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{
    // Add a menu and a message handler to the program
    g_hMenu = LoadMenu( NULL, MAKEINTRESOURCE(IDR_MENU) );
    SetMenu( hWnd, g_hMenu );
    SetWindowLong( hWnd, GWL_WNDPROC, (LONG)App_OverridenWndProc );

    //Reserved memory for vertices, triangles and spline points.
    g_Tunnel.pvVertices = new D3DVERTEX[ NUM_VERTICES ];
    g_Tunnel.pwIndices  = new WORD[ 3*NUM_TRIANGLES ];
    g_Tunnel.pvPoints   = new D3DVECTOR[ NUM_SPLINE_POINTS ];

    if( NULL==g_Tunnel.pvVertices || NULL==g_Tunnel.pwIndices || 
        NULL==g_Tunnel.pvPoints )
        return E_OUTOFMEMORY;

    FLOAT fPosition = 0.0f; // Curve position counter.
    WORD  i;                // counter

    // Generate spline points
    for( i=0; i<NUM_SPLINE_POINTS; i++ )
    {
        g_Tunnel.pvPoints[i].x = (FLOAT)(cos(i * 4.0) * 20.0);
        g_Tunnel.pvPoints[i].y = (FLOAT)(sin(i * 4.0) * 20.0);
        g_Tunnel.pvPoints[i].z = i * 20.0f;
    }
     //Create the initial tube section in memory.
    g_Tunnel.vEndN = D3DVECTOR( 0.0f, 1.0f, 0.0f );
    
    for( i=0; i<NUM_SEGMENTS+1; i++ )
    {
        MoveToPosition( fPosition, &g_Tunnel.vEndP, &g_Tunnel.vEndD, &g_Tunnel.vEndN );
        fPosition += (FLOAT)SEGMENT_LENGTH;
        MakeRing( g_Tunnel.vEndP, g_Tunnel.vEndD, g_Tunnel.vEndN, 
                 (FLOAT)(i % NUM_TEX_RINGS) / NUM_TEX_RINGS,
                 &g_Tunnel.pvVertices[(NUM_SEGMENTS - i) * NUM_SIDES]);
    }
    for( i=0; i<NUM_SEGMENTS; i++ )
        MakeSegment(i + 1, i, &g_Tunnel.pwIndices[ 3 * i * NUM_SIDES * 2 ]);

    // Move the camera to the begining and set some globals
    g_Tunnel.vCameraN = D3DVECTOR( 0.0f, 1.0f, 0.0f );
    MoveToPosition( 0.0f, &g_Tunnel.vCameraP, &g_Tunnel.vCameraD, &g_Tunnel.vCameraN);
    g_Tunnel.wCurrentRing    = 0;
    g_Tunnel.wCurrentSegment = 0;
    g_Tunnel.fCameraPos      = 0.0f;
    g_Tunnel.fEndPos         = fPosition;

    // Load in tunnel texture
    if( FAILED( Texture_Create( "TEX_IMAGE", &g_pTextureContainer ) ) )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT App_FrameMove( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{
    // Move the camera through the tunnel.  Create new segments of the tunnel
    // when the camera gets close to the end of the section in memory.
    g_Tunnel.fCameraPos += SPEED;
    if( g_Tunnel.fCameraPos > PATH_LENGTH )
        g_Tunnel.fCameraPos -= PATH_LENGTH;

    MoveToPosition( g_Tunnel.fCameraPos, &g_Tunnel.vCameraP,
                    &g_Tunnel.vCameraD, &g_Tunnel.vCameraN );

    // If the camera is close to the end, add a new segment.
    if( ( g_Tunnel.fEndPos - g_Tunnel.fCameraPos ) < DEPTH ) 
    {
        g_Tunnel.fEndPos += SEGMENT_LENGTH;
        if( g_Tunnel.fEndPos > PATH_LENGTH )
            g_Tunnel.fEndPos -= PATH_LENGTH;
        UpdateTubeInMemory();
    }

    // Move the camera and the light
    PositionCamera( &g_matView, &g_Tunnel.vCameraP, &g_Tunnel.vCameraD,
                    &g_Tunnel.vCameraN );
    g_Tunnel.light.dvPosition = g_Tunnel.vCameraP;

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
    // Restore our texture, if it's surface was lost
    if( g_pTextureContainer->pddsSurface->IsLost() )
    {
        g_pTextureContainer->pddsSurface->Restore();
        Texture_CopyBitmapToSurface( g_pTextureContainer );
    }

    // No need to clear the viewport since we redraw over every pixel

    // Begin the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        // Update the camera and the light
        pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &g_matView );
        g_Tunnel.pLight->SetLight( &g_Tunnel.light );

        // Set the user-selectable render states
        if( D3DFILTER_LINEAR == g_dwFilterType )
        {
            pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
            pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
        }
        else
        {
            pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_POINT );
            pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_POINT );
        }
        pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, g_bSpecularEnabled );
        pd3dDevice->SetRenderState( D3DRENDERSTATE_TEXTUREPERSPECTIVE, g_bPerspectiveCorrect );
        pd3dDevice->SetRenderState( D3DRENDERSTATE_SHADEMODE,    g_dwShading );
        pd3dDevice->SetRenderState( D3DRENDERSTATE_ANTIALIAS,    g_dwAntialias );
        pd3dDevice->SetRenderState( D3DRENDERSTATE_FOGENABLE,    g_bFogEnabled );
        pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, g_bDitherEnabled );

        // Draw the tunnel using draw primitives
        while( DDERR_WASSTILLDRAWING == pd3dDevice->DrawIndexedPrimitive(
			                               D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                           g_Tunnel.pvVertices, NUM_VERTICES,
                                           g_Tunnel.pwIndices, 3*NUM_TRIANGLES,
									       NULL ) )
		{
			// Loop while waiting for the DrawPrim() call to succeed. We could
			// use this time to do some other processing.
		}

        // End the scene.
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
    SAFE_DELETE( g_Tunnel.pvVertices );
    SAFE_DELETE( g_Tunnel.pwIndices );
    SAFE_DELETE( g_Tunnel.pvPoints );

    Texture_Delete( g_pTextureContainer );

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
    Texture_Invalidate( g_pTextureContainer );

    SAFE_RELEASE( g_Tunnel.pLight );
    SAFE_RELEASE( g_pmtrlTunnelMtrl );
}




//-----------------------------------------------------------------------------
// Name: App_RestoreSurfaces
// Desc: Restores any previously lost surfaces. Must do this for all surfaces
//       (including textures) that the app created.
//-----------------------------------------------------------------------------
HRESULT App_RestoreSurfaces()
{
    // Note: we are checking for our lost texture surface in the render loop.
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_ConfirmDevice()
// Desc: Called during device intialization, this code checks to see if
//       the device can support some minimum capabilites required by this demo.
//-----------------------------------------------------------------------------
HRESULT App_ConfirmDevice( DDCAPS* pddDriverCaps,
                           D3DDEVICEDESC* pd3dDeviceDesc )
{
    return S_OK;
}





//-----------------------------------------------------------------------------
// Name: BuildInvertedRotationMatrix()
// Desc: Set the rotation part of a matrix such that the vector pD is the new
//       z-axis and pU is the new y-axis.
//-----------------------------------------------------------------------------
D3DMATRIX* BuildInvertedRotationMatrix( D3DMATRIX* pM, D3DVECTOR* pD, 
                                        D3DVECTOR* pU )
{
    // Normalize the direction vector.
    D3DVECTOR d = Normalize( (*pD) );
    D3DVECTOR u = (*pU);

    // Project u into the plane defined by d and normalise.
    u = Normalize( u - d * DotProduct( u, d ) );

    // Calculate the vector pointing along the matrix x axis (in a right
    // handed coordinate system) using cross product.
    D3DVECTOR r = CrossProduct( u, d );

    // Build the matrix INVERTED (all elements are reversed)
    pM->_11 = r.x;  pM->_21 = r.y,  pM->_31 = r.z;
    pM->_12 = u.x;  pM->_22 = u.y,  pM->_32 = u.z;
    pM->_13 = d.x;  pM->_23 = d.y;  pM->_33 = d.z;
    pM->_14 = 0.0f; pM->_24 = 0.0f; pM->_34 = 0.0f; pM->_44 = 1.0f;

    return pM;
}




//-----------------------------------------------------------------------------
// Name: PositionCamera()
// Desc: Creates a matrix which is equivalent to having the camera at a
//       specified position. This matrix can be used to convert vertices to
//       camera coordinates. Input parameters are the position of the camera,
//       the direction of the view, and the up vector.
//-----------------------------------------------------------------------------
VOID PositionCamera( D3DMATRIX* pM, D3DVECTOR* pP, D3DVECTOR* pD,
                     D3DVECTOR* pN )
{
    // Set the rotation part of the matrix and invert it. Vertices must be
    // inverse rotated to achieve the same result of a corresponding 
    // camera rotation.
    BuildInvertedRotationMatrix( pM, pD, pN );

    // Multiply the rotation matrix by a translation transform.  The
    // translation matrix must be applied first (left of rotation).
    pM->_41 = -( pM->_11 * pP->x + pM->_21 * pP->y + pM->_31 * pP->z );
    pM->_42 = -( pM->_12 * pP->x + pM->_22 * pP->y + pM->_32 * pP->z );
    pM->_43 = -( pM->_13 * pP->x + pM->_23 * pP->y + pM->_33 * pP->z );
}


  

//-----------------------------------------------------------------------------
// Name: spline()
// Desc: Calculates a point along a B-Spline curve defined by four points. The
//       parameter, t, is the parametric value along the spline function.
//-----------------------------------------------------------------------------
D3DVECTOR spline( FLOAT t, const LPD3DVECTOR p[4] )
{
    FLOAT     t2  = t * t;
    FLOAT     t3  = t * t * t;
    D3DVECTOR ret = D3DVECTOR( 0.0f, 0.0f, 0.0f );
    FLOAT     m[4];

    m[0] = ( 0.5f * ( (-1.0f * t3) + ( 2.0f * t2) + (-1.0f * t) ) );
    m[1] = ( 0.5f * ( ( 3.0f * t3) + (-5.0f * t2) + ( 0.0f * t) + 2.0f ) );
    m[2] = ( 0.5f * ( (-3.0f * t3) + ( 4.0f * t2) + ( 1.0f * t) ) );
    m[3] = ( 0.5f * ( ( 1.0f * t3) + (-1.0f * t2) + ( 0.0f * t) ) );

    for( WORD i=0; i<4; i++ )
        ret += *p[i]*m[i];

    return ret;
}




//-----------------------------------------------------------------------------
// Name: MoveToPosition()
// Desc: Updates the given position, direction and normal vectors to a given
//       position on the spline curve. The given up vector is used to
//       determine the new up vector.
//-----------------------------------------------------------------------------
VOID MoveToPosition( FLOAT fPosition, D3DVECTOR* pvP, D3DVECTOR* pvD, 
                     D3DVECTOR* pvN )
{
    D3DVECTOR* pvSplinePoint[4];
    WORD       point = 0;

    // Loop from back for get number of point at the camera (where pos==1.0f)
    for( FLOAT fPos = fPosition; fPos>1.0f; fPos-- )
    {
        if( ++point==NUM_SPLINE_POINTS )
            point = 0;
    }

    // Find the four points along the curve which are around the position.
    for( WORD j=0; j<4; j++ )
    {
        pvSplinePoint[j] = &g_Tunnel.pvPoints[point];
        if( ++point==NUM_SPLINE_POINTS )
            point = 0;
    }

    // Calculate the direction from the given position, and the position
    // just before it
    *pvP = spline( fPos, pvSplinePoint );
    *pvD = Normalize( *pvP - spline( fPos - 0.01f, pvSplinePoint ) );
    
    // Find the new normal.  This method will work provided the change in
    // the normal is not very large.
    *pvN = Normalize( CrossProduct( *pvD, CrossProduct( *pvN, *pvD ) ) );
}




//-----------------------------------------------------------------------------
// Name: MakeRing()
// Desc: Generates a ring of vertices in a plane defined by vN and the cross
//       product of vN and vP.  On exit, pvJoint contains the vertices. Normals
//       are generated pointing in.
//-----------------------------------------------------------------------------
VOID MakeRing( const D3DVECTOR& vP, const D3DVECTOR& vD, const D3DVECTOR& vN,
               FLOAT fTV, D3DVERTEX* pvJoint)
{
    D3DVECTOR vNxD = CrossProduct( vN, vD );

    for( WORD wSpoke = 0; wSpoke < NUM_SIDES; wSpoke++ )
    {
        FLOAT fTheta = (2.0f * g_PI) * wSpoke / NUM_SIDES;
         
        // x, y, z define a unit vector in standard coordiante space
        D3DVECTOR vPt = vNxD*(FLOAT)cos( fTheta ) + vN*(FLOAT)sin( fTheta );

        // Position, normals and texture coordiantes.
        pvJoint[wSpoke].x  = vP.x + vPt.x * TUNNEL_RADIUS;
        pvJoint[wSpoke].y  = vP.y + vPt.y * TUNNEL_RADIUS;
        pvJoint[wSpoke].z  = vP.z + vPt.z * TUNNEL_RADIUS;
        pvJoint[wSpoke].nx = -vPt.x;
        pvJoint[wSpoke].ny = -vPt.y;
        pvJoint[wSpoke].nz = -vPt.z;
        pvJoint[wSpoke].tu = 1.0f - fTheta / (2.0f * g_PI);
        pvJoint[wSpoke].tv = fTV;

    }
}




//-----------------------------------------------------------------------------
// Name: MakeSegment()
// Desc: Defines the triangles (or indices for triangles) which form a segment
//       between the two rings of the tunnel.
//-----------------------------------------------------------------------------
VOID MakeSegment( WORD  wRing1, WORD wRing2, WORD* pwIndices )
{
    for( WORD wSide=0, wTri=0; wSide<NUM_SIDES; wSide++ )
    {
        // Each side consists of two triangles.
        pwIndices[3*wTri+0] = wRing1 * NUM_SIDES + wSide;
        pwIndices[3*wTri+1] = wRing2 * NUM_SIDES + wSide;
        pwIndices[3*wTri+2] = wRing2 * NUM_SIDES + ((wSide + 1) % NUM_SIDES);
        wTri++;

        pwIndices[3*wTri+1] = wRing2 * NUM_SIDES + ((wSide + 1) % NUM_SIDES);
        pwIndices[3*wTri+2] = wRing1 * NUM_SIDES + ((wSide + 1) % NUM_SIDES);
        pwIndices[3*wTri+0] = wRing1 * NUM_SIDES + wSide;
        wTri++;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateTubeInMemory()
// Desc: Creates a new segment of the tunnel at the current end position.
//-----------------------------------------------------------------------------
VOID UpdateTubeInMemory()
{
    static WORD wTexRing = 0;         // Counter defining the position of
                                      // this ring on the texture.
    WORD wEndRing;                    // Ring at the end of the tunnel in mem
    WORD wRingOffset, wSegmentOffset; // Offsets into the vertex and triangle 
                                      // lists for the new data.

    // Replace the back ring with a new ring at the front of the tube
    // in memory.
    memmove( &g_Tunnel.pvVertices[NUM_SIDES], &g_Tunnel.pvVertices[0], 
            sizeof(D3DVERTEX) * (NUM_VERTICES - NUM_SIDES));
    
    MakeRing( g_Tunnel.vEndP, g_Tunnel.vEndD, g_Tunnel.vEndN,
              wTexRing/(FLOAT)NUM_TEX_RINGS,
              &g_Tunnel.pvVertices[0] );

    //Replace the back segment with a new segment at the front of the
    //tube in memory. Update the current end position of the tube in
    //memory.
    wEndRing = (g_Tunnel.wCurrentRing + NUM_SEGMENTS) % (NUM_SEGMENTS + 1);
    MoveToPosition( g_Tunnel.fEndPos, &g_Tunnel.vEndP, &g_Tunnel.vEndD,
                    &g_Tunnel.vEndN);

    // Update the execute buffer with the new vertices and triangles.
    wRingOffset    = sizeof(D3DVERTEX) * g_Tunnel.wCurrentRing * NUM_SIDES;
    wSegmentOffset = sizeof(D3DTRIANGLE) * g_Tunnel.wCurrentSegment * 
                     NUM_SIDES * 2;

    // Update the position of the back of the tube in memory and texture
    // counter.
    g_Tunnel.wCurrentRing = (g_Tunnel.wCurrentRing + 1) % (NUM_SEGMENTS + 1);
    g_Tunnel.wCurrentSegment = (g_Tunnel.wCurrentSegment + 1) % NUM_SEGMENTS;
    wTexRing = (wTexRing + 1) % NUM_TEX_RINGS;

}




//-----------------------------------------------------------------------
// Name: App_InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3 pd3dDevice,
                               LPDIRECT3DVIEWPORT3 pvViewport )
{
    // Update the menu settings
    SetMenuStates();

	// Get a ptr to the ID3D object to create materials and/or lights. Note:
	// the Release() call just serves to decrease the ref count.
    LPDIRECT3D3 pD3D;
    pd3dDevice->GetDirect3D( &pD3D );
    pD3D->Release();

    // Set the view, projection and world matrices in an execute buffer
    D3DUtil_SetIdentityMatrix( g_matWorld );
    D3DUtil_SetIdentityMatrix( g_matView );
    D3DUtil_SetProjectionMatrix( g_matProj, g_PI/4, 1.0f, 1.0f, 1000.0f );

    // Adjust the aspect ratio in the projection matrix
    D3DVIEWPORT2 vpData;
    D3DUtil_InitViewport( vpData );
    pvViewport->GetViewport2( &vpData );
    if( vpData.dwWidth > vpData.dwHeight )
        g_matProj._11 *= ((FLOAT)vpData.dwWidth) / vpData.dwHeight;
    else
        g_matProj._22 *= ((FLOAT)vpData.dwHeight) / vpData.dwWidth;

    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &g_matWorld );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW,       &g_matView );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &g_matProj );

    // Restore the texture for the current device
    g_pTextureContainer->pd3dDevice = pd3dDevice;
    Texture_Restore( g_pTextureContainer );
    if( NULL == g_pTextureContainer->ptexTexture )
        return E_FAIL;
    pd3dDevice->SetTexture( 0, g_pTextureContainer->ptexTexture );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

    // Setup materials and lights
    if( FAILED( pD3D->CreateMaterial( &g_pmtrlTunnelMtrl, NULL ) ) )
        return E_FAIL;
    D3DMATERIAL mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    g_pmtrlTunnelMtrl->SetMaterial( &mtrl );
    g_pmtrlTunnelMtrl->GetHandle( pd3dDevice, &g_Tunnel.hMtrl );
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_MATERIAL, g_Tunnel.hMtrl );

    if( FAILED( pD3D->CreateLight( &g_Tunnel.pLight, NULL ) ) )
        return E_FAIL;
    D3DUtil_InitLight( g_Tunnel.light, D3DLIGHT_POINT );
    g_Tunnel.light.dvPosition     = g_Tunnel.vCameraP;
    g_Tunnel.light.dcvColor.r     = 1.0f;
    g_Tunnel.light.dcvColor.g     = 1.0f;
    g_Tunnel.light.dcvColor.b     = 1.0f;
    g_Tunnel.light.dvAttenuation2 = 0.05f;
    g_Tunnel.pLight->SetLight( &g_Tunnel.light );
    pvViewport->AddLight( g_Tunnel.pLight );

    pd3dDevice->SetLightState(  D3DLIGHTSTATE_AMBIENT, 0x38383838 );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_WRAPU,  TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_WRAPV,  TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE );

    // Setup fog states
    FLOAT fFogStart = 0.0f, fFogEnd = 25.0f;
    pd3dDevice->SetLightState( D3DLIGHTSTATE_FOGMODE,  D3DFOG_LINEAR );
    pd3dDevice->SetLightState( D3DLIGHTSTATE_FOGSTART, *(DWORD*)&fFogStart );
    pd3dDevice->SetLightState( D3DLIGHTSTATE_FOGEND,   *(DWORD*)&fFogEnd );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_FOGCOLOR, 0x004488ff );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetMenuStates()
// Desc: Checks and unchecks the menu items so the agreee with the app
//       settings.
//-----------------------------------------------------------------------------
VOID SetMenuStates()
{
    CheckMenuItem( g_hMenu, IDM_TOGGLEFOG,
                   g_bFogEnabled ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( g_hMenu, IDM_TOGGLEDITHER,
                   g_bDitherEnabled ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( g_hMenu, IDM_TOGGLEPERSPECTIVE,
                   g_bPerspectiveCorrect ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( g_hMenu, IDM_TOGGLESPECULAR,
                   g_bSpecularEnabled ? MF_CHECKED : MF_UNCHECKED );

    CheckMenuItem( g_hMenu, IDM_TOGGLEGOURAUD,
             g_dwShading==D3DSHADE_GOURAUD ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( g_hMenu, IDM_POINTSAMPLE,
             g_dwFilterType==D3DFILTER_NEAREST ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( g_hMenu, IDM_BILINEARFILTER,
             g_dwFilterType==D3DFILTER_LINEAR ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( g_hMenu, IDM_TOGGLEANTIALIAS, 
             g_dwAntialias==D3DANTIALIAS_NONE ? MF_UNCHECKED : MF_CHECKED );
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
            case IDM_TOGGLEANTIALIAS:
                if( g_dwAntialias == D3DANTIALIAS_NONE )
                    g_dwAntialias = D3DANTIALIAS_SORTDEPENDENT;
                else
                    g_dwAntialias = D3DANTIALIAS_NONE;
                break;
            case IDM_TOGGLEFOG:
                g_bFogEnabled = !g_bFogEnabled;
                break;
            case IDM_TOGGLEDITHER:
                g_bDitherEnabled = !g_bDitherEnabled;
                break;
            case IDM_TOGGLEPERSPECTIVE:
                g_bPerspectiveCorrect = !g_bPerspectiveCorrect;
                break;
            case IDM_TOGGLESPECULAR:
                g_bSpecularEnabled = !g_bSpecularEnabled;
                break;
            case IDM_TOGGLEGOURAUD:
                if( g_dwShading == D3DSHADE_GOURAUD )
                    g_dwShading = D3DSHADE_FLAT;
                else
                    g_dwShading = D3DSHADE_GOURAUD;
                break;
            case IDM_POINTSAMPLE:
                g_dwFilterType = D3DFILTER_NEAREST;
                break;
            case IDM_BILINEARFILTER:
                g_dwFilterType = D3DFILTER_LINEAR;
                break;
        }
        SetMenuStates();
    }
    return WndProc( hWnd, uMsg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: Texture_Create()
// Desc: Creates the texture structure and loads image data from a bitmap
//-----------------------------------------------------------------------------
HRESULT Texture_Create( CHAR* strTextureName, TextureContainer** ppTexture )
{
    TextureContainer* pTexture;

    if( NULL == ( pTexture = new TextureContainer ) )
        return NULL;
    pTexture->pddsSurface = NULL;
    pTexture->ptexTexture = NULL;

    // Load the bitmaps from a file
    pTexture->hbmBitmap = (HBITMAP)LoadImage( NULL, strTextureName, IMAGE_BITMAP,
                               0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION );

    // If that didn't work, trying loading bitmaps from the resource
    if( NULL == pTexture->hbmBitmap ) 
        pTexture->hbmBitmap = (HBITMAP)LoadImage( GetModuleHandle(NULL), strTextureName,
                           IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION );
        
    if( NULL == pTexture->hbmBitmap ) 
    {
        delete pTexture;
        (*ppTexture) = NULL;
        return E_FAIL;
    }

    // Return successfully with the texture container
    (*ppTexture) = pTexture;
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Texture_CopyBitmapToSurface()
// Desc: Updates the Texture surface with image stored in the bitmap
//-----------------------------------------------------------------------------
HRESULT Texture_CopyBitmapToSurface( TextureContainer* pTexture )
{
    // Get a DDraw object to create a temporary surface
    LPDIRECTDRAW4 pDD;
    if( FAILED( pTexture->pddsSurface->GetDDInterface( (VOID**)&pDD ) ) )
        return NULL;
    pDD->Release();
        
    // Use a temporary systemmemory surface to load the bitmaps.
    LPDIRECTDRAWSURFACE4 pddsTempSurface;
    DDSURFACEDESC2 ddsd;
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    pTexture->pddsSurface->GetSurfaceDesc( &ddsd );
    ddsd.dwFlags          = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps   = DDSCAPS_TEXTURE|DDSCAPS_SYSTEMMEMORY;
    ddsd.ddsCaps.dwCaps2  = 0L;

    // Create the temp surface
    if( FAILED( pDD->CreateSurface( &ddsd, &pddsTempSurface, NULL ) ) )
        return E_FAIL;

    // Copy the bitmap image to the surface
    BITMAP bm; 
    GetObject( pTexture->hbmBitmap, sizeof(BITMAP), &bm ); 

    // Create a DC and setup the bitmap
    HDC hdcBitmap = CreateCompatibleDC( NULL );
    if( NULL == hdcBitmap )
    {
        pddsTempSurface->Release();
        return E_FAIL;
    }
    SelectObject( hdcBitmap, pTexture->hbmBitmap );

    HDC hdcSurface;
    if( SUCCEEDED( pddsTempSurface->GetDC( &hdcSurface ) ) )
    {
        BitBlt( hdcSurface, 0, 0, bm.bmWidth, bm.bmHeight, hdcBitmap,
                0, 0, SRCCOPY );
        pddsTempSurface->ReleaseDC( hdcSurface );
    }

    DeleteDC( hdcBitmap );


    LPDIRECT3DTEXTURE2 ptexTempTexture;
    if( FAILED( pddsTempSurface->QueryInterface( IID_IDirect3DTexture2,
                                         (VOID**)&ptexTempTexture ) ) )
        return E_FAIL;
    

    pTexture->ptexTexture->Load( ptexTempTexture );
    
    ptexTempTexture->Release();
    pddsTempSurface->Release();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: TextureSearchCallback()
// Desc: Callback function used to enumerate texture formats.
//-----------------------------------------------------------------------------
HRESULT CALLBACK TextureSearchCallback( DDPIXELFORMAT* pddpf, VOID* param )
{
    DDSURFACEDESC2* pddsd = (DDSURFACEDESC2*)param;

    // Skip unwanted formats
    if( pddpf->dwRGBBitCount != pddsd->dwFlags )
        return DDENUMRET_OK;
    if( pddpf->dwFlags & (DDPF_LUMINANCE|DDPF_ALPHAPIXELS) )
        return DDENUMRET_OK;
    if( pddpf->dwFlags & (DDPF_BUMPLUMINANCE|DDPF_BUMPDUDV) )
        return DDENUMRET_OK;
    if( 0 != pddpf->dwFourCC )
        return DDENUMRET_OK;

    memcpy( &pddsd->ddpfPixelFormat, pddpf, sizeof(DDPIXELFORMAT) );
    return DDENUMRET_CANCEL;
}




//-----------------------------------------------------------------------------
// Name: Texture_Restore()
// Desc: Creates the device-dependant surface and texture
//-----------------------------------------------------------------------------
HRESULT Texture_Restore( TextureContainer* pTexture )
{
    // Check params
    if( NULL==pTexture || NULL==pTexture->pd3dDevice )
        return E_INVALIDARG;

    // Release any previously created objects
    SAFE_RELEASE( pTexture->ptexTexture );
    SAFE_RELEASE( pTexture->pddsSurface );

    // Get a DDraw ptr (from the device's render target) for creating surfaces
	// Note: the Release() call just serves to decrement the ref count, but the
	// ptr is still valid.
    LPDIRECTDRAWSURFACE4 pddsRender;
    LPDIRECTDRAW4        pDD = NULL;
    pTexture->pd3dDevice->GetRenderTarget( &pddsRender );
    pddsRender->GetDDInterface( (VOID**)&pDD );
    pddsRender->Release();
    pDD->Release();

    // Get size info for the bitmap
    BITMAP bm; 
    GetObject( pTexture->hbmBitmap, sizeof(BITMAP), &bm ); 

    // Set up and create the texture surface
    DDSURFACEDESC2 ddsd;
    ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
    ddsd.dwSize          = sizeof(DDSURFACEDESC2);
    ddsd.dwFlags         = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE;
    ddsd.dwWidth         = bm.bmWidth;
    ddsd.dwHeight        = bm.bmHeight;

    // Enumerate a good texture format. Search for a 16-bit format first
    DDSURFACEDESC2 ddsdSearch;
    ddsdSearch.dwFlags = 16;
    pTexture->pd3dDevice->EnumTextureFormats( TextureSearchCallback, &ddsdSearch );
    
    // If that wasn't found, check for a 32-bit format
    if( 16 != ddsdSearch.ddpfPixelFormat.dwRGBBitCount )
    {
        ddsdSearch.dwFlags = 32;
        pTexture->pd3dDevice->EnumTextureFormats( TextureSearchCallback,
                                                  &ddsdSearch );
        if( 32 != ddsdSearch.ddpfPixelFormat.dwRGBBitCount )
            return E_FAIL;
    }

    // If we got a good texture format, use it to create the surface
    memcpy( &ddsd.ddpfPixelFormat, &ddsdSearch.ddpfPixelFormat,
            sizeof(DDPIXELFORMAT) );

    // Create the surface and texture
    if( FAILED( pDD->CreateSurface( &ddsd, &pTexture->pddsSurface, NULL ) ) )
        return E_FAIL;
    if( FAILED( pTexture->pddsSurface->QueryInterface( IID_IDirect3DTexture2,
                                                     (VOID**)&pTexture->ptexTexture ) ) )
        return E_FAIL;

    return Texture_CopyBitmapToSurface( pTexture );
}




//-----------------------------------------------------------------------------
// Name: Texture_Invalidate()
// Desc: Frees device dependant objects for the texture
//-----------------------------------------------------------------------------
VOID Texture_Invalidate( TextureContainer* pTexture )
{
    if( pTexture )
    {
        SAFE_RELEASE( pTexture->pddsSurface );
        SAFE_RELEASE( pTexture->ptexTexture );
    }
}




//-----------------------------------------------------------------------------
// Name: Texture_Delete()
// Desc: Frees device dependant objects for the texture
//-----------------------------------------------------------------------------
VOID Texture_Delete( TextureContainer* pTexture )
{
    if( pTexture )
        DeleteObject( pTexture->hbmBitmap );
}




