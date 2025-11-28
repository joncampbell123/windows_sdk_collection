//-----------------------------------------------------------------------------
// File: XFileLoader.cpp
//
// Desc: Support code for loading DirectX .X files.
//
//
// Copyright (c) 1997-1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define STRICT
#define D3D_OVERLOADS
#include <stdio.h>
#include "D3DUtil.h"
#include "D3DMath.h"
#include "D3DTextr.h"
#include "dxfile.h"
#include "rmxfguid.h"
#include "rmxftmpl.h"
#include "XFileLoader.h"




//-----------------------------------------------------------------------------
// Name: class CD3DFileBaseObject
// Desc: 
//-----------------------------------------------------------------------------
class CD3DFileBaseObject
{
protected:
    CD3DFileBaseObject* m_pNext;
    CD3DFileBaseObject* m_pChild;

    CHAR                m_strName[80];

public:
    VOID    AddNext( CD3DFileBaseObject* );
    VOID    AddChild( CD3DFileBaseObject* );
    
    virtual VOID Render( LPDIRECT3DDEVICE3, LPDIRECT3DMATERIAL3 )=0;
    
    virtual VOID ComputeBoundingSphereCenter( D3DMATRIX* pmat, D3DVECTOR*, DWORD*  );
    virtual VOID ComputeBoundingSphereRadius( D3DMATRIX* pmat, D3DVECTOR* pvCenter,
                                              FLOAT* pfRadius );

    virtual HRESULT GetFrameMatrix( CHAR* strName, D3DMATRIX** ppMatrix );
    virtual HRESULT GetMeshVertices( CHAR* strName, D3DVERTEX** ppvVertices,
                                     DWORD* pdwNumVertices, WORD** ppwIndices, 
                                     DWORD* pdwNumIndices );

    CD3DFileBaseObject( CHAR* strName );
    ~CD3DFileBaseObject();
};




//-----------------------------------------------------------------------------
// Name: class CD3DFileFrame
// Desc: 
//-----------------------------------------------------------------------------
class CD3DFileFrame : public CD3DFileBaseObject
{
    D3DMATRIX m_mat;
public:
    VOID    SetMatrix( D3DMATRIX* pmat ) { m_mat = *pmat; }

    VOID    Render( LPDIRECT3DDEVICE3, LPDIRECT3DMATERIAL3 );

    HRESULT GetFrameMatrix( CHAR* strName, D3DMATRIX** ppMatrix );
    VOID    ComputeBoundingSphereCenter( D3DMATRIX* pmat, D3DVECTOR*, DWORD*  );
    VOID    ComputeBoundingSphereRadius( D3DMATRIX* pmat, D3DVECTOR* pvCenter,
                                         FLOAT* pfRadius );

    CD3DFileFrame( CHAR* strName );
};




//-----------------------------------------------------------------------------
// Name: class CD3DFileMesh
// Desc: 
//-----------------------------------------------------------------------------
class CD3DFileMesh : public CD3DFileBaseObject
{
    CHAR         m_strTexture[80];
    D3DMATERIAL  m_mtrl;

    DWORD        m_dwNumVertices;
    D3DVERTEX*   m_pVertices;
    DWORD        m_dwNumIndices;
    WORD*        m_pIndices;
public:

    VOID    SetTexture( CHAR* strName )       { strcpy( m_strTexture, strName ); }
    VOID    SetMaterial( D3DMATERIAL* pmtrl ) { m_mtrl = *pmtrl; }
    VOID    SetNormals( D3DVECTOR* pNormals );
    VOID    SetTextureCoords( FLOAT* pTexCoords );

    VOID    Render( LPDIRECT3DDEVICE3, LPDIRECT3DMATERIAL3 );

    VOID    ComputeBoundingSphereCenter( D3DMATRIX* pmat, D3DVECTOR*, DWORD*  );
    VOID    ComputeBoundingSphereRadius( D3DMATRIX* pmat, D3DVECTOR* pvCenter,
                                         FLOAT* pfRadius );

    HRESULT GetMeshVertices( CHAR* strName, D3DVERTEX** ppvVertices,
                             DWORD* pdwNumVertices, WORD** ppwIndices, 
                             DWORD* pdwNumIndices );

