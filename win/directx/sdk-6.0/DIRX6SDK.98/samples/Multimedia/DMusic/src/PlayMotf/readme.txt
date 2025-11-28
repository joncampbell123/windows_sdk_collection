//-----------------------------------------------------------------------------
// 
// Sample Name: PlayMotif Sample
// 
// Copyright © 1998, Microsoft Corp. All Rights Reserved.
//
// GM/GS® Sound Set Copyright ©1996, Roland Corporation U.S.
// 
//-----------------------------------------------------------------------------


  [This is preliminary documentation and subject to change.] 

Description
===========
  This sample application shows how motifs can be played over a primary 
  segment.

Path
====
  Source: Mssdk\Samples\Multimedia\Dmusic\Src\PlayMotf

  Executable: Mssdk\Samples\Multimedia\DMusic\Bin

User's Guide
============
  Click the buttons to play the various secondary segments (motifs) on top of 
  the main playing primary segment. Click Close to exit the application.

Programming Notes
=================
  The program uses the registry key set up by the DirectX 6.0 SDK setup to find 
  the media file path.

  Helper.cpp contains useful functions that set up DirectMusic. These functions 
  are called from main.cpp.

