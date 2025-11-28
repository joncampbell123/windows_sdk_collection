//-----------------------------------------------------------------------------
// 
// Sample Name: XFile Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  This sample shows how to load and render .x files. 

Path
====
  Source: Mssdk\Samples\Multimedia\D3dim\Src\Xfile

  Executable: Mssdk\Samples\Multimedia\D3dim\Bin

User's Guide
============
  When you run the program, an Open File dialog box appears. You can find some 
  .x files in the media folder of the D3DIM samples directory. If there is a 
  .bmp file of the same name in the directory, the bmp is automatically used as 
  a texture for the object.

  Press F1 to see available commands. In addition, you can load a different 
  file from the File menu.

Programming Notes
=================
  If a texture appears upside-down on an object you have loaded, it is because 
  of the way the object loads the texture. An earlier version of Direct3D 
  loaded bitmaps upside-down, so the creators of some .x files compensated by 
  reversing the coordinates.

   

  

