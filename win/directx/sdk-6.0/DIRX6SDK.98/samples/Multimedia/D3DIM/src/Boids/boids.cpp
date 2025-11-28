//-----------------------------------------------------------------------------
// File: Boids.cpp
//
// Desc: Example code showing how to do flocking. 
//
//       Note: This code uses the D3D Framework helper library.
//
//
// Copyright (c) 1996-1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define STRICT
#define D3D_OVERLOADS
#include <math.h>
#include <time.h>
#include "D3DTextr.h"
#include "D3DUtil.h"
#include "D3DMath.h"


//-----------------------------------------------------------------------------
// Declare the application globals for use in WinMain.cpp
//-----------------------------------------------------------------------------
TCHAR* g_strAppTitle       = TEXT("Boids: Flocking objects");
BOOL   g_bAppUseZBuffer    = TRUE;
BOOL   g_bAppUseBackBuffer = TRUE;




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define MIN(a,b) ((a<b)?a:b)
#define MAX(a,b) ((a>b)?a:b)

LPDIRECT3DMATERIAL3 g_pmtrlObjectMtrl     = NULL;
LPDIRECT3DLIGHT     g_pLight1             = NULL;
LPDIRECT3DLIGHT     g_pLight2             = NULL;

#define NUM_BOIDS      13
#define NUM_OBSTACLES   8
#define OBSTACLE_RADIUS 3.0f
#define NUM_GRID    20
#define GRID_WIDTH  190.0

D3DVERTEX* g_pObstacleVertices = NULL;
WORD*      g_pObstacleIndices  = NULL;
DWORD      g_dwNumObstacleVertices;
DWORD      g_dwNumObstacleIndices;

D3DVERTEX  pvGridVertices[NUM_GRID*NUM_GRID];
D3DVERTEX  pvBoidVertices[16];
WORD       pwBoidIndices[30];




//-----------------------------------------------------------------------------
// Name: struct Boid
// Desc: Structure for holding data for each boid
//-----------------------------------------------------------------------------
struct Boid
{
    D3DVECTOR   vLoc;
    D3DVECTOR   vDir;       // Current direction
    D3DVECTOR   vDeltaPos;  // Change in position from flock centering
    D3DVECTOR   vDeltaDir;  // Change in direction
    WORD        wDeltaCnt;  // Number of boids that influence this vDeltaDir
    FLOAT       fSpeed;
    FLOAT       fYaw, fPitch, fRoll, fDYaw;
    FLOAT       r, g, b;    // Color of the boid
    FLOAT       afDist[NUM_BOIDS];      // Array of boid distances, yuk what a waste
	
	D3DMATRIX   matLocal;
};


Boid      g_pBoids[NUM_BOIDS];
D3DVECTOR g_vObstacleLocations[NUM_OBSTACLES];
D3DVECTOR g_vGoal;
D3DMATRIX g_matGrid;


const FLOAT InfluenceRadius        = 20.0f;
const FLOAT InfluenceRadiusSquared = InfluenceRadius * InfluenceRadius;
const FLOAT CollisionFraction      = 0.8f;
const FLOAT InvCollisionFraction   = 1.0f/(1.0f-CollisionFraction);
const FLOAT NormalSpeed            = 0.1f;
const FLOAT AngleTweak             = 0.02f;
const FLOAT PitchToSpeedRatio      = 0.002f;




//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
VOID    AppPause( BOOL );
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
BOOL    GenerateSphere( FLOAT, WORD, WORD, FLOAT, FLOAT, FLOAT, D3DVERTEX**,
                        DWORD*, WORD**, DWORD* );
#define rnd()  (((FLOAT)rand() ) / RAND_MAX)





