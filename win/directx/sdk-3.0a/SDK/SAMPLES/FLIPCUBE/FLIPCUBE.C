/*==========================================================================
 *
 *  Copyright (C) 1995, 1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File: flipcube.c
 *
 *  Mouse controls: Left-click stops cube rotating.  Holding down left mouse
 *     button and moving mouse moves cube.  Right-click resumes rotating.
 *
 ***************************************************************************/

#include <windows.h>
#include <windowsx.h>
#include <math.h>
#include <malloc.h>
#include "d3ddemo.h"

BOOL KeyboardHandler(UINT message, WPARAM wParam, LPARAM lParam);
BOOL MouseHandler(UINT message, WPARAM wParam, LPARAM lParam);
void ConcatenateXRotation(LPD3DMATRIX lpM, float Degrees );
void ConcatenateYRotation(LPD3DMATRIX lpM, float Degrees );
void ConcatenateZRotation(LPD3DMATRIX lpM, float Degrees );

//*** Cube colors - one RGB color per material
const D3DCOLOR MaterialColors[6][2] =
{
  RGBA_MAKE(240,  20,  20, 255),    // Unsaturated Red
  RGB_MAKE (240,  20,  20),         // Unsaturated Red
  RGBA_MAKE( 20, 240,  20, 255),    // Unsaturated Green
  RGB_MAKE ( 20, 240,  20),         // Unsaturated Green
  RGBA_MAKE( 20,  20, 240, 255),    // Unsaturated Blue  
  RGB_MAKE ( 20,  20, 240),         // Unsaturated Blue
  RGBA_MAKE(128,  64,   0, 255),    // Brown
  RGB_MAKE (128,  64,   0),         // Brown
  RGBA_MAKE(240,  20, 240, 255),    // Unsaturated Magenta
  RGB_MAKE (240,  20, 240),         // Unsaturated Magenta
  RGBA_MAKE(240, 240,  20, 255),    // Unsaturated Yellow
  RGB_MAKE (240, 240,  20),         // Unsaturated Yellow
};

//*** Lighting
const D3DCOLOR AmbientColor = RGBA_MAKE(20, 20, 20, 20);
LPDIRECT3DLIGHT lpD3DLight;

