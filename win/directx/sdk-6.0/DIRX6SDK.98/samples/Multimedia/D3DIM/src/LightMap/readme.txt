//-----------------------------------------------------------------------------
// 
// Sample Name: LightMap Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  This sample shows how to use multitexturing and multipass techniques to do 
  some complex lighting effects. There is a light swinging in a room, which 
  dynamically lights up the walls and ceiling as the light moves.

Path
====
  Source: Mssdk\Samples\Multimedia\D3dim\Src\LightMap

  Executable: Mssdk\Samples\Multimedia\D3dim\Bin

User's Guide
============
  On the Options menu, choose between multipass and multiple texture blending. 
  The latter will not be available if your hardware does not support it.

  Press F1 to see other available commands. 

Programming Notes
=================
  There is no "true" lighting in this sample-everything is done with light 
  maps. Light maps are extremely popular in games theses days, because they are 
  much faster than real lighting. Also, real lighting is calculated only at the 
  vertices, so highly tesselated meshes are required.