    HRESULT ComputeNormals();

    CD3DFileMesh( CHAR* strName, DWORD, D3DVECTOR*, DWORD, VOID* );
    ~CD3DFileMesh();
};




//-----------------------------------------------------------------------------
// Name: class CD3DFileBaseObject
// Desc: The following functions implement the CD3DFileBaseObject class
//-----------------------------------------------------------------------------



CD3DFileBaseObject::CD3DFileBaseObject( CHAR* strName )
{
    m_pNext      = NULL;
    m_pChild     = NULL;
    m_strName[0] = 0;
    
    if( strName )
        strcpy( m_strName, strName );
}




CD3DFileBaseObject::~CD3DFileBaseObject()
{
    SAFE_DELETE( m_pChild );
    SAFE_DELETE( m_pNext );
}




VOID CD3DFileBaseObject::AddChild( CD3DFileBaseObject* pChild )
{
    if( m_pChild ) m_pChild->AddNext( pChild );
    else           m_pChild = pChild;
}




VOID CD3DFileBaseObject::AddNext( CD3DFileBaseObject* pNext )
{
    if( m_pNext ) m_pNext->AddNext( pNext );
    else          m_pNext = pNext;
}




VOID CD3DFileBaseObject::ComputeBoundingSphereCenter( D3DMATRIX* pmat,
                                                          D3DVECTOR* pvPos, 
                                                          DWORD* pdwCount )
{
    if( m_pChild )
        m_pChild->ComputeBoundingSphereCenter( pmat, pvPos, pdwCount );
    if( m_pNext )
        m_pNext->ComputeBoundingSphereCenter( pmat, pvPos, pdwCount );
}




VOID CD3DFileBaseObject::ComputeBoundingSphereRadius( D3DMATRIX* pmat,
                                                          D3DVECTOR* pvCenter,
                                                          FLOAT* pfRadius )
{
    if( m_pChild )
        m_pChild->ComputeBoundingSphereRadius( pmat, pvCenter, pfRadius );
    if( m_pNext )
        m_pNext->ComputeBoundingSphereRadius( pmat, pvCenter, pfRadius );
}




HRESULT CD3DFileBaseObject::GetFrameMatrix( CHAR* strName,
                                                D3DMATRIX** ppMatrix )
{
    if( NULL == *ppMatrix && m_pChild )
        m_pChild->GetFrameMatrix( strName, ppMatrix );
    if( NULL == *ppMatrix && m_pNext)
        m_pNext->GetFrameMatrix( strName, ppMatrix );

    return S_OK;
}




HRESULT CD3DFileBaseObject::GetMeshVertices( CHAR* strName,
                                D3DVERTEX** ppvVertices, DWORD* pdwNumVertices,
                                WORD** ppwIndices, DWORD* pdwNumIndices )
{
    if( NULL == *ppvVertices )
        if( m_pChild) m_pChild->GetMeshVertices( strName, ppvVertices,
                                                 pdwNumVertices, ppwIndices,
                                                 pdwNumIndices );
    if( NULL == *ppvVertices )
        if( m_pNext) m_pNext->GetMeshVertices( strName, ppvVertices,
                                               pdwNumVertices, ppwIndices,
                                               pdwNumIndices );
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: class CD3DFileFrame
// Desc: The following functions implement the CD3DFileFrame class
//-----------------------------------------------------------------------------


CD3DFileFrame::CD3DFileFrame( CHAR* strName )
              :CD3DFileBaseObject( strName )
{
    D3DUtil_SetIdentityMatrix( m_mat );
}




VOID CD3DFileFrame::Render( LPDIRECT3DDEVICE3 pd3dDevice,
                            LPDIRECT3DMATERIAL3 pmtrl )
{
    if( m_pChild )
    {
        // Save the old matrix sate
        D3DMATRIX matWorldOld, matWorldNew;
        pd3dDevice->GetTransform( D3DTRANSFORMSTATE_WORLD, &matWorldOld );

        // Concat the frame matrix with the current world matrix
        D3DMath_MatrixMultiply( matWorldNew, matWorldOld, m_mat );
        pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorldNew );
        
        // Render the child nodes
        m_pChild->Render( pd3dDevice, pmtrl );

        // Restore the old matrix state
        pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorldOld );
    }

    // Render the remaining sibling nodes
    if( m_pNext )
        m_pNext->Render( pd3dDevice, pmtrl );
}




