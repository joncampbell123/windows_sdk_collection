//-----------------------------------------------------------------------------
// 
// Sample Name: Donut Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The Donut program uses DirectDraw to display an animated sprite directly on 
  the screen. In non-exclusive mode, the sprite appears on top of the desktop 
  and any windows. In exclusive mode, it appears on an otherwise blank screen.

Path
====
  Source: Mssdk\Samples\Multimedia\DDraw\Src\Donut

  Executable: Mssdk\Samples\Multimedia\DDraw\Bin

User's Guide
============
  The Donut application requires no user input. Press F12 or ESC to quit. Note 
  that because the program does not run in a window, you may have to switch to 
  it by using the taskbar or ALT+TAB before you can close it. 

  You can specify various command line switches to modify the operational 
  characteristics of this program. Each command line switch consists of one 
  character, and need not be preceded with a hyphen or slash. Alphabetical 
  characters must be capitals. The switches are as follows:

  0	Default. Display the donut in the left position. 

  1	Display the donut in the middle position.

  2	Display the donut in the right position. 

  X	Use exclusive mode. The default is non-exclusive mode.

  A	Switch to 640x480x8 resolution and use exclusive mode.

  B	Switch to 800x600x8 resolution and use exclusive mode.

  C	Switch to 1024x768x8 resolution and use exclusive mode.

  D	Switch to 1280x1024x8 resolution and use exclusive mode.

  

  The switches can be combined. If you specify two or more command line 
  switches that contradict each other, the last switch is used. 

  If you run the program in non-exclusive mode, it attempts to continue to run 
  even when it loses focus. If you run it in exclusive mode, it does not 
  attempt to modify the screen when it doesn't have focus.

Programming Notes
=================
  The Donut program creates a primary DirectDraw surface and two off-screen 
  surfaces. Animation images are blitted from the off-screen surfaces directly 
  to the primary surface during each frame.

  The program demonstrates how DirectX applications can set the video mode 
  based on user input. It is also useful for testing multiple exclusive mode 
  applications interacting with multiple non-exclusive mode applications.

