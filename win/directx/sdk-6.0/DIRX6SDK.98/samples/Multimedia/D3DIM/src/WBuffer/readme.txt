//-----------------------------------------------------------------------------
// 
// Sample Name: WBuffer Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The WBuffer sample shows how to use w-buffering.

Path
====
  Source: Mssdk\Samples\Multimedia\D3dim\Src\WBuffer

  Executable: Mssdk\Samples\Multimedia\D3dim\Bin

User's Guide
============
  Press F1 to see a list of the usual commands. In addition, you can change the 
  buffering mechanism by pressing W (w-buffering, if supported), Z 
  (z-buffering), or N (no buffering). Note the artifacts that appear when using 
  z-buffering.

Programming Notes
=================
  W-buffering is a depth-buffering alternative to z-buffering, and should be 
  used in cases where z-buffering produces artifacts. W-buffering does a much 
  better job of quantizing the depth buffer.

