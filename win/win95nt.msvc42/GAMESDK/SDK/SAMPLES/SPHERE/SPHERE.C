/*==========================================================================
 *
 *  Copyright (C) 1995, 1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File: sphere.c
 *
 ***************************************************************************/

#include <math.h>
#include <malloc.h>
#include "d3d.h"
#include "d3ddemo.h"

/*
 * Globals to keep track of execute buffer
 */
static D3DEXECUTEDATA d3dExData;
static LPDIRECT3DEXECUTEBUFFER lpD3DExBuf;
static D3DEXECUTEBUFFERDESC debDesc;
/*
 * Gobals for materials and lights
 */
LPDIRECT3DMATERIAL lpbmat;
LPDIRECT3DMATERIAL lpmat;
LPDIRECT3DLIGHT lpD3DLight;             
/*
 * Global projection, view, world and identity matricies
 */
D3DMATRIXHANDLE hProj;
D3DMATRIXHANDLE hView;
D3DMATRIXHANDLE hWorld;
D3DMATRIXHANDLE hDWorld;

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
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(7.0), D3DVAL(1.0)
};
D3DMATRIX world= {
    D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0)
};
D3DMATRIX identity = {
    D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0)
};

/*
 * A structure which holds the object's data
 */
struct {
    D3DMATERIALHANDLE hmat;                /* material handle      */
    D3DTEXTUREHANDLE hTex;                 /* texture map handles  */
    LPD3DVERTEX lpV;                       /* object's vertices    */
    LPD3DTRIANGLE lpTri;                   /* object's triangles   */
    int num_vertices, num_faces;
} objData;

#define PI 3.14159265359
#define DS 0.08 /* amount to spin world each time */

void
OverrideDefaults(Defaults* defaults)
{
    lstrcpy(defaults->Name, "Sphere D3D Example");
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
    if (lpDev->lpVtbl->BeginScene(lpDev) != D3D_OK)
        return FALSE;
    if (lpDev->lpVtbl->Execute(lpDev, lpD3DExBuf,
                               lpView, D3DEXECUTE_UNCLIPPED) != D3D_OK)
        return FALSE;
    if (lpDev->lpVtbl->EndScene(lpDev) != D3D_OK)
        return FALSE;
    if (lpD3DExBuf->lpVtbl->GetExecuteData(lpD3DExBuf, &d3dExData)!= D3D_OK)
        return FALSE;
    *lpExtent = d3dExData.dsStatus.drExtent;

    return TRUE;
}

BOOL
InitScene(void)
{
    /*
     * Generate the sphere.
     */
    if (!(GenerateSphere((float)2.2, 18, 20, (float)1.0, (float)1.0,
                         (float)1.0, &objData.lpV, &objData.lpTri,
                         &objData.num_vertices, &objData.num_faces)))
                                return FALSE;
    return TRUE;
}

void
ReleaseScene(void)
{
    if (objData.lpV)
        free(objData.lpV);
    if (objData.lpTri)
        free(objData.lpTri);
}

/*
 * Release the memory allocated for the scene and all D3D objects created.
 */
void
ReleaseView(LPDIRECT3DVIEWPORT lpView)
{
    if (lpView)
        lpView->lpVtbl->DeleteLight(lpView, lpD3DLight);
    RELEASE(lpD3DLight);
    RELEASE(lpD3DExBuf);
    RELEASE(lpmat);
    RELEASE(lpbmat);
}

/*
 * Builds the scene and initializes the execute buffer for rendering.  Returns 0 on failure.
 */
