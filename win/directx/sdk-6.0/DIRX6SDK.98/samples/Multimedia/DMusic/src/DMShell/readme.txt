//-----------------------------------------------------------------------------
// 
// Sample Name: DirectMusic Shell Sample
// 
// Copyright © 1998, Microsoft Corp. All Rights Reserved.
//
// GM/GS® Sound Set Copyright ©1996, Roland Corporation U.S.
// 
//-----------------------------------------------------------------------------


  [This is preliminary documentation and subject to change.] 

Description
===========
  The DirectMusic Shell sample demonstrates interactive music that responds to 
  Windows system events.

Path
====
  Source: Mssdk\Samples\Multimedia\Dmusic\Src\DMShell

  Executable: Mssdk\Samples\Multimedia\DMusic\Bin

User's Guide
============
  When you run the program, its icon appears in the tray on the taskbar. Click 
  on the icon to see a menu that allows you to change music schemes, select the 
  output device, start and stop the music, and close the program.

  Listen to the music and note how it changes and how motifs are introduced in 
  response to system events such as minimizing, restoring, or closing a window, 
  opening an application menu or the Start menu, and pressing a key (there are 
  special sounds for a few keys).

Programming Notes
=================
  The Windows system messages are obtained in Dmhook.dll, the source code for 
  which is found in the Mssdk\Samples\Multimedia\Dmusic\Src\DMHook folder.

