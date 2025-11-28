//-----------------------------------------------------------------------------
// 
// Sample Name: Stretch2 Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  Stretch2 is an extension of the Stretch program. In addition to the 
  capabilities of the Stretch example, Stretch2 illustrates how an application 
  can use DirectDraw on multiple monitors.

Path
====
  Source: Mssdk\Samples\Multimedia\DDraw\Src\Stretch2

  Executable: Mssdk\Samples\Multimedia\DDraw\Bin

User's Guide
============
  Resize the window to see the bitmap stretch. If you have more than one 
  monitor attached to your computer, the window can be dragged from monitor to 
  monitor.

Programming Notes
=================
  Look at how the application handles WM_MOVE to detect when the window moves 
  monitors. Also note how it converts from window client coordinates to device 
  coordinates.

  Multimon.h (in the Mssdk\Include directory) contains stub functions that 
  enable the program to run on Windows 95 or Windows NT 4.0.

