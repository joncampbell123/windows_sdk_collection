//-----------------------------------------------------------------------------
// 
// Sample Name: DSStream Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The DSStream sample illustrates the use of streaming sound buffers. It works 
  best with sounds that are more than 2 seconds in length.

Path
====
  Source: \Mssdk\Samples\Multimedia\DSound\Src\DSStream

  Executable: \Mssdk\Samples\Multimedia\DSound\Bin

User's Guide
============
  Load a single wave file by selecting Open from the File menu. Play the sound 
  once by choosing Play! from the main menu or by clicking the Play button. 
  Halt the playback by choosing Stop! from the main menu or by clicking the 
  Stop button.

  To make the program play the sound continuously, select the checkbox labeled 
  Looped.

  The DSStream program displays controls for panning the sound, decreasing the 
  volume, and changing the frequency.

  You can make DSStream enumerate the sound drivers when it is next started. To 
  do this, select Enumerate Drivers from the Options menu.

Programming Notes
=================
  The program demonstrates how to stream a lengthy wave file into a secondary 
  buffer, using notification positions and a separate thread to handle 
  notifications.