VOID CD3DFileFrame::ComputeBoundingSphereCenter( D3DMATRIX* pmat, D3DVECTOR* pvPos,
                                               DWORD* pdwCount )
{
    D3DMATRIX matFrame;
    D3DMath_MatrixMultiply( matFrame, m_mat, *pmat );

    if( m_pChild )
        m_pChild->ComputeBoundingSphereCenter( &matFrame, pvPos, pdwCount );
    if( m_pNext )
        m_pNext->ComputeBoundingSphereCenter( pmat, pvPos, pdwCount );
}




VOID CD3DFileFrame::ComputeBoundingSphereRadius( D3DMATRIX* pmat, D3DVECTOR* pvCenter,
                                               FLOAT* pfRadius )
{
    D3DMATRIX matFrame;
    D3DMath_MatrixMultiply( matFrame, m_mat, *pmat );
    
    if( m_pChild )
        m_pChild->ComputeBoundingSphereRadius( &matFrame, pvCenter, pfRadius );

    if( m_pNext )
        m_pNext->ComputeBoundingSphereRadius( pmat, pvCenter, pfRadius );
}




HRESULT CD3DFileFrame::GetFrameMatrix( CHAR* strName, D3DMATRIX** ppMatrix )
{
    if( !strcmp( m_strName, strName ) )
    {
        *ppMatrix = &m_mat;
    }

    return CD3DFileBaseObject::GetFrameMatrix( strName, ppMatrix );
}




//-----------------------------------------------------------------------------
// Name: class CD3DFileMesh
// Desc: The following functions implement the CD3DFileMesh class
//-----------------------------------------------------------------------------


CD3DFileMesh::CD3DFileMesh( CHAR* strName, DWORD dwNumVertices,
                            D3DVECTOR* pVertexData, DWORD dwNumFaces, 
                            VOID* pFaceData ) 
             :CD3DFileBaseObject( strName )
{
    D3DUtil_InitMaterial( m_mtrl, 1.0f, 1.0f, 1.0f );
    m_strTexture[0] = 0;
    m_dwNumVertices = dwNumVertices;
    m_pVertices     = new D3DVERTEX[m_dwNumVertices];

    for( DWORD i=0; i< m_dwNumVertices; i++ )
    {
        ZeroMemory( &m_pVertices[i], sizeof(D3DVERTEX) );
        m_pVertices[i].x = pVertexData[i].x;
        m_pVertices[i].y = pVertexData[i].y;
        m_pVertices[i].z = pVertexData[i].z;
    }

    // Count the number of indices (converting n-sided faces to triangles)
    m_dwNumIndices   = 0L;
    LONG pAddr = (LONG)pFaceData;
    for( i=0; i<dwNumFaces; i++ )
    {
        DWORD dwNumVerticesPerFace = *((DWORD*)pAddr);
        
        pAddr += 4;
        pAddr += 4*dwNumVerticesPerFace;

        m_dwNumIndices += 3*(dwNumVerticesPerFace-2);
    }

    // Allocate memory for the indices
    m_pIndices = new WORD[m_dwNumIndices];

    pAddr = (LONG)pFaceData;
    WORD* pIndices = m_pIndices;

    // Assign the indices
    for( i=0; i<dwNumFaces; i++ )
    {
        DWORD dwNumVerticesPerFace = *((DWORD*)pAddr);
        pAddr += 4;

        WORD v1, v2, v3, vn;

        for( DWORD j=0; j<dwNumVerticesPerFace; j++ )
        {
            if( j==0 )
            {
                v1 = (WORD)(*((DWORD*)pAddr));
                *pIndices++ = v1;
            }
            else if( j==1 )
            {
                v2 = (WORD)(*((DWORD*)pAddr));
                *pIndices++ = v2;
            }
            else if( j==2 )
            {
                v3 = (WORD)(*((DWORD*)pAddr));
                *pIndices++ = v3;
                vn = v3;
            }
            else
            {
                v3 = (WORD)(*((DWORD*)pAddr));
                *pIndices++ = v1;
                *pIndices++ = vn;
                *pIndices++ = v3;
                vn = v3;
            }
            
            pAddr += 4;
        }
    }
}




