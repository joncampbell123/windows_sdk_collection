//-----------------------------------------------------------------------------
// File: XFile.cpp
//
// Desc: Support code for loading .X files. Note: this code has been simplified
//       and should not be used to load in any ol' X files, but just ones that
//       have only one mesh in them.
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
#include <rmxfguid.h>
#include <rmxftmpl.h>
#include <dxfile.h>
#include "XFile.h"


// The first mesh read in from the xfile.
CDXFileMesh* g_pFirstMesh = NULL;




//-----------------------------------------------------------------------------
// Name: class CDXFileObject
// Desc: The following functions implement the CDXFileObject class
//-----------------------------------------------------------------------------




CDXFileObject::CDXFileObject()
{
	m_pChild = m_pNext = NULL;
}




CDXFileObject::~CDXFileObject()
{
	if( m_pChild ) delete m_pChild;
	if( m_pNext )  delete m_pNext;
}




VOID CDXFileObject::AddChild( CDXFileObject* pChild )
{
	if( m_pChild )
		m_pChild->AddNext( pChild );
	else
		m_pChild = pChild;
}




VOID CDXFileObject::AddNext( CDXFileObject* pNext )
{
	if( m_pNext )
		m_pNext->AddNext( pNext );
	else
		m_pNext = pNext;
}




//-----------------------------------------------------------------------------
// Name: class CDXFileMesh
// Desc: The following functions implement the CDXFileMesh class
//-----------------------------------------------------------------------------




CDXFileMesh::CDXFileMesh( DWORD dwNumVertices, VOID* pVertexData, DWORD dwNumFaces,
						 VOID* pFaceData ) : CDXFileObject()
{
	D3DUtil_InitMaterial( m_mtrl, 1.0f, 1.0f, 1.0f );
	m_bHasNormals   = FALSE;
	m_strTexture[0] = '\0';
	m_dwNumVertices = dwNumVertices;
	m_pVertices     = new D3DVERTEX[m_dwNumVertices];

	for( DWORD i=0; i< m_dwNumVertices; i++ )
	{
		ZeroMemory( &m_pVertices[i], sizeof(D3DVERTEX) );
		m_pVertices[i].x = ((FLOAT*)pVertexData)[i*3+0];
		m_pVertices[i].y = ((FLOAT*)pVertexData)[i*3+1];
		m_pVertices[i].z = ((FLOAT*)pVertexData)[i*3+2];
	}

	// Count the number of indices
	m_dwNumIndices   = 0L;
	LONG pAddr = (LONG)pFaceData;
	for( i=0; i<dwNumFaces; i++ )
	{
		DWORD dwNumVerticesPerFace = *((DWORD*)pAddr);
		
		pAddr += 4;
		pAddr += 4*dwNumVerticesPerFace;

		m_dwNumIndices += 3*(dwNumVerticesPerFace-2);
	}

	m_pIndices = new WORD[m_dwNumIndices];

	pAddr = (LONG)pFaceData;
	WORD* pIndices = m_pIndices;

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




CDXFileMesh::~CDXFileMesh()
{
	D3DTextr_DestroyTexture( m_strTexture );

	if( m_pVertices ) delete m_pVertices;
	if( m_pIndices )  delete m_pIndices;
}




//-----------------------------------------------------------------------------
// Name: ParseXXXX()
// Desc: The following routines implement the DirectX .X file loader.
//-----------------------------------------------------------------------------




HRESULT ParseMaterial( LPDIRECTXFILEDATA pDXFileData, CDXFileMesh* pMesh )
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
	memcpy( &pMesh->m_mtrl, &mtrl, sizeof(D3DMATERIAL) );

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

				strcpy( pMesh->m_strTexture, *string );
				D3DTextr_CreateTexture( pMesh->m_strTexture );
			}

			pDXChildData->Release();
		}

		pDXChildObj->Release();
	}
	return S_OK;
}




HRESULT ParseMeshMaterialList( LPDIRECTXFILEDATA pDXFileData, CDXFileMesh* pMesh )
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