//-----------------------------------------------------------------------------
// Name: CreateSphere()
// Desc: Sets up the vertices for a sphere.
//-----------------------------------------------------------------------------
HRESULT CreateSphere( D3DVERTEX** ppVertices, DWORD* pdwNumVertices,
					  WORD** ppIndices, DWORD* pdwNumIndices,
				      FLOAT fRadius, DWORD dwNumRings )
{
	// Allocate memory for the vertices and indices
	DWORD      dwNumVertices = (dwNumRings*(2*dwNumRings+1)+2);
	DWORD      dwNumIndices  = 6*(dwNumRings*2)*((dwNumRings-1)+1);
	D3DVERTEX* pVertices     = new D3DVERTEX[dwNumVertices];
	WORD*      pIndices      = new WORD[dwNumIndices];

	(*ppVertices) = pVertices;
	(*ppIndices)  = pIndices;
	
	// Counters
    WORD x, y, vtx = 0, index = 0;

	// Angle deltas for constructing the sphere's vertices
    FLOAT fDAng   = g_PI / dwNumRings;
    FLOAT fDAngY0 = fDAng;

    // Make the middle of the sphere
    for( y=0; y<dwNumRings; y++ )
	{
        FLOAT y0 = (FLOAT)cos(fDAngY0);
        FLOAT r0 = (FLOAT)sin(fDAngY0);
		FLOAT tv = (1.0f - y0)/2;

        for( x=0; x<(dwNumRings*2)+1; x++ )
		{
            FLOAT fDAngX0 = x*fDAng;
        
			D3DVECTOR v( r0*(FLOAT)sin(fDAngX0), y0, r0*(FLOAT)cos(fDAngX0) );
			FLOAT tu = 1.0f - x/(dwNumRings*2.0f);

            *pVertices++ = D3DVERTEX( fRadius*v, v, tu, tv );
			vtx ++;
        }
        fDAngY0 += fDAng;
    }

    for( y=0; y<dwNumRings-1; y++ )
	{
        for( x=0; x<(dwNumRings*2); x++ )
		{
            *pIndices++ = (WORD)( (y+0)*(dwNumRings*2+1) + (x+0) );
            *pIndices++ = (WORD)( (y+1)*(dwNumRings*2+1) + (x+0) );
            *pIndices++ = (WORD)( (y+0)*(dwNumRings*2+1) + (x+1) );
            *pIndices++ = (WORD)( (y+0)*(dwNumRings*2+1) + (x+1) );
            *pIndices++ = (WORD)( (y+1)*(dwNumRings*2+1) + (x+0) ); 
            *pIndices++ = (WORD)( (y+1)*(dwNumRings*2+1) + (x+1) );
			index += 6;
        }
    }
    // Make top and bottom
	D3DVECTOR vy( 0, 1, 0 );
	WORD wNorthVtx = vtx;
    *pVertices++ = D3DVERTEX( fRadius*vy, vy, 0.5f, 0.0f );
    vtx++;
	WORD wSouthVtx = vtx;
    *pVertices++ = D3DVERTEX( -fRadius*vy,-vy, 0.5f, 1.0f );
    vtx++;

    for( x=0; x<(dwNumRings*2); x++ )
	{
		WORD p1 = wSouthVtx;
		WORD p2 = (WORD)( (y)*(dwNumRings*2+1) + (x+1) );
		WORD p3 = (WORD)( (y)*(dwNumRings*2+1) + (x+0) );

        *pIndices++ = p1;
        *pIndices++ = p3;
        *pIndices++ = p2;
		index += 3;
    }

    for( x=0; x<(dwNumRings*2); x++ )
	{
		WORD p1 = wNorthVtx;
		WORD p2 = (WORD)( (0)*(dwNumRings*2+1) + (x+1) );
		WORD p3 = (WORD)( (0)*(dwNumRings*2+1) + (x+0) );

        *pIndices++ = p1;
        *pIndices++ = p3;
        *pIndices++ = p2;
		index += 3;
    }

	(*pdwNumVertices) = vtx;
	(*pdwNumIndices)  = index;

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateFlock()
// Desc: Update posiiton of each boid in flock
//-----------------------------------------------------------------------------
VOID UpdateFlock()
{
    FLOAT fDist;

    // first update the dist array 0.0..1.0 with 0.0 being furthest away
    for( WORD i=0; i<NUM_BOIDS; i++ )
    {
        for( WORD j=i+1; j<NUM_BOIDS; j++ )
        {
            fDist = SquareMagnitude( g_pBoids[i].vLoc - g_pBoids[j].vLoc );
            fDist = InfluenceRadiusSquared - fDist;
            if( fDist < 0.0f )
                fDist = 0.0f;
            else
                fDist /= InfluenceRadiusSquared;

            g_pBoids[i].afDist[j] = g_pBoids[j].afDist[i] = fDist;
        }
        g_pBoids[i].afDist[i] = 0.0f;
        g_pBoids[i].vDeltaDir = D3DVECTOR( 0.0f, 0.0f, 0.0f );
        g_pBoids[i].vDeltaPos = D3DVECTOR( 0.0f, 0.0f, 0.0f );
        g_pBoids[i].wDeltaCnt = 0;
    }

    for( i=0; i<NUM_BOIDS; i++ )
    {
        for( WORD j=i+1; j<NUM_BOIDS; j++ )
        {
            // if i is near j have them influence each other
            if( g_pBoids[i].afDist[j] > 0.0f )
            {
                D3DVECTOR   vDiff = Normalize( g_pBoids[i].vLoc - g_pBoids[j].vLoc );
                D3DVECTOR   vDelta;
                FLOAT       fCollWeight = 0.0f;     // collision weighting

                // only do collision testing against the nearest ones
                if( g_pBoids[i].afDist[j] - CollisionFraction > 0.0f )
                    fCollWeight = (g_pBoids[i].afDist[j] - CollisionFraction) * InvCollisionFraction;

                // add in a little flock centering
                if( g_pBoids[i].afDist[j] - (1.0f-CollisionFraction) > 0.0f )
                    fCollWeight -= g_pBoids[i].afDist[j] * (1.0f-fCollWeight);

                vDelta = fCollWeight * vDiff;

                // add in the collision avoidance
                g_pBoids[i].vDeltaPos += vDelta;
                g_pBoids[j].vDeltaPos -= vDelta;

                // add in the velocity influences
                g_pBoids[i].vDeltaDir += g_pBoids[j].vDir * g_pBoids[i].afDist[j];
                g_pBoids[j].vDeltaDir += g_pBoids[i].vDir * g_pBoids[i].afDist[j];
                g_pBoids[i].wDeltaCnt++;
                g_pBoids[j].wDeltaCnt++;
            }
        }
    }

    // update the boids
    for( i=0; i<NUM_BOIDS; i++ )
    {
        if( g_pBoids[i].wDeltaCnt )
        {
            g_pBoids[i].vDeltaDir /= (FLOAT)g_pBoids[i].wDeltaCnt;
            g_pBoids[i].vDeltaDir -= g_pBoids[i].vDir;
            g_pBoids[i].vDeltaDir *= 1.5f;
        }
        D3DVECTOR vDelta = g_pBoids[i].vDeltaDir + g_pBoids[i].vDeltaPos;
        D3DVECTOR vOffset;

        // add in the influence of the global goal
        D3DVECTOR vGoal = 0.5 * Normalize(g_vGoal-g_pBoids[i].vLoc );
        vDelta += vGoal;

        // add in any obstacles
        for( WORD j=0; j<NUM_OBSTACLES; j++ )
        {
            D3DVECTOR vOb = g_pBoids[i].vLoc - g_vObstacleLocations[j];
            FLOAT     fDist = Magnitude(vOb);

            if( fDist > 2*OBSTACLE_RADIUS )
                continue;

            vOb /= fDist;   // normalize
            fDist = 1.0f - fDist/(2*OBSTACLE_RADIUS);
            vDelta += fDist * vOb * 5.0f;
        }

        // first deal with pitch changes
        if( vDelta.y > 0.01f )
        {           // we're too low
            g_pBoids[i].fPitch += AngleTweak;
            if( g_pBoids[i].fPitch > 0.8f )
                g_pBoids[i].fPitch = 0.8f;
        }
        else if( vDelta.y < -0.01f )
        {   // we're too high
            g_pBoids[i].fPitch -= AngleTweak;
            if( g_pBoids[i].fPitch < -0.8f )
                g_pBoids[i].fPitch = -0.8f;
        } 
        else
        {
            // add damping
            g_pBoids[i].fPitch *= 0.98f;
        }

        // speed up or slow down depending on angle of attack
        g_pBoids[i].fSpeed -= g_pBoids[i].fPitch * PitchToSpeedRatio;
        // damp back to normal
        g_pBoids[i].fSpeed = (g_pBoids[i].fSpeed-NormalSpeed)*0.99f + NormalSpeed;

        if( g_pBoids[i].fSpeed < NormalSpeed/2 )
            g_pBoids[i].fSpeed = NormalSpeed/2;
        if( g_pBoids[i].fSpeed > NormalSpeed*5 )
            g_pBoids[i].fSpeed = NormalSpeed*5;

        // now figure out yaw changes
        vOffset    = vDelta;
        vOffset.y  = 0.0f;
        vDelta     = g_pBoids[i].vDir;
        vOffset    = Normalize( vOffset );
        FLOAT fDot = DotProduct( vOffset, vDelta );
        
        // speed up slightly if not turning much
        if( fDot > 0.7f )
        {
            fDot -= 0.7f;
            g_pBoids[i].fSpeed += fDot * 0.005f;
        }
        vOffset = CrossProduct( vOffset, vDelta );
        fDot = (1.0f-fDot)/2.0f * 0.07f;
        if( vOffset.y > 0.05f )
            g_pBoids[i].fDYaw = (g_pBoids[i].fDYaw*19.0f + fDot) * 0.05f;
        else if( vOffset.y < -0.05f )
            g_pBoids[i].fDYaw = (g_pBoids[i].fDYaw*19.0f - fDot) * 0.05f;
        else
            g_pBoids[i].fDYaw *= 0.98f; // damp it

        g_pBoids[i].fYaw += g_pBoids[i].fDYaw;
        g_pBoids[i].fRoll = -g_pBoids[i].fDYaw * 20.0f;
    }
}




//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{
    // Points and normals which make up a boid geometry
    D3DVECTOR p1 = D3DVECTOR( 0.00f, 0.00f, 0.50f );
    D3DVECTOR p2 = D3DVECTOR( 0.50f, 0.00f,-0.50f );
    D3DVECTOR p3 = D3DVECTOR( 0.15f, 0.15f,-0.35f );
    D3DVECTOR p4 = D3DVECTOR(-0.15f, 0.15f,-0.35f );
    D3DVECTOR p5 = D3DVECTOR( 0.15f,-0.15f,-0.35f );
    D3DVECTOR p6 = D3DVECTOR(-0.15f,-0.15f,-0.35f );
    D3DVECTOR p7 = D3DVECTOR(-0.50f, 0.00f,-0.50f );
    D3DVECTOR n1 = Normalize( D3DVECTOR( 0.2f, 1.0f, 0.0f ) );
    D3DVECTOR n2 = Normalize( D3DVECTOR( 0.1f, 1.0f, 0.0f ) );
    D3DVECTOR n3 = Normalize( D3DVECTOR( 0.0f, 1.0f, 0.0f ) );
    D3DVECTOR n4 = Normalize( D3DVECTOR(-0.1f, 1.0f, 0.0f ) );
    D3DVECTOR n5 = Normalize( D3DVECTOR(-0.2f, 1.0f, 0.0f ) );
    D3DVECTOR n6 = Normalize( D3DVECTOR(-0.4f, 0.0f, -1.0f ) );
    D3DVECTOR n7 = Normalize( D3DVECTOR(-0.2f, 0.0f, -1.0f ) );
    D3DVECTOR n8 = Normalize( D3DVECTOR( 0.2f, 0.0f, -1.0f ) );
    D3DVECTOR n9 = Normalize( D3DVECTOR( 0.4f, 0.0f, -1.0f ) );

    // Vertices for the top
    pvBoidVertices[ 0] = D3DVERTEX( p1, n1, 0.000f, 0.500f );
    pvBoidVertices[ 1] = D3DVERTEX( p2, n2, 0.500f, 1.000f );
    pvBoidVertices[ 2] = D3DVERTEX( p3, n3, 0.425f, 0.575f );
    pvBoidVertices[ 3] = D3DVERTEX( p4, n4, 0.425f, 0.425f );
    pvBoidVertices[ 4] = D3DVERTEX( p7, n5, 0.500f, 0.000f );

    // Vertices for the bottom
    pvBoidVertices[ 5] = D3DVERTEX( p1, -n5, 1.000f, 0.500f );
    pvBoidVertices[ 6] = D3DVERTEX( p2, -n4, 0.500f, 1.000f );
    pvBoidVertices[ 7] = D3DVERTEX( p5, -n3, 0.575f, 0.575f );
    pvBoidVertices[ 8] = D3DVERTEX( p6, -n2, 0.575f, 0.425f );
    pvBoidVertices[ 9] = D3DVERTEX( p7, -n1, 0.500f, 0.000f );

    // Vertices for the  rear
    pvBoidVertices[10] = D3DVERTEX( p2, n6, 0.500f, 1.000f );
    pvBoidVertices[11] = D3DVERTEX( p3, n7, 0.425f, 0.575f );
    pvBoidVertices[12] = D3DVERTEX( p4, n8, 0.425f, 0.425f );
    pvBoidVertices[13] = D3DVERTEX( p7, n9, 0.500f, 0.000f );
    pvBoidVertices[14] = D3DVERTEX( p6, n8, 0.575f, 0.425f );
    pvBoidVertices[15] = D3DVERTEX( p5, n7, 0.575f, 0.575f );

    // Vertex inidices for the boid
    pwBoidIndices[ 0] =  0; pwBoidIndices[ 1] =  1; pwBoidIndices[ 2] =  2;
    pwBoidIndices[ 3] =  0; pwBoidIndices[ 4] =  2; pwBoidIndices[ 5] =  3;
    pwBoidIndices[ 6] =  0; pwBoidIndices[ 7] =  3; pwBoidIndices[ 8] =  4;
    pwBoidIndices[ 9] =  5; pwBoidIndices[10] =  7; pwBoidIndices[11] =  6;
    pwBoidIndices[12] =  5; pwBoidIndices[13] =  8; pwBoidIndices[14] =  7;
    pwBoidIndices[15] =  5; pwBoidIndices[16] =  9; pwBoidIndices[17] =  8;
    pwBoidIndices[18] = 10; pwBoidIndices[19] = 15; pwBoidIndices[20] = 11;
    pwBoidIndices[21] = 11; pwBoidIndices[22] = 15; pwBoidIndices[23] = 12;
    pwBoidIndices[24] = 12; pwBoidIndices[25] = 15; pwBoidIndices[26] = 14;
    pwBoidIndices[27] = 12; pwBoidIndices[28] = 14; pwBoidIndices[29] = 13;

    // seed the random number generator
    srand(time(NULL));

    g_vGoal = D3DVECTOR(0.0f, 0.0f, 0.0f);

    for( WORD i=0; i<NUM_BOIDS; i++ )
    {
        g_pBoids[i].vLoc   = D3DVECTOR(100.0f*(rnd()-rnd()), 10.0f*rnd(), 100.0f*(rnd()-rnd()));
        g_pBoids[i].vDir   = Normalize(D3DVECTOR(rnd()-rnd(), rnd()-rnd(), rnd()-rnd()));
        g_pBoids[i].fYaw   = 0.0f;
        g_pBoids[i].fPitch = 0.0f;
        g_pBoids[i].fRoll  = 0.0f;
        g_pBoids[i].fDYaw  = 0.0f;
        g_pBoids[i].fSpeed = 0.1f;
        g_pBoids[i].r      = rnd();
        g_pBoids[i].g      = rnd();
        g_pBoids[i].b      = rnd();

        FLOAT fMin = MIN( g_pBoids[i].r, MIN( g_pBoids[i].g, g_pBoids[i].b ) );
        FLOAT fMax = MAX( g_pBoids[i].r, MIN( g_pBoids[i].g, g_pBoids[i].b ) );

        g_pBoids[i].r = (g_pBoids[i].r - fMin) / (fMax-fMin);
        g_pBoids[i].g = (g_pBoids[i].g - fMin) / (fMax-fMin);
        g_pBoids[i].b = (g_pBoids[i].b - fMin) / (fMax-fMin);
    }

	// Position the obstacles
    g_vObstacleLocations[0] = D3DVECTOR( 100, 10,    0 );
    g_vObstacleLocations[1] = D3DVECTOR(-100, 10,    0 );
    g_vObstacleLocations[2] = D3DVECTOR(   0, 10,  100 );
    g_vObstacleLocations[3] = D3DVECTOR(   0, 10, -100 );
    for( i=4; i<NUM_OBSTACLES; i++ )
        g_vObstacleLocations[i] = D3DVECTOR( 100*(rnd()-rnd()), 10*rnd(), 100*(rnd()-rnd()) );

    FLOAT fSize   = GRID_WIDTH/(NUM_GRID-1.0f);
    FLOAT fOffset = GRID_WIDTH/2.0f;
    
    for( i=0; i<NUM_GRID; i++ )
    {
        for( WORD j=0; j<NUM_GRID; j++ )
        {
            pvGridVertices[j+i*NUM_GRID] = D3DVERTEX( 
				          D3DVECTOR( i*fSize-fOffset, 0.0f, j*fSize-fOffset ),
				          D3DVECTOR( 0.0f, 1.0f, 0.0f ), 0.0f, 0.0f );
        }
    }

    // generate the sphere data
	CreateSphere( &g_pObstacleVertices, &g_dwNumObstacleVertices, 
		          &g_pObstacleIndices, &g_dwNumObstacleIndices, 1.0f, 8 );
 

    // Create some textures
    D3DTextr_CreateTexture( "earth.bmp" );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT App_FrameMove( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{
    // Set the view and projection matrices. Note: these are static since
	// the view changes each frame
    D3DMATRIX matProj, matView, matWorld;
    static D3DVECTOR vEyePt( 0.0f, 30.0f, 100.0f );
    static D3DVECTOR vLookatPt( 0.0f, 0.0f, 50.0f );
    static D3DVECTOR vUpVec( 0.0f, 1.0f, 0.0f );

    D3DUtil_SetViewMatrix( matView, vEyePt, vLookatPt, vUpVec );
    pd3dDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &matView );

    static FLOAT tic = -200.0f * rnd();
    tic += 0.01f;

    // Update grid matrix
    D3DVECTOR vOffset;
    vOffset.x = (FLOAT)floor( vLookatPt.x/20 ) * 20.0f - 10.0f;
    vOffset.y = 0.0f;
    vOffset.z = (FLOAT)floor( vLookatPt.z/20 ) * 20.0f - 10.0f;

    D3DUtil_SetTranslateMatrix( g_matGrid, vOffset );

    UpdateFlock();

    vLookatPt = D3DVECTOR( 0.0f, 0.0f, 0.0f );
    // draw the boids
    for( WORD i=0; i<NUM_BOIDS; i++)
    {
        D3DVECTOR   step;

        // Build the world matrix for the boid. First translate into place, 
		// then set orientation, then scale (if needed)
        D3DUtil_SetTranslateMatrix( matWorld, g_pBoids[i].vLoc );

        D3DMATRIX matTemp, matRotateX, matRotateY, matRotateZ;
        D3DUtil_SetRotateYMatrix( matRotateY, -g_pBoids[i].fYaw );
        D3DUtil_SetRotateXMatrix( matRotateX, -g_pBoids[i].fPitch );
        D3DUtil_SetRotateZMatrix( matRotateZ, -g_pBoids[i].fRoll );
        D3DMath_MatrixMultiply( matTemp, matRotateY, matRotateX );
        D3DMath_MatrixMultiply( matTemp, matTemp, matRotateZ );
        D3DMath_MatrixMultiply( matWorld, matWorld, matTemp );
        
		g_pBoids[i].matLocal = matWorld;

        g_pBoids[i].vDir.x = matWorld(2, 0);
        g_pBoids[i].vDir.y = matWorld(2, 1);
        g_pBoids[i].vDir.z = matWorld(2, 2);

        g_pBoids[i].vLoc += g_pBoids[i].vDir * g_pBoids[i].fSpeed;

        vLookatPt += g_pBoids[i].vLoc;
    }
    vLookatPt /= NUM_BOIDS;
    vEyePt.x = vLookatPt.x + (FLOAT)( 30.0f*sin(tic*0.223) );
    vEyePt.y = vLookatPt.y + (FLOAT)( 21.0f + 20.0f*sin(tic*0.33f) );
    vEyePt.z = vLookatPt.z + (FLOAT)( 30.0f*cos(tic*0.31f) );

    g_vGoal.x = 105.0f * (FLOAT)sin(tic*0.1f);
    g_vGoal.y = 10.0f;
    g_vGoal.z = 105.0f * (FLOAT)cos(tic*0.1f);

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
	D3DMATERIAL mtrl;

    // Clear the viewport
    pvViewport->Clear2( 1UL, prcViewRect, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
		                0x00000000, 1.0f, 0L );

    // Begin the scene
    if( FAILED( pd3dDevice->BeginScene() ) )
		return S_OK; // Don't return a "fatal" error

    pd3dDevice->SetTexture( 0, NULL );

	// Draw the north-south lines
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &g_matGrid );
    pd3dDevice->DrawPrimitive( D3DPT_LINELIST, D3DFVF_VERTEX,
		                       pvGridVertices, NUM_GRID*NUM_GRID, 0 );

	// Draw the east-west lines
    D3DMATRIX matRotateY;
    D3DUtil_SetRotateYMatrix( matRotateY, g_PI/2.0f );
    D3DMath_MatrixMultiply( matRotateY, g_matGrid, matRotateY );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matRotateY );
    pd3dDevice->DrawPrimitive( D3DPT_LINELIST, D3DFVF_VERTEX, 
		                       pvGridVertices, NUM_GRID*NUM_GRID, 0 );

    // Draw the boids
    for( WORD i=0; i<NUM_BOIDS; i++)
    {
        // Set the color for the boid
        D3DUtil_InitMaterial( mtrl, g_pBoids[i].r, g_pBoids[i].g, g_pBoids[i].b );
        g_pmtrlObjectMtrl->SetMaterial( &mtrl );

        // Apply the boid's local matrix
        pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &g_pBoids[i].matLocal );

        // Draw the boid
        pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                          pvBoidVertices, 16,
                                          pwBoidIndices, 30, 0 );
    }

    // Draw the obstacles
    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    g_pmtrlObjectMtrl->SetMaterial( &mtrl );
    pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("earth.bmp") );
        
    for( i=0; i<NUM_OBSTACLES; i++ )
    {
        D3DMATRIX matScale, matTrans, matWorld;
        D3DUtil_SetTranslateMatrix( matTrans, g_vObstacleLocations[i] );
        D3DUtil_SetScaleMatrix( matScale, OBSTACLE_RADIUS, OBSTACLE_RADIUS,
			                    OBSTACLE_RADIUS );
        D3DMath_MatrixMultiply( matWorld, matTrans, matScale );
        
        pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld );

        // Draw the obstacle
        pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                              g_pObstacleVertices, g_dwNumObstacleVertices,
		                      g_pObstacleIndices,  g_dwNumObstacleIndices, 0 );
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

    HRESULT           hr;
    D3DMATERIAL       mtrl;
    D3DMATERIALHANDLE hmtrl;

	// Get a ptr to the ID3D object to create materials and/or lights. Note:
	// the Release() call just serves to decrease the ref count.
    LPDIRECT3D3 pD3D;
    if( FAILED( pd3dDevice->GetDirect3D( &pD3D ) ) )
        return E_FAIL;
    pD3D->Release();

    // Create and set up the object material
    if( FAILED( hr = pD3D->CreateMaterial( &g_pmtrlObjectMtrl, NULL ) ) )
        return E_FAIL;

    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    mtrl.power = 40.0f;
    g_pmtrlObjectMtrl->SetMaterial( &mtrl );
    g_pmtrlObjectMtrl->GetHandle( pd3dDevice, &hmtrl );
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_MATERIAL, hmtrl );

    pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, TRUE );
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_AMBIENT,  0x0a0a0a0a );

	// Setup texture states
    D3DTextr_RestoreAllTextures( pd3dDevice );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );

    // Set the transform matrices
	D3DMATRIX matProj;
   	D3DVIEWPORT2 vp;
	vp.dwSize = sizeof(vp);
	pvViewport->GetViewport2(&vp);
	FLOAT fAspect = ((FLOAT)vp.dwHeight) / vp.dwWidth;

    D3DUtil_SetProjectionMatrix( matProj, 0.75f, fAspect, 1.0f, 1000.0f );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

    // Set up the lights
    if( SUCCEEDED( hr = pD3D->CreateLight( &g_pLight1, NULL ) ) )
    {
        D3DLIGHT light;
        D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, 0.0, 0.0, -12.0 );
        light.dvDirection = Normalize( D3DVECTOR( -0.5f, -1.0f, -.3f ) );
        light.dcvColor.r = 1.0f;
        light.dcvColor.g = 1.0f;
        light.dcvColor.b = 1.0f;
        light.dvAttenuation0 = 1.0f;
        light.dvRange = FLT_MAX;

        hr = g_pLight1->SetLight( &light );
        hr = pvViewport->AddLight( g_pLight1 );
    }

    if( SUCCEEDED( hr = pD3D->CreateLight( &g_pLight2, NULL ) ) )
    {
        D3DLIGHT light;
        D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, 0.0, 0.0, -12.0 );
        light.dvDirection = Normalize( D3DVECTOR( 0.5f, 1.0f, .3f ) );
        light.dcvColor.r = 0.5f;
        light.dcvColor.g = 0.5f;
        light.dcvColor.b = 0.5f;
        light.dvAttenuation0 = 1.0f;
        light.dvRange = FLT_MAX;

        hr = g_pLight2->SetLight( &light );
        hr = pvViewport->AddLight( g_pLight2 );
    }

    return hr;
}




//-----------------------------------------------------------------------------
// Name: App_FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT App_FinalCleanup( LPDIRECT3DDEVICE3 pd3dDevice, 
                          LPDIRECT3DVIEWPORT3 pvViewport )
{
	SAFE_DELETE( g_pObstacleVertices );
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

    SAFE_RELEASE( g_pLight1 );
    SAFE_RELEASE( g_pLight2 );
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





