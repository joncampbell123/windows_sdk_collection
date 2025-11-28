//-----------------------------------------------------------------------------
// 
// Sample Name: Mosquito Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  This program demonstrates DirectDraw animation using overlays.

Path
====
  Source: Mssdk\Samples\Multimedia\DDraw\Src\Mosquito

  Executable: Mssdk\Samples\Multimedia\DDraw\Bin

User's Guide
============
  To run the Mosquito application, you must have a display adapter that 
  supports overlays. On a computer with overlay support, the program creates a 
  large mosquito that flies around the screen. If your display adapter card 
  doesn't support source color keying for overlays, you'll see an ugly, black, 
  rectangular background around the mosquito.

  Some cards have better overlay support in certain resolutions that others. If 
  you know your card has overlay support through DirectDraw, but the Mosquito 
  program is having problems creating or displaying the overlay, try switching 
  to a lower screen resolution or color depth and restarting the application.

Programming Notes
=================
  The program creates a complex overlay surface and animates by flipping.

