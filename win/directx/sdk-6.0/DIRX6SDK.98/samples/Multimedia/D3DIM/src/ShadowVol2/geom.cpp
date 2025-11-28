//-----------------------------------------------------------------------------
// File: geom.cpp
//
// Desc: Example code showing how to use stencil buffers to implement shadows
//
//       Note: This code uses the D3D Framework helper library.
//
//
// Copyright (c) 1998 Mirosoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "shadow.h"




HRESULT Init3DGeometry( LPDIRECT3DDEVICE3 pd3dDevice )
{
    DWORD i;
    HRESULT hr;

#define PWIDTH 5.5f
//#define DEPTHBELOW -4.7f
#define DEPTHBELOW -5.5f

    g_pvPolyVertices[0].p = D3DVECTOR( -PWIDTH, DEPTHBELOW, -PWIDTH );
    g_pvPolyVertices[1].p = D3DVECTOR(  PWIDTH, DEPTHBELOW, -PWIDTH );
    g_pvPolyVertices[2].p = D3DVECTOR(  PWIDTH, DEPTHBELOW,  PWIDTH );
    g_pvPolyVertices[3].p = D3DVECTOR( -PWIDTH, DEPTHBELOW,  PWIDTH );

    g_pvPolyVertices[0].c = RGBA_MAKE( 0xff, 0x00, 0x00, 0xff );
    g_pvPolyVertices[1].c = RGBA_MAKE( 0xff, 0x00, 0x00, 0xff );
    g_pvPolyVertices[2].c = RGBA_MAKE( 0xff, 0x00, 0x00, 0xff );
    g_pvPolyVertices[3].c = RGBA_MAKE( 0xff, 0x00, 0x00, 0xff );

    // Indices for a 1-sided square
    g_pwPolyIndices[0] = 0; g_pwPolyIndices[1] = 2; g_pwPolyIndices[2] = 1;
    g_pwPolyIndices[3] = 0; g_pwPolyIndices[4] = 3; g_pwPolyIndices[5] = 2;

    ZeroMemory( g_Shad,   NUM_SHADOWS*sizeof(SHADOW) );
    ZeroMemory( g_Caster, NUM_SHADOWS*sizeof(SHADOWCASTER) );

	// Get a ptr to the ID3D object to create materials and/or lights. Note:
	// the Release() call just serves to decrease the ref count.
    LPDIRECT3D3 pD3D;
    pd3dDevice->GetDirect3D( &pD3D );
    pD3D->Release();

    D3DVERTEXBUFFERDESC vbDesc;

    vbDesc.dwSize= sizeof(D3DVERTEXBUFFERDESC);
    vbDesc.dwCaps= D3DVBCAPS_SYSTEMMEMORY;  

        // Generate the object data
    D3DVECTOR sphcenter=D3DVECTOR(0.0f,0.0f,0.0f);
    WORD SphNumRings=g_TessLevs[g_CurTessLevel],SphNumSections=g_TessLevs[g_CurTessLevel];

    float SphRadius,SphXLen,SphYLen,SphZLen;
#define BARLENGTH 3.0f

    DWORD casternum=0;

    // one-bit stencil buffer case will use simpler geometry
    if( g_bUseOneBitStencil )
	{
        vbDesc.dwNumVertices= NUM_SHADOWS;  // this VB is just for z-ordering the casters
        vbDesc.dwFVF =  D3DFVF_XYZ | D3DFVF_DIFFUSE;  // this is a src vbuf
        if( FAILED( hr = pD3D->CreateVertexBuffer( &vbDesc, 
			               &g_pVB_castertestverts, D3DDP_DONOTCLIP, NULL ) ) )
          return hr;
    
        SphRadius = 1.0f;
        SphXLen   = 1.0f;
        SphYLen   = 0.4f;
        SphZLen   = 1.0f;
    
        sphcenter = D3DVECTOR( BARLENGTH, 0.0f, 0.0f );
        GenerateSphere( &g_Caster[casternum++], &sphcenter, SphRadius,
			            SphNumRings, SphNumSections,
                        SphXLen, SphYLen, SphZLen );
    
        sphcenter = D3DVECTOR( -BARLENGTH, 0.0f, 0.0f );
        GenerateSphere( &g_Caster[casternum++], &sphcenter, SphRadius, 
			            SphNumRings, SphNumSections,
                        SphXLen, SphYLen, SphZLen );
    
        sphcenter = D3DVECTOR( 0.0f, 0.0f, BARLENGTH );
        GenerateSphere( &g_Caster[casternum++], &sphcenter, SphRadius, 
			            SphNumRings, SphNumSections,
                        SphXLen, SphYLen, SphZLen );

        sphcenter = D3DVECTOR( 0.0f, 0.0f, -BARLENGTH );
        GenerateSphere( &g_Caster[casternum++], &sphcenter, SphRadius, 
			            SphNumRings, SphNumSections,
                        SphXLen, SphYLen, SphZLen );

        g_NumCasters = casternum;

         // spheres on plane -- to receive shadows
         // these wont really cast shadows -- cant since they would overlap and we've only got 1bit stenc

         SphNumRings=g_TessLevs[g_CurTessLevel],SphNumSections=g_TessLevs[g_CurTessLevel];
         SphRadius=2.0f;

         sphcenter=D3DVECTOR(-BARLENGTH,DEPTHBELOW,0.0f);

         GenerateSphere( &g_Caster[casternum++],&sphcenter,SphRadius, SphNumRings, SphNumSections,
                  SphXLen, SphYLen, SphZLen);

         sphcenter=D3DVECTOR(BARLENGTH,DEPTHBELOW,0.0f);

         GenerateSphere( &g_Caster[casternum++],&sphcenter,SphRadius, SphNumRings, SphNumSections,
                  SphXLen, SphYLen, SphZLen);

         sphcenter=D3DVECTOR(0.0f,DEPTHBELOW,-BARLENGTH);

         GenerateSphere( &g_Caster[casternum++],&sphcenter,SphRadius, SphNumRings, SphNumSections,
                  SphXLen, SphYLen, SphZLen);

         sphcenter=D3DVECTOR(0.0f,DEPTHBELOW,BARLENGTH);

         GenerateSphere( &g_Caster[casternum++],&sphcenter,SphRadius, SphNumRings, SphNumSections,
                  SphXLen, SphYLen, SphZLen);

        g_NumObjs=casternum;
    
    } else {

         SphRadius=1.0f,SphXLen=BARLENGTH,SphYLen=0.5f,SphZLen=0.5f;
    
         SphNumRings=g_TessLevs[g_CurTessLevel],SphNumSections=g_TessLevs[g_CurTessLevel];
        // since numrings/numsections is same for all spheres, indices will be the same,
        // so can use same counts and index array for all of them
    
    
         GenerateSphere( &g_Caster[casternum++],&sphcenter,SphRadius, SphNumRings, SphNumSections,
                  SphXLen, SphYLen, SphZLen);

         GenerateSphere( &g_Caster[casternum++],&sphcenter,SphRadius, SphNumRings, SphNumSections,
                  SphZLen, SphYLen, SphXLen);


         SphRadius=1.0f,SphXLen=1.0f,SphYLen=1.0f;SphZLen=1.0f;

         sphcenter=D3DVECTOR(BARLENGTH,0.0f,0.0f);

         GenerateSphere( &g_Caster[casternum++],&sphcenter,SphRadius, SphNumRings, SphNumSections,
                  SphXLen, SphYLen, SphZLen);

         sphcenter=D3DVECTOR(-BARLENGTH,0.0f,0.0f);

         GenerateSphere( &g_Caster[casternum++],&sphcenter,SphRadius, SphNumRings, SphNumSections,
                  SphXLen, SphYLen, SphZLen);

         sphcenter=D3DVECTOR(0.0f,0.0f,BARLENGTH);

         GenerateSphere( &g_Caster[casternum++],&sphcenter,SphRadius, SphNumRings, SphNumSections,
                  SphXLen, SphYLen, SphZLen);

         sphcenter=D3DVECTOR(0.0f,0.0f,-BARLENGTH);

         GenerateSphere( &g_Caster[casternum++],&sphcenter,SphRadius, SphNumRings, SphNumSections,
                  SphXLen, SphYLen, SphZLen);

         // above plane sph to receive shadows
         SphNumRings=g_TessLevs[g_CurTessLevel],SphNumSections=g_TessLevs[g_CurTessLevel];
         sphcenter=D3DVECTOR(0.0f,DEPTHBELOW/1.3f,0.0f);
         SphRadius=1.7f,SphXLen=1.0f,SphYLen=0.35f,SphZLen=1.0f;

         GenerateSphere( &g_Caster[casternum++],&sphcenter,SphRadius, SphNumRings, SphNumSections,
                  SphXLen, SphYLen, SphZLen);

         g_NumCasters=casternum;
    
         // spheres on plane -- to receive main shadows

         SphNumRings=g_TessLevs[g_CurTessLevel],SphNumSections=g_TessLevs[g_CurTessLevel];
         SphXLen=1.0f,SphZLen=1.0f; SphRadius=2.0f;          SphYLen=0.4f;

         sphcenter=D3DVECTOR(-BARLENGTH,DEPTHBELOW,0.0f);

         GenerateSphere( &g_Caster[casternum++],&sphcenter,SphRadius, SphNumRings, SphNumSections,
                  SphXLen, SphYLen, SphZLen);

         sphcenter=D3DVECTOR(BARLENGTH,DEPTHBELOW,0.0f);

         GenerateSphere( &g_Caster[casternum++],&sphcenter,SphRadius, SphNumRings, SphNumSections,
                  SphXLen, SphYLen, SphZLen);

         sphcenter=D3DVECTOR(0.0f,DEPTHBELOW,-BARLENGTH);

         GenerateSphere( &g_Caster[casternum++],&sphcenter,SphRadius, SphNumRings, SphNumSections,
                  SphXLen, SphYLen, SphZLen);

         sphcenter=D3DVECTOR(0.0f,DEPTHBELOW,BARLENGTH);

         GenerateSphere( &g_Caster[casternum++],&sphcenter,SphRadius, SphNumRings, SphNumSections,
                  SphXLen, SphYLen, SphZLen);

         g_NumObjs=casternum;
    }

    // set up vertex buffer to use as target for z-sorting shadow volumes in 1bit stencil mode and for
    // rendering shadow volumes

    g_MaxVertCount=0;

    for(i=0;i<g_NumObjs;i++) {
        g_Caster[i].pRVerts= new D3DVERTEX[g_Caster[i].VertCount];
        if(g_MaxVertCount<g_Caster[i].VertCount)
            g_MaxVertCount=g_Caster[i].VertCount;
    }

    assert(g_MaxVertCount>1);  

    vbDesc.dwFVF =  D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
    vbDesc.dwNumVertices= g_MaxVertCount*2;  // *2 to hold top of shadvol for infin light source
                                             // this is an big overestimate, the shadvol will not include all verts of obj (unless it's a simple polygon)
    hr = pD3D->CreateVertexBuffer(&vbDesc, &g_pVB_xformed, 0, 0);

    return hr;
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
                FLOAT ny = pvInVertices[i].ny;
                FLOAT nz = pvInVertices[i].nz;

                pvOutVertices[i]=pvInVertices[i];  // copy everything else

                pvOutVertices[i].y = fCos*y + fSin*z;
                pvOutVertices[i].z = -fSin*y + fCos*z;

                pvOutVertices[i].ny = fCos*ny + fSin*nz;
                pvOutVertices[i].nz = -fSin*ny + fCos*nz;
        }
}