HRESULT ParseMesh( LPDIRECTXFILEDATA pFileData, CDXFileFrame* pParentFrame )
{
	// Read the Mesh data from the file
    LONG  pData;
    DWORD dwSize;
    if( FAILED( pFileData->GetData(NULL, &dwSize, (VOID**)&pData) ) )
		return E_FAIL;

	DWORD dwNumVertices = *((DWORD*)pData);    pData += 4;
	VOID* pVertices     = ((D3DVECTOR*)pData); pData += 12*dwNumVertices;
	DWORD dwNumFaces    = *((DWORD*)pData);    pData += 4;
	VOID* pFaceData     = (VOID*)pData;

	// Create the Mesh object
	CDXFileMesh* pMesh = new CDXFileMesh( dwNumVertices, pVertices, dwNumFaces, pFaceData );


	g_pFirstMesh = pMesh;

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

				if( dwNumNormals == pMesh->m_dwNumVertices )
				{
					for( DWORD i=0; i<dwNumNormals; i++ )
					{
						pMesh->m_pVertices[i].nx = pNormals[i].x;
						pMesh->m_pVertices[i].ny = pNormals[i].y;
						pMesh->m_pVertices[i].nz = pNormals[i].z;
					}

					pMesh->m_bHasNormals = TRUE;
				}
			}
			
			if( TID_D3DRMMeshTextureCoords == *pGUID )
			{
				// Copy the texture coords into the mesh's vertices
				DWORD  dwNumTexCoords = *((DWORD*)pData);
				FLOAT* pTexCoords     = (FLOAT*)(pData+4);

				if( dwNumTexCoords == pMesh->m_dwNumVertices )
				{
					for( DWORD i=0; i<dwNumTexCoords; i++ )
					{
						pMesh->m_pVertices[i].tu = *pTexCoords++;
						pMesh->m_pVertices[i].tv = -*pTexCoords++;
					}
				}
			}

			pChildData->Release();
		}

	pChildObj->Release();
	}

	pParentFrame->AddChild( pMesh );
	return S_OK;
}




HRESULT ParseFrame( LPDIRECTXFILEDATA pFileData, CDXFileFrame* pParentFrame )
{
	CDXFileFrame* pFrame = new CDXFileFrame();
	D3DUtil_SetIdentityMatrix( pFrame->m_mat );

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
					memcpy( &pFrame->m_mat, pData, sizeof(D3DMATRIX) );
			}

			pChildData->Release();
		}

	pChildObj->Release();
	}

	pParentFrame->AddChild( pFrame );
	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: LoadMeshFromXFile()
// Desc: Loads the specified the .X file, and returns a pointer to the root
//       object in the file.
//-----------------------------------------------------------------------------
HRESULT LoadMeshFromXFile( CHAR* strFilename, D3DVERTEX** ppVertices,
				  DWORD* pdwNumVertices, WORD** ppIndices,
				  DWORD* pdwNumIndices )
{
	CHAR strPathname[256];

	// Look for file
	FILE* file = fopen( strFilename, "rb" );

	if( file )
	{
		strcpy( strPathname, strFilename );
		fclose( file );
	}
	else
	{
		HKEY  key;
		DWORD type, size=512;

		// Get the media path from the registry. Then, look in the D3DIM\media
		// directory.
		if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, 
								TEXT("Software\\Microsoft\\DirectX"),
								0, KEY_READ, &key ) )
		{
			if( ERROR_SUCCESS == RegQueryValueEx( key,
							TEXT("DX6SDK Samples Path"), NULL,
										&type, (BYTE*)strPathname, &size ) )
			{
				strcat( strPathname, "\\D3DIM\\Media\\" );
				strcat( strPathname, strFilename );
			}
			RegCloseKey( key );
		}
	}
	
	HRESULT                 hr;
	LPDIRECTXFILE           pDXFile;
	LPDIRECTXFILEENUMOBJECT pEnumObj;
	LPDIRECTXFILEDATA       pFileData;
	const GUID*             pGUID;
	CDXFileFrame*           pRootFrame = NULL;
	CDXFileFrame*           pFrame = NULL;

	// Create the file object, and register the D3DRM templates for .X files
	if( FAILED( DirectXFileCreate( &pDXFile ) ) )
		return E_FAIL;
	if( FAILED( pDXFile->RegisterTemplates( (VOID*)D3DRM_XTEMPLATES,
						    D3DRM_XTEMPLATE_BYTES ) ) )
	{
		pDXFile->Release();
		return E_FAIL;
	}

	// Create an enumerator object, to enumerate through the .X file objects
	if( FAILED( hr = pDXFile->CreateEnumObject( strPathname,
					       DXFILELOAD_FROMFILE, &pEnumObj ) ) )
	{
		pDXFile->Release();
		return E_FAIL;
	}

	// Create a root frame for all objects in the .X file
	pRootFrame = new CDXFileFrame();
	D3DUtil_SetIdentityMatrix( pRootFrame->m_mat );

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
	pDXFile->Release();

	// Record the first mesh in the file, so the rest of the sample can access
	// it. Note: a full .X file reader would provide a mechanism to access all
	// objects in the .X file. See the sample called xfile for that purpose.
	if( g_pFirstMesh )
	{
		(*ppVertices)     = g_pFirstMesh->m_pVertices;
		(*pdwNumVertices) = g_pFirstMesh->m_dwNumVertices;
		(*ppIndices)      = g_pFirstMesh->m_pIndices;
		(*pdwNumIndices)  = g_pFirstMesh->m_dwNumIndices;
	}

	return S_OK;
}





