//-----------------------------------------------------------------------------
// File: XFile.h
//
// Desc: Support code for loading DirectX .X files.
//
//       Note: This code uses the D3D Framework helper library.
//
//
// Copyright (c) 1997-1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XFILE_H
#define XFILE_H



//-----------------------------------------------------------------------------
// Name: class CDXFileObject
// Desc: 
//-----------------------------------------------------------------------------
class CDXFileObject
{
public:
	CDXFileObject*   m_pNext;
	CDXFileObject*   m_pChild;
	
	VOID AddNext( CDXFileObject* );
	VOID AddChild( CDXFileObject* );
	
	CDXFileObject();
	~CDXFileObject();
};




//-----------------------------------------------------------------------------
// Name: class CDXFileFrame
// Desc: 
//-----------------------------------------------------------------------------
class CDXFileFrame : public CDXFileObject
{
public:
	D3DMATRIX m_mat;
};




//-----------------------------------------------------------------------------
// Name: class CDXFileMesh
// Desc: 
//-----------------------------------------------------------------------------
class CDXFileMesh : public CDXFileObject
{
public:
	DWORD       m_dwNumVertices;
	D3DVERTEX*  m_pVertices;
	DWORD       m_dwNumIndices;
	WORD*       m_pIndices;
	BOOL        m_bHasNormals;

	CHAR        m_strTexture[80];
	D3DMATERIAL m_mtrl;

	CDXFileMesh( DWORD, VOID*, DWORD, VOID* );
	~CDXFileMesh();
};




#endif // XFILE_H