//*** Viewing and perspective
D3DMATRIXHANDLE hProj, hView, hWorld;
D3DMATRIX proj = {
    D3DVAL(2.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(2.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(1.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(0.0)
};
D3DMATRIX view = {
    D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(10.0), D3DVAL(1.0)
};
D3DMATRIX identity = {
    D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0)
};
D3DMATRIX world, spin;

//*** Execute buffer
static D3DEXECUTEDATA d3dExData;
static LPDIRECT3DEXECUTEBUFFER lpD3DExBuf;
static D3DEXECUTEBUFFERDESC debDesc;

//*** Interaction
static POINT       Move;
static POINT       Last;

/*
 * A structure which holds the object's data
 */

LPDIRECT3DMATERIAL lpBackgroundMaterial;
LPDIRECT3DMATERIAL lpD3DMaterial[6];
D3DMATERIALHANDLE  D3DMaterialHandle[6];
/* Cube vertices, normals, shades, and modeling transform */
int NumVertices = 24;
static D3DVERTEX CubeVertices[] = {
  {D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(0.0) },
  {D3DVAL(1.0),D3DVAL(1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(1.0) },
  {D3DVAL(1.0),D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0) },
  {D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(0.0) },

  {D3DVAL(1.0),D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(0.0) },
  {D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(1.0) },
  {D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(1.0) },
  {D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(0.0) },

  {D3DVAL(1.0),D3DVAL(1.0),D3DVAL(1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(0.0) },
  {D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(1.0) },
  {D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0) },
  {D3DVAL(1.0),D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(0.0) },

  {D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(1.0),D3DVAL(0.0) },
  {D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(1.0),D3DVAL(1.0) },
  {D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(1.0) },
  {D3DVAL(1.0),D3DVAL(1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(0.0) },

  {D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0) },
  {D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(1.0) },
  {D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(0.0) },
  {D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(0.0) },
  
  {D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(1.0) },
  {D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0) },
  {D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(0.0) },
  {D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(0.0) }
};

//*** Cube edges - ordered indices into the vertex array
const int NumTri = 12;
const int CubeTri[] = {
   0,  1,  2,  0, 2, 3,
   4,  5,  6,  4, 6, 7,
   8,  9, 10, 8, 10, 11,
  12, 13, 14, 12, 14, 15,
  16, 17, 18, 16, 18, 19,
  20, 21, 22, 20, 22, 23
};

void
OverrideDefaults(Defaults* defaults)
{
    lstrcpy(defaults->Name, "Flipcube D3D Example");
    defaults->rs.ShadeMode = D3DSHADE_FLAT;
    defaults->rs.bZBufferOn = FALSE;
    defaults->bTexturesDisabled = TRUE;
}

BOOL
TickScene()
{
    if (GetAsyncKeyState(VK_LBUTTON) < 0) {
        if(Move.x || Move.y) {
            D3DMATRIX Movement;
            Movement = identity;
            ConcatenateYRotation(&Movement, (float)Move.x);
            ConcatenateXRotation(&Movement, (float)Move.y);
            Move.x = Move.y = 0;
            MultiplyD3DMATRIX(&world, &world, &Movement);
        }
    } else {
        MultiplyD3DMATRIX(&world, &spin, &world);
    }
    return TRUE;
}

/*
 * Each frame, renders the scene and calls mod_buffer to modify the object
 * for the next frame.
 */
BOOL
RenderScene(LPDIRECT3DDEVICE lpDev, LPDIRECT3DVIEWPORT lpView,
            LPD3DRECT lpExtent)
{
    /*
     * Execute the instruction buffer
     */
    if (lpDev->lpVtbl->SetMatrix(lpDev, hWorld, &world) != D3D_OK)
        return FALSE;
    if (lpDev->lpVtbl->BeginScene(lpDev) != D3D_OK)
        return FALSE;
    if (lpDev->lpVtbl->Execute(lpDev, lpD3DExBuf,
                               lpView, D3DEXECUTE_CLIPPED) != D3D_OK)
        return FALSE;
    if (lpDev->lpVtbl->EndScene(lpDev) != D3D_OK)
        return FALSE;
    if (lpD3DExBuf->lpVtbl->GetExecuteData(lpD3DExBuf, &d3dExData)!= D3D_OK)
        return FALSE;
    *lpExtent = d3dExData.dsStatus.drExtent;
    if (!(TickScene()))
        return FALSE;
    return TRUE;
}

void
InitSpin(void)
{
    spin = identity;
    ConcatenateYRotation(&spin, D3DVAL(6.0));
    ConcatenateXRotation(&spin, D3DVAL(3.5));
    ConcatenateZRotation(&spin, D3DVAL(2.0));
}

BOOL
InitScene(void)
{
    world = identity;
    InitSpin();
    if (!(SetKeyboardCallback(KeyboardHandler)))
        return FALSE;
    if (!(SetMouseCallback(MouseHandler)))
        return FALSE;
    return TRUE;
}

void
ReleaseScene(void)
{
   
}

/*
 * Release the memory allocated for the scene and all D3D objects created.
 */
void
ReleaseView(LPDIRECT3DVIEWPORT lpView)
{
    int i;
    if (lpView)
        lpView->lpVtbl->DeleteLight(lpView, lpD3DLight);
    RELEASE(lpD3DLight);
    RELEASE(lpD3DExBuf);

    RELEASE(lpBackgroundMaterial);
    for (i = 0; i < 6; i++) {
        RELEASE(lpD3DMaterial[i]);
    }
}

/*
 * Builds the scene and initializes the execute buffer for rendering.  Returns 0 on failure.
 */
BOOL
InitView(LPDIRECTDRAW lpDD, LPDIRECT3D lpD3D, LPDIRECT3DDEVICE lpDev,
           LPDIRECT3DVIEWPORT lpView, int NumTextures,
           LPD3DTEXTUREHANDLE TextureHandle)
{
    D3DMATERIAL MaterialDesc;
    D3DMATERIALHANDLE BackgroundHandle;
    D3DLIGHT LightDesc;
    LPDIRECT3DEXECUTEBUFFER lpD3DExCmdBuf;
    LPVOID lpBufStart, lpInsStart, lpPointer;
    DWORD size;
    int i;

    for (i = 0; i < 6; i++) {
        if (lpD3D->lpVtbl->CreateMaterial(lpD3D, &lpD3DMaterial[i], NULL) != D3D_OK)
           return FALSE;
        memset(&MaterialDesc, 0, sizeof(D3DMATERIAL));
        MaterialDesc.dwSize = sizeof(D3DMATERIAL);
        MaterialDesc.diffuse.r = (D3DVALUE)(RGBA_GETRED(MaterialColors[i][0]) / 255.0);
        MaterialDesc.diffuse.g = (D3DVALUE)(RGBA_GETGREEN(MaterialColors[i][0]) / 255.0);
        MaterialDesc.diffuse.b = (D3DVALUE)(RGBA_GETBLUE(MaterialColors[i][0]) / 255.0);
        MaterialDesc.diffuse.a = (D3DVALUE)(RGBA_GETALPHA(MaterialColors[i][0]) / 255.0);
        MaterialDesc.ambient.r = (D3DVALUE)(RGBA_GETALPHA(MaterialColors[i][1]) / 255.0);
        MaterialDesc.ambient.g = (D3DVALUE)(RGBA_GETALPHA(MaterialColors[i][1]) / 255.0);
        MaterialDesc.ambient.b = (D3DVALUE)(RGBA_GETALPHA(MaterialColors[i][1]) / 255.0);
        MaterialDesc.ambient.a = (D3DVALUE)(RGBA_GETALPHA(MaterialColors[i][1]) / 255.0);
        MaterialDesc.specular.r = (D3DVALUE)1.0;
        MaterialDesc.specular.g = (D3DVALUE)1.0;
        MaterialDesc.specular.b = (D3DVALUE)1.0;
        MaterialDesc.power = (float)20.0;
        MaterialDesc.dwRampSize = 16;
        MaterialDesc.hTexture = TextureHandle[1];
        lpD3DMaterial[i]->lpVtbl->SetMaterial(lpD3DMaterial[i], &MaterialDesc);
        lpD3DMaterial[i]->lpVtbl->GetHandle(lpD3DMaterial[i], lpDev,
                                            &D3DMaterialHandle[i]);
    }
    /*
     * Set background to black material
     */
    if (lpD3D->lpVtbl->CreateMaterial(lpD3D, &lpBackgroundMaterial, NULL) != D3D_OK)
        return FALSE;
    memset(&MaterialDesc, 0, sizeof(D3DMATERIAL));
    MaterialDesc.dwSize = sizeof(D3DMATERIAL);
    MaterialDesc.dwRampSize = 1;
    lpBackgroundMaterial->lpVtbl->SetMaterial(lpBackgroundMaterial, &MaterialDesc);
    lpBackgroundMaterial->lpVtbl->GetHandle(lpBackgroundMaterial, lpDev,
                                            &BackgroundHandle);
    lpView->lpVtbl->SetBackground(lpView, BackgroundHandle);
    /*
     * Add one directional light.
     */
    memset(&LightDesc, 0, sizeof(D3DLIGHT));
    LightDesc.dwSize = sizeof(D3DLIGHT);
    LightDesc.dltType = D3DLIGHT_POINT;
    LightDesc.dcvColor.r = D3DVAL(0.9);
    LightDesc.dcvColor.g = D3DVAL(0.9);
    LightDesc.dcvColor.b = D3DVAL(0.9);
    LightDesc.dcvColor.a = D3DVAL(1.0);
    LightDesc.dvPosition.x = D3DVALP(0.0, 12);
    LightDesc.dvPosition.y = D3DVALP(0.0, 12);
    LightDesc.dvPosition.z = D3DVALP(-12.0, 12);
    LightDesc.dvAttenuation0 = D3DVAL(1.0);
    LightDesc.dvAttenuation1 = D3DVAL(0.0);
    LightDesc.dvAttenuation2 = D3DVAL(0.0);

//    LightDesc.type = D3DLIGHT_DIRECTIONAL;
    LightDesc.dvDirection.x = D3DVALP(0.0, 12);
    LightDesc.dvDirection.y = D3DVALP(0.0, 12);
    LightDesc.dvDirection.z = D3DVALP(1.0, 12);

    if (lpD3D->lpVtbl->CreateLight(lpD3D, &lpD3DLight, NULL) != D3D_OK)
        return FALSE;
    if (lpD3DLight->lpVtbl->SetLight(lpD3DLight, &LightDesc) != D3D_OK)
        return FALSE;
    if (lpView->lpVtbl->AddLight(lpView, lpD3DLight) != D3D_OK)
        return FALSE;

    /*
     * Set the view, world and projection matrices
     * Create a buffer for matrix set commands etc.
     */
    MAKE_MATRIX(lpDev, hView, view);
    MAKE_MATRIX(lpDev, hProj, proj);
    MAKE_MATRIX(lpDev, hWorld, world);
    size = 0;
    size += sizeof(D3DINSTRUCTION) * 3;
    size += sizeof(D3DSTATE) * 4;
    memset(&debDesc, 0, sizeof(D3DEXECUTEBUFFERDESC));
    debDesc.dwSize = sizeof(D3DEXECUTEBUFFERDESC);
    debDesc.dwFlags = D3DDEB_BUFSIZE;
    debDesc.dwBufferSize = size;
    if (lpDev->lpVtbl->CreateExecuteBuffer(lpDev, &debDesc, &lpD3DExCmdBuf,
                                         NULL) != D3D_OK)
        return FALSE;

    /*
     * lock it so it can be filled
     */
    if (lpD3DExCmdBuf->lpVtbl->Lock(lpD3DExCmdBuf, &debDesc) != D3D_OK)
        return FALSE;
    lpBufStart = debDesc.lpData;
    memset(lpBufStart, 0, size);
    lpPointer = lpBufStart;

    lpInsStart = lpPointer;
    OP_STATE_TRANSFORM(3, lpPointer);
        STATE_DATA(D3DTRANSFORMSTATE_PROJECTION, hProj, lpPointer);
        STATE_DATA(D3DTRANSFORMSTATE_VIEW, hView, lpPointer);
        STATE_DATA(D3DTRANSFORMSTATE_WORLD, hWorld, lpPointer);
    OP_STATE_LIGHT(1, lpPointer);
        STATE_DATA(D3DLIGHTSTATE_AMBIENT, AmbientColor, lpPointer);
    OP_EXIT(lpPointer);
    
    /*
     * Setup the execute data describing the buffer
     */
    lpD3DExCmdBuf->lpVtbl->Unlock(lpD3DExCmdBuf);
    memset(&d3dExData, 0, sizeof(D3DEXECUTEDATA));
    d3dExData.dwSize = sizeof(D3DEXECUTEDATA);
    d3dExData.dwInstructionOffset = (ULONG) 0;
    d3dExData.dwInstructionLength = (ULONG) ((char *)lpPointer - (char*)lpInsStart);
    lpD3DExCmdBuf->lpVtbl->SetExecuteData(lpD3DExCmdBuf, &d3dExData);
    lpDev->lpVtbl->BeginScene(lpDev);
    lpDev->lpVtbl->Execute(lpDev, lpD3DExCmdBuf, lpView, D3DEXECUTE_UNCLIPPED);
    lpDev->lpVtbl->EndScene(lpDev);

    /*
     * We are done with the command buffer.
     */
    lpD3DExCmdBuf->lpVtbl->Release(lpD3DExCmdBuf);
    /*
     * Create an execute buffer
     */
    // calculate the size of the buffer
    size = sizeof(D3DVERTEX) * NumVertices;
    size += sizeof(D3DSTATUS) * 1;
    size += sizeof(D3DPROCESSVERTICES) * 6;
    size += sizeof(D3DINSTRUCTION) * 17;
    size += sizeof(D3DSTATE) * 9;
    size += sizeof(D3DTRIANGLE) * NumTri;
    // Create an execute buffer
    memset(&debDesc, 0, sizeof(D3DEXECUTEBUFFERDESC));
    debDesc.dwSize = sizeof(D3DEXECUTEBUFFERDESC);
    debDesc.dwFlags = D3DDEB_BUFSIZE;
    debDesc.dwBufferSize = size;
    if (lpDev->lpVtbl->CreateExecuteBuffer(lpDev, &debDesc, &lpD3DExBuf, NULL)
        != D3D_OK)
            return FALSE;
    if (lpD3DExBuf->lpVtbl->Lock(lpD3DExBuf, &debDesc) != D3D_OK)
        return FALSE;
    lpBufStart = debDesc.lpData;
    memset(lpBufStart, 0, size);
    lpPointer = lpBufStart;

    VERTEX_DATA(&CubeVertices[0], NumVertices, lpPointer);

    lpInsStart = lpPointer;
    OP_SET_STATUS(D3DSETSTATUS_ALL, D3DSTATUS_DEFAULT, 2048, 2048, 0, 0, lpPointer);
    for (i = 0; i < 6; i++) {
        OP_STATE_LIGHT(1, lpPointer);
            STATE_DATA(D3DLIGHTSTATE_MATERIAL, D3DMaterialHandle[i], lpPointer);
        OP_PROCESS_VERTICES(1, lpPointer);
            PROCESSVERTICES_DATA(D3DPROCESSVERTICES_TRANSFORMLIGHT, i * 4, 4, lpPointer);
    }
    OP_STATE_RENDER(3, lpPointer);
        STATE_DATA(D3DRENDERSTATE_TEXTUREHANDLE, TextureHandle[1], lpPointer);
        STATE_DATA(D3DRENDERSTATE_WRAPU, FALSE, lpPointer);
        STATE_DATA(D3DRENDERSTATE_WRAPV, FALSE, lpPointer);
    /*
     * Make sure that the triangle data (not OP) will be QWORD aligned
     */
    if (QWORD_ALIGNED(lpPointer)) {
        OP_NOP(lpPointer);
    }
    OP_TRIANGLE_LIST(NumTri, lpPointer);
    for (i = 0; i < NumTri; i++) {
        ((LPD3DTRIANGLE)lpPointer)->v1 = CubeTri[i*3];
        ((LPD3DTRIANGLE)lpPointer)->v2 = CubeTri[i*3 + 1];
        ((LPD3DTRIANGLE)lpPointer)->v3 = CubeTri[i*3 + 2];
        ((LPD3DTRIANGLE)lpPointer)->wFlags = D3DTRIFLAG_EDGEENABLETRIANGLE;
        ((LPD3DTRIANGLE)lpPointer)++;
    }
    OP_EXIT(lpPointer);
    /*
     * Setup the execute data describing the buffer
     */
    lpD3DExBuf->lpVtbl->Unlock(lpD3DExBuf);
    memset(&d3dExData, 0, sizeof(D3DEXECUTEDATA));
    d3dExData.dwSize = sizeof(D3DEXECUTEDATA);
    d3dExData.dwVertexCount = NumVertices;
    d3dExData.dwInstructionOffset = (ULONG)((char*)lpInsStart - (char*)lpBufStart);
    d3dExData.dwInstructionLength = (ULONG)((char*)lpPointer - (char*)lpInsStart);
    lpD3DExBuf->lpVtbl->SetExecuteData(lpD3DExBuf, &d3dExData);

    return TRUE;
}

/****************************************************************************
  Keyboard and mouse handlers
 ****************************************************************************/

BOOL CDECL
KeyboardHandler(UINT message, WPARAM wParam, LPARAM lParam) {
    D3DMATRIX m;
    if (message == WM_KEYDOWN) {
        if ((int)wParam == VK_F11) {
            m = identity;
            m._11 = D3DVAL(1.5);
            m._22 = D3DVAL(1.5);
            m._33 = D3DVAL(1.5);
            MultiplyD3DMATRIX(&world, &world,  &m);
            return TRUE;
        } else if ((int)wParam == VK_F12) {
            m = identity;
            m._11 = D3DVAL(0.9);
            m._22 = D3DVAL(0.9);
            m._33 = D3DVAL(0.9);
            MultiplyD3DMATRIX(&world, &world, &m);
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CDECL
MouseHandler(UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_LBUTTONDOWN) {
        /* Get the start location for mouse rotations */
        spin = identity;
        Last.x = LOWORD(lParam);
        Last.y = HIWORD(lParam);
        return TRUE;
    } else if (message == WM_RBUTTONDOWN) {
        /* start spinning again */
        InitSpin();
        return TRUE;
    } else if (message == WM_MOUSEMOVE) {
        /* While the mouse button is down, keep track of movement
         * to update the eye position
         */
        if(GetKeyState(VK_LBUTTON) < 0) {
            Move.x = (int)LOWORD(lParam) - Last.x;
            Move.y = (int)HIWORD(lParam) - Last.y;
            Last.x = LOWORD(lParam);
            Last.y = HIWORD(lParam);
            return TRUE;
        }
    }
    return FALSE;
}

/**************************************************************************
  TransformCube

  Description:
    Multiplies the world matrix by the given transform matrix
 **************************************************************************/

/*****************************************************************************
  Rotatations
 *****************************************************************************/
#define M_PI            3.14159265358979323846
void
ConcatenateXRotation(LPD3DMATRIX lpM, float Degrees )
{
  float Temp01, Temp11, Temp21, Temp31;
  float Temp02, Temp12, Temp22, Temp32;
  float aElements[4][4];

  float Radians = (float)((Degrees/360) * M_PI * 2.0);

  float Sin = (float)sin(Radians), Cos = (float)cos(Radians);

  memcpy(aElements, lpM, sizeof(D3DMATRIX));
  Temp01 = aElements[0][1] * Cos + aElements[0][2] * Sin;
  Temp11 = aElements[1][1] * Cos + aElements[1][2] * Sin;
  Temp21 = aElements[2][1] * Cos + aElements[2][2] * Sin;
  Temp31 = aElements[3][1] * Cos + aElements[3][2] * Sin;

  Temp02 = aElements[0][1] * -Sin + aElements[0][2] * Cos;
  Temp12 = aElements[1][1] * -Sin + aElements[1][2] * Cos;
  Temp22 = aElements[2][1] * -Sin + aElements[2][2] * Cos;
  Temp32 = aElements[3][1] * -Sin + aElements[3][2] * Cos;

  lpM->_12 = Temp01;
  lpM->_22 = Temp11;
  lpM->_32 = Temp21;
  lpM->_42 = Temp31;
  lpM->_13 = Temp02;
  lpM->_23 = Temp12;
  lpM->_33 = Temp22;
  lpM->_43 = Temp32;
}

void
ConcatenateYRotation(LPD3DMATRIX lpM, float Degrees )
{
  float Temp00, Temp10, Temp20, Temp30;
  float Temp02, Temp12, Temp22, Temp32;
  float aElements[4][4];

  float Radians = (float)((Degrees/360) * M_PI * 2);

  float Sin = (float)sin(Radians), Cos = (float)cos(Radians);

  memcpy(aElements, lpM, sizeof(D3DMATRIX));
  Temp00 = aElements[0][0] * Cos + aElements[0][2] * -Sin;
  Temp10 = aElements[1][0] * Cos + aElements[1][2] * -Sin;
  Temp20 = aElements[2][0] * Cos + aElements[2][2] * -Sin;
  Temp30 = aElements[3][0] * Cos + aElements[3][2] * -Sin;

  Temp02 = aElements[0][0] * Sin + aElements[0][2] * Cos;
  Temp12 = aElements[1][0] * Sin + aElements[1][2] * Cos;
  Temp22 = aElements[2][0] * Sin + aElements[2][2] * Cos;
  Temp32 = aElements[3][0] * Sin + aElements[3][2] * Cos;

  lpM->_11 = Temp00;
  lpM->_21 = Temp10;
  lpM->_31 = Temp20;
  lpM->_41 = Temp30;
  lpM->_13 = Temp02;
  lpM->_23 = Temp12;
  lpM->_33 = Temp22;
  lpM->_43 = Temp32;
}

void
ConcatenateZRotation(LPD3DMATRIX lpM, float Degrees )
{
  float Temp00, Temp10, Temp20, Temp30;
  float Temp01, Temp11, Temp21, Temp31;
  float aElements[4][4];

  float Radians = (float)((Degrees/360) * M_PI * 2);

  float Sin = (float)sin(Radians), Cos = (float)cos(Radians);

  memcpy(aElements, lpM, sizeof(D3DMATRIX));
  Temp00 = aElements[0][0] * Cos + aElements[0][1] * Sin;
  Temp10 = aElements[1][0] * Cos + aElements[1][1] * Sin;
  Temp20 = aElements[2][0] * Cos + aElements[2][1] * Sin;
  Temp30 = aElements[3][0] * Cos + aElements[3][1] * Sin;

  Temp01 = aElements[0][0] * -Sin + aElements[0][1] * Cos;
  Temp11 = aElements[1][0] * -Sin + aElements[1][1] * Cos;
  Temp21 = aElements[2][0] * -Sin + aElements[2][1] * Cos;
  Temp31 = aElements[3][0] * -Sin + aElements[3][1] * Cos;

  lpM->_11 = Temp00;
  lpM->_21 = Temp10;
  lpM->_31 = Temp20;
  lpM->_41 = Temp30;
  lpM->_12 = Temp01;
  lpM->_22 = Temp11;
  lpM->_32 = Temp21;
  lpM->_42 = Temp31;
}