CD3DFileMesh::~CD3DFileMesh()
{
    D3DTextr_DestroyTexture( m_strTexture );
    SAFE_DELETE( m_pVertices );
    SAFE_DELETE( m_pIndices );
}




VOID CD3DFileMesh::Render( LPDIRECT3DDEVICE3 pd3dDevice,
                           LPDIRECT3DMATERIAL3 pmtrl )
{
    // Set up new render states
    pmtrl->SetMaterial( &m_mtrl );
    pd3dDevice->SetTexture( 0, D3DTextr_GetTexture( m_strTexture ) );
    
    // Render the mesh
    pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                      m_pVertices, m_dwNumVertices, 
                                      m_pIndices, m_dwNumIndices, NULL );

    if( m_pNext )
        m_pNext->Render( pd3dDevice, pmtrl );
}




VOID CD3DFileMesh::ComputeBoundingSphereCenter( D3DMATRIX* pmat, D3DVECTOR* pvPos,
                                                DWORD* pdwCount )
{
    for( DWORD i=0; i<m_dwNumVertices; i++ )
    {
        FLOAT x = m_pVertices[i].x;
        FLOAT y = m_pVertices[i].y;
        FLOAT z = m_pVertices[i].z;

        pvPos->x += pmat->_11*x + pmat->_21*y + pmat->_31*z + pmat->_41;
        pvPos->y += pmat->_12*x + pmat->_22*y + pmat->_32*z + pmat->_42;
        pvPos->z += pmat->_13*x + pmat->_23*y + pmat->_33*z + pmat->_43;
    }

    (*pdwCount) += m_dwNumVertices;
    
    CD3DFileBaseObject::ComputeBoundingSphereCenter( pmat, pvPos, pdwCount );
}




VOID CD3DFileMesh::ComputeBoundingSphereRadius( D3DMATRIX* pmat, D3DVECTOR* pvCenter,
                                              FLOAT* pfRadius )
{
    FLOAT cx = pvCenter->x;
    FLOAT cy = pvCenter->y;
    FLOAT cz = pvCenter->z;

    for( DWORD i=0; i<m_dwNumVertices; i++ )
    {
        FLOAT x = m_pVertices[i].x;
        FLOAT y = m_pVertices[i].y;
        FLOAT z = m_pVertices[i].z;

        FLOAT dx = ( pmat->_11*x + pmat->_21*y + pmat->_31*z + pmat->_41 ) - cx;
        FLOAT dy = ( pmat->_12*x + pmat->_22*y + pmat->_32*z + pmat->_42 ) - cy;
        FLOAT dz = ( pmat->_13*x + pmat->_23*y + pmat->_33*z + pmat->_43 ) - cz;

        FLOAT fDist = (FLOAT)sqrt( dx*dx + dy*dy + dz*dz );

        if( fDist > (*pfRadius) )
            (*pfRadius) = fDist;
    }

    CD3DFileBaseObject::ComputeBoundingSphereRadius( pmat, pvCenter, pfRadius );
}




HRESULT CD3DFileMesh::GetMeshVertices( CHAR* strName, D3DVERTEX** ppvVertices,
                                     DWORD* pdwNumVertices, WORD** ppwIndices, 
                                     DWORD* pdwNumIndices )
{
    if( !strcmp( m_strName, strName ) )
    {
        *ppvVertices    = m_pVertices;
        *pdwNumVertices = m_dwNumVertices;
        *ppwIndices     = m_pIndices;
        *pdwNumIndices  = m_dwNumIndices;
    }

    return CD3DFileBaseObject::GetMeshVertices( strName, ppvVertices,
                                          pdwNumVertices, ppwIndices, 
                                          pdwNumIndices );
}




