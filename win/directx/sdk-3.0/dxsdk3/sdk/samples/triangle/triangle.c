/*==========================================================================
 *
 *  Copyright (C) 1995, 1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File: triangle.c
 *
 ***************************************************************************/

#include <math.h>
#include <d3d.h>
#include "d3ddemo.h"

static D3DEXECUTEDATA d3dExData;
static LPDIRECT3DEXECUTEBUFFER lpD3DExBuf;
LPDIRECT3DMATERIAL lpBmat, lpMat1;
#define NUM_VERTICES 3
#define NUM_TRIANGLES 1

void
OverrideDefaults(Defaults* defaults)
{
    lstrcpy(defaults->Name, "Triangle D3D Example");
    defaults->rs.bPerspCorrect = FALSE;
    defaults->bResizingDisabled = TRUE;
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
    if (lpDev->lpVtbl->Execute(lpDev, lpD3DExBuf,
                               lpView, D3DEXECUTE_UNCLIPPED) != D3D_OK)
        return FALSE;
    if (lpDev->lpVtbl->EndScene(lpDev) != D3D_OK)
        return FALSE;
    if (lpD3DExBuf->lpVtbl->GetExecuteData(lpD3DExBuf, &d3dExData) != D3D_OK)
        return FALSE;
    *lpExtent = d3dExData.dsStatus.drExtent;
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
    lpView;
    RELEASE(lpD3DExBuf);
    RELEASE(lpMat1);
    RELEASE(lpBmat);
}

BOOL
InitScene(void)
{
    return TRUE;
}

