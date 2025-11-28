//-----------------------------------------------------------------------------
// 
// Sample Name: DMDonuts Sample
// 
// Copyright © 1998, Microsoft Corp. All Rights Reserved.
//
// GM/GS® Sound Set Copyright ©1996, Roland Corporation U.S.
// 
//-----------------------------------------------------------------------------


  [This is preliminary documentation and subject to change.] 

Description
===========
  DM Donuts is a variation of the Space Donuts sample that adds support for 
  interactive music.

Path
====
  Source: Mssdk\Samples\Multimedia\Dmusic\Src\DMDonuts

  Executable: Mssdk\Samples\Multimedia\DMusic\Bin

User's Guide
============
  Before running the program, make sure that you've set your MIDI control panel 
  to your default MIDI device. DirectMusic will use this for hardware MIDI. If 
  there is no sound when you run DMDonuts, it was unable to find the music 
  subdirectory.

  While the donut floats in space, notice that the music is subtle and spacey. 
  Press the arrow keys to move the ship Bounce around and see how the music and 
  rhythm respond.

  Shoot the donut by firing with the space bar. Immediately there is an 
  explosion of music and the background music grows in intensity.

  Press the 7 key on the numeric keypad. This turns on the shields. Notice how 
  the music becomes muted as if you were listening from inside the shield.

  When all the donut fragments are destroyed, notice that the music immediately 
  transitions into an ending, then starts the next level on the start of the 
  next musical theme.

  Notice that the music is never the same.

  You can control the ship with the joystick if you prefer, by changing the 
  input device under the Game menu.

  The following is a complete list of game commands. All numbers must be 
  entered from the numeric keypad. "Joy" refers to a joystick button.

  Key	Command

  

  ESC, F12	Quit

  4	Turn left

  6	Turn right

  5 (Joy 3)	Stop

  8	Accelerate forward

  2	Accelerate backward

  7 (Joy 2)	Shields 

  SPACEBAR (Joy 1)	Fire

  ENTER	Start game

  F1	Toggle trailing afterimage effect on/off

  F5	Toggle frame rate display on/off

  F7	Turn music on/off

  F10	Main menu

  

  The display defaults to 640x480 at 256 colors. You can specify a different 
  resolution and pixel depth on the command line. 

  The game uses the following command line switches, which are case-sensitive:

  e	Use software emulation, not hardware acceleration

  t	Test mode, no input required

  x	Stress mode. Never halt if you can help it

  

  These switches may be followed by three option numbers representing 
  x-resolution, y-resolution, and bits per pixel. For example:
  donuts -t 800 600 16

   

Programming Notes
=================
  Techniques illustrated include the following:
  	Composing and performing style based segments. 

  	Autotransitions on game state changes.

  	Motifs (short musical clips) to highlight actions. Because the motifs track 
  the rhythm and harmony of the underlying music, they add to the music while 
  providing sonic reinforcement.

  	Dynamic bands that change the orchestration in response to real-time 
  events.
  

  By default, this sample runs on the software synthesizer.  For the best sound
  in Windows 95 and Windows 98, on good wavetable synthesizers that follow the GS/DLS1 standards
  for volume and velocity behavior, recompile and undefine the _SOFTWARE_SYNTH_
  compile flag. This newly compiled version will not run on Windows NT 5.
