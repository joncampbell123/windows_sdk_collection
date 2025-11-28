//-----------------------------------------------------------------------------
// 
// Sample Name: JoyFFeed Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  This application applies raw forces to a force feedback joystick, 
  illustrating how a simulator-type application can use force feedback to 
  generate forces computed by a physics engine.

  You must have a force feedback device connected to your system in order to 
  run the application.

Path
====
  Source: Mssdk\Samples\Multimedia\DInput\Src\JoyFFeed

  Executable: Mssdk\Samples\Multimedia\DInput\Bin

User's Guide
============
  When you run the application, it displays a window with a crosshair and a 
  black spot in it. Click the mouse anywhere within the window's client area to 
  move the black spot. (Note that moving the joystick handle does not do 
  anything.) JoyFFeed exerts a constant force on the joystick handle from the 
  direction of the spot, in proportion to the distance from the crosshair. You 
  can also hold down the mouse button and move the spot continuously.

Programming Notes
=================
  This sample program enumerates the input devices and acquires the first 
  force-feedback joystick that it finds. If none are detected, it displays a 
  message and terminates.

  When the user moves the black spot, the joySetForcesXY function converts the 
  cursor coordinates to a force direction and magnitude. This data is used to 
  modify the parameters of the constant force effect.

