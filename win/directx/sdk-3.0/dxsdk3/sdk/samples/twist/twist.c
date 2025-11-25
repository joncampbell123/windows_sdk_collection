/*==========================================================================
 *
 *  Copyright (C) 1995, 1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File: twist.c
 *
 ***************************************************************************/

#include <d3d.h>
#include <math.h>
#include <malloc.h>
#include "d3dmacs.h"
#include "d3ddemo.h"

/*
 * Globals to keep track of execute buffer
 */
static D3DEXECUTEDATA d3dExData;
static LPDIRECT3DEXECUTEBUFFER lpD3DExBuf;
static D3DEXECUTEBUFFERDESC debDesc;
LPDIRECT3DMATERIAL lpbmat; /* Object for background material */
LPDIRECT3DLIGHT lpD3DLight; /* object for light */
LPDIRECT3DMATERIAL lpred_mat, lpblue_mat;           /* Objects for materials */

/*
 * Global projection, view, world and identity matricies
 */
D3DMATRIXHANDLE hProj;
D3DMATRIXHANDLE hView;
D3DMATRIXHANDLE hSpin;
D3DMATRIXHANDLE hDSpin;
D3DMATRIXHANDLE hWorld;
D3DMATRIXHANDLE hWorld1;
D3DMATRIXHANDLE hWorld2;
D3DMATRIXHANDLE hDWorld1;
D3DMATRIXHANDLE hDWorld2;

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
D3DMATRIX spin;
D3DMATRIX dspin;
D3DMATRIX world;
D3DMATRIX world1;
D3DMATRIX dworld1;
D3DMATRIX world2;
D3DMATRIX dworld2;
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
    D3DMATERIALHANDLE hred_mat, hblue_mat; /* two material handles */
    D3DTEXTUREHANDLE hTex;                 /* texture map handles  */
    LPD3DVERTEX lpV;                       /* object's vertices    */
    LPD3DTRIANGLE lpTri;                   /* object's triangles   */
    int num_vertices, num_faces;
} objData;

#define PI 3.14159265359

void
OverrideDefaults(Defaults* defaults)
{
    lstrcpy(defaults->Name, "Twist D3D Example");
}

/*
 * A function to rotate a number of D3DVERTEX points around the X axis.
 */
void 
XRotateD3DVERTEX(float theta, int count, LPD3DVERTEX lpV)
{
    float st, ct;
    int i;

    st = (float)sin(theta); ct = (float)cos(theta);
    for (i = 0; i < count; i++) {
        float y, z;
        y = lpV[i].y; z = lpV[i].z;
        lpV[i].y = ct * y + st * z;
        lpV[i].z = -st * y + ct * z;
        y = lpV[i].ny; z = lpV[i].nz;
        lpV[i].ny = ct * y + st * z;
        lpV[i].nz = -st * y + ct * z;
    }
}


/*
 * Renders the scene
 */
BOOL
RenderScene(LPDIRECT3DDEVICE lpDev, LPDIRECT3DVIEWPORT lpView,
            LPD3DRECT lpExtent)
{
    HRESULT ddrval;

    /*
     * Execute the instruction buffer and blt
     */
    ddrval = lpDev->lpVtbl->BeginScene(lpDev);
    if (ddrval != D3D_OK)
        return FALSE;
    ddrval = lpDev->lpVtbl->Execute(lpDev, lpD3DExBuf, lpView, D3DEXECUTE_CLIPPED);
    if (ddrval != D3D_OK)
        return FALSE;
    ddrval = lpDev->lpVtbl->EndScene(lpDev);
    if (ddrval != D3D_OK)
        return FALSE;
    ddrval = lpD3DExBuf->lpVtbl->GetExecuteData(lpD3DExBuf, &d3dExData);
    if (ddrval != D3D_OK)
        return FALSE;
    /*
     * Return the extent
     */
    *lpExtent = d3dExData.dsStatus.drExtent;
    return TRUE;
}


void
ReleaseScene(void)
{
    if (objData.lpTri)
        free(objData.lpTri);
    if (objData.lpV)
        free(objData.lpV);
}

void
ReleaseView(LPDIRECT3DVIEWPORT lpView)
{
    if (lpView)
        lpView->lpVtbl->DeleteLight(lpView, lpD3DLight);
    RELEASE(lpbmat);
    RELEASE(lpred_mat);
    RELEASE(lpblue_mat);
    RELEASE(lpD3DExBuf);
    RELEASE(lpD3DLight);
}

/*
 * Builds the scene
 */
