//-----------------------------------------------------------------------------
// File: Shadow.h
//
// Desc: Header for stencil shadow example
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
#include "D3DMath.h"
#include "D3DEnum.h"
#include "assert.h"
#include "resource.h"


// Most objects are spheres, this defines several user-selectable levels of
// tesselation
#define NUMTESSLEVELS 4
extern WORD  g_TessLevs[NUMTESSLEVELS];
extern DWORD g_CurTessLevel;


// If g_max_StencilVal<NUM_SHADOWS, default to 1-bit stencil buffer mode, which
// only allows non-overlapping shadows and must be sorted front-to-back
#define NUM_SHADOWS 11

struct COLORVERTEX
{
    D3DVECTOR p;
    D3DCOLOR  c;
};


// These are the shadow volumes
struct SHADOW
{
  DWORD   totalverts, num_objverts;
  DWORD   num_side_indices,num_cap_indices;
  LPDIRECT3DVERTEXBUFFER VB;      // holds vertices of shadow volumes
  WORD*   pwShadVolIndices;      // tri indices into vertex buffer VB for DrawPrim
  WORD*   pwShadVolSideIndices;  // ptrs into main index array pwShadVolIndices for Side tris of shadow volume
  WORD*   pwShadVolCapIndices;   // ptrs into main index array pwShadVolIndices for cap tris of shadow volume
};

// Objects that cast shadows
struct SHADOWCASTER
{
    D3DVERTEX* pVerts;
	D3DVERTEX* pRVerts;
    DWORD      VertCount,RVertCount,IndexCount;
    WORD*      pIndices;
    D3DVECTOR  center;  // un-xformed
};

#define NUM_POLY_VERTS 4
#define NUM_POLY_INDICES 6

extern COLORVERTEX  g_pvPolyVertices[NUM_POLY_VERTS];      // verts of square
extern WORD         g_pwPolyIndices[NUM_POLY_INDICES];     // indices of square's tris

extern SHADOW       g_Shad[NUM_SHADOWS];             
extern SHADOWCASTER g_Caster[NUM_SHADOWS];

extern DWORD g_MaxVertCount;

extern DWORD g_NumCasters;   // number of active shadow caster objects
extern DWORD g_NumObjs;      // number of casters+receivers

// "tmp" vertex buffers used to compute shadow volumes and sort shadows in Z
extern LPDIRECT3DVERTEXBUFFER g_pVB_xformed,g_pVB_castertestverts;
extern BOOL   g_bUseOneBitStencil;  // if true, use one-bit stencil buffer algorithm

extern HRESULT Init3DGeometry(LPDIRECT3DDEVICE3 pd3dDevice);
extern void Find2DConvexHull(DWORD nverts,COLORVERTEX *pntptr,DWORD *cNumOutIdxs,WORD **OutHullIdxs);
extern VOID    RotateVertexInX( FLOAT, DWORD, D3DVERTEX*, D3DVERTEX* );
extern VOID    RotateVertexInY( FLOAT, DWORD, D3DVERTEX*, D3DVERTEX* );
extern VOID    RotateVertexInZ( FLOAT, DWORD, D3DVERTEX*, D3DVERTEX* );
extern VOID    TransRotateVertexInX(D3DVECTOR &transvec, FLOAT fTheta, DWORD dwCount,
                                          D3DVERTEX* pvInVertices, D3DVERTEX* pvOutVertices );
extern VOID    TransRotateVertexInY(D3DVECTOR &transvec, FLOAT fTheta, DWORD dwCount,
                                          D3DVERTEX* pvInVertices, D3DVERTEX* pvOutVertices );
extern VOID    TransRotateVertexInZ(D3DVECTOR &transvec, FLOAT fTheta, DWORD dwCount,
                                          D3DVERTEX* pvInVertices, D3DVERTEX* pvOutVertices );
extern BOOL    GenerateSphere(SHADOWCASTER *,D3DVECTOR*, FLOAT, WORD, WORD, FLOAT, FLOAT, FLOAT);