HRESULT CD3DFileMesh::ComputeNormals()
{
    D3DVECTOR* pNormals = new D3DVECTOR[m_dwNumVertices];
    ZeroMemory( pNormals, sizeof(D3DVECTOR)*m_dwNumVertices );

    for( DWORD i=0; i<m_dwNumIndices; i+=3 )
    {
        WORD a = m_pIndices[i+0];
        WORD b = m_pIndices[i+1];
        WORD c = m_pIndices[i+2];

        D3DVECTOR* v1 = (D3DVECTOR*)&m_pVertices[a];
        D3DVECTOR* v2 = (D3DVECTOR*)&m_pVertices[b];
        D3DVECTOR* v3 = (D3DVECTOR*)&m_pVertices[c];

        D3DVECTOR n = Normalize( CrossProduct( *v2-*v1, *v3-*v2 ) );

        pNormals[a] += n;
        pNormals[b] += n;
        pNormals[c] += n;
    }

    for( i=0; i<m_dwNumVertices; i++ )
    {
        pNormals[i] = Normalize( pNormals[i] );

        m_pVertices[i].nx = pNormals[i].x;
        m_pVertices[i].ny = pNormals[i].y;
        m_pVertices[i].nz = pNormals[i].z;
    }

    delete pNormals;

    return S_OK;
}




VOID CD3DFileMesh::SetNormals( D3DVECTOR* pNormals )
{
    for( DWORD i=0; i<m_dwNumVertices; i++ )
    {
        m_pVertices[i].nx = pNormals[i].x;
        m_pVertices[i].ny = pNormals[i].y;
        m_pVertices[i].nz = pNormals[i].z;
    }
}


                    
                    
VOID CD3DFileMesh::SetTextureCoords( FLOAT* pTexCoords )
{
    for( DWORD i=0; i<m_dwNumVertices; i++ )
    {
        m_pVertices[i].tu = pTexCoords[2*i+0];
        m_pVertices[i].tv = pTexCoords[2*i+1];
    }
}


                    
                    
//-----------------------------------------------------------------------------
// Name: ParseXXXX()
// Desc: The following routines implement the DirectX .X file loader.
//-----------------------------------------------------------------------------


HRESULT ParseMaterial( LPDIRECTXFILEDATA pDXFileData, CD3DFileMesh* pMesh )
{
    // Read data from the file
    LONG  pData;
    DWORD dwSize;
    if( FAILED( pDXFileData->GetData(NULL, &dwSize, (VOID**)&pData) ) )
        return NULL;

    // Set the material properties for the mesh
    D3DMATERIAL mtrl;
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL) );
    mtrl.dwSize = sizeof(D3DMATERIAL);
    memcpy( &mtrl.diffuse,  (VOID*)(pData+0),  sizeof(FLOAT)*4 );
    memcpy( &mtrl.ambient,  (VOID*)(pData+0),  sizeof(FLOAT)*4 );
    memcpy( &mtrl.power,    (VOID*)(pData+16), sizeof(FLOAT)*1 );
    memcpy( &mtrl.specular, (VOID*)(pData+20), sizeof(FLOAT)*3 );
    memcpy( &mtrl.emissive, (VOID*)(pData+32), sizeof(FLOAT)*3 );
    pMesh->SetMaterial( &mtrl );

    LPDIRECTXFILEOBJECT pDXChildObj;
    if( SUCCEEDED( pDXFileData->GetNextObject(&pDXChildObj) ) )
    {
        LPDIRECTXFILEDATA pDXChildData;

        if( SUCCEEDED( pDXChildObj->QueryInterface( IID_IDirectXFileData,
                                                    (VOID**)&pDXChildData) ) )
        {
            const GUID* pguid;
            pDXChildData->GetType( &pguid );

            if( TID_D3DRMTextureFilename == *pguid )
            {
                CHAR** string;

                if( FAILED( pDXChildData->GetData(NULL, &dwSize, (VOID**)&string) ) )
                    return NULL;

                D3DTextr_CreateTexture( *string );
                pMesh->SetTexture( *string );
            }

            pDXChildData->Release();
        }

        pDXChildObj->Release();
    }
    return S_OK;
}