BOOL
InitView(LPDIRECTDRAW lpDD, LPDIRECT3D lpD3D, LPDIRECT3DDEVICE lpDev,
           LPDIRECT3DVIEWPORT lpView, int NumTextures,
           LPD3DTEXTUREHANDLE TextureHandle)
{
    /* Pointers into the exectue buffer. */
    LPVOID lpBufStart, lpInsStart, lpPointer;
    LPDIRECT3DEXECUTEBUFFER lpD3DExCmdBuf;
    size_t size;

    /* Light and materials */
    D3DLIGHT light;                                     
    D3DMATERIAL bmat;
    D3DMATERIALHANDLE hbmat;
    D3DMATERIAL mat;

    D3DMATRIX dworld;
    float ct, st;

    /*
     * Set the view, world and projection matrices
     */
    MAKE_MATRIX(lpDev, hView, view);
    MAKE_MATRIX(lpDev, hProj, proj);
    MAKE_MATRIX(lpDev, hWorld, world);
    /*
     * Create a buffer for matrix set commands etc.
     */
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
    if (lpD3DExCmdBuf->lpVtbl->Lock(lpD3DExCmdBuf, &debDesc) != D3D_OK)
        return FALSE;
    lpBufStart = debDesc.lpData;
    memset(lpBufStart, 0, size);
    lpPointer = lpBufStart;

    lpInsStart = lpPointer;
    OP_STATE_TRANSFORM(3, lpPointer);
        STATE_DATA(D3DTRANSFORMSTATE_WORLD, hWorld, lpPointer);
        STATE_DATA(D3DTRANSFORMSTATE_VIEW, hView, lpPointer);
        STATE_DATA(D3DTRANSFORMSTATE_PROJECTION, hProj, lpPointer);
    OP_STATE_LIGHT(1, lpPointer);
        STATE_DATA(D3DLIGHTSTATE_AMBIENT, RGBA_MAKE(10, 10, 10, 10), lpPointer);
    OP_EXIT(lpPointer);
    /*
     * Setup the execute data describing the buffer
     */
    lpD3DExCmdBuf->lpVtbl->Unlock(lpD3DExCmdBuf);
    memset(&d3dExData, 0, sizeof(D3DEXECUTEDATA));
    d3dExData.dwSize = sizeof(D3DEXECUTEDATA);
    d3dExData.dwInstructionOffset = (ULONG) 0;
    d3dExData.dwInstructionLength = (ULONG) ((char*)lpPointer - (char*)lpInsStart);
    lpD3DExCmdBuf->lpVtbl->SetExecuteData(lpD3DExCmdBuf, &d3dExData);
    lpDev->lpVtbl->BeginScene(lpDev);
    lpDev->lpVtbl->Execute(lpDev, lpD3DExCmdBuf, lpView, D3DEXECUTE_UNCLIPPED);
    lpDev->lpVtbl->EndScene(lpDev);
    /*
     * We are done with the command buffer.
     */
    lpD3DExCmdBuf->lpVtbl->Release(lpD3DExCmdBuf);
    /*
     * Set background to black material
     */
    if (lpD3D->lpVtbl->CreateMaterial(lpD3D, &lpbmat, NULL) != D3D_OK)
        return FALSE;
    memset(&bmat, 0, sizeof(D3DMATERIAL));
    bmat.dwSize = sizeof(D3DMATERIAL);
    bmat.dwRampSize = 1;
    lpbmat->lpVtbl->SetMaterial(lpbmat, &bmat);
    lpbmat->lpVtbl->GetHandle(lpbmat, lpDev, &hbmat);
    lpView->lpVtbl->SetBackground(lpView, hbmat);
    /*
     * Create a material, set its description and obtain a handle to it.
     */
    objData.hTex = TextureHandle[1];
    if (lpD3D->lpVtbl->CreateMaterial(lpD3D, &lpmat, NULL) != D3D_OK)
        return FALSE;
    memset(&mat, 0, sizeof(D3DMATERIAL));
    mat.dwSize = sizeof(D3DMATERIAL);
    mat.diffuse.r = (D3DVALUE)1.0;
    mat.diffuse.g = (D3DVALUE)1.0;
    mat.diffuse.b = (D3DVALUE)1.0;
    mat.diffuse.a = (D3DVALUE)1.0;
    mat.ambient.r = (D3DVALUE)0.0;
    mat.ambient.g = (D3DVALUE)0.0;
    mat.ambient.b = (D3DVALUE)1.0;
    mat.specular.r = (D3DVALUE)1.0;
    mat.specular.g = (D3DVALUE)1.0;
    mat.specular.b = (D3DVALUE)1.0;
    mat.power = (float)40.0;
    mat.dwRampSize = 32;
    mat.hTexture = objData.hTex;
    lpmat->lpVtbl->SetMaterial(lpmat, &mat);
    lpmat->lpVtbl->GetHandle(lpmat, lpDev, &objData.hmat);
    /*
     * Create the matrix which spins the sphere
     */
    ct = D3DVAL(cos(DS));
    st = D3DVAL(sin(DS));
    dworld = identity;
    dworld._11 = ct;
    dworld._13 = -st;
    dworld._31 = st;
    dworld._33 = ct;
    MAKE_MATRIX(lpDev, hDWorld, dworld);
    /*
     * Create the main execute buffer
     */
    size = sizeof(D3DVERTEX) * objData.num_vertices;
    size += sizeof(D3DPROCESSVERTICES) * 1;
    size += sizeof(D3DSTATUS) * 1;
    size += sizeof(D3DINSTRUCTION) * 8;
    size += sizeof(D3DSTATE) * 5;
    size += sizeof(D3DMATRIXMULTIPLY) * 1;
    size += sizeof(D3DTRIANGLE) * objData.num_faces;
    memset(&debDesc, 0, sizeof(D3DEXECUTEBUFFERDESC));
    debDesc.dwSize = sizeof(D3DEXECUTEBUFFERDESC);
    debDesc.dwFlags = D3DDEB_BUFSIZE;
    debDesc.dwBufferSize = size;
    if (lpDev->lpVtbl->CreateExecuteBuffer(lpDev, &debDesc, &lpD3DExBuf, 
                                           NULL) != D3D_OK)
                                               return FALSE;
    /*
     * lock it so it can be filled
     */
    if (lpD3DExBuf->lpVtbl->Lock(lpD3DExBuf, &debDesc) != D3D_OK)
        return FALSE;
    lpBufStart = debDesc.lpData;
    memset(lpBufStart, 0, size);
    lpPointer = lpBufStart;

    VERTEX_DATA(objData.lpV, objData.num_vertices, lpPointer);
    /*
     * Save the location of the first instruction and add instructions to 
     * execute buffer.
     */
    lpInsStart = lpPointer;
    OP_STATE_LIGHT(1, lpPointer);
        STATE_DATA(D3DLIGHTSTATE_MATERIAL, objData.hmat, lpPointer);
    OP_MATRIX_MULTIPLY(1, lpPointer);
        MATRIX_MULTIPLY_DATA(hDWorld, hWorld, hWorld, lpPointer);
        
    OP_SET_STATUS(D3DSETSTATUS_ALL, D3DSTATUS_DEFAULT, 2048, 2048, 0, 0, lpPointer);
    
    OP_PROCESS_VERTICES(1, lpPointer);
        PROCESSVERTICES_DATA(D3DPROCESSVERTICES_TRANSFORMLIGHT, 0, objData.num_vertices, lpPointer);
    OP_STATE_RENDER(2, lpPointer);
        STATE_DATA(D3DRENDERSTATE_TEXTUREHANDLE, objData.hTex, lpPointer);
        STATE_DATA(D3DRENDERSTATE_WRAPU, TRUE, lpPointer);
    /*
     * Make sure that the triangle data (not OP) will be QWORD aligned
     */
    if (QWORD_ALIGNED(lpPointer)) {
        OP_NOP(lpPointer);
    }
    OP_TRIANGLE_LIST(objData.num_faces, lpPointer);
        TRIANGLE_LIST_DATA(objData.lpTri, objData.num_faces, lpPointer);
    OP_EXIT(lpPointer);
    /*
     * Setup the execute data describing the buffer
     */
    lpD3DExBuf->lpVtbl->Unlock(lpD3DExBuf);
    memset(&d3dExData, 0, sizeof(D3DEXECUTEDATA));
    d3dExData.dwSize = sizeof(D3DEXECUTEDATA);
    d3dExData.dwVertexCount = objData.num_vertices;
    d3dExData.dwInstructionOffset = (ULONG)((char*)lpInsStart - (char*)lpBufStart);
    d3dExData.dwInstructionLength = (ULONG)((char*)lpPointer - (char*)lpInsStart);
    lpD3DExBuf->lpVtbl->SetExecuteData(lpD3DExBuf, &d3dExData);
    /*
     *  Create the light
     */
    memset(&light, 0, sizeof(D3DLIGHT));
    light.dwSize = sizeof(D3DLIGHT);
    light.dltType = D3DLIGHT_DIRECTIONAL;
    light.dcvColor.r = D3DVAL(0.9);
    light.dcvColor.g = D3DVAL(0.9);
    light.dcvColor.b = D3DVAL(0.9);
    light.dcvColor.a = D3DVAL(1.0);
    light.dvDirection.x = D3DVALP(0.0, 12);
    light.dvDirection.y = D3DVALP(0.0, 12);
    light.dvDirection.z = D3DVALP(1.0, 12);
    light.dvAttenuation0 = (float)1.0;
    light.dvAttenuation1 = (float)0.0;
    light.dvAttenuation2 = (float)0.0;
    if (lpD3D->lpVtbl->CreateLight(lpD3D, &lpD3DLight, NULL) != D3D_OK)
        return FALSE;
    if (lpD3DLight->lpVtbl->SetLight(lpD3DLight, &light) != D3D_OK)
        return FALSE;
    if (lpView->lpVtbl->AddLight(lpView, lpD3DLight) != D3D_OK)
        return FALSE;

    return TRUE;
}


