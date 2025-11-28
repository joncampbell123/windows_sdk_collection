//-----------------------------------------------------------------------------
// 
// Sample Name: Spheremap Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  This samples loads a 3-D object and renders it using a spheremap.  

Path
====
  Source: Mssdk\Samples\Multimedia\D3dim\Src\Spheremap

  Executable: Mssdk\Samples\Multimedia\D3dim\Bin

User's Guide
============
  Press F1 to see available commands.

Programming Notes
=================
  The spheremap itself is a special, preconstructed texture map containing a 
  180-degree view of an environment. Before a frame is rendered, the object's 
  normals are used to compute the texture coordinates for each vertex of the 
  object. When rendered, the object looks as if it reflects the environment.

