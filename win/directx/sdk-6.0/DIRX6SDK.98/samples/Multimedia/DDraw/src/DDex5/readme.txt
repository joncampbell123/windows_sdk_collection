//-----------------------------------------------------------------------------
// 
// Sample Name: DDEx5 Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The DDEx5 program is an extension of DDEx4. It demonstrates a simple palette 
  manipulation.

Path
====
  Source: Mssdk\Samples\Multimedia\DDraw\Src\Ddex5

  Executable: Mssdk\Samples\Multimedia\DDraw\Bin

User's Guide
============
  DDEx4 requires no user input. Press F12 or ESC to quit the program.

  This program requires at least 1.2 MB of video RAM.

Programming Notes
=================
  The program uses IDirectDrawPalette::GetEntries to read a palette, modifies 
  the entires, and then uses IDirectDrawPalette::SetEntries to update the 
  palette.