VOID TransRotateVertexInX(D3DVECTOR &transvec, FLOAT fTheta, DWORD dwCount,
                                          D3DVERTEX* pvInVertices, D3DVERTEX* pvOutVertices )
{
    FLOAT fSin = (FLOAT)sin(fTheta); 
        FLOAT fCos = (FLOAT)cos(fTheta);
    
        for( DWORD i=0; i<dwCount; i++ )
        {
                FLOAT y = pvInVertices[i].y+transvec.y;
                FLOAT z = pvInVertices[i].z+transvec.z;
                FLOAT ny = pvInVertices[i].ny;
                FLOAT nz = pvInVertices[i].nz;

                pvOutVertices[i]=pvInVertices[i];  // copy everything else

                pvOutVertices[i].y = fCos*y + fSin*z - transvec.y;
                pvOutVertices[i].z = -fSin*y + fCos*z - transvec.z;

                pvOutVertices[i].ny = fCos*ny + fSin*nz;
                pvOutVertices[i].nz = -fSin*ny + fCos*nz;
        }
}

VOID RotateVertexInZ( FLOAT fTheta, DWORD dwCount,
                                          D3DVERTEX* pvInVertices, D3DVERTEX* pvOutVertices )
{
    FLOAT fSin = (FLOAT)sin(fTheta); 
        FLOAT fCos = (FLOAT)cos(fTheta);
    
        for( DWORD i=0; i<dwCount; i++ )
        {

                FLOAT x = pvInVertices[i].x;
                FLOAT y = pvInVertices[i].y;
                FLOAT ny = pvInVertices[i].ny;
                FLOAT nx = pvInVertices[i].nx;

                pvOutVertices[i]=pvInVertices[i];  // copy everything else

                pvOutVertices[i].x = fCos*x + fSin*y;
                pvOutVertices[i].y = -fSin*x + fCos*y;


                pvOutVertices[i].nx = fCos*nx + fSin*ny;
                pvOutVertices[i].ny = -fSin*nx + fCos*ny;
        }
}

