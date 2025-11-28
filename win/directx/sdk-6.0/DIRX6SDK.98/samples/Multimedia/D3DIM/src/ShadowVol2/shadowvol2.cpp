//-----------------------------------------------------------------------------
// File: Shadowvol2.cpp
//
// Desc: Example code showing how to use stencil buffers to implement shadows
//
//       Note: This code uses the D3D Framework helper library.
//
//
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "shadow.h"




//-----------------------------------------------------------------------------
// Declare the application globals for use in WinMain.cpp
//-----------------------------------------------------------------------------
BOOL   g_bAppUseZBuffer    = FALSE;
BOOL   g_bAppUseBackBuffer = TRUE;
TCHAR* g_strAppTitle       = TEXT( "Stencil Shadows Example" );




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define BAR_TEXTURENAME      TEXT("banana.bmp")
#define SPHERE_TEXTURENAME   TEXT("banana.bmp")
#define OBJ_TEXTURENAME      TEXT("stripe2.bmp")
#define RECEIVER_TEXTURENAME TEXT("stripe.bmp")

COLORVERTEX  g_pvPolyVertices[NUM_POLY_VERTS];  // Vertices of square
WORD         g_pwPolyIndices[NUM_POLY_INDICES]; // Indices of square's tris
SHADOW       g_Shad[NUM_SHADOWS];             
SHADOWCASTER g_Caster[NUM_SHADOWS];

DWORD g_MaxVertCount = 0L;
DWORD g_NumCasters; // number of active shadow caster objects
DWORD g_NumObjs;    // number of casters+receivers

// "tmp" vertex buffers used to compute shadow volumes and sort shadows in Z
LPDIRECT3DVERTEXBUFFER g_pVB_xformed         = NULL;
LPDIRECT3DVERTEXBUFFER g_pVB_castertestverts = NULL;

// Defines several user-selectable levels of tesselation
WORD  g_TessLevs[NUMTESSLEVELS] = {4,8,17,28};
DWORD g_CurTessLevel = 0;

DWORD g_max_StencilVal;   // maximum value the stencil buffer will hold

LPDIRECTDRAWSURFACE4 g_pddsDepthBuffer = NULL;
LPDIRECT3DMATERIAL3  g_pmtrlObjectMtrl = NULL;
LPDIRECT3DLIGHT      g_pLight          = NULL;

BOOL g_bUseOneBitStencil       = FALSE; // Use one-bit stencil buffer algorithm
BOOL g_bCanOnlyDoOneBitStencil = FALSE; // Stencil buffer only holds 1 bit
BOOL g_bDrawShadowVolumes      = FALSE; // Instead of shadows, draw transparent shadow volumes
BOOL g_bDrawShadowVolCaps      = TRUE;  // Draw tops of shadowvols (needed for correct rendering if top would be visible)
BOOL g_bSortZInOneBitStencil   = TRUE;  // Render shadvols in sorted z order, front-to-back (needed for correct rendering in 1-bit mode)
BOOL g_bDoShadows              = TRUE;  // Draw shadows/shadow volumes
BOOL g_bShowQuarterView        = FALSE; // Show scene from different viewpoint

BOOL g_bViewChanged            = FALSE; // Viewpoint changed from last frame
BOOL g_bSwitchBitModes         = FALSE; // Changed to/from 1-bit algorithm this frame
BOOL g_bReInitObjs             = FALSE; // Need to reinit geometry

BOOL g_bInvertStencilBufferSense = FALSE; // if true, count down from max stencil value instead of counting up from 0
D3DSTENCILOP g_StencDecOp,g_StencIncOp;     

HMENU g_hMenu;




//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
VOID    AppPause( BOOL );
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    ResetViewMatrix( LPDIRECT3DDEVICE3 );
VOID    DeleteShadowObjects();
VOID    SetMenuStates( HMENU hMenu );
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK App_OverridenWndProc( HWND, UINT, WPARAM, LPARAM );