HRESULT ParseMeshMaterialList( LPDIRECTXFILEDATA pDXFileData, CD3DFileMesh* pMesh )
{
    LPDIRECTXFILEOBJECT        pDXChildObj;
    LPDIRECTXFILEDATA          pDXChildData;
    LPDIRECTXFILEDATAREFERENCE pDXChildDataRef;

    if( SUCCEEDED( pDXFileData->GetNextObject(&pDXChildObj) ) )
    {
        if( SUCCEEDED( pDXChildObj->QueryInterface( IID_IDirectXFileData,
                                                    (VOID**)&pDXChildData) ) )
        {
            const GUID* pguid;
            pDXChildData->GetType( &pguid );

            if( TID_D3DRMMaterial == *pguid )
            {
                ParseMaterial( pDXChildData, pMesh );
            }

            pDXChildData->Release();
        }

        if( SUCCEEDED( pDXChildObj->QueryInterface( IID_IDirectXFileDataReference,
                                                    (VOID**)&pDXChildDataRef) ) )
        {
            if( SUCCEEDED( pDXChildDataRef->Resolve( &pDXChildData ) ) )
            {
                const GUID* pguid;
                pDXChildData->GetType( &pguid );

                if( TID_D3DRMMaterial == *pguid )
                {
                    ParseMaterial( pDXChildData, pMesh );
                }

                pDXChildData->Release();
            }
            pDXChildDataRef->Release();
        }

        pDXChildObj->Release();
    }
    return S_OK;
}




HRESULT ParseMesh( LPDIRECTXFILEDATA pFileData, CD3DFileFrame* pParentFrame )
{
    DWORD dwNameLen=80;
    CHAR  strName[80];
    if( FAILED( pFileData->GetName( strName, &dwNameLen ) ) )
        return E_FAIL;

    // Read the Mesh data from the file
    LONG  pData;
    DWORD dwSize;
    if( FAILED( pFileData->GetData(NULL, &dwSize, (VOID**)&pData) ) )
        return E_FAIL;

    DWORD      dwNumVertices = *((DWORD*)pData);    pData += 4;
    D3DVECTOR* pVertices     = ((D3DVECTOR*)pData); pData += 12*dwNumVertices;
    DWORD      dwNumFaces    = *((DWORD*)pData);    pData += 4;
    VOID*      pFaceData     = (VOID*)pData;

    // Create the Mesh object
    CD3DFileMesh* pMesh = new CD3DFileMesh( strName, dwNumVertices, pVertices,
                                            dwNumFaces, pFaceData );

    BOOL bHasNormals = FALSE;

    // Enumerate child objects.
    LPDIRECTXFILEOBJECT pChildObj;
    while( SUCCEEDED( pFileData->GetNextObject( &pChildObj ) ) )
    {
        LPDIRECTXFILEDATA pChildData;

        if( SUCCEEDED( pChildObj->QueryInterface( IID_IDirectXFileData,
                            (VOID**)&pChildData ) ) )
        {
            const GUID* pGUID;
            LONG        pData;
            DWORD       dwSize;
            
            pChildData->GetType( &pGUID );
            if( FAILED( pChildData->GetData(NULL, &dwSize, (VOID**)&pData) ) )
            {
                delete pMesh;
                return NULL;
            }

            if( TID_D3DRMMeshMaterialList == *pGUID )
                ParseMeshMaterialList( pChildData, pMesh );
            
            if( TID_D3DRMMeshNormals == *pGUID )
            {
                DWORD      dwNumNormals = *((DWORD*)pData);
                D3DVECTOR* pNormals     = (D3DVECTOR*)(pData+4); 

                if( dwNumNormals == dwNumVertices )
                {
                    pMesh->SetNormals( pNormals );
                    bHasNormals = TRUE;
                }
            }
            
            if( TID_D3DRMMeshTextureCoords == *pGUID )
            {
                // Copy the texture coords into the mesh's vertices
                DWORD  dwNumTexCoords = *((DWORD*)pData);
                FLOAT* pTexCoords     = (FLOAT*)(pData+4);

                if( dwNumTexCoords == dwNumVertices )
                    pMesh->SetTextureCoords( pTexCoords );
            }

            pChildData->Release();
        }

        pChildObj->Release();
    }

    if( FALSE == bHasNormals )
        pMesh->ComputeNormals();

    pParentFrame->AddChild( pMesh );
    return S_OK;
}