BOOL
InitScene(void)
{
    int i;
    /*
     * Generate the sphere which will be used as the twisted object.  Scale
     * one axis long.  Rotate the sphere around the x axis so the end caps
     * are around the z axis (for effect).  Stretch one end of the sphere 
     * out to allow an easier view of the twisting.
     */
    if (!(GenerateSphere((float)1.7, 9, 15, (float)1.0, (float)2.0,
                         (float)1.0, &objData.lpV, &objData.lpTri,
                         &objData.num_vertices, &objData.num_faces)))
                                return FALSE;
                                
    /*
     * As we are going to twist the sphere the quads in the sphere will
     * no longer stay flat. We need to change all start flat setting to just
     * start.
     */
    for (i = 0; i < objData.num_faces; i++) {
        if (((objData.lpTri[i].wFlags & 0x1f) < 30) &&
            ((objData.lpTri[i].wFlags & 0x1f) > 0)) {
            objData.lpTri[i].wFlags &= ~0x1f;
        }
    }
        
    XRotateD3DVERTEX((float)(PI / 2.0), objData.num_vertices, objData.lpV);
    for (i = 0; i < objData.num_vertices; i++)
        if (objData.lpV[i].z > 0) objData.lpV[i].z += (float)2.0;
    return TRUE;
}

/*
 * Initializes the execute buffer for rendering.
 */