//-----------------------------------------------------------------------------
// Name: MakeShadowVolume()
// Desc: Build the tris of the shadow volume for a directional light
//       InVerts are copied into a vertex buffer created within *shad
//-----------------------------------------------------------------------------
HRESULT MakeShadowVolume( LPDIRECT3DDEVICE3 pd3dDevice, SHADOW *shad,
                          D3DVERTEX *InVerts, DWORD nverts, D3DVECTOR *light )
{
	D3DMATRIX matWorld,matView,matProj,IDmat;
	DWORD i;
	HRESULT hr;

	// Get a ptr to the ID3D object to create materials and/or lights. Note:
	// the Release() call just serves to decrease the ref count.
	LPDIRECT3D3 pD3D;
	pd3dDevice->GetDirect3D( &pD3D );
	pD3D->Release();

	LPDIRECT3DVERTEXBUFFER VB_Proj;
	D3DVERTEXBUFFERDESC vbDesc;
	vbDesc.dwSize = sizeof(D3DVERTEXBUFFERDESC);
	vbDesc.dwCaps = D3DVBCAPS_SYSTEMMEMORY;
	vbDesc.dwFVF  =  D3DFVF_XYZ | D3DFVF_DIFFUSE;
	// xyz+color so we can render them in showshadvol mode

	// Create vertex buffer to hold shadow volumes verts
	if( shad->VB==NULL )
	{
		// else re-use old VB
		assert(shad->pwShadVolIndices==NULL);

		// now form array of indices that will make the tris

		ZeroMemory( shad, sizeof(SHADOW) );
		shad->num_objverts=nverts;
		vbDesc.dwNumVertices= nverts*2;  // *2 to hold top of shadvol for infin light source

		if( FAILED( hr = pD3D->CreateVertexBuffer( &vbDesc, &shad->VB, 0, 0 ) ) )
			return hr;

		// alloc enough to hold largest-case shadvol (max # of verts in c-hull is nverts)
		// (nverts+1)*2 for tri mesh to hold shadvol sides + nverts to hold tri-fan
		shad->pwShadVolIndices = new WORD[(nverts+1)*3];
	} 

	// create VB_Proj vertex buffer as a target for the vertex-projection operation used to compute
	// the silhouette

	vbDesc.dwNumVertices = nverts;
	vbDesc.dwFVF =  D3DFVF_XYZRHW;
	// even though RHW not used, must specify it or ProcessVerts will not consider this as a valid
	// target to xform verts into

	if( FAILED( hr = pD3D->CreateVertexBuffer(&vbDesc, &VB_Proj, D3DDP_DONOTCLIP, NULL ) ) )
		return hr;

	// must lock VB, then copy verts into its space.

	COLORVERTEX *VBvertptr;

	shad->VB->Lock( DDLOCK_NOSYSLOCK | DDLOCK_WRITEONLY | DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,
		            (VOID**) &VBvertptr,NULL);

	// have to copy verts into VB memory.  I reformat into COLORVERTEX to do this.
	// could prevent reformat and do a straight memcpy if Find2DConvexHull used D3DVERTEX tho.

	COLORVERTEX* cvptr   = VBvertptr;
	D3DVERTEX*   d3dvptr = InVerts;

	// reformat D3DVERTEX array to COLORVERTEX array
	for( i=0;i<nverts;i++ ) 
	{
		cvptr->p = *((D3DVECTOR*)(&d3dvptr->x));
		cvptr->c = RGBA_MAKE(0x0,0x0,0x0,0x5f);  // shadvol is semi-transparent black
		cvptr++;  d3dvptr++;
	}

	shad->VB->Unlock();

	// save cur matrices so we can use xform pipeln to project verts supafast
	pd3dDevice->GetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
	pd3dDevice->GetTransform( D3DTRANSFORMSTATE_VIEW,       &matView );
	pd3dDevice->GetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

	D3DUtil_SetIdentityMatrix(IDmat);

	pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &IDmat);
	pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &IDmat);


	// for view matrix, all we want is anything that projects the verts onto a plane
	// perp to light direction.  so any eyepoint is OK (try to make obj near origin though,
	// so look at one of the verts).  dont care what direction is view up vector (y).

	D3DVECTOR at   = VBvertptr[0].p;
	D3DVECTOR from = at - 7.0f*(*light);  // make sure eye is far enough behind obj
	D3DVECTOR up;

	// anything perp to light vector is OK 
	if((light->y==0.0f) && (light->x==0.0f))
	{
		up = D3DVECTOR(0.0f, 1.0f, 0.0f);  
	} 
	else 
	{
		up = D3DVECTOR(light->y, -light->x, 0.0f);  
	}

	D3DMATRIX newView;
	D3DUtil_SetViewMatrix( newView, from, at, up);
	pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &newView );

	// do the planar projection
	VB_Proj->ProcessVertices(D3DVOP_TRANSFORM,
							   0,  // write new verts at idx 0
							   nverts,
							   shad->VB,
							   0,  // read src verts from idx 0
							   pd3dDevice,
							   0x0); // no flags

	pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
	pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW,       &matView );
	pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

	COLORVERTEX *pntptr;

	VB_Proj->Lock(DDLOCK_NOSYSLOCK | DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,
		   (void**) &pntptr,NULL);  

	WORD *OutHullIdxs;
	DWORD n_idxs;

	Find2DConvexHull(nverts,pntptr,&n_idxs,&OutHullIdxs);

	VB_Proj->Unlock();
	VB_Proj->Release();   // just needed the indices of hull

	shad->VB->Lock(DDLOCK_NOSYSLOCK | DDLOCK_WRITEONLY | DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,
			 (void **) &VBvertptr,NULL);

	// make shadow volume by taking hull verts and project them along light dir far enough
	// to be offscreen

	// add verts to end of VB
	for(i=0;i<n_idxs;i++) 
	{
		VBvertptr[nverts+i].p = VBvertptr[OutHullIdxs[i]].p + 20.0f*(*light);  // scale factor of 10 should be enough
		VBvertptr[nverts+i].c = RGBA_MAKE(0x0,0xff,0x0,0x7f);  
	}

	shad->totalverts=nverts+n_idxs;

	// now form array of indices that will make the tris
	// shad vol will have n_idxs square sides

	shad->num_side_indices = (n_idxs+1)*2;

	// if shadvol is not capped, shadow may be drawn in place where a backfacing cap is missing even
	// though no geometry is there

	if(g_bDrawShadowVolCaps)
	   shad->num_cap_indices=n_idxs;
	 else shad->num_cap_indices=0;

	WORD *idxptr;

	idxptr=shad->pwShadVolSideIndices=shad->pwShadVolIndices;

	// tris for all facets but final one
	for(i=0;i<n_idxs;i++) 
	{
		// outhullidx[i] is the index of the ith vertex of the n_idx convex hull verts
		// nverts+i is the index of the projected vert corresponding to the OutHullIdx[i] vertex

		*idxptr++ = OutHullIdxs[i];  
		*idxptr++ = (WORD) (nverts+i);  
	}
	// add tris for final facet (i==n_idxs)

	*idxptr++ = OutHullIdxs[0];  
	*idxptr++ = (WORD) (nverts+0);  

	shad->pwShadVolCapIndices=idxptr;

	if(g_bDrawShadowVolCaps) 
	{
		for(i=(nverts+n_idxs-1);i>=nverts;i--) 
		{
			// draw a fan over the shadvolume cap.  note I only do the back-facing cap here (which is why I count backwards)
			*idxptr++ = (WORD) i;
		}
	}

	free(OutHullIdxs);   // allocated by Find2DConvexHull
	shad->VB->Unlock();

	return D3D_OK;
}




