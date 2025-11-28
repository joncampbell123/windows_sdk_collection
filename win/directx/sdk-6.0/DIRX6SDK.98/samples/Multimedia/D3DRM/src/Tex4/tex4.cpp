//-----------------------------------------------------------------------------
// File: tex4.cpp
//
// Desc: A texture mapping example using a spherical mapping relative to the
//       camera. The texture is also used as a background for the scene.
//
// Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
//-----------------------------------------------------------------------------
#include <d3drmwin.h>

#define SAFE_RELEASE(x) if (x != NULL) {x->Release(); x = NULL;}
#define MSG(str) MessageBox( NULL, str, "Application Message", MB_OK )




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void __cdecl
ApplyWrapCallback(LPDIRECT3DRMFRAME3 frame, void* arg, D3DVALUE delta)
{
    LPDIRECT3DRMWRAP wrap = (LPDIRECT3DRMWRAP) arg;
    LPDIRECT3DRMFRAME frame_1;
    DWORD dwCount, i;
    LPDIRECT3DRMMESH mesh;

    // QI for the LPDIRECT3DRMFRAME interface
    if (FAILED (frame->QueryInterface( IID_IDirect3DRMFrame,
                                        (VOID**)&frame_1) ) ) {
        MSG("Failed to QI for D3DFrame in Callback.\n" );
        return;
    }

    // Get the number of Visuals
    frame->GetVisuals( &dwCount, NULL );

    // Allocate space for the Visuals
    LPUNKNOWN *ppVisuals = new LPUNKNOWN[dwCount];
    frame->GetVisuals( &dwCount, ppVisuals);

    for (i = 0; i < dwCount; i++)
    {
        if (SUCCEEDED((ppVisuals[i])->QueryInterface(
                                         IID_IDirect3DRMMesh, (VOID**)&mesh)))
        {
            wrap->ApplyRelative( frame_1, mesh);
            mesh->Release();
        }
    }

    delete [] ppVisuals;
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void __cdecl
CleanupWrapCallback(LPDIRECT3DRMOBJECT, void *arg)
{
    LPDIRECT3DRMWRAP wrap = (LPDIRECT3DRMWRAP) arg;

    wrap->Release();
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL BuildScene( LPDIRECT3DRM3 pD3DRM, 
				 LPDIRECT3DRMDEVICE3, LPDIRECT3DRMVIEWPORT2,
				 LPDIRECT3DRMFRAME3 scene, LPDIRECT3DRMFRAME3 camera)
{
    LPDIRECT3DRMMESH mesh = NULL;
    LPDIRECT3DRMFRAME3 frame = NULL;
    LPDIRECT3DRMFRAME3 axis = NULL;
    LPDIRECT3DRMFRAME3 child = NULL;
    LPDIRECT3DRMTEXTURE3 tex = NULL;
    LPDIRECT3DRMWRAP wrap = NULL;
    LPDIRECT3DRMLIGHT light1 = NULL;
    LPDIRECT3DRMLIGHT light2 = NULL;
    LPDIRECT3DRMMESHBUILDER3 builder = NULL;

    if (FAILED( pD3DRM->CreateLightRGB( D3DRMLIGHT_AMBIENT, D3DVAL(0.2),
                                  D3DVAL(0.2), D3DVAL(0.2), &light1)))
		goto generic_error;
    if (FAILED( pD3DRM->CreateLightRGB( D3DRMLIGHT_DIRECTIONAL, D3DVAL(1),
                                  D3DVAL(1), D3DVAL(1), &light2)))
		goto generic_error;
    if (FAILED( scene->AddLight( light1)))
		goto generic_error;
    if (FAILED( pD3DRM->CreateFrame( scene, &frame)))
		goto generic_error;
    if (FAILED( frame->SetOrientation( scene, -D3DVAL(1), -D3DVAL(1),
                                  D3DVAL(1), D3DVAL(0), D3DVAL(1), D3DVAL(0))))
		goto generic_error;
    if (FAILED( frame->AddLight( light2)))
		goto generic_error;
    SAFE_RELEASE(frame);

    if (FAILED( pD3DRM->CreateFrame( scene, &frame)))
		goto generic_error;
    if (FAILED( frame->SetPosition( scene, D3DVAL(0), D3DVAL(0), D3DVAL(15))))
		goto generic_error;
    if (FAILED( frame->SetRotation( scene, D3DVAL(1.1), D3DVAL(0.3),
                               D3DVAL(0.0), D3DVAL(0.04))))
		goto generic_error;
    if (FAILED( pD3DRM->CreateMeshBuilder( &builder)))
		goto generic_error;
    if( FAILED( builder->Load( "sphere3.x", NULL, D3DRMLOAD_FROMFILE, NULL, NULL) ) )
    {
        MSG("Failed to load sphere3.x.\n");
		goto ret_with_error;
    }
    if (FAILED( builder->Scale( D3DVAL(0.2), D3DVAL(0.2), D3DVAL(0.2))))
		goto generic_error;
    if (FAILED( builder->CreateMesh( &mesh)))
		goto generic_error;
    SAFE_RELEASE(builder);
    if (FAILED( frame->AddVisual( (LPDIRECT3DRMVISUAL) mesh)))
		goto generic_error;
    SAFE_RELEASE(mesh);

    if (FAILED( pD3DRM->CreateWrap( D3DRMWRAP_SPHERE, camera,
			      D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
			      D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0),
			      D3DVAL(0.0), D3DVAL(0.0), -D3DVAL(1.0),
			      D3DVAL(0.0), D3DVAL(0.0),
			      D3DVAL(5.0), D3DVAL(5.0),
			      &wrap)))
		goto generic_error;

    if (FAILED( pD3DRM->CreateFrame( frame, &axis)))
		goto generic_error;
    if (FAILED( axis->SetRotation( frame, D3DVAL(0), D3DVAL(1), D3DVAL(0), D3DVAL(0.04))))
		goto generic_error;
    if (FAILED( pD3DRM->CreateFrame( axis, &child)))
		goto generic_error;
    if (FAILED( child->SetPosition( axis, D3DVAL(3), D3DVAL(0), D3DVAL(0))))
		goto generic_error;
    if (FAILED( child->SetRotation( axis, D3DVAL(1), D3DVAL(1), D3DVAL(0),
                               D3DVAL(0.04))))
		goto generic_error;

    if( FAILED( pD3DRM->LoadTexture( "lake.ppm", &tex) ) )
	{
		MSG("Failed to load lake.ppm.\n");
		goto ret_with_error;
    }
    if (FAILED( tex->SetColors( 32)))
		goto generic_error;
    if (FAILED( tex->SetDecalScale( TRUE)))
		goto generic_error;
    if (FAILED( tex->SetDecalOrigin( 128, 128)))
		goto generic_error;
    if (FAILED( pD3DRM->CreateMeshBuilder( &builder)))
		goto generic_error;
    if( FAILED( builder->Load( "tpot2.x", NULL, D3DRMLOAD_FROMFILE, NULL, NULL) ) )
	{
        MSG("Failed to load tpot2.x.\n");
		goto ret_with_error;
    }

    if (FAILED( builder->Scale( D3DVAL(2), D3DVAL(2), D3DVAL(2))))
	    goto generic_error;
    if (FAILED( builder->SetTexture( tex)))
	    goto generic_error;
    if (FAILED( builder->SetQuality( D3DRMRENDER_GOURAUD)))
	    goto generic_error;
    if (FAILED( builder->CreateMesh( &mesh)))
	    goto generic_error;
    SAFE_RELEASE(builder);

    if (FAILED( child->AddVisual( mesh)))
	    goto generic_error;
    if (FAILED( child->AddMoveCallback( ApplyWrapCallback, (void *) wrap,
                                        D3DRMCALLBACK_PREORDER )))
	    goto generic_error;
    if (FAILED( child->AddDestroyCallback( CleanupWrapCallback, wrap)))
	    goto generic_error;
    if (FAILED( scene->SetSceneBackgroundImage( tex)))
	    goto generic_error;

    SAFE_RELEASE(mesh);
    SAFE_RELEASE(frame);
    SAFE_RELEASE(axis);
    SAFE_RELEASE(child);
    SAFE_RELEASE(tex);
    SAFE_RELEASE(light1);
    SAFE_RELEASE(light2);

    /* don't release the wrap */
    return TRUE;
generic_error:
    MSG("A failure occurred while building the scene.\n");
ret_with_error:
    SAFE_RELEASE(mesh);
    SAFE_RELEASE(frame);
    SAFE_RELEASE(axis);
    SAFE_RELEASE(child);
    SAFE_RELEASE(tex);
    SAFE_RELEASE(wrap);
    SAFE_RELEASE(light1);
    SAFE_RELEASE(light2);
    SAFE_RELEASE(builder);
    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
VOID OverrideDefaults( BOOL* pbNoTextures, BOOL* pbResizingDisabled, 
					   BOOL* pbConstRenderQuality, CHAR** pstrName )
{
    (*pbConstRenderQuality) = TRUE;
    (*pstrName) = "Texture Mapping IV Direct3DRM Example";
}