HRESULT ParseFrame( LPDIRECTXFILEDATA pFileData, CD3DFileFrame* pParentFrame )
{
    DWORD dwNameLen=80;
    CHAR  strName[80];
    if( FAILED( pFileData->GetName( strName, &dwNameLen ) ) )
        return E_FAIL;

    CD3DFileFrame* pFrame = new CD3DFileFrame( strName );

    // Enumerate child objects.
    LPDIRECTXFILEOBJECT pChildObj;
    while( SUCCEEDED( pFileData->GetNextObject( &pChildObj ) ) )
    {
        LPDIRECTXFILEDATA pChildData;
        if( SUCCEEDED( pChildObj->QueryInterface( IID_IDirectXFileData,
                            (VOID**)&pChildData ) ) )
        {
            const GUID* pGUID;
            pChildData->GetType( &pGUID );

            if( TID_D3DRMFrame == *pGUID )
                ParseFrame( pChildData, pFrame );

            if( TID_D3DRMMesh == *pGUID )
                ParseMesh( pChildData, pFrame );

            if( TID_D3DRMFrameTransformMatrix == *pGUID )
            {
                DWORD dwSize;
                VOID* pData;
                if( FAILED( pChildData->GetData(NULL, &dwSize, (VOID**)&pData) ) )
                {
                    delete pFrame;
                    return NULL;
                }
                if( dwSize == sizeof(D3DMATRIX) )
                    pFrame->SetMatrix( (D3DMATRIX*)pData );
            }

            pChildData->Release();
        }

        pChildObj->Release();
    }

    pParentFrame->AddChild( pFrame );
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: LoadFile()
// Desc: Loads the specified the .X file, and returns a pointer to the root
//       object in the file.
//-----------------------------------------------------------------------------
static CD3DFileBaseObject* LoadXFile( CHAR* strFilename )
{
    HRESULT                 hr;
    LPDIRECTXFILE           pDXFile;
    LPDIRECTXFILEENUMOBJECT pEnumObj;
    LPDIRECTXFILEDATA       pFileData;
    const GUID*             pGUID;
    CD3DFileFrame*          pRootFrame = NULL;
    CD3DFileFrame*          pFrame = NULL;

    // Create the file object, and register the D3DRM templates for .X files
    if( FAILED( DirectXFileCreate( &pDXFile ) ) )
        return NULL;
    if( FAILED( pDXFile->RegisterTemplates( (VOID*)D3DRM_XTEMPLATES,
                            D3DRM_XTEMPLATE_BYTES ) ) )
    {
        pDXFile->Release();
        return NULL;
    }

    // Create an enumerator object, to enumerate through the .X file objects
    if( SUCCEEDED( hr = pDXFile->CreateEnumObject( strFilename,
                           DXFILELOAD_FROMFILE, &pEnumObj ) ) )
    {
        pRootFrame = new CD3DFileFrame("root");

        // Cycle through each object. Parse meshes and frames as appropriate
        while( SUCCEEDED( hr = pEnumObj->GetNextDataObject( &pFileData ) ) )
        {
            pFileData->GetType( &pGUID );

            if( *pGUID == TID_D3DRMFrame )
                ParseFrame( pFileData, pRootFrame );

            if( *pGUID == TID_D3DRMMesh )
                ParseMesh( pFileData, pRootFrame );
     
            pFileData->Release();
        }

        // If an error occurred, scrap the whole thing
        if( DXFILEERR_NOMOREOBJECTS != hr )
            SAFE_DELETE( pRootFrame );

        pEnumObj->Release();
    }

    pDXFile->Release();
    return pRootFrame;
}




//-----------------------------------------------------------------------------
// Name: CD3DFileObject()
// Desc: Class constructor
//-----------------------------------------------------------------------------
CD3DFileObject::CD3DFileObject()
{
    m_pData = NULL;
}




//-----------------------------------------------------------------------------
// Name: ~CD3DFileObject()
// Desc: Class destructor
//-----------------------------------------------------------------------------
CD3DFileObject::~CD3DFileObject()
{
    SAFE_DELETE( m_pData );
}




//-----------------------------------------------------------------------------
// Name: Load()
// Desc: Loads a .X geometry file, and creates a hierarchy of frames and meshes
//       to represent the geometry in that file.
//-----------------------------------------------------------------------------
HRESULT CD3DFileObject::Load( CHAR* strFilename )
{
    if( m_pData )
        return E_FAIL;

    if( NULL == ( m_pData = LoadXFile( strFilename ) ) )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetBoundingSphere()
// Desc: Computes the bounding sphere for all the meshes in the file object.
//-----------------------------------------------------------------------------
HRESULT CD3DFileObject::GetBoundingSphere( D3DVECTOR* pvCenterPt, FLOAT* pfRadius )
{
    (*pvCenterPt) = D3DVECTOR(0,0,0);
    (*pfRadius)   = -1.0f;

    if( m_pData )
    {
        D3DMATRIX mat;
        DWORD     dwNumPoints;

        D3DUtil_SetIdentityMatrix( mat );
        m_pData->ComputeBoundingSphereCenter( &mat, pvCenterPt, &dwNumPoints );
        (*pvCenterPt) = (*pvCenterPt) / (FLOAT)dwNumPoints;

        D3DUtil_SetIdentityMatrix( mat );
        m_pData->ComputeBoundingSphereRadius( &mat, pvCenterPt, pfRadius );
    }

    if( *pfRadius <= 0.0f )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetMeshVertices()
// Desc: Traverse the hierarchy of frames and meshes that make up the file
//       object, and retrieves the matrix ptr for the specified frame.
//-----------------------------------------------------------------------------
HRESULT CD3DFileObject::GetFrameMatrix( CHAR* strName, D3DMATRIX** ppMatrix )
{
    (*ppMatrix) = NULL;

    if( m_pData )
        m_pData->GetFrameMatrix( strName, ppMatrix );
    
    if( NULL == (*ppMatrix) )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetMeshVertices()
// Desc: Traverse the hierarchy of frames and meshes that make up the file
//       object, and retrieves the vertices for the specified mesh.
//-----------------------------------------------------------------------------
HRESULT CD3DFileObject::GetMeshVertices( CHAR* strName, D3DVERTEX** ppvVertices,
                                         DWORD* pdwNumVertices, WORD** ppwIndices, 
                                         DWORD* pdwNumIndices )
{
    (*ppvVertices)    = NULL;
    (*pdwNumVertices) = NULL;
    (*ppwIndices)     = NULL;
    (*pdwNumIndices)  = NULL;

    if( m_pData )
        m_pData->GetMeshVertices( strName, ppvVertices, pdwNumVertices,
                                  ppwIndices, pdwNumIndices );

    if( NULL == (*ppvVertices) )
        return E_FAIL;

    return S_OK;
}



    
//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the hierarchy of frames and meshes that make up the file
//       object
//-----------------------------------------------------------------------------
HRESULT CD3DFileObject::Render( LPDIRECT3DDEVICE3 pd3dDevice,
                                LPDIRECT3DMATERIAL3 pmtrl )
{
    LPDIRECT3DTEXTURE2 ptexSaved;
    D3DMATRIX          matSaved;
    D3DMATERIAL        mtrlSaved;
    mtrlSaved.dwSize = sizeof( mtrlSaved );
    
    if( m_pData )
    {
        // State render states that will be overwritten
        pmtrl->GetMaterial( &mtrlSaved );
        pd3dDevice->GetTexture( 0, &ptexSaved );
        pd3dDevice->GetTransform( D3DTRANSFORMSTATE_WORLD, &matSaved );

        // Render the file object's hierarchy of frames and meshes
        m_pData->Render( pd3dDevice, pmtrl );

        // Restore the render states
        pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matSaved );
        pd3dDevice->SetTexture( 0, ptexSaved );
        pmtrl->SetMaterial( &mtrlSaved );
    }

    return S_OK;
}