//-----------------------------------------------------------------------------
// Name: SetMenuStates()
// Desc: init menu state based on app globals
//-----------------------------------------------------------------------------
VOID SetMenuStates( HMENU hMenu )
{
	// set the state of all the menus
	CheckMenuItem(  hMenu, IDM_DO_SHADOWS, (g_bDoShadows)?MF_CHECKED:MF_UNCHECKED );

	BOOL bIn1BitMode;

	if(g_bSwitchBitModes)
		bIn1BitMode=!g_bUseOneBitStencil;
	else 
		bIn1BitMode=g_bUseOneBitStencil;

	if(!g_bDoShadows) 
	{
		EnableMenuItem( hMenu, IDM_SHOWSHADVOL, MF_GRAYED );
		EnableMenuItem( hMenu, IDM_1BITMODE, MF_GRAYED );
		EnableMenuItem( hMenu, IDM_SORTEDZ, MF_GRAYED );
		EnableMenuItem( hMenu, IDM_DRAWSHADOWVOLUMECAPS, MF_GRAYED );
	} 
	else 
	{
		EnableMenuItem( hMenu, IDM_SHOWSHADVOL, MF_ENABLED);
		EnableMenuItem( hMenu, IDM_1BITMODE, MF_ENABLED);
		EnableMenuItem( hMenu, IDM_DRAWSHADOWVOLUMECAPS, MF_ENABLED);
		if(bIn1BitMode)
			EnableMenuItem( hMenu, IDM_SORTEDZ, MF_ENABLED );
		else 
			EnableMenuItem( hMenu, IDM_SORTEDZ, MF_GRAYED);
	}

	CheckMenuItem(  hMenu, IDM_SHOWSHADVOL, (g_bDrawShadowVolumes)?MF_CHECKED:MF_UNCHECKED );
	CheckMenuItem(  hMenu, IDM_1BITMODE,   (bIn1BitMode)?MF_CHECKED:MF_UNCHECKED );
	CheckMenuItem(  hMenu, IDM_SORTEDZ,    (g_bSortZInOneBitStencil)?MF_CHECKED:MF_UNCHECKED );
	CheckMenuItem(  hMenu, IDM_DRAWSHADOWVOLUMECAPS,  (g_bDrawShadowVolCaps)?MF_CHECKED:MF_UNCHECKED );
	CheckMenuItem(  hMenu, IDM_VIEWPOINT, (g_bShowQuarterView)?MF_CHECKED:MF_UNCHECKED );

	for(DWORD i=0;i<NUMTESSLEVELS;i++)
	{
		CheckMenuItem(  hMenu, IDM_TESS1+i, (i==g_CurTessLevel)?MF_CHECKED:MF_UNCHECKED );
	}
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
        if( LOWORD(wParam)>=IDM_TESS1 && LOWORD(wParam)<=IDM_TESS4 ) 
		{
            g_CurTessLevel = LOWORD(wParam) - IDM_TESS1;
            g_bReInitObjs  = TRUE;
        } 
		else switch( LOWORD(wParam) )
        {
            case IDM_DO_SHADOWS:
                g_bDoShadows=!g_bDoShadows;
                break;

            case IDM_SHOWSHADVOL:
                g_bDrawShadowVolumes=!g_bDrawShadowVolumes;
                break;

            case IDM_DRAWSHADOWVOLUMECAPS:
                g_bDrawShadowVolCaps=!g_bDrawShadowVolCaps;
                break;

            case IDM_1BITMODE:
                if(g_bUseOneBitStencil && g_bCanOnlyDoOneBitStencil)
                    break;
                g_bSwitchBitModes=TRUE;
                break;

            case IDM_SORTEDZ:
                g_bSortZInOneBitStencil=!g_bSortZInOneBitStencil;
                break;

            case IDM_VIEWPOINT:
                g_bShowQuarterView=!g_bShowQuarterView;
                g_bViewChanged=TRUE;
                break;
        }

        // Update the menu to reflect any new changes
        SetMenuStates( g_hMenu );
    }
    return WndProc( hWnd, uMsg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{
	// Add a menu and a message handler to the program
	g_hMenu = GetMenu( hWnd );
	SetWindowLong( hWnd, GWL_WNDPROC, (LONG)App_OverridenWndProc );

	// Create some textures
	D3DTextr_CreateTexture( BAR_TEXTURENAME );
	D3DTextr_CreateTexture( OBJ_TEXTURENAME );
	D3DTextr_CreateTexture( SPHERE_TEXTURENAME );
	D3DTextr_CreateTexture( RECEIVER_TEXTURENAME );

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
    static FLOAT fRotateViewAngle = 0.0f;
    static FLOAT fRotateAngleY    = 0.0f;
    static FLOAT fRotateAngleX    = 0.0f;
    static FLOAT fDegToRad        = (g_PI/180.0f);
    static FLOAT fRotateAngleXInc = 2.0f*fDegToRad;
    static FLOAT fRotateAngleYInc = 20.0f*fDegToRad;
    static DWORD dwFrameCount     = 0L;
    FLOAT lx,ly,lz;
    DWORD i;
    HRESULT hr;

    dwFrameCount++;

    if( (dwFrameCount%11)==10 )
        g_bInvertStencilBufferSense = !g_bInvertStencilBufferSense;

    if( g_bViewChanged ) 
	{
        g_bViewChanged = FALSE;
        ResetViewMatrix( pd3dDevice );
    }

    if( g_bSwitchBitModes )
	{
        g_bSwitchBitModes   = FALSE;
        g_bUseOneBitStencil = !g_bUseOneBitStencil;
        g_bReInitObjs       = TRUE;
    }

    if( g_bReInitObjs ) 
	{
       // Free old geom, make new stuff
       g_bReInitObjs = FALSE;

       DeleteShadowObjects();

       if( FAILED( hr = Init3DGeometry( pd3dDevice ) ) )
           return hr;
    }

    fRotateViewAngle += 1*fDegToRad;

    lx =  0.6f*(FLOAT)cos(5*fRotateViewAngle*fDegToRad);
    ly = -1.0f;
    lz =  0.6f*(FLOAT)sin(5*fRotateViewAngle*fDegToRad);

    if( g_bUseOneBitStencil )
	{
		// Reduce amplitude to ensure shadows dont overlap
		lx*=0.5f;  lz*=0.5f;

		// Keep rot angle under control to insure shadows dont overlap
		if( fRotateAngleX < -(g_PI/4.0f) ) 
		{
			fRotateAngleXInc = -fRotateAngleXInc;
            fRotateAngleX    = -(g_PI/4.0f);  
			// Note: might have been in non-1bit mode, so need to full reset
		} 
		else if( fRotateAngleX > (g_PI/4.0f) ) 
		{
			fRotateAngleXInc = -fRotateAngleXInc;
			fRotateAngleX    = (g_PI/4.0f);  
			// Note: might have been in non-1bit mode, so need to full reset
		}
    } 
        
    fRotateAngleY += fRotateAngleYInc;
    fRotateAngleX += fRotateAngleXInc;

    D3DVECTOR light = D3DVECTOR( lx, ly, lz );

    // Setup the world spin matrix
    D3DMATRIX matWorldSpin;
    D3DUtil_SetRotateYMatrix( matWorldSpin, fRotateViewAngle );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorldSpin );

    D3DVERTEX *pTempVerts = new D3DVERTEX[g_MaxVertCount];

    for(i=0;i<g_NumObjs;i++) 
	{
        if(i>=g_NumCasters) 
		{
			// dont move receivers 
			memcpy(g_Caster[i].pRVerts, g_Caster[i].pVerts, g_Caster[i].VertCount*sizeof(D3DVERTEX));
			continue;
        }
        if(g_bUseOneBitStencil) 
		{
            if(g_Caster[i].center.z!=0.0)  
				TransRotateVertexInX(-g_Caster[i].center, -fRotateAngleY, g_Caster[i].VertCount, g_Caster[i].pVerts,g_Caster[i].pRVerts);
            else 
				TransRotateVertexInZ(-g_Caster[i].center, fRotateAngleY,g_Caster[i].VertCount, g_Caster[i].pVerts,g_Caster[i].pRVerts);
            RotateVertexInY( fRotateAngleY,g_Caster[i].VertCount, g_Caster[i].pRVerts, pTempVerts); 
        } 
		else 
		{
            if(i==g_NumCasters-1)
				TransRotateVertexInY(-g_Caster[i].center, fRotateAngleY, g_Caster[i].VertCount, g_Caster[i].pVerts, pTempVerts);
			else 
				RotateVertexInY( fRotateAngleY, g_Caster[i].VertCount, g_Caster[i].pVerts, pTempVerts); 
        }

        if( (!g_bUseOneBitStencil) && (i==g_NumCasters-1) ) 
		{
            TransRotateVertexInZ(-g_Caster[i].center, fRotateAngleX*2.0f, g_Caster[i].VertCount, pTempVerts, g_Caster[i].pRVerts);
        } 
		else 
		{
			RotateVertexInX( fRotateAngleX, g_Caster[i].VertCount, pTempVerts, g_Caster[i].pRVerts);
        }
    }

    delete pTempVerts;

    if(g_bDoShadows) 
	{
        // make new shadow volumes from xformed objects
        for(i=0;i<g_NumCasters;i++) 
		{
            if( FAILED( hr = MakeShadowVolume(pd3dDevice,&g_Shad[i],g_Caster[i].pRVerts,g_Caster[i].VertCount,&light) ) )
               return hr;
        }
    }

    D3DLIGHT light_struct;
    D3DUtil_InitLight( light_struct, D3DLIGHT_DIRECTIONAL, lx,ly,lz); 
    g_pLight->SetLight( &light_struct );

    return D3D_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderShadow()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT RenderShadow( LPDIRECT3DDEVICE3 pd3dDevice, SHADOW* pShad,
					  LPDIRECT3DVERTEXBUFFER lpVBuf )
{
    // Turn depth buffer off, and stencil buffer on
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ZWRITEENABLE,  FALSE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SHADEMODE, D3DSHADE_FLAT );  // dont want to bother interpolating color

    // Set up stencil compare fuction, reference value, and masks
    // Stencil test passes if ((ref & mask) cmpfn (stencil & mask)) is true
    pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILFUNC,     D3DCMP_ALWAYS );

    pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILZFAIL, D3DSTENCILOP_KEEP );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILFAIL,  D3DSTENCILOP_KEEP );

    if(g_bUseOneBitStencil) 
	{
        // If ztest passes, write !(g_bInvertStencilBufferSense) into stencil buffer
        pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILREF,  (g_bInvertStencilBufferSense)? 0x0 : 0x1);
        pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILMASK,     0x1 );
        pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILWRITEMASK,0x1 );
        pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILPASS,  D3DSTENCILOP_REPLACE );
    } 
	else 
	{
        // If ztest passes, inc/decrement stencil buffer value
        pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILREF,      0x1 );
        pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILMASK,     0xffffffff );
        pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILWRITEMASK,0xffffffff );
        pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILPASS, (g_bInvertStencilBufferSense)? g_StencDecOp: g_StencIncOp);
    }

    // Since destcolor=SRCBLEND * SRC_COLOR + DESTBLEND * DEST_COLOR,
    // this should result in the tri color being completely dropped
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,  D3DBLEND_ZERO );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE );

    // draw front-side of shadow volume in stencil/z only

    pd3dDevice->DrawIndexedPrimitiveVB( D3DPT_TRIANGLESTRIP, /*pShad->VB*/lpVBuf,
                                        pShad->pwShadVolSideIndices,
                                        pShad->num_side_indices, 0x0);
    if(g_bDrawShadowVolCaps) 
	{
        pd3dDevice->DrawIndexedPrimitiveVB( D3DPT_TRIANGLEFAN,/*pShad->VB*/lpVBuf,
                                            pShad->pwShadVolCapIndices,
                                            pShad->num_cap_indices, 0x0);
    }

    // Now reverse cull order so back sides of shadow volume are written.

    if(g_bUseOneBitStencil) 
	{
        // write 0's/1's into stencil buffer to erase pixels beyond back of shadow
        pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILREF,  (g_bInvertStencilBufferSense)? 0x1 : 0x0);
    } 
	else 
	{
        // increment stencil buffer value
        pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILPASS, (g_bInvertStencilBufferSense)? g_StencIncOp: g_StencDecOp);
    }

    pd3dDevice->SetRenderState( D3DRENDERSTATE_CULLMODE,   D3DCULL_CW );

    // Draw back-side of shadow volume in stencil/z only
    pd3dDevice->DrawIndexedPrimitiveVB( D3DPT_TRIANGLESTRIP, lpVBuf,
                                            pShad->pwShadVolSideIndices,
                                            pShad->num_side_indices, 0x0);

    if(g_bDrawShadowVolCaps) 
	{
        pd3dDevice->DrawIndexedPrimitiveVB( D3DPT_TRIANGLEFAN, lpVBuf,
                                            pShad->pwShadVolCapIndices,
                                            pShad->num_cap_indices, 0x0);
    }

    // Restore render states
    pd3dDevice->SetRenderState( D3DRENDERSTATE_CULLMODE, D3DCULL_CCW );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ZWRITEENABLE,     TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILENABLE,    FALSE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD );
    
	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DrawShadow()
