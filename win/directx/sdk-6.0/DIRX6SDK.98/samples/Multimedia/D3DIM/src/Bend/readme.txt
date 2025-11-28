//-----------------------------------------------------------------------------
// 
// Sample Name: Bend Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The Bend sample demonstrates a technique called surface skinning. It displays 
  3-D object which rotates about the y-axis and appears to bend. 

Path
====
  Source: Mssdk\Samples\Multimedia\D3dim\Src\Bend

  Executable: Mssdk\Samples\Multimedia\D3dim\Bin

User's Guide
============
  Press F1 to see available commands.

Programming Notes
=================
  The sample achieves the surface skinning effect by using two static copies of 
  the object, one of which is oscillating along an axis. Each frame, the 
  vertices of the two objects are merged and blended into a third object. The 
  sample program displays the object derived from the third set of vertices.

  This sample was built using the Direct3D sample framework. 