BOOL
InitView(LPDIRECTDRAW lpDD, LPDIRECT3D lpD3D, LPDIRECT3DDEVICE lpDev,
           LPDIRECT3DVIEWPORT lpView, int NumTextures,
           LPD3DTEXTUREHANDLE TextureHandle)
{
    LPVOID lpBufStart, lpInsStart, lpPointer;
    D3DEXECUTEBUFFERDESC debDesc;
    size_t size;
    D3DTLVERTEX src_v[NUM_VERTICES];
    int t[8][3] = {
        0, 1, 2,
    };
    D3DMATERIAL bmat, mat;
    D3DMATERIALHANDLE hBmat, hMat1;

    if (lpD3D->lpVtbl->CreateMaterial(lpD3D, &lpBmat, NULL) != D3D_OK) {
        return FALSE;
    }
    memset(&bmat, 0, sizeof(D3DMATERIAL));
    bmat.dwSize = sizeof(D3DMATERIAL);
    bmat.diffuse.r = (D3DVALUE)1.0;
    bmat.diffuse.g = (D3DVALUE)1.0;
    bmat.diffuse.b = (D3DVALUE)1.0;
    bmat.ambient.r = (D3DVALUE)1.0;
    bmat.ambient.g = (D3DVALUE)1.0;
    bmat.ambient.b = (D3DVALUE)1.0;
    bmat.hTexture = TextureHandle[0];
    bmat.dwRampSize = 1;
    lpBmat->lpVtbl->SetMaterial(lpBmat, &bmat);
    lpBmat->lpVtbl->GetHandle(lpBmat, lpDev, &hBmat);
    lpView->lpVtbl->SetBackground(lpView, hBmat);

    if (lpD3D->lpVtbl->CreateMaterial(lpD3D, &lpMat1, NULL) != D3D_OK) {
        return FALSE;
    }
    memset(&mat, 0, sizeof(D3DMATERIAL));
    mat.dwSize = sizeof(D3DMATERIAL);
    mat.diffuse.r = (D3DVALUE)1.0;
    mat.diffuse.g = (D3DVALUE)1.0;
    mat.diffuse.b = (D3DVALUE)1.0;
    mat.ambient.r = (D3DVALUE)1.0;
    mat.ambient.g = (D3DVALUE)1.0;
    mat.ambient.b = (D3DVALUE)1.0;
#define SPECULAR
#ifdef SPECULAR
    mat.specular.r = (D3DVALUE)1.0;
    mat.specular.g = (D3DVALUE)1.0;
    mat.specular.b = (D3DVALUE)1.0;
    mat.power = (float)40.0;
#else
    mat.specular.r = (D3DVALUE)0.0;
    mat.specular.g = (D3DVALUE)0.0;
    mat.specular.b = (D3DVALUE)0.0;
    mat.power = (float)0.0;
#endif    
    mat.hTexture = TextureHandle[1];
    mat.dwRampSize = 16;
    lpMat1->lpVtbl->SetMaterial(lpMat1, &mat);
    lpMat1->lpVtbl->GetHandle(lpMat1, lpDev, &hMat1);
    /*
     * Setup vertices
     */
    memset(&src_v[0], 0, sizeof(D3DVERTEX) * NUM_VERTICES);
    /* V 0 */
    src_v[0].sx = D3DVAL(10.0);
    src_v[0].sy = D3DVAL(10.0);
    src_v[0].sz = D3DVAL(0.1);
    src_v[0].rhw = D3DVAL(1.0);
    src_v[0].color = RGBA_MAKE(255, 0, 255, 255);
    src_v[0].specular = RGB_MAKE(0, 0, 255);
    src_v[0].tu = D3DVAL(0.0);
    src_v[0].tv = D3DVAL(0.0);
    /* V 1 */
    src_v[1].sx = D3DVAL(300.0);
    src_v[1].sy = D3DVAL(50.0);
    src_v[1].sz = D3DVAL(0.9);
    src_v[1].rhw = D3DVAL(2.0);
    src_v[1].color = RGBA_MAKE(255, 255, 255, 255);
    src_v[1].specular = RGB_MAKE(0, 0, 0);
    src_v[1].tu = D3DVAL(1.0);
    src_v[1].tv = D3DVAL(1.0);
    /* V 2 */
    src_v[2].sx = D3DVAL(150.0);
    src_v[2].sy = D3DVAL(180.0);
    src_v[2].sz = D3DVAL(0.6);
    src_v[2].rhw = D3DVAL(1.0);
    src_v[2].color = RGBA_MAKE(255, 255, 0, 255);
    src_v[2].specular = RGB_MAKE(0, 0, 0);
    src_v[2].tu = D3DVAL(0.0);
    src_v[2].tv = D3DVAL(1.0);
    /*
     * Create an execute buffer
     */
    size = sizeof(D3DVERTEX) * NUM_VERTICES;
    size += sizeof(D3DINSTRUCTION) * 6;
    size += sizeof(D3DSTATE) * 2;
    size += sizeof(D3DPROCESSVERTICES);
    size += sizeof(D3DTRIANGLE) * 1;
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
    VERTEX_DATA(&src_v[0], NUM_VERTICES, lpPointer);
    /*
     * Setup instructions in execute buffer
     */
    lpInsStart = lpPointer;
    OP_STATE_LIGHT(1, lpPointer);
        STATE_DATA(D3DLIGHTSTATE_MATERIAL, hMat1, lpPointer);
    OP_PROCESS_VERTICES(1, lpPointer);
        PROCESSVERTICES_DATA(D3DPROCESSVERTICES_COPY |
                             D3DPROCESSVERTICES_UPDATEEXTENTS, 0, NUM_VERTICES, lpPointer);
    OP_STATE_RENDER(1, lpPointer);
        STATE_DATA(D3DRENDERSTATE_TEXTUREHANDLE, TextureHandle[1], lpPointer);
    /*
     * Make sure that the triangle data (not OP) will be QWORD aligned
     */
    if (QWORD_ALIGNED(lpPointer)) {
        OP_NOP(lpPointer);
    }
    OP_TRIANGLE_LIST(1, lpPointer);
        ((LPD3DTRIANGLE)lpPointer)->v1 = 0;
        ((LPD3DTRIANGLE)lpPointer)->v2 = 1;
        ((LPD3DTRIANGLE)lpPointer)->v3 = 2;
        ((LPD3DTRIANGLE)lpPointer)->wFlags = D3DTRIFLAG_EDGEENABLETRIANGLE;
        lpPointer = ((char*)lpPointer) + sizeof(D3DTRIANGLE);
    OP_EXIT(lpPointer);
    /*
     * Setup the execute data
     */
    lpD3DExBuf->lpVtbl->Unlock(lpD3DExBuf);
    memset(&d3dExData, 0, sizeof(D3DEXECUTEDATA));
    d3dExData.dwSize = sizeof(D3DEXECUTEDATA);
    d3dExData.dwVertexCount = NUM_VERTICES;
    d3dExData.dwInstructionOffset = (ULONG) ((char *)lpInsStart - (char *)lpBufStart);
    d3dExData.dwInstructionLength = (ULONG) ((char *)lpPointer - (char *)lpInsStart);
    lpD3DExBuf->lpVtbl->SetExecuteData(lpD3DExBuf, &d3dExData);

    return TRUE;
}

