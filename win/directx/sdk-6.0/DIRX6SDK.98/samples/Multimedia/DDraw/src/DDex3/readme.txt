//-----------------------------------------------------------------------------
// 
// Sample Name: DDEx3 Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The DDEx3 program is an extension of DDEx2. This example demonstrates the use 
  of off-screen surfaces.

Path
====
  Source: Mssdk\Samples\Multimedia\DDraw\Src\Ddex3

  Executable: Mssdk\Samples\Multimedia\DDraw\Bin

User's Guide
============
  DDEx3 requires no user input. Press F12 or ESC to quit the program.

  The program requires at least 1.2 MB of video RAM.

Programming Notes
=================
  In addition to the front and back surfaces, the program creates two 
  off-screen surfaces and loads bitmaps into them. It calls the 
  IDirectDrawSurface4::BltFast method to copy the contents of an off-screen 
  surface to the back surface, alternating the source surface on each frame. 
  After it blits the bitmap to the back surface, DDEx3 flips the front and back 
  surfaces. 

