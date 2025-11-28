//-----------------------------------------------------------------------------
// 
// Sample Name: DSShow Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  This sample application demonstrates many of the capabilities of DirectSound 
  2-D buffers.

Path
====
  Source: \Mssdk\Samples\Multimedia\DSound\Src\DSShow

  Executable: \Mssdk\Samples\Multimedia\DSound\Bin

User's Guide
============
  The title bar displays information about the hardware mixing capabilities of 
  your sound card: the number of free hardware mixing channels and hardware 
  memory, if you have an ISA card. If both of those numbers are 0, your card 
  does not have hardware mixing.

  Choose Open from the File menu to load one or more short wave files. If you 
  try to open a file that is very large, you will run out of system memory 
  because all sounds are loaded into static buffers.

  You can also load files by dragging them onto the program's icon, or from the 
  command line using the following syntax:
  DSShow [/PLAY [/LOOP]] [file] [file] ...

  
  Specifying /PLAY causes any specified files to be played as they're opened. 
  Adding /LOOP causes them to loop as well, but /LOOP without /PLAY means 
  nothing. File names can be enclosed in quotation marks. 

  For each sound, DSShow displays buffer information and a group of controls. 
  The file name is displayed at the top. The next line states whether the 
  buffer is mixed in software (SW) or hardware (HW), and whether the buffer is 
  playing or stopped. DSShow will put buffers in hardware whenever possible. On 
  the next two lines, the positions of the current play and write cursors are 
  displayed. 

  You can play, stop, and remove the sound, set the frequency, and adjust the 
  master volume. The Pan slider also controls the left-right volume indicators, 
  which cannot be moved separately. If you want the sound to play continuously, 
  check the Looped checkbox. To remove the sound file, select the Close button.

  The Output Type choice on the Options menu lets you change the format of the 
  primary buffer, and Check Latency brings up a dialog box that you can use to 
  test how rapidly a sound starts and stops after it is triggered. You can tell 
  the DSShow application to enumerate all available sound drivers the next time 
  it starts up by selecting Enumerate Drivers.

Programming Notes
=================
  The application demonstrates device enumeration, changing the format of the 
  primary buffer, and changing parameters of secondary buffers. It uses static 
  sound buffers.

