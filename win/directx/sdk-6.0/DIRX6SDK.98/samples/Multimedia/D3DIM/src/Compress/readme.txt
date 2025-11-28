//-----------------------------------------------------------------------------
// 
// Sample Name: Compress Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The Compress sample demonstrates how to load the DDS file format into a 
  compressed texture surface. DDS textures can be created using the DxTex 
  program included with the DirectX SDK.  

Path
====
  Source: Mssdk\Samples\Multimedia\D3dim\Src\Compress

  Executable: Mssdk\Samples\Multimedia\D3dim\Bin

User's Guide 
=============
  Press F1 to see available commands.

Programming Notes
=================
  The file format is called DDS because it encapsulates the information in a 
  DirectDrawSurface. The data can be read directly into a surface of a matching 
  format.

  The ReadDDSTexture function demonstrations how a DDS surface is read from a 
  file.

  A DDS file has the following format:

  DWORD dwMagic	(0x20534444, or "DDS ")

  DDSURFACEDESC2 ddsd	Information about the surface format

  BYTE bData1[]	Data for the main surface

  [BYTE bData2[]	Data for attached surfaces, if any, follow

  

  This format is easy to read and write, and supports features such as alpha 
  and multiple mip levels, as well as DXTn compression. If it uses DXTn 
  compression, it may be one of 5 compressed types. See Compressed Texture 
  Surfaces.

  After the texture is read in, a pixel format must be chosen that is supported 
  by the renderer. In the sample, the supported pixel formats are enumerated 
  and stored in a linked list. After the pixel formats are collected, the list 
  is searched for a best match, using the FindBestPixelFormatMatch function.

  Some Direct3D devices such as the reference rasterizer and some hardware 
  devices can render compressed textures directly. But for renderers that don't 
  directly support this, the compressed surface must be blitted to a 
  non-compressed surface. The function BltToUncompressedSurface demonstrates 
  how this is done.

