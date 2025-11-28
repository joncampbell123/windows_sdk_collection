//-----------------------------------------------------------------------------
// 
// Sample Name: DSShow3D Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  DSShow3D allows you to modify the parameters of 3-D sound buffers and the 3-D 
  listener.

Path
====
  Source: \Mssdk\Samples\Multimedia\DSound\Src\DSShow3D

  Executable: \Mssdk\Samples\Multimedia\DSound\Bin

User's Guide
============
  Using Open from the File menu, you can open one or more short sound files and 
  play them at the same time. 

  You can also load files by dragging them onto the program's icon, or from the 
  command line using the following syntax:
  DSShow3D [/PLAY [/LOOP]] [file] [file] ...

  
  Specifying /PLAY causes any specified files to be played as they're opened. 
  Adding /LOOP causes them to loop as well, but /LOOP without /PLAY means 
  nothing. File names can be enclosed in quotation marks. 

  For each wave file that you open, DSShow3D displays a group of buffer 
  controls in a client window. The listener settings, which apply to all open 
  sounds, are shown in a separate window.

  To play the sound, click the Play button in its window. To play it 
  continuously, press the Looped button. You can create a duplicate of the 
  sound buffer by choosing Duplicate! from the client window's menu. 

  To close a buffer, choose Close from the buffer's File menu, or simply close 
  the window.

  You can change the format of the primary buffer by choosing Output Format 
  from the Options menu in the main window. On the same menu, Settings lets you 
  choose preferences that will take effect next time the application is 
  started. 

  Changes made in the 3D Listener Object Settings window do not take effect 
  until you click the Commit button.

Programming Notes
=================
  The source code demonstrates creation and duplication of 3-D sound buffers, 
  changing parameters of sound buffers, changing listener parameters, changing 
  the primary format, and using notification positions. The program uses static 
  sound buffers.

