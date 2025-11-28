//-----------------------------------------------------------------------------
// File: XFile.cpp
//
// Desc: Example code showing how to load .X files in D3DIM.
//
//       Note: This code uses the D3D Framework helper library.
//
//
// Copyright (c) 1997-1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define STRICT
#define D3D_OVERLOADS
#include <windows.h>
#include <commdlg.h>
#include <math.h>
#include <stdio.h>
#include "D3DTextr.h"
#include "D3DUtil.h"
#include "D3DMath.h"
#include "resource.h"
#include "XFileLoader.h"


//-----------------------------------------------------------------------------
// Declare the application globals for use in WinMain.cpp
//-----------------------------------------------------------------------------
TCHAR* g_strAppTitle       = TEXT( "XFile: Loading .X files in D3DIM" );
BOOL   g_bAppUseZBuffer    = TRUE;
BOOL   g_bAppUseBackBuffer = TRUE;




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
LPDIRECT3DMATERIAL3 g_pmtrlObjectMtrl     = NULL;
LPDIRECT3DLIGHT     g_pLight              = NULL;
CD3DFileObject*     g_pFileObject         = NULL;
BOOL                g_bNewFileObject      = FALSE;




//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
VOID    AppPause( BOOL );
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
CHAR*   PromptUserForFileToLoad( HWND );
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK App_OverridenWndProc( HWND, UINT, WPARAM, LPARAM );




