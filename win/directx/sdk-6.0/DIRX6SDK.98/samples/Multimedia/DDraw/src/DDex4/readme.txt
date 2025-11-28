//-----------------------------------------------------------------------------
// 
// Sample Name: DDEx4 Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The DDEx4 program is an extension of DDEx3. It demonstrates a simple 
  animation technique.

Path
====
  Source: Mssdk\Samples\Multimedia\DDraw\Src\Ddex4

  Executable: Mssdk\Samples\Multimedia\DDraw\Bin

User's Guide
============
  DDEx4 requires no user input. Press F12 or ESC to quit the program.

  This program requires at least 1.2 MB of video RAM.

Programming Notes
=================
  Unlike DDEx3, the DDEx4 program creates only one off-screen surface. It loads 
  a bitmap containing a series of animation images onto this surface. To create 
  the animation, it blits portions of the off-screen surface to the back 
  surface, then flips the front and back surfaces.

  The blitting routines illustrate the use of a source color key to create a 
  sprite with a transparent background.

