//-----------------------------------------------------------------------------
// 
// Sample Name: Switcher Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  This sample shows how to switch between the normal and exclusive cooperative 
  levels in DirectDraw.

Path
====
  Source: Mssdk\Samples\Multimedia\DDraw\Src\Switcher

  Executable: Mssdk\Samples\Multimedia\DDraw\Bin

User's Guide
============
  Press ALT+ENTER to switch between full-screen and windowed mode. Quit the 
  program by pressing ESC.

Programming Notes
=================
  In normal (windowed) mode, the sample assigns a clipper, shows the mouse 
  cursor, and handles window moves, WM_PAINT messages, and pausing caused by 
  losing focus to other applications. In exclusive (full-screen) mode it uses 
  page flipping rather than blitting to update the scene.