//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{
    // Attach a message handler
    SetWindowLong( hWnd, GWL_WNDPROC, (LONG)App_OverridenWndProc );

    // Create a container for the object to hold the xfile data
    g_pFileObject = new CD3DFileObject();

    // Load a DirectX .X file
    while( FALSE == g_bNewFileObject ) 
    {
        CHAR* strFileName = PromptUserForFileToLoad( hWnd );

        if( NULL == strFileName )
        {
            SendMessage( hWnd, WM_CLOSE, 0, 0 );
            return 0;
        }

        g_pFileObject->Load( strFileName );

        // If the file was loaded, exit the loop. Else, display an error.
        if( g_pFileObject )
            g_bNewFileObject = TRUE;
        else
            MessageBox( NULL, TEXT("Error loading specified X file"),
                        TEXT("XFile"), MB_OK|MB_ICONERROR );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT App_FrameMove( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{
    // Setup the world spin matrix
    D3DMATRIX matRotateY;
    D3DUtil_SetRotateYMatrix( matRotateY, fTimeKey/2  );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matRotateY );

    // If a new file object loaded since the last frame, adjust the view to
    // point at it. Also, restore its textures before rendering.
    if( g_pFileObject && g_bNewFileObject )
    {
        g_bNewFileObject = FALSE;

        D3DVECTOR vPos;
        FLOAT     fRadius;
        g_pFileObject->GetBoundingSphere( &vPos, &fRadius );

        D3DVECTOR vEyePt    = D3DVECTOR( 0.0f, 0.0f, fRadius*3.0f ) + vPos;
        D3DVECTOR vLookatPt = vPos;
        D3DVECTOR vUpVec    = D3DVECTOR( 0.0f, 1.0f, 0.0f );
        D3DMATRIX matView;

        D3DUtil_SetViewMatrix( matView, vEyePt, vLookatPt, vUpVec );
        pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &matView );

        D3DTextr_RestoreAllTextures( pd3dDevice );
    }

    // Just for kicks, demonstrate the feature to find a mesh or a frame
    // and animate it. Here, if the user loads the "triplane.x" file, which
    // contains a frame named "prop", this code will find the matrix for that
    // frame and animate. The end result is that the airplane has a rotating
    // propeller.
    if( g_pFileObject )
    {
        D3DMATRIX* pmat;
        if( SUCCEEDED( g_pFileObject->GetFrameMatrix( "prop", &pmat ) ) )
        {
            D3DUtil_SetRotateZMatrix( *pmat, 5*fTimeKey );
        }
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
                    LPDIRECT3DVIEWPORT3 pvViewport, D3DRECT* prcViewRect )
{
    //Clear the viewport
    pvViewport->Clear2( 1UL, prcViewRect, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                        0x000000ff, 1.0f, 0L );

    // Begin the scene 
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        if( g_pFileObject )
            g_pFileObject->Render( pd3dDevice, g_pmtrlObjectMtrl );

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

    // Get a ptr to the ID3D object to create materials and/or lights. Note:
    // the Release() call just serves to decrease the ref count.
    LPDIRECT3D3 pD3D;
    if( FAILED( pd3dDevice->GetDirect3D( &pD3D ) ) )
        return E_FAIL;
    pD3D->Release();

    HRESULT           hr;
    D3DMATERIAL       mtrl;
    D3DMATERIALHANDLE hmtrl;
    
    // Create and set up the object material
    if( FAILED( hr = pD3D->CreateMaterial( &g_pmtrlObjectMtrl, NULL ) ) )
        return E_FAIL;

    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    g_pmtrlObjectMtrl->SetMaterial( &mtrl );
    g_pmtrlObjectMtrl->GetHandle( pd3dDevice, &hmtrl );
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_MATERIAL, hmtrl );
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_AMBIENT, 0x0 );

    D3DTextr_RestoreAllTextures( pd3dDevice );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE,   TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE,        TRUE );

    // Set the transform matrices
    D3DVECTOR vEyePt    = D3DVECTOR( 0.0f, 0.0f, -8.5f );
    D3DVECTOR vLookatPt = D3DVECTOR( 0.0f, 0.0f,  0.0f );
    D3DVECTOR vUpVec    = D3DVECTOR( 0.0f, 1.0f,  0.0f );
    D3DMATRIX matWorld, matView, matProj;

    // If we have an object loaded, position the view accordingly
    if( g_pFileObject )
    {
        FLOAT fRadius;
        g_pFileObject->GetBoundingSphere( &vLookatPt, &fRadius );
        vEyePt = D3DVECTOR( 0.0f, 0.0f, fRadius*3.0f ) + vLookatPt;

        g_bNewFileObject = FALSE;
    }

    D3DUtil_SetIdentityMatrix( matWorld );
    D3DUtil_SetViewMatrix( matView, vEyePt, vLookatPt, vUpVec );
    D3DUtil_SetProjectionMatrix( matProj, g_PI/4, 1.0f, 1.0f, 1000.0f );

    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW,       &matView );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );


    // Set up the light
    if( FAILED( hr = pD3D->CreateLight( &g_pLight, NULL ) ) )
        return E_FAIL;
    
    D3DLIGHT light;
    D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, 0.0f, -1.0f, -1.0f );
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
                          LPDIRECT3DVIEWPORT3 pvViewport )
{
    App_DeleteDeviceObjects( pd3dDevice, pvViewport );
    
    SAFE_DELETE( g_pFileObject );

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




//----------------------------------------------------------------------------
// Name: App_OverridenWndProc
// Desc: App custom WndProc function for handling mouse and keyboard input.
//----------------------------------------------------------------------------
LRESULT CALLBACK App_OverridenWndProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                       LPARAM lParam )
{
    // Check for the "Load File..." option selected from the menu
    if( WM_COMMAND == uMsg )
    {
        if( IDM_LOADFILE == LOWORD(wParam) )
        {
            AppPause( TRUE );
            
            CHAR* strFileName = PromptUserForFileToLoad( hWnd );
        
            if( strFileName )
            {
                CD3DFileObject* pFileObject = new CD3DFileObject();
                
                // If the file was successfully loaded, delete the old one
                // and use this was instead. Otherwise, display an error.
                if( SUCCEEDED( pFileObject->Load( strFileName ) ) )
                {
                    SAFE_DELETE( g_pFileObject );
                    g_pFileObject = pFileObject;
                    g_bNewFileObject = TRUE;
                }
                else
                {
                    MessageBox( NULL, TEXT("Error loading specified X file"),
                                TEXT("XFile"), MB_OK|MB_ICONERROR );
                }
            }

            AppPause( FALSE );
        }
    }

    // Fall through to the app's main windows proc function
    return WndProc( hWnd, uMsg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: PromptUserForFileToLoad()
// Desc: Uses Windows' OpenFileName dialog to get the name of an X file to
//       load, then proceeds to load that file.
//-----------------------------------------------------------------------------
CHAR* PromptUserForFileToLoad( HWND hWnd )
{
    static TCHAR strInitialDir[512] = "";
    static TCHAR strPath[512];
    static TCHAR strFileName[512];
    TCHAR        strCurrentName[512] = "*.x";
    HKEY         key;
    DWORD        type, size = 512;
    
    OPENFILENAME ofn = { sizeof(OPENFILENAME), hWnd, NULL, "X Files\0.X", NULL,
                         0, 1, strCurrentName, 512, strFileName, 512, NULL,
                         "Open X File", OFN_FILEMUSTEXIST, 0, 1, ".X", 0,
                         NULL, NULL };

    if( '\0' == strInitialDir[0] )
    {
        // Get the media path from the registry. For X files, we will actually
        // look in the D3DRM\media directory
        if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, 
                                        TEXT("Software\\Microsoft\\DirectX"),
                                        0, KEY_READ, &key ) )
        {
            if( ERROR_SUCCESS == RegQueryValueEx( key,
                                        TEXT("DX6SDK Samples Path"), NULL,
                                        &type, (BYTE*)strPath, &size ) )
            {
                sprintf( strInitialDir, TEXT("%s\\D3DIM\\Media"), strPath );
                ofn.lpstrInitialDir = strInitialDir;
            }

            RegCloseKey( key );
        }
    }

    // Display the OpenFileName dialog. Then, try to load the specified file
    if( TRUE == GetOpenFileName( &ofn ) )
        return strFileName;

    return NULL;
}





