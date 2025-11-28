//-----------------------------------------------------------------------------
// File: XFileLoader.h
//
// Desc: Support code for loading DirectX .X files.
//
//
// Copyright (c) 1997-1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XFILELOADER_H
#define XFILELOADER_H


// Internal class used by the CD3DFileObject to hold data for a geometry file
class CD3DFileBaseObject;




//-----------------------------------------------------------------------------
// Name: class CD3DFileObject
// Desc: 
//-----------------------------------------------------------------------------
class CD3DFileObject
{
    CD3DFileBaseObject* m_pData;

public:
    HRESULT GetBoundingSphere( D3DVECTOR* pvCenterPt, FLOAT* pfRadius );
    HRESULT GetFrameMatrix( CHAR* strName, D3DMATRIX** ppmat );
    HRESULT GetMeshVertices( CHAR* strName, D3DVERTEX** ppVertices, 
                             DWORD* pdwNumVertices, WORD** ppwIndices, 
                             DWORD* pdwNumIndices );

    HRESULT Load( CHAR* strFilename );
    HRESULT Render( LPDIRECT3DDEVICE3, LPDIRECT3DMATERIAL3 );

    CD3DFileObject();
    ~CD3DFileObject();
};




#endif // XFILELOADER_H




