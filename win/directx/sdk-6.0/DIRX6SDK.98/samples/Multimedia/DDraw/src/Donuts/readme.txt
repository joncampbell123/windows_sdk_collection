//-----------------------------------------------------------------------------
// 
// Sample Name: Space Donuts Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  This simple game shows how to combine DirectDraw, DirectInput, and 
  DirectSound. Although it demonstrates other DirectX components, it is 
  primarily intended to show how to animate multiple sprites.

Path
====
  Source: Mssdk\Samples\Multimedia\DDraw\Src\Donuts

  Executable: Mssdk\Samples\Multimedia\DDraw\Bin

User's Guide
============
  Input is from the keyboard by default, but you can select a joystick from the 
  Game menu.

  The commands are listed on the opening screen. All numbers must be entered 
  from the numeric keypad. "Joy" refers to a joystick button.

  Key	Command

  

  ESC, F12	Quit

  4	Turn left

  6	Turn right

  5 (Joy 3)	Stop

  8	Accelerate forward

  2	Accelerate backward

  7 (Joy 2)	Shield

  SPACEBAR (Joy 1)	Fire

  ENTER	Start game

  F1	Toggle trailing afterimage effect on/off

  F3	Turn audio on/off

  F5	Toggle frame rate display on/off

  F10	Main menu

  

  Space Donuts defaults to 640x480 at 256 colors. You can specify a different 
  resolution and pixel depth on the command line. 

  The game uses the following command line switches, which are case-sensitive:

  e	Use software emulation, not hardware acceleration

  t	Test mode, no input required

  x	Stress mode. Never stop if you can help it

  S	Turn sound off/on

  

  These switches may be followed by three option numbers representing 
  x-resolution, y-resolution, and bits per pixel. For example:
  donuts -S 800 600 16

   

Programming Notes
=================
  This game demonstrates many of the features of DirectDraw. It takes advantage 
  of hardware acceleration if it is supported by the driver. 

  The program requires less than 1 MB of video RAM. 

  The sound code is deliberately designed to stress the DirectSound API. It is 
  not intended to be a demonstration of how to use DirectSound API efficiently. 
  For example, each bullet on the screen uses a different sound buffer. Space 
  Donuts creates over 70 sound buffers (including duplicates), and between 20 
  to 25 may be playing at any time. 

  The sounds are implemented using the helper functions in Dsutil.h and 
  Dsutil.c (found in the Sdk\Samples\Misc directory). These functions might 
  help you to add sound to your application quickly and easily.