// Desc: Draws a big grey polygon over scene, and blend it with pixels with
//       stencil 1, which are in shadow.  Could optimize this by keeping track
//       of rendered 2D extent rect of all shadow vols.
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

    // stencil cmp func is defined as (ref cmpfn stencbufval).

    if(g_bInvertStencilBufferSense) 
	{
        // Only write where stencil val < maxstencilval  
        pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILREF,(g_bUseOneBitStencil)?0x1:g_max_StencilVal);
        pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILFUNC, D3DCMP_GREATER );
    } 
	else 
	{
		// Only write where stencil val >= 1.  (count indicates # of shadows that overlap that pixel)
        pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILREF,  0x1 );
        pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILFUNC, D3DCMP_LESSEQUAL );
    }

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
    sqverts[0].c = sqverts[1].c = sqverts[2].c = sqverts[3].c = RGBA_MAKE(0x0,0x0,0x0,0x7f);  // blend .5 black

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
	DWORD i;

	pvViewport->Clear2( 1UL, prcViewportRect, dwClearFlags, dwClearColor, 1.0f, (g_bInvertStencilBufferSense)? ((g_bUseOneBitStencil)?0x1:g_max_StencilVal) : 0x0);

	// Begin the scene
	if( SUCCEEDED( pd3dDevice->BeginScene() ) )
	{
		// Draw ground square
		pd3dDevice->SetTexture( 0, NULL);

		pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,(D3DFVF_XYZ | D3DFVF_DIFFUSE),
		g_pvPolyVertices, 4, g_pwPolyIndices, 6, NULL );

		if(g_bUseOneBitStencil) 
		{
			pd3dDevice->SetTexture( 0, D3DTextr_GetTexture(SPHERE_TEXTURENAME) );

			for(i=0;i<g_NumObjs;i++)
			{
				if(i==g_NumCasters)
					pd3dDevice->SetTexture( 0, D3DTextr_GetTexture(RECEIVER_TEXTURENAME) );

				pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
												  g_Caster[i].pRVerts, g_Caster[i].VertCount,
												  g_Caster[i].pIndices, g_Caster[i].IndexCount, NULL );
			}
		} 
		else 
		{
			pd3dDevice->SetTexture( 0, D3DTextr_GetTexture(BAR_TEXTURENAME) );

			for(i=0;i<g_NumObjs;i++)
			{
				if(i==2)
					pd3dDevice->SetTexture( 0, D3DTextr_GetTexture(SPHERE_TEXTURENAME) );
				else if(i==g_NumCasters)
					pd3dDevice->SetTexture( 0, D3DTextr_GetTexture(RECEIVER_TEXTURENAME) );
				else if(i==(g_NumCasters-1))
					pd3dDevice->SetTexture( 0, D3DTextr_GetTexture(OBJ_TEXTURENAME) );

				pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
												  g_Caster[i].pRVerts, g_Caster[i].VertCount,
												  g_Caster[i].pIndices, g_Caster[i].IndexCount, NULL );
			}
		}

		pd3dDevice->SetTexture( 0, NULL );

		if( g_bDoShadows )
		{
			SHADOW *shadorder[NUM_SHADOWS];
			DWORD shadnum;

			for(shadnum=0;shadnum<g_NumCasters;shadnum++)
			{
				shadorder[shadnum]=&g_Shad[shadnum];
			}

			if( g_bUseOneBitStencil && g_bSortZInOneBitStencil )
			{
				// need to render in front-to-back order so that backside of front shadow vols
				// do not erase stencil pixels set by background shadow vols.
				// shadowvols are restricted to be non-overlapping in Z, might be able to allow
				// overlapping shadows as long as I could guarantee I could render the shadvols sorted
				// in order of their backfaces, so that the frontmost remaining
				// backface always is rendered first (and thus no already-rendered shadow pixels behind it
				// would be incorrectly turned off)

				COLORVERTEX *cvptr;
				FLOAT Z_array[NUM_SHADOWS];

				g_pVB_castertestverts->Lock( DDLOCK_NOSYSLOCK | DDLOCK_WRITEONLY | DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,
											 (VOID**)&cvptr,NULL );

				for( shadnum=0;shadnum<g_NumCasters;shadnum++,cvptr++ )
				{
					// taking first vtx as representative point
					memcpy( &cvptr->p, (D3DVECTOR*)(&g_Caster[shadnum].pRVerts[0].x),sizeof(D3DVECTOR));
				}

				g_pVB_castertestverts->Unlock();

				g_pVB_xformed->ProcessVertices( D3DVOP_TRANSFORM, 0, g_NumCasters,
					                            g_pVB_castertestverts, 0, pd3dDevice, 0 );

				g_pVB_xformed->Lock( DDLOCK_NOSYSLOCK | DDLOCK_READONLY | DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,
									 (VOID**)&cvptr,NULL);

				for( shadnum=0;shadnum<g_NumCasters;shadnum++ )
				{
					Z_array[shadnum]=cvptr->p.z;

					// output VB is XYZ+RHW+COLOR
					cvptr=(COLORVERTEX*)(((char*)cvptr)+(sizeof(D3DVECTOR)+sizeof(float)+sizeof(DWORD)));
				}

				g_pVB_xformed->Unlock();
	
				// insertion sort Z_array and shadorder array to get front-to-back shadorder

				int i,j;
				SHADOW *tmp_sptr;
				float tmp_z;

				for(i=1;i<((int)g_NumCasters);i++) 
				{
					tmp_z=Z_array[i];
					tmp_sptr=shadorder[i];

					j=i;
					while((--j >=0) && (tmp_z < Z_array[j]))
					{
						Z_array[j+1]=Z_array[j];
						shadorder[j+1]=shadorder[j];
					}

					Z_array[j+1]=tmp_z;
					shadorder[j+1]=tmp_sptr;
				}
			}

			if(g_bDrawShadowVolumes) 
			{
				pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );

				// Since destcolor=SRCBLEND * SRC_COLOR + DESTBLEND * DEST_COLOR,
				// this results in destcolor= (AlphaSrc) * SRC_COLOR + (1-AlphaSrc)*DestColor
				pd3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
				pd3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);

				pd3dDevice->SetRenderState( D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD );
			}

			for(shadnum=0;shadnum<g_NumCasters;shadnum++) 
			{
				g_pVB_xformed->ProcessVertices( (D3DVOP_TRANSFORM | D3DVOP_CLIP),
												0, shadorder[shadnum]->totalverts, 
												shadorder[shadnum]->VB, 0, pd3dDevice, 0 );
				if(g_bDrawShadowVolumes) 
				{
					pd3dDevice->DrawIndexedPrimitiveVB( D3DPT_TRIANGLESTRIP,
													    g_pVB_xformed,
														shadorder[shadnum]->pwShadVolSideIndices,
														shadorder[shadnum]->num_side_indices,
														0x0);
					if(g_bDrawShadowVolCaps) 
					{
						pd3dDevice->DrawIndexedPrimitiveVB( D3DPT_TRIANGLEFAN,
												g_pVB_xformed,
												shadorder[shadnum]->pwShadVolCapIndices,
												shadorder[shadnum]->num_cap_indices, 0x0);
					}
				}
				else 
				{
					// Render the shadow volume into the stencil buffer
					RenderShadow( pd3dDevice, shadorder[shadnum],g_pVB_xformed);
				}
			}

			if(g_bDrawShadowVolumes) 
			{
				pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
			} 
			else 
			{
				DrawShadow( pd3dDevice, prcViewportRect );
			}
		}

		// End the scene.
		pd3dDevice->EndScene();
	}

	return DD_OK;
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