VOID TransRotateVertexInZ(D3DVECTOR &transvec, FLOAT fTheta, DWORD dwCount,
                                          D3DVERTEX* pvInVertices, D3DVERTEX* pvOutVertices )
{
    FLOAT fSin = (FLOAT)sin(fTheta); 
        FLOAT fCos = (FLOAT)cos(fTheta);
    
        for( DWORD i=0; i<dwCount; i++ )
        {

                FLOAT x = pvInVertices[i].x+transvec.x;
                FLOAT y = pvInVertices[i].y+transvec.y;
                FLOAT ny = pvInVertices[i].ny;
                FLOAT nx = pvInVertices[i].nx;

                pvOutVertices[i]=pvInVertices[i];  // copy everything else

                pvOutVertices[i].x = fCos*x + fSin*y - transvec.x;
                pvOutVertices[i].y = -fSin*x + fCos*y - transvec.y;


                pvOutVertices[i].nx = fCos*nx + fSin*ny;
                pvOutVertices[i].ny = -fSin*nx + fCos*ny;
        }
}


VOID TransRotateVertexInY(D3DVECTOR &transvec, FLOAT fTheta, DWORD dwCount,
                                          D3DVERTEX* pvInVertices, D3DVERTEX* pvOutVertices )
{
    FLOAT fSin = (FLOAT)sin(fTheta); 
        FLOAT fCos = (FLOAT)cos(fTheta);
    
        for( DWORD i=0; i<dwCount; i++ )
        {

                FLOAT x = pvInVertices[i].x+transvec.x;
                FLOAT z = pvInVertices[i].z+transvec.z;
                FLOAT nz = pvInVertices[i].nz;
                FLOAT nx = pvInVertices[i].nx;

                pvOutVertices[i]=pvInVertices[i];  // copy everything else first

                pvOutVertices[i].x = fCos*x + fSin*z - transvec.x;
                pvOutVertices[i].z = -fSin*x + fCos*z - transvec.z;


                pvOutVertices[i].nx = fCos*nx + fSin*nz;
                pvOutVertices[i].nz = -fSin*nx + fCos*nz;
        }
}

