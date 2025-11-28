//-----------------------------------------------------------------------------
// 
// Sample Name: BumpMap Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The BumpMap program demonstrates the bump mapping capabilities of Direct3D. 
  Bump mapping is a texture blending technique used to render the appearance of 
  rough surfaces.

Path
====
  Source: Mssdk\Samples\Multimedia\D3dim\Src\Bumpmap

  Executable: Mssdk\Samples\Multimedia\D3dim\Bin

User's Guide
============
  Press F1 to see available commands.

  Your graphics hardware might not support bump mapping, in which case Direct3D 
  displays a message to that effect when you attempt to run this program. The 
  solution is to enable the reference rasterizer. You can do so by running 
  Mssdk\Samples\Multimedia\D3dim\Bin\Enablerefrast.reg.

Programming Notes
=================
  Bump mapping is an advanced multitexture blending technique that can be used 
  to render the appearance of rough surfaces. The bump map itself is a texture 
  that stores the perturbation data.

  In this sample program, the map of the world is a texture. The program blends 
  both the map texture and the bump map texture onto the sphere to give the 
  appearance of a high-resolution topographical map. 

  For more details on this technique, see Bump Mapping.

  This sample was built using the Direct3D sample framework. 