//-----------------------------------------------------------------------------
// Name: CreateStencilBuffer()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CreateStencilBuffer( LPDIRECT3DDEVICE3 pd3dDevice,
                             LPDIRECTDRAWSURFACE4* ppddsDepthBuffer )
{
    GUID*           pDriverGUID;
    GUID*           pDeviceGUID;
    DDSURFACEDESC2* pMode;
    D3DEnum_GetSelectedDriver( &pDriverGUID, &pDeviceGUID, &pMode );

    g_bCanOnlyDoOneBitStencil=FALSE;

    // Verify stencil caps
    DWORD dwStencilCaps;
    D3DDEVICEDESC ddHwDesc,ddSwDesc;
    ZeroMemory( &ddHwDesc, sizeof(D3DDEVICEDESC) );
    ZeroMemory( &ddSwDesc, sizeof(D3DDEVICEDESC) );
    ddSwDesc.dwSize = ddHwDesc.dwSize = sizeof(D3DDEVICEDESC);

    pd3dDevice->GetCaps(&ddHwDesc,&ddSwDesc);

    if(ddHwDesc.dwFlags)
		dwStencilCaps = ddHwDesc.dwStencilCaps;
    else 
		dwStencilCaps = ddSwDesc.dwStencilCaps;

    if( (!(dwStencilCaps & D3DSTENCILCAPS_KEEP)) || (!(dwStencilCaps & D3DSTENCILCAPS_REPLACE))) {
        MessageBox( NULL, TEXT("D3DSTENCILCAPS_KEEP and D3DSTENCILCAPS_REPLACE are not supported, failing"), 
                               g_strAppTitle, 
                               MB_ICONWARNING|MB_OK );
        return E_FAIL;
    }

    if( (!(dwStencilCaps & D3DSTENCILCAPS_INCR) && !(dwStencilCaps & D3DSTENCILCAPS_INCRSAT))
       ||(!(dwStencilCaps & D3DSTENCILCAPS_DECR) && !(dwStencilCaps & D3DSTENCILCAPS_DECRSAT)))
	{
		// Must do 1-bit stencil buffer
        g_bCanOnlyDoOneBitStencil=TRUE;
    } 
	else 
	{
        // Prefer sat ops that cap at 0/max, but can use other ones as long as enough stencil bits
        g_StencIncOp=(dwStencilCaps & D3DSTENCILCAPS_INCRSAT)? D3DSTENCILOP_INCRSAT:D3DSTENCILOP_INCR;
        g_StencDecOp=(dwStencilCaps & D3DSTENCILCAPS_DECRSAT)? D3DSTENCILOP_DECRSAT:D3DSTENCILOP_DECR;
    }

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

    pdds->DeleteAttachedSurface( 0,NULL );

    // Get z-buffer dimensions from the render target
    // Setup the surface desc for the z-buffer.
    DDSURFACEDESC2 ddsd;
    D3DUtil_InitSurfaceDesc( ddsd );
    pdds->GetSurfaceDesc( &ddsd );

    DWORD dwMemType = D3DUtil_GetDeviceMemoryType( pd3dDevice );

    ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | dwMemType;
    ddsd.ddpfPixelFormat.dwFlags = DDPF_ZBUFFER | DDPF_STENCILBUFFER;
        
    // Get an appropiate pixel format from enumeration of the formats.
    pD3D->EnumZBufferFormats( *pDeviceGUID, EnumZBufferFormatsCallback,
                                      (VOID*)&ddsd.ddpfPixelFormat );

    assert(ddsd.ddpfPixelFormat.dwStencilBitDepth!=0);

    g_bCanOnlyDoOneBitStencil=g_bCanOnlyDoOneBitStencil || ((1<<ddsd.ddpfPixelFormat.dwStencilBitDepth)<NUM_SHADOWS);

    g_max_StencilVal=(1<<ddsd.ddpfPixelFormat.dwStencilBitDepth)-1;

	// Leave g_bUseOneBitStencil set for window-resize case
    if( !g_bUseOneBitStencil )  
        g_bUseOneBitStencil=g_bCanOnlyDoOneBitStencil;

    SetMenuStates( g_hMenu );

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
// Name: ResetViewMatrix()
// Desc: 
//-----------------------------------------------------------------------------
VOID ResetViewMatrix( LPDIRECT3DDEVICE3 pd3dDevice )
{
	
    D3DVECTOR vEyePt    = D3DVECTOR( 0.0f,  0.0f, -6.5f );
    D3DVECTOR vLookatPt = D3DVECTOR( 0.0f, -1.0f,  0.0f );
    D3DVECTOR vUpVec    = D3DVECTOR( 0.0f,  1.0f,  0.0f );
    D3DMATRIX matView;

    if( g_bShowQuarterView )
		vEyePt = D3DVECTOR( 0.0f, 3.0f, -6.5f );

    D3DUtil_SetViewMatrix( matView, vEyePt, vLookatPt, vUpVec );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &matView );
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
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,  D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );

    // Set the transform matrices
    ResetViewMatrix(pd3dDevice);

    D3DMATRIX matWorld, matProj;

    D3DUtil_SetIdentityMatrix( matWorld );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
    D3DUtil_SetProjectionMatrix( matProj, 2.0f, 1.0f, 0.2f, 50.0f );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

    // Set up the light
    if( FAILED( hr = pD3D->CreateLight( &g_pLight, NULL ) ) )
            return E_FAIL;

    D3DLIGHT light;
    D3DUtil_InitLight( light, D3DLIGHT_POINT, 0.0f, 0.0f, -12.0f );
    light.dvAttenuation0 = 1.0f;
    g_pLight->SetLight( &light );
    pvViewport->AddLight( g_pLight );

    return Init3DGeometry(pd3dDevice);
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
    return DD_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteShadowObjects()
