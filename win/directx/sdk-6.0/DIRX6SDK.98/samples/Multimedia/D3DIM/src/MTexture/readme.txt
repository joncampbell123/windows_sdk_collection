//-----------------------------------------------------------------------------
// 
// Sample Name: MTexture Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The MTexture program shows how to use multitexturing. The scene consists of a 
  room with walls that have a base texture and a spotlight texture, each using 
  a different set of texture coordinates.

Path
====
  Source: Mssdk\Samples\Multimedia\D3dim\Src\MTexture

  Executable: Mssdk\Samples\Multimedia\D3dim\Bin

User's Guide
============
  Press F1 to see available commands.

Programming Notes
=================
  This sample shows how to program the multitexture stages using the new 
  IDirect3DDevice3::SetTextureStageState method. Dozens of different effects 
  are attainable with this method. The sample just shows one, monochrome light 
  mapping, which is very popular in current game titles.