#define DT 0.05 /* amount to twist, different from spin for effect */
#define DS 0.08 /* amount to spin world each time */
BOOL
InitView(LPDIRECTDRAW lpDD, LPDIRECT3D lpD3D, LPDIRECT3DDEVICE lpDev,
           LPDIRECT3DVIEWPORT lpView, int NumTextures,
           LPD3DTEXTUREHANDLE TextureHandle)
{
    /* Variables for execute buffer generation */
    LPDIRECT3DEXECUTEBUFFER lpD3DExCmdBuf;
    LPD3DTRIANGLE lpTri;
    LPVOID lpBufStart, lpInsStart, lpPointer;
    size_t size;

    /* Materials and lights */
    D3DLIGHT light;                                     
    D3DMATERIAL bmat;
    D3DMATERIALHANDLE hbmat;
    D3DMATERIAL red_mat, blue_mat;

    int i;
    float ct, st;

    HRESULT ddrval;

    /*
     * Set background to black material
     */
    if (lpD3D->lpVtbl->CreateMaterial(lpD3D, &lpbmat, NULL) != D3D_OK)
        return FALSE;
    memset(&bmat, 0, sizeof(D3DMATERIAL));
    bmat.dwSize = sizeof(D3DMATERIAL);
    bmat.dwRampSize = 1;
    ddrval = lpbmat->lpVtbl->SetMaterial(lpbmat, &bmat);
    ddrval = lpbmat->lpVtbl->GetHandle(lpbmat, lpDev, &hbmat);
    ddrval = lpView->lpVtbl->SetBackground(lpView, hbmat);

    /*
     * Set the view, world and projection matrices
     */
    MAKE_MATRIX(lpDev, hView, view);
    MAKE_MATRIX(lpDev, hProj, proj);
    MAKE_MATRIX(lpDev, hWorld, identity);
    MAKE_MATRIX(lpDev, hSpin, identity);
    MAKE_MATRIX(lpDev, hWorld1, identity);
    MAKE_MATRIX(lpDev, hWorld2, identity);

    /*
     * Setup the matrices which spin the buffer
     */
    ct = D3DVAL(cos(DS));
    st = D3DVAL(sin(DS));
    dspin = identity;
    dspin._11 = ct;
    dspin._13 = -st;
    dspin._31 = st;
    dspin._33 = ct;
    MAKE_MATRIX(lpDev, hDSpin, dspin);

    ct = D3DVAL(cos(DT));
    st = D3DVAL(sin(DT));
    dworld1 = identity;
    dworld1._11 = ct;
    dworld1._21 = -st;
    dworld1._12 = st;
    dworld1._22 = ct;
    MAKE_MATRIX(lpDev, hDWorld1, dworld1);
    
    ct = D3DVAL(cos(-DT));
    st = D3DVAL(sin(-DT));
    dworld2 = identity;
    dworld2._11 = ct;
    dworld2._21 = -st;
    dworld2._12 = st;
    dworld2._22 = ct;
    MAKE_MATRIX(lpDev, hDWorld2, dworld2);

    /*
     * Use an execute buffer to set some transform state
     */
    size = 0;
    size += sizeof(D3DINSTRUCTION) * 3;
    size += sizeof(D3DSTATE) * 3;
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
    OP_STATE_TRANSFORM(2, lpPointer);
        STATE_DATA(D3DTRANSFORMSTATE_PROJECTION, hProj, lpPointer);
        STATE_DATA(D3DTRANSFORMSTATE_VIEW, hView, lpPointer);
    OP_STATE_LIGHT(1, lpPointer);
        STATE_DATA(D3DLIGHTSTATE_AMBIENT, RGBA_MAKE(20, 20, 20, 20), lpPointer);
    OP_EXIT(lpPointer);
    
    lpD3DExCmdBuf->lpVtbl->Unlock(lpD3DExCmdBuf);
    memset(&d3dExData, 0, sizeof(D3DEXECUTEDATA));
    d3dExData.dwSize = sizeof(D3DEXECUTEDATA);
    d3dExData.dwInstructionOffset = (ULONG) 0;
    d3dExData.dwInstructionLength = (ULONG) ((char *)lpPointer - (char *)lpInsStart);
    lpD3DExCmdBuf->lpVtbl->SetExecuteData(lpD3DExCmdBuf, &d3dExData);
    ddrval = lpDev->lpVtbl->BeginScene(lpDev);
    if (ddrval != D3D_OK)
        return FALSE;
    lpDev->lpVtbl->Execute(lpDev, lpD3DExCmdBuf, lpView, D3DEXECUTE_UNCLIPPED);
    ddrval = lpDev->lpVtbl->EndScene(lpDev);
    if (ddrval != D3D_OK)
        return FALSE;
    lpD3DExCmdBuf->lpVtbl->Release(lpD3DExCmdBuf);

    /*
     * Create each material, set its description and obtain a handle to it.
     */
    objData.hTex = TextureHandle[1];
    if (lpD3D->lpVtbl->CreateMaterial(lpD3D, &lpred_mat, NULL) != D3D_OK)
        return FALSE;
    memset(&red_mat, 0, sizeof(D3DMATERIAL));
    red_mat.dwSize = sizeof(D3DMATERIAL);
    red_mat.diffuse.r = (D3DVALUE)1.0;
    red_mat.diffuse.g = (D3DVALUE)0.8;
    red_mat.diffuse.b = (D3DVALUE)0.8;
    red_mat.diffuse.a = (D3DVALUE)1.0;
    red_mat.ambient.r = (D3DVALUE)1.0;
    red_mat.ambient.g = (D3DVALUE)0.0;
    red_mat.ambient.b = (D3DVALUE)0.0;
    red_mat.specular.r = (D3DVALUE)1.0;
    red_mat.specular.g = (D3DVALUE)1.0;
    red_mat.specular.b = (D3DVALUE)1.0;
    red_mat.power = (float)20.0;
    red_mat.dwRampSize = 16;
    red_mat.hTexture = objData.hTex;
    lpred_mat->lpVtbl->SetMaterial(lpred_mat, &red_mat);
    lpred_mat->lpVtbl->GetHandle(lpred_mat, lpDev, &objData.hred_mat);
    if (lpD3D->lpVtbl->CreateMaterial(lpD3D, &lpblue_mat, NULL) != D3D_OK)
        return FALSE;
    memset(&blue_mat, 0, sizeof(D3DMATERIAL));
    blue_mat.dwSize = sizeof(D3DMATERIAL);
    blue_mat.diffuse.r = (D3DVALUE)0.5;
    blue_mat.diffuse.g = (D3DVALUE)0.5;
    blue_mat.diffuse.b = (D3DVALUE)1.0;
    blue_mat.diffuse.a = (D3DVALUE)1.0;
    blue_mat.ambient.r = (D3DVALUE)0.0;
    blue_mat.ambient.g = (D3DVALUE)0.0;
    blue_mat.ambient.b = (D3DVALUE)1.0;
    blue_mat.specular.r = (D3DVALUE)1.0;
    blue_mat.specular.g = (D3DVALUE)1.0;
    blue_mat.specular.b = (D3DVALUE)1.0;
    blue_mat.power = (float)20.0;
    blue_mat.dwRampSize = 16;
    blue_mat.hTexture = objData.hTex;
    lpblue_mat->lpVtbl->SetMaterial(lpblue_mat, &blue_mat);
    lpblue_mat->lpVtbl->GetHandle(lpblue_mat, lpDev, &objData.hblue_mat);

    /*
     * Create the main execute buffer
     */
    size = sizeof(D3DVERTEX) * objData.num_vertices * 2; /* two copies */
    size += sizeof(D3DPROCESSVERTICES) * 2;
    size += sizeof(D3DSTATUS) * 1;
    size += sizeof(D3DINSTRUCTION) * 16;
    size += sizeof(D3DSTATE) * 6;
    size += sizeof(D3DMATRIXMULTIPLY) * 5;
    size += sizeof(D3DTRIANGLE) * objData.num_faces;

    memset(&debDesc, 0, sizeof(D3DEXECUTEBUFFERDESC));
    debDesc.dwSize = sizeof(D3DEXECUTEBUFFERDESC);
    debDesc.dwFlags = D3DDEB_BUFSIZE;
    debDesc.dwBufferSize = size;
    if (lpDev->lpVtbl->CreateExecuteBuffer(lpDev, &debDesc, &lpD3DExBuf, 
                                           NULL) != D3D_OK)
                                               return FALSE;
    if (lpD3DExBuf->lpVtbl->Lock(lpD3DExBuf, &debDesc) != D3D_OK)
        return FALSE;
    lpBufStart = debDesc.lpData;
    memset(lpBufStart, 0, size);
    lpPointer = lpBufStart;
    /*
     * Copy two sets of the vertices into the buffer for the first render.
     */
    VERTEX_DATA(objData.lpV, objData.num_vertices, lpPointer);
    VERTEX_DATA(objData.lpV, objData.num_vertices, lpPointer);

    lpInsStart = lpPointer;
    OP_SET_STATUS(D3DSETSTATUS_ALL, D3DSTATUS_DEFAULT, 2048, 2048, 0, 0, lpPointer);
    OP_MATRIX_MULTIPLY(1, lpPointer);
        MATRIX_MULTIPLY_DATA(hDSpin, hSpin, hSpin, lpPointer);
    OP_STATE_LIGHT(1, lpPointer);
        STATE_DATA(D3DLIGHTSTATE_MATERIAL, objData.hblue_mat, lpPointer);
    OP_MATRIX_MULTIPLY(1, lpPointer);
        MATRIX_MULTIPLY_DATA(hWorld1, hDWorld1, hWorld1, lpPointer);
    OP_MATRIX_MULTIPLY(1, lpPointer);
        MATRIX_MULTIPLY_DATA(hWorld1, hSpin, hWorld, lpPointer);
    OP_STATE_TRANSFORM(1, lpPointer);
        STATE_DATA(D3DTRANSFORMSTATE_WORLD, hWorld, lpPointer);
    OP_PROCESS_VERTICES(1, lpPointer);
        PROCESSVERTICES_DATA(D3DPROCESSVERTICES_TRANSFORMLIGHT, 0, objData.num_vertices, lpPointer);

    OP_STATE_LIGHT(1, lpPointer);
        STATE_DATA(D3DLIGHTSTATE_MATERIAL, objData.hred_mat, lpPointer);
    OP_MATRIX_MULTIPLY(1, lpPointer);
        MATRIX_MULTIPLY_DATA(hWorld2, hDWorld2, hWorld2, lpPointer);
    OP_MATRIX_MULTIPLY(1, lpPointer);
        MATRIX_MULTIPLY_DATA(hWorld2, hSpin, hWorld, lpPointer);
    OP_STATE_TRANSFORM(1, lpPointer);
        STATE_DATA(D3DTRANSFORMSTATE_WORLD, hWorld, lpPointer);
    OP_PROCESS_VERTICES(1, lpPointer);
        PROCESSVERTICES_DATA(D3DPROCESSVERTICES_TRANSFORMLIGHT, objData.num_vertices, objData.num_vertices, lpPointer);
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
        lpTri = lpPointer;
        TRIANGLE_LIST_DATA(objData.lpTri, objData.num_faces, lpPointer);
    /*
     * If the z coordinate of a vertex referenced by a triangle is > 0, 
     * make the triangle reference the same vertex in the second copy of 
     * the vetices.
     */ 
    for (i = 0; i < objData.num_faces; i++) {   
        if (objData.lpV[lpTri->v1].z > 0)
            lpTri->v1 += objData.num_vertices;
        if (objData.lpV[lpTri->v2].z > 0)
            lpTri->v2 += objData.num_vertices;
        if (objData.lpV[lpTri->v3].z > 0)
            lpTri->v3 += objData.num_vertices;
        lpTri++;
    }
    OP_EXIT(lpPointer);

    lpD3DExBuf->lpVtbl->Unlock(lpD3DExBuf);
    /*
     * Setup the execute data describing the buffer
     */
    memset(&d3dExData, 0, sizeof(D3DEXECUTEDATA));
    d3dExData.dwSize = sizeof(D3DEXECUTEDATA);
    d3dExData.dwVertexCount = objData.num_vertices * 2;
    d3dExData.dwInstructionOffset = (ULONG)((char *)lpInsStart - (char *)lpBufStart);
    d3dExData.dwInstructionLength = (ULONG)((char*)lpPointer - (char *)lpInsStart);
    lpD3DExBuf->lpVtbl->SetExecuteData(lpD3DExBuf, &d3dExData);

    /*
     * Add one light.
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