// Desc: 
//-----------------------------------------------------------------------------
VOID DeleteShadowObjects()
{
    for( DWORD shadnum=0; shadnum<NUM_SHADOWS; shadnum++ )
	{
        SAFE_RELEASE( g_Shad[shadnum].VB );
        SAFE_DELETE( g_Shad[shadnum].pwShadVolIndices );
    }

    for( DWORD i=0; i<NUM_SHADOWS; i++ )
	{
        SAFE_DELETE( g_Caster[i].pVerts );
        SAFE_DELETE( g_Caster[i].pRVerts );
        SAFE_DELETE( g_Caster[i].pIndices );
    }

    ZeroMemory( g_Caster, NUM_SHADOWS*sizeof(SHADOWCASTER) );
    ZeroMemory( g_Shad,   NUM_SHADOWS*sizeof(SHADOW) );

    // Must release all VertexBufs because they are associated with D3D3,
	// which is being released
    SAFE_RELEASE( g_pVB_xformed );
    SAFE_RELEASE( g_pVB_castertestverts );
}




//-----------------------------------------------------------------------------
// Name: App_DeleteDeviceObjects()
// Desc: Called when the app is exitting, or the device is being changed,
//       this function deletes any device dependant objects.
//-----------------------------------------------------------------------------
VOID App_DeleteDeviceObjects( LPDIRECT3DDEVICE3 pd3dDevice, 
                              LPDIRECT3DVIEWPORT3 pvViewport )
{
    DeleteShadowObjects();
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
	return D3D_OK;
}




//-----------------------------------------------------------------------------
// Name: App_ConfirmDevice()
// Desc: Called during device intialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT App_ConfirmDevice( DDCAPS* pddDriverCaps, 
						   D3DDEVICEDESC* pd3dDeviceDesc )
{
    DWORD dwStencilCaps = pd3dDeviceDesc->dwStencilCaps;

    if( !(dwStencilCaps & D3DSTENCILCAPS_KEEP) ) 
        return E_FAIL;
	if( !(dwStencilCaps & D3DSTENCILCAPS_REPLACE) )
        return E_FAIL;

    return S_OK;
}



