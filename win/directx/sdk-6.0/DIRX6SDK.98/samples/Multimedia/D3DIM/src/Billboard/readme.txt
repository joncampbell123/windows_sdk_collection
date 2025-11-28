//-----------------------------------------------------------------------------
// 
// Sample Name: Billboard Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The Billboard sample illustrates the billboarding technique. Billboarding is 
  a way of making 2-D sprites appear to be 3-D. It can also be used for smoke, 
  clouds, vapor trails, energy blasts and more. For more information, see 
  Common Techniques and Special Effects.

Path
====
  Source: Mssdk\Samples\Multimedia\D3dim\Src\Billboard

  Executable: Mssdk\Samples\Multimedia\D3dim\Bin

User's Guide
============
  Press F1 to see available commands.

Programming Notes
=================
  The sample displays a grassy field with trees in it. The trees look like 3-D 
  objects. However, they are actually 2-D texture bitmaps that are blended onto 
  invisible rectangular polygons. 

  As the sample program executes, the viewpoint changes. Each time it does, all 
  of the billboard polygons that the trees are painted onto are rotated so they 
  face the viewer. The program then blends the images of the trees onto the 
  billboard polygons. The trees appear to be 3-D because they can be viewed 
  from all angles. However, close inspection reveals that the trees have 
  exactly the same appearance from all angles. For many applications, users 
  will not notice this minor drawback.

  The shadows are also 2-D textures.

  This sample was built using the Direct3D sample framework. 

