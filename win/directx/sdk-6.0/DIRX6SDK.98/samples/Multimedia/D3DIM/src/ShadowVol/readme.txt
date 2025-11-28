//-----------------------------------------------------------------------------
// 
// Sample Name: ShadowVol Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The ShadowVol sample demonstrates how to create and use stencil buffers to 
  implement shadow volumes, which are used to cast shadows on arbitrarily 
  complex objects.

Path
====
  Source: Mssdk\Samples\Multimedia\D3dim\Src\ShadowVol

  Executable: Mssdk\Samples\Multimedia\D3dim\Bin

User's Guide
============
  Press F1 to see available commands.

Programming Notes
=================
  Shadow volumes are a fairly advanced technique. To start, take an object that 
  you'd like to have cast a shadow. From that object, build a set of polygonal 
  faces that encompass the volume of its shadow. Next, use the stencil buffer 
  to render the front facing planes of the shadow volume. Then, set up the 
  stencil buffer to render the back-facing planes, this time subtracting values 
  from the stencil buffer. Afterwards, the stencil buffer contains a mask of 
  the cast shadow. Just draw a large gray or black rectangle using the stencil 
  buffer as a mask, and the frame buffer will get updated with the shadow.

