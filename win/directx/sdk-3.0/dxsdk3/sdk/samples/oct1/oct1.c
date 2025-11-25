/*==========================================================================
 *
 *  Copyright (C) 1995, 1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File: oct1.c
 *
 ***************************************************************************/

#include <d3d.h>
#include <math.h>
#include "d3ddemo.h"

static D3DEXECUTEDATA d3dExData;
static LPDIRECT3DEXECUTEBUFFER lpD3DExBuf;
static D3DEXECUTEBUFFERDESC debDesc;
LPDIRECT3DLIGHT lpD3DLight;
LPDIRECT3DMATERIAL lpBmat, lpMat1, lpMat2;

extern LPD3DVECTOR D3DVECTORNormalise(LPD3DVECTOR);

/*
 * Global projection, view, world and identity matricies
 */
D3DMATRIXHANDLE hProj;
D3DMATRIXHANDLE hView;
D3DMATRIXHANDLE hViewRot, hDViewRot;
D3DMATRIXHANDLE hViewPos;
D3DMATRIXHANDLE hWorld, hDWorld;

D3DMATRIX proj = {
    D3DVAL(2.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(2.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(1.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(0.0)
};

D3DMATRIX viewpos = {
    D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(10.0), D3DVAL(1.0)
};
D3DMATRIX viewrot, view, dviewrot, dworld;
D3DMATRIX identity = {
    D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0)
};

void
OverrideDefaults(Defaults* defaults)
{
    lstrcpy(defaults->Name, "Octagon D3D Example");
}

BOOL
TickScene(LPDIRECT3DDEVICE lpDev, LPDIRECT3DVIEWPORT lpView)
{
    static int dir = 1;

    if (viewpos._43 < D3DVAL(4.0))
        dir = 0;
    if (viewpos._43 > D3DVAL(12.0))
        dir = 1;
    if (dir) 
        viewpos._43 -= D3DVAL(0.4);
    else 
        viewpos._43 += D3DVAL(0.4);
    if (lpDev->lpVtbl->SetMatrix(lpDev, hViewPos, &viewpos) != D3D_OK)
        return FALSE;
    return TRUE;
}

BOOL
RenderScene(LPDIRECT3DDEVICE lpDev, LPDIRECT3DVIEWPORT lpView,
            LPD3DRECT lpExtent)
{
    /*
     * Execute the instruction buffer
     */
    if (lpDev->lpVtbl->BeginScene(lpDev) != D3D_OK)
        return FALSE;
    if (lpDev->lpVtbl->Execute(lpDev, lpD3DExBuf, lpView, D3DEXECUTE_CLIPPED) != D3D_OK)
        return FALSE;
    if (lpDev->lpVtbl->EndScene(lpDev) != D3D_OK)
        return FALSE;
    if (lpD3DExBuf->lpVtbl->GetExecuteData(lpD3DExBuf, &d3dExData) != D3D_OK)
        return FALSE;
    *lpExtent = d3dExData.dsStatus.drExtent;
    if (!(TickScene(lpDev, lpView)))
        return FALSE;
    return TRUE;
}

void
ReleaseScene(void)
{
    return;
}

void
ReleaseView(LPDIRECT3DVIEWPORT lpView)
{
    if (lpView && lpD3DLight)
        lpView->lpVtbl->DeleteLight(lpView, lpD3DLight);
    RELEASE(lpD3DLight);
    RELEASE(lpD3DExBuf);
    RELEASE(lpMat2);
    RELEASE(lpBmat);
}

BOOL
InitScene(void)
{
    return TRUE;
}

#define NUM_VERTICES 6
#define NUM_TRIANGLES 8

BOOL
InitView(LPDIRECTDRAW lpDD, LPDIRECT3D lpD3D, LPDIRECT3DDEVICE lpDev, 
           LPDIRECT3DVIEWPORT lpView, int NumTextures,
           LPD3DTEXTUREHANDLE TextureHandle)
{
    D3DVERTEX v[NUM_VERTICES];
    D3DLIGHT light;
    LPVOID lpBufStart, lpInsStart, lpPointer;
    LPD3DTRIANGLE lpTri;
    LPDIRECT3DEXECUTEBUFFER lpD3DExCmdBuf;
    size_t size;
    int t[8][3] = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 1,
        5, 2, 1,
        5, 3, 2,
        5, 4, 3,
        5, 1, 4
    };
    D3DMATERIAL bmat, mat1, mat2;
    D3DMATERIALHANDLE hBmat, hMat1, hMat2;
    D3DTEXTUREHANDLE bTex;
    D3DTEXTUREHANDLE fooTex;
    int i;
    D3DVALUE ct, st;

    bTex = TextureHandle[1];
    fooTex = TextureHandle[0];
    memset(&bmat, 0, sizeof(D3DMATERIAL));
    bmat.dwSize = sizeof(D3DMATERIAL);
    bmat.diffuse.r = (D3DVALUE)1.0;
    bmat.diffuse.g = (D3DVALUE)1.0;
    bmat.diffuse.b = (D3DVALUE)1.0;
    bmat.ambient.r = (D3DVALUE)1.0;
    bmat.ambient.g = (D3DVALUE)1.0;
    bmat.ambient.b = (D3DVALUE)1.0;
    bmat.hTexture = fooTex;
    bmat.dwRampSize = 1;
    if (lpD3D->lpVtbl->CreateMaterial(lpD3D, &lpBmat, NULL) != D3D_OK) {
        return FALSE;
    }
    if (lpBmat->lpVtbl->SetMaterial(lpBmat, &bmat) != D3D_OK) {
        return FALSE;
    }
    if (lpBmat->lpVtbl->GetHandle(lpBmat, lpDev, &hBmat) != D3D_OK) {
        return FALSE;
    }
    if (lpView->lpVtbl->SetBackground(lpView, hBmat) != D3D_OK) {
        return FALSE;
    }

    /*
     * Set the view, world and projection matrices
     * Create a buffer for matrix set commands etc.
     */
    MAKE_MATRIX(lpDev, hViewRot, identity);
    MAKE_MATRIX(lpDev, hViewPos, viewpos);
    MAKE_MATRIX(lpDev, hView, identity);
    MAKE_MATRIX(lpDev, hProj, proj);
    MAKE_MATRIX(lpDev, hWorld, identity);
    ct = D3DVAL(cos(0.1));
    st = D3DVAL(sin(0.1));
    dviewrot = identity;
    dviewrot._22 = ct;
    dviewrot._23 = -st;
    dviewrot._32 = st;
    dviewrot._33 = ct;
    MAKE_MATRIX(lpDev, hDViewRot, dviewrot);
    dworld = identity;
    dworld._11 = ct;
    dworld._13 = -st;
    dworld._31 = st;
    dworld._33 = ct;
    MAKE_MATRIX(lpDev, hDWorld, dworld);
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
        STATE_DATA(D3DTRANSFORMSTATE_VIEW, hView, lpPointer);
        STATE_DATA(D3DTRANSFORMSTATE_WORLD, hWorld, lpPointer);
        STATE_DATA(D3DTRANSFORMSTATE_PROJECTION, hProj, lpPointer);
    OP_STATE_LIGHT(1, lpPointer);
        STATE_DATA(D3DLIGHTSTATE_AMBIENT, RGBA_MAKE(64, 64, 64, 64), lpPointer);
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
     * Setup a material
     */
    if (lpD3D->lpVtbl->CreateMaterial(lpD3D, &lpMat1, NULL) != D3D_OK) {
        return FALSE;
    }
    if (lpD3D->lpVtbl->CreateMaterial(lpD3D, &lpMat2, NULL) != D3D_OK) {
        return FALSE;
    }
    memset(&mat1, 0, sizeof(D3DMATERIAL));
    mat1.dwSize = sizeof(D3DMATERIAL);
    
    mat1.diffuse.r = (D3DVALUE)1.0;
    mat1.diffuse.g = (D3DVALUE)0.0;
    mat1.diffuse.b = (D3DVALUE)0.0;
    mat1.diffuse.a = (D3DVALUE)1.0;
    mat1.ambient.r = (D3DVALUE)1.0;
    mat1.ambient.g = (D3DVALUE)0.0;
    mat1.ambient.b = (D3DVALUE)0.0;
    mat1.specular.r = (D3DVALUE)1.0;
    mat1.specular.g = (D3DVALUE)1.0;
    mat1.specular.b = (D3DVALUE)1.0;
    mat1.power = (float)20.0;
    mat1.dwRampSize = 16;
    mat1.hTexture = bTex;
    lpMat1->lpVtbl->SetMaterial(lpMat1, &mat1);
    lpMat1->lpVtbl->GetHandle(lpMat1, lpDev, &hMat1);
    memset(&mat2, 0, sizeof(D3DMATERIAL));
    mat2.dwSize = sizeof(D3DMATERIAL);
    mat2.diffuse.r = (D3DVALUE)1.0;
    mat2.diffuse.g = (D3DVALUE)1.0;
    mat2.diffuse.b = (D3DVALUE)1.0;
    mat2.diffuse.a = (D3DVALUE)1.0;
    mat2.ambient.r = (D3DVALUE)1.0;
    mat2.ambient.g = (D3DVALUE)1.0;
    mat2.ambient.b = (D3DVALUE)1.0;
    mat2.specular.r = (D3DVALUE)1.0;
    mat2.specular.g = (D3DVALUE)1.0;
    mat2.specular.b = (D3DVALUE)1.0;
    mat2.power = (float)20.0;
    mat2.dwRampSize = 16;
    mat2.hTexture = bTex;
    lpMat2->lpVtbl->SetMaterial(lpMat2, &mat2);
    lpMat2->lpVtbl->GetHandle(lpMat2, lpDev, &hMat2);
    /*
     * Setup vertices
     */
    memset(&v[0], 0, sizeof(D3DVERTEX) * NUM_VERTICES);
    /* V 0 */
    v[0].x = D3DVALP(0.0, 12);
    v[0].y = D3DVALP(1.0, 12);
    v[0].z = D3DVALP(1.0, 12);

    v[0].nx = D3DVALP(0.0, 12);
    v[0].ny = D3DVALP(1.0, 12);
    v[0].nz = D3DVALP(0.0, 12);

    v[0].tu = D3DVAL(0.0);
    v[0].tv = D3DVAL(0.0);

    /* V 1 */
    v[1].x = D3DVALP(1.0, 12);
    v[1].y = D3DVALP(0.0, 12);
    v[1].z = D3DVALP(0.0, 12);

    v[1].nx = D3DVALP(1.0, 12);
    v[1].ny = D3DVALP(0.0, 12);
    v[1].nz = D3DVALP(-1.0, 12);

    v[1].tu = D3DVAL(1.0);
    v[1].tv = D3DVAL(1.0);

    D3DVECTORNormalise((LPD3DVECTOR) & v[1].nx);

    /* V 2 */
    v[2].x = D3DVALP(-1.0, 12);
    v[2].y = D3DVALP(0.0, 12);
    v[2].z = D3DVALP(0.0, 12);

    v[2].nx = D3DVALP(-1.0, 12);
    v[2].ny = D3DVALP(0.0, 12);
    v[2].nz = D3DVALP(-1.0, 12);

    v[2].tu = D3DVAL(0.0);
    v[2].tv = D3DVAL(1.0);

    D3DVECTORNormalise((LPD3DVECTOR) & v[2].nx);

    /* V 3 */
    v[3].x = D3DVALP(-1.0, 12);
    v[3].y = D3DVALP(0.0, 12);
    v[3].z = D3DVALP(2.0, 12);

    v[3].nx = D3DVALP(-1.0, 12);
    v[3].ny = D3DVALP(0.0, 12);
    v[3].nz = D3DVALP(1.0, 12);

    v[3].tu = D3DVAL(1.0);
    v[3].tv = D3DVAL(1.0);

    D3DVECTORNormalise((LPD3DVECTOR) & v[3].nx);

    /* V 4 */
    v[4].x = D3DVALP(1.0, 12);
    v[4].y = D3DVALP(0.0, 12);
    v[4].z = D3DVALP(2.0, 12);

    v[4].nx = D3DVALP(1.0, 12);
    v[4].ny = D3DVALP(0.0, 12);
    v[4].nz = D3DVALP(1.0, 12);

    v[4].tu = D3DVAL(0.0);
    v[4].tv = D3DVAL(1.0);

    D3DVECTORNormalise((LPD3DVECTOR) & v[4].nx);

    /* V 5 */
    v[5].x = D3DVALP(0.0, 12);
    v[5].y = D3DVALP(-1.0, 12);
    v[5].z = D3DVALP(1.0, 12);

    v[5].nx = D3DVALP(0.0, 12);
    v[5].ny = D3DVALP(-1.0, 12);
    v[5].nz = D3DVALP(0.0, 12);

    v[5].tu = D3DVAL(0.0);
    v[5].tv = D3DVAL(0.0);

    D3DVECTORNormalise((LPD3DVECTOR) & v[5].nx);

    /*
     * Create an execute buffer
     */
    size = sizeof(D3DVERTEX) * NUM_VERTICES;
    size += sizeof(D3DSTATUS) * 1;
    size += sizeof(D3DPROCESSVERTICES) * 2;
    size += sizeof(D3DINSTRUCTION) * 10;
    size += sizeof(D3DMATRIXMULTIPLY) * 3;
    size += sizeof(D3DSTATE) * 5;
    size += sizeof(D3DTRIANGLE) * NUM_TRIANGLES;
    memset(&debDesc, 0, sizeof(D3DEXECUTEBUFFERDESC));
    debDesc.dwSize = sizeof(D3DEXECUTEBUFFERDESC);
    debDesc.dwFlags = D3DDEB_BUFSIZE;
    debDesc.dwBufferSize = size;
    if (lpDev->lpVtbl->CreateExecuteBuffer(lpDev, &debDesc, &lpD3DExBuf,
                                           NULL) != D3D_OK) {
        return FALSE;
    }
    if (lpD3DExBuf->lpVtbl->Lock(lpD3DExBuf, &debDesc) != D3D_OK) {
        return FALSE;
    }
    lpBufStart = debDesc.lpData;
    memset(lpBufStart, 0, size);
    lpPointer = lpBufStart;
    /*
     * Copy vertices to execute buffer
     */
    VERTEX_DATA(&v[0], NUM_VERTICES, lpPointer);
    /*
     * Setup instructions in execute buffer
     */
    lpInsStart = lpPointer;
    OP_MATRIX_MULTIPLY(3, lpPointer);
        MATRIX_MULTIPLY_DATA(hViewRot, hDViewRot, hViewRot, lpPointer);
        MATRIX_MULTIPLY_DATA(hViewRot, hViewPos, hView, lpPointer);
        MATRIX_MULTIPLY_DATA(hWorld, hDWorld, hWorld, lpPointer);
    OP_STATE_LIGHT(1, lpPointer);
        STATE_DATA(D3DLIGHTSTATE_MATERIAL, hMat1, lpPointer);
    OP_SET_STATUS(D3DSETSTATUS_ALL, D3DSTATUS_DEFAULT, 2048, 2048, 0, 0, lpPointer);
    OP_PROCESS_VERTICES(1, lpPointer);
        PROCESSVERTICES_DATA(D3DPROCESSVERTICES_TRANSFORMLIGHT, 0, 5, lpPointer);
    OP_STATE_LIGHT(1, lpPointer);
        STATE_DATA(D3DLIGHTSTATE_MATERIAL, hMat2, lpPointer);
    OP_PROCESS_VERTICES(1, lpPointer);
        PROCESSVERTICES_DATA(D3DPROCESSVERTICES_TRANSFORMLIGHT, 5, 1, lpPointer);
    OP_STATE_RENDER(3, lpPointer);
        STATE_DATA(D3DRENDERSTATE_TEXTUREHANDLE, bTex, lpPointer);
        STATE_DATA(D3DRENDERSTATE_WRAPU, FALSE, lpPointer);
        STATE_DATA(D3DRENDERSTATE_WRAPV, FALSE, lpPointer);
    /*
     * Make sure that the triangle data (not OP) will be QWORD aligned
     */
    if (QWORD_ALIGNED(lpPointer)) {
        OP_NOP(lpPointer);
    }
    OP_TRIANGLE_LIST(NUM_TRIANGLES, lpPointer);
        lpTri = (LPD3DTRIANGLE)lpPointer;
        for (i = 0; i < NUM_TRIANGLES; i++) {
            lpTri->v1 = t[i][0];
            lpTri->v2 = t[i][1];
            lpTri->v3 = t[i][2];
            lpTri->wFlags = D3DTRIFLAG_EDGEENABLETRIANGLE;
            lpTri++;
        }
        lpPointer = (void*)lpTri;
    OP_EXIT(lpPointer);
    /*
     * Setup the execute data
     */
    lpD3DExBuf->lpVtbl->Unlock(lpD3DExBuf);
    memset(&d3dExData, 0, sizeof(D3DEXECUTEDATA));
    d3dExData.dwSize = sizeof(D3DEXECUTEDATA);
    d3dExData.dwVertexCount = NUM_VERTICES;
    d3dExData.dwInstructionOffset = (ULONG) ((char *)lpInsStart - (char *)lpBufStart);
    d3dExData.dwInstructionLength = (ULONG) ((char *)lpPointer - (char*)lpInsStart);
    lpD3DExBuf->lpVtbl->SetExecuteData(lpD3DExBuf, &d3dExData);
    /*
     * Setup lights
     */
    memset(&light, 0, sizeof(D3DLIGHT));
    light.dwSize = sizeof(D3DLIGHT);
    light.dltType = D3DLIGHT_DIRECTIONAL;
    light.dcvColor.r = D3DVAL(1.0);
    light.dcvColor.g = D3DVAL(1.0);
    light.dcvColor.b = D3DVAL(1.0);
    light.dcvColor.a = D3DVAL(1.0);
    light.dvDirection.x = D3DVALP(0.0, 12);
    light.dvDirection.y = D3DVALP(0.0, 12);
    light.dvDirection.z = D3DVALP(1.0, 12);
    if (lpD3D->lpVtbl->CreateLight(lpD3D, &lpD3DLight, NULL) != D3D_OK) {
        return FALSE;
    }
    if (lpD3DLight->lpVtbl->SetLight(lpD3DLight, &light) != D3D_OK) {
        return FALSE;
    }
    if (lpView->lpVtbl->AddLight(lpView, lpD3DLight) != D3D_OK) {
        return FALSE;
    }
    return TRUE;
}
