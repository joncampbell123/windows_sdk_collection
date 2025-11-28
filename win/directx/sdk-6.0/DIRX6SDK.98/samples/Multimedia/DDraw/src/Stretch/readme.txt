//-----------------------------------------------------------------------------
// 
// Sample Name: Stretch Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The Stretch sample application program illustrates stretching and clipping 
  while blitting a bitmap image.

Path
====
  Source: Mssdk\Samples\Multimedia\DDraw\Src\Stretch

  Executable: Mssdk\Samples\Multimedia\DDraw\Bin

User's Guide
============
  Stretch must be run in a video mode that uses 8 bits per pixel. It will not 
  work properly in other video modes.

  The program displays a red torus moving in its client window. Control the 
  rotational speed with the Stop, Slow, and Fast options in the Rotation menu. 
  Alter the size of the window by selecting items from the Size menu, or by 
  resizing the window with the mouse.

Programming Notes
=================
  Any time you resize the Stretch program window to a size other than 1x1, you 
  are using the image stretching capabilities of the DirectDraw blitting 
  methods.

  The clipper for the primary surface is set to the client window. To 
  demonstrate clipping, partially overlap another window over the Stretch 
  program's window. When Stretch blits the bitmap, the portion of the bitmap 
  that would fall within the other window is clipped.