VOID RotateVertexInY( FLOAT fTheta, DWORD dwCount,
                                          D3DVERTEX* pvInVertices, D3DVERTEX* pvOutVertices )
{
    FLOAT fSin = (FLOAT)sin(fTheta); 
        FLOAT fCos = (FLOAT)cos(fTheta);
    
        for( DWORD i=0; i<dwCount; i++ )
        {

                FLOAT x = pvInVertices[i].x;
                FLOAT z = pvInVertices[i].z;
                FLOAT nz = pvInVertices[i].nz;
                FLOAT nx = pvInVertices[i].nx;

                pvOutVertices[i]=pvInVertices[i];  // copy everything else first

                pvOutVertices[i].x = fCos*x + fSin*z;
                pvOutVertices[i].z = -fSin*x + fCos*z;

                pvOutVertices[i].nx = fCos*nx + fSin*nz;
                pvOutVertices[i].nz = -fSin*nx + fCos*nz;
        }
}




//-----------------------------------------------------------------------------
// Name: GenerateSphere()
// Desc: Makes vertex and index data for a sphere.
//-----------------------------------------------------------------------------
BOOL GenerateSphere(SHADOWCASTER *obj,D3DVECTOR *center, FLOAT fRadius, WORD wNumRings, WORD wNumSections,
                                         FLOAT sx, FLOAT sy, FLOAT sz)
{
    FLOAT x, y, z, v, rsintheta; // Temporary variables
    WORD  i, j, n, m;            // counters
    D3DVECTOR vPoint;

    //Generate space for the required triangles and vertices.
    WORD       wNumTriangles = (wNumRings + 1) * wNumSections * 2;
    DWORD      dwNumIndices   = wNumTriangles*3;
    DWORD      dwNumVertices  = (wNumRings + 1) * wNumSections + 2;
    D3DVERTEX* pvVertices     = new D3DVERTEX[dwNumVertices];
    WORD*      pwIndices      = new WORD[3*wNumTriangles];

    // Generate vertices at the top and bottom points.
    D3DVECTOR vTopPoint  = D3DVECTOR( center->x+ 0.0f, center->y+sy*fRadius, center->z+0.0f);
    D3DVECTOR vBotPoint  = D3DVECTOR( center->x+ 0.0f, center->y-sy*fRadius, center->z+0.0f);
    D3DVECTOR vNormal = D3DVECTOR( 0.0f, 0.0f, 1.0f );
    pvVertices[0]               = D3DVERTEX( vTopPoint,  vNormal, 0.0f, 0.0f );
    pvVertices[dwNumVertices-1] = D3DVERTEX( vBotPoint, -vNormal, 0.0f, 0.0f );

    // Generate vertex points for rings
    FLOAT dtheta = (float)(g_PI / (wNumRings + 2));     //Angle between each ring
    FLOAT dphi   = (float)(2*g_PI / wNumSections); //Angle between each section
    FLOAT theta  = dtheta;
    n = 1; //vertex being generated, begins at 1 to skip top point

    for( i = 0; i < (wNumRings+1); i++ )
        {
        y = fRadius * (float)cos(theta); // y is the same for each ring
        v = theta / g_PI;     // v is the same for each ring
        rsintheta = fRadius * (float)sin(theta);
        FLOAT phi = 0.0f;

        for( j = 0; j < wNumSections; j++ )
                {
            x = rsintheta * (float)sin(phi);
            z = rsintheta * (float)cos(phi);
        
            FLOAT u = (FLOAT)(1.0 - phi / (2*g_PI) );
            
            vPoint        = D3DVECTOR( center->x+sx*x, center->y+sy*y, center->z+sz*z );
            vNormal       = D3DVECTOR( x/fRadius, y/fRadius, z/fRadius );
            pvVertices[n] = D3DVERTEX( vPoint, vNormal, u, v );

            phi += dphi;
            ++n;
        }
        theta += dtheta;
    }

    // Generate triangles for top and bottom caps.
    for( i = 0; i < wNumSections; i++ )
        {
        pwIndices[3*i+0] = 0;
        pwIndices[3*i+1] = i + 1;
        pwIndices[3*i+2] = 1 + ((i + 1) % wNumSections);

        pwIndices[3*(wNumTriangles - wNumSections + i)+0] = (WORD)( dwNumVertices - 1 );
        pwIndices[3*(wNumTriangles - wNumSections + i)+1] = (WORD)( dwNumVertices - 2 - i );
        pwIndices[3*(wNumTriangles - wNumSections + i)+2] = (WORD)( dwNumVertices - 2 - 
                ((1 + i) % wNumSections) );
    }

    // Generate triangles for the rings
    m = 1;            // first vertex in current ring,begins at 1 to skip top point
    n = wNumSections; // triangle being generated, skip the top cap 
        
    for( i = 0; i < wNumRings; i++ )
        {
        for( j = 0; j < wNumSections; j++ )
                {
            pwIndices[3*n+0] = m + j;
            pwIndices[3*n+1] = m + wNumSections + j;
            pwIndices[3*n+2] = m + wNumSections + ((j + 1) % wNumSections);
            
            pwIndices[3*(n+1)+0] = pwIndices[3*n+0];
            pwIndices[3*(n+1)+1] = pwIndices[3*n+2];
            pwIndices[3*(n+1)+2] = m + ((j + 1) % wNumSections);
            
            n += 2;
        }
        m += wNumSections;
    }

    obj->VertCount= dwNumVertices;
    obj->IndexCount= dwNumIndices;
    obj->pIndices = pwIndices;
    obj->pVerts = pvVertices;
    obj->center = *center;

    return TRUE;
}



